// Gate 31: the level HANDOFF -- choosing a level, leaving the one you are in,
// and the teardown in between.
//
// ---- what was already here, and what was missing ---------------------------
//
// The whole game-side handoff already links. LoadLevel, LoadLevelNoReturn,
// SetNextLevel, ExitLevel, KillPlayer, HitDeathPlane and
// StartExitCharacterWipe are all matched src in slice_gate10.txt, and the
// fader wipes they end on are staged in hal/fader_wipes.cpp. Calling
// ExitLevel() in the port has therefore always "worked": it writes
//
//     data_02092110 = 1        the next level  (castle grounds)
//     data_0209f268 = 0xd      the next entrance
//
// and snaps a wipe. What was missing is the OTHER HALF. On the ROM those two
// words are read back by Stage::InitResources, which is the level boot; the
// port boots a level through port_stage_a_boot in hal/level_boot.cpp instead,
// and nothing anywhere read data_02092110. Every level change in the game
// wrote its request into a word no one was listening to.
//
// This file is the listener. It polls the request, tears the current level
// down, latches the request the same four lines Stage::InitResources latches
// it with, and boots the new level.
//
// ---- the seam the level-boot stream meets ----------------------------------
//
// A level id becomes an LVL_Overlay through port_level_overlay(). The ROM's
// own answer is one array index:
//
//     data_02092208[level]     -> the LVL_Overlay inside the level overlay
//     data_020758c8[level]     -> which overlay that is (it is level + 8)
//
// Both are arm9 .data and both are dumped by tools/romdata.py, so the table
// is Nintendo's rather than a guess. What a host cannot use directly is the
// VALUE: every level overlay is linked over the same 0x0211xxxx window (only
// one is resident at a time on the DS), so data_02092208[2] and
// data_02092208[1] are addresses in two different files that happen to
// overlap. Turning one into host bytes is a mount, and a mount is per-level
// work: ovdata --whole for that overlay, its relocations applied, its own
// static initialisers run.
//
// So the seam is a REGISTRY, not a table lookup:
//
//     port_level_mount_register(level, fn)   the mount stream registers
//     port_level_overlay(level)              the handoff calls
//
// `fn` returns the host address of that level's LVL_Overlay and must be
// idempotent -- the handoff calls it on every entry to the level, and a
// second call must not re-apply the overlay's relocations. hal/level_boot.cpp
// registers every row of its own level table through port_level_mounts_install,
// each against a thunk over a per-level mount cache, which is what gives that
// guarantee even when a session goes 1 -> 6 -> 1. A level not in that table
// answers "not mounted in this build" and the handoff declines the change with
// a message instead of booting into null.
//
// port_level_ds_overlay() is there so a mount can check itself: it returns
// the DS address the ROM's own table holds for that level, which is what the
// mount's LVL_Overlay symbol has to be.
//
// ---- teardown --------------------------------------------------------------
//
// The teardown is the ROM's per-actor path, driven from the port's side.
// ActorBase::MarkForDestruction on every live actor, then the cleanup phase
// pumped until the lists drain: each actor's own CleanupResources runs, its
// dedicated heap is destroyed, its slot-16 destructor runs and the object
// goes back to the game heap (port/unmatched/ActorBase_AfterCleanupResources
// .cpp is that path, hosted). Nothing here frees an actor by hand.
//
// What this file DOES own is the host storage the game's teardown cannot know
// about, because the game never allocated it: the per-level statics in
// hal/level_boot.cpp (the LoadFile handle table, the entrance record cache)
// and the engine globals the port's boot seats by hand. port_level_reset_host
// is that half, and hal/level_boot.cpp implements it next to the statics it
// clears.
//
// NOT torn down, deliberately: the Stage. On the ROM the Stage actor is
// respawned per level (scene 3), because on the ROM the level boot IS
// Stage::InitResources. The port's boot is port_stage_a_boot, a hand-written
// subset of it, and Stage::CleanupResources is the symmetric undo of the
// FULL one -- it releases twelve SharedFilePtrs that only Stage::InitResources
// fills, unloads archives the port never loaded, and calls
// Sound::ResetPlayerVoiceGroup. Running it against the port's boot would tear
// down things that were never built. So the Stage object persists and its two
// level-owned sub-objects are re-seated in place, which is what
// port_level_stage_reseat does. When the boot becomes Stage::InitResources
// for real, this is the piece that goes away.
//
// WHAT THE HANDOFF NOW GETS RIGHT that it did not: the boot it drives mounts
// the level it warped TO. The latch writes the new level into data_0209f2f8;
// this file hands the same id to port_level_set_target before port_stage_a_boot,
// so the boot's overlay mount and its sound-row seat resolve to the warped-to
// level. Before that the boot resolved through the SM64DS_LEVEL-cached desc and
// re-booted the castle grounds -- the census came back the castle's, doubled,
// and this line said "level 1 up" after a select of level 6.
#include <cstdio>
#include <cstdlib>
#include <cstring>
/* IsBadReadPtr, for the two places here that have to read an actor whose
   pointer may already be torn (port_level_name_survivors on the decline path
   and the scene-tree drop). It is the same guard port_q_is_frozen and
   port_q_actor_id in port/unmatched/func_02043fdc_hostcopy.cpp have carried since
   playlog 041729, and this file needs it for the same reason. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "hal/comms_seam.h"   /* run mg16 lane MP3: port::vs_player_count() */

extern "C" {

/* ---- the ROM's request words ---------------------------------------------
   data_02092110 is arm9 .data and the ROM ships it as -1; romdata.py carries
   it, so the port starts with "no level pending" for the same reason the DS
   does. */
extern signed char data_02092110;    /* next level, -1 = nothing pending */
extern unsigned char data_0209f268;  /* next entrance */
extern unsigned char data_0209f1f0;  /* next star */
extern signed char data_0209f2f8;    /* current level */
extern int data_0209f264[];          /* current entrance */
extern int data_0209f220[];          /* current star */
extern unsigned char data_0209f26c;  /* why we are entering (1 fresh, 2 death) */

/* the ROM tables, from romdata.py */
extern unsigned char data_02092208[];   /* level -> LVL_Overlay (DS address) */
extern unsigned char data_020758c8[];   /* level -> overlay id */

/* the engine pieces the change drives */
void _ZN9ActorBase18MarkForDestructionEv(void *self);
void port_actor_tick(void);
void port_quarantine_reset(void);   /* port/unmatched/func_02043fdc_hostcopy.cpp: clear
                                       the per-actor fault freeze set */
int  port_quarantine_frozen_count(void);  /* same TU: how many actors
                                       the instance freeze set holds, so the
                                       teardown log can say what it reaped */
int  port_quarantine_is_frozen(void *actor);  /* same TU: is this actor frozen?
                                       teardown excludes a frozen actor from the
                                       live census it pumps -- a frozen actor is
                                       never dispatched, so it never runs its own
                                       cleanup and never unlinks, and a census
                                       that counts it can never converge.
                                       It answers for BOTH freeze legs (the
                                       instance set and the class latch); see the
                                       note on its definition for why a
                                       teardown-side predicate that knew only the
                                       instance leg reopened the soft-lock. */
/* the ROM's own list primitives (src/func_0203b3c0.c, src/func_0203b394.c):
   scene-tree unlink, and the scene-tree pre-order successor. The drop at the
   tail of the teardown needs both -- see the scene-tree pass there. */
int   func_0203b3c0(void *list, void *node);
void *func_0203b394(void *node);
void port_actor_scene_pass(void);
void *port_stage_object(void);
void *port_stage_a_boot(void *mc, int spawn);
void port_level_reset_host(void);        /* hal/level_boot.cpp (also captures the
                                            outgoing level's orphaned KCL image --
                                            the ~108KB re-entry leak -- for
                                            port_level_stage_reseat to free once
                                            ResetMeshColliders has cleared the
                                            registry that points at it) */
void port_level_set_target(int level);   /* hal/level_boot.cpp: which level the
                                            next port_stage_a_boot mounts */
void CleanCommonModelDataArr(void);
void port_model_vram_reset(void);   /* hal/model_host.cpp */
void sd_sound_level_reap(void);     /* hal/sdat/consumer.cpp: the ROM's
                                       Scene::BeforeCleanupResources reap */
int  port_course_loop_live(void);
const char *port_actor_class_name(unsigned id);   /* hal/star_flow.cpp: live loop handles */
void port_level_stage_reseat(void *stage);
/* hal/level_boot.cpp: the [lvl-perf] level-entry spans. This file owns span
   0 (teardown, everything between the change firing and the boot call) and
   the emit on the warp path; the direct boot emits from walk_window. */
double port_lvlperf_now(void);
void port_lvlperf_note(int span, double ms);
void port_lvlperf_emit(void);
unsigned _ZN22ExpandingHeapAllocator10MemoryLeftEv(void *self);
extern void *data_020a0eac;              /* Memory::gameHeapPtr */

/* the three actor-pointer cells Stage::CleanupResources clears, hosted at the
   teardown below because the port never runs Stage::CleanupResources */
extern void *data_0209f394[];        /* the local players, [0] is ours */
extern void *data_0209f318;          /* the Camera */
extern unsigned char data_0209f2c4;  /* the input/VS-timer suppress flag */

extern int data_020a4b6c[];   /* scene tree     {head, cb, 0} */
extern int data_020a4b78[];   /* behaviour list {head, tail, cb, 0} */
extern int data_020a4b88[];   /* pending list */
extern int data_020a4b98[];   /* render list */
extern int data_020a4ba8[];   /* cleanup list */

}  /* extern "C" */

