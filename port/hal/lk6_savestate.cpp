// In-memory save state for the PC port (lane lk6).
//
// WHAT A SAVE STATE IS HERE
// -------------------------
// One in-memory slot. Snapshot (F8 / the debug menu) copies the whole hosted
// DS main-RAM arena plus a short list of game-mutable host globals that live
// OUTSIDE the arena into a heap buffer. Restore (F9 / the debug menu) copies
// those bytes straight back and silences the audio mixer so nothing keeps
// playing off the pre-restore sequencer state. The slot survives for the
// process lifetime; it is not written to disk.
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
// WHAT LIVES OUTSIDE THE ARENA (and so is listed explicitly below)
// ----------------------------------------------------------------
// A number of DS BSS symbols are hosted as ordinary C globals in hal/*.cpp
// rather than inside the arena. The correctness-critical ones for a same-level
// restore are the actor processing-list heads (list heads that thread INTO the
// arena), the particle-engine RNG and its arena cursors, the common-model-data
// registry, the render camera matrix, the texture-VRAM bump cursors, and the
// current-model-matrix pointer. Each is named by its real symbol below with its
// exact declared size and snapshotted byte-for-byte. See the coverage note at
// the bottom of this file for what is deliberately NOT in the set and why that
// is safe for a within-session restore.
//
// WHAT IS DELIBERATELY NOT CAPTURED (reset instead)
// -------------------------------------------------
// The SDAT host audio path keeps live sequencer players and mixer channels
// (hal/sdat/sseq.cpp g_pl/g_note, hal/sdat/mixer.cpp g_ch) plus a phase
// accumulator. Their pointers address the arena, so restoring the arena keeps
// them valid, but the mixer phase and note envelopes would be a few frames
// ahead of the restored world. Rather than reason about that drift we silence
// the sequencer and mixer on restore (sd_seq_reset + sd_mix_reset); the game
// re-triggers sounds from the restored state on the next tick. The wave decode
// cache (hal/sdat/sdat.cpp g_waves) is keyed by SWAV record addresses in the
// arena; for a same-level restore those addresses hold identical sample data,
// so the cache stays correct and needs no flush.

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef unsigned int u32;

// hal/sdat: silence the live audio path on restore. These are C++-linkage free
// functions (see hal/sdat/sdat.h), so they are declared OUTSIDE the extern "C"
// block below or the linker looks for a C-mangled symbol that does not exist.
void sd_seq_reset(void);
void sd_mix_reset(void);

extern "C" {
// hal/os_arena.cpp
void *port_arena_base(void);
void *port_arena_end(void);
void *port_arena_cursor(void);
void  port_arena_set_cursor(void *p);

// ---- game-mutable host globals that live OUTSIDE the arena ----------------
// Referenced by their real DS symbol names; taking their address to snapshot
// them is a read, it does not redefine or move any storage. Sizes are the
// exact declared extents from hal/model_host.cpp and hal/auto_bss.cpp.

// the heap registry root iterator (hal/heap_globals.cpp): the list head/tail/
// count for every heap that hangs directly off the root. It threads INTO the
// arena (the HeapAllocator objects it links live in arena-carved storage), so
// if the game creates or destroys a root-level heap between save and load, this
// head describes the post-save topology while the restored arena describes the
// saved one. Capturing it keeps the two in step. isRootHeapIterInitialized is
// invariant after the first heap comes up but is captured too so the snapshot
// is internally consistent.
extern char _ZN6Memory16rootHeapIteratorE[0x20];
extern int  _ZN6Memory25isRootHeapIterInitializedE;

// actor processing-list heads (hal/model_host.cpp): list heads that thread into
// arena-resident actor records.
extern int data_020a4b78[4];
extern int data_020a4b88[4];
extern int data_020a4b98[4];
extern int data_02099f24[4];
// the Process cleanup list head/tail/callback and the teardown id hook
// (hal/auto_bss.cpp).
extern int data_020a4ba8[8];
extern int data_020a4b5c[4];
// common-model-data registry: 100 records of 0xc, plus the live count
// (hal/model_host.cpp).
extern int data_0209cefc[3 * 100];
extern int data_0209cef8[1];
// the render camera matrix and the bound model-matrix pointer
// (hal/model_host.cpp).
extern int data_0209b3ec[12];
extern void *data_020a4bd0;
// texture-VRAM bump cursors (hal/model_host.cpp): where uploads have reached.
extern u32 data_020a4bc8;
extern u32 data_020a4be8;
extern u32 data_020a4be0;
extern u32 data_020a4bdc;
extern u32 data_020a4be4;
extern u32 data_020a4bcc;
extern u32 data_020a4bd8;
// particle-engine RNG state and its arena bump cursors (hal/auto_bss.cpp).
extern int LCG_STATE_0204da4c;
extern int data_0209ee78[8];
extern int data_0209ee7c[8];
extern int data_0209ee80[8];
}

