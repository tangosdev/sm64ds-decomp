// ---- THE FRAME LOOP HANDS Stage::Behavior BACK ---------------------------
//
// (run link100, lane FRAME.) port/stage_lifecycle_map.txt sections 12b and 12c
// named the one thing that had kept _ZTV5Stage slots 6 and 9 on host occupants
// through five Stage lanes, and it was never a missing symbol:
//
//     THE PORT'S FRAME LOOP IS THE STAGE.
//
// tests/walk_window.cpp carried Stage::Behavior transcribed statement by
// statement -- Stage::CheckInput in the input block, ShadowModel::CleanAll and
// ProcessKuppaScript above the actor tick, the message pump and the VS
// countdown below it -- with the freeze-mask latch in hal/actor_registry.cpp
// and the countdown body in hal/star_flow.cpp. Seating slot 6 on the ROM's own
// body without retiring those runs the cutscene script twice a frame.
//
// This file is the small half of that job: the FACES the ROM's body needs in
// order to be called at all, and the entry point hal/stage_bridges.cpp's slot-6
// thunk dispatches into. The frame-loop surgery itself is in
// tests/walk_window.cpp; the accounting is section 13.
//
// SLOT 9 IS NOT HERE. Stage::Render stays on its host occupant this lane, for
// the reason section 13's own slot-9 half measures.
//
// WHY A SEPARATE FILE AND NOT hal/stage_bridges.cpp. That file includes
// "Stage.h", and include/Stage.h is generated from matched-function evidence:
// it declares CleanupResources, LoadFog, LoadSkybox, LoadTextureTransformers,
// OnPendingDestroy, RenderFog, RenderModel and RenderModelTransparent, and it
// does NOT declare Behavior or Render. A translation unit cannot declare class
// Stage twice, so the members this file needs cannot be added beside that
// include -- and adding them to the generated header would put them in every
// TU that includes it. One file, one declaration, no header change.
//
// EVERY SPELLING BELOW IS READ OUT OF THE ROM TU THAT PUBLISHES IT, not
// guessed. MSVC encodes access, static-vs-instance, the calling convention and
// the return type into a decorated name, so `static void PS_Cleanup()` and
// `void PS_Cleanup()` are two different symbols and a face that picks the wrong
// one links against nothing. The table is:
//
//   what Stage::Behavior SPELLS                   what the matched TU PUBLISHES
//   ------------------------------------------   -----------------------------
//   ?Behavior@Stage@@QAEHXZ                       itself   (_ZN5Stage8BehaviorEv.cpp)
//   ?CheckCameraInput@Stage@@SAXXZ                itself   (_ZN5Stage16CheckCameraInputEv.cpp)
//   ?LC_Update@Stage@@SAXXZ                       itself   (_ZN5Stage9LC_UpdateEv.cpp)
//   ?PS_Update@Stage@@SAXXZ                       itself   (_ZN5Stage9PS_UpdateEv.cpp)
//   ?PS_Init@Stage@@SAXXZ                         itself   (_ZN5Stage7PS_InitEv.cpp -- the .cpp, see below)
//   ?StartSceneFade@Scene@@SAXIIG@Z               itself   (already in the image)
//   ?StopTimer@Timer@@QAEXXZ                      itself   (already in the image)
//   ?CleanAll@ShadowModel@@SAXXZ                  itself   (already in the image)
//   ---- these THREE the tree already had plumbing for; see below ----
//   ?CheckInput@Stage@@QAEXXZ                     _ZN5Stage10CheckInputEv
//   ?CanPause@Stage@@SAHXZ                        _ZN5Stage8CanPauseEv
//   ?IsPauseDisabled@Stage@@SAHXZ                 _ZN5Stage15IsPauseDisabledEv
//   ---- and these FOUR are what this file is for ----
//   ?PS_Cleanup@Stage@@SAXXZ                      _ZN5Stage10PS_CleanupEv       (flat C name)
//   ?VE_Init@Stage@@SAXXZ                         _ZN5Stage7VE_InitEv           (flat C name)
//   ?VE_Update@Stage@@SAXXZ                       _ZN5Stage9VE_UpdateEv         (flat C name)
//   ?SetSceneToSpawn@Scene@@SAXII@Z               _ZN5Scene15SetSceneToSpawnEjj (flat C name)
//
// THE FOUR ARE A PURE SPELLING DIFFERENCE and the faces below are one-line
// forwarders. All are __cdecl on both sides -- a `static` member is __cdecl in
// MSVC, and so is a flat C function -- so each forward is a call and nothing
// else.
//
// THE THREE ABOVE THEM ARE NOT THIS FILE'S, AND THAT IS A CORRECTION.
// This file carried faces for them and the build refused it:
// port/tools/alternatename_guard.py reported three NEW defeated aliases,
// because hal/w8a_stage_faces.cpp:133-136 has carried
//
//     /alternatename:?CanPause@Stage@@SAHXZ=__ZN5Stage8CanPauseEv
//     /alternatename:?IsPauseDisabled@Stage@@SAHXZ=__ZN5Stage15IsPauseDisabledEv
//     /alternatename:?CheckInput@Stage@@QAEXXZ=__ZN5Stage10CheckInputEv
//
// since lane w8a, and a face DEFINES the left-hand side, which makes an alias
// inert. That file says exactly what it was waiting for: "ALL FIVE ARE INERT
// TODAY. Their LHS is referenced only by Stage::Behavior, Stage::Render and
// Stage::RenderNumber, none of which is in this lane's slice ... They land now
// with their provenance so the piece that adds those bodies adds translation
// units and not plumbing." This is that piece. The faces are deleted and the
// aliases fire. ?CheckInput@Stage@@QAEXXZ is __thiscall against a __cdecl
// target, which the alias survives for the reason a face would have: neither
// spelling takes a stack argument and both clean zero bytes, so the only
// difference is an ecx the flat-C body never reads.
//
// AND ONE MORE NAME IS NOT A SPELLING DIFFERENCE AT ALL, AND MUST NOT BE READ
// AS ONE.
// ?UpdateMessage@Stage@@SAXXZ is what Stage::Behavior spells;
// src/_ZN5Stage13UpdateMessageEv.cpp declares the method NON-static and so
// publishes ?UpdateMessage@Stage@@QAEXXZ. Renaming that gap away would be a
// mistake, because the matched TU is a STUB in a second and worse way:
// it defines its OWN empty Message::UpdateWindow, Message::Update,
// Message::DisplaySaving and SaveData::SaveCurrentFile locally and calls those,
// so linking it would tick nothing at all -- the message box would be driven by
// four empty bodies. hal/message_pump.cpp is the port's answer to that and has
// been since it was written: port_message_pump IS Stage::UpdateMessage's body,
// statement for statement, calling the REAL matched Message methods. So the
// face points there. The stub stays out of the link, and hal/message_pump.cpp's
// header keeps the whole derivation.
//
// PS_Init HAS TWO FILES WITH THE SAME STEM. src/_ZN5Stage7PS_InitEv.c is flat C
// and publishes _ZN5Stage7PS_InitEv; src/_ZN5Stage7PS_InitEv.cpp declares
// `struct Stage { static void PS_Init(); }` and publishes ?PS_Init@Stage@@SAXXZ,
// which is what Stage::Behavior calls. config/match_attempts.jsonl records the
// .cpp as the matching attempt (0 divergences). port/slice_gate220.txt enrols
// the .cpp ONLY -- both would give CMake two objects with one name in one
// directory -- and there is deliberately no face here, because a face would
// make the choice look optional.