// ---- deleting a model-family object without the ambiguous slot -----------
//
// THE PROBLEM. mwcc emits two destructor slots (D1 at 0, D0 at 1) where MSVC
// emits one, so every synthetic vtable array in this file is one slot short of
// the ROM's from DoSetFile onward. A TU that dispatches through a LOCAL SHADOW
// CLASS counts ROM slots; a TU compiled against Model.h counts MSVC's. Render
// happens to be servable both ways (slot 5 is double-filled above) and the
// rest are not: ROM slot 1, the DELETING destructor, is MSVC slot 1, DoSetFile.
//
// Two matched TUs delete a model that way -- Player::CleanupResources and
// daDoor_c::CleanupResources, both of them `obj->v1()` on a two-virtual shadow
// -- and both landed on Model::DoSetFile with no arguments and faulted inside
// Model::AddToCommonModelDataArr. Neither was reachable until the port had a
// level teardown to run them (gate 31).
//
// THE FIX is to spell the delete rather than dispatch it, and to pick the body
// by the object's OWN vtable pointer rather than by what the call site thinks
// it holds. The three model classes are distinguishable that way and nothing
// else is guessed: an object carrying a fourth vptr aborts by name instead of
// running the wrong destructor over it.
extern "C" {
extern void *_ZTV5Model[8];
extern void *_ZTV9ModelAnim[10];
extern void *_ZTV10ModelAnim2[12];
void *_ZN5ModelD1Ev(void *self);
void *_ZN9ModelAnimD1Ev(void *self);
void *_ZN10ModelAnim2D1Ev(void *self);
void _ZdlPv(void *p);

void port_model_family_delete(void *obj)
{
    if (!obj)
        return;
    void *vt = *(void **)obj;
    if (vt == (void *)_ZTV5Model)
        _ZN5ModelD1Ev(obj);
    else if (vt == (void *)_ZTV9ModelAnim)
        _ZN9ModelAnimD1Ev(obj);
    else if (vt == (void *)_ZTV10ModelAnim2)
        _ZN10ModelAnim2D1Ev(obj);
    else {
        fprintf(stderr, "FATAL: port_model_family_delete: %p carries vtable "
                "%p, which is not Model, ModelAnim or ModelAnim2\n", obj, vt);
        abort();
    }
    _ZdlPv(obj);
}
}

// It lives here rather than beside the vtables it reads because the three
// destructors it names are in gate slices only the level-carrying targets
// link; in hal/cxxname_bridge.cpp it broke smoke_actor's link.

/* ---- the mount registry ---------------------------------------------------
   Fifty-two levels; the array is the ROM's own count so a registration for a
   level the ROM does not have is caught rather than stored. */
enum { PORT_LEVEL_COUNT = 52 };

typedef void *(*PortLevelMount)(void);

static PortLevelMount g_mount[PORT_LEVEL_COUNT];

extern "C" int port_level_mount_register(int level, PortLevelMount fn)
{
    if (level < 0 || level >= PORT_LEVEL_COUNT || !fn) {
        std::fprintf(stderr, "  [lvl] REGISTER REFUSED: level %d is outside "
                     "the ROM's 0..%d\n", level, PORT_LEVEL_COUNT - 1);
        return 0;
    }
    g_mount[level] = fn;
    return 1;
}

extern "C" unsigned port_level_ds_overlay(int level)
{
    if (level < 0 || level >= PORT_LEVEL_COUNT)
        return 0;
    return *(const unsigned *)(data_02092208 + level * 4);
}

extern "C" int port_level_overlay_id(int level)
{
    if (level < 0 || level >= PORT_LEVEL_COUNT)
        return -1;
    return *(const int *)(data_020758c8 + level * 4);
}

extern "C" void *port_level_overlay(int level)
{
    if (level < 0 || level >= PORT_LEVEL_COUNT || !g_mount[level])
        return 0;
    return g_mount[level]();
}

extern "C" int port_level_is_mounted(int level)
{
    return level >= 0 && level < PORT_LEVEL_COUNT && g_mount[level] != 0;
}

/* ---- what the game is asking for ------------------------------------------ */

extern "C" int port_level_change_pending(void)
{
    return data_02092110 >= 0;
}

/* ---- teardown -------------------------------------------------------------
   Two passes matter and the loop bounds both. Marking an actor does not
   remove it: the phase-1 scene pass (func_02043880) is what moves a marked
   actor onto the cleanup list, and the phase-4 cleanup walk is what runs its
   Process. An actor whose own CleanupResources spawns or marks something --
   the castle grounds' trees mark their cylinder owners -- needs another
   round, so this alternates the two until the lists are empty or the budget
   runs out. Sixteen is far past what the castle grounds needs (it settles in
   three) and small enough that a teardown that cannot converge says so
   instead of hanging. */
static int port_level_live_count(void)
{
    int n = 0;
    void *stage = port_stage_object();
    /* A quarantine-frozen actor is never dispatched, so it can never run its
       own CleanupResources and never unlinks itself. Counting it here would
       make the teardown loop never converge (the "1 actor still live after 16
       rounds" soft-lock a persistent frozen actor produced). Exclude it: it is
       reaped by name by the dangling-node drop at the tail of the teardown once
       the real actors are gone. */
    for (int *node = (int *)(size_t)data_020a4b78[0]; node && n < 8192;
         node = (int *)(size_t)node[1])
        if (node[2] && (void *)(size_t)node[2] != stage &&
            !port_quarantine_is_frozen((void *)(size_t)node[2]))
            ++n;
    for (int *node = (int *)(size_t)data_020a4b88[0]; node && n < 8192;
         node = (int *)(size_t)node[1])
        if (node[2] && (void *)(size_t)node[2] != stage &&
            !port_quarantine_is_frozen((void *)(size_t)node[2]))
            ++n;
    return n;
}

/* The census the level cycle reports: how many actors are alive that are not
   the Stage. Two runs of the same cycle have to produce the same number, and
   an actor that survives a teardown shows up here as the count going up. */
extern "C" int port_actor_live_count(void) { return port_level_live_count(); }

static int port_level_mark_all(void)
{
    int n = 0;
    void *stage = port_stage_object();
    /* Snapshot first. MarkForDestruction runs the actor's OnPendingDestroy,
       and an OnPendingDestroy is allowed to mark other actors -- which
       relinks nodes under the walk. */
    static void *victim[4096];
    int v = 0;
    /* Skip frozen actors: MarkForDestruction runs the actor's OnPendingDestroy
       synchronously, which for a frozen actor is exactly the code that faulted
       and was quarantined -- running it here would re-enter the faulting path
       (and OnPendingDestroy is NOT inside the walker's __try). The frozen node
       is reaped by name by the dangling-node drop after convergence. */
    for (int *node = (int *)(size_t)data_020a4b78[0]; node && v < 4096;
         node = (int *)(size_t)node[1])
        if (node[2] && (void *)(size_t)node[2] != stage &&
            !port_quarantine_is_frozen((void *)(size_t)node[2]))
            victim[v++] = (void *)(size_t)node[2];
    for (int *node = (int *)(size_t)data_020a4b88[0]; node && v < 4096;
         node = (int *)(size_t)node[1])
        if (node[2] && (void *)(size_t)node[2] != stage &&
            !port_quarantine_is_frozen((void *)(size_t)node[2]))
            victim[v++] = (void *)(size_t)node[2];
    for (int i = 0; i < v; ++i) {
        char *o = (char *)victim[i];
        if (*(unsigned char *)(o + 0xf))   /* already marked */
            continue;
        _ZN9ActorBase18MarkForDestructionEv(o);
        ++n;
    }
    return n;
}

/* Name every non-stage actor still on the two pumped lists, with its class and
   whether the quarantine net is holding it. This runs only on the path that
   declines a level change, and it is the difference between a player report
   that says "TEARDOWN DID NOT CONVERGE: 1 actors still live" and one that says
   WHICH actor. A decline strands the player -- the fader has already wiped and
   the leaving state is latched, so the level never changes and the player reads
   it as "can't move after collecting a star" -- so the next such report needs
   to name the blocker rather than leave it to be guessed at. Changes nothing.

   IT MUST NOT DEREFERENCE THE ACTOR UNGUARDED, and the reason is that the most
   likely input to this function is exactly the input that would fault on it.
   A node whose owner word has been stomped is counted by port_level_live_count
   BY DESIGN: port_q_is_frozen reads an unreadable pointer as not-frozen, so a
   torn owner keeps the census above zero and lands us here. That is the case
   this diagnostic exists to name. Reading a+0xc and a+0xf without a guard would
   therefore fault on its own headline case, outside any __try, and turn a
   player soft-lock -- bad, but survivable and reported -- into a hard crash, on
   a path players demonstrably already reach. port_q_actor_id and
   port_q_is_frozen have carried this guard since playlog 041729 (a scene node
   with owner 0x62980 that took the process down through exactly this kind of
   unguarded read); this is the same guard for the same reason.

   AN UNREADABLE ID IS NOT A REASON TO SKIP THE ENTRY, it is the single most
   valuable thing this function could report: it says the survivor is not merely
   stuck but corrupt, which points at a stray write rather than at a cleanup
   that never ran. So the line degrades to the address plus "id UNREADABLE"
   instead of vanishing. */
