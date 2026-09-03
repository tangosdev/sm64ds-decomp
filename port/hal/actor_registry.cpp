// The actor registry: which classes the port can build, and the gate that
// turns the rest away.
//
// ---- what the ROM reads ----------------------------------------------------
//
// func_02043098 is the spawn spine and it consults exactly two globals.
// data_020a4bb8[actorID] is a SpawnInfo*: a factory function pointer at +0
// that the spine CALLS, and two halfwords at +4/+6 that the ActorBase
// constructor reads back out of the same record and drops into the two
// processing-list nodes it just built. A null slot is a call through zero, so
// the table cannot be left empty for an id the level names.
//
// data_020a4b58 is the pre-spawn hook. func_02043060 calls it with the actor
// id BEFORE the table is touched and 3 means "abort, cleanly" -- an exercised
// ROM path, since LoadEntranceObjects already stores a possibly-null result.
// So the gate is: registered ids proceed, everything else is named once and
// skipped. A level's unhosted classes come out as a list rather than a crash.
//
// ---- the SpawnInfo records are Nintendo's ----------------------------------
//
// Every record here is the ROM's own, mounted through port/tools/ovdata.py,
// with ONLY the factory word repointed at the host. The two priority
// halfwords are what the ActorBase constructor re-reads, and inventing them
// would put the actor in the wrong place in both processing lists. The ROM
// also happens to store the actor's own id in the +4 halfword, which makes a
// free cross-check: if the record's id and the table's id disagree, the
// registry is pointing at the wrong bytes and says so.
//
// ---- the per-frame processing lists ---------------------------------------
//
// func_02044120 is the ROM's whole actor frame and it is five list walks:
//
//     phase 4  data_020a4ba8 -> func_020432e4   cleanup Process   (slots 3/4/5)
//     phase 2  data_020a4b88 -> func_0204335c   init Process      (slots 0/1/2)
//     phase 3  data_020a4b78 -> func_02043288   behaviour Process (slots 6/7/8)
//     phase 5  data_020a4b98 -> func_0204322c   render Process    (slots 9/10/11)
//     phase 1  data_020a4b6c -> func_02043880   scene-tree housekeeping
//
// read straight out of the five PMF pairs __sinit_02075154 copies into the
// list heads (arm9 0x02099f48/50/60/68/70; each is {function, 0}, a plain
// nonvirtual pointer-to-member). The port seats the same five functions in
// the same slots and walks the lists with host copies of func_02043fdc /
// func_020441cc, because MSVC has no representation for an mwcc PMF -- the
// same treatment func_0204335c and func_02043288 already have.
//
// The frame is SPLIT here rather than driven through one func_02044120 call
// for one reason: the render pass has to run inside the host's render frame,
// after gx_reset and the camera push. Phases 4/2/3 are the tick, phase 5 is
// the render bucket, phase 1 closes the frame. Same functions, same order.
#include <cstdio>
#include <cstdlib>
#include <cstring>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

extern "C" {

/* storage: hal/actor_vtables.cpp (actorID -> SpawnInfo*) */
extern void **data_020a4bb8;
/* storage: hal/player_bridges.cpp (the pre-spawn hook slot) */
extern int data_020a4b58[4];

/* the five processing lists. data_020a4b6c is the scene tree (three words:
   head, callback pair); the other four are {head, tail, callback pair}. */
extern int data_020a4b6c[8];
extern int data_020a4b78[8];
extern int data_020a4b88[8];
extern int data_020a4b98[8];
extern int data_020a4ba8[8];
extern int data_02099f24[4];       /* the current phase */

/* the actor freeze mask. b454 is the persistent freeze-REQUEST word (storage:
   hal/auto_bss.cpp); b464 is this frame's copy of it and the only word
   Actor::BeforeBehavior reads (storage: hal/actor_vtables.cpp). Latched at the
   head of phase 3 below -- see the long note there. */
extern int data_0209b454[8];
extern unsigned int data_0209b464;

/* the four Process wrappers (host copies in port/unmatched/) and the
   scene-tree housekeeping pass (matched src) */
int func_0204335c(void *self);
int func_02043288(void *self);
int func_0204322c(void *self);
int func_020432e4(void *self);
int func_02043880(void *self);
void *func_02043fdc(void *list);
void *func_020441cc(void *list);

}  /* extern "C" */

// ---- the class table -------------------------------------------------------
//
// One row per class the port can build. `info` is the mounted ROM SpawnInfo,
// `factory` the matched <Class>_Spawn compiled for the host, and `fill` the
// runtime vtable fill (the vtable law: MSVC slot order, host __fastcall
// thunks, everything else trapped by name).
struct PortActorClass {
    unsigned short id;
    const char *name;
    unsigned char *info;
    void *(*factory)(void);
    void (*fill)(void);
};

extern "C" {
extern unsigned char Player_SpawnInfo[];      /* ov002 0x0210a704 */
extern unsigned char Camera_SpawnInfo[];      /* arm9  0x02086d78 */
void *_ZN6PlayerC3Ev(void);                   /* ov002 0x020e6c0c */
void *_ZN6CameraC1Ev(void *);                 /* arm9  0x0200e444 */
void hal_fill_player_vtable(void);
void hal_fill_camera_vtable(void);
}

/* Both ROM factories allocate their own object and ignore the argument
   register they were entered with, which is why they can be called through a
   no-argument pointer at all. */
static void *port_factory_player(void) { return _ZN6PlayerC3Ev(); }
static void *port_factory_camera(void) { return _ZN6CameraC1Ev(0); }
/* The bottom screen's classes put the CONSTRUCTOR in the SpawnInfo's +0 word
   rather than a separate Spawn veneer -- it allocates and returns the object
   itself, which is all the spine asks of a factory. */
extern "C" int *_ZN3HUDC1Ev(void);
static void *port_factory_hud(void) { return _ZN3HUDC1Ev(); }
extern "C" int *_ZN7MinimapC1Ev(void);
static void *port_factory_minimap(void) { return _ZN7MinimapC1Ev(); }

#include "actor_classes.inc"

static const PortActorClass port_actor_classes[] = {
    {0x0bf, "PLAYER", Player_SpawnInfo, port_factory_player,
     hal_fill_player_vtable},
    /* The camera's vtable fill is the gate-13 seam and runs from the same
       place the layout check does; the registry only needs its factory. */
    {0x14c, "CAMERA", Camera_SpawnInfo, port_factory_camera, 0},
    PORT_ACTOR_CLASS_ROWS
    {0, 0, 0, 0, 0},
};

// ---- the census ------------------------------------------------------------
//
// The boot is one pass over the level's own object tables, so what it spawned
// is a fact worth reading back rather than inferring. Two counters per id,
// filled by the gate itself.
enum { PORT_ACTOR_IDS = 512 };
static unsigned short g_spawned[PORT_ACTOR_IDS];
static unsigned short g_skipped[PORT_ACTOR_IDS];

/* The counters are per-BOOT, not per-run. A level change tears the previous
   level's actors down and the next boot spawns its own; carrying the counts
   across meant a warped-into level's census read the sum of both, which is
   both harder to compare against a direct boot and wrong as a statement about
   "what this level spawned". hal/level_change.cpp zeroes them in its teardown,
   right where it drops the other per-level host state. */
extern "C" void port_actor_census_reset(void)
{
    std::memset(g_spawned, 0, sizeof g_spawned);
    std::memset(g_skipped, 0, sizeof g_skipped);
}

