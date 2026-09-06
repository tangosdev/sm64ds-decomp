// The Stage actor, host side.
//
// On the ROM the Stage IS the scene. It is the first actor built, the one
// every other actor hangs off, and it owns in its own storage the two objects
// the harness used to stage by hand:
//
//     Stage + 0x50   Particle::SysTracker
//     Stage + 0x86c  Model            -- the level model
//     Stage + 0x91c  MeshCollider     -- the level collision
//
// The port stood in for it twice over: a bare 0x60-byte MeshCollider in
// walk_window (mc_storage) and a 0x50-byte ActorBase-shaped block seated as
// the scene root (hal_scene_root, hal/level_boot.cpp). This is the real object
// taking both seats.
//
// Nothing here is behaviour. Stage::Stage is the matched src file
// (src/_ZN5StageC3Ev.c, arm9 0x0202e088) and this is the seam it needs: the
// vtable array its last vptr store names, the spawn context the ActorBase
// constructor reads, and one honest stub for the particle constructor.
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "dsstate_seg.h"

extern "C" {

void *_ZN5StageC3Ev(void);

/* The ROM's own SpawnInfo for the Stage: arm9 0x0209213c, emitted from the
   extracted image by port/tools/romdata.py (Nintendo's bytes never enter the
   repo). Layout, the same record shape hal/actor_registry.cpp documents:
       +0  factory   0x0202e088, Stage::Stage itself
       +4  u16       the actor id, which the ActorBase ctor also reads back as
                     the BEHAVIOUR-list priority  -- 3 for the Stage
       +6  u16       the RENDER-list priority                  -- 6
   Cross-checked against Camera_SpawnInfo, whose +4 is 0x14c and matches the
   registry's own row id. */
extern unsigned char _ZN5Stage9spawnDataE[];

extern void **data_020a4bb8;          /* actorID -> SpawnInfo* */
extern unsigned short data_020a4b54;  /* the PENDING actor id the ctor reads */
extern int data_020a4b6c[];           /* the scene tree */
extern void *data_0209f314;           /* the level AREA table (Stage+0x8bc) */

/* ---- the vtable -----------------------------------------------------------
   ROM SLOT ORDER, the same law as the Camera's (hal/camera_bridges.cpp) and
   ArrowSignRight's (hal/actor_vtables.cpp): MSVC slot order, host __fastcall
   thunks, everything the port cannot serve trapped BY NAME rather than left
   pointing at an arm9 address the host cannot execute.

   The array the fill targets is _ZTV5Stage, because that is what Stage::Stage
   installs: its transcription stores three vptrs in a row (data_0208e4b8,
   _ZTV5Scene, _ZTV5Stage) and the last one wins. Both other names are storage
   only, same rule as the base tables.

   Read out of the ROM at 0x020921c0 with its relocations applied:
       0  0x0202cc0c Stage::InitResources        10 0x0202e3a4 Scene::BeforeRender
       1  0x0202ddc8 Stage::BeforeInitResources  11 0x0202e398 Scene::AfterRender
       2  0x0202e62c Scene::AfterInitResources   12 0x0202b8a0 Stage::OnPendingDestroy
       3  0x0202c9a8 Stage::CleanupResources     13 0x0204357c ActorBase
       4  0x0202e5f0 Scene::BeforeCleanupRes.    14 0x0204349c ActorBase
       5  0x0202e5d0 Scene::AfterCleanupRes.     15 0x02043494 ActorBase::OnHeapCreated
       6  0x0202bbbc Stage::Behavior             16 0x02023688 ~Stage (D1)
       7  0x0202e3d4 Scene::BeforeBehavior       17 0x020236f0 ~Stage (D0)
       8  0x0202e3c8 Scene::AfterBehavior
       9  0x0202b8a4 Stage::Render
   (0x020921c0 + 18*4 is data_02092208, the per-level LVL_Overlay pointer
   table -- the table Stage::InitResources indexes by level. It is NOT two
   more vtable slots; the ROM table is eighteen entries.)

   EVERY SLOT TRAPS TODAY. Nothing dispatches through the Stage yet: it is in
   the scene tree but on neither processing list, exactly as the stand-in it
   replaces was. A trap turns the first real dispatch into a named abort with
   the slot number instead of a jump into an unmapped arm9 address, which is
   how the next step finds out which faces it owes.

   The names follow config/arm9/symbols.txt, which puts _ZTV5Stage at
   0x020921c0 and _ZTV5Scene at 0x02092680. That is the ROM's layout: the
   0x02092680 table carries the Scene destructors in slots 16/17 and the
   ActorBase default in slot 0, while 0x020921c0 is the one the constructor
   installs last. Config had the two swapped until 87a55cfab, so a checkout
   older than that calls 0x02092680 _ZTV5Stage and will not link this file. */
void *_ZTV5Stage[20];      /* the table the ctor installs last; trap-filled */
void *_ZTV5Scene[20];      /* transient ctor install, storage only */

}  /* extern "C" */

static const char *const hal_stage_slot_name[20] = {
    "InitResources", "BeforeInitResources", "AfterInitResources",
    "CleanupResources", "BeforeCleanupResources", "AfterCleanupResources",
    "Behavior", "BeforeBehavior", "AfterBehavior",
    "Render", "BeforeRender", "AfterRender",
    "OnPendingDestroy", "Virtual34", "Virtual38", "OnHeapCreated",
    "~Stage (D1)", "~Stage (D0)", "(past the table)", "(past the table)"};

/* ---- THE TRAP, AND THE TWO DEFECTS THE 2026-08-16 EXIT-LEVEL REPORT FOUND --
 *
 * The comment above promises "a named abort with the slot number". It named
 * slot 0 every time, for three independent reasons, and the bill came due on a
 * real session: seventeen minutes of play in level 4, pause, exit course, and
 *
 *     FATAL: Stage vtable slot 0 (InitResources) is not hosted
 *
 * Slot 0 is SEATED -- hal_seat_stage_lifecycle writes st_init into it below --
 * so slot 0 could not have been the caller and never was.
 *
 *   1. hal_stage_trap_slot WAS DECLARED, READ, AND NEVER WRITTEN. Not once,
 *      anywhere in the tree. One st_trap went into every slot, and a single
 *      function cannot know which table entry dispatched it, so the variable
 *      sat at its .bss zero and every trap in the table reported slot 0.
 *      The fix is the shape hal/sub_actors.cpp already uses for its slots 13
 *      and 14: one thunk per slot, each recording its own index. That file
 *      installs a bare sa_trap in the other sixteen and has the same bug in
 *      them; this lane fixes it there too.
 *
 *   2. THE NAME LOOKUP WAS A BITMASK WHERE A BOUNDS CLAMP WAS MEANT.
 *      `hal_stage_slot_name[hal_stage_trap_slot & 19]`. 19 is 0b10011, so it
 *      keeps bits 0, 1 and 4 and throws the rest away: slot 12 reads as 0
 *      ("InitResources"), 13 as 1, 14 as 2, 9 as 1. Twelve of the eighteen
 *      slots name the wrong function even once the number is right. Slot 3 is
 *      one of the six the mask happens to pass through unchanged, which is why
 *      the corrected message for this crash reads correctly.
 *
 *   3. THE DEFAULT WAS ITSELF A VALID SLOT INDEX, which is why 1 produced a
 *      confident wrong answer rather than an obviously broken one: 0 is a real
 *      slot, so an unrecorded trap and a genuine InitResources trap printed
 *      the same line and there was nothing to distrust. All three reporters
 *      now start at -1 and say so if they are ever read unwritten.
 *
 * WHICH SLOTS TRAP IS UNCHANGED BY ANY OF THIS. This is the report only. */
static int hal_stage_trap_slot = -1;

static int __fastcall st_trap(void *, void *)
{
    const char *name = "the trap did not record its slot -- fix the thunk";
    if (hal_stage_trap_slot >= 0 && hal_stage_trap_slot < 20)
        name = hal_stage_slot_name[hal_stage_trap_slot];
    std::fprintf(stderr, "FATAL: Stage vtable slot %d (%s) is not hosted\n",
                 hal_stage_trap_slot, name);
    std::abort();
    return 0;
}

/* One thunk per slot, generated rather than typed out twenty times. Same body
   as sa_trap13/sa_trap14 in hal/sub_actors.cpp. */
#define HAL_STAGE_TRAP(n)                                                    \
    static int __fastcall st_trap##n(void *s, void *d)                       \
    { hal_stage_trap_slot = (n); return st_trap(s, d); }