#include <cstdio>
#include <cstdlib>

extern "C" {
/* the flat-C matched bodies the faces below forward to */
void _ZN5Stage10PS_CleanupEv(void);
void _ZN5Stage7VE_InitEv(void);
void _ZN5Stage9VE_UpdateEv(void);
void _ZN5Scene15SetSceneToSpawnEjj(unsigned int a, unsigned int b);
/* Stage::UpdateMessage's body, on the port: hal/message_pump.cpp */
void port_message_pump(void);
}

/* The class, declared exactly the way the ROM TU declares it -- public
   members, non-static Behavior, static everything else. This is a DECLARATION
   of the ROM's own methods, not a second definition of the class the port uses;
   nothing here has storage. */
class Stage {
public:
    /* the ROM body this lane seats. Defined in src/, not here. */
    int  Behavior();
    /* the faces */
    static void PS_Cleanup();
    static void UpdateMessage();
    static void VE_Init();
    static void VE_Update();
};

/* Scene::SetSceneToSpawn is the same shape one class over. Stage::Behavior's
   level-change arm spells the static member ?SetSceneToSpawn@Scene@@SAXII@Z,
   and src/_ZN5Scene15SetSceneToSpawnEjj.c -- already in the image on
   port/slice_gate10.txt -- is flat C. Declared here rather than taken from a
   header for the same reason class Stage is: this file must not pull in a
   second declaration of either class. */
