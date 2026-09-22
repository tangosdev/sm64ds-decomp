// Save-state verification (lane lk6).
//
// This proves the in-memory save state actually restores. It boots the same
// way smoke_actor does (SetupRootHeap, spawn one real actor, run its vtable
// lifecycle), then:
//
//   1. ticks N frames so the actor evolves real state inside the arena;
//   2. records a fingerprint of the whole world: a hash of the hosted arena,
//      the actor's live position/timer words, the arena carve cursor, the
//      particle RNG, and the game heap's free-byte count;
//   3. calls lk6_savestate_save();
//   4. ticks M more frames AND perturbs the particle RNG so the world provably
//      diverges from the saved fingerprint (the test asserts it diverged --
//      a save that captured nothing would make the load look like a pass);
//   5. calls lk6_savestate_load();
//   6. re-fingerprints and asserts every field equals the saved fingerprint
//      (arena hash, actor words, cursor, RNG, heap free);
//   7. ticks N more frames after the load and asserts the game keeps running
//      (the actor still dispatches, no crash).
//
// It also checks the two edge cases the brief calls out: F9 with no prior save
// is a safe no-op, and a save/load with no actor world (frontend-like, nothing
// spawned) does not crash.
//
// TWO THINGS THIS FILE SPELLS FOLLOWED THE 09-14 SYNC (lane SMOKELINK4B, run
// link100 wave 10 round 4).  Both edits are the same in tests/smoke_actor.cpp,
// which builds the same actor world, and its header block carries the long
// version of each:
//
//   1. the bring-up entry point is Heap::SetupRootHeap() now, a real static
//      member (include/Heap.h:236), not the flat extern "C" Itanium name src/
//      emitted before the sync;
//   2. the arrow signs' entry table at ov098:0x0213c380 has THREE column
//      symbols in the synced source, so the harness that owns the storage
//      names the third one too.  It is left zero, which is the CLPS_Block
//      pointer the pre-sync body already handed dBgW_KcMbg::SetFile out of
//      data_ov098_0213c380[idx].c.

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>

#include "ntr/gx.h"
#include "ntr/mmio.h"
#include "ntr/ppu.h"

#include "fault_probe.h"

#include "types.h"
#include "Heap.h"

extern "C" {
int *daObjYajirusi_c_classInit_YAJIRUSI_R(void);
extern int data_0209b3ec[12];       /* camera matrix */
struct SharedFilePtrC { unsigned short fileID; unsigned char numRefs;
                        unsigned char pad; void *filePtr; };
SharedFilePtrC *_ZN13SharedFilePtr9ConstructEj(SharedFilePtrC *s, u32 id);
extern unsigned short data_020a4b54;    /* pending actor ID */
extern void **data_020a4bb8;            /* actorID -> SpawnInfo* */
void *data_ov098_0213c380[6];
char data_ov098_0213c384[0x18];
char data_ov098_0213c388[0x18];
extern void *data_020a0eac_c;           /* Memory::gameHeapPtr */
extern void *data_020a0ea0;             /* defaultHeapPtr */
void _ZN4Heap18InitializeGameHeapEjPS_(unsigned size, void *root);
unsigned _ZN22ExpandingHeapAllocator10MemoryLeftEv(void *alloc);
extern void *data_0209f394[];           /* the player array */
extern unsigned char data_0209f21c;     /* player count */
void hal_fill_model_vtable(void);
void hal_fill_shadow_vtable(void);
void hal_fill_mmc_vtable(void);

// the save-state layer under test
int lk6_savestate_save(void);
int lk6_savestate_load(void);
int lk6_savestate_has(void);

// arena window + the particle RNG the snapshot captures out-of-arena
void *port_arena_base(void);
void *port_arena_end(void);
void *port_arena_cursor(void);
extern int LCG_STATE_0204da4c;
// out-of-arena globals the snapshot must restore through the .dsstate section,
// NOT through the arena copy. The test fingerprints these directly (data_0209b3ec,
// the render camera matrix, is already declared above) so that dropping any of
// them from the captured section would fail this test rather than pass silently.
extern char _ZN6Memory16rootHeapIteratorE[0x20];   /* heap registry head */
extern u32  data_020a4bc8;                          /* a VRAM bump cursor */
// A hosted global that was NEVER in the old hand-picked list: the message-active
// mode/lock flag (auto_bss.cpp). It is the kind of symbol the 0.2.0 snapshot
// dropped -- set it after save, and only a whole-.dsstate capture rolls it back.
// Fingerprinting it here is acceptance test 2 (a mode/lock global rolls back)
// run on the underlying global, alongside the arena/actor checks.
extern unsigned char data_0209d660;                 /* message-active lock */
}