HAL_STAGE_TRAP(0)  HAL_STAGE_TRAP(1)  HAL_STAGE_TRAP(2)  HAL_STAGE_TRAP(3)
HAL_STAGE_TRAP(4)  HAL_STAGE_TRAP(5)  HAL_STAGE_TRAP(6)  HAL_STAGE_TRAP(7)
HAL_STAGE_TRAP(8)  HAL_STAGE_TRAP(9)  HAL_STAGE_TRAP(10) HAL_STAGE_TRAP(11)
HAL_STAGE_TRAP(12) HAL_STAGE_TRAP(13) HAL_STAGE_TRAP(14) HAL_STAGE_TRAP(15)
HAL_STAGE_TRAP(16) HAL_STAGE_TRAP(17) HAL_STAGE_TRAP(18) HAL_STAGE_TRAP(19)
#undef HAL_STAGE_TRAP

static void *const hal_stage_trap_thunk[20] = {
    (void *)st_trap0,  (void *)st_trap1,  (void *)st_trap2,  (void *)st_trap3,
    (void *)st_trap4,  (void *)st_trap5,  (void *)st_trap6,  (void *)st_trap7,
    (void *)st_trap8,  (void *)st_trap9,  (void *)st_trap10, (void *)st_trap11,
    (void *)st_trap12, (void *)st_trap13, (void *)st_trap14, (void *)st_trap15,
    (void *)st_trap16, (void *)st_trap17, (void *)st_trap18, (void *)st_trap19};

extern "C" void hal_seat_stage_lifecycle(void);
extern "C" void port_stage_seat_probe(void);   /* below the seat */

extern "C" void hal_fill_stage_vtable(void)
{
    for (int i = 0; i < 20; ++i)
        _ZTV5Stage[i] = hal_stage_trap_thunk[i];
    hal_seat_stage_lifecycle();
}

// ---- THE LIFECYCLE SEAT (run link60, lane L4) ------------------------------
//
// Thirteen of the eighteen slots get the ROM's own body. The derivation is the
// table above, re-read out of extracted/arm9_dec.bin at (0x020921c0 -
// 0x02004000) with every word cross-checked against config/arm9/relocs.txt --
// all eighteen carry a kind:load module:main relocation, and 0x020921c0 + 18*4
// is the START of data_02092208, so the table really is eighteen words and the
// two spare host slots stay trapped.
//
// The full per-slot accounting, including the five that do NOT get a ROM body
// and the measured reason for each, is port/stage_lifecycle_map.txt. The short
// version, because a reader here should not have to go and get it:
//
//   0  InitResources     HOSTED. The matched TU calls LoadLevelOverlays and
//                        LoadArchive -- an overlay loader and an archive
//                        mounter the port has decided, in two other lanes'
//                        files, not to have. Its occupant is the boot body
//                        that was already running, now dispatched here.
//   3  CleanupResources  SEATED AND ENV-GATED (run link100, lanes STAGE then
//                        STAGEFIX). The word is the ROM's own body, reached
//                        only under SM64DS_STAGE_SLOT3_ROM; unset -- the
//                        shipped default -- the thunk is the named abort the
//                        slot carried before, byte for byte. GATED BECAUSE
//                        SLOT 3 IS REACHABLE: port/stage_lifecycle_map.txt
//                        section 5 measured a VS match-end scene request
//                        reaching this dispatch and section 9 traces a live
//                        death-plane chain into it. Its link wants exactly two
//                        matched TUs on this tree,
//                        Sound::ResetPlayerVoiceGroup and
//                        Message::ResetAllGlobals; the six the first reading
//                        named all resolve now.
//   6  Behavior          HOSTED, and measured BLOCKED on a file this lane does
//                        not own: tests/walk_window.cpp transcribes three of
//                        Stage::Behavior's own statements into the frame loop
//                        (Stage::CheckInput at :9107, ShadowModel::CleanAll at
//                        :10767, ProcessKuppaScript at :10788), and
//                        hal/actor_registry.cpp:908 a fourth. Seating the ROM
//                        body without retiring those runs the cutscene script
//                        twice per frame. See port/stage_lifecycle_map.txt
//                        section 12.
//   9  Render            HOSTED, and blocked the same way and harder:
//                        walk_window's render frame is Stage::Render
//                        transcribed statement by statement (:11984-:12060),
//                        and it deliberately runs the ROM's render BUCKET
//                        before the world/scene matrix shim so ROM actors and
//                        host models each get the matrix they were written
//                        against. Slot 9 dispatches inside that bucket, so the
//                        ROM body would draw the level on the wrong side of
//                        the shim.
//  16  ~Stage (D2)       SEATED (run link100, lane STAGE). Nothing destroys
//  17  ~Stage (D0)       the Stage, so neither executes; the ROM's own words
//                        in the ROM's own table are the reference edge.
//
// SLOT 12 IS THE ROM BODY, AND THE FIRST DRAFT OF THIS SEAT HAD IT WRONG.
// It was trapped on the same "nothing destroys the Stage" argument as 16 and
// 17, and this seat is what falsified that argument -- review caught it, not
// this file. Slot 7 now runs Scene::BeforeBehavior on the Stage every frame,
// and that function carries two ActorBase::MarkForDestruction(self) edges.
// MarkForDestruction is not deferred bookkeeping: src/
// _ZN9ActorBase18MarkForDestructionEv.cpp sets shouldBeKilled and then calls
// OnPendingDestroy() straight through the vptr, so either edge landed on
// st_trap and aborted.
//
//   Edge A, the data_0209f1e0 block, is dead: no linked TU writes that byte.
//   Edge B is shut only by data_02092664 resting at its 0x187 sentinel, and
//   hal/star_flow.cpp:127 opens it. HitDeathPlane calls
//   Scene::StartSceneFade(8, 0, 0), which is Scene::SetSceneToSpawn(8, 0),
//   which writes data_02092664 = 8. Nothing in a level run ever spawns scene
//   8, so the byte STAYS at 8 and Scene::BeforeBehavior takes its middle
//   branch on every later frame; from then on the installed fader reading
//   at-end marks the Stage for destruction. The battery cannot see this --
//   idle selftest frames never collect a star and never hit a death plane.
//
// AND SLOT 3 IS THE NEXT TRAP IN THE SAME CHAIN, which is new evidence rather
// than a residual worry. shouldBeKilled is +0x0f, which func_02043880 reads as
// its `dirty` flag, so the frame after MarkForDestruction phase 1 moves the
// Stage onto data_020a4ba8 and the cleanup Process dispatches slots 4, 3 and 5
// -- and slot 3 is trapped. Seating 12 does not make the chain survivable; it
// moves the abort one frame later, from a slot with a compiled ROM body to a
// slot that has none. That is the right place for it to stop: a port whose
// Stage is being torn down has lost the scene root, the level collider and the
// level model, and a named abort is the correct answer until the port has a
// real Stage teardown. Recorded in port/stage_lifecycle_map.txt as the first
// thing the slot-0 lane inherits.
//
// AND SLOT 3 STILL STOPS THERE. Slot 3 now holds the ROM's own
// Stage::CleanupResources, but behind SM64DS_STAGE_SLOT3_ROM and nothing else:
// with the variable unset -- every shipped run -- the chain above ends in the
// same named abort it ended in before, for the same reason. The gate and the
// four places that reach the dispatch are written out at the slot-3 thunk
// below.
//
// THREE SLOTS ARE VENEERS AND MUST NOT TAKE THEIR OWN src TU. Slots 2, 8 and
// 11 are 0xc-byte `ldr ip,[pc]; bx ip` tail calls in the ROM, transcribed as
// `void f(void)` calling `void g(void)`; under __cdecl a (void) caller pushes
// nothing and the receiver and the vfSuccess code arrive as garbage.
// hal/scene_actor_faces.cpp already carries the three faces that dispatch
// straight to each veneer's TARGET with both arguments, and this file reuses
// them rather than repeating the derivation.
//
// SLOT 1 IS A VENEER TOO, and that is this lane's find rather than an inherited
// one. src/_ZN5Stage19BeforeInitResourcesEv.cpp is
//     ldr ip, [pc]; bx ip; .word 0x0202e66c
// to Scene::ResetFadersAndSound, written as `void(void)` calling `void(void)`.
// Scene::ResetFadersAndSound takes the receiver and RETURNS int, and the init
// Process reads that return value to decide whether InitResources runs at all
// -- so the transcription would hand the Process whatever was in eax. The slot
// dispatches to the veneer's target directly, with the receiver, and
// port/slice_w8a.txt's line for that TU stays dropped.
extern "C" {
/* slot 1's real destination */
int  _ZN5Scene19ResetFadersAndSoundEv(void *self);
/* slots 4, 5, 7, 10 -- Scene halves with declared arguments, all four already
   in the image before this seat existed */
int  _ZN5Scene22BeforeCleanupResourcesEv(void *self);
void _ZN5Scene21AfterCleanupResourcesEj(void *self, unsigned a);
int  _ZN5Scene14BeforeBehaviorEv(void *self);
int  _ZN5Scene12BeforeRenderEv(void *self);
/* slots 2, 8, 11, 15 -- hal/scene_actor_faces.cpp */
void port_scene_after_init(void *self, unsigned vfSuccess);
void port_scene_after_behavior(void *self, unsigned vfSuccess);
void port_scene_after_render(void *self, unsigned vfSuccess);
int  port_scene_on_heap_created(void *self);
/* slot 12. src/_ZN5Stage16OnPendingDestroyEv.cpp compiles a real C++ method
   and publishes only ?OnPendingDestroy@Stage@@QAEXXZ, never the Itanium C
   name, so the slot needs a face rather than a plain pointer. The face is at
   the bottom of this file with the other Stage.h method faces; the body is an
   empty override, which is what the ROM's is. */
void port_stage_on_pending_destroy(void *self);
/* the ROM's own "thread a freshly built actor onto the init list" entry, the
   last thing func_02043098 does for every other class */
void func_020433b8(void *self);
/* slot 3 (run link100, lane STAGE). src/_ZN5Stage16CleanupResourcesEv.cpp
   compiles a real C++ method against include/Stage.h, so it publishes only
   ?CleanupResources@Stage@@QAEHXZ and needs a face, exactly like slot 12's.
   The face is at the bottom of this file with the other Stage.h method
   faces. */
int  port_stage_cleanup_resources(void *self);
/* slots 16 and 17 (run link100, lane STAGE). Both matched TUs are FLAT C --
   `struct Stage *f(struct Stage *thiz)`, receiver as an ordinary first
   argument -- so they need no face, only the ecx->arg adapter every seat in
   this family uses. */
void *_ZN5StageD2Ev(void *thiz);
void *_ZN5StageD0Ev(void *thiz);
}