class Scene {
public:
    static void SetSceneToSpawn(unsigned int a, unsigned int b);
};

/* ---- the faces ---------------------------------------------------------- */

void Stage::PS_Cleanup()            { _ZN5Stage10PS_CleanupEv(); }
void Stage::VE_Init()               { _ZN5Stage7VE_InitEv(); }
void Stage::VE_Update()             { _ZN5Stage9VE_UpdateEv(); }
void Stage::UpdateMessage()         { port_message_pump(); }
void Scene::SetSceneToSpawn(unsigned int a, unsigned int b)
{ _ZN5Scene15SetSceneToSpawnEjj(a, b); }

/* ---- TWO MORE FACES, AND THEY ARE NOT SPELLING FIXES --------------------
 *
 * The eleven above forward one __cdecl name to another and an /alternatename
 * would have done the same job. These two would NOT: both targets are
 * __thiscall INSTANCE methods and both callers are __cdecl with the receiver
 * (or the argument) on the stack, so an alias would leave the callee reading
 * ecx for a receiver nobody put there and cleaning a number of bytes the
 * caller also cleans. That is the wrong-arity raw cast that smashes the stack
 * on first exercise, and it is why every row in the block generated for this
 * gate was checked for its convention rather than only for its name.
 *
 * Player::CanPause. src/func_02029408.c calls `func_020bd828(data_0209f394
 * [data_0209f250])` -- one __cdecl argument, the local player. 0x020bd828 is
 * _ZN6Player8CanPauseEv in config/arm9/overlays/ov002/symbols.txt line 435,
 * and src/_ZN6Player8CanPauseEv.cpp compiles it as a real C++ method against
 * Player.h, so it publishes ?CanPause@Player@@QAEHXZ and takes its receiver in
 * ecx. The face moves the argument into the receiver, which is what the ARM
 * call did with r0.
 *
 * Message::Display. Same shape and worth reading twice, because the recovered
 * source looks like it has an argument it does not use. src/_ZN7Message7Display
 * Ej.cpp declares `void Message::Display(unsigned int msg)` and its body never
 * mentions `msg` -- it passes `this` to _ZN7Message11DisplayTextEt, whose own
 * TU defines it as `void (unsigned short t)`. So the ROM's function takes ONE
 * value in r0 and the recovery modelled that value as the receiver; `msg` is
 * the register nothing wrote. The caller (src/_ZN5Stage9PS_UpdateEv.cpp:204,
 * `_ZN7Message7DisplayEj(data_020756d0[data_0209f2c8])`) passes exactly that
 * one value. The face therefore hands the argument in as the RECEIVER and
 * anything at all as msg, which reproduces the register state the ARM caller
 * left. */

class Message {
public:
    void Display(unsigned int msg);
};

class Player {
public:
    int CanPause();
};

extern "C" void _ZN7Message7DisplayEj(int m)
{ ((Message *)(void *)(unsigned)m)->Message::Display(0); }

extern "C" int func_020bd828(void *p)
{ return ((Player *)p)->Player::CanPause(); }

/* ---- THE ONE SEAM (run link100, lane FRAME) ------------------------------
 *
 * func_020199a4 is the only body in Stage::Behavior's closure this gate does
 * not enrol, and the reason is a wall rather than a preference.
 *
 * WHERE IT IS REACHED FROM: Stage::Behavior's VS arm, and only there --
 *
 *     u8 c9c = data_0209fc9c;
 *     if (c9c != 0) {
 *         func_02032f54();
 *         if (data_0209fcc8 >= 7u)
 *             func_020199a4();
 *         return 1;
 *     }
 *
 * so an adventure frame never evaluates the condition (the whole block is
 * inside `data_0209f2d8 == 1`) and a VS frame reaches it only past a second
 * gate.
 *
 * WHY IT IS NOT ENROLLED: its own closure is sixty-three further matched TUs
 * behind a thirty-six-symbol wall, and the wall is the DS's OWN HEAP
 * ALLOCATOR -- func_0204xxxx/func_0205xxxx, HeapAllocator, NestedHeapIterator
 * -- together with the IRQ and CP15 primitives underneath it. Four of those
 * leaves (WaitByLoop, func_02057014, func_02057198, func_02059d8c) are ARM
 * assembly the host compiler cannot assemble at all. The port has its own
 * allocator by design and hosting the cartridge's beside it is a memory-lane
 * decision, not a Stage seat's.
 *
 * IT IS LOUD, and that is the whole point of a seam over a stub. A silent
 * empty body would let a VS exit walk past the thing the port cannot do and
 * carry on; this prints once, names itself and names this comment, so the
 * first run that reaches it says so in the log. It does not abort: unlike
 * _ZTV5Stage slot 3, nothing downstream of this call depends on it having
 * happened -- Stage::Behavior returns 1 on the very next line either way. */