static void port_level_name_survivors(void)
{
    void *stage = port_stage_object();
    struct { const char *name; int *list; } lists[2] = {
        {"behaviour", data_020a4b78}, {"pending", data_020a4b88}};
    for (int i = 0; i < 2; ++i) {
        int guard = 0;
        for (int *node = (int *)(size_t)lists[i].list[0];
             node && guard < 8192; node = (int *)(size_t)node[1], ++guard) {
            char *a = (char *)(size_t)node[2];
            if (!a || (void *)a == stage)
                continue;
            /* the freeze check is pointer identity first and is itself guarded,
               so it is safe on a torn pointer */
            const int frozen = port_quarantine_is_frozen(a);
            if (IsBadReadPtr(a, 0x10)) {
                std::fprintf(stderr, "  [lvl]   STILL LIVE: actor %p id "
                             "UNREADABLE (the owner word is torn -- a stray "
                             "write, not a cleanup that never ran) on the %s "
                             "list%s\n", (void *)a, lists[i].name,
                             frozen ? " -- QUARANTINE-FROZEN" : "");
                continue;
            }
            {
                const unsigned id = *(unsigned short *)(a + 0xc);
                std::fprintf(stderr, "  [lvl]   STILL LIVE: actor %p id 0x%x "
                             "(%s) on the %s list, marked %d%s\n", (void *)a,
                             id, id < 0x400 ? port_actor_class_name(id) : "?",
                             lists[i].name, (int)*(unsigned char *)(a + 0xf),
                             frozen ? " -- QUARANTINE-FROZEN" : "");
            }
        }
    }
}

/* Unlink every quarantine-frozen actor from ALL FIVE structures the engine
   walks -- the four processing lists and the scene tree -- and answer how many
   frozen nodes are still linked afterwards. Zero means the freeze set is now
   safe to clear.

   THIS IS WHAT MAKES THE RESET SAFE ON A PATH THAT KEEPS THE LEVEL. Clearing
   the freeze set un-freezes the actor, and an un-frozen actor is dispatched
   again -- which, in a level whose other actors the teardown rounds have
   already destroyed, is a walk over wreckage. The freeze is the only thing
   holding it back only for as long as it is still LINKED. Once it is out of
   every list and out of the tree, nothing can reach it to dispatch it, the
   object is simply leaked, and the freeze set is holding nothing but stale
   pointers. So: unlink first, then clear. That ordering is the whole reason the
   clear can move upstream of the decline.

   It answers a COUNT rather than void, and the count is checked, because
   func_0203b3c0 refuses a scene node that still has children. If one frozen
   node cannot come out, clearing the set would hand a dead object back to the
   scene walk -- exactly the failure this exists to prevent -- so the caller
   keeps the freeze in that case and says so out loud. Failing back to the old
   permanent freeze is bad; failing forward into a per-frame walk over a dead
   actor is worse. */
static int port_level_drop_frozen(void)
{
    /* NOTHING FROZEN IS THE OVERWHELMING COMMON CASE, and it gets to cost
       nothing. Without this the walk below relinks every node on all four
       lists on EVERY level change just to discover it has nothing to drop.
       The relink is order-preserving and a no-op in effect, but "in effect" is
       not the same as "does not write", and a level change that has never seen
       a quarantine should not have this pass touching the engine's lists at
       all. The class latch cannot be set without the instance set being
       non-empty (every quarantine records its instance), so a count of zero
       really does mean nothing is frozen by either leg. */
    if (port_quarantine_frozen_count() == 0)
        return 0;
    struct { const char *name; int *list; } lists[4] = {
        {"behaviour", data_020a4b78}, {"pending", data_020a4b88},
        {"render", data_020a4b98}, {"cleanup", data_020a4ba8}};
    int stuck = 0;
    for (int i = 0; i < 4; ++i) {
        int *keep_head = 0, *keep_tail = 0;
        int dropped = 0, guard = 0;
        int *n = (int *)(size_t)lists[i].list[0];
        for (; n && guard < 8192; ++guard) {
            int *next = (int *)(size_t)n[1];
            char *a = (char *)(size_t)n[2];
            /* keep everything that is not frozen, the stage and a null owner
               included: the generic dangling-node drop further down owns those
               and it only runs once the rounds have converged. This pass is
               allowed to run on a level that is going to STAND, so it must
               touch nothing but the frozen. */
            if (!a || !port_quarantine_is_frozen(a)) {
                n[0] = (int)(size_t)keep_tail;
                n[1] = 0;
                if (keep_tail) keep_tail[1] = (int)(size_t)n;
                else keep_head = n;
                keep_tail = n;
                n = next;
                continue;
            }
            if (IsBadReadPtr(a, 0x10))
                std::fprintf(stderr, "  [lvl] %s list: REAPED frozen node %p "
                             "(actor %p id UNREADABLE) -- quarantined, so its "
                             "cleanup never ran\n", lists[i].name, (void *)n,
                             (void *)a);
            else {
                const unsigned id = *(unsigned short *)(a + 0xc);
                std::fprintf(stderr, "  [lvl] %s list: REAPED frozen node %p "
                             "(actor %p id 0x%x %s) -- quarantined, so its "
                             "cleanup never ran\n", lists[i].name, (void *)n,
                             (void *)a, id,
                             id < 0x400 ? port_actor_class_name(id) : "?");
            }
            n[0] = 0; n[1] = 0;      /* the ROM unlink zeroes both; so do we */
            ++dropped;
            n = next;
        }
        if (dropped) {
            /* head AND tail: every insert path links through list[1] */
            lists[i].list[0] = (int)(size_t)keep_head;
            lists[i].list[1] = (int)(size_t)keep_tail;
        }
    }
    /* the scene tree, the fifth structure. Snapshot then unlink in REVERSE
       pre-order, for the reason spelled out at the success-path drop below. */
    {
        static void *snode[1024];
        int sn = 0, guard = 0;
        int *n = (int *)(size_t)data_020a4b6c[0];
        for (; n && guard < 8192; n = (int *)func_0203b394(n), ++guard) {
            char *a = (char *)(size_t)n[4];
            if (!a || !port_quarantine_is_frozen(a))
                continue;
            if (sn == (int)(sizeof snode / sizeof snode[0])) {
                std::fprintf(stderr, "  [lvl] scene tree: FROZEN WALK DID NOT "
                             "FINISH (cap %d) -- the freeze set will be kept\n",
                             sn);
                ++stuck;
                break;
            }
            snode[sn++] = n;
        }
        for (int i = sn - 1; i >= 0; --i) {
            int *nd = (int *)snode[i];
            if (func_0203b3c0(data_020a4b6c, nd)) {
                std::fprintf(stderr, "  [lvl] scene tree: REAPED frozen node "
                             "%p (actor %p)\n", (void *)nd,
                             (void *)(size_t)nd[4]);
                continue;
            }
            std::fprintf(stderr, "  [lvl] scene tree: COULD NOT DROP frozen "
                         "node %p (actor %p) -- it still has children\n",
                         (void *)nd, (void *)(size_t)nd[4]);
            ++stuck;
        }
    }
    return stuck;
}

/* SM64DS_TEST_NOCONVERGE=<n>: force the next <n> teardowns down the DECLINE
   branch whatever the census says. TEST BUILDS ONLY (PORT_TEST_HOOKS), and
   compiled out of everything else -- it collapses to a `return 0` the optimiser
   deletes.

   It exists because the fix makes the branch it tests UNREACHABLE from a
   quarantine, which is the point of the fix and also means the decline path's
   own repair -- reaping the frozen actors and clearing the freeze set before
   returning -- would otherwise never execute in any proof run. A path that
   cannot be reached cannot be shown to work, and "we reasoned about it" is what
   the previous attempt offered. This reaches it. */
#ifndef PORT_TEST_HOOKS
#define PORT_TEST_HOOKS 0
#endif
#if PORT_TEST_HOOKS
static int port_test_noconverge(void)
{
    static int n = -1;
    if (n < 0) {
        const char *e = std::getenv("SM64DS_TEST_NOCONVERGE");
        n = e ? std::atoi(e) : 0;
        if (n > 0)
            std::fprintf(stderr, "  [lvl] [testnc] ARMED: the next %d "
                         "teardown(s) will be forced to DECLINE. TEST BUILD.\n",
                         n);
    }
    if (n > 0) { --n; return 1; }
    return 0;
}
#else
static int port_test_noconverge(void) { return 0; }
#endif