/* Slots 13 and 14. The same local NON-VIRTUAL declaration hal/scene_boot.cpp
   makes, and for the same reason: src/_ZN9ActorBase9Virtual34Ejj.cpp and its
   sibling declare the method non-virtual in their own struct, so the
   definitions are ?Virtual34@ActorBase@@QAEHII@Z. include/Stage.h does not
   pull in ActorBase.h, so there is nothing here for this to collide with. */
struct ActorBase {
    int Virtual34(unsigned a, unsigned b);
    int Virtual38(unsigned a, unsigned b);
};

/* THE BOOT BODY, reached through slot 0. port_stage_a_boot (hal/level_boot.cpp)
   stashes its two arguments and its result here so the slot can carry them:
   the ROM's init Process dispatches int(void) and has nowhere to put them. */
extern "C" {
void *port_stage_boot_body(void *mc, int spawn);   /* hal/level_boot.cpp */
void *port_stage_boot_arg_mc(void);
int   port_stage_boot_arg_spawn(void);
void  port_stage_boot_set_result(void *o);
}

/* Run link60 Stage 4, lane SD0: the slot-0 swap, env-gated. Returns the ROM
   Stage::InitResources' own result under SM64DS_SLOT0_ROM, or -2 for "declined,
   keep the host answer", which is what an unset environment gives. The whole
   apparatus and both walls it bisects are documented in hal/stage_slot0.cpp
   part 4; the one thing to know here is that mode 0 leaves this thunk doing
   exactly what it did before. */
extern "C" int port_slot0_rom_init(void *self);   /* hal/stage_slot0.cpp */

static int __fastcall st_init(void *self, void *)
{
    port_stage_boot_set_result(
        port_stage_boot_body(port_stage_boot_arg_mc(),
                             port_stage_boot_arg_spawn()));
    {
        const int rom = port_slot0_rom_init(self);
        if (rom != -2)
            return rom;
    }
    /* 1 is the ROM's own "initialisation finished" return. The init Process
       turns it into vfSuccess 2, which is the only code
       ActorBase::AfterInitResources promotes on; 0 becomes 1, which is
       ActorDerived::AfterInitResources' MarkForDestruction. */
    return 1;
}

/* Slot 6. THE HOST OCCUPANT IS RETIRED AND THE ROM'S OWN Stage::Behavior IS
   HERE (run link100, lane FRAME).
   ---------------------------------------------------------------------------
   What used to stand in this comment is worth keeping in front of a reader,
   because it was true when it was written and it is exactly what changed:

     "NEW HOST OCCUPANTS, and this file is not going to pretend otherwise: the
      seat needs something in them because an actor that survives the init pass
      is on the behaviour and render lists on the next frame, and both ROM
      bodies are measured blocked. ... They exist to keep the dispatch from
      landing on the trap, and they retire when their ROM bodies close."

   Slot 6's body closes here. port/stage_lifecycle_map.txt section 12b measured
   what was actually holding it, and it was never a symbol: tests/walk_window
   .cpp WAS the port's copy of Stage::Behavior, transcribed statement by
   statement into the frame loop, with the freeze-mask latch in
   hal/actor_registry.cpp, UpdateMessage's body in hal/message_pump.cpp and the
   VS countdown statement in hal/star_flow.cpp. All five sites are retired in
   the same commit as this line; section 13 is the accounting.

   THE THUNK IS AN ADAPTER AND NOTHING ELSE. src/_ZN5Stage8BehaviorEv.cpp
   compiles a real C++ method and publishes ?Behavior@Stage@@QAEHXZ, which no C
   name in this file can spell and which include/Stage.h does not declare, so
   the call goes through hal/stage_frame.cpp -- one file, one declaration of
   class Stage, the seven faces the ROM body's own callees need, and the entry
   point below. The ROM's Behavior returns 1 on every path, which is the code
   the behaviour Process reads as "this pass succeeded".

   TWO OF THE THREE TARGETS, and this is the one place this seat differs from
   slot 0's row in port/CMakeLists.txt. Stage::Behavior calls
   Stage::UpdateMessage, whose body on this port is port_message_pump, and
   hal/message_pump.cpp is on walk_window and walk_window_hires only -- it
   brings the message compositor and the generated text with it, which
   smoke_player has no use for and does not carry. So the gate is on those two
   and smoke_player keeps the host occupant, selected here by the compile
   definition rather than by a second copy of the thunk living somewhere else.
   smoke_player does not run a level frame loop, so the occupant it keeps is
   the program it already had. */
#if defined(SM64DS_STAGE_SLOT6_ROM)
extern "C" int port_stage_rom_behavior(void *self);   /* hal/stage_frame.cpp */

static int __fastcall st_behavior(void *s, void *)
{ return port_stage_rom_behavior(s); }
#else
/* smoke_player: the host occupant, unchanged. Behaviour-neutral by
   construction -- it is what every target had before gate 220. */
static int __fastcall st_behavior(void *, void *) { return 1; }
#endif

/* Slot 9. STILL A HOST OCCUPANT, and the reason is the frame loop rather than
   the link -- port/stage_lifecycle_map.txt section 12c, and 13b for the two
   things lane FRAME shipped wrong.

   AND ONE CORRECTION, because this comment carried it first (lane FRAME2). What
   stood here was that tests/walk_window.cpp "runs the ROM's actor render bucket
   BEFORE the world/scene matrix handling that follows it, so a slot-9 dispatch
   (which happens from inside that bucket) draws the level model, the skybox and
   the transparent pass on the other side of it". There is no matrix handling
   after the bucket any more: the block below port_actor_render in that file
   opens "THE VIEW MATRIX IS USED AS THE ROM PRODUCED IT ... The R6 shim that
   scaled this row by 8 for the harness's world-unit models is gone", and every
   model matrix in the frame is scene units. What blocks slot 9 is section 6's
   twelve pieces and the fact that tests/walk_window.cpp IS Stage::Render,
   transcribed statement by statement -- the same shape of job slot 6 was, with
   a selftest-BMP A/B in place of a script cursor where the proof is concerned.
   12c is the whole reading.

   Behaviour-neutral by construction, exactly as the retired comment above says:
   walk_window still draws all three from its own render phase, so this occupant
   removes and adds no pixel. It retires the day the render half of that file
   moves. */
static int __fastcall st_render(void *, void *)   { return 1; }