/* ---- host-ABI per-level spawn blockers ------------------------------------
   A class can be hosted, registered and fault-free on one level and hit a host
   translation limit on another because the level's object table hands it a
   different tuning param. That is a real blocker, not a missing class, so it is
   named here rather than left to fault: the gate turns the id away on the
   listed levels exactly the way it turns away an unregistered class, and the
   census reports it as skipped so the boot is honest about what did not spawn.

   The PAINTING (daPicGate_c, 307, ov080) is the one case so far. Its
   InitResources/Behavior/Render dispatch a per-mode state through a
   pointer-to-member of an INCOMPLETE class; mwccarm makes that PMF 8 bytes and
   the recovered Disp/Entry stride lands on the ROM's 8-byte {fn,delta} records,
   but MSVC widens it to the 16-byte generic form, so &data_ov080_02128628[mi]
   strides wrong and the call shifts `this` into unmapped memory. Level 2's six
   paintings all carry mode 0 and survived by luck of the layout; level 4
   (castle_b1) and level 5 (castle_2f) carry mode>0 / high-texture params that
   fault one call into the state function (measured: a write to this+0x16c with
   this = 0xfffad16c). Hosting the three dispatchers is the fix, deferred; until
   then the painting spawns on levels 1..3 and is skipped on 4/5.

   Level 13 (Hazy Maze Cave, cave) carries a PAINTING too, and it faults on the
   same PMF-stride path: measured, the id-0x133 spawn dispatches one call into
   the mis-strided state function and writes through a `this` shifted into
   unmapped memory (the identical func_0203b27c teardown fault at this+0x1b8 the
   levels 4/5 paintings hit). Same blocker, same fix pending, so it joins the
   list; the cave boots and is walkable with the painting skipped and named in
   the census. */
extern "C" int port_level_id(void);
static int port_host_abi_blocked(unsigned id)
{
    /* The PAINTING's levels-4/5/13 skip came off 2026-08-08: the mis-strided
       PMF dispatch behind those faults is hosted in
       port/unmatched/Painting_Dispatch.cpp now (see the long note above),
       and all three levels boot their paintings through it. */

    /* run linkw wave 7 (lane w7-c): BOWSER_SKY_PLATFORM (167) on LEVEL 40, the
       Bowser in the Sky arena -- the second case this hook exists for, and it
       is a HOST BOOT-ORDER limit rather than a class defect. Level 40 is the
       only level in all fifty-two that places id 167, so the class had never
       actually spawned until this lane mounted the arena; it is fault-free
       everywhere it is reachable, which is nowhere else.

       MEASURED, not reasoned. Its InitResources (src/func_ov060_021182b0.cpp,
       daKpa3Bg_c::InitResources) calls CopyTexPalFromLevelModel(this + 0xdc),
       and src/CopyTexPalFromLevelModel.c opens on

           struct Entry* sbase = data_0209f320->entries;

       -- a load through data_0209f320 + 4. data_0209f320 is the Stage's
       ModelComponents pointer, and the only src/ writer that SEATS it is
       Stage::LoadModel (src/_ZN5Stage9LoadModelEv.cpp, last line);
       Stage::CleanupResources clears it back to 0
       (src/_ZN5Stage16CleanupResourcesEv.cpp:92). The fault caught under
       SM64DS_FAULTS_FATAL is exactly that load:

           FAULT code c0000005 at +0x0008e07f accessing 00000004, eax=00000000
           _CopyTexPalFromLevelModel+0xf
             <- _func_ov060_021182b0+0x44  <- ?skyplat_init  <- Actor::Spawn
             <- LoadStandardObjects <- LoadObjects <- Stage::LoadClsnAndObjects
             <- _port_stage_a_boot+0x211  <- _main+0x797

       THE ROM RUNS THE TWO THE OTHER WAY ROUND. Stage::InitResources
       (src/_ZN5Stage13InitResourcesEv.cpp) is
           :361  Stage::LoadModel(thiz);
           :363  Stage::LoadClsnAndObjects(...);
       so on the DS data_0209f320 is seated before any actor initialises. The
       host harness inverts them: port/tests/walk_window.cpp calls
       port_stage_a_boot (which runs LoadClsnAndObjects) before it calls
       Stage::LoadModel -- confirmed on this tree by print order, not by
       reading (level 36's log has [census] ahead of "level model loaded by
       Stage::LoadModel ... components 3003AF24"). The level-change path has
       the same inversion. Line numbers are deliberately not cited here: they
       were :2099/:2265/:4382 when this was written and :2390/:2556/:4735 by
       the time it merged. Search for port_stage_a_boot and Stage::LoadModel
       in port/tests/walk_window.cpp; the ORDER is the durable fact.

       AND IT CANNOT BE FIXED FROM port_stage_a_boot. Calling Stage::LoadModel
       early there would leave the harness's own call as a SECOND one, and a
       second Stage::LoadModel is not idempotent: LoadAndSetFile -> the port's
       cached LoadFile(handle) -> func_02016ff4, which calls
       Model::UpdateFileOffsets UNCONDITIONALLY, and that function rebases the
       BMD's file-relative offsets IN PLACE (`file.bones = REBASE(file.bones)`,
       src/_ZN5Model17UpdateFileOffsetsER8BMD_File.cpp). A duplicate pass adds
       the base a second time and sends every pointer in the level model out of
       the file -- on EVERY level, not just this one. So the fix is one line in
       port/tests/walk_window.cpp (hoist the LoadModel call above the boot, the
       ROM's own order), and that file is reserved to another owner.

       Until then id 167 is declined ON LEVEL 40 ONLY. The arena boots, exits 0
       under FAULTS_FATAL, and the census names the ten skipped platforms rather
       than the port faking them. Nothing else changes: no other level places
       167, and the class's registry row, vtable fill and ov060 mount are
       untouched.

       The scope of that "no other level" is worth stating exactly, because a
       review re-derived it: of the 52 rows, 8 (levels 0, 29, 30, 32, 34, 41,
       42, 46) point into data-only overlays whose data_02092208 word does not
       decode as a LVL_Overlay at all, so the sweep covers the 44 that do
       decode. Across those 44, id 167 appears only on level 40 (x10) and id
       301 only on level 36 (x16).

       THE DECLINE IS LIFTED (run link60, lane SL0), because the fix it names
       has landed. Stage::LoadModel now runs inside port_stage_boot_body ahead
       of Stage::LoadClsnAndObjects, which is the order Stage::InitResources
       has it in, so data_0209f320 is seated before the object pass reaches
       CopyTexPalFromLevelModel. Both of port/tests/walk_window.cpp's calls
       were REMOVED rather than one being added, so the duplicate-rebase the
       paragraph above worried about cannot happen: there is exactly one
       Stage::LoadModel per boot and one rebase per loaded buffer.

       One correction to that paragraph while it is being answered. It reads
       the duplicate-rebase risk onto "the port's cached LoadFile", meaning
       hal/fs.cpp. That cache is not the hazard: fs_hand_out memcpys pristine
       bytes into a fresh allocation on every call, so it never hands back a
       rebased buffer. The sharing was in hal/level_boot.cpp's own
       LoadFile(handle) handle table, which returned the same filePtr for a
       repeat handle inside one level. Same conclusion, different file.

       AND THAT SHARING IS GONE, run link60 lane LF1: LoadFile loads a fresh
       block on every call now, the ROM's own contract, so no repeat request
       can hand back a buffer another caller has already rebased or freed.

       DEVIATION, DECLARED: hal/actor_registry.cpp is on neither this lane's
       owned list nor its do-not-touch list. The lane took this one line
       because it is the line this file asked for by name, and lifting it is
       what makes the hoist observable. Nothing else here is touched. */

    return 0;
}

