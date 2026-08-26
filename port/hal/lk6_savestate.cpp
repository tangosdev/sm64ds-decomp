// In-memory save state for the PC port (lane lk6).
//
// WHAT A SAVE STATE IS HERE
// -------------------------
// One in-memory slot. Snapshot (F8 / the debug menu) copies the whole hosted
// DS main-RAM arena plus every game-mutable hosted DS global that lives OUTSIDE
// the arena into a heap buffer. Restore (F9 / the debug menu) copies those
// bytes straight back and silences the audio mixer so nothing keeps playing off
// the pre-restore sequencer state. The slot survives for the process lifetime;
// it is not written to disk.
//
// WHY THE OUT-OF-ARENA SET IS A LINKER SECTION, NOT A LIST
// -------------------------------------------------------
// The 0.2.0 slot captured the arena plus a hand-picked list of 23 out-of-arena
// globals. Real players crashed after a restore: Player::SetAnim read a null
// BCA_File* out of the ANIM_PTRS SharedFilePtr table (ov002's
// data_ov002_020ff480), a hosted overlay global the list never named. The arena
// rolled back to the saved animation while that table -- Released and reloaded
// as the player animated between save and load -- did not, so the file pointer
// came back null. A dialogue or cutscene left the same way: the message-active
// lock (data_0209d660) is a hosted global outside the list, so a restore rolled
// the world back but left the game wedged in the message.
//
// A hand list that has to be grown for every hosted symbol IS that bug. So the
// out-of-arena set is now defined by a linker section, not a list. Every file
// that hosts DS globals brackets them with DSSTATE_BEGIN/END (hal/dsstate_seg.h)
// and the overlay-data generators emit the same markers, which routes all of
// them -- auto_bss, model_host, the overlay data/pack/whole mounts, the sound
// bss, every bridge -- into one .dsstate section. hal/dsstate_seg.cpp brackets
// that section with two sentinels, and the snapshot copies the bytes between
// them. A newly hosted global is captured the moment it is placed in the
// section; there is nothing to keep in sync by hand. The dsstate_guard build
// step (tools/dsstate_guard.py) fails the build if a hosted DS symbol landed
// OUTSIDE the section, so a file that forgets the markers is caught at build
// time rather than by a player.
//
// WHY THE ARENA IS THE BULK OF IT
// -------------------------------
// hal/os_arena.cpp reserves one calloc'd block and carves the game's root heap
// out of it (port_arena_base .. port_arena_end, low-water cursor at
// port_arena_cursor). Every heap allocation the game makes -- the Player, every
// Actor, the camera objects, the level's collision, the model registry records
// -- lives inside that block, so a memcpy of [base, end) captures the entire
// object graph in one shot. The block never moves for the life of the process,
// which is the property the whole scheme leans on: a pointer captured at save
// (an actor-list head, a model-matrix pointer, an intrusive next-link) still
// addresses the same arena offset after restore, now holding the restored
// bytes, so nothing has to be relocated.
//
// The arena carve cursor (port_arena_cursor) is itself mutable allocator state.
// If we restored the bytes but left the cursor where divergence pushed it, the
// next allocation would land past the restored high-water mark and stomp
// freshly restored data. So the cursor is captured and restored like any other
// value.
//
// WHAT LIVES OUTSIDE THE ARENA (and so is captured as the .dsstate section)
// ------------------------------------------------------------------------
// A large set of DS BSS/data symbols are hosted as ordinary C globals in
// hal/*.cpp and in the generated overlay-data files rather than inside the
// arena: the actor processing-list heads, the particle-engine RNG and its arena
// cursors, the common-model-data registry, the render camera matrix, the
// texture-VRAM cursors, the message and cutscene mode/lock flags, the ANIM_PTRS
// SharedFilePtr table every overlay actor animates through, and much more. All
// of them are routed into one linker section, .dsstate, by the DSSTATE_BEGIN/END
// markers in each hosting file (see hal/dsstate_seg.h). The snapshot copies the
// whole section in one shot, so any hosted global is captured by virtue of
// being in the section -- there is no per-symbol list to keep current.
//
// WHAT IS DELIBERATELY NOT CAPTURED (reset instead)
// -------------------------------------------------
// The SDAT host audio path keeps live sequencer players and mixer channels
// (hal/sdat/sseq.cpp g_pl/g_note, hal/sdat/mixer.cpp g_ch) plus a phase
// accumulator. Their storage IS in .dsstate and so is copied back like anything
// else, but the mixer phase and note envelopes would be a few frames ahead of
// the restored world if merely restored, so after the section copy the load
// path additionally silences the sequencer and mixer (sd_seq_reset +
// sd_mix_reset); the game re-triggers sounds from the restored state on the next
// tick. The wave decode cache (hal/sdat/sdat.cpp g_waves) is keyed by SWAV
// record addresses in the arena; those keys only mean the same thing when the
// restore does not cross an area, so the load path drops it (sd_waves_reset)
// and it refills on demand.
//
// THE HARDWARE CONTENT STORES (captured since 0.2.2)
// --------------------------------------------------
// Palette, video and sprite memory are reserved host memory at the DS addresses
// (ntr/io.cpp), written at area-mount time, and belong to neither the arena nor
// .dsstate. Not capturing them was the 0.2.1 cross-area bug: save, walk into a
// door or painting (the new area's textures upload over the same VRAM offsets),
// load -- the world rolls back, the pixels do not, and every texture reference
// resolves into the wrong area's bytes. The slot now carries the three regions
// via the port_hw_regions_* hooks, and restoring them drops the ntr texture
// decode cache so the next bind re-decodes from the restored VRAM.
//
// WHAT IS NOT IN .dsstate ON PURPOSE (host-only, must not roll back)
// -----------------------------------------------------------------
// Host bookkeeping that describes the SESSION, not the game: the playlog path
// and frame counter (walk_window.cpp), telemetry, the launcher glue, ntr_2x's
// host framebuffer/window state, and the C runtime's own globals (locale, FILE
// tables). None of those files use the DSSTATE markers, so their globals stay in
// the ordinary .bss/.data the snapshot never touches. Rolling the frame counter
// or a FILE* back would be wrong.

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#if defined(_WIN32)
/* the cross-process landmine scan at the bottom of this file classifies every
   captured word through VirtualQuery */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