static int  __fastcall st_binit(void *s, void *)
{ return _ZN5Scene19ResetFadersAndSoundEv(s); }
static void __fastcall st_ainit(void *s, void *, unsigned a)
{ port_scene_after_init(s, a); }
static int  __fastcall st_bclean(void *s, void *)
{ return _ZN5Scene22BeforeCleanupResourcesEv(s); }
static void __fastcall st_aclean(void *s, void *, unsigned a)
{ _ZN5Scene21AfterCleanupResourcesEj(s, a); }
static int  __fastcall st_bbeh(void *s, void *)
{ return _ZN5Scene14BeforeBehaviorEv(s); }
static void __fastcall st_abeh(void *s, void *, unsigned a)
{ port_scene_after_behavior(s, a); }
static int  __fastcall st_bren(void *s, void *)
{ return _ZN5Scene12BeforeRenderEv(s); }
static void __fastcall st_aren(void *s, void *, unsigned a)
{ port_scene_after_render(s, a); }
static int  __fastcall st_v34(void *s, void *, unsigned a, unsigned b)
{ return ((ActorBase *)s)->ActorBase::Virtual34(a, b); }
static int  __fastcall st_v38(void *s, void *, unsigned a, unsigned b)
{ return ((ActorBase *)s)->ActorBase::Virtual38(a, b); }
static int  __fastcall st_heap(void *s, void *)
{ return port_scene_on_heap_created(s); }
static void __fastcall st_pdes(void *s, void *)
{ port_stage_on_pending_destroy(s); }

/* ---- SLOTS 3, 16 AND 17: THE LAST THREE TRAPS (run link100, lane STAGE) ----
 *
 * Every index below is read out of the ROM rather than inferred from the
 * ActorBase slot order, though that order agrees with all three:
 *
 *     from:0x020921cc kind:load to:0x0202c9a8 module:main   slot  3
 *     from:0x02092200 kind:load to:0x02023688 module:main   slot 16
 *     from:0x02092204 kind:load to:0x020236f0 module:main   slot 17
 *
 * and config/arm9/symbols.txt puts a matched TU on each destination:
 * _ZN5Stage16CleanupResourcesEv (0x0202c9a8), _ZN5StageD2Ev (0x02023688) and
 * _ZN5StageD0Ev (0x020236f0). port/slice_gate213.txt carries the enrolment
 * and the link measurement.
 *
 * SLOT 3 IS REACHABLE, and the first draft of this seat said it was not.
 * "Nothing dispatches any of the three" is true of 16 and 17 and FALSE of 3,
 * and the repo said so in four places, all of them written before this seat:
 *
 *   port/stage_lifecycle_map.txt section 5 -- lane VSEND MEASURED a VS
 *     match-end scene request reaching this dispatch, with the frame loop
 *     behind it then holding a destroyed world.
 *   port/stage_lifecycle_map.txt section 9 -- the live chain, link by link:
 *     seated slot 7 runs Scene::BeforeBehavior every frame; HitDeathPlane
 *     calls Scene::StartSceneFade(8, 0, 0), which is Scene::SetSceneToSpawn(8,
 *     0), which writes data_02092664 = 8; nothing in a level run spawns scene
 *     8, so the byte never returns to its 0x187 sentinel and from that frame
 *     on Scene::BeforeBehavior marks the Stage for destruction -- and one
 *     frame later the cleanup Process dispatches slots 4, 3 and 5.
 *   hal/star_flow.cpp -- its env switch is documented as existing BECAUSE the
 *     chain aborts at this slot.
 *   this file, above -- the slot-12 seat's own paragraph, which named slot 3
 *     as "the next trap in the same chain" and called the abort "the right
 *     place for it to stop".
 *
 * SO AN UNCONDITIONAL SEAT WOULD REMOVE A DELIBERATE SAFETY STOP. Before it, a
 * Stage marked for destruction hit a named abort. After it the ROM's real
 * teardown runs -- src/_ZN5Stage16CleanupResourcesEv.cpp releases twelve level
 * file handles, deletes the skybox and every area's TextureTransformer, zeroes
 * data_0209f314 (the area table) and data_0209f318 (the Camera), destroys the
 * wipe array, disables and resets the mesh collider and calls Deallocate --
 * and the port, which keeps ONE Stage alive across every level change
 * (hal/level_change.cpp) and never rebuilds it, would carry on running on the
 * torn-down level instead of stopping.
 *
 * SLOT 3'S ROM CALL IS THEREFORE ENV-GATED, in exactly the shape
 * hal/stage_slot0.cpp gates its own ROM path with: SM64DS_STAGE_SLOT3_ROM,
 * read once, PRESENCE MEANS ON. Unset -- which is every shipped run, every
 * battery row and every gate run -- leaves this thunk doing what the trap did:
 * the same reporter, the same "FATAL: Stage vtable slot 3 (CleanupResources)
 * is not hosted", the same abort. The two differ only in which function object
 * the dispatch lands in.
 *
 * THE GATE DOES NOT UNDO THE SEAT, and that is measured rather than hoped. The
 * word in the table is this thunk either way; the thunk names
 * port_stage_cleanup_resources on a branch the linker cannot prove dead, so
 * ?CleanupResources@Stage@@QAEHXZ stays in walk_window.map from its own
 * object and the linked count does not move. The proof already in the tree is
 * slot 0: ?InitResources@Stage@@QAEHXZ is in the map today behind exactly this
 * kind of getenv gate (hal/stage_slot0.cpp part 4, SM64DS_SLOT0_ROM).
 *
 * SLOTS 16 AND 17 TAKE NO GATE AND NEED NONE. They are reached by a delete
 * through the vptr, and nothing in the port deletes the Stage -- g_stage below
 * holds it for the process's life. Their seat changes no frame; what it
 * changes is that two words of the port's copy of the ROM's own table are the
 * ROM's own bodies instead of a named abort, which is the reference edge
 * /OPT:REF needs. Each keeps a ONE-SHOT stderr NOTE so that a dispatch nobody
 * expects is still named on the way through.
 */

/* THE GATE. Read once, so a run cannot change its mind halfway, and read for
   PRESENCE: SM64DS_STAGE_SLOT3_ROM=0 is still on. That is deliberately simpler
   than SM64DS_SLOT0_ROM, which parses a number because it has four modes to
   select between; slot 3 has one, so the only question is whether the variable
   is there. */
static int st_slot3_rom(void)
{
    static int on = -1;
    if (on < 0)
        on = std::getenv("SM64DS_STAGE_SLOT3_ROM") != 0;
    return on;
}

/* ---- THE SKYBOX'S OWN VTABLE, patched, one object only (run link100, lane
 * STAGEFIX) ------------------------------------------------------------
 *
 * WHAT RUNG 7 FOUND. With the gate open, the dispatch above reached the
 * ROM's own Stage::CleanupResources and it faulted -- not on a bad `this`,
 * not on a calling-convention mismatch at the dispatch itself (st_clean and
 * the probe agree on __fastcall(self,dummy), byte for byte, dumpbin-checked),
 * but three calls deep, inside Model::LoadTexAndPal, reading a near-null
 * BMD_File&. The chain, read out of the .obj disassembly and the matched
 * src it calls into:
 *
 *   1. src/_ZN5Stage16CleanupResourcesEv.cpp's DestroyVirt(o) is a faithful
 *      transcription of the ROM's own two-word Itanium vtable read:
 *      `((void(*)(void*))o[0][1])(o)` -- read the object's vptr, take SLOT 1,
 *      call it with the receiver. On real hardware slot 1 is always the
 *      deleting destructor (mwcc's Itanium ABI: slot 0 the complete-object
 *      D1, slot 1 the deleting D0), for every class, uniformly. That part of
 *      the translation is correct and this lane does not touch it.
 *   2. The object DestroyVirt is handed here is Stage+0x9bc, the skybox --
 *      confirmed live in this exact run ("skybox loaded by Stage::LoadSkybox,
 *      id 1, model set", src/_ZN5Stage10LoadSkyboxEv.c calling the ROM's own
 *      _ZN5ModelC1Ev) -- so `o[0]` is _ZTV5Model, the ONE shared table every
 *      Model in the game points at.
 *   3. _ZTV5Model[1] IS NOT A DESTRUCTOR ON THIS PORT. hal/cxxname_bridge.cpp
 *      seats it to mv_dosetfile (Model::DoSetFile) -- MSVC folds the ROM's
 *      D1/D0 pair into one slot (host index 0, hal/model_dtor_seat.cpp), and
 *      the freed index 1 already carries a REAL, load-bearing virtual every
 *      ModelBase::SetFile caller in the game depends on
 *      (port/hal/method_faces.cpp:199 names this exact fold as the reason
 *      Player::CleanupResources could not seat its own matched TU either --
 *      same hazard, same table, a different object). So DestroyVirt(sky)
 *      calls Model::DoSetFile(garbage_f, garbage_a, garbage_b): DestroyVirt
 *      only pushes the receiver, DoSetFile wants three more stack words, and
 *      whatever was sitting above CleanupResources's own frame is read as
 *      `f`. DoSetFile's first statement is AddToCommonModelDataArr(*f), which
 *      -- once the shared array already holds a real BMD (true by the time a
 *      level has rendered a frame) -- calls Model::LoadTexAndPal(*f). That
 *      is the exact function and the exact three-call depth the crash
 *      showed.
 *   4. _ZTV5Model CANNOT be renumbered -- it is the one array method_faces.cpp
 *      already ruled out, and every other Model in the game needs index 1 to
 *      stay DoSetFile. So the fix is not the shared table. It is this one
 *      object's own vptr.
 *
 * NOTHING ELSE IN THE PORT EVER READS THE SKYBOX'S VPTR. Both existing
 * consumers already bypass it: port_stage_render_skybox above calls
 * `sky->Model::Render(0)` qualified, and hal/level_boot.cpp's
 * port_level_stage_reseat destroys it by calling _ZN5ModelD2Ev and
 * Memory::Deallocate BY NAME. This gated dispatch is therefore the only
 * live vtable read this object will ever see, so giving it a private copy of
 * _ZTV5Model with ONLY slot 1 corrected is complete and changes nothing any
 * other object, class or dispatch can observe.
 *
 * st_skybox_vtable_fixup runs ONLY from st_clean's ROM branch below, so it
 * exists only when SM64DS_STAGE_SLOT3_ROM is set -- the default (unset) path
 * still returns st_trap(s, d) two lines up and never reaches this function.
 */