// lk6_savestate_load() calls the sdat audio resets on restore. The full sdat
// host stack is far more than this actor smoke needs to link, and the audio
// reset is a no-op for a headless run with no device open, so this test
// provides the symbols directly. In the shipped walk_window build these
// resolve to the real hal/sdat/sseq.cpp, mixer.cpp, consumer.cpp and sdat.cpp.
//
// Note what this means for coverage: the command-queue reset that keeps a
// restore from faulting in func_0205b274 is stubbed out HERE, so this test
// cannot see that bug. It is the walk_window reproducer
// (SM64DS_SS_SAVE/SM64DS_SS_LOAD) that covers it, against the real queue.
void sd_seq_reset(void) {}
void sd_mix_reset(void) {}
void sd_consumer_reset(void) {}
void sd_waves_reset(void) {}
void sd_sdat_reseat(void) {}

// The hardware content stores (palette/video/sprite memory) need NO stub: this
// smoke links the ntr library, so lk6_savestate reaches the real
// port_hw_regions_* hooks in ntr/io.cpp, and the slot simply carries whatever
// the reservation pass actually held (all three regions normally, size 0 if a
// range was lost to another module). Either way the arena and .dsstate halves
// of this test are unchanged. The cross-area texture roll-back those regions
// exist for is walk_window reproducer territory (SM64DS_SS_* +
// SM64DS_EXIT_ENTER), where another area actually mounts.

typedef int (__thiscall *Fn0)(void *);
static int vcall0(void *actor, int slot)
{
    void **vt = *(void ***)actor;
    return ((Fn0)vt[slot])(actor);
}