extern "C" const char *port_actor_class_name(unsigned id);

extern "C" int port_prespawn_hook(void *idv)
{
    unsigned id = (unsigned)(size_t)idv;
    if (id < PORT_ACTOR_IDS && data_020a4bb8[id] && !port_host_abi_blocked(id)) {
        static int trc = -1;
        if (trc < 0) trc = std::getenv("SM64DS_TRACE_SPAWN") != 0;
        if (trc) std::fprintf(stderr, "  [spawn+] actor 0x%x %s proceed\n",
                              id, port_actor_class_name(id));
        ++g_spawned[id];
        return 2;                     /* what a null hook returns: proceed */
    }
    if (id < PORT_ACTOR_IDS) {
        if (!g_skipped[id]) {
            std::printf("  [spawn] actor 0x%x %s, skipped\n", id,
                        data_020a4bb8[id] ? "host-ABI blocked on this level"
                                          : "not registered");
            /* stderr too: playlogs capture stderr, and a silent decline here is
               how the rabbit-key grant (id 229) went missing without a trace.
               Keep the decline quiet-but-VISIBLE in every real-play log. */
            std::fprintf(stderr, "  [spawn-declined] actor 0x%x %s\n", id,
                         data_020a4bb8[id] ? "host-ABI blocked on this level"
                                           : "not registered");
        }
        ++g_skipped[id];
    }
    return 3;
}

/* Name an actor id for a diagnostic; the trap in actor_classes.cpp is the
   customer. Never returns null so it can sit inside a printf. */
extern "C" const char *port_actor_class_name(unsigned id)
{
    for (const PortActorClass *k = port_actor_classes; k->name; ++k)
        if (k->id == id)
            return k->name;
    return "?";
}

extern "C" void port_actor_census(void)
{
    int spawned = 0, skipped = 0, kinds = 0, skipkinds = 0;
    for (int i = 0; i < PORT_ACTOR_IDS; ++i) {
        spawned += g_spawned[i];
        skipped += g_skipped[i];
        kinds += g_spawned[i] != 0;
        skipkinds += g_skipped[i] != 0;
    }
    std::printf("[census] %d spawned (%d classes), %d skipped (%d classes)\n",
                spawned, kinds, skipped, skipkinds);
    for (int i = 0; i < PORT_ACTOR_IDS; ++i)
        if (g_spawned[i]) {
            const char *nm = "?";
            for (const PortActorClass *k = port_actor_classes; k->name; ++k)
                if (k->id == i) nm = k->name;
            std::printf("         + %3d x%-3u %s\n", i, g_spawned[i], nm);
        }
    for (int i = 0; i < PORT_ACTOR_IDS; ++i)
        if (g_skipped[i])
            std::printf("         - %3d x%-3u\n", i, g_skipped[i]);
}

// ---- installation ----------------------------------------------------------
extern "C" void hal_fill_moving_cylinder_vtables(void);

/* fault_probe.h's rich dump and the quarantine walker resolve actor ids to
   class names through this weak pointer; wire it to the real table here (the one
   TU that has both). Null in a bare smoke, where the dump prints "?". */
typedef const char *(*port_classname_fn)(unsigned id);
extern "C" port_classname_fn port_classname_resolver;

extern "C" void port_actor_registry_install(void)
{
    int n = 0;
    port_classname_resolver = port_actor_class_name;
    /* The two actor-attached CYLINDER tables, before any class fills its own.
       They belong to no single actor -- MovingCylinderClsn and its WithPos
       child are shared bases that a dozen classes construct -- so they are
       seated here rather than hung off whichever class happens to register
       first. CylinderClsn::Process dispatches GetPos and GetOwnerID on every
       node of data_0209cee8, and an unfilled table there is a call through
       null on the first frame an actor cylinder is threaded. */
    hal_fill_moving_cylinder_vtables();
    /* SM64DS_SKIP_CLASS=NAME[,NAME...] leaves a class unregistered, so the
       spine's own gate turns it away and the level boots without it. The
       cheapest way to ask "is this class the one breaking the run" -- it is
       how the HUD was pinned as the owner of a fault three phases downstream
       of it. Substring match, so SM64DS_SKIP_CLASS=HUD,BIRD works. */
    const char *skip = std::getenv("SM64DS_SKIP_CLASS");
    for (const PortActorClass *k = port_actor_classes; k->name; ++k) {
        if (skip && std::strstr(skip, k->name)) {
            std::printf("  [reg] %s skipped by SM64DS_SKIP_CLASS\n", k->name);
            continue;
        }
        if (!k->info || !k->factory) {
            std::fprintf(stderr, "  [reg] %s has no SpawnInfo or factory\n",
                         k->name);
            continue;
        }
        /* The ROM stores the actor's own id in the record's +4 halfword. A
           mismatch means the mount is pointing at the wrong bytes, which
           would otherwise show up as two silently wrong list priorities. */
        {
            unsigned rec = *(unsigned short *)(k->info + 4);
            if (rec != k->id)
                std::fprintf(stderr, "  [reg] %s: SpawnInfo at %p says id %u, "
                             "registry says %u -- WRONG RECORD\n", k->name,
                             (void *)k->info, rec, k->id);
        }
        *(void **)(k->info + 0) = (void *)k->factory;
        data_020a4bb8[k->id] = k->info;
        if (k->fill) k->fill();
        ++n;
    }
    data_020a4b58[0] = (int)(size_t)port_prespawn_hook;
    std::printf("[reg] %d actor classes registered, the gate is armed\n", n);
}

// ---- the frame -------------------------------------------------------------
//
// Seat the five list callbacks the way __sinit_02075154 does, with the host
// copies of the Process wrappers in place of the ROM's PMFs. The list heads
// are the port's own zeroed storage, so the two words the ROM's sinit copies
// out of data_02099f48..70 are written directly here.
typedef int (*PortListFn)(void *);

extern "C" void port_actor_lists_seat(void)
{
    /* {head, callback, 0} -- the scene tree, walked by func_020441cc */
    data_020a4b6c[1] = (int)(size_t)(PortListFn)func_02043880;
    data_020a4b6c[2] = 0;
    /* {head, tail, callback, 0} -- the four walked by func_02043fdc */
    data_020a4b88[2] = (int)(size_t)(PortListFn)func_0204335c;
    data_020a4b88[3] = 0;
    data_020a4b78[2] = (int)(size_t)(PortListFn)func_02043288;
    data_020a4b78[3] = 0;
    data_020a4b98[2] = (int)(size_t)(PortListFn)func_0204322c;
    data_020a4b98[3] = 0;
    data_020a4ba8[2] = (int)(size_t)(PortListFn)func_020432e4;
    data_020a4ba8[3] = 0;
}

/* Phases 4, 2 and 3 of func_02044120: cleanup, the init pass for anything
   spawned since the last frame, then behaviour. */
/* SM64DS_TRACE_LISTS=1: name every node on a list before it is walked --
   actor id, alive state, kill flag and vtable. The only window into a frame
   that is otherwise entirely matched code walking Nintendo's own structures. */
static void port_list_trace(const char *name, int *list)
{
    static int on = -1;
    if (on < 0) on = std::getenv("SM64DS_TRACE_LISTS") != 0;
    if (!on)
        return;
    std::printf("  [list] %s head %08x:", name, list[0]);
    for (int *n = (int *)(size_t)list[0]; n; n = (int *)(size_t)n[1]) {
        char *o = (char *)(size_t)n[2];
        std::printf(" {node %p actor %p id %u alive %u kill %u vt %p}", (void *)n,
                    (void *)o, o ? *(unsigned short *)(o + 0xc) : 0u,
                    o ? *(unsigned char *)(o + 0xe) : 0u,
                    o ? *(unsigned char *)(o + 0xf) : 0u,
                    o ? *(void **)o : (void *)0);
    }
    std::printf("\n");
}