extern "C" {
void *_ZN5ModelD0Ev(void *self);
extern void *_ZTV5Model[];   /* storage in hal/model_host.cpp; shared, see
                                 hal/method_faces.cpp:199 -- do not renumber */
}

static void __fastcall stage_skybox_d0(void *s, void *) { _ZN5ModelD0Ev(s); }

static void *g_stage_skybox_vtable[8];

static void st_skybox_vtable_fixup(void *self)
{
    void **sky = (void **)((char *)self + 0x9bc);
    if (!*sky)
        return;                                   /* no skybox this level */
    if (*(void **)*sky != (void *)_ZTV5Model)
        return;                                    /* already patched, or not
                                                        the shape expected --
                                                        leave it alone */
    for (int i = 0; i < 8; ++i)
        g_stage_skybox_vtable[i] = _ZTV5Model[i];
    g_stage_skybox_vtable[1] = (void *)stage_skybox_d0;
    *(void **)*sky = g_stage_skybox_vtable;
}

/* ---- THE WIPE ARRAY'S OWN SUBSTITUTE, gated path only (run link100, lane
 * STAGEFIX) -------------------------------------------------------------
 *
 * Past the skybox fix above, the same dispatch reached
 * `func_02073244(data_0209f324, 0x60, 8, _ZN9FaderWipeD1Ev)` -- the WIPES
 * teardown -- and faulted again, this time inside the allocator
 * (ExpandingHeapAllocator::UnlinkNode on the address of this file's own
 * texxfm_d0 thunk, read as if it were a heap node). Two separate,
 * PRE-EXISTING host/ROM mismatches sit behind that one call, neither
 * introduced by this lane and both already named, in general terms, by
 * hal/level_boot.cpp's own banner ("func_02073244 over the FaderWipe array
 * ... the port has no answer for"):
 *
 *   1. NO COOKIE. hal/fader_wipes.cpp builds WIPES as a plain
 *      `HalFaderWipe hal_wipes[7]` -- an ordinary static array, no array-new
 *      header in front of it. port/unmatched/func_02073244_hostcopy.c (the
 *      host copy already linked in -- confirmed against walk_window.map)
 *      reads the element count from base[-1] and frees the whole block with
 *      `_ZdlPv(base - 8)`, exactly mirroring the ROM's own func_02073244
 *      (src/func_02073244.c: `func_0203cbc0(a - c)`). Base[-1] on a plain
 *      static array is whatever four bytes the linker put before it -- which
 *      is where texxfm_d0's own address came from -- and freeing static
 *      storage through _ZdlPv would be its own crash even with a real count.
 *   2. WRONG LAYOUT. _ZN9FaderWipeD1Ev (src/engine/fader/_ZN9FaderWipeD1Ev.c)
 *      is the ROM's own FaderWipe: vtable at +0, an embedded Model at +0x10.
 *      HalFaderWipe, the host class, is Fix12i/Fix12i/two shorts/model[0x50]
 *      raw bytes from +0xc -- offset +0x10 there is four bytes into that
 *      buffer, not a Model. Calling the matched destructor on a live
 *      HalFaderWipe reads those bytes as a Model vtable pointer.
 *
 * NEITHER is fixable by changing WIPES itself -- Scene::SetFaders,
 * FUN_02029980/FUN_020299f4 and every respawn/exit path need the live,
 * host-shaped array right where it is, unharmed, for the rest of the
 * process. And neither is this lane's job to redesign; the dispatch this
 * seat wired up runs AT PROCESS EXIT (port_stage_slot3_dispatch, above), so
 * nothing after it ever reads WIPES again. So, same shape as the skybox
 * fix: a THROWAWAY block, built through the cookie'd allocator the port
 * already trusts for this exact teardown (func_02073470 -- the pairing with
 * _ZdlPv is the one port/unmatched/func_02073244_hostcopy.c's own banner
 * already uses for Player's queue), zeroed rather than copied from the real
 * wipes because every write _ZN9FaderWipeD1Ev's whole chain makes (traced
 * through _ZN5ModelD1Ev, _ZN9ModelBaseD2Ev, _ZN5ColorD1Ev,
 * func_020177c4, func_02017838) is a vtable-pointer store, and every
 * resource field it checks first (ModelBase::res, Model::unk4C) is safe at
 * zero -- so a zeroed span is both sufficient and the one shape guaranteed
 * not to double-free something the real wipes still own. */
extern "C" {
void *func_02073470(int count, int size, int cookie,
                    void (*ctor)(void *), void (*dtor)(void *));
extern void *data_0209f324;   /* WIPES; storage in hal/fader_wipes.cpp */
}

static void st_wipes_vtable_fixup(void)
{
    void *fresh = func_02073470(7, 0x60, 8, 0, 0);
    if (!fresh)
        return;
    std::memset(fresh, 0, 7 * 0x60);
    data_0209f324 = fresh;
}

static int  __fastcall st_clean(void *s, void *d)
{
    if (!st_slot3_rom()) {
        /* THE SAFETY STOP, unchanged. Same reporter, same message, same
           abort -- st_trap is the trap thunks' own body, called here with the
           slot recorded the way HAL_STAGE_TRAP(3) records it. */
        hal_stage_trap_slot = 3;
        return st_trap(s, d);
    }
    static int noted;
    if (!noted) {
        noted = 1;
        std::fprintf(stderr, "[stage] slot 3: SM64DS_STAGE_SLOT3_ROM is set, so "
                     "the ROM's Stage::CleanupResources is running instead of "
                     "the named abort. The port keeps one Stage across every "
                     "level change and does not rebuild it -- see "
                     "port/stage_lifecycle_map.txt sections 5 and 9.\n");
    }
    st_skybox_vtable_fixup(s);
    st_wipes_vtable_fixup();
    return port_stage_cleanup_resources(s);
}

static void *__fastcall st_d2(void *s, void *)
{
    static int noted;
    if (!noted) {
        noted = 1;
        std::fprintf(stderr, "[stage] slot 16: ~Stage (D2) is running. Nothing "
                     "in the port was supposed to destroy the Stage.\n");
    }
    return _ZN5StageD2Ev(s);
}

static void *__fastcall st_d0(void *s, void *)
{
    static int noted;
    if (!noted) {
        noted = 1;
        std::fprintf(stderr, "[stage] slot 17: ~Stage (D0) is running -- the "
                     "Stage is being deleted and its storage returned.\n");
    }
    return _ZN5StageD0Ev(s);
}

extern "C" void hal_seat_stage_lifecycle(void)
{
    _ZTV5Stage[0]  = (void *)st_init;
    _ZTV5Stage[1]  = (void *)st_binit;
    _ZTV5Stage[2]  = (void *)st_ainit;
    _ZTV5Stage[3]  = (void *)st_clean;
    _ZTV5Stage[4]  = (void *)st_bclean;
    _ZTV5Stage[5]  = (void *)st_aclean;
    _ZTV5Stage[6]  = (void *)st_behavior;
    _ZTV5Stage[7]  = (void *)st_bbeh;
    _ZTV5Stage[8]  = (void *)st_abeh;
    _ZTV5Stage[9]  = (void *)st_render;
    _ZTV5Stage[10] = (void *)st_bren;
    _ZTV5Stage[11] = (void *)st_aren;
    _ZTV5Stage[12] = (void *)st_pdes;
    _ZTV5Stage[13] = (void *)st_v34;
    _ZTV5Stage[14] = (void *)st_v38;
    _ZTV5Stage[15] = (void *)st_heap;
    _ZTV5Stage[16] = (void *)st_d2;
    _ZTV5Stage[17] = (void *)st_d0;
    /* 18 and 19 stay trapped -- they are past the ROM's eighteen-word table,
       host storage only, and a dispatch there is a bug worth aborting on. */
    port_stage_seat_probe();
}