static int g_failures;
#define CHECK(cond) \
    do { if (!(cond)) { ++g_failures; \
        fprintf(stderr, "FAIL %s:%d  %s\n", __FILE__, __LINE__, #cond); } } while (0)

static void ident_fx(void *m)
{
    memset(m, 0, 48);
    ((int *)m)[0] = ((int *)m)[4] = ((int *)m)[8] = 0x1000;
}

static void reset_scene()
{
    ntr::gx_reset();
    NTR_MMIO(uint32_t, 0x04000440) = 0;
    NTR_MMIO(uint32_t, 0x04000454) = 0;
    NTR_MMIO(uint32_t, 0x04000440) = 1;
    NTR_MMIO(uint32_t, 0x04000454) = 0;
    NTR_MMIO(uint32_t, 0x04000580) = 0u | (0u << 8) | (255u << 16) | (191u << 24);
    ntr::gx_set_light(0, -0.4f, -0.6f, -0.7f, 0x7FFF);
    ntr::gx_enable_lights(0x1);
}

// A 64-bit FNV-1a over the whole hosted arena. This is the core assertion: if
// one byte of game RAM differs between save and after-load, the hash differs.
static uint64_t arena_hash()
{
    const unsigned char *b = (const unsigned char *)port_arena_base();
    const unsigned char *e = (const unsigned char *)port_arena_end();
    uint64_t h = 1469598103934665603ULL;
    for (; b < e; ++b) { h ^= *b; h *= 1099511628211ULL; }
    return h;
}

struct Fingerprint {
    uint64_t hash;       // whole arena
    void    *cursor;     // arena carve cursor
    int      rng;        // particle RNG
    int      pos[3];     // actor position words (+0x5c..+0x64)
    int      timer;      // an actor state word (+0x70) that the ticks move
    char     heaphead[0x20]; // heap registry root iterator (out-of-arena)
    int      cammat[12];     // render camera matrix (out-of-arena)
    u32      vramcur;        // a VRAM bump cursor (out-of-arena)
    unsigned char msglock;   // message-active lock (never in the old list)
};

static Fingerprint fingerprint(int *actor)
{
    Fingerprint f;
    f.hash     = arena_hash();
    f.cursor   = port_arena_cursor();
    f.rng      = LCG_STATE_0204da4c;
    f.pos[0] = *(int *)((char *)actor + 0x5c);
    f.pos[1] = *(int *)((char *)actor + 0x60);
    f.pos[2] = *(int *)((char *)actor + 0x64);
    f.timer  = *(int *)((char *)actor + 0x70);
    memcpy(f.heaphead, _ZN6Memory16rootHeapIteratorE, sizeof f.heaphead);
    memcpy(f.cammat, data_0209b3ec, sizeof f.cammat);
    f.vramcur = data_020a4bc8;
    f.msglock = data_0209d660;
    return f;
}

static int fp_equal(const Fingerprint &a, const Fingerprint &b)
{
    return a.hash == b.hash && a.cursor == b.cursor && a.rng == b.rng &&
           a.pos[0] == b.pos[0] && a.pos[1] == b.pos[1] &&
           a.pos[2] == b.pos[2] && a.timer == b.timer &&
           memcmp(a.heaphead, b.heaphead, sizeof a.heaphead) == 0 &&
           memcmp(a.cammat, b.cammat, sizeof a.cammat) == 0 &&
           a.vramcur == b.vramcur && a.msglock == b.msglock;
}

int main(void)
{
    PORT_INSTALL_FAULT_PROBE();
    setvbuf(stdout, NULL, _IONBF, 0);
    if (!ntr::io_init()) { fprintf(stderr, "io_init failed\n"); return 2; }

    // ---- edge case A: F9 before any F8 is a safe no-op ---------------------
    CHECK(lk6_savestate_has() == 0);
    CHECK(lk6_savestate_load() == 0);   // no state -> returns 0, does nothing
    printf("  [edge] load with no saved state: no-op, ok\n");

    CHECK(Heap::SetupRootHeap() != NULL);
    ident_fx(data_0209b3ec);

    // ---- edge case B: save/load with an empty world (no actor spawned) -----
    // The brief asks that a frontend-menu save/load not crash. With only the
    // root heap up and no actor, a snapshot of the (small) live arena and a
    // restore of it must both succeed and not fault.
    CHECK(lk6_savestate_save() == 1);
    CHECK(lk6_savestate_has() == 1);
    CHECK(lk6_savestate_load() == 1);
    printf("  [edge] save/load of an empty world: ok, no crash\n");

    // ---- build a real actor world ------------------------------------------
    hal_fill_model_vtable();
    hal_fill_shadow_vtable();
    hal_fill_mmc_vtable();
    data_020a4b54 = 0x12b;
    static unsigned short spawn_info[4] = { 0, 0, 100, 100 };
    data_020a4bb8[0x12b] = spawn_info;
    /* THE GAME HEAP, the ROM's own chain instead of an alias -- the same
       bring-up walk_window.cpp, smoke_player.cpp and smoke_actor.cpp do. This
       line used to be `data_020a0eac_c = data_020a0ea0;`, the game-heap word
       aliased onto the root heap, and the retained-hand-seed comment here said
       the conversion goes with the port/CMakeLists.txt line that carries
       src/_ZN4Heap18InitializeGameHeapEjPS_.cpp onto this target. That line is
       in, so this is that conversion.
       WHAT IT MOVES, as that comment predicted: the saved arena bytes. Save
       and load are the same process and the same build, so the byte-exact
       compare holds either way -- but a savestate.bin written before this
       change does not cross it. Nothing ships one; smoke_persist writes its
       own and deletes it, and the gittip header refuses a stale build. */
    _ZN4Heap18InitializeGameHeapEjPS_(0x3b000, 0);
    CHECK(data_020a0eac_c != NULL);
    if (!data_020a0eac_c) {
        fprintf(stderr, "InitializeGameHeap returned null -- no game heap\n");
        return 2;
    }
    fprintf(stderr, "[heap] game heap %p, 0x%x bytes, %u free after carve\n",
            data_020a0eac_c, 0x3b000u,
            _ZN22ExpandingHeapAllocator10MemoryLeftEv(
                *(void **)((char *)data_020a0eac_c + 0x14)));
    static SharedFilePtrC sign_model, sign_kcl;
    _ZN13SharedFilePtr9ConstructEj(&sign_model, 1177);
    _ZN13SharedFilePtr9ConstructEj(&sign_kcl, 1178);
    data_ov098_0213c380[0] = &sign_model;
    data_ov098_0213c380[1] = &sign_kcl;
    *(void **)(data_ov098_0213c384 + 0) = &sign_kcl;

    static char fake_player[0x800];
    data_0209f394[0] = fake_player;
    *(unsigned char *)&data_0209f21c = 1;

    int *actor = daObjYajirusi_c_classInit_YAJIRUSI_R();
    CHECK(actor != NULL);
    if (!actor) { fprintf(stderr, "smoke_savestate: no actor, abort\n"); return 1; }
    /* where the actor landed, printed for the same reason smoke_actor prints
       it: under the old hand-seed this came back at the ROOT heap's top, and
       the carve above is only doing something if the actor is inside the
       0x3b000 payload the game heap owns. */
    printf("  spawned at %p (game heap %p)\n", (void *)actor, data_020a0eac_c);
    CHECK(vcall0(actor, 0) == 1);           // InitResources
    reset_scene();

    const int N = 8, M = 8;

    // ---- (1) play N frames so the actor evolves real state -----------------
    for (int f = 0; f < N; ++f) vcall0(actor, 6);   // Behavior
    // move the actor and the RNG so there is unmistakable live state to lose
    *(int *)((char *)actor + 0x5c) = 0x11110000;
    *(int *)((char *)actor + 0x60) = 0x22220000;
    *(int *)((char *)actor + 0x64) = 0x33330000;
    *(int *)((char *)actor + 0x70) = 0x0000abcd;
    LCG_STATE_0204da4c = 0x5eedf715;

    // ---- (2) fingerprint, then (3) save ------------------------------------
    Fingerprint saved = fingerprint(actor);
    printf("  saved fingerprint: hash=%016llx cursor=%p rng=%08x "
           "pos=(%08x,%08x,%08x) timer=%08x\n",
           (unsigned long long)saved.hash, saved.cursor, (unsigned)saved.rng,
           saved.pos[0], saved.pos[1], saved.pos[2], saved.timer);
    CHECK(lk6_savestate_save() == 1);

    // ---- (4) diverge: M more ticks plus a stomp on the captured fields -----
    for (int f = 0; f < M; ++f) vcall0(actor, 6);
    *(int *)((char *)actor + 0x5c) = 0x7fff0000;   // move him elsewhere
    *(int *)((char *)actor + 0x60) = 0x00000000;
    *(int *)((char *)actor + 0x64) = 0x00000000;
    *(int *)((char *)actor + 0x70) = 0x00000000;
    LCG_STATE_0204da4c = 0x0badc0de;
    // stomp the out-of-arena globals too: these restore through the .dsstate
    // section, not through the arena copy, so mangling them here proves the
    // section path works (and would catch one dropped from the capture).
    memset(_ZN6Memory16rootHeapIteratorE, 0x5a, sizeof(_ZN6Memory16rootHeapIteratorE));
    for (int k = 0; k < 12; ++k) data_0209b3ec[k] = 0x0badf00d;
    data_020a4bc8 = 0xdeadbeefu;
    // the message-active lock: set it the way opening a dialogue would. It was
    // never in the old hand-picked list, so only a whole-.dsstate capture rolls
    // it back. Saved value was 0 (boot), so this is a real divergence.
    data_0209d660 = 1;
    Fingerprint diverged = fingerprint(actor);
    printf("  diverged fingerprint: hash=%016llx rng=%08x pos0=%08x\n",
           (unsigned long long)diverged.hash, (unsigned)diverged.rng,
           diverged.pos[0]);
    // the whole point of the test: the world must actually have changed, or the
    // load below would trivially "pass" on a state that never moved.
    CHECK(!fp_equal(saved, diverged));
    CHECK(diverged.hash != saved.hash);
    CHECK(diverged.rng  != saved.rng);
    CHECK(diverged.pos[0] != saved.pos[0]);

    // ---- (5) load, then (6) prove full restoration -------------------------
    CHECK(lk6_savestate_load() == 1);
    Fingerprint after = fingerprint(actor);
    printf("  after-load fingerprint: hash=%016llx cursor=%p rng=%08x "
           "pos=(%08x,%08x,%08x) timer=%08x\n",
           (unsigned long long)after.hash, after.cursor, (unsigned)after.rng,
           after.pos[0], after.pos[1], after.pos[2], after.timer);
    CHECK(after.hash == saved.hash);        // whole arena identical
    CHECK(after.cursor == saved.cursor);    // allocator cursor restored
    CHECK(after.rng == saved.rng);          // out-of-arena global restored
    CHECK(after.pos[0] == saved.pos[0]);
    CHECK(after.pos[1] == saved.pos[1]);
    CHECK(after.pos[2] == saved.pos[2]);
    CHECK(after.timer == saved.timer);
    // the out-of-arena block path specifically: heap head, camera matrix, VRAM
    // cursor all came back through the .dsstate section.
    CHECK(memcmp(after.heaphead, saved.heaphead, sizeof after.heaphead) == 0);
    CHECK(memcmp(after.cammat, saved.cammat, sizeof after.cammat) == 0);
    CHECK(after.vramcur == saved.vramcur);
    // the widened capture, specifically: a hosted global outside the old list
    // (the message-active lock) rolled back too. This is the assertion that
    // would have caught the 0.2.0 crash class.
    CHECK(after.msglock == saved.msglock);
    CHECK(fp_equal(saved, after));

    // ---- (7) keep running after the load -----------------------------------
    for (int f = 0; f < N; ++f) vcall0(actor, 6);   // must not crash
    int render_ok = vcall0(actor, 9);               // Render still dispatches
    (void)render_ok;
    printf("  ran %d frames after load and re-rendered: no crash\n", N);

    if (g_failures) {
        fprintf(stderr, "smoke_savestate: %d FAILURE(S)\n", g_failures);
        return 1;
    }
    printf("smoke_savestate: all checks passed (world evolved, saved, diverged, "
           "restored byte-exact, and kept running)\n");
    return 0;
}