typedef unsigned int u32;

// hal/sdat: silence the live audio path on restore. These are C++-linkage free
// functions (see hal/sdat/sdat.h), so they are declared OUTSIDE the extern "C"
// block below or the linker looks for a C-mangled symbol that does not exist.
void sd_seq_reset(void);
void sd_mix_reset(void);
void sd_consumer_reset(void);
void sd_waves_reset(void);
void sd_sdat_reseat(void);

extern "C" {
// hal/os_arena.cpp
void *port_arena_base(void);
void *port_arena_end(void);
void *port_arena_cursor(void);
void  port_arena_set_cursor(void *p);

// ntr/io.cpp: the game-mutable hardware content stores (palette, video and
// sprite memory). Their allocator cursors are hosted globals in .dsstate and
// always rolled back; the BYTES at the reserved DS addresses were not, which
// is the 0.2.1 cross-area bug: save, mount another area (its textures upload
// over the same VRAM offsets), load -- the world rolls back, the pixels do
// not. size is 0 when the regions are not reserved (the savestate smoke links
// no ntr and stubs all three); copy_in also drops the ntr texture decode
// cache, since the bytes under its keys just changed.
unsigned port_hw_regions_size(void);
void port_hw_regions_copy_out(void *dst);
void port_hw_regions_copy_in(const void *src);

// The two sentinels hal/dsstate_seg.cpp places at the low and high ends of the
// .dsstate section family. The captured out-of-arena region is the bytes
// between them: &dsstate_hi - &dsstate_lo, recomputed by the linker every build.
extern char dsstate_lo;
extern char dsstate_hi;
}

namespace {

char  *dsstate_base() { return &dsstate_lo; }
size_t dsstate_size()
{
    // The linker orders .dsstate$aaa (dsstate_lo) before every $mmm/$pk... run
    // and .dsstate$zzz (dsstate_hi) after them, so hi is always >= lo. Guard
    // anyway: a zero or inverted span means the section collapsed (a build
    // without the sentinels) and the caller must not copy a bogus length.
    char *lo = &dsstate_lo, *hi = &dsstate_hi;
    return hi > lo ? (size_t)(hi - lo) : 0;
}

struct Slot {
    int    valid;
    char  *arena;        // captured [base, end) bytes
    size_t arena_size;   // end - base
    void  *arena_cursor; // captured low-water carve pointer
    char  *globals;      // captured [dsstate_lo, dsstate_hi) bytes
    size_t globals_size;
    char  *hw;           // captured palette + video + sprite memory bytes
    size_t hw_size;      // port_hw_regions_size() at save; 0 = not captured
};

Slot g_slot;

} // namespace