/* GATE 33's read-back: with SM64DS_SPAWN_ACTOR set, report every sixty frames
   how many of each named id are still on the behaviour list and what the coin
   counter says. The SPAWNING is port-beta-lvl's port_debug_spawn_env, called
   from the boot; this only follows what it made. A no-op with the variable
   unset. */
extern "C" void port_bob_debug_watch(void);

/* ---- SM64DS_POUND_PROBE: the ground-pound dispatch, on demand --------------
   SM64DS_POUND_PROBE=<classId>[:<frame>] runs ONE real ground-pound hit on the
   first live actor of that class, through the ROM's own hit path, on the given
   frame (default 240). Unset, nothing below runs and nothing below is reached.

   The path is func_ov002_020ef2a4, which Player::St_GroundPound_Main calls with
   the player's WithMeshClsn and the player: it asks the collider whether the
   player is on the ground, reads the floor ClsnResult's ClsnID, resolves that
   id with Actor::FindWithID and dispatches vtable slot 21, OnGroundPounded, on
   what it finds. Everything except the collider is the live game: the actor is
   a real spawn with its real vtable, the argument is the real Player, and the
   dispatch is the ROM function's own `call dword ptr [edx+0x54]`.

   The collider is a zeroed scratch record, not the player's, so the probe can
   name the target without having to stand on it: set the on-ground bit the
   accessor reads and write the target's unique id where the floor result keeps
   its ClsnID. The unique id is found by asking Actor::FindWithID, so no field
   offset is assumed here. */
extern "C" {
int _ZNK12WithMeshClsn10IsOnGroundEv(void *c);
void *_ZNK12WithMeshClsn14GetFloorResultEv(void *c);
void *_ZN5Actor10FindWithIDEj(unsigned id);
int func_ov002_020ef2a4(void *clsn, void *arg);
int func_ov002_020eee3c(void *clsn, void *arg);
int _ZNK12WithMeshClsn8IsOnWallEv(void *c);
void *_ZNK12WithMeshClsn13GetWallResultEv(void *c);
/* the CEILING third of the same family, for SM64DS_POUND_PROBE=<id>:<f>:28.
   func_ov002_020eeca8 is func_ov002_020ef2a4 with the hit-ceiling accessors in
   place of the floor pair and slot 28, OnHitFromUnderneath, in place of slot
   21 (`a->m70(arg)`, vtable+0x70). It is the ROM function the player's ceiling
   collision runs; the head-bonk raycast (unmatched/Player_HeadBonk.cpp) is the
   other site and reaches the same slot with the same __thiscall contract.
   func_02035638 is the hit-ceiling accessor (`c[0x90] & 0x10`) and
   func_0203567c the ceiling ClsnResult (`c + 0x30`) -- both matched src, so
   the flag bit and the record offset below are read off the ROM's own
   accessors rather than assumed. */
int func_ov002_020eeca8(void *clsn, void *arg);
int func_02035638(void *c);
void *func_0203567c(void *c);
unsigned _ZNK10ClsnResult9GetClsnIDEv(void *r);
/* the block's own refusal test and the closest-player cache it consults */
int func_ov102_02149078(void *self);
void *_ZN5Actor13ClosestPlayerEv(void *self);
extern signed char data_0209f2f8;      /* current level */
extern void *data_0209b458;            /* the closest-player cache itself */
}

/* Gate 203's read-back. The QuestionBlock/ExclamationBlock keeps its state
   index at +0x3e8 and the character that hit it at +0x3f2; state 1 is the
   bounce and state 2 is spent. Printing those two across the hit is what
   distinguishes "the box opened" from "the frame did not crash". */
static void pp_qblock_state(const char *when, void *o)
{
    if (!o)
        return;
    /* +0x9c and +0xa8 are the recoil the slot-28 body writes BEFORE it
       consults the refusal test, so they separate "the body never ran" from
       "the body ran and was refused" without guessing which. */
    std::fprintf(stderr, "[pound]   %-6s block state %d  ch %u  recoil "
                 "%08x/%08x\n", when,
                 *(int *)((char *)o + 0x3e8),
                 *(unsigned char *)((char *)o + 0x3f2),
                 *(unsigned *)((char *)o + 0x9c),
                 *(unsigned *)((char *)o + 0xa8));
    std::fflush(stderr);
}

/* After the hit, follow the block for a while: entering state 1 only proves
   the trigger fired, and the payout happens when the bounce runs out and
   func_ov102_021498e0 spawns the content and parks it in state 2. */
static void *g_qb_watch;
static int g_qb_watch_left;

static void pp_qblock_follow(void)
{
    static int last = -99;
    if (!g_qb_watch || g_qb_watch_left <= 0)
        return;
    --g_qb_watch_left;
    int st = *(int *)((char *)g_qb_watch + 0x3e8);
    if (st != last) {
        last = st;
        std::fprintf(stderr, "[pound]   watch  block state -> %d (%d frames "
                     "left)\n", st, g_qb_watch_left);
        std::fflush(stderr);
    }
    if (g_qb_watch_left == 0) {
        std::fprintf(stderr, "[pound]   watch  FINAL block state %d\n", st);
        std::fflush(stderr);
        g_qb_watch = 0;
    }
}

static void *pp_first_of_class(unsigned id)
{
    int n = 0;
    for (int *node = (int *)(size_t)data_020a4b78[0]; node && n++ < 4096;
         node = (int *)(size_t)node[1]) {
        char *o = (char *)(size_t)node[2];
        if (o && *(unsigned short *)(o + 0xc) == (unsigned short)id)
            return o;
    }
    return 0;
}

/* TEMP repro helper (SM64DS_YOSHI_EGG_REPRO in hal/player_bridges.cpp): the
   first live actor of a class, so the egg-lay driver targets a real, currently
   constructed enemy rather than a stale boot-time pointer. Inert otherwise. */
extern "C" void *port_first_live_actor_of_class(unsigned id)
{
    return pp_first_of_class(id);
}

/* TEMP repro helper: the enemy's unique Actor id, so the egg-lay driver can set
   the tongue's target (Player+0x338) and let the REAL grab->swallow run. */
extern "C" unsigned port_unique_id_of_actor(void *actor)
{
    for (unsigned u = 0; u < 65536u; ++u)
        if (_ZN5Actor10FindWithIDEj(u) == actor)
            return u;
    return 0xffffffffu;
}

static unsigned pp_unique_id_of(void *actor)
{
    for (unsigned u = 0; u < 65536u; ++u)
        if (_ZN5Actor10FindWithIDEj(u) == actor)
            return u;
    return 0xffffffffu;
}

/* Nineteen filler virtuals put the twentieth at vtable+0x4c, so MSVC emits
   the same `call dword ptr [reg+0x4c]` for it that the ROM's three Yoshi
   call sites emit for slot 19. Declared, never defined: only the call is
   wanted, and the object is always a real actor with a real vtable. */
struct PpEggSlot {
    virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3();
    virtual void v4(); virtual void v5(); virtual void v6(); virtual void v7();
    virtual void v8(); virtual void v9(); virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18();
    virtual int OnTurnIntoEgg(void *player);          /* vtable + 0x4c */
};