/* ---- THE SEAT PROBE, and its negative control ------------------------------
 *
 * SM64DS_STAGE_SEAT_PROBE=1 prints every one of the twenty words the fill just
 * wrote, each classified as TRAP or SEATED by comparing it against this file's
 * own trap-thunk table -- which is the only comparison that can tell the two
 * apart, because both are host addresses and neither is the ROM's.
 *
 * =2 IS THE NEGATIVE CONTROL and it is a real one: it re-installs the trap in
 * slots 3, 16 and 17 AFTER the seat and prints again, so a reader can see the
 * probe report TRAP for the three slots it reported SEATED a line earlier. A
 * probe that can only ever print SEATED proves nothing about the seat; this one
 * has to be able to print the other answer on the same binary in the same run.
 * Mode 2 leaves the traps installed, which is the pre-seat program -- it is a
 * control, not a mode anyone should ship in.
 *
 * SEATED IS ABOUT THE WORD, NOT ABOUT WHAT THE WORD DOES, and for slot 3 those
 * are two different questions since the gate above. The word is st_clean in
 * every build; whether a dispatch through it reaches the ROM body or the named
 * abort is SM64DS_STAGE_SLOT3_ROM's answer, so the probe prints that answer on
 * its own line rather than letting a reader infer it from "SEATED".
 */
extern "C" void port_stage_seat_probe(void)
{
    const char *e = std::getenv("SM64DS_STAGE_SEAT_PROBE");
    if (!e)
        return;
    const int mode = std::atoi(e);
    for (int pass = 0; pass < (mode >= 2 ? 2 : 1); ++pass) {
        if (pass == 1) {
            _ZTV5Stage[3]  = hal_stage_trap_thunk[3];
            _ZTV5Stage[16] = hal_stage_trap_thunk[16];
            _ZTV5Stage[17] = hal_stage_trap_thunk[17];
            std::printf("[stage-seat] NEGATIVE CONTROL: slots 3/16/17 put back "
                        "on the trap\n");
        }
        for (int i = 0; i < 20; ++i)
            std::printf("[stage-seat] slot %2d %-22s %s\n", i,
                        hal_stage_slot_name[i],
                        _ZTV5Stage[i] == hal_stage_trap_thunk[i] ? "TRAP"
                                                                 : "SEATED");
    }
    std::printf("[stage-seat] slot 3 gate: SM64DS_STAGE_SLOT3_ROM %s, so a "
                "dispatch reaches %s\n",
                st_slot3_rom() ? "SET" : "unset",
                st_slot3_rom() ? "the ROM's Stage::CleanupResources"
                               : "the named abort");
}

typedef int(__fastcall *StageSlot0)(void *self, void *dummy);

// ---- Particle::SysTracker::SysTracker -------------------------------------
//
// THE STUB THAT USED TO LIVE HERE IS GONE, and this file now carries none.
//
// Stage::Stage's second sub-object constructor is the particle system's, at
// Stage+0x50. It used to be an empty body that printed "the particle
// subsystem is not hosted" once, on the reasoning that the tracker was read
// only by Initialise and Update and neither ran.
//
// Gate 29 linked the real one. src/_ZN8Particle10SysTrackerC1Ev.c and its
// whole closure -- func_02021c90, Particle::SimpleCallback::SimpleCallback,
// func_020226a4, func_020225fc, and the thirteen data_0208f3xx vtables they
// install -- are in slice_gate29.txt. The vtables are hal/particle_vtable.cpp.
// Nothing is needed here any more: the constructor is ordinary matched src
// and the linker finds it in the slice.

// ---- construction ----------------------------------------------------------
//
// Stage::Stage is a FACTORY, not a placement constructor: it allocates its own
// 0x9c8 through ActorBase::operator new and returns the object, the same shape
// as the Player's and the Camera's factories in the registry.
//
// The one thing it needs staged is the spawn context. ActorBase::ActorBase
// reads data_020a4bb8[data_020a4b54] and drops the record's two halfwords into
// the processing-list nodes it just built, so the pending id has to be the
// Stage's and the table has to have the Stage's record under it. Both are put
// back afterwards: the registry owns that table for every other class, and the
// Stage is not spawned through the spine that would consult it.
//
// CAPTURED. This is a HOST MIRROR OF A WORLD POINTER -- the Stage lives in the
// hosted arena (3003a6b0) and port_stage_render_* reads it every frame -- and
// a save state rolls the arena back underneath it. It is right today for the
// same reason RELOAD's file-handle table looked right for thirty arms: the
// boot allocates the Stage first, so every launch of one build puts it at the
// same address, so the mirror happens to agree with the world it is mirroring.
// That is an agreement by coincidence of layout, not by construction, and it
// is exactly the argument hal/lk7_persist.cpp refutes for the handle table.
// Four bytes buys the construction. RELOADRV's reverse scan ([ss-rscan])
// named this symbol; this is the adjudication.
DSSTATE_BEGIN
static void *g_stage;
DSSTATE_END

extern "C" void *port_stage_object(void) { return g_stage; }

/* ---- THE SLOT-3 DISPATCH PROBE (run link100, lane STAGEFIX) ----------------
 *
 * SM64DS_STAGE_SLOT3_DISPATCH=1 dispatches _ZTV5Stage[3] on the Stage once,
 * through the table, the way the cleanup Process would. It exists because the
 * gate at st_clean has two answers and prose can show neither: with
 * SM64DS_STAGE_SLOT3_ROM unset the dispatch has to land on the named abort,
 * and with it set the dispatch has to land in the ROM's own body and come back
 * with the ROM's own 1. port/tools/stage_seat_proof.py rungs 6 and 7 are those
 * two runs on the same binary, which is what turns "gated" into a measurement
 * instead of a claim about a branch nobody took.
 *
 * AT EXIT, and that is the whole reason this is safe to arm at all. The ROM's
 * CleanupResources really does tear the level down -- twelve file handles
 * released, the skybox and every area transformer deleted, the area table and
 * the Camera zeroed, the collider disabled, Deallocate called -- so anything
 * that ran after it would be running on a destroyed world, which is exactly
 * what the gate exists to prevent. std::atexit registered from inside
 * port_stage_create runs after main returns and, being registered long after
 * every static constructor, runs before every static destructor and before any
 * handler the boot registered earlier. Nothing in the port runs behind it.
 *
 * A PROBE, not a mode anyone ships in -- the same standing as
 * SM64DS_STAGE_SEAT_PROBE=2, which also leaves the program in a state no
 * player should be in. */
static void port_stage_slot3_dispatch(void)
{
    if (!g_stage) {
        std::fprintf(stderr, "[stage-slot3] no Stage to dispatch on\n");
        return;
    }
    std::fprintf(stderr, "[stage-slot3] dispatching _ZTV5Stage[3] on %p "
                 "(SM64DS_STAGE_SLOT3_ROM %s)\n", g_stage,
                 st_slot3_rom() ? "SET" : "unset");
    std::fflush(stderr);
    const int r = ((StageSlot0)_ZTV5Stage[3])(g_stage, 0);
    std::fprintf(stderr, "[stage-slot3] Stage::CleanupResources returned %d\n",
                 r);
    std::fflush(stderr);
}

