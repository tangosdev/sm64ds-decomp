// The host I/O bus: a real memory window at the DS I/O addresses, plus side-effect
// dispatch for the registers whose writes do something.
//
// Register semantics here are from GBATEK. No SDK source is reproduced.

#include "ntr/mmio.h"

#include "ntr/gx.h"
#include "ntr/ppu_audit.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <sys/mman.h>
#endif

namespace ntr {
namespace {

uint8_t *g_io = nullptr;

inline uint8_t *slot(uint32_t addr) { return g_io + (addr - IO_BASE); }

inline uint64_t raw_read(uint32_t addr, unsigned width) {
    uint64_t v = 0;
    std::memcpy(&v, slot(addr), width);
    return v;
}

inline void raw_write(uint32_t addr, uint64_t value, unsigned width) {
    std::memcpy(slot(addr), &value, width);
}

// --- CP: divider and square root -----------------------------------------------
// GBATEK: 0x4000280 DIVCNT, 0x4000290 NUMER(64), 0x4000298 DENOM(64),
//         0x40002A0 RESULT(64), 0x40002A8 REMAINDER(64),
//         0x40002B0 SQRTCNT, 0x40002B4 SQRT_RESULT(32), 0x40002B8 SQRT_PARAM(64).
// The busy bit (0x8000) is never set: the host computes synchronously, so any
// `while (DIVCNT & 0x8000);` spin-wait falls straight through.

constexpr uint32_t DIVCNT = 0x4000280, DIV_NUMER = 0x4000290, DIV_DENOM = 0x4000298;
constexpr uint32_t DIV_RESULT = 0x40002A0, DIV_REM = 0x40002A8;
constexpr uint32_t SQRTCNT = 0x40002B0, SQRT_RESULT = 0x40002B4, SQRT_PARAM = 0x40002B8;

// GXSTAT is NOT a plain latch, and modelling it as one deadlocks the renderer.
//
// Bits 16..27 -- the command-FIFO entry count (16-24), "FIFO less than half
// full" (25), "FIFO empty" (26) and "geometry engine busy" (27) -- are driven
// by the geometry engine and are READ-ONLY. A store to GXSTAT on a DS reaches
// only the matrix-stack error flag (15) and the FIFO IRQ mode (30-31); the
// status field keeps reporting the hardware's own state.
//
// The game leans on that. func_0205583c (Initialise3dGraphics, and
// Scene::Initialise3dGraphics under it) touches GXSTAT four times, a busy-wait
// READ and then three stores:
//
//     while (*(volatile unsigned int *)0x4000600 & 0x8000000) ;  // bit 27 busy
//     ...
//     *(volatile unsigned int *)0x4000600 = 0;      // src/func_0205583c.c
//     ...
//     *(volatile unsigned int *)0x4000600 |= 0x8000;
//     *(volatile unsigned int *)0x4000600 = (... & ~0xc0000000) | 0x80000000;
//
// The opening read matters as much as the stores: bit 27 is in the same
// hardware-owned field, so a latch that ever came up with it set would hang
// func_0205583c itself, before the display list is even reached. GXSTAT_IDLE
// clears it for the same reason it sets 25 and 26.
//
// On hardware that clears the error flag and selects an IRQ mode, and bits
// 25/26 stay set because the FIFO really is empty. On a latch the `= 0` wipes
// them and nothing ever puts them back, so GXSTAT reads 0x80008000 forever --
// and func_0205a358, the display-list submit, opens with
//
//     while (((GXSTAT & 0x7000000) >> 0x18 & 2) == 0) ;
//
// which is a wait for bit 25. Measured: with the latch, the star select's
// first submit (a correctly loaded and correctly rebased 692-byte list) never
// returns, and PORT_WATCHDOG catches the main thread in that spin, reading
// GXSTAT through ntr::io_read. See the io_write hook below.
//
// The host's FIFO is always empty and never busy: ntr executes a display list
// synchronously inside the submit, so there is no queue to be backed up.
// GXSTAT_IDLE is that truth, and io_init already brings the register up
// holding it.
// THE MATRIX-STACK HALF, bits 8..14. These are read-only on hardware too -- the
// position/vector matrix stack level (8..12), the projection stack level (13)
// and matrix-stack-busy (14) -- and func_0205583c's `= 0` leaves every one of
// them a dead latch. An earlier revision of this note left them alone on the
// grounds that "the host has no matrix-stack model to normalise them FROM".
// That was wrong about the host: ntr/gx.cpp has kept both stacks and both stack
// pointers all along (g.pos_sp over pos_stack[32], g.proj_sp over
// proj_stack[2]), so the level is not a guess, it is a value the geometry
// engine already tracks. gx_matrix_stack_levels reports it and the two fields
// are normalised from it below.
//
// BUSY IS ZERO, EXACTLY. ntr executes a display list synchronously inside the
// submit, so there is no window in which the game can observe a push or pop in
// flight. That is the same argument GXSTAT_IDLE rests on for the FIFO.
//
// THE FIELD POSITIONS COME FROM THE GAME, not from docs alone. Its own two
// readers spell them out:
//
//     func_02055464: if (GXSTAT & 0x4000) return -1;  *out = (GXSTAT & 0x2000) >> 13;
//     func_02055490: if (GXSTAT & 0x4000) return -1;  *out = (GXSTAT & 0x1f00) >> 8;
//
// so 0x4000 is busy, 0x1f00 is the position/vector level and 0x2000 the
// projection level, which is what GBATEK says as well.
//
// AND THIS CORRECTS WHAT THAT EARLIER NOTE SAID THE SPIN WAS. It read
// func_02055624's
//
//     while (func_02055490(&a)) ;
//     while (func_02055464(&b)) ;
//
// as "spin until the stack level reads 0". It is not: both helpers return -1
// only when BUSY and 0 otherwise, so the loops spin until NOT BUSY and then use
// `a` and `b` as the levels. That changes the risk in this fix's favour. With
// busy pinned to 0 the loops still fall through immediately, exactly as they did
// against the dead latch, so there is no new hang. What changes is the second
// half: func_02055624 then pops the stacks by `a` and `b`
// (0x4000440 MTX_MODE, 0x4000448 MTX_POP), which against a dead-zero latch
// popped NOTHING and left the stack unwound.
//
// func_02055624 IS LINKED, and an earlier revision of this note said it was not.
// The "0 references in walk_window.map" was measured against a 4950-symbol
// build and carried forward unchecked into a 5533-symbol one, where the map has
// it at 0001:0003d520 along with func_020557b4 and their caller
// func_ov007_020bcf90. What is true is narrower: the chain is UNREACHABLE, not
// unlinked. Its only linked caller is func_ov007_020bcf90, which is on scene
// 1's Render path, and scene 1 faults inside InitResources before frame 0
// (port/ov007_seat.txt section 5). The other caller of func_020557b4,
// func_0200f4b4, is genuinely absent from the map.
//
// So this lands the day scene 1 runs, not "the day it links" -- it is already
// linked. On that day the two pops become real against gx.cpp's live sp
// counters instead of no-ops against zeros, which is the point of the fix.
//
// WHAT THIS IS NOT. gxstat_normalize runs only on accesses that come THROUGH
// this proxy, and almost nothing on the 2D or 3D surface does. So bits 8..14
// are not live: they are a snapshot taken at the last hostgen'd touch of
// GXSTAT, and a reader reaching mapped memory directly sees whatever that
// snapshot left. That is strictly better than the dead latch, which never
// tracked anything at all, and it is not the same as modelling the register.
// Making it live needs the stack write-through to happen in gx.cpp at push and
// pop time, which is that file's lane, not this one's.
//
// Bits 0..7, the box-test result, stay a latch. The host runs no box test, so
// there is nothing truthful to report and no linked reader asks (the sweep in
// port/tools/ppu_reg_readers.py covers the register file this belongs to).
constexpr uint32_t GXSTAT = 0x4000600;
constexpr uint32_t GXSTAT_HW = 0x0FFF7F00;    // bits 8..14 + 16..27, hardware owned
constexpr uint32_t GXSTAT_IDLE = 0x06000000;  // empty + less-than-half-full, not busy

// SM64DS_DIV_UNIT_OFF=1: the divider computes nothing and DIV_RESULT stays
// whatever the memory window holds. THE ESCAPE HATCH FOR run link60 Stage 5
// lane T2, and read what it is before reading it as a general switch.
//
// That lane routed src/_ZN4cstd10fdiv_asyncE... and src/_ZN4cstd4ldivEii
// through hostgen, so cstd::ldiv reaches this unit for the first time. The
// change is a BUILD-TIME routing and there is no runtime seam inside a
// generated copy of byte-verified source to hang a switch on, so the hatch
// goes at the unit instead: with it set, every proxied caller reads a dead
// window, which is the state cstd::ldiv's caller was in before the routing.
//
// WHAT IT IS NOT, AND THIS IS THE HALF THAT MATTERS. It is not caller-scoped
// and it is NOT a reconstructed before. G3i::PerspectiveW_'s host copy already
// reached this unit and it goes dark too, so on scene 1 the hatch is MORE off
// than the pre-change state was: PerspectiveW_ loses proj[5] and the depth row
// as well, all 96 triangles collapse to the single point 256,192 instead of to
// the pre-change vertical line, and the frame carries 230 distinct colours
// where the start commit's carried 472. Both hashes are in
// port/ov007_seat.txt 5i. It switches the behaviour off, which is what a hatch
// owes, and it does not rebuild the old frame; the before is the start
// commit's binary. On a LEVEL it is a new breakage rather than an old
// behaviour, so it is a bisect switch there and nothing more.
//
// AN EXACT SAME-BINARY BEFORE WOULD NEED A DIFFERENT SEAM. The cheap one is a
// CMake option on the routing block in port/CMakeLists.txt that keeps the
// plain slice entries, which is a rebuild rather than one binary; nobody has
// needed it yet.
bool div_unit_off() {
    static int off = -1;
    if (off < 0) {
        const char *e = std::getenv("SM64DS_DIV_UNIT_OFF");
        off = (e && *e && *e != '0') ? 1 : 0;
    }
    return off != 0;
}

void run_divide() {
    if (div_unit_off()) return;
    const uint16_t mode = static_cast<uint16_t>(raw_read(DIVCNT, 2)) & 3;
    const int64_t numer = static_cast<int64_t>(raw_read(DIV_NUMER, 8));
    const int64_t denom = static_cast<int64_t>(raw_read(DIV_DENOM, 8));

    int64_t n = numer, d = denom;
    if (mode == 0) {                       // 32 / 32
        n = static_cast<int32_t>(numer);
        d = static_cast<int32_t>(denom);
    } else if (mode == 1) {                // 64 / 32
        d = static_cast<int32_t>(denom);
    }

    int64_t quot, rem;
    if (d == 0) {
        // GBATEK: division by zero leaves remainder = numerator and a sign-based
        // quotient. Mirror that rather than trapping.
        rem = n;
        quot = (n < 0) ? 1 : -1;
        if (mode == 0) quot = (n < 0) ? 1 : -1;
    } else {
        quot = n / d;
        rem = n % d;
    }
    raw_write(DIV_RESULT, static_cast<uint64_t>(quot), 8);
    raw_write(DIV_REM, static_cast<uint64_t>(rem), 8);
}

uint32_t isqrt64(uint64_t v) {
    // Integer floor(sqrt(v)) by restoring bit-by-bit; matches the DS unit's result.
    uint64_t rem = 0, root = 0;
    for (int i = 0; i < 32; ++i) {
        root <<= 1;
        rem = (rem << 2) | (v >> 62);
        v <<= 2;
        if (rem > root) {
            rem -= root | 1;
            root += 2;
        }
    }
    return static_cast<uint32_t>(root >> 1);
}

unsigned long g_sqrt_runs;

void run_sqrt() {
    ++g_sqrt_runs;
    const uint16_t mode = static_cast<uint16_t>(raw_read(SQRTCNT, 2)) & 1;
    uint64_t param = raw_read(SQRT_PARAM, 8);
    if (mode == 0) param &= 0xFFFFFFFFull;      // 32-bit mode
    raw_write(SQRT_RESULT, isqrt64(param), 4);
}

}  // namespace

// See the block over the declaration in ntr/mmio.h: this is how a lane tells
// "nothing changed because nothing called it" apart from "nothing changed
// because the answer did not flip a branch". A plain TU never reaches here, so
// the difference between the two arms of a routing A/B IS the call count.
unsigned long sqrt_runs() { return g_sqrt_runs; }

// ---------------------------------------------------------------------------
// CLAIMING THE DS ADDRESSES
//
// These addresses are not a preference, they are the port's whole model. The
// relocated romdata and every overlay pointer table carry LITERAL DS addresses,
// so 0x02000000 is simultaneously the DS address and the host address. Mapping
// the range anywhere else would make every relocated pointer wrong, so there is
// no fallback address to fall back to: the process either wins these ranges or
// it cannot run. What CAN be improved is how often it wins.
//
// THE RACE. Nothing about these addresses is reserved for us. Anything the
// process allocates before we ask -- a CRT heap segment, a thread stack, an
// injected DLL, a shell extension, a security product's hook module -- can land
// in the low address space and take part of a range, and in a 32-bit process
// the low 128 MB is prime real estate. Whether it happens is down to load order
// and ASLR, so the same build on the same machine can lose one launch and win
// the next. A player hit exactly that: one launch died here, and a launch six
// minutes later on the same machine got in with nothing changed.
//
// WHEN WE ASK. The reservation used to run from io_init(), called from main().
// By then the CRT has initialised, every static initialiser in the program has
// run, the fault probe has created its crash directory, the watchdog thread has
// a stack, and the flight recorder has opened a file -- thousands of pages of
// other allocations, every one of them a chance to lose. The pass now runs from
// a TLS callback in .CRT$XLC instead, which the loader calls BEFORE the CRT
// entry point and before any static initialiser, i.e. the earliest point at
// which this program's own code runs at all. io_init() still calls the same
// pass, so a build or platform without the callback behaves as before, and so
// that a range lost early gets one more chance later.
//
// WHY NOT CLAIM IT AT LINK TIME. Because it was tried, and it works, and it is
// still the wrong trade. MSVC will pin an absolute VA: a `#pragma bss_seg` of
// 0x400000 bytes costs nothing on disk (raw size 0), and linking with
// /ALIGN:0x10000 /BASE:<0x02000000 minus the section RVA> /FIXED /DYNAMICBASE:NO
// puts that section at exactly 0x02000000, mapped by the loader before the
// process runs a single instruction. A test exe built that way reported its
// array at 02000000 and wrote both ends. What kills it:
//   * /FIXED means the loader MAY NOT relocate. If the range is unavailable the
//     process does not start at all, so the one failure this file exists to
//     report becomes a failure with no message, no exit code of ours and no
//     startup_error.txt. That is strictly worse than what a player gets today.
//   * The base has to be 0x02000000 minus the section's RVA, and that RVA moves
//     whenever any preceding section changes size, which is every build.
//   * It covers ONE range. The other five are spread to 0x07900000, so pinning
//     them all means one contiguous ASLR-free image spanning ~88 MB of a 32-bit
//     address space.
//   * /DYNAMICBASE:NO on the shipped exe is its own regression, and a fixed-base
//     no-ASLR image is exactly the shape the security software implicated here
//     looks at hardest.
// So: earliest-possible RUNTIME reservation, which is the TLS callback below.
//
// THE CALLBACK RUNS BEFORE THE CRT. Nothing on the reservation path may touch
// CRT state: no printf, no snprintf, no locale, no LoadLibrary (the loader lock
// is held). Raw Win32 only -- the same discipline fault_probe.h keeps, for the
// same reason. Anything needing the CRT happens later, at report time, off the
// facts captured here.
//
// WHEN WE LOSE. Three things, in order, none of which costs anything unless the
// first attempt already failed:
//   1. RETRY. A transient occupant may be gone a few milliseconds later, and
//      the player's successful second launch proves transient is a real case.
//   2. FILL THE HOLES, main RAM only. Partly held main RAM beats none, because
//      the range is a safety net for pointer rebases the port missed rather
//      than storage the game needs. The small ranges stay all-or-nothing on
//      purpose: half an I/O window would turn a clean exit into an access
//      violation at the first unmapped register, which is strictly worse.
//   3. RECORD WHO TOOK IT. VirtualQuery every failed range and keep the raw
//      facts. That is the difference between "it did not work" and a name.
// ---------------------------------------------------------------------------

namespace {

struct RegionDef {
    uintptr_t base;
    size_t size;
    const char *name;   // what a player would call it
    bool fatal;         // false: the game runs without it
};

// Main RAM is only needed by the ROM-image fs backend; in the port build the
// game's globals are host-linked symbols and file images live on the game heap,
// so a lost race for 0x02000000 is survivable. Everything else is load-bearing.
const RegionDef kRegions[] = {
    {MAIN_BASE,   MAIN_SIZE,   "main memory",             false},
    {IO_BASE,     IO_SIZE,     "hardware registers",      true },
    {PLTT_BASE,   PLTT_SIZE,   "palette memory",          true },
    {VRAM_BASE,   VRAM_SIZE,   "video memory",            true },
    {OAM_BASE,    OAM_SIZE,    "sprite memory",           true },
    {SHARED_BASE, SHARED_SIZE, "the shared system block", true },
};
const unsigned kRegionCount = sizeof kRegions / sizeof kRegions[0];

void *g_held[kRegionCount];        // what we actually hold, per region
unsigned g_lost_mask;              // regions still missing after the last pass
unsigned g_attempts;               // reservation passes run
int g_stage_first;                 // 1 = TLS callback, 2 = io_init (late)
int g_stage_won;                   // stage in which the last range came up
size_t g_main_held;                // bytes of main RAM held, hole fill included
bool g_partial_main;               // main RAM came up in pieces

// One occupied block found inside a range we could not get. Raw facts only:
// names are resolved later, off the loader lock (see io_reserve_report).
struct Occupant {
    uintptr_t region_base;         // which DS range it was blocking
    uintptr_t alloc_base, base;
    size_t size;
    unsigned state, type, protect;
};
Occupant g_occ[24];
unsigned g_occ_n;

// Reserve one DS region at its real address. Fixed low bases keep (u32)&x exact.
void *map_fixed(uintptr_t base, size_t size) {
#if defined(_WIN32)
    void *p = VirtualAlloc(reinterpret_cast<void *>(base), size,
                           MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
#else
    void *p = mmap(reinterpret_cast<void *>(base), size, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE, -1, 0);
    if (p == MAP_FAILED) p = nullptr;
#endif
    if (p) std::memset(p, 0, size);
    return p;
}

#if defined(_WIN32)

// Record what is sitting inside a range we failed to get. VirtualQuery is a
// plain syscall, so this is safe from the TLS callback; module names are NOT
// resolved here, because that would mean calling into the loader while the
// loader is calling us. We keep the allocation base and look the name up later.
void note_occupants(uintptr_t base, size_t size) {
    uintptr_t a = base, end = base + size;
    MEMORY_BASIC_INFORMATION mbi;
    while (a < end && g_occ_n < sizeof g_occ / sizeof g_occ[0]) {
        if (!VirtualQuery(reinterpret_cast<void *>(a), &mbi, sizeof mbi)) break;
        uintptr_t rbeg = reinterpret_cast<uintptr_t>(mbi.BaseAddress);
        uintptr_t rend = rbeg + mbi.RegionSize;
        if (rend <= a) break;                       // no forward progress
        if (mbi.State != MEM_FREE) {
            uintptr_t ab = reinterpret_cast<uintptr_t>(mbi.AllocationBase);
            // One mapped image is many regions, one per section protection. They
            // are all the same answer, so fold them into the entry we already
            // have: five identical lines naming the same DLL is noise, and worse,
            // it can fill this table before the range that actually stopped the
            // game gets a row.
            if (g_occ_n && g_occ[g_occ_n - 1].alloc_base == ab &&
                g_occ[g_occ_n - 1].region_base == base &&
                g_occ[g_occ_n - 1].base + g_occ[g_occ_n - 1].size == rbeg) {
                g_occ[g_occ_n - 1].size += mbi.RegionSize;
            } else {
                Occupant &o = g_occ[g_occ_n++];
                o.region_base = base;
                o.alloc_base = ab;
                o.base = rbeg;
                o.size = mbi.RegionSize;
                o.state = mbi.State;
                o.type = mbi.Type;
                o.protect = mbi.Protect;
            }
        }
        a = rend;
    }
}

// Take every free 64 KB granule inside a range we could not take whole. Only
// ever called for main RAM (see the header comment on why the small ranges are
// all or nothing).
size_t fill_holes(uintptr_t base, size_t size) {
    const uintptr_t kGran = 0x10000;
    size_t got = 0;
    uintptr_t a = base, end = base + size;
    MEMORY_BASIC_INFORMATION mbi;
    while (a < end) {
        if (!VirtualQuery(reinterpret_cast<void *>(a), &mbi, sizeof mbi)) break;
        uintptr_t rend =
            reinterpret_cast<uintptr_t>(mbi.BaseAddress) + mbi.RegionSize;
        if (rend <= a) break;
        if (rend > end) rend = end;
        if (mbi.State == MEM_FREE) {
            // VirtualAlloc rounds a reservation out to allocation granularity in
            // both directions, so only whole granules INSIDE the free run can be
            // asked for; anything else would round into the occupied part and
            // fail the whole call.
            uintptr_t s = (a + kGran - 1) & ~(kGran - 1);
            if (s < rend) {
                size_t n = (rend - s) & ~(kGran - 1);
                if (n) {
                    void *p = VirtualAlloc(reinterpret_cast<void *>(s), n,
                                           MEM_RESERVE | MEM_COMMIT,
                                           PAGE_READWRITE);
                    if (p) got += n;
                }
            }
        }
        a = rend;
    }
    return got;
}

#endif  // _WIN32

// One reservation pass over whatever is still missing. True when all are held.
bool reserve_pass() {
    bool all = true;
    for (unsigned i = 0; i < kRegionCount; ++i) {
        if (g_held[i]) continue;
        void *p = map_fixed(kRegions[i].base, kRegions[i].size);
        if (p) {
            g_held[i] = p;
            if (kRegions[i].base == MAIN_BASE) g_main_held = MAIN_SIZE;
        } else {
            all = false;
        }
    }
    return all;
}

bool all_held() {
    for (unsigned i = 0; i < kRegionCount; ++i)
        if (!g_held[i]) return false;
    return true;
}

}  // namespace

// The reservation itself. Idempotent, and safe to call before the CRT is up.
// stage is 1 from the TLS callback, 2 from io_init.
void io_reserve(int stage) {
    if (all_held()) return;
    if (!g_stage_first) g_stage_first = stage;

#if defined(_WIN32)
    // The retry is the cheap half of the fix: the failure that reached us was
    // transient (the same player launched again minutes later and got in), so a
    // range that is taken now may be free in a moment. The delays are only ever
    // paid on the failure path; a clean first pass breaks out immediately.
    static const unsigned kBackoffMs[] = {0, 10, 25, 60};
    const unsigned kTries = sizeof kBackoffMs / sizeof kBackoffMs[0];
    for (unsigned t = 0; t < kTries; ++t) {
        if (kBackoffMs[t]) Sleep(kBackoffMs[t]);
        ++g_attempts;
        if (reserve_pass()) break;
    }
#else
    ++g_attempts;
    reserve_pass();
#endif

    g_lost_mask = 0;
    bool fatal_all = true;
    for (unsigned i = 0; i < kRegionCount; ++i) {
        if (g_held[i]) continue;
        g_lost_mask |= 1u << i;
        if (kRegions[i].fatal) fatal_all = false;
    }
    // "Won" means the game can run, so it is the REQUIRED ranges that decide it.
    // Main memory can still be missing here; that is reported separately and is
    // survivable.
    if (fatal_all && !g_stage_won) g_stage_won = stage;

#if defined(_WIN32)
    if (g_lost_mask) {
        // Who has it? Capture before anything else moves.
        g_occ_n = 0;
        for (unsigned i = 0; i < kRegionCount; ++i)
            if (!g_held[i]) note_occupants(kRegions[i].base, kRegions[i].size);

        // Main RAM in pieces beats main RAM not at all.
        if (!g_held[0]) {
            g_main_held = fill_holes(MAIN_BASE, MAIN_SIZE);
            g_partial_main = g_main_held != 0;
        }
    }
#endif
}

bool io_init() {
    if (g_io) return true;
    // Normally a no-op: the TLS callback already did this before main. It still
    // runs so that a range lost early gets a second chance with more elapsed
    // time behind it, and so a build without the callback still works.
    io_reserve(2);

    void *io = nullptr;
    for (unsigned i = 0; i < kRegionCount; ++i) {
        if (!g_held[i] && kRegions[i].fatal) return false;
        /* Losing this race is not fatal, but it IS the difference between two
           runs of the same build, so say so rather than carrying on quietly. */
        if (!g_held[i]) std::fprintf(stderr, "[io] main RAM %08x NOT mapped\n",
                                     (unsigned)kRegions[i].base);
        if (kRegions[i].base == IO_BASE) io = g_held[i];
    }
    g_io = static_cast<uint8_t *>(io);
    // Reset state: the geometry FIFO starts empty. GXSTAT bit 26 (empty) and
    // bit 25 (less than half full) are set on hardware at boot; the game's
    // display-list pump spin-waits on bit 25 before touching the FIFO, so the
    // latch must come up idle or the wait never falls through.
    raw_write(0x04000600u, 0x06000000u, 4);
    /* The shared block comes up saying the sound system is already busy.
       func_0203d974 is `*(u16 *)0x27ffc40 == 2 || data_020a0f10`, and it is the
       guard on func_02011f7c's ARM7 handshake -- the one that loads a
       character's voice bank. Answering 1 skips the handshake and leaves the
       bank id recorded, which is what the port wants: there is no ARM7, the
       SDAT player owns banks, and the ROM path would reach for a core that is
       not there. Reachable the moment the Player is anyone but Mario, since
       func_ov002_020e6330 loads the bank off the character byte. */
    *reinterpret_cast<volatile uint16_t *>(SHARED_BASE + 0xc40) = 2;
    return true;
}

// Force the geometry-engine-owned half of GXSTAT back to the host's real state.
// Done in the LATCH rather than only in the returned value on purpose: most of
// the game's GXSTAT accesses are NOT routed through this proxy. Only three of
// the nine functions that touch the register are hostgen'd (func_02055780,
// func_0205a21c and func_0205a358, all GATE4A); the other six, func_0205583c --
// the store that wipes the status field -- among them, reach mapped memory
// directly. Normalising the latch on every proxied touch is what keeps those
// six reading something true.
static void gxstat_normalize() {
    const uint32_t v = static_cast<uint32_t>(raw_read(GXSTAT, 4));
    // The matrix-stack field, from the geometry engine's own stack pointers.
    // Busy (bit 14) is left clear: execution is synchronous, so the game can
    // never observe a push or pop in flight.
    unsigned pos = 0, proj = 0;
    gx_matrix_stack_levels(pos, proj);
    const uint32_t stack = (pos << 8) | (proj << 13);
    const uint32_t fixed = (v & ~GXSTAT_HW) | GXSTAT_IDLE | stack;
    if (fixed != v) raw_write(GXSTAT, fixed, 4);
}

static bool hits_gxstat(uint32_t addr, unsigned width) {
    return addr < GXSTAT + 4 && addr + width > GXSTAT;
}

uint64_t io_read(uint32_t addr, unsigned width) {
    if (!g_io && !io_init()) return 0;
    if (hits_gxstat(addr, width)) gxstat_normalize();
    const uint64_t v = raw_read(addr, width);
    if (ppu_audit_on()) ppu_audit_proxy(addr, v, width, false);
    return v;
}

void io_write(uint32_t addr, uint64_t value, unsigned width) {
    if (!g_io && !io_init()) return;
    if (ppu_audit_on()) ppu_audit_proxy(addr, value, width, true);
    raw_write(addr, value, width);

    // Writing the low half of the operand is what starts the unit on hardware.
    // SQRT_PARAM is 64-bit and the game writes it as two 32-bit words (the
    // hostgen'd func_02053008 stores 0x40002b8 then 0x40002bc); GBATEK has a
    // write to EITHER half restarting the unit, and dispatching only on the
    // low word left SQRT_RESULT computed from the stale high half -- found on
    // the WorkElevator rider-push chain (run linkw wave 5, lane w5-b).
    // Recomputing on both halves is idempotent for low-word-only writers.
    if (addr == DIV_DENOM || addr == DIV_NUMER || addr == DIVCNT) run_divide();
    // The sqrt trigger covers BOTH halves of the 64-bit parameter. The ROM's
    // own driver (func_02053008, and its sibling func_020531a4) writes
    // SQRT_PARAM as two 32-bit stores, LOW half first and the real operand in
    // the HIGH half LAST -- so a trigger on the base address alone runs the
    // unit before the operand has landed and leaves SQRT_RESULT stale, which
    // is exactly GBATEK's model inverted (the unit restarts on any write to
    // SQRTCNT or either SQRT_PARAM half).
    //
    // CORRECTED, run mg15 lane SQRT. This block used to claim the widening
    // "silently fixed a SECOND split-half SQRT_PARAM writer already in the
    // build: func_02053274 (slice_gate10)". It did not and it could not.
    // Widening this dispatch only helps a store that REACHES io_write, and
    // func_02053274 was built PLAIN out of slice_gate10, so its stores latched
    // in the mapped I/O window and never entered this function at all. It stayed
    // a 3D distance function that returned the PREVIOUS routed sqrt's result
    // for as long as that sentence stood here -- which is most of why nobody
    // looked at it again. What the widening actually fixed is what the
    // paragraph above says and nothing more: the ROM's own routed drivers,
    // func_02053008 and func_020531a4, whose high-half-last store order left
    // SQRT_RESULT computed from a stale operand under a base-address-only
    // trigger. What fixed func_02053274 is routing it through hostgen so its
    // stores arrive here in the first place (CMakeLists SQ_DIST_SYMS, and
    // SM64DS_SQRT_SELFTEST=1 for the two-known-roots demonstration).
    // The general lesson, and it is the one worth carrying: a trigger set in
    // this file can only ever be as wide as the set of TUs that are ROUTED.
    // Widening it says nothing about a TU that is built plain.
    // The divide side is NOT widened the same way on purpose -- and not
    // because divide clients write single 64-bit stores at base (they do
    // not: cstd::mod, cstd::div and two others store 32-bit at the base
    // addresses). The true invariant is that every divide-side store lands
    // at a trigger address (ARMMathLoadState reaches the unit via DIVCNT),
    // so the existing equality tests are the ones that fire, and this
    // change must not perturb that unit at all.
    else if (addr == SQRT_PARAM || addr == SQRT_PARAM + 4 || addr == SQRTCNT)
        run_sqrt();
    // Geometry: 0x4000400 is the packed FIFO, 0x4000440.. are the command ports.
    // These execute on write and leave nothing readable behind, so unlike every
    // other block they cannot be served by the memory window alone.
    else if (addr == 0x04000400u) gx_write_fifo(static_cast<uint32_t>(value));
    else if (addr >= 0x04000440u && addr <= 0x040005FFu)
        gx_write_port(addr, static_cast<uint32_t>(value));

    // Put the read-only half of GXSTAT back after any store that overlapped it,
    // whatever its width. See the GXSTAT note above the constants: the writable
    // bits (15, 30-31) keep whatever the game just stored.
    //
    // DELIBERATELY BELOW THE CHAIN ABOVE, not inside it. This is not one more
    // register dispatch competing with the others: it is a fixup that has to
    // run after whatever the store did. Written as a plain `if` in the middle
    // of the chain it also captured the `else` that belongs to the divide test,
    // which silently restructured the dispatch. GXSTAT is outside every range
    // the chain matches (0x4000600 is past gx_write_port's 0x40005FF), so
    // nothing was mis-dispatched in practice, but the structure said something
    // the code did not mean.
    if (hits_gxstat(addr, width)) gxstat_normalize();
}

// ---------------------------------------------------------------------------
// REPORTING A LOST RANGE
//
// Everything below runs long after the reservation, from main(), so the CRT is
// up and the loader is idle: this is where the module names get resolved off
// the allocation bases captured earlier. All of it is failure-path only.
// ---------------------------------------------------------------------------

namespace {

// Long enough for any Win32 path this can produce, and independent of
// windows.h so the POSIX build still compiles.
const size_t kNameMax = 260;

// How the block is being held. Only ever non-empty on Windows, where the
// occupant walk runs at all.
const char *occupant_state(const Occupant &o) {
#if defined(_WIN32)
    return o.state == MEM_COMMIT ? "committed" : "reserved";
#else
    (void)o;
    return "in use";
#endif
}

// Name the thing at an allocation base, if it can be named. A mapped image is
// the interesting case, because that is the answer a player can act on. Falls
// back to the memory type when there is no file behind it.
const char *occupant_name(const Occupant &o, char *buf, size_t cap) {
#if defined(_WIN32)
    if (o.type == MEM_IMAGE && o.alloc_base) {
        if (GetModuleFileNameA(reinterpret_cast<HMODULE>(o.alloc_base),
                               buf, static_cast<DWORD>(cap)))
            return buf;
    }
    if (o.type == MEM_MAPPED && o.alloc_base) {
        // K32GetMappedFileNameA lives in kernel32, which is already loaded, so
        // this needs no LoadLibrary.
        typedef DWORD(WINAPI * MappedName_t)(HANDLE, LPVOID, LPSTR, DWORD);
        HMODULE k32 = GetModuleHandleA("kernel32.dll");
        MappedName_t f = k32 ? reinterpret_cast<MappedName_t>(
            reinterpret_cast<void *>(
                GetProcAddress(k32, "K32GetMappedFileNameA"))) : nullptr;
        if (f && f(GetCurrentProcess(), reinterpret_cast<void *>(o.base), buf,
                   static_cast<DWORD>(cap)))
            return buf;
    }
    switch (o.type) {
        case MEM_IMAGE:  return "a loaded module with no readable name";
        case MEM_MAPPED: return "a mapped file with no readable name";
        case MEM_PRIVATE: return "private memory (a heap, a stack or a reservation)";
        default: break;
    }
#else
    (void)o; (void)buf; (void)cap;
#endif
    return "an unidentified allocation";
}

char g_detail[2048];
char g_player[1024];

}  // namespace

// The technical record: which ranges were lost and what was sitting in them.
// This is what goes in the log and the crash record.
const char *io_reserve_detail() {
    if (!g_lost_mask) return "";
    if (g_detail[0]) return g_detail;
    int n = 0;
    n += std::snprintf(g_detail + n, sizeof g_detail - n,
                       "[io] could not reserve the fixed DS ranges "
                       "(first tried at %s, %u attempt(s))\n",
                       g_stage_first == 1 ? "process start, before the CRT"
                                          : "io_init, from main",
                       g_attempts);
    for (unsigned i = 0; i < kRegionCount && n < (int)sizeof g_detail; ++i) {
        if (g_held[i]) continue;
        n += std::snprintf(g_detail + n, sizeof g_detail - n,
                           "[io]   LOST %08x..%08x  %s%s\n",
                           (unsigned)kRegions[i].base,
                           (unsigned)(kRegions[i].base + kRegions[i].size),
                           kRegions[i].name,
                           kRegions[i].fatal ? " (required)" : "");
    }
    for (unsigned k = 0; k < g_occ_n && n < (int)sizeof g_detail; ++k) {
        char namebuf[kNameMax];
        const char *who = occupant_name(g_occ[k], namebuf, sizeof namebuf);
        n += std::snprintf(g_detail + n, sizeof g_detail - n,
                           "[io]   in %08x: %08x..%08x %s -> %s\n",
                           (unsigned)g_occ[k].region_base,
                           (unsigned)g_occ[k].base,
                           (unsigned)(g_occ[k].base + g_occ[k].size),
                           occupant_state(g_occ[k]), who);
    }
    if (g_partial_main && n < (int)sizeof g_detail)
        std::snprintf(g_detail + n, sizeof g_detail - n,
                      "[io]   main memory recovered in pieces: %u KB of %u KB\n",
                      (unsigned)(g_main_held / 1024),
                      (unsigned)(MAIN_SIZE / 1024));
    return g_detail;
}

// The sentence a player reads. No jargon, and it leads with the action that
// actually works, because this failure really is usually transient: the one
// report of it came from someone whose next launch minutes later got in.
const char *io_reserve_player_text() {
    if (!g_lost_mask) return "";
    if (g_player[0]) return g_player;

    // Name the first REQUIRED range that was lost; that is the one that stopped
    // the game. A lost main memory range on its own does not.
    const char *lost_name = nullptr;
    unsigned lost_base = 0;
    for (unsigned i = 0; i < kRegionCount; ++i) {
        if (!g_held[i] && kRegions[i].fatal) {
            lost_name = kRegions[i].name;
            lost_base = (unsigned)kRegions[i].base;
            break;
        }
    }
    if (!lost_name) return "";

    // The clearest single fact we have: what was actually sitting there.
    char namebuf[MAX_PATH];
    const char *who = nullptr;
    for (unsigned k = 0; k < g_occ_n; ++k) {
        if (g_occ[k].region_base == lost_base) {
            who = occupant_name(g_occ[k], namebuf, sizeof namebuf);
            break;
        }
    }

    int n = std::snprintf(g_player, sizeof g_player,
        "The game needs one fixed range of memory addresses and could not get "
        "it this time, so it closed instead of starting.\n\n"
        "This is usually temporary. Press Play again. The same problem has been "
        "seen to clear on its own on the next attempt with nothing changed.\n\n"
        "If it keeps happening, restart the computer and try once more. Software "
        "that loads itself into other programs, such as antivirus, game "
        "overlays and screen recorders, can take these addresses first.\n\n"
        "What went wrong: the range the game calls %s was already in use.",
        lost_name);
    if (who && n > 0 && n < (int)sizeof g_player)
        std::snprintf(g_player + n, sizeof g_player - n,
                      "\nIt was held by: %s", who);
    return g_player;
}

unsigned io_reserve_lost_mask() { return g_lost_mask; }
unsigned io_reserve_attempts() { return g_attempts; }
int io_reserve_stage() { return g_stage_first; }
int io_reserve_stage_won() { return g_stage_won; }

}  // namespace ntr

/* ---- save-state capture of the game-mutable hardware regions ---------------
   The save state (hal/lk6_savestate.cpp) captures the arena and the .dsstate
   section, and for one area that is the whole game. What it did NOT capture
   until 0.2.2 is the hardware content stores this file reserves: palette
   memory, video memory and sprite memory. Their allocator CURSORS are hosted
   globals in .dsstate and rolled back fine; the BYTES at these addresses did
   not, so a save in one area restored after mounting another came back with
   the new area's textures under the old area's world -- the cross-area
   texture-destruction bug (the same visual damage as the stale-VRAM minimap
   bug, reached through the save state instead of a warp).

   These three hooks are the fix's transport. They live here because this file
   owns the region table and knows what is actually held; hal/lk6_savestate.cpp
   calls them through plain C declarations so it stays free of ntr headers, and
   smoke_savestate (which links no ntr) stubs them to size 0, keeping its
   capture exactly what it always was.

   The order of regions inside the blob is the kRegions order, fixed by
   construction. IO register shadows and the shared block are deliberately NOT
   captured: the game rewrites the per-frame registers on the next tick, and
   restoring shadows the host GX layer has already consumed would desynchronize
   host state rather than roll it back. Palette, video and sprite memory are
   the stores written at MOUNT time, which is exactly what a cross-area restore
   has to put back. */
namespace {
// The three content-store regions, by base address, in blob order.
const uintptr_t kHwCaptureBases[] = { ntr::PLTT_BASE, ntr::VRAM_BASE,
                                      ntr::OAM_BASE };

int hw_region_index(uintptr_t base)
{
    for (unsigned i = 0; i < ntr::kRegionCount; ++i)
        if (ntr::kRegions[i].base == base)
            return (int)i;
    return -1;
}
}  // namespace

extern "C" unsigned port_hw_regions_size(void)
{
    unsigned total = 0;
    for (uintptr_t base : kHwCaptureBases) {
        const int i = hw_region_index(base);
        if (i < 0 || !ntr::g_held[i])
            return 0;   /* any store missing: no hw capture (headless smokes) */
        total += (unsigned)ntr::kRegions[i].size;
    }
    return total;
}

extern "C" void port_hw_regions_copy_out(void *dst)
{
    char *p = (char *)dst;
    for (uintptr_t base : kHwCaptureBases) {
        const int i = hw_region_index(base);
        std::memcpy(p, (const void *)base, ntr::kRegions[i].size);
        p += ntr::kRegions[i].size;
    }
}

extern "C" void port_hw_regions_copy_in(const void *src)
{
    const char *p = (const char *)src;
    for (uintptr_t base : kHwCaptureBases) {
        const int i = hw_region_index(base);
        std::memcpy((void *)base, p, ntr::kRegions[i].size);
        p += ntr::kRegions[i].size;
    }
    /* The 3D texture decode cache keys on VRAM words plus a cheap content
       probe; a probe is not a proof, and the bytes under every key just
       changed. Drop it and let the next bind re-decode from the restored
       VRAM. The 2D side re-reads VRAM every frame and keeps no cache. */
    ntr::gx_invalidate_textures();
}

#if defined(_WIN32)
// ---------------------------------------------------------------------------
// THE EARLY CLAIM.
//
// A TLS callback is the earliest this program can run code. The loader calls
// the callbacks in .CRT$XLx order at DLL_PROCESS_ATTACH for the process, which
// is after the statically imported DLLs are initialised but BEFORE the CRT
// entry point, so before the CRT heap grows, before any static initialiser, and
// before anything this program loads for itself. That is as far up the startup
// sequence as the reservation can be moved without replacing the entry point.
//
// The two /INCLUDE directives are load-bearing. __tls_used pulls in the TLS
// directory (without it the callback array is never emitted at all) and the
// second keeps this callback pointer from being dropped as unreferenced data
// out of the static library. Both take the 32-bit leading underscore; io.cpp is
// always pulled from libntr because io_init is referenced, so the directives in
// this object always reach the linker.
extern "C" void NTAPI ntr_io_tls_callback(PVOID, DWORD reason, PVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        // SM64DS_IO_LATE=1 skips the early claim, leaving the reservation to
        // io_init the way it ran before this callback existed. It is the A/B
        // control for the whole change: same binary, the old timing.
        // GetEnvironmentVariableA is kernel32, so it is safe here; getenv is
        // not, because the CRT has not initialised yet.
        char v[8];
        if (GetEnvironmentVariableA("SM64DS_IO_LATE", v, sizeof v) == 0)
            ntr::io_reserve(1);
    }
}

#pragma section(".CRT$XLC", long, read)
extern "C" __declspec(allocate(".CRT$XLC"))
    PIMAGE_TLS_CALLBACK ntr_io_tls_entry = ntr_io_tls_callback;

#ifdef _WIN64
#pragma comment(linker, "/INCLUDE:_tls_used")
#pragma comment(linker, "/INCLUDE:ntr_io_tls_entry")
#else
#pragma comment(linker, "/INCLUDE:__tls_used")
#pragma comment(linker, "/INCLUDE:_ntr_io_tls_entry")
#endif
#endif  // _WIN32