extern "C" int port_level_teardown(void)
{
    const int trace = std::getenv("SM64DS_TRACE_LEVEL") != 0;
    int rounds = 0;
    for (; rounds < 16; ++rounds) {
        int marked = port_level_mark_all();
        /* phase 1 moves the marked onto the cleanup list, phase 4 runs it */
        port_actor_scene_pass();
        port_actor_tick();
        port_actor_scene_pass();
        int left = port_level_live_count();
        if (trace)
            std::printf("  [lvl] teardown round %d: marked %d, %d left\n",
                        rounds, marked, left);
        if (!left)
            break;
    }
    /* ---- REAP THE FROZEN, THEN CLEAR THE FREEZE SET, ON BOTH PATHS ---------

       This sits ABOVE the verdict on purpose, and that position is half the
       fix. The reset used to be at the very tail, so the `return 0` below
       skipped it: once a teardown declined, the freeze set was never cleared
       again for the rest of the session, the frozen actor kept blocking every
       later census, and every later level change was refused. Permanent, from
       one caught fault. Moving the clear upstream of the decline is what makes
       a declined change survivable rather than terminal.

       It is only sound because the drop runs FIRST. Clearing the set un-freezes
       the actor, and an un-frozen actor gets dispatched again -- in a level
       whose other actors these rounds have already destroyed, that is a walk
       over wreckage, and it is the reason the clear was left on the success
       path in the first place. port_level_drop_frozen answers that by taking
       the actor out of all five structures the engine walks before the set is
       touched: after it, nothing can reach the object to dispatch it, so
       un-freezing it cannot resurrect anything. If a node will not come out,
       the count says so and the freeze is KEPT -- failing back to the old
       permanent freeze is bad, dispatching a dead actor every frame is worse.

       The other objection to clearing here was the class latch, the rate
       limiter that stops the second fault of a class becoming a fault every
       frame. It is cleared too, and the flood it guarded against does not
       follow: the frozen instances are unlinked, so they cannot fault again,
       and any survivor that faults gets frozen on its first fault, after which
       the filter writes no second dump for it. The bound is one dump per actor,
       which is the bound a freshly booted level already has.

       port_stage_a_boot also resets on the load side, which covers a level that
       BOOTS. It cannot cover a level that stands, which is precisely the state
       a decline leaves behind, and that is the state that was poisoning
       sessions. */
    {
        const int stuck = port_level_drop_frozen();
        const int held = port_quarantine_frozen_count();
        if (stuck) {
            std::fprintf(stderr, "  [lvl] quarantine: %d frozen node(s) could "
                         "not be unlinked -- KEEPING the freeze set (%d actor%s)"
                         " rather than handing a dead actor back to the walk\n",
                         stuck, held, held == 1 ? "" : "s");
        } else if (held) {
            std::fprintf(stderr, "  [lvl] quarantine: %d frozen actor%s reaped "
                         "from every list and the scene tree; CLEARING the "
                         "freeze set\n", held, held == 1 ? "" : "s");
            port_quarantine_reset();
        } else {
            port_quarantine_reset();   /* no-op; keeps the class latch honest */
        }
    }
    int left = port_level_live_count();
    if (port_test_noconverge() && !left)
        left = 1;                      /* test builds only; see the note above */
    if (left) {
        std::fprintf(stderr, "  [lvl] TEARDOWN DID NOT CONVERGE: %d actors "
                     "still live after %d rounds -- the change will be DECLINED "
                     "and the player is stranded in the leaving state\n",
                     left, rounds);
        port_level_name_survivors();
        /* The freeze set has already been reaped and cleared above, so this
           return no longer poisons the session: the next level change starts
           from a clean net and can converge.

           Anything reaching this branch now is a second, non-quarantine cause
           -- a frozen actor cannot block the census any more -- and the
           survivor line just above is what names it.

           ONE HOLE, WRITTEN DOWN RATHER THAN FIXED. Neither freeze leg covers
           everything. The instance set caps at PORT_Q_MAX (256) and the class
           latch table only spans ids below PORT_Q_IDS (512), so an actor with
           an id at or above 512, or an unreadable id, that faults after 256
           instances are already frozen in this level is held by neither leg:
           port_quarantine_is_frozen answers no, the census counts it, and it
           blocks convergence exactly the way things did before the reap. It
           needs 256 frozen instances in a single level to reach, and it fails
           back to pre-fix behaviour rather than to something worse, so it is
           not worth code today -- but the survivor line above is what would
           expose it, and this is the note that says what to suspect. */
        return 0;
    }
    /* The four processing lists have to be genuinely empty, not just free of
       actors this walk could see. A stale head is a dangling node the next
       level's phase walk would step through.

       Every non-stage actor is destroyed by this point (the rounds ran to
       zero), so a non-stage node here is DANGLING -- a cleanup that never
       unlinked. Declining the change over it (the first reading) was
       unsound: the level's actors are already gone, so "the level stands"
       handed the walk a world of freed objects and it faulted on the next
       tick (playlog 001951, a stale cleanup node after the garden door).
       Repair instead: keep the stage links, drop the dangling nodes BY NAME
       so the leak is loud, and proceed with the boot. */
    struct { const char *name; int *list; } lists[4] = {
        {"behaviour", data_020a4b78}, {"pending", data_020a4b88},
        {"render", data_020a4b98}, {"cleanup", data_020a4ba8}};
    void *stage = port_stage_object();
    for (int i = 0; i < 4; ++i) {
        int *keep_head = 0, *keep_tail = 0;
        int dropped = 0;
        for (int *n = (int *)(size_t)lists[i].list[0]; n;) {
            int *next = (int *)(size_t)n[1];
            if ((void *)(size_t)n[2] == stage) {
                /* relink: prev then next, both directions */
                n[0] = (int)(size_t)keep_tail;
                n[1] = 0;
                if (keep_tail) keep_tail[1] = (int)(size_t)n;
                else keep_head = n;
                keep_tail = n;
                n = next;
                continue;
            }
            unsigned id = 0xffff;
            char *a = (char *)(size_t)n[2];
            if (a) id = *(unsigned short *)(a + 0xc);
            std::fprintf(stderr, "  [lvl] %s list: DROPPED dangling node %p "
                         "(actor %p id 0x%x %s) -- its cleanup never "
                         "unlinked\n", lists[i].name, (void *)n, (void *)a,
                         id, id < 0x400 ? port_actor_class_name(id) : "?");
            /* scrub the dropped node's own links (the ROM unlink
               func_0203b27c always zeroes both) so no stale interior
               pointer into a freed object can ever be walked */
            n[0] = 0; n[1] = 0;
            ++dropped;
            n = next;
        }
        if (dropped) {
            /* Head AND TAIL. The first repair rewrote only list[0]; every
               insert path (func_0203b244/func_0203b2ec via func_0204405c)
               links through list[1], so a dropped former tail left the
               next level appending through a freed actor's node -- the BoB
               render-walk use-after-free of playlog 002712. */
            lists[i].list[0] = (int)(size_t)keep_head;
            lists[i].list[1] = (int)(size_t)keep_tail;
        }
    }
    /* THE SCENE TREE, which is the FIFTH structure an actor is linked into and
       the one the four-list drop above cannot see.
       ActorBase::AfterCleanupResources unlinks TWO things, not one: the
       cleanup-list node at self+0x28 (func_0203b27c over data_020a4ba8) AND the
       SceneNode at self+0x14 (func_0203b3c0 over data_020a4b6c). A frozen actor
       runs neither. Dropping only its four processing-list nodes left its
       SceneNode in the tree, so func_020441cc walked into the next level still
       dispatching that actor's scene phase off a dead object -- every frame,
       forever, because port_quarantine_reset below had meanwhile cleared the
       freeze set that was the only thing skipping it. Measured with
       SM64DS_SCENE_CANARY=1 across a 1 -> 5 warp: 65 nodes on the control, 66
       with a frozen actor.
       Unlink with the ROM's own primitive rather than by hand, and REVERSE the
       pre-order snapshot before unlinking: func_0203b3c0 refuses a node that
       still has children (n->f4 != 0), and pre-order lists a parent before its
       children, so walking the snapshot backwards retires every child before
       its parent. Snapshot first for the same reason the victim list above does
       -- the unlink rewrites the sibling links under the walk.

       The owner read is GUARDED for the same reason the survivor diagnostic
       above is: an actor reached through a dangling node is exactly the actor
       whose owner word is most likely to be torn, and this one runs on the
       SUCCESS path, so a fault here would crash a level change that was
       otherwise about to work. NOTE, not fixed here: the four-list drop above
       and port_level_mark_all read the same +0xc and +0xf unguarded. Those are
       pre-existing and are left alone deliberately -- widening the change to
       chase them would put an unrelated edit in a commit whose repro is about
       the scene tree. They are worth their own pass.

       And the snapshot SAYS SO WHEN IT TRUNCATES. A silent cap reads as "we
       walked the whole tree" when we did not, which is the worst possible thing
       for a diagnostic whose entire job is to prove the tree came out empty --
       the canary count would come back short and nothing would explain why.
       1024 is far past the 83 nodes the castle grounds carries, so hitting it
       means something else is wrong and the line is the first thing that would
       say so. */
    {
        static void *snode[1024];
        int sn = 0, guard = 0, truncated = 0;
        int *n = (int *)(size_t)data_020a4b6c[0];
        for (; n && guard < 8192; n = (int *)func_0203b394(n), ++guard) {
            if ((void *)(size_t)n[4] == stage)
                continue;
            if (sn == (int)(sizeof snode / sizeof snode[0])) { truncated = 1; break; }
            snode[sn++] = n;
        }
        if (truncated || guard >= 8192)
            std::fprintf(stderr, "  [lvl] scene tree: WALK DID NOT FINISH "
                         "(%d nodes snapshotted, cap %d, steps %d) -- nodes "
                         "past this point are NOT dropped and will survive into "
                         "the next level\n", sn,
                         (int)(sizeof snode / sizeof snode[0]), guard);
        for (int i = sn - 1; i >= 0; --i) {
            int *nd = (int *)snode[i];
            char *a = (char *)(size_t)nd[4];
            const int readable = a && !IsBadReadPtr(a, 0x10);
            const int ok = func_0203b3c0(data_020a4b6c, nd);
            const char *verb = ok ? "DROPPED" : "COULD NOT DROP";
            if (!readable)
                std::fprintf(stderr, "  [lvl] scene tree: %s dangling node %p "
                             "(actor %p id UNREADABLE) -- its cleanup never "
                             "unlinked\n", verb, (void *)nd, (void *)a);
            else {
                const unsigned id = *(unsigned short *)(a + 0xc);
                std::fprintf(stderr, "  [lvl] scene tree: %s dangling node %p "
                             "(actor %p id 0x%x %s) -- its cleanup never "
                             "unlinked\n", verb, (void *)nd, (void *)a, id,
                             id < 0x400 ? port_actor_class_name(id) : "?");
            }
        }
    }

    /* THE STAGE TEARDOWN'S OWN POINTER CLEARS, and they are the ROM's lines,
       not the port's invention. Stage::CleanupResources
       (src/_ZN5Stage16CleanupResourcesEv.cpp) ends its actor half with

           for (k = 0; k < 4; k++) data_0209f394[k] = 0;   the local players
           data_0209f318 = 0;                              the Camera
           data_0209f2c4 = 0;

       three lines below the CleanCommonModelDataArr() the change already hosts
       for exactly the same reason. The port keeps the Stage alive across levels
       so Stage::CleanupResources never runs, and these three were the half of
       it nobody had picked up: after a teardown they still hold the addresses
       of actors that have just been destroyed.

       That was a use-after-free waiting for a heap layout that noticed. It
       noticed as soon as a quarantine leak shifted one: with a frozen PLAYER
       leaked, the next level's Player was allocated 0x778 lower, and the STALE
       data_0209f318 -- the freed previous Camera at +0x5c0 into that block --
       now pointed INSIDE the live Player. Camera::ChangeState's
       `self->unk_138 = state` then wrote four bytes at camera+0x138, which is
       Player+0x6f9, which is mIsMetal. Player::GetBodyModelID answers 4 for a
       metal player, func_ov002_020e5948 deliberately never seats body model 4
       on levels 2/4/5 (no metal cap indoors), and Player::SetAnim's
       `*(int *)(model + 0x60) = 0` went through a null: FAULT c0000005
       accessing 00000060 in the new level's Player spawn, 2/2 reproducible.
       Nothing about that is quarantine-specific -- any leak or allocator shift
       could have aimed the same stale pointer somewhere else -- so the fix is
       the ROM's own clear, at the seam the ROM's own sibling line already
       occupies, rather than anything about the frozen actor. LoadEntranceObjects
       refills both arrays on the next boot (it is the writer for both), so
       clearing them here is the same window the DS has.

       WHICH OF THE THREE, measured rather than argued. One binary, the three
       clears each behind their own switch, the same forced-PLAYER-freeze
       1 -> 5 warp: keeping data_0209f318 reproduces
       "FAULT c0000005 accessing 00000060", exit 139. Keeping data_0209f394 or
       data_0209f2c4 instead exits 0. So the Camera pointer is the one that was
       killing players; the other two are its siblings in the same ROM
       statement, stale for the same reason, and are cleared because leaving a
       known-dangling pointer live to wait for a different heap layout is how
       this one got found in the first place. */
    for (int k = 0; k < 4; ++k)
        data_0209f394[k] = 0;
    data_0209f318 = 0;
    data_0209f2c4 = 0;

    /* The freeze set was reaped and cleared BEFORE the verdict, not here. There
       used to be a port_quarantine_reset() at this line and it was the second
       half of the bug: it is downstream of the `return 0` above, so the one
       path that needed it most -- the declined one, which keeps the level and
       therefore keeps the frozen actor -- was the one path that never reached
       it. Nothing re-freezes between there and here (the drops above dispatch
       no actor code), so a second call would be a no-op that invited a reader
       to think this was where the clearing happened. It is not. */
    return 1;
}