/* SM64DS_VT_AUDIT=<frame> prints, once, the slot-21 entry of every class
   actually alive on this level, as a module-relative offset. Resolving those
   offline against walk_window.map and checking each for `ret 4` is a complete
   audit of what the level can really dispatch, which a scan of vtable-fill
   code shapes is not: a fill can write a slot through a register the scanner
   does not model, and only the live table settles it. */
static void port_vt_audit(void)
{
    static int armed = -1, at = 0;
    static long frame;
    if (armed < 0) {
        const char *e = std::getenv("SM64DS_VT_AUDIT");
        armed = 0;
        if (e) { at = (int)std::strtol(e, 0, 0); armed = at > 0; }
    }
    if (!armed || ++frame != at)
        return;
    armed = 0;
    unsigned char seen[PORT_ACTOR_IDS];
    std::memset(seen, 0, sizeof seen);
    char *base = (char *)(size_t)GetModuleHandleA(0);
    int n = 0;

    /* The set this audit walks has to be the set the CALLER can reach.
       func_ov002_020cef84 reaches its target through Actor::FindWithID, not
       through the behaviour list, and the two are different registries: an
       actor can be findable without being on the behaviour list. Enumerating
       by asking FindWithID for every id is exact and assumes nothing about
       the list's layout. Printed with the same [vt] shape. */
    for (unsigned u = 0; u < 8192u; ++u) {
        char *o = (char *)_ZN5Actor10FindWithIDEj(u);
        if (!o) continue;
        unsigned id = *(unsigned short *)(o + 0xc);
        char **vt = *(char ***)o;
        std::printf("[vtfind] uid %5u id %3u %-28s vt %p", u, id,
                    id < PORT_ACTOR_IDS ? port_actor_class_name(id) : "?",
                    (void *)vt);
        if (!vt) { std::printf(" NO VTABLE\n"); continue; }
        for (int s = 18; s <= 31; ++s)
            std::printf(" %d:+%08x", s, (unsigned)(size_t)(vt[s] - base));
        std::printf("\n");
    }

    for (int *node = (int *)(size_t)data_020a4b78[0]; node && n++ < 4096;
         node = (int *)(size_t)node[1]) {
        char *o = (char *)(size_t)node[2];
        if (!o) continue;
        unsigned id = *(unsigned short *)(o + 0xc);
        if (id >= PORT_ACTOR_IDS || seen[id]) continue;
        seen[id] = 1;
        char **vt = *(char ***)o;
        if (!vt) { std::printf("[vt] id %3u %-24s NO VTABLE\n", id,
                               port_actor_class_name(id)); continue; }
        std::printf("[vt] id %3u %-28s vt %p", id,
                    port_actor_class_name(id), (void *)vt);
        for (int s = 18; s <= 31; ++s)
            std::printf(" %d:+%08x", s, (unsigned)(size_t)(vt[s] - base));
        std::printf("\n");
    }
    std::fflush(stdout);
}

static void port_pound_probe(void)
{
    static int armed = -1, at = 240, slot = 21;
    static unsigned want;
    static long frame;
    if (armed < 0) {
        const char *e = std::getenv("SM64DS_POUND_PROBE");
        armed = 0;
        if (e) {
            char *end;
            want = (unsigned)std::strtoul(e, &end, 0);
            if (end != e) {
                armed = 1;
                if (*end == ':') {
                    at = (int)std::strtol(end + 1, &end, 0);
                    if (*end == ':')
                        slot = (int)std::strtol(end + 1, 0, 0);
                }
            }
        }
    }
    if (!armed || ++frame != at)
        return;
    armed = 0;

    void *target = pp_first_of_class(want);
    void *player = pp_first_of_class(191);
    if (!target || !player) {
        std::fprintf(stderr, "[pound] frame %ld: class %u %s, player %s -- "
                     "nothing to hit\n", frame, want, target ? "up" : "ABSENT",
                     player ? "up" : "ABSENT");
        std::fflush(stderr);
        return;
    }
    /* SM64DS_POUND_PROBE=<id>:<frame>:19 dispatches slot 19, OnTurnIntoEgg,
       instead of the pound. Slot 19 has no single ROM entry point the way
       slot 21 has func_ov002_020ef2a4 -- the three call sites are Yoshi's --
       so the shadow class below reproduces the call site rather than reusing
       one: nineteen filler virtuals put the twentieth at vtable+0x4c, and
       MSVC emits the same `call dword ptr [reg+0x4c]` with the Player in the
       one pushed slot. Same contract, same instruction. */
    if (slot == 19) {
        /* Two readbacks, one per class. POWER_STAR's body stores the argument
           it was handed at +0x438, so that word says whether the argument
           survived the veneer; every Actor has shouldBeKilled at +0xf, which
           is what BabyPenguin's OnTurnIntoEgg exists to set. */
        int star = (want == 178);
        unsigned arg0 = star ? *(unsigned *)((char *)target + 0x438) : 0u;
        unsigned char kill0 = *((unsigned char *)target + 0xf);
        std::fprintf(stderr, "[pound] frame %ld: class %u %s actor %p, player "
                     "%p -- dispatching slot 19 (OnTurnIntoEgg)\n", frame, want,
                     port_actor_class_name(want), target, player);
        std::fflush(stderr);
        int r = ((PpEggSlot *)target)->OnTurnIntoEgg(player);
        unsigned char kill1 = *((unsigned char *)target + 0xf);
        std::fprintf(stderr, "[pound] RETURNED %d -- frame survived; "
                     "shouldBeKilled %u -> %u\n", r, kill0, kill1);
        if (star)
            std::fprintf(stderr, "[pound] POWER_STAR +0x438 %08x -> %08x "
                         "(player is %p; a module code address there is the "
                         "dropped-argument bug)\n", arg0,
                         *(unsigned *)((char *)target + 0x438), player);
        std::fflush(stderr);
        return;
    }

    unsigned uid = pp_unique_id_of(target);
    if (uid == 0xffffffffu) {
        std::fprintf(stderr, "[pound] class %u actor %p has no unique id\n",
                     want, target);
        std::fflush(stderr);
        return;
    }

    static unsigned char clsn[512];
    std::memset(clsn, 0, sizeof clsn);
    int hit;
    if (slot == 28) {
        /* The CEILING third: whatever the player's head is under gets
           OnHitFromUnderneath(player). This is the exclamation/question
           block's own opening trigger. Flag and record offset both come from
           the matched accessors, not from a guess: func_02035638 tests
           c[0x90] & 0x10 and func_0203567c returns c + 0x30. */
        *(unsigned char *)(clsn + 0x90) |= 0x10u;   /* hit-ceiling */
        void *res = func_0203567c(clsn);
        if (!res || !func_02035638(clsn)) {
            std::fprintf(stderr, "[pound] scratch collider will not report a "
                         "ceiling (res %p, flags %02x)\n", res,
                         *(unsigned char *)(clsn + 0x90));
            std::fflush(stderr);
            return;
        }
        *(unsigned *)((char *)res + 0x1c) = uid;
        if (_ZNK10ClsnResult9GetClsnIDEv(res) != uid) {
            std::fprintf(stderr, "[pound] ceiling ClsnID readback %u != uid "
                         "%u -- the rig would dispatch on the wrong actor\n",
                         _ZNK10ClsnResult9GetClsnIDEv(res), uid);
            std::fflush(stderr);
            return;
        }
        std::fprintf(stderr, "[pound] frame %ld: class %u %s actor %p uid %u, "
                     "player %p -- dispatching slot 28 "
                     "(OnHitFromUnderneath) through func_ov002_020eeca8\n",
                     frame, want, port_actor_class_name(want), target, uid,
                     player);
        std::fflush(stderr);
        pp_qblock_state("before", target);
        /* The refusal test the five hit slots all consult, read directly so a
           refusal is distinguishable from a dispatch that never landed. Its
           own inputs are printed beside it: the level it switches on, the
           cached closest player, and the +0x706 byte it actually tests. */
        {
            void *cp_arg = _ZN5Actor13ClosestPlayerEv(target);
            std::fprintf(stderr, "[pound]   gate   func_ov102_02149078 -> %d "
                         "| level %d | cache %p | ClosestPlayer(this) %p "
                         "+0x706 %u | player %p +0x706 %u\n",
                         func_ov102_02149078(target), (int)data_0209f2f8,
                         (void *)data_0209b458, cp_arg,
                         cp_arg ? *(unsigned char *)((char *)cp_arg + 0x706) : 0u,
                         player, *(unsigned char *)((char *)player + 0x706));
            std::fflush(stderr);
        }
        hit = func_ov002_020eeca8(clsn, player);
        std::fprintf(stderr, "[pound] RETURNED %d -- the caller's frame "
                     "survived the dispatch\n", hit);
        pp_qblock_state("after", target);
        /* and follow it: state 1 is the bounce, and the block reaches state 2
           only by running func_ov102_021498e0 to its end, which is where the
           content is actually spawned. */
        g_qb_watch = target;
        g_qb_watch_left = 90;
        return;
    }
    if (slot == 25) {
        /* The WALL half of the same pair. func_ov002_020eee3c is what
           func_ov002_020ef2a4 is, with IsOnWall/GetWallResult in place of the
           floor pair and slot 25, OnPushed, in place of slot 21: whatever the
           player is pushing against gets OnPushed(player). Walking into a
           thing, which is why it reaches players far more often than a pound
           does. */
        *(unsigned char *)(clsn + 0x90) |= 0x8u;  /* WithMeshClsn::IsOnWall */
        void *res = _ZNK12WithMeshClsn13GetWallResultEv(clsn);
        if (!res || !_ZNK12WithMeshClsn8IsOnWallEv(clsn)) {
            std::fprintf(stderr, "[pound] scratch collider will not report a "
                         "wall (res %p, flags %02x)\n", res,
                         *(unsigned char *)(clsn + 0x90));
            std::fflush(stderr);
            return;
        }
        *(unsigned *)((char *)res + 0x1c) = uid;
        std::fprintf(stderr, "[pound] frame %ld: class %u %s actor %p uid %u, "
                     "player %p -- dispatching slot 25 through "
                     "func_ov002_020eee3c\n", frame, want,
                     port_actor_class_name(want), target, uid, player);
        std::fflush(stderr);
        hit = func_ov002_020eee3c(clsn, player);
    } else {
        *(unsigned *)(clsn + 0x10) |= 0x10u;      /* WithMeshClsn::IsOnGround */
        void *res = _ZNK12WithMeshClsn14GetFloorResultEv(clsn);
        if (!res || !_ZNK12WithMeshClsn10IsOnGroundEv(clsn)) {
            std::fprintf(stderr, "[pound] scratch collider will not report "
                         "ground (res %p)\n", res);
            std::fflush(stderr);
            return;
        }
        *(unsigned *)((char *)res + 0x1c) = uid;  /* ClsnResult::GetClsnID */
        std::fprintf(stderr, "[pound] frame %ld: class %u %s actor %p uid %u, "
                     "player %p -- dispatching slot 21 through "
                     "func_ov002_020ef2a4\n", frame, want,
                     port_actor_class_name(want), target, uid, player);
        std::fflush(stderr);
        hit = func_ov002_020ef2a4(clsn, player);
    }
    std::fprintf(stderr, "[pound] RETURNED %d -- the caller's frame survived "
                 "the dispatch\n", hit);
    std::fflush(stderr);
}