extern "C" void *port_stage_create(void)
{
    if (g_stage)
        return g_stage;

    hal_fill_stage_vtable();

    const unsigned stage_id = *(unsigned short *)(_ZN5Stage9spawnDataE + 4);
    if (stage_id != 3)
        std::fprintf(stderr, "  [stage] SPAWNDATA LOOKS WRONG: +4 is %u, the "
                     "ROM's Stage id is 3\n", stage_id);

    void *saved_info = data_020a4bb8[stage_id];
    unsigned short saved_pending = data_020a4b54;
    data_020a4bb8[stage_id] = _ZN5Stage9spawnDataE;
    data_020a4b54 = (unsigned short)stage_id;

    g_stage = _ZN5StageC3Ev();

    data_020a4bb8[stage_id] = saved_info;
    data_020a4b54 = saved_pending;

    if (!g_stage) {
        std::fprintf(stderr, "FATAL: Stage::Stage returned null\n");
        std::abort();
    }

    /* The tree head, asserted rather than assumed. The Stage is built with
       data_020a4b6c[0] still null, so func_0203b438 takes its no-parent branch
       and writes the Stage's own SceneNode into the head -- which is how the
       ROM's tree gets its root. If that ever stops holding, every actor
       spawned after it hangs off nothing and the phase-1 pass walks a null. */
    if (data_020a4b6c[0] != (int)(size_t)((char *)g_stage + 0x14))
        std::fprintf(stderr, "  [stage] TREE HEAD IS NOT THE STAGE: head %p, "
                     "Stage node %p\n", (void *)(size_t)data_020a4b6c[0],
                     (void *)((char *)g_stage + 0x14));

    /* ---- THE PAUSE BITS, and who really owns them -------------------------
       Stage::Stage sets its own +0x13 bits 0 and 2 -- verified instruction by
       instruction against the arm9 bytes at 0x0202e088, which really do read
       `ldrb; orr #1; strb; ldrb; orr #4; strb`. The Stage is built PAUSED and
       HIDDEN because on the ROM it is built while the level is still loading.

       That is not a detail the port can ignore, because the phase-1 pass
       (func_02043880) propagates a parent's bits 0|1 into every CHILD's bit 1,
       and ActorBase::BeforeBehavior returns 0 on bit 1. The moment the Stage
       became the scene root every actor under it -- the Player included --
       stopped ticking and sat at its spawn point.

       The ROM clears them in Scene::BeforeBehavior (0x0202e3d4, matched src
       _ZN5Scene14BeforeBehaviorEv.cpp), in exactly one place:

           if ((*(u8*)(self + 0x13) & 1) != 0) {
               if (func_020431c4(self) == 0) { *p13 &= ~1; *p13 &= ~4; }
               return 0;
           }

       which runs because the Stage is ON THE BEHAVIOUR PROCESSING LIST at
       priority 3 and its vtable slot 7 is that function. The port does not run
       the Stage as an actor yet, so nothing calls it.

       THIS LINE WAS A STAND-IN FOR THAT ONE STATEMENT and nothing else, and
       IT IS RETIRED (run link60, lane L4). The named job it asked for is done:
       _ZN5Scene14BeforeBehaviorEv is linked, func_020431c4 is linked,
       data_0209f5bc is a live FaderWipe from static init
       (hal/fader_wipes.cpp:296), and the Stage now takes the processing-list
       seat its SpawnInfo already described, so slot 7 clears the bits where
       the ROM clears them.

       WHAT THAT COSTS, measured rather than glossed: the ROM clears the bits
       only once func_020431c4 says every child of the Stage's SceneNode has
       finished initialising, so on a level entry the bits survive one pass of
       phase 1, which propagates them into every child's bit 1, and
       ActorBase::BeforeBehavior skips a child whose bit 1 is set. Actors are
       therefore one frame later to their first Behavior than they were under
       the hand-clear. That is what the ROM does; the battery is what says
       whether the port can tell.

       (data_0209f5bc is NOT a "scene manager", as this comment used to call
       it: Scene::SetFaders ends on `data_0209f5bc = thiz` with thiz a
       FaderBrightness*, and dispatches ROM bytes 0x14 and 0x18 -- IsAtStart
       and IsAtEnd -- through it. It is the INSTALLED FADER, which is how
       hal/fader_wipes.cpp describes and defines it. Scene::BeforeBehavior
       dispatches three of its slots before it ever reaches the pause-bit
       branch.) */
    std::printf("[stage] flags +0x13 = 0x%02x out of the ctor "
                "(Scene::BeforeBehavior clears these now, slot 7)\n",
                *(unsigned char *)((char *)g_stage + 0x13));
    /* ---- the AREA TABLE, which Stage::InitResources owns ------------------
       Stage+0x8bc is the level's area table (stride 0xc: the area's
       TextureTransformer at +0, the "is this area showing" flag at +4).
       ShowArea/HideArea/IsAreaShowing all reach it through data_0209f314, and
       Stage::InitResources' own line is

           data_0209f314 = (void *)((char *)thiz + 0x8bc);

       The port had it pointed at a 64-entry static in hal/camera_bridges.cpp
       instead, seated back when no Stage existed. That static is exactly why
       Stage::RenderModel drew an empty world the first time it was switched
       on: Camera::InitResources calls ChangeArea with the entrance's area,
       ChangeArea calls ShowArea, and ShowArea was setting the flag in the
       HARNESS table while RenderModel read the Stage's own -- which is zero,
       and a zero flag means "this area is hidden", so every component of the
       castle got bit 31 set and the level vanished.
       Seated here, before the boot, because the Camera is spawned inside it. */
    data_0209f314 = (char *)g_stage + 0x8bc;

    std::printf("[stage] Stage actor %p (id %u, prio beh %u ren %u), "
                "collider +0x91c %p, model +0x86c %p\n", g_stage, stage_id,
                *(unsigned short *)(_ZN5Stage9spawnDataE + 4),
                *(unsigned short *)(_ZN5Stage9spawnDataE + 6),
                (void *)((char *)g_stage + 0x91c),
                (void *)((char *)g_stage + 0x86c));

    /* The slot-3 dispatch probe, armed here because this is the first moment
       there is a Stage to dispatch on. See its banner above port_stage_object:
       the call itself happens at exit, never during a frame. */
    if (std::getenv("SM64DS_STAGE_SLOT3_DISPATCH"))
        std::atexit(port_stage_slot3_dispatch);
    return g_stage;
}

// ---- the tree probe --------------------------------------------------------
//
// THE FLAG LAW, read back rather than assumed. Stage::Stage sets its own
// +0x13 bits 0 and 2, and the phase-1 pass (func_02043880) propagates a
// parent's bits 0|1 into every CHILD's bit 1 -- which ActorBase::BeforeBehavior
// reads as "skip this actor's behaviour entirely". So whether the Stage is
// merely the spawn parent (data_0209f5c0) or the actual SCENE-TREE parent
// (SceneNode+0x0) is the difference between a level that runs and one frozen
// at its spawn point.
//
// The two are NOT the same seat. func_0203b438 links a node under the parent
// node it is HANDED, which the ActorBase constructor takes from data_020a4b64,
// not from data_0209f5c0.
extern "C" {
int func_02043810(void *actor);
extern int data_020a4b64[];
}

extern "C" void port_stage_tree_probe(void *child, const char *what)
{
    char *s = (char *)g_stage;
    std::printf("[stage] tree: head %p, Stage node %p, Stage flags %02x, "
                "spawn parent node %p\n", (void *)(size_t)data_020a4b6c[0],
                (void *)(s + 0x14), *(unsigned char *)(s + 0x13),
                (void *)(size_t)data_020a4b64[0]);
    if (!child)
        return;
    char *c = (char *)child;
    void *parent = (void *)(size_t)func_02043810(child);
    std::printf("[stage] tree: %s %p node.parent %p -> parent actor %p%s, "
                "flags %02x\n", what, child, *(void **)(c + 0x14), parent,
                parent == (void *)s ? " (THE STAGE)" : "",
                *(unsigned char *)(c + 0x13));
}

// ---- the render face -------------------------------------------------------
//
// Stage::RenderModel is a real MSVC method compiled against include/Stage.h,
// so it lands in the map as ?RenderModel@Stage@@QAEXXZ and no C caller can
// spell it. Same face pattern as hal/method_faces.cpp.
#include "Stage.h"
#include "Animation.h"
extern "C" void port_stage_render_model(void *self)
{ ((Stage *)self)->Stage::RenderModel(); }
/* _ZTV5Stage slot 12. The ROM's Stage overrides OnPendingDestroy with an empty
   body (src/_ZN5Stage16OnPendingDestroyEv.cpp, 0x0202b8a0), so the face costs
   a call and nothing else; what it buys is that the two
   ActorBase::MarkForDestruction edges in Scene::BeforeBehavior land on the
   ROM's body instead of on the slot trap. */
extern "C" void port_stage_on_pending_destroy(void *self)
{ ((Stage *)self)->Stage::OnPendingDestroy(); }
/* _ZTV5Stage slot 3 (run link100, lane STAGE). Same shape and same reason as
   slot 12's face above: src/_ZN5Stage16CleanupResourcesEv.cpp is a real C++
   method against include/Stage.h and publishes only
   ?CleanupResources@Stage@@QAEHXZ, so the slot cannot hold a plain pointer to a
   C name that does not exist. The return value is the ROM's own 1, which the
   cleanup Process reads. */
extern "C" int port_stage_cleanup_resources(void *self)
{ return ((Stage *)self)->Stage::CleanupResources(); }
extern "C" void port_stage_render_model_transparent(void *self)
{ ((Stage *)self)->Stage::RenderModelTransparent(); }

