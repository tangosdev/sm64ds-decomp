// The host I/O bus: a real memory window at the DS I/O addresses, plus side-effect
// dispatch for the registers whose writes do something.
//
// Register semantics here are from GBATEK. No SDK source is reproduced.

#include "ntr/mmio.h"

#include "ntr/gx.h"

#include <cstdio>
#include <cstring>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <sys/mman.h>
/* The reservation diagnostics below size a name buffer with MAX_PATH, which is
   a windows.h constant. Same spelling and purpose as the one in
   hal/host_platform_linux.h, defined locally rather than including that header
   so this library keeps not depending on the HAL. Sized for a host path rather
   than Windows' 260, since that is what the buffer holds. */
#ifndef MAX_PATH
#define MAX_PATH 4096
#endif
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

void run_divide() {
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

void run_sqrt() {
    const uint16_t mode = static_cast<uint16_t>(raw_read(SQRTCNT, 2)) & 1;
    uint64_t param = raw_read(SQRT_PARAM, 8);
    if (mode == 0) param &= 0xFFFFFFFFull;      // 32-bit mode
    raw_write(SQRT_RESULT, isqrt64(param), 4);
}

}  // namespace

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

uint64_t io_read(uint32_t addr, unsigned width) {
    if (!g_io && !io_init()) return 0;
    return raw_read(addr, width);
}

void io_write(uint32_t addr, uint64_t value, unsigned width) {
    if (!g_io && !io_init()) return;
    raw_write(addr, value, width);

    // Writing the low half of the operand is what starts the unit on hardware.
    if (addr == DIV_DENOM || addr == DIV_NUMER || addr == DIVCNT) run_divide();
    else if (addr == SQRT_PARAM || addr == SQRTCNT) run_sqrt();
    // Geometry: 0x4000400 is the packed FIFO, 0x4000440.. are the command ports.
    // These execute on write and leave nothing readable behind, so unlike every
    // other block they cannot be served by the memory window alone.
    else if (addr == 0x04000400u) gx_write_fifo(static_cast<uint32_t>(value));
    else if (addr >= 0x04000440u && addr <= 0x040005FFu)
        gx_write_port(addr, static_cast<uint32_t>(value));
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