extern "C" void port_actor_tick(void)
{
    port_bob_debug_watch();
    port_pound_probe();
    pp_qblock_follow();
    port_vt_audit();
    data_02099f24[0] = 4;
    port_list_trace("cleanup", data_020a4ba8);
    func_02043fdc(data_020a4ba8);
    data_02099f24[0] = 2;
    port_list_trace("pending", data_020a4b88);
    func_02043fdc(data_020a4b88);
    /* THE FREEZE MASK LATCH -- Stage::Behavior's FIRST statement, arm9
       0x0202bbbc, src/_ZN5Stage8BehaviorEv.cpp:105:

           data_0209b464 = data_0209b454;

       and it belongs exactly here. data_0209b454 is the persistent
       freeze-REQUEST word; data_0209b464 is this frame's copy of it, and it is
       the only word Actor::BeforeBehavior reads
       (src/_ZN5Actor14BeforeBehaviorEv.cpp:74):

           if ((f & 9) != 9 && (data_0209b464 == 0 || (f & data_0209b464) != 0))
               goto do_copy;             // -> return 1, Behavior runs
           if (data_0209fc68 != 0)
               goto do_copy;             // cutscene override: think anyway
           return 0;                     // Behavior is SKIPPED this frame

       Zero means "no freeze, every actor thinks". Nonzero means "only actors
       whose own mFlags intersect the mask think" -- and slot 7 returning 0
       means ActorBase::Process (func_02043288) never calls slot 6 at all, so
       the actor's whole state machine stands still for the frame.

       WHAT USES IT. The message system. The Player talk state's Main
       (src/func_ov002_020c8540.c:45-52, the state Player::ShowMessage2 puts the
       player into) sets its OWN mFlags |= 0x800000, CLEARS the talk partner's,
       ORs 0x800000 into data_0209b454, and only then opens the box. So a
       dialogue freezes every actor in the level except the player reading it,
       and unfreezes them at :58-59 when data_0209d660 goes back to 0. Same
       set/clear pair at func_ov002_020c4188.c:43/64/83, func_ov002_020c92fc.c:35,
       func_ov002_020c91bc.c:12, func_020345b0.c:66. The message code itself is
       unaffected because it runs from Stage, not from the actor lists.

       WHY IT WAS MISSING. The port already sets data_0209b454 correctly -- the
       setters above are all live matched code -- but nothing in port/ ever
       COPIED it into data_0209b464, because all twenty Stage slots trap
       (hal/stage_bridges.cpp) and the port hand-hosts only Stage::Behavior's
       LAST statement (tests/walk_window.cpp). So the mask read 0 forever, the
       `data_0209b464 == 0` arm always passed, and no actor was ever frozen by a
       dialogue. Measured, castle grounds, glowing rabbit: b454 went 00800000
       the frame the box opened and b464 stayed 00000000 for all 900 frames
       while the RabbitKey kept falling.

       WHY AT THE HEAD OF PHASE 3 rather than the head of the whole tick. On the
       ROM the Stage is IN the behaviour list -- its spawn record at 0x0209213c
       carries behaviour priority 3 against the hundreds every other class uses
       -- so Stage::Behavior is the first body phase 3 runs and every actor
       walked after it sees the value it just latched. Phases 4 and 2 are
       cleanup and init; neither dispatches slot 7. This line is therefore the
       same statement, in the same position in the frame, reaching the same
       word. Stage::VE_Init, Stage::LC_Update and Stage::PS_Update re-latch it
       at their own points on the ROM; those paths are not hosted here and are
       named in runs/mg16/status/FIXC4.md as an honest gap. */
    data_0209b464 = (unsigned)data_0209b454[0];

    data_02099f24[0] = 3;
    port_list_trace("behaviour", data_020a4b78);
    func_02043fdc(data_020a4b78);
    data_02099f24[0] = 0;
}