extern "C" void func_020199a4(void)
{
    static int said;
    if (!said) {
        said = 1;
        std::fprintf(stderr,
                     "[stageframe] SEAM: func_020199a4 (Stage::Behavior's VS "
                     "arm, data_0209fcc8 >= 7) is not hosted -- its closure is "
                     "the DS heap allocator and the IRQ primitives. See "
                     "port/slice_gate220.txt and hal/stage_frame.cpp.\n");
    }
}

/* ---- the entry point hal/stage_bridges.cpp dispatches into ---------------
 *
 * The slot thunks there are __fastcall with a dummy second parameter, which is
 * how every seat in that family carries a __thiscall receiver. The ROM's
 * Stage::Behavior is __thiscall int(void), so the thunk hands the receiver
 * straight through and returns what the ROM returns -- 1 on every path, which
 * is the "this pass succeeded" code the behaviour Process reads.
 *
 * THE COUNTER IS NOT DECORATION. port/tools/stage_pause_proof.py needs to know
 * that the ROM's body ran EXACTLY ONCE per frame, which is the whole hazard
 * port/stage_lifecycle_map.txt section 12b named: a seat that leaves a
 * transcribed statement behind in the frame loop runs the kuppa script twice
 * and every cutscene in the game plays at double speed. An exit code cannot see
 * that and a screenshot cannot either. It is incremented by the thunk itself,
 * so it cannot disagree with what ran. SM64DS_STAGE_FRAME_COUNT=1 prints the
 * total at process exit; unset, one increment is all this costs.
 *
 * AND THE FRAME LOOP READS IT LIVE. tests/walk_window.cpp compares it across a
 * frame to tell "the Stage ticked" from "it did not", which is the question its
 * three Stage-less frames (the debug menu, and the two no-spawn dev-rig arms)
 * raise and a DS frame never does. */

static unsigned g_beh_calls;

static void stage_frame_report(void)
{
    std::fprintf(stderr, "[stageframe] Stage::Behavior ran %u time(s)\n",
                 g_beh_calls);
}

static void stage_frame_arm(void)
{
    static int armed;
    if (armed)
        return;
    armed = 1;
    if (std::getenv("SM64DS_STAGE_FRAME_COUNT"))
        std::atexit(stage_frame_report);
}

extern "C" unsigned port_stage_behavior_calls(void) { return g_beh_calls; }

/* THE Ctrl BRIDGE RUNS HERE, and the placement is the point.

   The port hosts five interior fields of the ROM's Ctrl block as SEPARATE
   arrays (data_0209f4a0/a2/a4/a6/ac), so it needs a copy the cartridge does
   not: on hardware every actor reads data_0209f498 + N*0x18 directly, and
   Stage::Behavior's own CheckInput is what fills it. The copy therefore
   belongs at exactly the instant the ROM's write finishes -- after
   Stage::Behavior returns, before any other actor's Behavior runs -- and
   that instant is this line.

   tests/walk_window.cpp defines port_frame_ctrl_publish and its banner
   carries the measurement: the same copy placed BELOW port_actor_tick hands
   src/_ZN6Player8BehaviorEv.cpp:170 last frame's stick, which is a
   one-frame input lag on every walk in the game.

   NOT A CALL THE ROM MAKES, and this file is not going to blur that. It is a
   host bridge at a ROM boundary; it moves nothing of the ROM's and adds
   nothing to what Stage::Behavior does. */
extern "C" void port_frame_ctrl_publish(void);

extern "C" int port_stage_rom_behavior(void *self)
{
    stage_frame_arm();
    ++g_beh_calls;
    const int r = ((Stage *)self)->Stage::Behavior();
    port_frame_ctrl_publish();
    return r;
}