extern "C" {

// Snapshot into the single slot. Returns 1 on success, 0 if allocation failed
// (the old slot is left intact on failure so a bad F8 does not lose a good
// earlier state). Safe to call every frame; it overwrites the slot.
int lk6_savestate_save(void)
{
    char *base = (char *)port_arena_base();
    char *end  = (char *)port_arena_end();
    if (!base || end <= base) {
        fprintf(stderr, "[savestate] no arena to capture; save ignored\n");
        return 0;
    }
    size_t asz = (size_t)(end - base);
    size_t gsz = dsstate_size();
    if (gsz == 0) {
        // The sentinels collapsed: the .dsstate section is empty, which only
        // happens in a build where hal/dsstate_seg.cpp did not link or no file
        // routed a global into the section. Capturing the arena alone would be
        // the old partial snapshot that crashed on restore, so refuse rather
        // than ship a state that cannot restore correctly.
        fprintf(stderr, "[savestate] .dsstate section is empty; save ignored "
                        "(build is missing the hosted-DS-state section)\n");
        return 0;
    }

    // The hardware content stores (palette, video, sprite memory). 0 in a
    // build that reserves none of them (the headless smokes); when present,
    // they are captured with everything else, because a save that skipped them
    // is exactly the 0.2.1 cross-area texture bug.
    size_t hsz = port_hw_regions_size();

    // Allocate first, commit second: if any malloc fails, keep the prior
    // slot untouched.
    char *na = (char *)malloc(asz);
    char *ng = (char *)malloc(gsz);
    char *nh = hsz ? (char *)malloc(hsz) : 0;
    if (!na || !ng || (hsz && !nh)) {
        free(na); free(ng); free(nh);
        fprintf(stderr, "[savestate] out of memory (%zu + %zu + %zu bytes); "
                        "save ignored\n", asz, gsz, hsz);
        return 0;
    }

    memcpy(na, base, asz);
    memcpy(ng, dsstate_base(), gsz);
    if (hsz)
        port_hw_regions_copy_out(nh);

    free(g_slot.arena);
    free(g_slot.globals);
    free(g_slot.hw);
    g_slot.arena        = na;
    g_slot.arena_size   = asz;
    g_slot.arena_cursor = port_arena_cursor();
    g_slot.globals      = ng;
    g_slot.globals_size = gsz;
    g_slot.hw           = nh;
    g_slot.hw_size      = hsz;
    g_slot.valid        = 1;

    fprintf(stderr, "[savestate] saved: arena %zu bytes, dsstate %zu bytes, "
                    "hw %zu bytes\n", asz, gsz, hsz);
    return 1;
}

// Restore from the slot. Returns 1 if a restore happened, 0 if there was no
// saved state (a no-op that is safe to trigger any time, including with F9
// before any F8) or the arena window changed shape underneath us.
int lk6_savestate_load(void)
{
    if (!g_slot.valid) {
        fprintf(stderr, "[savestate] no saved state; load is a no-op\n");
        return 0;
    }
    char *base = (char *)port_arena_base();
    char *end  = (char *)port_arena_end();
    if (!base || (size_t)(end - base) != g_slot.arena_size) {
        // The arena is fixed for the process lifetime, so this should never
        // happen; refuse rather than copy a mismatched length.
        fprintf(stderr, "[savestate] arena window changed (%zu vs saved %zu); "
                        "load refused\n",
                base ? (size_t)(end - base) : 0, g_slot.arena_size);
        return 0;
    }
    if (dsstate_size() != g_slot.globals_size) {
        // The section is fixed at link time, so a mismatch means the running
        // binary is not the one that saved (a disk state from another build,
        // once lk7's persistence lands). Refuse rather than copy a wrong length.
        fprintf(stderr, "[savestate] dsstate section changed (%zu vs saved "
                        "%zu); load refused\n",
                dsstate_size(), g_slot.globals_size);
        return 0;
    }
    if (port_hw_regions_size() != g_slot.hw_size) {
        // Same discipline for the hardware stores: the region set is fixed for
        // the process lifetime, so a mismatch means the slot came from a build
        // with a different capture shape. Refuse rather than restore a world
        // whose textures cannot be put back with it.
        fprintf(stderr, "[savestate] hw regions changed (%u vs saved %zu); "
                        "load refused\n",
                port_hw_regions_size(), g_slot.hw_size);
        return 0;
    }

    memcpy(base, g_slot.arena, g_slot.arena_size);
    memcpy(dsstate_base(), g_slot.globals, g_slot.globals_size);
    if (g_slot.hw_size)
        port_hw_regions_copy_in(g_slot.hw);   /* also drops the decode cache */
    port_arena_set_cursor(g_slot.arena_cursor);

    // The audio path is the one thing that cannot be memcpy'd back cleanly:
    // even though its storage rode back in with the section above, the live
    // sequencer phase and mixer envelopes would be a few frames ahead of the
    // restored world. Silence them and let the restored game re-trigger sound.
    sd_seq_reset();
    sd_mix_reset();
    // Same reasoning one layer down, and this one FAULTS rather than just
    // sounding wrong. The ARM7 command queue is described by DS-named globals
    // (captured) and by host cursors sitting beside them (not captured), so a
    // restore leaves the two halves disagreeing and func_0205b274 walks the
    // free list into a null. Re-seed both halves together.
    sd_consumer_reset();
    // The wave decode cache is keyed by SWAV record ADDRESSES in the arena.
    // Its own comment scoped that keying to a same-level restore; a cross-area
    // restore rolls the arena back to different sample data at the same
    // addresses, so the keys would serve the other area's decoded audio. Drop
    // it; it refills on demand.
    sd_waves_reset();
    // One captured global is a HOST BOOT POINTER, not game state: data_020a5bb8
    // holds sdat_init's heap-allocated root. An in-process restore rewrites it
    // with the same value; a DISK state from an earlier run (lk7) rewrites it
    // with a heap address that no longer exists, and the first sound lookup
    // walks it into a fault. Re-seat the live process's own root, which is
    // correct in both cases.
    sd_sdat_reseat();

    fprintf(stderr, "[savestate] restored: arena %zu bytes, dsstate %zu bytes, "
                    "hw %zu bytes, audio reset\n", g_slot.arena_size,
            g_slot.globals_size, g_slot.hw_size);
    return 1;
}

// 1 if the slot currently holds a state (drives the menu label).
int lk6_savestate_has(void) { return g_slot.valid; }

// THE CROSS-PROCESS LANDMINE SCAN.
//
// data_020a5bb8 (sdat's calloc'd root) is documented above as ONE captured
// global that holds a HOST HEAP pointer, with the note that "there may be more
// of these; they only bite CROSS-PROCESS, and only crash-driven runs find
// them". That note is a standing invitation to be surprised by a player. This
// answers it by measurement instead: walk the captured bytes as machine words
// and classify every one that looks like an address.
//
// A captured word may point at
//   the exe image      fine, /DYNAMICBASE:NO pins the base every launch
//   the hosted arena   fine, hal/os_arena.cpp pins it at 0x30000000
//   a DS reservation   fine, ntr/io.cpp reserves those at their DS addresses
//   .dsstate itself    fine, it is inside the image
// and anything else that is real committed private memory is a pointer into
// THIS PROCESS's heap or stack. Restored into another process it addresses
// whatever that process happened to put there -- or nothing at all. Those are
// the words a disk state cannot carry, and each one needs the same treatment
// data_020a5bb8 got: re-seat the live value after the copy.
//
// Regions are classified once and cached, so the scan is a few hundred
// VirtualQuery calls rather than one per word.
#if defined(_WIN32)
extern "C" IMAGE_DOS_HEADER __ImageBase;
namespace {
struct Region { uintptr_t lo, hi; int ok; };   /* ok: 1 safe, 0 suspect */
Region g_rgn[256];
int g_rgn_n;

int region_ok(uintptr_t p)
{
    for (int i = 0; i < g_rgn_n; ++i)
        if (p >= g_rgn[i].lo && p < g_rgn[i].hi)
            return g_rgn[i].ok;
    MEMORY_BASIC_INFORMATION mbi;
    if (!VirtualQuery((void *)p, &mbi, sizeof mbi))
        return 1;                       /* unmapped: not a live pointer */
    uintptr_t lo = (uintptr_t)mbi.BaseAddress;
    uintptr_t hi = lo + mbi.RegionSize;
    int ok = 1;
    if (mbi.State == MEM_COMMIT) {
        if (mbi.Type == MEM_IMAGE || mbi.Type == MEM_MAPPED)
            ok = 1;                     /* the exe (pinned) or a mapping */
        else {
            uintptr_t ab = (uintptr_t)mbi.AllocationBase;
            uintptr_t arena = (uintptr_t)port_arena_base();
            uintptr_t aend  = (uintptr_t)port_arena_end();
            if (p >= arena && p < aend)
                ok = 1;                 /* the pinned arena */
            else if (ab < 0x08000000u)
                ok = 1;                 /* a DS-address reservation */
            else
                ok = 0;                 /* host heap or stack: DEAD next run */
        }
    }
    if (g_rgn_n < (int)(sizeof g_rgn / sizeof g_rgn[0]))
        { g_rgn[g_rgn_n].lo = lo; g_rgn[g_rgn_n].hi = hi;
          g_rgn[g_rgn_n].ok = ok; ++g_rgn_n; }
    return ok;
}
}  // namespace
#endif

int lk6_savestate_scan_host_pointers(const char *when, int list_max)
{
#if !defined(_WIN32)
    (void)when; (void)list_max;
    return -1;
#else
    char *lo = dsstate_base();
    size_t gsz = dsstate_size();
    if (!lo || !gsz) return -1;
    g_rgn_n = 0;
    int bad = 0, listed = 0;
    const size_t n = gsz / sizeof(uintptr_t);
    const uintptr_t *w = (const uintptr_t *)lo;
    for (size_t i = 0; i < n; ++i) {
        uintptr_t p = w[i];
        if (p < 0x10000u) continue;          /* small ints, flags, counts */
        if (region_ok(p)) continue;
        ++bad;
        if (listed < list_max) {
            ++listed;
            fprintf(stderr, "[ss-scan]   .dsstate+0x%06x = %08x -- host "
                    "memory, DEAD in the next process\n",
                    (unsigned)(i * sizeof(uintptr_t)), (unsigned)p);
        }
    }
    fprintf(stderr, "[ss-scan] %s: %d captured word(s) point at this process's "
            "own heap or stack%s\n", when, bad,
            bad > listed ? " (list truncated)" : "");
    fflush(stderr);
    return bad;
#endif
}

// THE SAME QUESTION, THE OTHER WAY ROUND.
//
// The scan above finds captured bytes that cannot survive a restart. This one
// finds the opposite hazard, and it is the one the level-handoff comment in
// tests/walk_window.cpp already knows about: HOST storage that holds a pointer
// INTO THE WORLD. "EVERY POINTER main() HOLDS INTO THE LEVEL IS STALE
// AFTERWARDS" is written about a level change; a restore replaces the world
// the same way and re-derives nothing.
//
// So: walk every WRITABLE image section that is not .dsstate -- the host's own
// .data and .bss, which a restore never touches -- and count the words that
// address the hosted arena. Each one is a host variable pointing at an object
// that a restore has just replaced with different bytes.
//
// It cannot see function locals (walk_window's `player`, `c`, `cam` and
// `stage` are locals in main's frame), which is exactly why those are handled
// by re-deriving them at the call site rather than by this census. What it
// does see is every FILE-SCOPE host mirror of world state, which is the class
// hal/level_boot.cpp's handle table and entrance cache belong to.
int lk6_savestate_scan_world_pointers(const char *when, int list_max)
{
#if !defined(_WIN32)
    (void)when; (void)list_max;
    return -1;
#else
    char *base = (char *)port_arena_base();
    char *end  = (char *)port_arena_end();
    if (!base || end <= base) return -1;
    /* Only the CARVED part of the arena counts. The whole 8MB window is a
       plausible-looking numeric range and .data is full of ROM bytes and
       string literals that fall inside it by accident (0x30523030 is the
       four characters "00R0"). Everything above the carve cursor is memory
       nothing has handed out, so a word pointing there is noise by
       construction. */
    char *hi_used = (char *)port_arena_cursor();
    if (hi_used > base && hi_used < end) end = hi_used;
    char *dlo = dsstate_base();
    char *dhi = dlo + dsstate_size();

    char *img = (char *)&__ImageBase;
    IMAGE_NT_HEADERS32 *nt = (IMAGE_NT_HEADERS32 *)(img +
        ((IMAGE_DOS_HEADER *)img)->e_lfanew);
    IMAGE_SECTION_HEADER *s = IMAGE_FIRST_SECTION(nt);
    int bad = 0, listed = 0;
    for (unsigned i = 0; i < nt->FileHeader.NumberOfSections; ++i, ++s) {
        if (!(s->Characteristics & IMAGE_SCN_MEM_WRITE))
            continue;
        char *a = img + s->VirtualAddress;
        char *b = a + s->Misc.VirtualSize;
        for (char *p = a; p + sizeof(uintptr_t) <= b;
             p += sizeof(uintptr_t)) {
            if (p >= dlo && p < dhi) continue;      /* captured: rolls back */
            char *v = *(char **)p;
            if (v < base || v >= end) continue;
            ++bad;
            if (listed < list_max) {
                ++listed;
                fprintf(stderr, "[ss-rscan]   %.8s+0x%06x (%08x) = %08x -- "
                        "host storage pointing INTO the world\n", s->Name,
                        (unsigned)(p - a), (unsigned)(size_t)p,
                        (unsigned)(size_t)v);
            }
        }
    }
    fprintf(stderr, "[ss-rscan] %s: %d host word(s) point into the arena a "
            "restore just replaced%s\n", when, bad,
            bad > listed ? " (list truncated)" : "");
    fflush(stderr);
    return bad;
#endif
}

// Where a byte lives with respect to the snapshot: bit 0 set if it is inside
// the .dsstate section, bit 1 set if it is inside the hosted arena, and 0 for
// neither -- which means it does NOT roll back on a load.
//
// This exists so a test can ask the question directly instead of deriving it
// from the linked map. Reading it off the map is a two-step inference (find
// the symbol, compare against the sentinels) that only answers for a symbol
// somebody already suspected. A pointer the game built at run time can be
// handed to this and answered without knowing its name, which is what the
// packed-gap reproducer needs: it follows the game's own pointer to whichever
// host array the generator put a DS address in, then asks whether that array
// is captured. Cheap enough to call in a probe, and it reads the same two
// sentinels the save path does, so it cannot drift from what is captured.
int lk6_savestate_covers(const void *p)
{
    int r = 0;
    const char *c = (const char *)p;
    const char *lo = dsstate_base();
    size_t gsz = dsstate_size();
    if (gsz && c >= lo && c < lo + gsz)
        r |= 1;
    const char *ab = (const char *)port_arena_base();
    const char *ae = (const char *)port_arena_end();
    if (ab && ae > ab && c >= ab && c < ae)
        r |= 2;
    return r;
}

}