/* Phase 5: the render bucket, in render-priority order. Runs inside the
   host's render frame rather than with the rest of func_02044120. Nothing
   converts units here any more: the whole frame is scene units, which is
   what an actor's own Render writes. */
/* GATE 33: the pairwise cylinder pass, arm9 0x02014aa8, BUILT AND WIRED BUT
   OFF BY DEFAULT -- SM64DS_CYLINDER_PASS=1 turns it on.

   It is the missing half of every cylinder interaction in the port. Each actor
   calls CylinderClsn::Update to link its own node into data_0209cee8; this
   walks that list, and for every overlapping pair writes hitFlags and
   otherOwner at CylinderClsn+0x20/+0x24 and unlinks the node again. Those two
   words are exactly what func_ov002_020b19dc reads at the coin's +0x198/+0x19c
   to decide it was collected. With the pass never running they stay zero, and
   Bob-omb Battlefield measures 60 coins alive with the counter at 0 while
   Mario stands inside one. The ROM calls it from Stage::Render (arm9
   0x0202ba14) and the Stage does not run here -- all twenty of its slots trap,
   hal/stage_bridges.cpp -- so this is where Stage::Render would have made the
   call: the head of the render phase.

   WHY IT IS OFF: it is the MSVC DESTRUCTOR SLOT SHIFT, in the one shape the
   dual-fill trick cannot serve. CylinderClsn declares ~CylinderClsn, then
   GetPos, then GetOwnerID, so the ROM table is {D1, D0, GetPos, GetOwnerID} at
   slots 0..3 while a header-compiled caller folds the two destructors into one
   and counts GetPos at 1 and GetOwnerID at 2. CylinderClsn::Process is
   header-compiled and dispatches both on every node.
   hal_fill_cylinder_withpos_vtable fills ROM slots 0..3, which is right for
   the Tree's shadow-TU callers, and _ZTV18MovingCylinderClsn -- what every
   actor on this gate's roster carries -- is unfilled storage. The two
   numberings COLLIDE (ROM slot 1 is D0, MSVC slot 1 is GetPos), so one array
   cannot serve both the way _ZTV5Model's Render does at 4 and 5.

   The fix is the one the MSVC-slot-shift note prescribes: keep the vtable
   ROM-shaped and host-copy the consumer with explicit slot indices -- a host
   Process reading slots 2 and 3 directly instead of dispatching. That is a
   0x49c-byte transcription and a gate of its own. Turning the switch on
   without it faults on the first frame with a jump to +0xffed0000, which is
   the null at MSVC slot 1 of an unfilled table.

   THAT TRANSCRIPTION WAS WRITTEN AND THE CYLINDER PASS RUNS. Everything above
   is still true of THIS SYMBOL and this switch, and reading it as "the port
   does not do cylinder collision" -- which is what it says on a quick read --
   is now wrong enough to send the next debugger somewhere else entirely.
   Corrected in passing by run mg16 lane MP3, which needed the answer and had
   to go and find it:

     port/unmatched/CylinderClsn_Process.cpp is the ROM-shaped host copy the
     paragraph above prescribes, port/slice_gate181.txt:22-25 records it, and
     tests/walk_window.cpp calls port_cylinder_clsn_process() UNCONDITIONALLY
     once per frame. SM64DS_CYLINDER_PASS gates only the MATCHED symbol below,
     which stays off for the dispatch reason given.

   So actor-vs-actor cylinder collision is LIVE by default, and MP3's rungs
   depend on it: two Player body cylinders land on the one list at
   data_0209cee8 and the host Process's symmetric branch pushes them apart. */
extern "C" void _ZN12CylinderClsn7ProcessEv(void);

static int port_cylinder_pass_on(void)
{
    static int on = -1;
    if (on < 0) on = std::getenv("SM64DS_CYLINDER_PASS") != 0;
    return on;
}

extern "C" void port_actor_render(void)
{
    if (port_cylinder_pass_on())
        _ZN12CylinderClsn7ProcessEv();
    data_02099f24[0] = 5;
    func_02043fdc(data_020a4b98);
    data_02099f24[0] = 0;
}

/* port/rollback: THE TICK-ONLY REPLAY'S RENDER PASS. status/ROLLBACK_SHIP.md
   has the audit: every Render body reached from list 5 is geometry
   submission and Render-private scratch, except InvisibleSecret (ids 329 and
   330), whose Render decrements the countdown its Behavior gates on and
   marks itself for destruction, and FlameChompFire (271) and MrI_Projectile
   (264), whose Render is the only keep-alive for their particle emitters,
   and the Minimap (335), whose Render advances the star-marker and star-key
   blink counters it keeps in its own object (+0x22e, +0x256) and writes the
   sub-screen sprite table; nothing in a tick reads those, but the arena
   would not be byte-identical after a replay without it (the DET rung's one
   arena byte was that counter), and it is a few sprite writes; and the
   UnchainedChomp (337), whose Render is where its ModelAnim and the five
   chain-link models advance and write their matrices (ModelAnim::Render
   through the +0x370 links), which its Behavior reads back for the chain.
   The DET rung named it by bisection (SM64DS_ROLLBACK_REPLAY_ALSO): with it
   in the walk every window is arena-identical; without it 14 to 20 bytes of
   link matrices drift. The rule this leaves: an actor whose Render advances
   an animation its tick reads belongs on this list, and the DET rung on a
   new map is how such an actor is found.
   A replayed frame walks list 5 exactly as func_02043fdc does (same phase
   word, same current-node global, same PMF dispatch) and runs only those
   four; everything else stands down. Each allowed node is run through the
   ROM's own walker on a one-node view of the list, so the dispatch, the
   `this` adjustment and data_020a4b68 are the walker's, not a copy. */
extern "C" void *data_020a4b68;
extern "C" void *func_02043fdc(void *thing);
/* SM64DS_ROLLBACK_REPLAY_ALSO="id,id,...": extra actor ids whose Render
   runs in the tick-only replay. A bisecting knob for the DET rung, so a
   Render body that writes tick-visible state can be named without a
   rebuild per guess; it decides nothing in a shipped run. */
static unsigned char g_replay_also[1024 / 8];
static int g_replay_also_read;
static int replay_also(unsigned id)
{
    if (!g_replay_also_read) {
        g_replay_also_read = 1;
        if (const char *e = std::getenv("SM64DS_ROLLBACK_REPLAY_ALSO")) {
            for (const char *q = e; *q;) {
                char *end;
                const long v = std::strtol(q, &end, 10);
                if (end == q) break;
                if (v >= 0 && v < 1024) g_replay_also[v >> 3] |= (unsigned char)(1u << (v & 7));
                q = *end == ',' ? end + 1 : end;
            }
        }
    }
    return id < 1024 && (g_replay_also[id >> 3] >> (id & 7)) & 1;
}

extern "C" void port_actor_render_replay(void)
{
    struct Node { int pad; Node *next; unsigned char *obj; };
    data_02099f24[0] = 5;
    Node *node = (Node *)(size_t)data_020a4b98[0];
    while (node) {
        Node *next = node->next;
        // actorID is the u16 at +0x0c (include/Actor.h:405); the byte above
        // it is aliveState, which is 1 on a live actor, so an unmasked read
        // of the word compares 0x1014f against 335 and matches nothing
        const unsigned id = *(const unsigned *)(node->obj + 0x0c) & 0xFFFFu;
        if (id == 329 || id == 330 || id == 271 || id == 264 || id == 335 ||
            id == 337 || replay_also(id)) {
            int one[8];
            std::memcpy(one, data_020a4b98, sizeof one);
            one[0] = (int)(size_t)node;
            node->next = 0;
            func_02043fdc(one);
            node->next = next;
        }
        node = next;
    }
    data_020a4b68 = 0;
    data_02099f24[0] = 0;
}