/* ---- the change ----------------------------------------------------------- */

static unsigned port_level_heap_free(void)
{
    if (!data_020a0eac)
        return 0;
    /* Heap's first word is its allocator (src/_ZN4HeapC1EPvjP4Heap.c). */
    void *alloc = *(void **)((char *)data_020a0eac + 4);
    if (!alloc)
        return 0;
    return _ZN22ExpandingHeapAllocator10MemoryLeftEv(alloc);
}

extern "C" unsigned port_level_heap_free_bytes(void)
{ return port_level_heap_free(); }

/* Runs the four lines Stage::InitResources runs, in its order:
       prev      = data_0209f2f8
       current   = pending
       entrance  = next entrance
       star      = next star
   and then clears the request, which is Stage::InitResources' own last
   statement (data_02092110 = -1). Everything between those two in the ROM is
   the level boot itself. */
static void port_level_latch(void)
{
    data_0209f2f8 = data_02092110;
    data_0209f264[0] = data_0209f268;
    data_0209f220[0] = data_0209f1f0;
    data_02092110 = -1;
}

/* THE SAME LATCH, FOR A BOOT RATHER THAN A CHANGE (run lvled, title entry).
   hal/title_entry.cpp needs exactly the four lines above and none of the rest
   of port_level_change_apply: coming off the title there is no level to tear
   down, no Stage to re-seat and no host state to reset, because no level has
   run yet in this process. What there IS is a request -- StartFile's own
   LoadLevelNoReturn(1, 0, 1, 0) wrote data_02092110 and data_0209f268 before
   the title tore itself down -- and the direct boot in tests/walk_window.cpp
   would otherwise ignore it and resolve the level from SM64DS_LEVEL instead.

   IT IS EXPORTED RATHER THAN COPIED so there is one latch in the port and not
   two. A second copy is how data_0209f2f8 and data_0209f264 drift apart.

   Returns the level it latched, or -1 when nothing was pending -- which the
   caller must treat as a refusal, not as level -1.

   WHAT THIS DOES NOT DO, and the caller must not assume it does: the direct
   boot re-seats data_0209f264[0] and data_0209f220[0] from SM64DS_ENTRANCE and
   SM64DS_STAR_FILTER itself (hal/level_boot.cpp, the block above the star
   filter comment), so on the boot path those two writes are overwritten a
   moment later. They are made anyway because this is the ROM's latch and a
   partial copy of it is worse than a redundant one -- and on the title route
   the two agree by value in any case: StartFile asks for entrance 0 and star 1,
   which are precisely the boot's own defaults. The one write that survives and
   matters is data_0209f2f8, which is what port_level_set_target is handed. */
extern "C" int port_level_entry_latch(void)
{
    if (data_02092110 < 0)
        return -1;

    /* ---- WHAT A SCENE LEAVES BEHIND THAT A LEVEL CANNOT INHERIT -----------
     *
     * Three of the four things port_level_change_apply does before it latches
     * are owed here too, for exactly the reasons that function gives, and they
     * are owed because the TITLE IS A REAL SCENE: it loaded models, it filled
     * the shared-model array and it started sounds, the same way a level does.
     * Measured rather than assumed -- without the two model lines the level
     * boot dies in the ROM's own panic, three frames into the castle grounds:
     *
     *     FATAL: game Crash() reached
     *       Heap::Crash <- Model::LoadTexAndPal <- Model::AddToCommonModelData
     *       Arr <- Model::LoadFile
     *
     * which is the failure the comment above port_model_vram_reset in
     * port_level_change_apply predicts in so many words: "Without it the second
     * boot exhausts the arena and Model::GetVramOffset reaches the game's
     * Crash()." The title's textures were still holding the VRAM cursors.
     *
     * WHAT IS *NOT* DONE HERE, and why: port_level_teardown. There is no level
     * to tear down -- this is the first one this process will boot -- and the
     * title has already destroyed itself through the ROM's own path (cleanup 1,
     * pending-destroy 1 in the scene census). Marking a torn-down cast for
     * destruction a second time is how a double free is written. */
    {
        const int loops_before = port_course_loop_live();
        sd_sound_level_reap();
        const int loops_after = port_course_loop_live();
        if (loops_before || loops_after)
            std::fprintf(stderr, "  [title-entry] looping-sound reap: %d live "
                         "-> %d\n", loops_before, loops_after);
    }
    CleanCommonModelDataArr();
    port_model_vram_reset();
    port_level_reset_host();

    port_level_latch();
    port_level_set_target((int)data_0209f2f8);
    return (int)data_0209f2f8;
}