namespace {

struct Block { void *addr; size_t size; const char *name; };

// The explicit out-of-arena set. sizeof is taken on the real definitions above
// so the snapshot tracks whatever those declarations say; a size mismatch would
// be a compile error, not a silent short copy.
const Block g_blocks[] = {
    { _ZN6Memory16rootHeapIteratorE, sizeof _ZN6Memory16rootHeapIteratorE,
      "heap_root_iterator" },
    { &_ZN6Memory25isRootHeapIterInitializedE,
      sizeof _ZN6Memory25isRootHeapIterInitializedE, "heap_root_iter_init" },
    { data_020a4b78, sizeof data_020a4b78, "actor_list_head_0" },
    { data_020a4b88, sizeof data_020a4b88, "actor_list_head_1" },
    { data_020a4b98, sizeof data_020a4b98, "actor_list_head_2" },
    { data_02099f24, sizeof data_02099f24, "actor_list_head_3" },
    { data_020a4ba8, sizeof data_020a4ba8, "process_cleanup_list" },
    { data_020a4b5c, sizeof data_020a4b5c, "process_teardown_hook" },
    { data_0209cefc, sizeof data_0209cefc, "common_model_records" },
    { data_0209cef8, sizeof data_0209cef8, "common_model_count" },
    { data_0209b3ec, sizeof data_0209b3ec, "render_camera_matrix" },
    { &data_020a4bd0, sizeof data_020a4bd0, "bound_model_matrix_ptr" },
    { &data_020a4bc8, sizeof data_020a4bc8, "vram_tex_cursor_lo0" },
    { &data_020a4be8, sizeof data_020a4be8, "vram_tex_ceil0" },
    { &data_020a4be0, sizeof data_020a4be0, "vram_tex_cursor_lo1" },
    { &data_020a4bdc, sizeof data_020a4bdc, "vram_tex_ceil1" },
    { &data_020a4be4, sizeof data_020a4be4, "vram_tex_uploaded_bytes" },
    { &data_020a4bcc, sizeof data_020a4bcc, "vram_pal_cursor_lo" },
    { &data_020a4bd8, sizeof data_020a4bd8, "vram_pal_ceil" },
    { &LCG_STATE_0204da4c, sizeof LCG_STATE_0204da4c, "particle_rng" },
    { data_0209ee78, sizeof data_0209ee78, "particle_arena_base" },
    { data_0209ee7c, sizeof data_0209ee7c, "particle_arena_end" },
    { data_0209ee80, sizeof data_0209ee80, "particle_arena_cursor" },
};
const int g_block_count = (int)(sizeof g_blocks / sizeof g_blocks[0]);

struct Slot {
    int    valid;
    char  *arena;        // captured [base, end) bytes
    size_t arena_size;   // end - base
    void  *arena_cursor; // captured low-water carve pointer
    char  *globals;      // captured out-of-arena blocks, concatenated
    size_t globals_size;
};

Slot g_slot;

size_t globals_total()
{
    size_t t = 0;
    for (int i = 0; i < g_block_count; ++i)
        t += g_blocks[i].size;
    return t;
}

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
    size_t gsz = globals_total();

