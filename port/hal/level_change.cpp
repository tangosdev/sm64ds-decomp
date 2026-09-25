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
/* THE SUBLEVEL THE PLAYER JUST CAME OUT OF, and the queued sublevel beside it
   (Stage::InitResources:222-223, written in port_level_latch below). Both are
   romdata-hosted -- host-src/romdata.c ships 6 and -1, StartFile's own values
   -- so they are declared as arrays, which is their host storage, while the
   ROM reads each as one signed byte. */
extern unsigned char data_02092124[];
extern unsigned char data_02092118[];

/* the ROM tables, from romdata.py */
extern unsigned char data_02092208[];   /* level -> LVL_Overlay (DS address) */
extern unsigned char data_020758c8[];   /* level -> overlay id */

/* the engine pieces the change drives */
void _ZN7fBase_c18MarkForDestructionEv(void *self);
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
/* hal/actor_vtables.cpp: how many spawns func_0203b438 refused to link into
   the scene tree (a parentless spawn into an already-rooted tree). */
int   port_tree_link_refusals(void);
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
void port_intro_arm_for_entry(void);     /* hal/level_boot.cpp: the intro seam --
                                            says "this entry came off a save-file
                                            pick"; the seam itself decides */
void CleanCommonModelDataArr(void);
void port_model_vram_reset(void);   /* hal/model_host.cpp */
void port_fader_wipes_reset(void);  /* hal/fader_wipes.cpp: the seven wipes
                                       Stage::InitResources builds fresh */
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
extern int data_0209f20c[];            /* the level-clear screen is up       */
extern unsigned char data_0209f2d4[];  /* LC_Update's / PS_Update's state     */
extern unsigned char data_0209f2e0[];  /* the menu row chosen                 */
extern unsigned char data_0209f244[];  /* menu-button state                   */
extern unsigned char data_0209f2b0[];  /* menu-button state                   */

extern int data_020a4b6c[];   /* scene tree     {head, cb, 0} */
/* the spawn parent ActorDerived::Spawn hands func_02043098 for every actor a
   level loads: on the ROM the Stage, written by Scene::ResetFadersAndSound
   (_ZTV5Stage slot 1) on every entry. hal/actor_vtables.cpp defines it. */
extern int data_0209f5c0[];
extern int data_020a4b78[];   /* behaviour list {head, tail, cb, 0} */
extern int data_020a4b88[];   /* pending list */
extern int data_020a4b98[];   /* render list */
extern int data_020a4ba8[];   /* cleanup list */
extern int data_0209b468[];   /* live-actor list {head, tail}; every
                                  dActor_c links its node at +0x50 in */

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

static int port_scene_crossing_due(void);   /* the level-to-scene crossing,
                                              below port_level_change_poll */