/* ---- WHAT A DECLINE OWES THE PLAYER ---------------------------------------
   Consuming the request is not the whole of a decline. Whatever asked for the
   level had already begun handing the player over, and on the ROM it never has
   to take that back, because on the ROM the level always boots.

   THE ONE THAT BIT TWO PLAYERS is the painting warp: EXIT (actor 349, class
   VirtualDoor, ov002), which is hosted and ticks on every mounted level. Its
   Behavior takes the player with the ROM's own

       Player::SetNoControlState(6, -1, 0)

   -- an INDEFINITE no-control -- and then, for a wall painting, seeds a
   counter at the exit's +0x98 and pins the player's depth to it a frame at a
   time until it clamps at -0x300000, which is 768 units behind the painting
   plane. At the clamp it calls LoadLevel, and here we are.

   Left alone, the decline below writes data_02092110 = -1 and returns, and
   NOTHING undoes either half:
     - the player stays in the no-control state, forever, because the argument
       was -1;
     - the exit's counter stays at the clamp, so the next frame clamps again,
       calls LoadLevel again and is declined again, at frame rate.
   Measured on castle_2f walking into the Snowman's Land painting (level 19,
   not mounted): 124 declines in a 220-frame run, the player 768 units inside
   the wall, and from there falling with no bottom and no rescue -- the run
   ended with him 8900 units under the level and still in the same state.

   The TILTED records -- the floor holes rather than the wall paintings, the
   ones whose rotX is not zero (Rainbow Ride, Bowser in the Sky, rainbow_mario
   on castle_2f, koopa2 in the basement) -- take the OTHER branch: they call
   LoadLevel on the trigger frame and never seed the counter. So they decline
   ONCE and go quiet, and the player is frozen with no repeating line to send
   us. Same freeze, no evidence.

   ---- WHY THE UNDO IS HERE AND NOT IN THE EXIT ------------------------------
   Because the exit cannot see a decline. The whole body of
   VirtualDoor::Behavior sits behind `data_02092110 < 0`, and after a decline
   that word IS -1 -- byte-identical to "nothing pending", which is the ROM's
   idle state. No bit anywhere distinguishes "no request" from "a request was
   refused". Giving the exit one means either editing src/, which is gated
   against the ROM's bytes, or shadowing a matched Behavior in port/hal with a
   second copy that drifts from it.

   And the exit is not the only requester. LoadLevel is reached from the star
   flow, ExitLevel, HitDeathPlane, KillPlayer, the warp pipes and the debug
   menu; teaching each one to notice a decline is this fix N times. The decline
   site is the single place that knows a change was refused, and which level
   was refused, on the frame it happened -- it already owns the consume line --
   and everything the undo needs is reachable from it through the game's own
   accessors.

   ---- WHAT IT UNDOES, AND WHAT IT DELIBERATELY DOES NOT ---------------------
   CONTROL comes back through the ROM's own way out of a no-control state, not
   a flag cleared by hand: Player::Unk_020ca150(4) (ov002 0x020ca150), which is
   what ov014's own actor calls to hand control back after its cutscene. It is
   guarded on IsState(ST_NO_CONTROL), so it is a no-op when the requester was
   not a cutscene at all, and its body is ChangeState(&data_ov002_0211013c) --
   one of the four states ChangeState's own gate lets a no-control player leave
   to. ChangeState is what clears the state: mIsControlDisabled directly, and
   mIsNoControl through func_ov002_020c9e18. Nothing here writes either.

   POSITION is undone only where the exit MOVED him, and the exit's own two
   words say whether it did. +0x98 is the pull counter: non-zero means this
   exit is mid pull-in and dragged him, zero means it never touched him. So:

     - mid pull-in (a wall painting): put him back on the side of the plane he
       came in from, at the depth the exit itself recorded when it took him
       (+0x88, which the pull branch never writes, so it still holds the
       crossing), MIRRORED across the plane -- the side he came from is by
       definition the other side of the coordinate he crossed to, which needs
       no assumption about which way the exit faces. His height is clamped back
       into the exit's OWN trigger box, because the pull put him behind a wall
       with no floor and he has been falling ever since; the bottom of that box
       is the lowest point at which the ROM itself considers him to be at this
       painting, and at a painting that is the floor in front of it. Then the
       counter is cleared and +0x88 is set to where he now is, so the next
       frame sees no crossing and the exit is armed again exactly as it was
       before he walked up to it.

     - never pulled (a tilted floor hole): HE IS NOT MOVED. The exit did not
       displace him -- it fired on the trigger frame and the decline lands the
       same frame -- so he is exactly where his own falling put him, one
       stride into the mouth of the hole. Putting him back above it would only
       drop him in again, and the record names no place to stand. Handing
       control back is the whole of it, and it is enough: measured on
       rainbow_mario (level 34) the frozen player falls through the level for
       ever, and the frame control comes back the game's own ground contact
       catches him 60 units down and he walks out.

   NOT TOUCHED: the screen. Measured on both variants, the blend registers read
   clear through the whole decline, so there is nothing covered to reopen, and
   opening a wipe nobody closed would be inventing a transition. Nothing is
   drawn and nothing is said on screen; whether a refused course should say so
   is a product decision that has not been made.

   NOT TOUCHED: anything on the path where the level change SUCCEEDS. This runs
   only from a decline. */
extern "C" {
extern unsigned char data_0209f250;      /* local player index */
extern void *data_0209f394[];            /* per-player Actor* */
/* the same two calls VirtualDoor::Behavior itself uses */
char *_ZN5Actor15FindWithActorIDEjPS_(unsigned int id, void *prev);
void MulVec3Mat4x3(const void *in, const void *m, void *out);
void InvMat4x3(const void *in, void *out);
/* the ROM's own return from a no-control state (hal/bob_enemy_header_faces) */
int _ZN6Player12Unk_020ca150Eh(void *self, unsigned char a);
}

enum { PORT_ACTOR_EXIT = 349 };

/* The exit that is mid pull-in, or null. Only one can be: the counter is
   seeded when the exit takes the player and cleared when it lets go, and
   there is one player. */
static char *port_exit_pulling(void)
{
    for (char *e = _ZN5Actor15FindWithActorIDEjPS_(PORT_ACTOR_EXIT, 0); e;
         e = _ZN5Actor15FindWithActorIDEjPS_(PORT_ACTOR_EXIT, e))
        if (*(int *)(e + 0x98) != 0)
            return e;
    return 0;
}

static void port_level_change_declined(void)
{
    char *player = (char *)data_0209f394[data_0209f250];
    if (!player)
        return;

    char *ex = port_exit_pulling();
    if (ex) {
        /* +0xd4 is the exit's world-to-local matrix, built and inverted by
           VirtualDoor::InitResources; the pull reads the player through it
           and writes him back through its inverse, so this is that, once,
           backwards. */
        int local[3], inv[12];
        MulVec3Mat4x3(player + 0x5c, ex + 0xd4, local);
        local[2] = -*(int *)(ex + 0x88);
        if (local[1] < 0)
            local[1] = 0;
        else if (local[1] > *(int *)(ex + 0x84))
            local[1] = *(int *)(ex + 0x84);
        InvMat4x3(ex + 0xd4, inv);
        MulVec3Mat4x3(local, inv, player + 0x5c);

        *(int *)(ex + 0x98) = 0;         /* let go of him */
        *(int *)(ex + 0x88) = local[2];  /* and see him where he now is, so
                                            standing still is not a crossing */
        std::fprintf(stderr, "  [lvl] the exit had him 768 units in: pull "
                     "released, put back at the painting (%d, %d, %d)\n",
                     *(int *)(player + 0x5c) >> 12,
                     *(int *)(player + 0x60) >> 12,
                     *(int *)(player + 0x64) >> 12);
    }

    /* and the half that matters even when no exit asked: give control back. */
    if (_ZN6Player12Unk_020ca150Eh(player, 4))
        std::fprintf(stderr, "  [lvl] control handed back to the player "
                     "(Player::Unk_020ca150, the ROM's own way out of "
                     "SetNoControlState)\n");
}

/* defined further down, with the whole derivation beside it */
extern "C" int port_scene_request_release(const char *why);