// ---- COVERAGE NOTE ---------------------------------------------------------
// COVERED: the whole hosted DS main-RAM arena (the entire heap object graph:
// Player, actors, camera, collision, model records, allocations) plus the arena
// carve cursor, and EVERY hosted DS global routed into the .dsstate section by
// the DSSTATE_BEGIN/END markers -- the actor processing-list heads, the heap
// registry root iterator, the Process cleanup list, the common-model-data
// registry, the render camera matrix, the texture-VRAM cursors, the particle
// RNG, the message/cutscene mode and lock flags, the ANIM_PTRS SharedFilePtr
// table and every other overlay-data global, the sound bss, and the bridge
// state. The set is defined by section membership, not by a list here, so a
// newly hosted global is captured the moment its file uses the markers. The
// dsstate_guard build step fails the build if a hosted DS symbol landed outside
// the section.
//
// RESET ON LOAD (captured, then re-silenced): the SDAT sequencer players, mixer
// channels and phase accumulator ride back in with the section, but the live
// mixer phase would be a few frames ahead of the restored world, so the load
// path calls sd_seq_reset / sd_mix_reset after the copy and the game re-triggers
// sound from the restored state on the next tick. This is the one deliberate
// departure from a byte-exact restore.
//
// DELIBERATELY OUTSIDE .dsstate (host-only, must NOT roll back): the playlog
// path and frame counter, telemetry, the launcher glue, ntr_2x's host window
// and framebuffer state, and the C runtime's own globals (locale, FILE tables).
// Those files do not use the DSSTATE markers, so their storage stays in the
// ordinary .bss/.data the snapshot never touches. Rolling the frame counter or a
// FILE* back would corrupt the session, not the game.