/* THE LEVEL'S OWN TEXTURE ANIMATIONS -- the waterfall, and every other BTA
   the stage model carries. Two halves of Stage code own them and the port
   ran neither, so the waterfall held frame 0 (the 2026-08-07 session):

     1. Stage::InitResources calls Stage::LoadTextureTransformers, which
        news a TextureTransformer into the area table's +0 slot for every
        area entry that names a BTA.
     2. Stage::Render's first block advances every SHOWN area's transformer
        each frame -- gated on the same pause trio Actor::BeforeBehavior
        reads -- and RenderModel (which the port already runs) then applies
        the current frame to the model's materials.

   This bridge is both halves at the same seam. The load is lazy and re-runs
   when the level id changes, because the port's level change keeps the Stage
   alive and never runs Stage::CleanupResources -- the slots would otherwise
   alias the OLD level's freed BTA file on the first post-change frame. The
   clear leaks the 0x14-byte transformer objects the ROM's cleanup would
   delete; a level change costs at most a few hundred bytes until the Stage
   runs as a real actor and its own InitResources/CleanupResources pair owns
   this.

   THE DOUBLE-LOAD, for whoever seats Stage::InitResources. Right now this
   bridge is the ONLY caller of Stage::LoadTextureTransformers in the port --
   Stage::InitResources does not run here at all -- so there is no double load
   today and none of this is a live bug. But the matched Stage::InitResources
   calls LoadTextureTransformers itself (src/_ZN5Stage13InitResourcesEv.cpp
   line 388), into a Stage it has just constructed. Seat it without touching
   this file and BOTH fire on the same entry: InitResources loads into fresh
   slots, then the level id changes, then the guard below sees a new id, zeroes
   the slots InitResources just filled and loads a SECOND set. The first set is
   unreachable from that moment -- 0x14 bytes per animating area, per entry,
   with no CleanupResources to take them back.

   The two halves are already coupled through port_stage_anims_rearm(), which
   hal/level_boot.cpp calls when it memsets the area records (the self-warp
   case, where the level id alone cannot see that the BTA was released and
   re-loaded). So the seat is a PAIR, not a single change: whoever gives
   Stage::InitResources the real call has to drop this bridge's load half and
   the rearm call together, leaving only the per-frame advance below -- which is
   the half that belongs to Stage::Render and is the only half the ROM runs
   every frame. Doing one without the other is the leak above, or, if the rearm
   goes first, texture animation dead for the rest of the session (measured on
   levels 13, 7 and 1 -- see the comment at hal/level_boot.cpp:2823). */
extern "C" {
extern signed char data_0209f2f8;                /* current level id */
extern int data_0209f294[], data_0209f2c4[], data_0209f20c[];
extern unsigned char *data_0209f340;             /* the level's area info */
}
/* The lazy load's trigger, hoisted to file scope so the level change can arm
   it. -2 is "nothing loaded yet", a value no data_0209f2f8 ever takes.

   THE LEVEL ID ALONE IS NOT A GOOD ENOUGH TRIGGER, and a SELF-WARP is why:
   level A -> level A leaves data_0209f2f8 unchanged, so the guard below never
   fires, while the boot in between has released and re-loaded that level's
   BTA. The surviving slots then point at transformers built over the freed
   copy of the file -- precisely the aliasing this guard exists to prevent, in
   the one case the guard cannot see. port_level_stage_reseat arms it
   explicitly per entry rather than leaving it to be inferred. */
static int port_stage_anims_loaded_level = -2;

extern "C" void port_stage_anims_rearm(void)
{
    port_stage_anims_loaded_level = -2;
}

extern "C" void port_stage_advance_anims(void *self)
{
    unsigned char *info = data_0209f340;
    if (!info)
        return;
    const unsigned n = info[0x14];               /* area count */
    char *slots = (char *)self + 0x8bc;          /* stride 0xc: anim, flag */

    int &loaded_level = port_stage_anims_loaded_level;
    if (loaded_level != (int)data_0209f2f8) {
        loaded_level = (int)data_0209f2f8;
        for (unsigned i = 0; i < n; ++i)
            *(void **)(slots + i * 0xc) = 0;     /* drop the old level's */
        ((Stage *)self)->Stage::LoadTextureTransformers();
        int live = 0;
        for (unsigned i = 0; i < n; ++i)
            if (*(void **)(slots + i * 0xc))
                ++live;
        std::printf("[stage] texture transformers: %d of %u areas animate "
                    "(level %d)\n", live, n, loaded_level);
    }

    if ((data_0209f294[0] | data_0209f2c4[0] | data_0209f20c[0]) & 0xff)
        return;
    for (unsigned i = 0; i < n; ++i) {
        Animation *a = *(Animation **)(slots + i * 0xc);
        if (a && *(unsigned char *)(slots + i * 0xc + 4))
            a->Animation::Advance();
    }
}

/* The SKYBOX draw, which is Stage::Render's own block between RenderFog and
   RenderModel (src/_ZN5Stage6RenderEv.cpp): the Model that LoadSkybox parked
   at Stage+0x9bc, its matrix translation (mat4x3 words 9..11 at +0x1c) glued
   to the camera EYE -- Camera+0x8c, the Vector3 Camera::Render hands LookAt_
   -- shifted from world fix12 into scene units by the same >> 3 the view
   matrix gets. Draw(0) on the ROM is the model vptr's slot 5, which is
   Model::Render(const Vector3 *) with a NULL scale, so the qualified call is
   the same dispatch. NULL Stage+0x9bc means GetSkyboxID said "no skybox"
   (LoadSkybox's early return) and the block is a no-op, same as the ROM. */
#include "Model.h"
extern "C" { extern void *data_0209f318; }   /* the Camera singleton */
extern "C" void port_stage_render_skybox(void *self)
{
    Model *sky = *(Model **)((char *)self + 0x9bc);
    if (!sky)
        return;
    int *m = (int *)((char *)sky + 0x1c);
    const int *eye = (const int *)((const char *)data_0209f318 + 0x8c);
    m[9] = eye[0] >> 3;
    m[10] = eye[1] >> 3;
    m[11] = eye[2] >> 3;
    sky->Model::Render(0);
}

/* ---- THE ROUTE IN, and the one place it does not take the ROM's own door ---
 *
 * hal/level_boot.cpp's port_stage_a_boot calls this instead of running its
 * body directly, so the level boot is dispatched through _ZTV5Stage slot 0 by
 * the ROM's own code rather than called by name.
 *
 * FIRST ENTRY: the Stage's aliveState (+0x0e) is 0, so func_020433b8 -- the
 * exact call func_02043098 makes for every other class -- runs the init
 * Process over slots 1/0/2 and then does its own list bookkeeping.
 * ActorBase::AfterInitResources(2) puts node +0x28 on data_020a4b78 and node
 * +0x38 on data_020a4b98, which is how the Stage reaches the behaviour and
 * render passes it has never been on.
 *
 * EVERY LATER ENTRY DISPATCHES SLOT 0 ALONE, and the reason is a corruption
 * that was traced rather than feared. On the ROM a level change tears the
 * Stage down and builds a new one; the port keeps ONE Stage alive across every
 * warp (hal/level_change.cpp). So on a second entry the Stage is already on
 * the behaviour and render lists, and ActorBase::AfterInitResources(2) opens
 * with
 *     func_0203b27c(data_020a4b88, this + 0x28);
 * -- unlink node +0x28 from the INIT list. func_0203b27c splices by the node's
 * own prev/next and touches the list's head/tail only when the node is at an
 * end, so handed the wrong list it unlinks the node from data_020a4b78 while
 * leaving data_020a4b78's head or tail still pointing at it. The func_0204405c
 * re-insert two lines later then walks from that stale head, finds the node's
 * own priority, and can splice it after itself. One warp, one self-looped
 * behaviour list, and every actor behind it stops.
 *
 * Running slot 0 through the vtable on those entries keeps the boot dispatched
 * by the ROM's own table on every level and declines only the Process wrapper
 * around it. Retiring this branch means giving the port a real Stage teardown,
 * which is slot 3, which is measured blocked (port/stage_lifecycle_map.txt
 * section 5). */
extern "C" void port_stage_lifecycle_boot(void)
{
    if (!g_stage) {
        std::fprintf(stderr, "FATAL: the level boot ran before the Stage\n");
        std::abort();
    }
    if (*(unsigned char *)((char *)g_stage + 0x0e) == 0) {
        func_020433b8(g_stage);
        return;
    }
    ((StageSlot0)_ZTV5Stage[0])(g_stage, 0);
}