extern "C" int port_level_change_apply(void)
{
    if (data_02092110 < 0)
        return 0;

    const int want = data_02092110;
    const int from = data_0209f2f8;
    const unsigned free_before = port_level_heap_free();

    if (!port_level_is_mounted(want)) {
        std::fprintf(stderr,
                     "  [lvl] level %d (overlay %d, LVL_Overlay DS 0x%08x) is "
                     "NOT MOUNTED in this build -- the handoff is real, the "
                     "mount is the level-boot seam (port_level_mount_register)"
                     "\n", want, port_level_overlay_id(want),
                     port_level_ds_overlay(want));
        data_02092110 = -1;        /* consume it: a stuck request re-fires */
        port_level_change_declined();   /* and take back the handover */
        return 0;
    }

    std::fprintf(stderr, "[lvl] change: level %d -> %d, entrance %u, reason %u\n",
                from, want, (unsigned)data_0209f268, (unsigned)data_0209f26c);

    /* BEFORE THE TEARDOWN, not after, and that ordering is the whole fix for
       the writers that never arm a host fade request. HitDeathPlane leaves
       scene 8 pending and then the change fires the same frame; releasing on
       the far side would let this very teardown dispatch the Stage's trapped
       slot 3, which is the abort being fixed. The port is committing to a level
       change here, so any scene it has not spawned by now it never will. */
    port_scene_request_release("a level change is being applied");

    const double lvlperf_t0 = port_lvlperf_now();
    if (!port_level_teardown()) {
        std::fprintf(stderr, "  [lvl] teardown failed; the change is "
                     "declined and the level stands\n");
        data_02092110 = -1;
        /* DELIBERATELY NOT port_level_change_declined() here. This decline is
           downstream of the teardown, which destroys every actor including
           the Player, so data_0209f394 is dangling by the time we reach it and
           handing "the player" control back would be a use-after-free. The
           unmounted decline above is the only one that runs with the level
           still standing. */
        return 0;
    }

    /* THE LOOPING-SOUND REAP, the ROM's Scene::BeforeCleanupResources. On the
       DS the Scene actor is respawned per level, so its slot-4 override fires on
       every level change and calls func_02011974 over data_0209b53c, stopping
       every looping sound the old level started before the new one boots. The
       port keeps the Scene alive across levels (see the header note), so that
       slot never dispatches; this is that reap, hosted at the same seam -- after
       the actors that own the loops are gone, before the new level boots. The
       per-frame reaper (func_020119c8) spares a loop refreshed on its last live
       frame, so without this a sound whose owner is torn down mid-change carries
       into the next level. */
    {
        int loops_before = port_course_loop_live();
        sd_sound_level_reap();
        int loops_after = port_course_loop_live();
        if (loops_before || loops_after)
            std::fprintf(stderr, "  [lvl] looping-sound reap "
                         "(Scene::BeforeCleanupResources): %d live -> %d\n",
                         loops_before, loops_after);
    }

    /* THE COMMON-MODEL ARRAY, and it is the ROM's own line. Model::LoadFile
       registers every BMD it loads in data_0209cefc so that two actors asking
       for the same file share one parsed copy, and Stage::CleanupResources
       calls CleanCommonModelDataArr to empty it -- which is the half of the
       ROM's Stage teardown the port's boot really does owe, because the port
       DOES run the loader that fills it. Left behind, the array holds
       pointers to models the actors' own destructors have already freed, and
       the next level's first Model::LoadFile walks one: the second boot got
       as far as spawning the castle grounds' butterflies and then called
       through a freed entry. */
    CleanCommonModelDataArr();

    /* and the texture-VRAM cursors, which are the port's own expression of
       InitialiseVramGlobals -- the line Stage::InitResources opens a level
       boot with. Without it the second boot exhausts the arena and
       Model::GetVramOffset reaches the game's Crash(). */
    port_model_vram_reset();

    const unsigned free_torn = port_level_heap_free();
    port_level_reset_host();
    port_level_latch();
    /* Point the boot at the level the latch just made current. Without this the
       boot's mount resolved to the env-cached level and the warp re-booted the
       castle grounds -- the [lvl] line said "level 1 up" after a select of
       level 6, and the census came back the castle's, doubled. The latch put
       the new level in data_0209f2f8; hand the same id to the boot. */
    port_level_set_target((int)data_0209f2f8);

    void *stage = port_stage_object();
    if (!stage) {
        std::fprintf(stderr, "  [lvl] no Stage: the change needs the real "
                     "boot (SM64DS_LEGACY_BOOT is on)\n");
        return 0;
    }
    port_level_stage_reseat(stage);
    /* [lvl-perf] span 0: everything between the change firing and the boot */
    port_lvlperf_note(0, port_lvlperf_now() - lvlperf_t0);
    port_stage_a_boot((char *)stage + 0x91c, 1);

    const unsigned free_after = port_level_heap_free();
    std::fprintf(stderr, "[lvl] level %d up. heap free: %u before, %u torn down, %u "
                "after (net %+d)\n", (int)data_0209f2f8, free_before,
                free_torn, free_after, (int)free_after - (int)free_before);
    port_lvlperf_emit();
    return 1;
}

/* The per-frame poll. Sits where Scene::SpawnIfNecessary sits in the ROM's
   own frame (func_020197b8 phase 3): after input, before the actor phases,
   so a level that comes up mid-frame gets its first tick from the same frame
   loop as any other. */
extern "C" int port_level_change_poll(void)
{
    if (data_02092110 < 0)
        return 0;
    return port_level_change_apply();
}

/* ---- the front door -------------------------------------------------------
   dScTitle_c's own selection table, and its own handoff call.
   func_ov003_020ad814 (the debug level select's Behavior) picks a row out of
   data_ov003_020b1180 -- 0x36 eight-byte rows, byte 0 the level id, byte 1
   the entrance -- and calls LoadLevelNoReturn(level, entrance, 1, 0). Two
   rows are sentinels: -1 means "back to the file select" and -2 "into the
   minigame menu", neither of which is a level.
   The port reads the SAME TABLE. What it does not run is the scene actor
   around it: dScTitle_c is an ov003 class and ov003 is not mounted, so the
   rendered grid, its cursor sprite and its music are not here. The row list
   and the call it ends in are Nintendo's; the presentation is the port's own
   menu (tests/walk_window.cpp). */
extern "C" {
extern signed char data_ov003_020b1180[];   /* romdata: the level-select rows */
void LoadLevelNoReturn(int level, unsigned entrance, unsigned star,
                       unsigned reason);
void SetPlayerGlobals(void);
void SetNumPlayers(unsigned n);
extern unsigned char data_0209f2d8;         /* game mode */
/* Scene::StartSceneFade is matched src (slice_gate10): it records the pending
   scene id in data_02092664 and writes the fade colour into data_0209f5e8+0xc.
   It does NOT put the fade in motion -- on the ROM the Scene actor's own
   BeforeBehavior does that when it sees the pending scene. */
void _ZN5Scene14StartSceneFadeEjjt(unsigned actorID, unsigned param,
                                   unsigned short fadeColor);
extern unsigned short data_02092664;         /* Scene::SetSceneToSpawn's id */
extern unsigned short data_0209f5e8[];        /* the color fader (its +0xc word) */
/* hal/fader_wipes.cpp: put the color fader in motion for the port's frame loop
   to step, since the port has no Scene actor to arm data_0209d4b0. */
void port_fader_start_color(int frames, int toEnd, unsigned short color);
}

/* ---- the scene-fade request (gate 31, deliverable 3) ----------------------
   Scene::StartSceneFade parks a pending scene id in data_02092664 and a colour
   in the fader. On the ROM the Scene actor consumes the pending id (spawns the
   scene) once the fade has covered the screen. The port has no scene spawner
   for ov003 scenes, so it records the request here and the frame loop acts on
   it: run the colour fade, and when a scene handler exists (dScStarSel_c, the
   stretch), hand off to it. Until then the request is recorded and reported so
   the flow is visible and the fade renders.

   ---- WHAT BOOTING dScStarSel_c NEEDS (the star select, scene 4) -----------
   The boot chain is fully mapped and none of it is a guess:

     data_02092664 = 4  (StartSceneFade set it)
       -> Scene::SpawnIfNecessary  calls func_02013edc(4, param, 1)
       -> func_02042fe4 -> func_02043098(4, 0, param, 1)   the spawn spine
       -> (*(Fn*)data_020a4bb8[4])()   the factory for scene id 4
       -> StarSelect_Spawn  (ov003, 0x020b04f0)

   StarSelect_Spawn (src/StarSelect_Spawn.cpp) is small and portable-shaped:
     - ActorBase::operator new(0x13c), ActorBase ctor
     - vptr = data_ov003_020b1704   (the dScStarSel_c vtable, IN ov003)
     - flags +0x13 |= 1|4
     - func_020733a8(self+0x64, 2, 0x50, Model::ctor, Model::dtor)  two Models

   The dScStarSel_c vtable (data_ov003_020b1704, from ov003 relocs) is:
     slot 0  0x020af8a0   (a method, ov003)
     slot 3  0x020af86c   CleanupResources           (ov003)
     slot 6  0x020af038   Behavior                   (ov003, NONMATCHING src)
     slot 7  0x0202e3d4   Scene::BeforeBehavior      (MAIN -- port HAS it)
     slot 8  0x0202e3c8   Scene::AfterBehavior       (MAIN -- port HAS it)
     slot 9  0x020ae6f4   Render (0x944 bytes of OAM) (ov003)
     slot 12 0x020ae6f0   OnPendingDestroy           (ov003)
   So the framework slots (Before/AfterBehavior) are already hosted; the
   scene-specific slots (Behavior, Render, CleanupResources, InitResources)
   are ov003 code.

   WHAT IS TOO DEEP for a first pass, and why the port records-and-reports
   rather than boots:
     1. The vtable and the four ov003 methods are not mounted -- ov003 is
        mounted for ONE data table (the row list), not its .text. Booting the
        scene means mounting ov003 code + relocs, the same per-overlay work a
        level mount is.
     2. dScStarSel_c::Render is 0x944 bytes of OAM::Render calls -- the star
        grid, the course thumbnails, the cursor -- built on the 2D sprite
        engine and the star/coin SAVE DATA. The port's OAM path exists (gate
        25) but the star-select graphics (its SpawnInfo-loaded 2D resources in
        InitResources) are not staged.
     3. dScStarSel_c::Behavior (0x020af038, above in this file's evidence) ends
        in StartSceneFade(3,0,0) -- the star select's OWN handoff back into the
        level (scene 3, the Stage boot). So the port's current path (title ->
        level, with the fade) already produces the END STATE the star select
        would hand to; what is missing is the intermediate star-choice UI, not
        a different level outcome.

   So the stretch is a real sub-project (mount ov003 .text, stage the star-grid
   2D resources, host the OAM render), landed here as analysis. The fade flow
   that would drive it is live: the request is recorded, the screen fades, and
   the frame loop is the seam a real StarSelect_Spawn registration would plug
   into (register a host factory at data_020a4bb8[4], then spawn on cover). */
static int g_scene_fade_scene = -1;   /* pending scene id, -1 = none */

extern "C" int port_scene_fade_pending(int *sceneId)
{
    if (g_scene_fade_scene < 0)
        return 0;
    if (sceneId) *sceneId = g_scene_fade_scene;
    return 1;
}