/* Phase 1: the scene tree. Priority re-sorts, parent flag propagation and the
   deferred list insertions -- the housekeeping that closes the ROM's frame. */
extern "C" void port_actor_scene_pass(void)
{
    data_02099f24[0] = 1;
    func_020441cc(data_020a4b6c);
    data_02099f24[0] = 0;
}

/* How many actors are currently on each list -- the read-back that says the
   spawn spine really linked them -- plus the AREA table, because an actor
   whose area is not showing has Actor::BeforeBehavior set its 0x38 flags and
   Actor::BeforeRender refuses to draw it. Every object in a sub-table carries
   the sub-table's index as its area id (LoadClsnAndObjects passes it as the
   loader's second argument; only the main table's objects get -1, which means
   "not area-bound"), so on the castle grounds that is area 0. */
extern "C" {
extern signed char data_02092120;         /* the area currently shown */
unsigned char IsAreaShowing(int idx);
}

extern "C" void port_actor_positions(void);
extern "C" void port_actor_collision_probe(void);

extern "C" void port_actor_lists_probe(void)
{
    std::printf("[area] shown %d, showing:", data_02092120);
    for (int i = 0; i < 4; ++i)
        std::printf(" [%d]=%u", i, IsAreaShowing(i));
    std::printf("\n");
    static const struct { const char *n; int *l; int step; } lists[] = {
        {"behaviour", data_020a4b78, 1}, {"render", data_020a4b98, 1},
        {"pending", data_020a4b88, 1}, {"cleanup", data_020a4ba8, 1},
    };
    for (int i = 0; i < 4; ++i) {
        int n = 0;
        for (int *node = (int *)(size_t)lists[i].l[0]; node && n < 4096;
             node = (int *)(size_t)node[1])
            ++n;
        std::printf("[lists] %-9s %d\n", lists[i].n, n);
    }
    port_actor_collision_probe();
    port_actor_positions();
}

/* THE COLLISION REGISTRY, which is what every Player probe walks.
   MeshColliderBase::Enable parks a collider in the first free slot of
   data_020a0c80[24]; RaycastGround::DetectClsn then walks all 24 and, for
   every slot past 0, applies a DISTANCE CULL before it will even ask the
   collider: when the owner actor's +0xb0 carries bit 1, the ray is skipped
   unless it is within (owner+0xb8 << 3) of the owner's position and no lower
   than owner.y + owner+0xb4 minus the same. owner+0xb8 is the Clipper's own
   cull radius in SCENE units, which is why the shift is there.
   So a collider that reads right in every other way can still be invisible to
   a probe standing on top of it, and this is the line that says so. */
extern "C" { extern void *data_020a0c80[24]; }

/* A registered collider's own KCL, in the world units its owner's matrix puts
   it in: MeshCollider+0x20 is the file, its vertex array runs up to the normal
   table, and every position is stored at 1/64 of a Fix12i (the <<6 the ITCM
   walk does on read). For a MovingMeshCollider the matrix translation is the
   owner's position, which the line above already prints. A probe that finds no
   water when it is standing in the moat is answered here: either the mesh is
   somewhere else, or it is exactly where it should be and the walk is the
   problem. */
static void port_clsn_kcl_bounds(const char *o)
{
    const char *const *f = (const char *const *)(o + 0x20);
    if (!*f) { std::printf("[clsnreg]      (no KCL)\n"); return; }
    const int (*pos)[3] = *(const int (**)[3])(*f + 0x00);
    const char *norm = *(const char *const *)(*f + 0x04);
    long n = (norm - (const char *)pos) / 12;
    if (n <= 0 || n > 65536) { std::printf("[clsnreg]      (KCL %ld?)\n", n);
                               return; }
    int lo[3] = {1 << 30, 1 << 30, 1 << 30};
    int hi[3] = {-(1 << 30), -(1 << 30), -(1 << 30)};
    for (long i = 0; i < n; ++i)
        for (int k = 0; k < 3; ++k) {
            int v = pos[i][k] << 6;
            if (v < lo[k]) lo[k] = v;
            if (v > hi[k]) hi[k] = v;
        }
    std::printf("[clsnreg]      KCL %ld verts, local world bounds "
                "x[%.0f..%.0f] y[%.0f..%.0f] z[%.0f..%.0f]\n", n,
                lo[0] / 4096.0, hi[0] / 4096.0, lo[1] / 4096.0, hi[1] / 4096.0,
                lo[2] / 4096.0, hi[2] / 4096.0);
}

extern "C" void port_actor_collision_probe(void)
{
    std::printf("[clsnreg] slot owner        id   pos                 "
                "flags    cull(+0xb8) yoff(+0xb4) +0x0c\n");
    for (int i = 0; i < 24; ++i) {
        const char *o = (const char *)data_020a0c80[i];
        if (!o) continue;
        const char *a = *(const char *const *)(o + 4);
        if (!a) {
            std::printf("[clsnreg] %2d   (level, owner NULL)\n", i);
            continue;
        }
        std::printf("[clsnreg] %2d   %p %4u (%6d,%6d,%6d) %08x %6d %6d %8d\n",
                    i, (const void *)a, *(const unsigned short *)(a + 0xc),
                    *(const int *)(a + 0x5c) >> 12,
                    *(const int *)(a + 0x60) >> 12,
                    *(const int *)(a + 0x64) >> 12,
                    *(const unsigned *)(a + 0xb0), *(const int *)(a + 0xb8),
                    *(const int *)(a + 0xb4), *(const int *)(o + 0x0c));
        port_clsn_kcl_bounds(o);
    }
}

/* Every live actor's own position, read back off the behaviour list in world
   units and grouped by class. The level's object tables are the reference:
   an actor that spawned but reads back somewhere else has had its record
   misread, and a class whose count here is short of the census has destroyed
   instances (which for the Tree is the ROM's own design -- see the gate-16
   commit). Actor pos is the Vector3 at +0x5c; the class is the id at +0xc,
   which the ActorBase constructor copied out of the spawn context. */
/* The global water level, which is not a harness number any more: since
   gate 17 CASTLE_WATER's own InitResources clamps data_0209f32c to its own Y
   minus 100 units, and the Player's state machine reads that word to decide
   it is swimming. */
extern "C" { extern int data_0209f32c[]; }

extern "C" void port_actor_positions(void)
{
    std::printf("[water] level %d (%.1f units)\n", data_0209f32c[0],
                data_0209f32c[0] / 4096.0f);
    for (const PortActorClass *k = port_actor_classes; k->name; ++k) {
        int n = 0;
        for (int *node = (int *)(size_t)data_020a4b78[0]; node && n < 4096;
             node = (int *)(size_t)node[1]) {
            char *o = (char *)(size_t)node[2];
            if (!o || *(unsigned short *)(o + 0xc) != k->id)
                continue;
            if (!n++)
                std::printf("[pos] %-17s", k->name);
            const int *p = (const int *)(o + 0x5c);
            std::printf(" (%d,%d,%d)", p[0] >> 12, p[1] >> 12, p[2] >> 12);
        }
        if (n)
            std::printf("  [%d]\n", n);
    }
}