extern "C" int port_level_change_pending(void)
{
    /* A level change, or (run linkfull, lane GAMEOVER1) the ROM tearing its
       Stage down for a scene: both are answered by port_level_change_poll at
       the ROM's own position for them, where Scene::SpawnIfNecessary sits. */
    return data_02092110 >= 0 || port_scene_crossing_due();
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

/* THE SCENE TREE'S OWN CENSUS, and it is the one number the teardown never
   printed. The banner above port_level_live_count says the phase-1 scene pass
   (func_02043880) is what moves a marked actor onto the cleanup list, so an
   actor that is marked and never destroyed is first of all an actor that pass
   never reached. Three things can put it out of reach and the three look
   identical from the behaviour list: the tree's callback word cleared (then
   func_020441cc walks nobody), the tree's head cleared, or the actor linked
   under a parent that is no longer in the tree (then the pre-order walk from
   the head simply never arrives at it).

   So this walks the tree exactly as func_020441cc does -- head at
   data_020a4b6c[0], successor from func_0203b394, owner at node[4] -- and
   answers how many nodes it reaches, how many of those own a live actor, and
   whether a named actor is among them. Printed only under SM64DS_TRACE_LEVEL;
   it reads and writes nothing. */
static int port_level_tree_count(int *reached_actors)
{
    int n = 0, owners = 0, guard = 0;
    for (int *nd = (int *)(size_t)data_020a4b6c[0];
         nd && guard < 8192; nd = (int *)func_0203b394(nd), ++guard) {
        ++n;
        if (nd[4])
            ++owners;
    }
    if (reached_actors)
        *reached_actors = owners;
    return n;
}

/* Is this actor's SceneNode (actor+0x14) reachable from the tree head? */
static int port_level_tree_holds(void *actor)
{
    if (!actor)
        return 0;
    void *want = (char *)actor + 0x14;
    int guard = 0;
    for (int *nd = (int *)(size_t)data_020a4b6c[0];
         nd && guard < 8192; nd = (int *)func_0203b394(nd), ++guard)
        if ((void *)nd == want)
            return 1;
    return 0;
}

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
        _ZN7fBase_c18MarkForDestructionEv(o);
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
    if (trace) {
        int owners = 0;
        const int nodes = port_level_tree_count(&owners);
        void *first = 0;
        for (int *node = (int *)(size_t)data_020a4b78[0]; node;
             node = (int *)(size_t)node[1])
            if (node[2] && (void *)(size_t)node[2] != port_stage_object()) {
                first = (void *)(size_t)node[2];
                break;
            }
        void *stage = port_stage_object();
        std::printf("  [lvl] teardown entry: %d live | scene tree head %p cb "
                    "%p, %d node(s), %d owned | first live actor %p %s in the "
                    "tree | %d spawn(s) refused by the tree this session\n",
                    port_level_live_count(),
                    (void *)(size_t)data_020a4b6c[0],
                    (void *)(size_t)data_020a4b6c[1], nodes, owners, first,
                    port_level_tree_holds(first) ? "IS" : "is NOT",
                    port_tree_link_refusals());
        /* SceneNode is {parent, firstChild, prevSibling, nextSibling, owner}
           (src/func_0203b438.c). Whether the subtree is orphaned at the TOP
           (the Stage's firstChild cleared) or at the BOTTOM (the actors
           pointing at a parent that is not the Stage) are two different bugs
           that look identical from the node count, so print both words. */
        if (stage)
            std::printf("  [lvl] teardown entry: Stage %p node %p "
                        "{parent %p, firstChild %p, nextSibling %p}\n", stage,
                        (void *)((char *)stage + 0x14),
                        *(void **)((char *)stage + 0x14),
                        *(void **)((char *)stage + 0x18),
                        *(void **)((char *)stage + 0x20));
        if (first)
            std::printf("  [lvl] teardown entry: actor %p node %p "
                        "{parent %p%s, firstChild %p, nextSibling %p}\n", first,
                        (void *)((char *)first + 0x14),
                        *(void **)((char *)first + 0x14),
                        (stage && *(void **)((char *)first + 0x14) ==
                         (void *)((char *)stage + 0x14)) ? " = THE STAGE" : "",
                        *(void **)((char *)first + 0x18),
                        *(void **)((char *)first + 0x20));
    }
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
    /* AND THE LIVE-ACTOR LIST, the sixth structure. dActor_c's constructor links
       every actor into data_0209b468 through its node at actor+0x50 and its
       D1/D2 unlink it again, so a cleanup that never reaches dActor_c's own
       destructor leaves a node here even when all four processing lists and the
       scene tree came out clean. The head then points into a block the next
       level's fBase_c::operator new hands out, the allocator zeroes the owner
       word, and the first walker (dActor_c::FindWithID through func_02043f98)
       reads [0 + 4]. Same node shape and same primitives as the four above, so
       it is one more row. */
    struct { const char *name; int *list; } lists[5] = {
        {"behaviour", data_020a4b78}, {"pending", data_020a4b88},
        {"render", data_020a4b98}, {"cleanup", data_020a4ba8},
        {"actor", data_0209b468}};
    void *stage = port_stage_object();
    for (int i = 0; i < 5; ++i) {
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
    for (int k = 0; k < (int)port::kCommsMaxPlayers; ++k)   /* 0.3.2: sixteen slots */
        data_0209f394[k] = 0;
    data_0209f318 = 0;
    data_0209f2c4 = 0;

    /* The next statement of the same ROM function the three lines above come
       from, src/_ZN5Stage16CleanupResourcesEv.cpp:120-127, and here for the
       same reason: the port keeps one Stage alive across every level change,
       so Stage::CleanupResources never runs and nothing else clears them.
       data_0209f20c is "the level-clear screen is up" and data_0209f2d4 is
       its state machine's step (src/_ZN5Stage8BehaviorEv.cpp:206 runs
       Stage::LC_Update off the first, and src/_ZN5Stage9LC_UpdateEv.cpp case 6
       is what clears both when a player answers the menu).

       Left behind, they carry a level-clear screen into the next level: a
       level booted at an entrance the game never sends a player to can end
       its arrival animation in the one that sets the flag
       (src/actors/Player.cpp:6741), and the courtyard's second boot under
       that stale flag took an access violation in a Boo's model render.

       The DS's other clear of the same words is
       src/_ZN5Stage13InitResourcesEv.cpp:412-418, under
       `data_0209f2fc != 1`, and it is deliberately NOT ported here: on a
       fresh entry that guard declines, so the teardown line is the one that
       does the work on every path. port/hal/level_boot.cpp clears the same
       word once per process at the shared bring-up, which is the first
       boot's half and stays. */
    data_0209f2e0[0] = 0;
    data_0209f2d4[0] = 0;
    data_0209f244[0] = 0;
    data_0209f2b0[0] = 0;
    data_0209f20c[0] = 0;

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
    /* Heap's first word is its allocator (src/_ZN4HeapC2EPvjPS_.cpp). */
    void *alloc = *(void **)((char *)data_020a0eac + 4);
    if (!alloc)
        return 0;
    return _ZN22ExpandingHeapAllocator10MemoryLeftEv(alloc);
}

extern "C" unsigned port_level_heap_free_bytes(void)
{ return port_level_heap_free(); }

/* ---- Stage::InitResources:177-218: THE SUBLEVEL CLEAR ---------------------

   THE ROM CLEARS FOUR THINGS WHEN A LEVEL CHANGE LEAVES THE COURSE IT WAS IN,
   and until now the port cleared none of them. port/stage_lifecycle_map.txt
   listed them under "WHAT THE ROM'S InitResources DOES THAT THE PORT DOES NOT
   DO AT ALL": "the three sublevel clear loops (data_0209f4f8, data_0209f30c,
   data_0209f310, data_0209f358)", and at its own row for those symbols, "none.
   The per-player star/coin clear loops are not run at all."

   data_0209f4f8 IS THE ACTOR DEATH TABLE, and it is the one that shows. Three
   level parts of sixteen words, 512 slots each, indexed by
   GetLevelPart(data_0209f2f8) (src/DeathTable_GetBit.c). A slot is the
   SPAWN-ORDER INDEX of a placed object: LoadStandardObjects hands each object
   the running counter data_ov002_0211118c as its deathTableID
   (src/_Z19LoadStandardObjectsRN11LVL_Overlay11ObjSubTableEij.cpp), so slot 7
   is the eighth object the walk placed, whatever level the walk was for.
   dActor_c::TrackInDeathTable raises the slot when the object dies -- 120 call
   sites in src/, every Bob-omb, Goomba, 1-Up, Boo and coin among them -- and
   dActor_c::BeforeInitResources reads it back: an actor whose slot is up marks
   itself for destruction instead of initialising
   (src/_ZN8dActor_c19BeforeInitResourcesEv.cpp:34-38).

   So with the clear missing, everything a player killed or collected in one
   course stayed marked for the whole session, and the marks landed on the
   SPAWN-ORDER SLOTS of every level entered afterwards -- and the castle
   grounds, Bob-omb Battlefield and Whomp's Fortress all read part 0, so they
   share one set of 512 slots. Measured on this tree with 32 slots marked, the
   settled census fell from 51 actors to 39 on the castle grounds, 184 to 156
   on Bob-omb Battlefield and 156 to 142 on Whomp's Fortress, and the count of
   slots held stayed at 32 through twelve level changes.

   THE OTHER THREE are the per-player coin and star counters the same statement
   group clears: data_0209f358 (the coin counter GiveCoins increments),
   data_0209f30c and data_0209f310 (the VS star arrays). They are in one ROM
   `if`, so they are transcribed with it rather than split; a half of a
   statement group is not the cartridge.

   THE CONDITION IS THE ROM'S, byte for byte: VS mode, or an entry reason of 2,
   or a pending level in the Bowser/key set (the (level + 0xDC) & 0xFF <= 0xD
   and (1 << that) & 0x2A15 test -- levels 36, 38, 40, 45, 47, 49), or a
   pending level whose course is 0x1D (the castle), or simply a DIFFERENT
   course from the one being left. Two sublevels of one course (Cool Cool
   Mountain and its slide) therefore keep their marks, which is the cartridge's
   own behaviour and the reason the test is there at all.

   WHY IT SITS HERE AND NOT IN port_stage_boot_body, where the port's other
   InitResources lines land: it reads data_0209f2f8 (the level being LEFT) and
   data_02092110 (the level being ENTERED), and the four lines below are what
   consume both. Run after them, `to != from` would be false on every change
   and the clear would never fire. This is InitResources' own order -- :188
   before :227.

   The ROM runs the block inside InitResources' `if (this+0x9c4 == 0)` arm,
   which is the arm the port's hand-rolled boot corresponds to. The direct
   SM64DS_LEVEL boot does not come through here and does not need to: nothing
   has run yet in that process and the table is at its bss zero. */
extern "C" {
extern int   data_0209f4f8[];        /* 3 level parts x 16 words, 512 slots */
extern int   data_0209f34c;
extern unsigned char data_0209f30c[];
extern signed char   data_0209f310[];
extern short data_0209f358[];        /* the coin counter GiveCoins increments */
extern unsigned char data_0209f21c;  /* the player count */
extern unsigned char data_0209f2d8;  /* game mode: 1 = VS */
int SublevelToLevel(int i);
}

static void port_level_sublevel_clear(void)
{
    const int to   = SublevelToLevel((int)data_02092110);
    const int from = SublevelToLevel((int)data_0209f2f8);
    const unsigned bits = ((unsigned)(unsigned char)data_02092110 + 0xDC) & 0xFF;
    const int boss = (bits <= 0xD && ((1u << bits) & 0x2A15u)) ? 1 : 0;
    const int vs   = (data_0209f2d8 == 1) ? 1 : 0;

    if (!(vs || data_0209f26c == 2 || boss || to == 0x1D || to != from))
        return;

    const int was_c = (int)data_0209f358[0];
    const int was_r = (int)data_0209f30c[0];
    const int was_s = (int)data_0209f310[0];

    data_0209f34c = 0;
    for (int part = 0; part < 3; ++part)
        for (int w = 0; w < 0x10; ++w)
            data_0209f4f8[part * 0x10 + w] = 0;

    const int count = (int)data_0209f21c;
    if (count > 0) {
        const unsigned char why = data_0209f26c;
        for (int i = 0; i < count; ++i) {
            if (vs || (boss == 0 && why != 1))
                data_0209f358[i] = 0;
            data_0209f30c[i] = 0;
            data_0209f310[i] = 0;
        }
    }
    if (std::getenv("SM64DS_DEATH_WATCH"))
        std::fprintf(stderr, "  [deathtab] sublevel clear: course %d -> %d, "
                     "reason %d -- the death table and the per-player counters "
                     "are back to zero (coins %d -> %d, red coins %d -> 0, "
                     "silver stars %d -> 0)\n", from, to,
                     (int)data_0209f26c, was_c, (int)data_0209f358[0],
                     was_r, was_s);
}

/* Runs the four lines Stage::InitResources runs, in its order:
       prev      = data_0209f2f8
       current   = pending
       entrance  = next entrance
       star      = next star
   and then clears the request, which is Stage::InitResources' own last
   statement (data_02092110 = -1). Everything between those two in the ROM is
   the level boot itself.

   THE "prev" LINE IS A REAL STATEMENT, NOT A COMMENT. Stage::InitResources
   :221-223 is

       data_0209f2fc = data_0209f26c;
       if (data_0209f2fc == 1) {
           data_02092124 = data_0209f2f8;    <- the sublevel being LEFT
           data_02092118 = -1;
       }

   immediately above the `data_0209f2f8 = data_02092110` below, and the port
   used to keep only the first of the three (hal/level_boot.cpp latches
   data_0209f2fc at the boot). data_02092124 was therefore pinned forever at
   romdata's 6, and 6 is Bob-omb Battlefield: src/_ZN5Stage9LC_UpdateEv.cpp:73
   hands SublevelToLevel(data_02092124) to Message::DisplayLevelClearText as
   THE COURSE (func_0201d850 prints that value + 1 as the course number and
   indexes the course name at value + 0x196 and the star name at value * 7 +
   0x1b3), and :95 uses it again for the 100-coin record. So every star in the
   game, in any course, came up as COURSE 1 / BOB-OMB BATTLEFIELD with a
   Bob-omb Battlefield star name.

   THE GUARD IS SPELLED data_0209f26c BECAUSE THAT IS WHAT data_0209f2fc IS
   ABOUT TO BE: the ROM tests the copy one statement after making it, and the
   port's copy is made in the boot body a moment after this latch. 1 is a
   fresh entry and 2 is a death, so a death return leaves both words alone,
   exactly as on the cartridge.

   WHAT THIS ARMS, said plainly: a data_02092124 that tracks the previous
   sublevel lets func_ov002_020c7cbc (src/actors/Player.cpp:7025) reach
   LoadKeyModels. Its switch takes only sublevels 0x24, 0x26, 0x2d, 0x2f and
   0x31 -- the five Bowser fights -- so it arms on a key handover and on
   nothing else; every course sublevel falls through the switch, leaves the
   key slot at its -1 and returns 0. */
static void port_level_latch(void)
{
    /* :188-218 first, because the two words below are its inputs. */
    port_level_sublevel_clear();

    if (data_0209f26c == 1) {
        data_02092124[0] = (unsigned char)data_0209f2f8;
        data_02092118[0] = 0xffu;         /* -1 */
    }
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

    /* ---- ARM THE OPENING -------------------------------------------------
     * This function is the ONE thing only the title bridge calls, which makes
     * it the honest place to say "this entry came off a save-file pick". The
     * seam it arms (hal/level_boot.cpp, port_intro_wants_play) then applies the
     * ROM's own rule -- game mode 0, flags2 bit 7 clear -- and the ROM's own
     * Stage::LoadClsnAndObjects decides. Arming is not deciding: a used file,
     * or a suppressed entry, still answers no.
     *
     * It is armed HERE rather than in the level boot because the boot cannot
     * tell a title crossing from the direct SM64DS_LEVEL boot the whole
     * battery runs, and the ROM's gate has no level check -- deriving the
     * decision from the bit alone would start the opening in every level
     * selftest. */
    port_intro_arm_for_entry();

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
char *_ZN8dActor_c15FindWithActorIDEjPS_(unsigned int id, void *prev);
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
    for (char *e = _ZN8dActor_c15FindWithActorIDEjPS_(PORT_ACTOR_EXIT, 0); e;
         e = _ZN8dActor_c15FindWithActorIDEjPS_(PORT_ACTOR_EXIT, e))
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

/* ---- THE PAINTING ENTRY: level -> star select -> level, in one process ----
 *
 * ON THE CARTRIDGE, entering a painting shows the star select. The decision is
 * not the painting's and not the port's: it is Stage::Behavior's, the Stage's
 * own vtable slot 6, dispatched every frame and matched src in this tree
 * (src/_ZN5Stage8BehaviorEv.cpp:190-198):
 *
 *     if (data_02092110 >= 0) {                        // a level change is pending
 *         lvl  = SublevelToLevel(data_02092110);       // the level being entered
 *         lvl2 = SublevelToLevel(data_0209f2f8);       // the level being left
 *         bb   = (data_0209f2d8 == 2);
 *         if (bb == 0 && lvl <= 0xe && lvl != lvl2 && data_02092118 < 0
 *             && (data_02092110 != 0xc || data_0209f268 != 4))
 *             dScene_c::SetSceneToSpawn(4, 0);         // THE STAR SELECT
 *         else
 *             dScene_c::SetSceneToSpawn(3, 0);         // straight into the level
 *     }
 *
 * EVERY REFUSAL IN THAT `if` IS THE ROM'S OWN and this file adds none: game
 * mode 2, anything above the fifteen main courses (the castle rooms, the caps,
 * the Bowser stages, the key courses, the Rec Room, every VS map), a sublevel
 * of the course you are already in, a latched return level, and Big Boo's
 * Haunt through entrance 4. A minigame and the opening never reach the block at
 * all -- it is inside Stage::Behavior and a minigame scene has no Stage, and
 * the opening's own change is level 1 -> level 1, which fails `lvl != lvl2`
 * and takes the scene-3 arm. Measured: the opening gate's run log prints
 * "[lvl] change: level 1 -> 1" followed by "pending scene 3 released".
 *
 * THAT BODY ALREADY RUNS HERE. _ZTV5Stage slot 6 is the ROM's Stage::Behavior
 * on walk_window and walk_window_hires (SM64DS_STAGE_SLOT6_ROM, the thunk in
 * hal/stage_bridges.cpp into hal/stage_frame.cpp's port_stage_rom_behavior), so
 * on every painting entry the cartridge's own code writes data_02092664 = 4 --
 * and the port threw the request away, one statement later, in
 * port_level_change_poll's release. The whole of what follows is the port
 * answering it instead.
 *
 * WHEN THE DECISION ARRIVES, which is the thing that shapes this code.
 * port_level_teardown's convergence loop calls port_actor_tick, and the Stage
 * HEADS the behaviour list, so Stage::Behavior runs FROM INSIDE THE TEARDOWN
 * (the banner above port_level_change_poll works that out for level 20). So the
 * scene id is not readable before the teardown; it is readable after it. The
 * cartridge's order is the same -- the Stage's own destruction is what produces
 * the next scene -- and that is why this call sits where it does, between the
 * teardown half of port_level_change_apply and its boot half.
 *
 * WHAT THE INTERLUDE RUNS. hal/scene_boot.cpp's port_scene_tick is the scene
 * frame, complete: the touch poll, the scripted pad, port_scene_comms_publish,
 * the SCENE-REQUEST CARRIER (the ROM's own Scene::SpawnIfNecessary, which is
 * what spawns dScStarSel_c and clears the pending id itself), port_actor_tick,
 * the frame clock, the fader advance, port_actor_render, gx_render and
 * hal_sub_screen_present. Nothing new is written here; the level path simply
 * had no caller for it. Measured, warp castle grounds -> Bob-omb Battlefield:
 *
 *     [starsel] at the seam: pending scene 4, spawned latch 0, level request 6
 *     [scene] CARRIER: scene 4 pending -> SpawnIfNecessary SPAWNED (frame 0)
 *     [starsel] the star select asked for SCENE 3 at frame 217; act 1
 *     [lvl] level 6 up.
 *     [lvl] re-seated: player 30039DEC camera 30039C34
 *
 * THE STOP IS THE STAR SELECT'S OWN HANDOFF. dScStarSel_c::Behavior ends in
 * StartSceneFade(3, 0, 0) (src/_ZN12dScStarSel_c8BehaviorEv.cpp:144) and writes
 * the act on the next line, data_0209f1f0 = FB(this, 0x115) + 1. Scene 3 is the
 * Stage, so the scene's own exit is "boot the level with this star", and
 * port_level_latch's third line -- data_0209f220[0] = data_0209f1f0 -- carries
 * it. The port chooses no act and no star.
 *
 * WHAT IS NOT DONE HERE AND IS MEASURED, not assumed. data_02092660, the ROM's
 * "a scene has spawned" latch, does not return to 0 on this route, so the star
 * select is not destroyed by the ROM's own path before the level boots: the
 * title bridge's stop test (`== 3 && data_02092660 == 0`) would never fire, and
 * this one deliberately does not use it. Measured either way the level boots
 * and the run stays clean for 1500 frames. The fade is also not armed across
 * the crossing, so the cut is hard rather than covered; arming a cover whose
 * reveal is not wired on this route would leave a white screen, which is worse
 * than a cut. Both are named in the lane's write-up rather than papered over.
 *
 * DEFAULT ON, because on the cartridge every course entry shows this screen.
 * SM64DS_STARSEL_PAINTING=0 declines the interlude on the same binary, which is
 * how the before/after is taken without a rebuild.
 */
extern "C" {
extern unsigned short data_02092664;    /* Scene::SetSceneToSpawn's pending id */
extern unsigned char  data_02092660;    /* its "already spawned" latch */
void port_scene_tick(int frame, int tick_game);   /* hal/scene_boot.cpp */
/* The ROM's own MarkForDestruction edge on a scene, latched in
   hal/scene_boot.cpp's sc_bbeh off fBase_c::shouldBeKilled at +0x0f. That is
   the frame the cartridge is FINISHED with the star select, and the banner
   below says why the pending id is not. */
void port_scene_killed_reset(void);
int  port_scene_killed(void);
/* the live scene object and the abort's MarkForDestruction stand-in; see the
   banner on the interlude's tail (run link100, lane STARSEL5) */
void *port_scene_live_object(void);
int   port_scene_force_kill(void);
unsigned port_scene_behavior_ticks(void);
/* PadData[4], {u16 held, u16 pressed} per player. The star select reads it
   directly (src/_ZN12dScStarSel_c8BehaviorEv.cpp), so it is the one word that
   says whether a press made by the host ever reached the screen. Reported on
   the interlude's own progress line: the whole of the reported bug is that it
   read 0 on every one of 1800 frames. */
extern int data_020a0e58[];
}

/* THE INTERLUDE'S FRAME, filled in by tests/walk_window.cpp's main (run
   link100, lane STARSEL5). A POINTER and not a call, because this file is on
   the smoke targets as well and walk_window.cpp is not; with nobody to fill it
   in -- a smoke target, a headless bring-up -- the interlude falls back to the
   bare scene tick this loop used to make, which is what those builds had.
   Filled in, it is the WHOLE host frame: the message pump, the host input poll
   that ends in PadData, the scene tick, the present and the pace. */
extern "C" int (*port_interlude_frame_hook)(int frame) = 0;

/* Read by the three ROM Stage slots (hal/stage_frame.cpp's
   port_stage_rom_behavior and port_stage_rom_render, hal/stage_bridges.cpp's
   st_bbeh). While this answers 1 the port has no Stage, which is what the
   cartridge means at this moment: Scene::BeforeBehavior marked the Stage for
   destruction before Scene::SpawnIfNecessary spawned the star select. The port
   keeps ONE Stage alive across every level change and slot 3
   (CleanupResources) is not hosted, so it cannot follow the ROM there; a
   dormant Stage is the closest honest host state, and it is not cosmetic.
   Measured with the slots live: Stage::Render reached Camera::IsUnderwater
   through Stage::RenderFog and faulted on the camera the teardown destroyed
   (c0000005 at Camera::IsUnderwater), and Scene::BeforeBehavior took
   its `data_02092664 != 0x187` arm and marked the Stage, which aborts at the
   slot-3 trap one frame later. Stage::Behavior would also re-issue
   SetSceneToSpawn(4) every frame on top of the star select's own
   StartSceneFade(3), because data_02092110 is still pending until the latch. */
static int g_interlude_live;

extern "C" int port_level_interlude_live(void) { return g_interlude_live; }

static int port_starsel_painting_on(void)
{
    static int on = -1;
    if (on < 0) {
        const char *e = std::getenv("SM64DS_STARSEL_PAINTING");
        on = (e && std::atoi(e) == 0) ? 0 : 1;   /* DEFAULT ON */
    }
    return on;
}

/* Between the teardown half and the boot half. The level is gone,
   data_02092110 still names the level being entered (the latch has not run, so
   the star select's own SublevelToLevel(data_02092110) test reads the right
   level), and data_02092664 holds what Stage::Behavior decided. */
static void port_level_scene_interlude(void)
{
    if (!port_starsel_painting_on())
        return;
    if (data_02092664 != 4)
        return;                     /* the ROM's own scene-3 arm: straight in */

    /* NO GIVE-UP TIMER IN REAL PLAY, and this is the whole of Tango's bug
       report's first half.
     *
     * The 1800-frame cap was written when the interlude ran port_scene_tick and
     * nothing else: a scene that could not be reached by any input had to be
     * abandoned or the process would hang. Now that the frame is a real frame
     * the cap is wrong in a session -- on the cartridge the star select waits
     * for the player as long as the player takes, and the window can be closed
     * -- so it is kept ONLY for a scripted run, where a row that waits forever
     * is a lane that never finishes. cap <= 0 means "wait", which is what a
     * player gets. SM64DS_STARSEL_FRAMES still overrides both ways, and 0
     * through it asks a scripted run to wait too. */
    const int selftest = std::getenv("SM64DS_WINDOW_SELFTEST") != 0;
    int cap = selftest ? 1800 : 0;
    if (const char *e = std::getenv("SM64DS_STARSEL_FRAMES"))
        cap = std::atoi(e);
    std::fprintf(stderr, "[starsel] at the seam: pending scene %u, spawned "
                 "latch %u, level request %d -- running the star select in "
                 "this process %s\n",
                 (unsigned)data_02092664, (unsigned)data_02092660,
                 (int)data_02092110,
                 cap > 0 ? "with a scripted-run backstop" : "until it is done");
    if (cap > 0)
        std::fprintf(stderr, "[starsel] BACKSTOP ARMED: this run gives the star "
                     "select %d frames and then boots the level anyway. That is "
                     "a harness rule, not the game's: a session has no cap.\n",
                     cap);

    /* On the DS the Stage IS scene 3, and Scene::SpawnIfNecessary destroys
       the current scene before it spawns the next one -- so the tree root is
       EMPTY when dScStarSel_c spawns, and the star select becomes the root
       itself. The port keeps ONE Stage object alive across every level
       change instead of destroying and recreating it, so without this the
       Stage's node is still sitting in the root when the scene spawns, the
       spawn is parentless, and func_0203b438's handle_a refuses to link it
       at all ("if (a->f0 != 0) return 0"). Hand the root to the scene here,
       the way the cartridge's own object lifetime would, and take it back
       once the scene is gone (see the pump and the restore below). */
    void *il_stage = port_stage_object();
    const int il_root_was = data_020a4b6c[0];
    if (il_stage &&
        il_root_was == (int)(size_t)((char *)il_stage + 0x14)) {
        data_020a4b6c[0] = 0;
        std::fprintf(stderr, "[starsel] tree root released for the scene "
                     "(was the Stage node %p)\n", (void *)(size_t)il_root_was);
    }

    int f = 0, asked = -1, closed = 0;
    g_interlude_live = 1;
    port_scene_killed_reset();
    for (; cap <= 0 || f < cap; ++f) {
        /* THE WHOLE HOST FRAME, not just the scene tick. See the hook's banner
           above and tests/walk_window.cpp's port_interlude_frame: the outer
           window loop is not running during the interlude, so every per-frame
           duty it performs -- the message pump, the host input poll that ends
           in PadData, the present, the pace -- has to be performed here or the
           star select cannot be seen, cannot be pressed and cannot be left. */
        if (port_interlude_frame_hook) {
            if (port_interlude_frame_hook(f)) {
                closed = 1;
                std::fprintf(stderr, "[starsel] the window was closed at frame "
                             "%d; the interlude stops\n", f);
                break;
            }
        } else {
            port_scene_tick(f, 1);
        }
        if ((f % 300) == 0)
            std::fprintf(stderr, "  [starsel] f%d pending %u latch %u act %d "
                         "pad %04x/%04x\n",
                         f, (unsigned)data_02092664, (unsigned)data_02092660,
                         (int)data_0209f1f0,
                         (unsigned)((unsigned short *)data_020a0e58)[0],
                         (unsigned)((unsigned short *)data_020a0e58)[1]);
        /* THE GRID ITSELF, once it exists and again on every change.
           dScStarSel_c::InitResources:283-320 derives the row the player is
           looking at from the save block, and every question about "why did
           the pick not move" is a question about these six bytes: how many
           icons there are (+0x114), which of them may be chosen (+0x131), the
           one under the cursor (+0x115, and the act is that plus one), and the
           three state bytes the Behavior's own arms are gated on (+0x133,
           +0x135 -- nonzero is what lets a button confirm at all -- and
           +0x139). Without them a row can only report that nothing happened. */
        {
            static unsigned char was[6];
            static int had;
            unsigned char *sc = (unsigned char *)port_scene_live_object();
            if (sc) {
                const unsigned char now[6] = { sc[0x114], sc[0x131], sc[0x115],
                                               sc[0x133], sc[0x135], sc[0x139] };
                if (!had || std::memcmp(now, was, sizeof now) != 0) {
                    had = 1;
                    std::memcpy(was, now, sizeof now);
                    std::fprintf(stderr, "  [starsel] f%d grid icons %u mask "
                                 "%02x cur %u (act %u) state %02x/%02x/%02x\n",
                                 f, now[0], now[1], now[2], now[2] + 1u,
                                 now[3], now[4], now[5]);
                }
            }
        }
        /* AND EVERY EDGE OF IT, because a press held for two hundred frames in
           the middle of a thousand is invisible on a line printed every three
           hundred. This is the line that separates "the host was polled" from
           "the press reached the game": the pad census is written by the frame
           the hook just ran, before the scene's own Behavior read it. */
        {
            static unsigned short pad_was;
            const unsigned short now = ((unsigned short *)data_020a0e58)[0];
            if (now != pad_was) {
                std::fprintf(stderr, "  [starsel] f%d PadData held %04x -> "
                             "%04x\n", f, (unsigned)pad_was, (unsigned)now);
                pad_was = now;
            }
        }
        if (data_02092664 == 3 && asked < 0) {
            asked = f;
            std::fprintf(stderr, "[starsel] the star select asked for SCENE 3 "
                         "at frame %d; the act it chose is data_0209f1f0 = %d "
                         "(dScStarSel_c::Behavior's own FB(this,0x115) + 1; the "
                         "port chose none of it)\n", f, (int)data_0209f1f0);
        }
        /* AND THE INTERLUDE DOES NOT END THERE, which is the whole of this
           change. On the cartridge the request is the START of the scene's
           exit, not the end of it: dScene_c::BeforeBehavior then runs the
           installed fader forward for 0x1e frames and calls
           MarkForDestruction, and only then is the Stage allowed back. Handing
           the frame to the boot half on the asking frame left a star select
           that had never faded out and had never been marked, still holding
           the bottom screen's sprite layer over the course that booted under
           it. Measured on the l1to6 route: the ask lands at interlude frame
           217 and the kill flag at 249, and with those 32 frames given the
           course's own bottom-screen capture is byte-identical to the same
           course reached with no star select at all (sha 20e7b97397a609c9,
           which is also fixer MAPSTAGE's own pre-interlude sha for that
           route); without them 2171 pixels of it are the star select's
           sprites. So the stop is the ROM's flag, read through
           hal/scene_boot.cpp's latch, and the cap is only a backstop. */
        if (asked >= 0 && (port_scene_killed() || data_02092660 == 0)) {
            std::fprintf(stderr, "[starsel] the cartridge is finished with the "
                         "star select at frame %d, %d frames after the ask: "
                         "dScene_c::BeforeBehavior ran the installed fader's "
                         "0x1e-frame fade-out and marked the scene for "
                         "destruction (shouldBeKilled latched %u, latch %u). "
                         "The boot half runs now.\n",
                         f, f - asked, (unsigned)port_scene_killed(),
                         (unsigned)data_02092660);
            break;
        }
    }
    /* AND IF IT DID NOT FINISH, THE SCENE IS STILL TORN DOWN FIRST. This is the
       second half of the report: with the interlude abandoned the star select
       stayed in the behaviour list, so its sprite layer sat over the course on
       both screens, and the first stylus press that reached it made it ask for
       a scene change -- which marks the Stage and aborts at the unhosted slot
       3. A course must never boot underneath a live star select, whatever ended
       the interlude. The cartridge never has to abandon this screen and so has
       no path to copy: the port sets the byte fBase_c::MarkForDestruction sets
       and then keeps running frames until the list reaps it, loudly, because
       this is the port standing in for a sequence the ROM does not have. */
    const int unfinished = (asked < 0 || !port_scene_killed());
    if (unfinished && port_scene_live_object()) {
        std::fprintf(stderr, "[starsel] the star select is being TORN DOWN "
                     "unfinished (asked at %d, pending %u, latch %u, "
                     "shouldBeKilled %u, %s). The port marks it the way "
                     "fBase_c::MarkForDestruction does and runs the frames the "
                     "reap needs; the course must not boot under a live star "
                     "select.\n", asked, (unsigned)data_02092664,
                     (unsigned)data_02092660, (unsigned)port_scene_killed(),
                     closed ? "the window was closed"
                            : "the scripted-run backstop fired");
        port_scene_force_kill();
        /* 0x1e is the fade dScene_c::BeforeBehavior runs before it marks, and
           the reap itself is one more list walk; 64 is that with slack. Not a
           second wait for the player: the object is already marked. The stop
           test is the ROM's own reap -- the frame on which the scene's
           behaviour slot is no longer dispatched -- and not a pointer, which
           never becomes null by itself. */
        int reaped = -1;
        unsigned beh_was = port_scene_behavior_ticks();
        for (int k = 0; k < 64; ++k) {
            if (port_interlude_frame_hook) {
                if (port_interlude_frame_hook(f + k))
                    break;
            } else {
                port_scene_tick(f + k, 1);
            }
            const unsigned beh_now = port_scene_behavior_ticks();
            if (beh_now == beh_was) { reaped = k; break; }
            beh_was = beh_now;
        }
        if (reaped >= 0)
            std::fprintf(stderr, "[starsel] the star select was reaped out of "
                         "the behaviour list %d frame(s) after the mark; the "
                         "course boots with no scene over it\n", reaped + 1);
        else
            std::fprintf(stderr, "[starsel] WARNING: the star select was still "
                         "being dispatched 64 frames after the mark; the course "
                         "boots with a scene the port could not reap\n");
    }
    /* The scene is MARKED at this point, not gone: on the ROM its own reap
       runs on the next dispatch of phase 1 (func_02043880), which is a frame
       this interlude does not otherwise run. Pump the ROM's own phase passes
       here, with the Stage still OUT of the root (g_interlude_live is still
       1), until the destroyed scene's node comes out of the tree by itself
       (func_0203b3c0 clears the head when it unlinks a root node with no
       parent and no previous sibling). This has to happen BEFORE the Stage
       goes back into the root: a root node with no parent and no previous
       sibling unlinks by clearing `*list` outright, so if the Stage were
       back in the root already, the scene's later reap would wipe the Stage
       back out of it and the next teardown would read an empty tree just the
       same. Measured that way before this pump was added. */
    {
        int k = 0;
        for (; k < 16 && data_020a4b6c[0]; ++k) {
            port_actor_scene_pass();
            port_actor_tick();
            port_actor_scene_pass();
        }
        std::fprintf(stderr, "[starsel] scene reap pump: %d round(s), tree "
                     "root now %p, live actors %d\n", k,
                     (void *)(size_t)data_020a4b6c[0], port_level_live_count());
    }
    g_interlude_live = 0;
    /* Put the Stage back where the cartridge's Scene::ResetFadersAndSound
       would have put it on its own next entry: back in the scene tree's
       root, and back in the spawn-parent seat (data_0209f5c0) that the star
       select's own ResetFadersAndSound took when it ran. The port never
       constructs a second Stage, so nothing else will ever do this. */
    if (il_stage) {
        std::fprintf(stderr, "[starsel] tree root after the scene: %p; spawn "
                     "parent %p -- restoring the Stage (%p / node %p)\n",
                     (void *)(size_t)data_020a4b6c[0],
                     (void *)(size_t)data_0209f5c0[0], il_stage,
                     (void *)((char *)il_stage + 0x14));
        data_020a4b6c[0] = (int)(size_t)((char *)il_stage + 0x14);
        data_0209f5c0[0] = (int)(size_t)il_stage;
    }
    if (cap > 0 && f >= cap)
        std::fprintf(stderr, "[starsel] the star select did not finish inside "
                     "%d frames (asked at %d, pending %u, latch %u, "
                     "shouldBeKilled %u); the level boots with the act as it "
                     "stands rather than hanging\n", cap, asked,
                     (unsigned)data_02092664, (unsigned)data_02092660,
                     (unsigned)port_scene_killed());
    /* Whatever happened, the port has now done everything it is going to do
       about the request, so it completes Scene::SpawnIfNecessary's other half
       exactly as the poll's own tail release does. */
    port_scene_request_release("the star-select interlude is over");
}

static int g_crossed_scenes;
extern "C" int port_level_crossed_scenes(void);
extern "C" void *port_stage_create(void);   /* hal/stage_bridges.cpp */

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

    /* and the seven fader wipes, for the same reason and in the same place:
       Stage::InitResources BUILDS that pool at every level boot, so on the
       cartridge a level always opens with seven fresh FaderWipes. The port's
       are static objects that outlive the Stage, and a wipe that carries the
       last transition's interpolator makes dScene_c::SetFaders read the
       outgoing fader as mid-fade instead of at the start -- which left the
       screen fully black from the second star of a session onwards. The banner
       over port_fader_wipes_reset in hal/fader_wipes.cpp has the measurement. */
    port_fader_wipes_reset();

    const unsigned free_torn = port_level_heap_free();
    port_level_reset_host();
    /* THE TEARDOWN HALF ENDS HERE AND THE BOOT HALF BEGINS BELOW. The
       banner above port_level_scene_interlude says why the scene runs
       between them and not before or after: the ROM's own decision is
       written from inside the teardown, and the latch below consumes the
       level request the star select has to read. */
    port_level_scene_interlude();
    port_level_latch();
    /* Point the boot at the level the latch just made current. Without this the
       boot's mount resolved to the env-cached level and the warp re-booted the
       castle grounds -- the [lvl] line said "level 1 up" after a select of
       level 6, and the census came back the castle's, doubled. The latch put
       the new level in data_0209f2f8; hand the same id to the boot. */
    port_level_set_target((int)data_0209f2f8);

    void *stage = port_stage_object();
    if (!stage && port_level_crossed_scenes()) {
        /* THE ROM DESTROYED THE LAST ONE (run linkfull, lane GAMEOVER1). A
           level-to-scene crossing (the Game Over screen) ran the Stage's own
           teardown, so this level entry is the cartridge's scene-3 spawn: a
           new Stage out of the ROM's own factory, booted through the same
           first-entry path the process's first level took (aliveState 0, so
           port_stage_lifecycle_boot runs the init Process over slots 1/0/2). */
        stage = port_stage_create();
        std::fprintf(stderr, "  [lvl] the level-to-scene crossing destroyed the "
                     "Stage; this entry builds a new one: %p\n", stage);
    }
    if (!stage) {
        std::fprintf(stderr, "  [lvl] no Stage: the change needs the real "
                     "boot (SM64DS_LEGACY_BOOT is on)\n");
        return 0;
    }
    port_level_stage_reseat(stage);
    /* [lvl-perf] span 0: everything between the change firing and the boot */
    port_lvlperf_note(0, port_lvlperf_now() - lvlperf_t0);
    port_stage_a_boot((char *)stage + 0x91c, 1);

    g_crossed_scenes = 0;
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
   loop as any other.

   ---- AND IT IS SpawnIfNecessary'S OTHER HALF TOO (run link100, lane FRAME2)
   ---------------------------------------------------------------------------
   The release below is the SECOND half of the ROM's Scene::SpawnIfNecessary --
   `data_02092664 = 0x187` -- run at the ROM's own seam. The block above
   port_scene_request_release explains why the port owes that statement at all
   (it runs SetSceneToSpawn and has no spawner for the ov003 scenes); this call
   is about WHEN, and it is here rather than only inside the apply because of a
   writer that did not exist until _ZTV5Stage slot 6 held the ROM's own body.

   MEASURED, level 20 (Snowman's Land igloo), SM64DS_WINDOW_SELFTEST=300
   SM64DS_LEVEL=20 SM64DS_FAULTS_FATAL=1, on the tip that seats slot 6 (cdb
   breakpoints, f = the slot-6 thunk's own Behavior counter):

     LoadLevel f=79 lvl=19 ret=func_ov002_020b0a0c+0x3c   the igloo exit
     RELEASE   f=79 pend=187      (from port_level_change_apply's head call:
                                   nothing pending yet, so it did nothing)
     SETSCENE  f=80 id=3   from ?Behavior@Stage@@QAEHXZ+0x2a9
     A_BOOT    f=80 p110=-1                               level 19 up
     MFD_STAGE f=111 pend=3   from _ZN8dScene_c14BeforeBehaviorEv+0xe1
     FATAL: Stage vtable slot 3 (CleanupResources) is not hosted   (0xc0000409)

   Read the two middle lines together. port_level_teardown's convergence loop
   calls port_actor_tick(), which walks phase 3, and the Stage HEADS the
   behaviour list -- so the ROM's Stage::Behavior runs from INSIDE the teardown,
   twenty lines before port_level_latch() clears data_02092110. It therefore
   sees the change still pending, takes its own level-change arm and calls
   Scene::SetSceneToSpawn(3, 0) -- the ROM's correct answer to a pending level
   change, and the exact statement the cartridge's SpawnIfNecessary consumes a
   moment later. The head release had already run, one statement earlier, so
   nothing ever took the id back off 3; from then on Scene::BeforeBehavior (slot
   7, the ROM's own body) took its `data_02092664 != 0x187` branch every frame
   and, once the post-change fade read at-end, marked the STAGE for destruction.
   The abort thirty frames later is the block above this one, verbatim.

   The head release inside the apply STAYS -- its own comment says why (a
   HitDeathPlane request latched before the change would otherwise let that very
   teardown dispatch the trapped slot 3). This one covers the other side: any
   request raised WHILE the change was being applied, by the ROM's own Stage or
   by anything else. Running it here rather than at the apply's tail also covers
   the apply's three declines, and it is where the ROM's consumer sits.

   IT DOES NOT RELEASE ON A FRAME WITH NO PENDING CHANGE. The early return above
   is unchanged, so a session that never changes level reaches nothing here, and
   the title-select path is untouched: its own fade gate reads g_scene_fade_scene
   (a host word), port_scene_fade_clear() still runs after the boot, and the id
   it would have released has already gone back to the sentinel by then.

   SM64DS_SCENE_LATCH=1 still declines every release, this one included, which
   is how the abort above is reproduced from a fixed binary. */
static int port_level_scene_crossing(void);

extern "C" int port_level_change_poll(void)
{
    if (data_02092110 < 0 && port_scene_crossing_due())
        port_level_scene_crossing();
    if (data_02092110 < 0)
        return 0;
    const int changed = port_level_change_apply();
    port_scene_request_release("the level change has been serviced (or "
                               "declined) and the port spawns no scene for it");
    return changed;
}

/* ---- THE LEVEL-TO-SCENE CROSSING (run linkfull, lane GAMEOVER1) -----------
 *
 * THE REPORT: "When you die with 0 Lives and get a game over, the game simply
 * crashes." Measured on every build before this one (lane BUGS2, R11):
 *
 *     HITDEATHPLANE arg=2 lives=0 -> STARTSCENEFADE scene=8
 *     FATAL: Stage vtable slot 3 (CleanupResources) is not hosted  (0xc0000409)
 *
 * WHAT THE CARTRIDGE DOES, every step a matched TU the port already runs.
 * KillPlayer (src/KillPlayer.c:15-18) and HitDeathPlane (src/HitDeathPlane.c:
 * 14-17) ask for scene 8 through dScene_c::StartSceneFade when no life is left,
 * and ask for NO level. Scene::BeforeBehavior, _ZTV5Stage slot 7, runs the
 * installed fader and, once it is at its end, marks the STAGE for destruction.
 * The next phase-1 pass (func_02043880) moves the Stage onto the cleanup list
 * and marks every child in its scene tree, which is every actor the level
 * spawned; the cleanup Process then runs each one's own CleanupResources --
 * the Stage's slots 4, 3 (Stage::CleanupResources) and 5, whose
 * dScene_c::AfterCleanupResources clears data_02092660, the "a scene has
 * spawned" latch, before fBase_c::AfterCleanupResources destroys the object.
 * With the latch clear and scene 8 pending, Scene::SpawnIfNecessary spawns
 * dScGameOver_c. Its CONTINUE row ends in StartFile(1, 0) (src/func_0202ae74.c)
 * -- a level request for the castle grounds and scene 3 -- and its QUIT row in
 * StartSceneFade(1, 0, 0), the title and its file select, which ends in the
 * same StartFile when a file is picked (src/_ZN9dScDSMT_c8BehaviorEv.cpp).
 *
 * WHAT THE PORT DID: nothing between the mark and the abort. Its level path has
 * no Scene::SpawnIfNecessary (the change poll sits in that seat), so nothing
 * consumed the id, and slot 3 was a deliberate named abort because a torn-down
 * Stage under a still-running frame loop is a walk over freed memory.
 *
 * WHAT THIS DOES: the same steps, with the frame loop held still. It is entered
 * from the change poll -- the ROM's own seat for SpawnIfNecessary -- on the
 * first frame after the ROM marked the Stage, and it
 *   1. pumps the ROM's own phase passes (the scene pass, then the tick whose
 *      phase 4 is the cleanup Process, then the scene pass) until the Stage
 *      has been destroyed by its own teardown. That is where
 *      Stage::CleanupResources runs, _ZTV5Stage slot 3, with the host
 *      adapters hal/stage_bridges.cpp's st_clean carries. Anything still live
 *      afterwards (a host actor the ROM's tree cascade did not reach) goes
 *      through the port's ordinary level teardown, the same as a warp's;
 *   2. runs the scene frames -- the WHOLE host frame, tests/walk_window.cpp's
 *      port_interlude_frame, the one the star select already runs in: input,
 *      the scene tick whose carrier is the ROM's own Scene::SpawnIfNecessary,
 *      the present and the pace -- until a scene hands the game back to a
 *      level: scene 3 asked for, a level request pending, and the outgoing
 *      scene torn down (data_02092660 == 0). Those are the title bridge's own
 *      two words plus the request StartFile writes (hal/title_entry.cpp);
 *   3. returns, and the poll applies that level request the ordinary way. The
 *      apply finds no Stage and builds one (port_stage_create), and the level
 *      boots through the first-entry path the process's first level took.
 *
 * SCOPE, and each exclusion is somebody else's working path:
 *   scene 3 and scene 4 are the level path's own (the level change and the
 *     star-select interlude release or run them before the Stage is marked);
 *   scene 1, the pause and level-clear menus' quit rows, is answered by
 *     tests/walk_window.cpp's port_front_end_quit_poll, which releases the id
 *     before the Stage is marked and relaunches at the front door;
 *   a scene the port does not host is refused here, loudly, and the dispatch
 *     keeps the named slot-3 abort (st_clean only runs the ROM body inside
 *     this crossing): tearing a world down for a spawn the carrier will
 *     decline would strand the session. That is the VS results request
 *     (scene 7, hal/star_flow.cpp, off by default) -- parked, and unchanged.
 *
 * A SCRIPTED RUN HAS A BACKSTOP, a session has none -- the interlude's rule.
 * SM64DS_CROSSING_FRAMES overrides it; SM64DS_CROSSING_SHOT=<f>[,<f>...]
 * writes the scene's two screens at those crossing frames
 * (crossing_f<f>.bmp and crossing_f<f>_subB.bmp in the working directory). */
extern "C" {
int port_scene_is_hosted(int id);            /* hal/scene_boot.cpp */
void *port_scene_live_object(void);         /* hal/scene_boot.cpp */
extern void *data_0209f5bc;                /* the installed fader */
extern int data_0209d4b0[];                /* the fader in motion */
const void *port_scene_framebuffer(void);    /* hal/scene_boot.cpp */
int hal_sub_screen_write_bmp(const char *path);   /* hal/sub_screen.cpp */
}

static int g_crossing_live;       /* the reap pump is running: slot 3 may run
                                     the ROM's own body (hal/stage_bridges.cpp) */
/* g_crossed_scenes (defined above port_level_change_apply): a crossing
   destroyed the Stage and no level has booted since, so the apply builds one */

extern "C" int port_level_crossing_live(void) { return g_crossing_live; }
extern "C" int port_level_crossed_scenes(void) { return g_crossed_scenes; }

static int port_scene_crossing_due(void)
{
    if (data_02092110 >= 0)
        return 0;                  /* a level change: the apply owns it */
    const unsigned id = data_02092664;
    if (id == 0x187 || id == 1 || id == 3 || id == 4)
        return 0;                  /* nothing, or a path above owns it */
    void *st = port_stage_object();
    if (!st)
        return 0;
    /* MARKED, or already moved onto the cleanup list. fBase_c::shouldBeKilled
       (+0x0f) is set by MarkForDestruction in the tick, and the frame's own
       phase-1 pass (func_02043880) clears it again as it moves the Stage onto
       the cleanup list with aliveState (+0x0e) = 2 -- so by the next frame's
       poll it is the second word that says so. */
    const unsigned char killed = *(unsigned char *)((char *)st + 0x0f);
    const unsigned char alive = *(unsigned char *)((char *)st + 0x0e);
    if (killed == 0 && alive != 2)
        return 0;                  /* the ROM has not marked the Stage yet
                                      (its fade is still running) */
    if (!port_scene_is_hosted((int)id)) {
        static unsigned said = 0xffff;
        if (said != id) {
            said = id;
            std::fprintf(stderr, "[cross] the ROM marked the Stage for scene %u, "
                         "which this build does not host: NOT crossing (the "
                         "spawn would be declined and the session stranded); "
                         "the Stage's teardown stops at the named slot-3 "
                         "abort as before\n", id);
        }
        return 0;
    }
    return 1;
}

namespace ntr { struct Framebuffer;
                bool ppu_write_bmp(const char *path, const Framebuffer &fb); }

static void port_crossing_shot(int f)
{
    static const char *spec = (const char *)1;
    if (spec == (const char *)1)
        spec = std::getenv("SM64DS_CROSSING_SHOT");
    if (!spec)
        return;
    for (const char *q = spec; *q;) {
        char *end;
        const long v = std::strtol(q, &end, 10);
        if (end == q)
            break;
        if (v == f) {
            char a[64], b[64];
            std::snprintf(a, sizeof a, "crossing_f%d.bmp", f);
            std::snprintf(b, sizeof b, "crossing_f%d_subB.bmp", f);
            const bool wa = ntr::ppu_write_bmp(
                a, *(const ntr::Framebuffer *)port_scene_framebuffer());
            const int wb = hal_sub_screen_write_bmp(b);
            std::fprintf(stderr, "[cross] f%d wrote %s (%s) and %s (%s)\n", f,
                         a, wa ? "ok" : "FAILED", b, wb ? "ok" : "FAILED");
            /* the display words a picture is made of, so a capture that looks
               wrong can be told apart from a layer that is merely faded */
            std::fprintf(stderr, "[cross] f%d DISPCNT %08x/%08x BLDCNT %04x/%04x "
                         "BLDY %04x/%04x MASTER_BRIGHT %04x/%04x\n", f,
                         *(volatile unsigned *)0x04000000,
                         *(volatile unsigned *)0x04001000,
                         *(volatile unsigned short *)0x04000050,
                         *(volatile unsigned short *)0x04001050,
                         *(volatile unsigned short *)0x04000054,
                         *(volatile unsigned short *)0x04001054,
                         *(volatile unsigned short *)0x0400006c,
                         *(volatile unsigned short *)0x0400106c);
            {
                volatile unsigned char *v = (volatile unsigned char *)0x04000240;
                std::fprintf(stderr, "[cross] f%d VRAMCNT A-I %02x %02x %02x %02x "
                             "%02x %02x %02x %02x %02x POWCNT1 %04x BG0-3CNT "
                             "%04x %04x %04x %04x / %04x %04x %04x %04x\n", f,
                             v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[8],
                             v[9], *(volatile unsigned short *)0x04000304,
                             *(volatile unsigned short *)0x04000008,
                             *(volatile unsigned short *)0x0400000a,
                             *(volatile unsigned short *)0x0400000c,
                             *(volatile unsigned short *)0x0400000e,
                             *(volatile unsigned short *)0x04001008,
                             *(volatile unsigned short *)0x0400100a,
                             *(volatile unsigned short *)0x0400100c,
                             *(volatile unsigned short *)0x0400100e);
            }
            {
                const unsigned char *o =
                    (const unsigned char *)port_scene_live_object();
                const int *fd = (const int *)data_0209f5bc;
                std::fprintf(stderr, "[cross] f%d scene %p alive %u kill %u "
                             "flags %02x state %02x/%02x/%02x/%02x/%02x | fader "
                             "%p vt %p interp %d speed %d | animating %p\n", f,
                             (const void *)o, o ? o[0x0e] : 0, o ? o[0x0f] : 0,
                             o ? o[0x13] : 0, o ? o[0x90] : 0, o ? o[0x91] : 0,
                             o ? o[0x92] : 0, o ? o[0x93] : 0, o ? o[0x94] : 0,
                             (const void *)fd, fd ? (void *)(size_t)fd[0] : 0,
                             fd ? fd[1] : 0, fd ? fd[2] : 0,
                             (void *)(size_t)data_0209d4b0[0]);
            }
            return;
        }
        q = *end == ',' ? end + 1 : end;
    }
}

static int port_level_scene_crossing(void)
{
    const unsigned want = data_02092664;
    void *stage = port_stage_object();
    const int live0 = port_level_live_count();
    std::fprintf(stderr, "[cross] the ROM marked the Stage %p for destruction "
                 "with scene %u pending and no level change (level %d, %d live "
                 "actor(s)): crossing to the scene in this process\n", stage,
                 want, (int)data_0209f2f8, live0);

    /* 1. THE ROM'S OWN TEARDOWN, pumped. */
    g_crossing_live = 1;
    int k = 0;
    for (; k < 32 && port_stage_object(); ++k) {
        port_actor_scene_pass();
        port_actor_tick();
        port_actor_scene_pass();
    }
    g_crossing_live = 0;
    if (port_stage_object()) {
        std::fprintf(stderr, "FATAL: [cross] the Stage %p was not destroyed by "
                     "its own teardown after %d rounds (frozen by the "
                     "quarantine, or a cleanup that never finished); the "
                     "level cannot be left for scene %u\n",
                     port_stage_object(), k, want);
        std::fflush(stderr);
        std::abort();
    }
    g_crossed_scenes = 1;
    int left = port_level_live_count();
    std::fprintf(stderr, "[cross] the Stage's own teardown ran in %d round(s): "
                 "Stage destroyed, %d live actor(s) left, scene tree head %p, "
                 "spawned latch %u, pending scene %u\n", k, left,
                 (void *)(size_t)data_020a4b6c[0], (unsigned)data_02092660,
                 (unsigned)data_02092664);
    if (left || data_020a4b6c[0]) {
        const int ok = port_level_teardown();
        std::fprintf(stderr, "[cross] the port's own teardown took the %d the "
                     "ROM's cascade did not reach: %s, scene tree head %p\n",
                     left, ok ? "converged" : "DID NOT CONVERGE",
                     (void *)(size_t)data_020a4b6c[0]);
    }

    /* 2. THE SCENE, in the whole host frame, until it hands back a level. */
    const int selftest = std::getenv("SM64DS_WINDOW_SELFTEST") != 0;
    int cap = selftest ? 1800 : 0;
    if (const char *e = std::getenv("SM64DS_CROSSING_FRAMES"))
        cap = std::atoi(e);
    if (cap > 0)
        std::fprintf(stderr, "[cross] BACKSTOP ARMED: a scripted run gives the "
                     "scene %d frames. That is a harness rule, not the game's: "
                     "a session has no cap.\n", cap);
    unsigned was_pending = 0xffffu, was_latch = 0xffu;
    int was_level = -2;
    int f = 0, closed = 0, handed = 0;
    for (; cap <= 0 || f < cap; ++f) {
        if (port_interlude_frame_hook) {
            if (port_interlude_frame_hook(f)) {
                closed = 1;
                break;
            }
        } else {
            port_scene_tick(f, 1);
        }
        port_crossing_shot(f);
        if (data_02092664 != was_pending || data_02092660 != was_latch ||
            (int)data_02092110 != was_level) {
            was_pending = data_02092664;
            was_latch = data_02092660;
            was_level = (int)data_02092110;
            std::fprintf(stderr, "[cross] f%d pending scene %u, spawned latch "
                         "%u, level request %d\n", f, was_pending, was_latch,
                         was_level);
        }
        if (data_02092664 == 3 && data_02092660 == 0 && data_02092110 >= 0) {
            handed = 1;
            break;
        }
    }
    if (handed) {
        std::fprintf(stderr, "[cross] the scene handed the game back at crossing "
                     "frame %d: level %d, entrance %u (the ROM's own StartFile / "
                     "LoadLevelNoReturn); the level boots into a new Stage\n", f,
                     (int)data_02092110, (unsigned)data_0209f268);
        return 1;
    }
    std::fprintf(stderr, "[cross] %s at crossing frame %d (pending scene %u, "
                 "latch %u, level request %d): no level to return to, so the "
                 "session ends here\n",
                 closed ? "the window was closed"
                        : "the scripted-run backstop fired",
                 f, (unsigned)data_02092664, (unsigned)data_02092660,
                 (int)data_02092110);
    std::fflush(stderr);
    std::fflush(stdout);
    std::exit(0);
}

/* ---- the front door -------------------------------------------------------
   dScTitle_c's own selection table, and its own handoff call.
   _ZN10dScTitle_c8BehaviorEv (the debug level select's Behavior) picks a row out of
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
void _ZN8dScene_c14StartSceneFadeEjjt(unsigned actorID, unsigned param,
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
       -> dScStarSel_c_classInit  (ov003, 0x020b04f0)

   dScStarSel_c_classInit (src/d_s_star_sel.cpp) is small and portable-shaped:
     - ActorBase::operator new(0x13c), ActorBase ctor
     - vptr = data_ov003_020b1704   (the dScStarSel_c vtable, IN ov003)
     - flags +0x13 |= 1|4
     - __cxa_vec_ctor(self+0x64, 2, 0x50, Model::ctor, Model::dtor)  two Models

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
   the frame loop is the seam a real dScStarSel_c_classInit registration would plug
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
 *         src/_ZN8dScene_c15SetSceneToSpawnEjj.cpp
 *     Scene::SpawnIfNecessary()           spawn the scene, THEN
 *                                         data_02092664 = 0x187
 *         src/_ZN8dScene_c16SpawnIfNecessaryEv.cpp
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

/* The else-branch of _ZN10dScTitle_c8BehaviorEv, now in the ROM's OWN order.
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

    /* dScTitle_c::Behavior's confirm branch, in order (_ZN10dScTitle_c8BehaviorEv):
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
    _ZN8dScene_c14StartSceneFadeEjjt(4, 0, 0);
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