/* ---- THE PENDING-SCENE SENTINEL, AND THE ABORT IT WAS CAUSING -------------
 *
 * REPORTED FROM REAL PLAY, playlog play_20260816_210444.log: pick a level from
 * the debug menu, play it for seventeen minutes, pause, choose "exit course",
 * and the level change aborts inside port_level_teardown with a Stage vtable
 * trap. Reproduced in 300 frames and isolated to one global; the whole
 * derivation is port/exitlevel_seat.txt.
 *
 * data_02092664 is Scene::SetSceneToSpawn's PENDING SCENE ID. 0x187 is the
 * "nothing pending" sentinel it carries in the ROM's .data. On the ROM exactly
 * one function takes it out of that state and exactly one puts it back, and
 * both are matched src in this tree:
 *
 *     Scene::SetSceneToSpawn(id, param)   data_02092664 = id
 *         src/_ZN5Scene15SetSceneToSpawnEjj.c
 *     Scene::SpawnIfNecessary()           spawn the scene, THEN
 *                                         data_02092664 = 0x187
 *         src/_ZN5Scene16SpawnIfNecessaryEv.c
 *
 * THE PORT RUNS THE FIRST HALF AND NOT THE SECOND. It has no spawner for the
 * ov003 scenes -- the long block above this one says why, at length: ov003's
 * .text is not mounted and the star grid's 2D resources are not staged -- so
 * SpawnIfNecessary never runs and the id stays exactly where the writer left
 * it, for the rest of the session. Three writers reach it in normal play:
 *
 *     port_title_select      StartSceneFade(4, 0, 0)   the debug level select
 *     HitDeathPlane          StartSceneFade(8, 0, 0)   hal/star_flow.cpp:127
 *     Scene::BeforeBehavior  StartSceneFade(1, 0, 0)   its own edge A
 *
 * WHAT A LATCHED ID COSTS IS NOT THE MISSING SCENE. Scene::BeforeBehavior --
 * the ROM's own body, correctly seated in _ZTV5Stage slot 7 and dispatched on
 * the Stage every frame -- reads it:
 *
 *     if (data_02092664 != 0x187) {
 *         if (fader->IsAtStart())    fader->SetForwardTime(0x1e, 0);
 *         else if (fader->IsAtEnd()) ActorBase::MarkForDestruction(self);
 *         return 1;
 *     }
 *
 * `self` is the Stage. So a latched id turns "the installed fader is at the end
 * of its travel" -- which every level change produces, because the change fades
 * out first -- into MarkForDestruction on the ONE actor this file keeps alive
 * across level changes. Phase 1 then moves the Stage onto the cleanup list and
 * the cleanup Process dispatches slot 3, which is trapped and measured blocked
 * on seven pieces (port/stage_lifecycle_map.txt section 5).
 *
 * THE ABORT IS CORRECT AND IS NOT WHAT THIS FIXES. A port whose Stage is being
 * torn down has lost the scene root, the level collider and the level model;
 * stage_lifecycle_map.txt section 9 argues at length that stopping there is
 * right, and this lane agrees. The defect is upstream: the port was asking for
 * a Stage teardown the ROM never asks for.
 *
 * THE FIX IS THE HALF OF THE ROM'S OWN TRANSITION THE PORT WAS MISSING. Once
 * the port has done everything it is going to do about a pending scene request,
 * it completes SpawnIfNecessary's state change -- the sentinel goes back --
 * without the spawn it has already declined. It does NOT invent a scene, and it
 * deliberately does NOT touch data_02092660, SpawnIfNecessary's "already
 * spawned" latch, because writing that would claim a spawn that did not happen.
 * On a session that never arms a scene fade this function never fires.
 *
 * SM64DS_SCENE_LATCH=1 declines the release and restores the old behaviour
 * exactly, which is how the abort is reproduced from a fixed binary. */
extern "C" int port_scene_request_release(const char *why)
{
    static int keep = -1;
    if (keep < 0) keep = std::getenv("SM64DS_SCENE_LATCH") != 0;
    if (data_02092664 == 0x187)
        return 0;                       /* nothing pending; the common case */
    if (keep) {
        std::fprintf(stderr, "  [scene] pending scene %u KEPT (%s): "
                     "SM64DS_SCENE_LATCH=1 is holding the pre-fix behaviour\n",
                     (unsigned)data_02092664, why);
        return 0;
    }
    std::fprintf(stderr, "  [scene] pending scene %u released to the 0x187 "
                 "sentinel (%s): the port declined the spawn, so it completes "
                 "Scene::SpawnIfNecessary's other half\n",
                 (unsigned)data_02092664, why);
    data_02092664 = 0x187;
    return 1;
}

extern "C" void port_scene_fade_clear(void)
{
    g_scene_fade_scene = -1;
    /* the title-select path's own consume point: the fade has covered, the
       level behind it is up, and the request has had every effect the port can
       give it. */
    port_scene_request_release("the scene fade is done and the level is up");
}

enum { PORT_TITLE_ROWS = 0x36 };

extern "C" int port_title_rows(void) { return PORT_TITLE_ROWS; }

extern "C" int port_title_row(int i, int *level, int *entrance)
{
    if (i < 0 || i >= PORT_TITLE_ROWS)
        return 0;
    const signed char *r = data_ov003_020b1180 + i * 8;
    if (level) *level = r[0];
    if (entrance) *entrance = (unsigned char)r[1];
    return r[0] >= 0;         /* -1 / -2 are the two scene sentinels */
}

/* The else-branch of func_ov003_020ad814, now in the ROM's OWN order.
   FaderColor is staged (hal/fader_wipes.cpp), so LoadLevel's opening
   Scene::SetAndStopColorFader call is safe and the mount check no longer has to
   come first to dodge a null fader slot. The ROM branch runs verbatim, then the
   port refuses an unmounted row AFTER it -- which is what that function's
   old comment promised staging the color fader would allow. */
extern "C" int port_title_select(int i)
{
    int level = 0, entrance = 0;
    if (!port_title_row(i, &level, &entrance)) {
        std::fprintf(stderr, "  [title] row %d is a scene sentinel (%d), not "
                     "a level\n", i, level);
        return 0;
    }

    /* dScTitle_c::Behavior's confirm branch, in order (func_ov003_020ad814):
           data_0209f2d8 = 0;                       single player
           LoadLevelNoReturn(level, entrance, 1, 0);
           SetPlayerGlobals();
           SetNumPlayers(1);
           Scene::StartSceneFade(4, 0, 0);          hand to the star select
           data_0209f5e8[6] = 0x7fff;               fade to WHITE
       LoadLevelNoReturn opens with SetAndStopColorFader (safe now: the color
       fader is a real object), so this runs whether or not the port can mount
       the row. */
    /* run mg16 lane MP3: THE VS FLAG STAYS 0, DELIBERATELY, and an earlier
       revision of this line set it from the player count -- which is wrong for
       a reason that is not obvious and cost nothing only because it was caught
       before it shipped.

       data_0209f2d8 == 1 is not "two players are present". It is DS VS MODE,
       and it is a mode switch with a very wide surface: eleven attack-cylinder
       wrappers in ov002 branch on it, so does func_ov002_020dd908 (the VS
       ground-pound shockwave), and so do Stage, StarMarker, the HUD and the
       fader. Turning it on to get two players is paying for the entire VS
       feature set to get one of its preconditions, and every one of those
       branches is then live and unproven.

       TWO PLAYERS DO NOT NEED IT. The spawn loop, the per-slot input fan-out
       and the cylinder solver that pushes two bodies apart are all mode-0 code
       that reads the player COUNT, not the VS flag. So the count moves and the
       mode does not, and what the owner asked for -- two characters that move
       each other -- lands on the path that is already proven.

       The flag is what the VS MENU sets, through src/PrepareVsMode.c, when the
       player actually chooses VS. That is where it belongs and where it will
       come from once ov075 is mounted. */
    data_0209f2d8 = 0;
    LoadLevelNoReturn(level, (unsigned)entrance, 1, 0);
    SetPlayerGlobals();
    SetNumPlayers(port::vs_player_count());
    /* Scene::StartSceneFade(4, 0, 0): records scene 4 (dScStarSel_c) as the
       pending scene and sets the fade colour. data_0209f5e8[6] (+0xc) = 0x7fff
       is the ROM's own next line: fade to WHITE, not black. */
    _ZN5Scene14StartSceneFadeEjjt(4, 0, 0);
    data_0209f5e8[6] = 0x7fff;
    /* Record the scene request for the frame loop, and put the colour fade in
       motion so it renders. 0x7fff (nonzero) is a white fade; 16 frames is the
       DS default for a scene transition. */
    g_scene_fade_scene = (int)data_02092664;
    port_fader_start_color(16, 1, 0x7fff);
    std::fprintf(stderr, "[title] row %d -> level %d entrance %d, scene fade to "
                 "%d (white)\n", i, level, entrance, (int)data_02092664);

    /* NOW the not-mounted refusal, after the ROM's own order. The level request
       (data_02092110) that LoadLevelNoReturn wrote is consumed by the change
       poll, which already declines an unmounted level with a message; but say
       so here too, so a row that cannot boot is legible at the point of the
       choice rather than only when the poll fires. The fade still ran, which is
       the intended feedback that the button was seen. */
    if (!port_level_is_mounted(level)) {
        std::fprintf(stderr, "  [title] row %d is level %d (overlay %d), which "
                     "is not mounted in this build -- the fade ran, the change "
                     "poll will decline the boot\n", i, level,
                     port_level_overlay_id(level));
        /* leave data_02092110 for the poll to consume and report */
    }
    return 1;
}