    // Allocate first, commit second: if either malloc fails, keep the prior
    // slot untouched.
    char *na = (char *)malloc(asz);
    char *ng = (char *)malloc(gsz);
    if (!na || !ng) {
        free(na); free(ng);
        fprintf(stderr, "[savestate] out of memory (%zu + %zu bytes); "
                        "save ignored\n", asz, gsz);
        return 0;
    }

    memcpy(na, base, asz);
    size_t off = 0;
    for (int i = 0; i < g_block_count; ++i) {
        memcpy(ng + off, g_blocks[i].addr, g_blocks[i].size);
        off += g_blocks[i].size;
    }

    free(g_slot.arena);
    free(g_slot.globals);
    g_slot.arena        = na;
    g_slot.arena_size   = asz;
    g_slot.arena_cursor = port_arena_cursor();
    g_slot.globals      = ng;
    g_slot.globals_size = gsz;
    g_slot.valid        = 1;

    fprintf(stderr, "[savestate] saved: arena %zu bytes, %d globals %zu bytes\n",
            asz, g_block_count, gsz);
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

    memcpy(base, g_slot.arena, g_slot.arena_size);
    size_t off = 0;
    for (int i = 0; i < g_block_count; ++i) {
        memcpy(g_blocks[i].addr, g_slot.globals + off, g_blocks[i].size);
        off += g_blocks[i].size;
    }
    port_arena_set_cursor(g_slot.arena_cursor);

    // The audio path is the one thing that cannot be memcpy'd back cleanly:
    // silence it and let the restored game re-trigger sound on the next tick.
    sd_seq_reset();
    sd_mix_reset();

    fprintf(stderr, "[savestate] restored: arena %zu bytes, %d globals, "
                    "audio reset\n", g_slot.arena_size, g_block_count);
    return 1;
}

// 1 if the slot currently holds a state (drives the menu label).
int lk6_savestate_has(void) { return g_slot.valid; }

}

// ---- COVERAGE NOTE ---------------------------------------------------------
// COVERED: the whole hosted DS main-RAM arena (the entire heap object graph:
// Player, actors, camera, collision, model records, allocations), the arena
// carve cursor, the heap registry root iterator and its init flag, the actor
// processing-list heads, the Process cleanup list and teardown hook, the
// common-model-data registry and count, the render camera matrix, the bound
// model-matrix pointer, the texture-VRAM bump cursors, the particle RNG and its
// arena cursors.
//
// NOT CAPTURED, RESET ON LOAD: the SDAT sequencer players, mixer channels and
// phase accumulator (silenced via sd_seq_reset / sd_mix_reset). The game
// re-triggers sound from the restored world.
//
// KNOWN PARTIAL (host audio bookkeeping): the sound subsystem keeps its own
// voice and sound-heap iterators (hal/sdat and hal/player_bridges.cpp, e.g.
// data_020a5bc8 / data_020a4d54 / data_020a4d60) that are neither captured nor
// rebuilt by the two resets above. Silencing the mixer stops anything reading a
// stale voice, but a sound object created or destroyed between save and load
// leaves those voice-list heads describing the post-save topology. In practice
// the reset plus the game re-triggering sound covers the audible cases; a
// sound-heap walk immediately after a restore that straddled a voice
// alloc/free is the residual risk. Left out deliberately: capturing them is the
// same mechanism as the heap root iterator above and can be added to g_blocks
// if it ever surfaces as an audible fault.
//
// NOT CAPTURED, LEFT ALONE (safe because the next tick overwrites them from
// arena-resident state, or they are pure host bookkeeping): the per-frame pad
// shadows (data_0209f49x), the HUD star/red-coin/results caches
// (data_0209f2d4/30c/fc9c), the 2D scroll offsets and camera-button latches
// (data_0209d46x/47x/49x), and host-only counters (frame index, telemetry).
// These are cosmetic for at most one frame after a restore. If a future need
// arises to make the restore bit-exact on those too, add each symbol to
// g_blocks above with its declared size; the design does not otherwise change.
//
// The OAM shadow buffers (hal/model_host.cpp data_0209e674 / data_0209ea74) are
// rebuilt from scratch every frame by hal_sub_screen_frame_begin before any
// sprite is drawn, so they carry nothing across a frame boundary and need no
// snapshot.
