// ===========================================================================
// C-linkage forwarder faces are LOAD-BEARING PLUMBING, not boilerplate.
//
// Every function below is a one-line forwarder that reads as trivial. Each one
// stands between the ROM's calling convention and the host's, and a face that
// looks correct can be silently wrong in three ways. All three have shipped or
// nearly shipped from this file; none is hypothetical.
//
//   1. WRONG TARGET. A face can forward to a plausible SIBLING instead of the
//      intended function. _Z14ApproachLinearRsss (below) forwarded to
//      ApproachLinear2, a near-identical copy 140 lines away that differs only
//      in whether it wraps to s16. It worked on every scalar and hung only on
//      angles, so it passed review and shipped (signs spinning forever). 156
//      src TUs reach ApproachLinear through this one face.
//
//   2. DROPPED RECEIVER. A method face reached in the zero-argument ()-form
//      drops the `this` the ROM delivered in a register. Actor::FarthestPlayer
//      calls _ZN8dActor_c13ClosestPlayerEv() with no receiver, so ClosestPlayer
//      reads this+0x5c off a null or garbage base. For a direct ClosestPlayer
//      reader that is a crash (the rabbit soft-lock); for FarthestPlayer it is
//      a SILENT wrong result, which is worse.
//
//   3. MIS-BRIDGED RECEIVER. A face entered by a thunk (or tail-called into)
//      with the object pushed on the stack must move it into ECX before the
//      real __thiscall body. Actor::OnTurnIntoEgg is a jmp tail-call into the
//      extern-C _ZN8dActor_c24KillAndTrackInDeathTableEv face (below), which is
//      correct only because that bridge is sized right. A cdecl body entered
//      with `this` still in ECX (or an aliased __thiscall body entered from a
//      cdecl frame) is the 2026-08-07 door-open crash; see the gate-22 note.
//
// PER-FACE REVIEW CHECKLIST. Verify each from the EMITTED bytes, never from the
// C signature (a different-signature sibling compiles fine through a face):
//   (a) TARGET  -- the call goes to the intended function, not a plausible
//                  sibling of the same shape.
//   (b) ARITY   -- the argument count matches the ROM callee's; a wrong-arity
//                  sibling still compiles clean.
//   (c) RECEIVER-- `this` is delivered: passed as the first argument for a
//                  dropped-receiver face, bridged into ECX for a thiscall face.
//
// These read as plumbing, reviewers skim plumbing, and each of the three above
// shipped or nearly shipped. Meet the class before you skim the next forwarder.
//
// HOW YOU CANNOT CHECK A FACE: BY LOOKING FOR IT IN THE MAP. (Written down at
// the wave-6 close, from lane w6-D's H5.) A face is usually the only reference
// to the body it forwards to, and it is usually one line. MSVC inlines the
// body into the face inside the same TU, and /OPT:REF then drops the external
// nothing references any more -- so the symbol is ABSENT from walk_window.map
// while the behaviour it carries is fully present, folded into the face.
// _ZN16dPathLiftActor_c12BaseBehaviorEv is the measured instance: PORT_HOST_ABI tagged
// on its definition, live in the binary, invisible as a public.
//
// Two consequences worth carrying. An absence from the map is NOT evidence
// that a face is missing, dead or unlinked -- prove those from the source list
// and the call sites instead. And linkage.py counts queue rows by symbol name,
// so a folded-away face is not counted; a by-name audit of the queue will
// undercount by exactly the faces the linker folded, and that is the tool
// being honest about the binary, not a bug.
// ===========================================================================
//
// C-linkage faces for METHOD-form definitions (gate 10/11 Behavior ring).
//
// The defining src files compile real methods against the shared headers;
// their .c-file callers reference Itanium C names. Each face forwards with
// a qualified call, the player_bridges pattern, batched here because the
// include surface spans most of the actor stack.
#include "dActor_c.h"
#include "fBase_c.h"
#include "dBgCh.h"
#include "Camera.h"
#include "dBgPi.h"
#include "dCc_c.h"
#include "dCcPos_c.h"
#include "Heap.h"
#include "Message.h"
#include "ModelBase.h"
#include "Model.h"
#include "ModelAnim2.h"
#include "OAM.h"
#include "PathPtr.h"
#include "Player.h"
#include "dBgCh_Lin.h"
#include "dBgCh_SphCrr.h"
#include "TextureSequence.h"
#include "Timer.h"
#include "dBgCh_Actr.h"

extern "C++" int ApproachLinear2(short &x, short target, short step);
/* src/_Z14ApproachLinearRsss.cpp -- a DIFFERENT function from ApproachLinear2,
   not a spelling of it. ApproachLinear wraps the difference back to s16
   (`(short)(x - target)`) both before and after the step, so on an ANGLE it
   turns the short way round and still recognises a crossing that happens
   through the s16 wrap. ApproachLinear2 compares in int and does neither. */
extern "C++" int ApproachLinear(short &x, short target, short step);

extern "C" {

int _Z15ApproachLinear2Rsss(short *x, short target, short step)
{ return ApproachLinear2(*x, target, step); }

/* Actor::SpawnSoundObj: a method since main rewrote its src file, while its
   nine callers across ov002/ov030/ov084/ov085 all still spell the Itanium
   name as a free function taking the actor. Void like the method and like the
   free function it replaced -- the callers that read a return value were
   already reading whatever Actor::Spawn left behind. */
void _ZN8dActor_c13SpawnSoundObjEj(void *self, u32 soundObjParam)
{ ((dActor_c *)self)->dActor_c::SpawnSoundObj(soundObjParam); }

/* Same rewrite, same story: a method now, spelled as a free function by every
   ov002 death path that calls it. */
void _ZN8dActor_c24KillAndTrackInDeathTableEv(void *self)
{ ((dActor_c *)self)->dActor_c::KillAndTrackInDeathTable(); }

/* Actor::FindWithID is static -- no `this`, so the face is a plain forward.
   hal/reverse_bridges.cpp used to run this the other way, wrapping a C-form
   src definition into a method; main made the src file the method, so the
   wrapper there is gone and this replaces it. */
dActor_c *_ZN8dActor_c10FindWithIDEj(u32 id)
{ return dActor_c::FindWithID(id); }

void _ZN10ModelAnim24CopyERKS_Pcj(void *self, const void *src, char *nf,
                                  unsigned nof)
{ ((ModelAnim2 *)self)->ModelAnim2::Copy(*(const ModelAnim2 *)src, nf, nof); }


void _ZN5dCc_c5ClearEv(void *self)
{ ((dCc_c *)self)->dCc_c::Clear(); }
void _ZN5dCc_c6UpdateEv(void *self)
{ ((dCc_c *)self)->dCc_c::Update(); }

void _ZN10dBgCh_Actr13SetGroundFlagEv(void *self)
{ ((dBgCh_Actr *)self)->dBgCh_Actr::SetGroundFlag(); }
void _ZN10dBgCh_Actr13SetLimMovFlagEv(void *self)
{ ((dBgCh_Actr *)self)->dBgCh_Actr::SetLimMovFlag(); }
void _ZN10dBgCh_Actr15ClearGroundFlagEv(void *self)
{ ((dBgCh_Actr *)self)->dBgCh_Actr::ClearGroundFlag(); }
void _ZN10dBgCh_Actr15ClearLimMovFlagEv(void *self)
{ ((dBgCh_Actr *)self)->dBgCh_Actr::ClearLimMovFlag(); }
void _ZN10dBgCh_Actr18StopDetectingWaterEv(void *self)
{ ((dBgCh_Actr *)self)->dBgCh_Actr::StopDetectingWater(); }
void _ZN10dBgCh_Actr19ClearAllGroundFlagsEv(void *self)
{ ((dBgCh_Actr *)self)->dBgCh_Actr::ClearAllGroundFlags(); }
void _ZN10dBgCh_Actr19StartDetectingWaterEv(void *self)
{ ((dBgCh_Actr *)self)->dBgCh_Actr::StartDetectingWater(); }

void _ZN15TextureSequence6UpdateER15ModelComponents(void *self, void *mc)
{ ((TextureSequence *)self)->TextureSequence::Update(
      *(ModelComponents *)mc); }

void _ZN5dBgCh19StartDetectingToxicEv(void *self)
{ ((dBgCh *)self)->dBgCh::StartDetectingToxic(); }
void _ZN5dBgCh21StopDetectingOrdinaryEv(void *self)
{ ((dBgCh *)self)->dBgCh::StopDetectingOrdinary(); }

void _ZN5Model14SetPolygonModeEi(void *self, int mode)
{ ((Model *)self)->Model::SetPolygonMode(mode); }

void _ZN5Timer10ResetTimerEv(void *self)
{ ((Timer *)self)->Timer::ResetTimer(); }
void _ZN5Timer10StartTimerEv(void *self)
{ ((Timer *)self)->Timer::StartTimer(); }
long long _ZN5Timer7GetTimeEv(void *self)
{ return ((Timer *)self)->Timer::GetTime(); }
void _ZN5Timer9StopTimerEv(void *self)
{ ((Timer *)self)->Timer::StopTimer(); }

int _ZN6Player12Unk_020c9e5cEh(void *self, unsigned char h)
{ return ((Player *)self)->Player::Unk_020c9e5c(h); }
int _ZN6Player16St_Shell_CleanupEv(void *self)
{ return ((Player *)self)->Player::St_Shell_Cleanup(); }
/* gate 194: func_ov002_020df7ac.c (one of HootTheOwl's ov002 carry-
   handshake leaves) calls this flat name directly -- gate 10 already links
   the matched src/_ZN6Player14St_Owl_CleanupEv.cpp body but nothing faced
   it before now. */
int _ZN6Player14St_Owl_CleanupEv(void *self)
{ return ((Player *)self)->Player::St_Owl_Cleanup(); }
void _ZN6Player18SetNewHatCharacterEjjb(void *self, unsigned a, unsigned b,
                                        unsigned char c)
{ ((Player *)self)->Player::SetNewHatCharacter(a, b, c != 0); }
void _ZN6Player18TurnOffToonShadingEj(void *self, unsigned j)
{ ((Player *)self)->Player::TurnOffToonShading(j); }
int _ZN6Player22IsBeingShotOutOfCannonEv(void *self)
{ return ((Player *)self)->Player::IsBeingShotOutOfCannon(); }
int _ZN6Player7IsInAirEv(void *self)
{ return ((Player *)self)->Player::IsInAir(); }
void _ZN6Player4HealEi(void *self, int amt)
{ ((Player *)self)->Player::Heal(amt); }


void _ZN7fBase_c18MarkForDestructionEv(void *self)
{ ((fBase_c *)self)->fBase_c::MarkForDestruction(); }

/* Gate 31: the Player's two DESTROY faces. Both definitions are real
   __thiscall methods -- Player::OnPendingDestroy in src, CleanupResources in
   port/unmatched/ -- and hal/level_boot.cpp seats them in vtable slots 12 and
   3, where the ROM's teardown dispatches them. Faces rather than
   /alternatename aliases for the usual reason: a slot thunk entered with the
   object in ecx would hand a cdecl body a `this` it never reads. */

/* PORT_HOST_ABI: matched TU deletes models through ROM vtable INDEX 1; MSVC folds D1/D0 so host _ZTV5Model numbers DoSetFile there.
   CleanupResources is the one of the pair whose body is a host copy
   (port/unmatched/Player_CleanupResources.cpp) and not the matched TU, which
   is why it is on the shadow list. That file's banner carries the full
   measurement; the two blockers, both structural:
     1. The matched TU deletes the body models, the head models and two more
        model-family objects through a two-virtual SHADOW CLASS, so `p->v1()`
        is ROM slot 1, mwcc's deleting destructor. MSVC folds D1 and D0 into
        one slot, so hal/cxxname_bridge.cpp's _ZTV5Model carries DoSetFile at
        index 1; every one of those five calls landed on Model::DoSetFile and
        faulted inside Model::AddToCommonModelDataArr on the first level
        teardown. Same class as the Bird/Flag ModelAnim tags below, and not
        fixable by renumbering one array: the MSVC-compiled callers of the
        same object need the MSVC order.
     2. The TU re-declares five functions include/decl_common.h already
        declares, `char *` against the header's `void *`. mwcc reads one
        declaration seen twice; MSVC reads an extern "C" overload and refuses
        the TU (C2733 x5, plus a C2664 on func_02073244's fourth parameter).
        Neither spelling is wrong about the ROM -- they disagree -- and src/
        and include/ are the byte-matched tree the port does not edit.
   Blocker 2 on its own would be a src/include hygiene question rather than an
   ABI one. Blocker 1 is the ABI floor and stands without it. */
int _ZN6Player16CleanupResourcesEv(void *self)
{ return ((Player *)self)->Player::CleanupResources(); }
void _ZN6Player16OnPendingDestroyEv(void *self)
{ ((Player *)self)->Player::OnPendingDestroy(); }

/* Gate 15: Actor::BeforeBehavior is a .c-style TU that calls its base by
   Itanium name, while the definition is a real __thiscall method. */
int _ZN7fBase_c14BeforeBehaviorEv(void *self)
{ return ((fBase_c *)self)->fBase_c::BeforeBehavior() ? 1 : 0; }

unsigned _ZNK7PathPtr8NumNodesEv(const void *self)
{ return ((const PathPtr *)self)->PathPtr::NumNodes(); }


}  /* extern "C" */



/* REVERSE faces: these St_ files define the ITANIUM C name; the state
   dispatcher references the MSVC method. Forward method -> C def. */
/* RETIRED at ALIAS2 (wave 8, the main -> port sync): both of these St_ TUs are
   real Player methods in main's tree now, so src/ defines
   ?St_GroundPound_Main@Player@@QAEHXZ and ?St_LongJump_Init@Player@@QAEHXZ
   itself and these faces were the second definition (LNK2005). The src body is
   the ROM's; the face only forwarded to it.
extern "C" int _ZN6Player19St_GroundPound_MainEv(void *self);
extern "C" int _ZN6Player16St_LongJump_InitEv(void *self);
int Player::St_GroundPound_Main()
{ return _ZN6Player19St_GroundPound_MainEv(this); }
int Player::St_LongJump_Init()
{ return _ZN6Player16St_LongJump_InitEv(this); }                             */

/* State Init refs the Main TUs call by Itanium name. Both are C linkage
   since main's mangled-declaration sweep, so the face defines the plain
   name and the aliases below cover the older C++ mangling. */
extern "C" void _ZN6Player14St_OnWall_InitEv(char *self)
{ ((Player *)self)->Player::St_OnWall_Init(); }
extern "C" void _ZN6Player17St_PunchKick_InitEv(void *self)
{ ((Player *)self)->Player::St_PunchKick_Init(); }
#pragma comment(linker, "/alternatename:?_ZN6Player14St_OnWall_InitEv@@YAXPAD@Z=__ZN6Player14St_OnWall_InitEv")
#pragma comment(linker, "/alternatename:?_ZN6Player17St_PunchKick_InitEv@@YAXPAX@Z=__ZN6Player17St_PunchKick_InitEv")
/* This face used to call ApproachLinear2 -- a copy of the body two hundred
   lines up, where that IS the right callee. The two ROM functions differ only
   on angles that wrap, so every caller that gates a state change on the
   arrival flag could stall: ApproachLinear2 turns the long way round and, when
   the crossing lands through the s16 wrap, never reports arrival at all. That
   is the SignPost read loop turning the player forever instead of opening its
   message box (sub-state 2, step 0x800, target = sign yaw + 0x8000: any sign
   placed within +/-0x800 of yaw 0 hangs for 25% to 99.95% of approach angles).
   156 src/ TUs reach ApproachLinear through this face. */
extern "C" int _Z14ApproachLinearRsss(short *x, short target, short step)
{ return ApproachLinear(*x, target, step); }
extern "C" int _ZN6Player15IsCollectingCapEv(char *self)
{ return ((Player *)self)->Player::IsCollectingCap(); }

/* gate-10 tier-2 wave: these St_ files define the ITANIUM C name, the
   state dispatcher calls the MSVC method. Forward method -> C def only;
   never the other way round for the same function. */
extern "C" int _ZN6Player18St_CameraZoom_MainEv(void *self);
extern "C" int _ZN6Player23St_MetalWaterWater_MainEv(void *self);
extern "C" int _ZN6Player15St_Respawn_InitEv(void *self);
extern "C" int _ZN6Player12St_Swim_MainEv(void *self);
extern "C" int _ZN6Player15St_Talk_CleanupEv(void *self);
/* RETIRED at ALIAS2 (wave 8, the main -> port sync): the thirteen rows below.
   Each of those St_ TUs is a real Player method in main's tree now, so src/
   defines ?St_<name>@Player@@QAEHXZ itself and the face here was the second
   definition (LNK2005). The five faces kept above are the ones whose src TU
   still defines only the flat Itanium C name, so they are still load bearing.
extern "C" int _ZN6Player15St_Balloon_MainEv(void *self);
extern "C" int _ZN6Player16St_BurnFire_InitEv(void *self);
extern "C" int _ZN6Player16St_BurnFire_MainEv(void *self);
extern "C" int _ZN6Player18St_DizzyStars_MainEv(void *self);
extern "C" int _ZN6Player19St_Electrocute_MainEv(void *self);
extern "C" int _ZN6Player18St_Grabbed_CleanupEv(void *self);
extern "C" int _ZN6Player12St_Hurt_MainEv(void *self);
extern "C" int _ZN6Player12St_Spin_MainEv(void *self);
extern "C" int _ZN6Player17St_SweepKick_InitEv(void *self);
extern "C" int _ZN6Player13St_Throw_InitEv(void *self);
extern "C" int _ZN6Player14St_Thrown_InitEv(void *self);
extern "C" int _ZN6Player19St_TornadoSpin_MainEv(void *self);                */
/* RETIRED at SYNC6, the same reason as the ALIAS2 block below and the one
   above it, one sync later: src/actors/Player.cpp is back on its seventeen
   slice rows now that main's #2666 made it compile on the host, and it
   defines ?St_<name>@Player@@QAEHXZ for all five of these itself. Measured:
   five LNK2005 rows against method_faces.cpp.obj in build_s2.log. The flat
   Itanium bodies these forwarded to are unchanged and still linked.
int Player::St_CameraZoom_Main()
{ return _ZN6Player18St_CameraZoom_MainEv(this); }
int Player::St_MetalWaterWater_Main()
{ return _ZN6Player23St_MetalWaterWater_MainEv(this); }
int Player::St_Respawn_Init()
{ return _ZN6Player15St_Respawn_InitEv(this); }
int Player::St_Swim_Main()
{ return _ZN6Player12St_Swim_MainEv(this); }
int Player::St_Talk_Cleanup()
{ return _ZN6Player15St_Talk_CleanupEv(this); }                              */
/* RETIRED at ALIAS2, same reason as the declarations above:
int Player::St_Balloon_Main()
{ return _ZN6Player15St_Balloon_MainEv(this); }
int Player::St_BurnFire_Init()
{ return _ZN6Player16St_BurnFire_InitEv(this); }
int Player::St_BurnFire_Main()
{ return _ZN6Player16St_BurnFire_MainEv(this); }
int Player::St_DizzyStars_Main()
{ return _ZN6Player18St_DizzyStars_MainEv(this); }
int Player::St_Electrocute_Main()
{ return _ZN6Player19St_Electrocute_MainEv(this); }
int Player::St_Grabbed_Cleanup()
{ return _ZN6Player18St_Grabbed_CleanupEv(this); }
int Player::St_Hurt_Main()
{ return _ZN6Player12St_Hurt_MainEv(this); }
int Player::St_Spin_Main()
{ return _ZN6Player12St_Spin_MainEv(this); }
int Player::St_SweepKick_Init()
{ return _ZN6Player17St_SweepKick_InitEv(this); }
int Player::St_Throw_Init()
{ return _ZN6Player13St_Throw_InitEv(this); }
int Player::St_Thrown_Init()
{ return _ZN6Player14St_Thrown_InitEv(this); }
int Player::St_TornadoSpin_Main()
{ return _ZN6Player19St_TornadoSpin_MainEv(this); }
extern "C" int _ZN6Player18St_YoshiPower_MainEv(void *self);
int Player::St_YoshiPower_Main()
{ return _ZN6Player18St_YoshiPower_MainEv(this); }                           */
/* St_Grabbed_Main calls DropActor by its Itanium name; the definition is a
   real method. Forward C name -> method (no face the other way). */
extern "C" int _ZN6Player9DropActorEv(void *self)
{ return ((Player *)self)->Player::DropActor(); }

/* gate 14: the init chain the actor spawn spine dispatches. Both are real
   __thiscall methods, so a linker alias onto the Itanium name their .c
   callers use would enter the body with `this` in whatever ecx held. */
extern "C" int _ZN8dActor_c18GetBitInDeathTableEv(void *self)
{ return ((dActor_c *)self)->dActor_c::GetBitInDeathTable(); }
extern "C" void _ZN8dActor_c18AfterInitResourcesEj(void *self, unsigned a)
{ ((dActor_c *)self)->dActor_c::AfterInitResources(a); }

/* gate 16: Actor::BeforeRender is the same shape -- a .c TU calling its base
   by Itanium name over a real __thiscall definition. Slot 10 of every actor
   class the registry carries goes through it. */
extern "C" int _ZN7fBase_c12BeforeRenderEv(void *self)
{ return ((fBase_c *)self)->fBase_c::BeforeRender(); }

/* wave 4 lane d: the same shape one slot over, for _ZTV8dScene_c slot 4.
   src/_ZN8dScene_c22BeforeCleanupResourcesEv.cpp is ordinary C and DOES pass its
   receiver (`_ZN7fBase_c22BeforeCleanupResourcesEv(thiz)`); only the
   C-linkage cdecl name was undefined. The matched ActorBase body is already in
   the link as ?BeforeCleanupResources@fBase_c@@UAEHXZ -- UAE, so a face and
   not an alias, and the checklist reads the same as its BeforeRender sibling
   above: qualified target (its near-sibling BeforeInitResources is a different
   slot and a different predicate), no arguments, receiver passed and cast. */
extern "C" int _ZN7fBase_c22BeforeCleanupResourcesEv(void *self)
{ return ((fBase_c *)self)->fBase_c::BeforeCleanupResources(); }

/* ---- wave 4 lane d: _ZTV8dScene_c slots 8 and 11, the two ROM TAIL VENEERS ----
   A no-op forwarder is exactly the kind of plumbing this file's header says
   gets skimmed, so here is the full reading before the two lines.

   src/_ZN8dScene_c13AfterBehaviorEj.cpp and src/_ZN8dScene_c11AfterRenderEj.cpp are
   ARM tail-call veneers -- `ldr ip,[pc]; bx ip; .word <ActorBase body>` -- so
   the ROM never gives them a frame at all: r0 (`this`) and r1 (the
   VirtualFuncSuccess code) ride straight through. The decompilation spells
   that as a void() calling a void(), which is the dropped-receiver shape this
   file lists as failure mode 2, and it is why wave 3 left both slots blocked.

   WHAT SETTLES IT IS THE CALLEE, not the veneer. Both ROM bodies are empty --

       void ActorBase::AfterBehavior(unsigned int vfSuccess_) { u32 v = ...; }

   and MSVC agrees, three bytes each, dumpbin /disasm on their own objects:

       ?AfterBehavior@fBase_c@@UAEXI@Z:   00000000: C2 04 00   ret 4
       ?AfterRender@fBase_c@@UAEXI@Z:     00000000: C2 04 00   ret 4

   -- so identical that /OPT:ICF folds them onto one address in the map
   (both at 0043dd10). Neither reads ecx, neither reads [ebp+8]. A dropped
   receiver cannot be wrong here in the way mode 2 means, because there is no
   read to be wrong: the failure mode needs a `this+0x5c`, and these have no
   loads at all.

   SO THIS DEFINITION IS THE BEHAVIOUR, EXACTLY, and it is correct for every
   caller rather than only for the Scene path. The only other src/ TU that
   spells either C name is the Actor veneer of the same pair
   (src/_ZN8dActor_c13AfterBehaviorEj.cpp, src/_ZN8dActor_c11AfterRenderEj.cpp,
   neither in the map today), and the ROM tail-jumps those to the SAME empty
   bodies -- so if a later lane links them, they resolve here and are still
   right.

   THE ONE THING IT IS NOT is a bridge into the matched body: the port's slot-8
   dispatch reaches this, not ?AfterBehavior@fBase_c@@UAEXI@Z. That gap is
   three bytes of `ret 4`, and both definitions are TAGGED rather than left to
   linkage.py's face heuristic, which would otherwise call them faces over a
   linked matched TU and be wrong about what they are. The fuller-fidelity
   alternative -- park the receiver and the argument in the slot adapter and
   have these read them back -- was weighed and NOT taken: it buys nothing
   observable over `ret 4` and adds a side channel with a live constraint to
   keep. */
/* PORT_HOST_ABI: the ROM body is `ret 4` and reads neither ecx nor its
   argument, so this IS its behaviour; the matched body is emitted
   __thiscall-with-an-argument and the veneer's own void() declaration cannot
   enter it without leaving ESP four bytes light on every dispatch. */
extern "C" void _ZN7fBase_c13AfterBehaviorEj(void) {}
/* PORT_HOST_ABI: same as AfterBehavior above -- `ret 4`, no receiver read, no
   argument read; /OPT:ICF folds the two matched bodies onto one address. */
extern "C" void _ZN7fBase_c11AfterRenderEj(void) {}

/* gate 16: ModelBase::ApplyOpacity is a real method whose only caller,
   Tree::Render, spells it as an Itanium C name (and passes a third argument
   the ROM's r2 carried into a two-parameter body; cdecl lets the caller keep
   cleaning it). */
extern "C" void _ZN9ModelBase12ApplyOpacityEj(void *self, unsigned a)
{ ((ModelBase *)self)->ModelBase::ApplyOpacity(a); }

/* Model::UpdateFileOffsets is a STATIC member (include/Model.h), which is why
   func_02016ff4 calls it with the file alone and no `this` -- the Itanium name
   is the same either way, so only the header says which. Face, not alias:
   MSVC decorates a static member differently again. */
extern "C" void _ZN5Model17UpdateFileOffsetsER8BMD_File(BMD_File *f)
{ Model::UpdateFileOffsets(*f); }

/* gate 16, the shrink-to-fit tail of Model::LoadAndSetFile: Heap::Reallocate
   is reached by Itanium name from func_02017060, and its face is below.
   (Heap::_Sizeof, the same tail's other Heap method, is no longer faced here:
   see the note above Reallocate's face.) */
/* gate 16: the actor teardown, FLIPPED. It used to be a host copy owning the
   Itanium name while this face supplied the MSVC method for the slot-5 thunks.
   The matched TU is in the binary now (hostgen --extern-data homes its three
   role-named engine globals on the HAL -- see GATE9_SYMS), and a matched TU
   that spells a real C++ member IS the MSVC method, so this face would be a
   duplicate of it. What is missing instead is the ROM's own C name, which the
   vtables and the Itanium call sites use: the face runs the other way now, the
   ordinary reverse-bridge shape. Qualified call, so the C name reaches
   ActorBase's body rather than re-dispatching through the vptr it was
   presumably called through. */
extern "C" void _ZN7fBase_c21AfterCleanupResourcesEj(void *self, unsigned a)
{ ((fBase_c *)self)->fBase_c::AfterCleanupResources(a); }

/* Heap::_Sizeof's face used to live here, RETIRED at run linkfull wave 27
   (lane P1). Its ruling was about a matched TU that no longer exists: the
   veneer at arm9 0x0203c274 (`ldr ip, [pc]; bx ip; .word 0x0203c454`) was
   transcribed as a void() calling a void() by its Itanium name, which drops
   both of the arguments ARM rides through r0 and r1 and named a callee the
   link did not define. src/_ZN4Heap7_SizeofEPv.cpp is now the real member
   `int Heap::_Sizeof(void *ptr) { return Sizeof(ptr); }`: the receiver and
   the pointer are spelled and passed, and the callee is the decorated
   ?Sizeof@Heap@@QAEHPAX@Z the link already carries. The matched TU is on
   port/slice_w28_p1.txt and the flat name its two callers spell is a reverse
   face in port/faces_sync.txt. */
extern "C" void _ZN4Heap10ReallocateEPvj(void *self, void *p, unsigned n)
{ ((Heap *)self)->Heap::Reallocate(p, n); }

/* gate 16, THE OTHER DIRECTION: CylinderClsnWithPos::Init is defined at C
   linkage in its own TU while Tree::InitResources declares it as a method on
   a local class shape and calls it __thiscall. An /alternatename would enter
   the cdecl body with `this` still in ecx, so this is a face. */
extern "C" void _ZN8dCcPos_c4InitERK7Vector35Fix12IiES4_jj(
    void *self, const void *pos, int radius, int height, unsigned flags,
    unsigned vulnFlags);
/* SYNC4: include/dCcPos_c.h declares this with the real Fix12<int> scalars
   (include/types.h keeps Fix12i as a plain s32), so the face follows the
   header and reads .val out of each. */
void dCcPos_c::Init(const Vector3 &pos, Fix12<int> radius,
                               Fix12<int> height, u32 flags, u32 vulnFlags)
{
    _ZN8dCcPos_c4InitERK7Vector35Fix12IiES4_jj(
        this, &pos, radius.val, height.val, flags, vulnFlags);
}


/* ---- gate 16: the collider faces the actor colliders need ---------------
   Three C-named references onto MSVC method definitions. The first two are
   the port's own transcriptions of the ITCM octree walks (port/unmatched/),
   which MovingMeshCollider's DetectClsn overrides call DIRECTLY -- not
   through a vtable -- after transforming the ray or sphere into the
   collider's own space. func_01ffb0fc is the line walk's ROM address, which
   is how the matched source spells it. */
#include "dBgW_Kc.h"
#include "dBgCh_SphCrr.h"
extern "C" {
int func_01ffb0fc(void *self, void *ray)
{ return ((dBgW_Kc *)self)->dBgW_Kc::DetectClsn(*(dBgCh_Lin *)ray); }
int _ZN7dBgW_Kc10DetectClsnER12dBgCh_SphCrr(void *self, void *sph)
{ return ((dBgW_Kc *)self)->dBgW_Kc::DetectClsn(*(dBgCh_SphCrr *)sph); }
int _ZN4dBgW9IsEnabledEv(void *self)
{ return ((dBgW *)self)->dBgW::IsEnabled(); }
int _ZN4dBgW7DisableEv(void *self)
{ return ((dBgW *)self)->dBgW::Disable(); }
}

/* ---- gate 16: five more C-named references onto method definitions -------
   The 1-up, the sign and the brick block reach these by their Itanium names
   from .c TUs; every one of them is a real __thiscall method in src. */
extern "C" {
int _ZN6Player14IsFrontSlidingEv(void *self)
{ return ((Player *)self)->Player::IsFrontSliding(); }
int _ZN6Player15IsEnteringLevelEv(void *self)
{ return ((Player *)self)->Player::IsEnteringLevel(); }
int _ZN6Player17LostGrabbedObjectEv(void *self)
{ return ((Player *)self)->Player::LostGrabbedObject(); }
int _ZN6Player20IsStateEnteringLevelEv(void *self)
{ return ((Player *)self)->Player::IsStateEnteringLevel(); }
}

#include "PowerStar.h"
extern "C" {
void _ZN10dBgCh_Actr20UpdateDiscreteNoLavaEv(void *self)
{ ((dBgCh_Actr *)self)->dBgCh_Actr::UpdateDiscreteNoLava(); }
void _ZN10dBgCh_Actr22UpdateDiscreteNoLava_2Ev(void *self)
{ ((dBgCh_Actr *)self)->dBgCh_Actr::UpdateDiscreteNoLava_2(); }
void _ZN9PowerStar13AddStarMarkerEv(void *self)
{ ((PowerStar *)self)->PowerStar::AddStarMarker(); }
}

/* ---- gate 17: the level overlay's own classes ---------------------------
   C-named references onto method definitions. The registry dispatches every
   one of them through a vtable slot, and every one is a real __thiscall
   method in src against its own generated header.

   CASTLE_WATER is not here: its four src files spell their own Itanium names
   in extern "C", so a face would be a second definition of each. */
#include "Bird.h"
#include "daObjMc_Metalnet_c.h"
#include "daMcFlag_c.h"
extern "C" {
int _ZN4Bird13InitResourcesEv(void *self)
{ return ((Bird *)self)->Bird::InitResources(); }
/* BIRD::RENDER AND FLAG::RENDER ARE RETIRED (run link100, lane PMFB2). Both
   are on port/slice_pmfb2.txt and compile from src/_ZN4Bird6RenderEv.cpp and
   src/game/actors/d_a_mc_flag.cpp; each matched TU recovered as a real C++ method
   (`int Bird::Render()`) while hal/actor_classes.cpp:1334 and :1506 call the
   Itanium C name, so each takes a cdecl face in hal/except_faces.cpp and NOT
   ONE FILL SITE CHANGES.

   WHAT CHANGED, because the block below says this could not be done. Both are
   one line in src, and that line is an INDEXED virtual call, not a named one:
   each declares a local six-virtual shadow struct over the ModelAnim member at
   +0xd4 and calls its sixth slot (`Base *b = &((Derived *)this)->base;
   b->m(0)`). When this block was written the host _ZTV9ModelAnim was filled in
   MSVC's FOLDED order -- one slot for the D1/D0 pair -- so index 5 landed on
   Virtual18, and seating the matched TU would have rendered through the wrong
   slot silently. hal/cxxname_bridge.cpp does not fill it that way any more:
   slots 0 and 1 carry the ROM's D1/D0 pair, seeded there and overwritten by the
   model dtor seat, and the fill reads

     _ZTV9ModelAnim[3] = ma2_updateverts   [4] = ma2_virtual10
     _ZTV9ModelAnim[5] = ma2_render        [6] = ma2_virtual18

   which is the ROM numbering include/ModelAnim.h annotates. Index 5 is Render
   on both machines now, so the stated precondition -- "retiring it needs a
   ROM-ordered ModelAnim table" -- is met by the tree as it stands, and the
   other half of the old objection went with it: the table is not dual-filled,
   so no genuine Virtual18 dispatch had to be broken to get here.

   RE-READ AT THE ADDRESS FOR THIS LANE out of extracted/overlays/
   overlay_0009.bin at ov009 base 0x021111a0 (runs/link100/out/PMFB2/
   rom_renders.txt). Both ROM bodies are the same ten instructions:
     02111870 _ZN4Bird6RenderEv   ldr r2,[r0,#0xd4]!   mov r1,#0
     0211211c _ZN10daMcFlag_c6RenderEv   ldr r2,[r2,#0x14]    blx r2    mov r0,#1
   `[r0,#0xd4]!` is pre-indexed WITH WRITEBACK, so the receiver at the blx is
   the ModelAnim sub-object and not the actor; `[r2,#0x14]` is vtable slot 5;
   `mov r1,#0` is the one argument, which ma2_render lands as
   ModelAnim::Render((const Vector3 *)0). MSVC compiles the matched TU's
   `b->m(0)` as a __thiscall indirect call -- receiver in ecx, the 0 pushed,
   callee cleans four -- and ma2_render is
   __fastcall(void *self, void *unused, const void *s): same receiver register,
   same stack argument, same cleanup. The shadow's `void m(int)` and the ROM's
   own `mov r0,#1` agree that the return value is discarded.

   THE READING THAT WAS OVERTURNED IS KEPT BELOW, because a later lane meeting
   a still-dual-filled table needs to be able to see the shape that was here.

   Two lanes (w8-faces and w8-shadows) re-derived this independently and agree,
   so both are TAGGED rather than left to linkage.py's shadow heuristic, which
   counts them as replaceable work. The two derivations come at the same fact
   from opposite ends and are both kept:

   THE TWO NUMBERINGS. include/ModelAnim.h annotates the ROM table as D1 0,
   D0 1, UpdateVerts 3, Virtual10 4, Render 5, Virtual18 6. The host array is
   filled in MSVC order instead, because real MSVC-compiled C++ TUs dispatch
   ModelAnim through MSVC indices and one array cannot carry both numberings.
   From the host fills rather than from the prose (hal/cxxname_bridge.cpp,
   :549-554 for the ModelAnim rows):

     hal/cxxname_bridge.cpp   _ZTV5Model[4] = mv_render
                              _ZTV5Model[5] = mv_render      <- dual-filled
                              _ZTV9ModelAnim[4] = ma2_render
                              _ZTV9ModelAnim[5] = ma2_virtual18

   MSVC folds D1 and D0 into one slot, which is what shifts everything: [0]
   dtor, [1] DoSetFile, [2] UpdateVerts, [3] Virtual10, [4] Render, [5]
   Virtual18. So the matched TU's index 5 lands on Virtual18.

   IT IS MEASURED, NOT PREDICTED. include/Bird.h:25 and include/Flag.h:17 both
   put a ModelAnim at +0xd4, and ModelAnim's host table is NOT dual-filled the
   way _ZTV5Model is -- so the matched body dispatches Virtual18. That is
   failure mode 1 arriving through the src body instead of through the face,
   and port/slice_gate17.txt records FLAG's first Render actually walking
   ModelAnim::Virtual18 -> Virtual10 -> Model::Virtual10 into a null-matrix
   fault.

   NOT RETIRABLE by seating the matched TU: it would compile and link and then
   render through the wrong slot, a silent wrong result rather than a link
   error. Dual-filling _ZTV9ModelAnim slot 5 the way _ZTV5Model is would fix
   these two by breaking every genuine Virtual18 dispatch, so there is no
   wiring that makes the matched TU behave here. Retiring it needs a
   ROM-ordered ModelAnim table, which is the opposite of what every MSVC caller
   of the same object needs. */
int _ZN18daObjMc_Metalnet_c13InitResourcesEv(void *self)
{ return ((daObjMc_Metalnet_c *)self)->daObjMc_Metalnet_c::InitResources(); }
int _ZN18daObjMc_Metalnet_c8BehaviorEv(void *self)
{ return ((daObjMc_Metalnet_c *)self)->daObjMc_Metalnet_c::Behavior(); }
int _ZN18daObjMc_Metalnet_c6RenderEv(void *self)
{ return ((daObjMc_Metalnet_c *)self)->daObjMc_Metalnet_c::Render(); }
int _ZN18daObjMc_Metalnet_c16CleanupResourcesEv(void *self)
{ return ((daObjMc_Metalnet_c *)self)->daObjMc_Metalnet_c::CleanupResources(); }
int _ZN10daMcFlag_c13InitResourcesEv(void *self)
{ return ((daMcFlag_c *)self)->daMcFlag_c::InitResources(); }
int _ZN10daMcFlag_c8BehaviorEv(void *self)
{ return ((daMcFlag_c *)self)->daMcFlag_c::Behavior(); }
/* _ZN10daMcFlag_c6RenderEv is retired with Bird's, on the same measurement.
   include/Flag.h:17 puts its ModelAnim at +0xd4 too and
   src/game/actors/d_a_mc_flag.cpp is byte-for-byte the same shape -- the same local
   six-virtual shadow, the same `b->m(0)` at index 5. Its face is in
   hal/except_faces.cpp with Bird's. */
}

/* ---- gate 143: level 11's own class, IceSlideManager (ov019) -------------
   The Cool Cool Mountain slide manager (actor 356). Its InitResources and
   Behavior are real __thiscall methods in src (.cpp against the generated
   IceSlideManager.h), dispatched through vtable slots 0 and 6 by the
   host-filled _ZTV10daSldMng_c (hal/actor_classes_ccm.cpp). Its D1/D0 are
   not faced: slot 16 is the empty-~Actor form the vtable fill spells inline and
   slot 17 traps. */
#include "daSldMng_c.h"
extern "C" {
int _ZN10daSldMng_c13InitResourcesEv(void *self)
{ return ((daSldMng_c *)self)->daSldMng_c::InitResources(); }
int _ZN10daSldMng_c8BehaviorEv(void *self)
{ return ((daSldMng_c *)self)->daSldMng_c::Behavior(); }
}

/* TextureTransformer: two slots (the destructor pair) and nothing else, so
   the vtable the constructor installs is storage. Its Prepare and Update are
   methods; the water reaches both by their Itanium names. */
#include "TextureTransformer.h"
extern "C" {
void *_ZTV18TextureTransformer[4];
/* Two arguments, not three, and the reason is in the header rather than in
   this file: include/TextureTransformer.h:42 declares Prepare STATIC.
   func_02046b64 resolves the BTA's own material NAMES against the BMD's table,
   and the water's call site passes exactly those two with no `this`.

   THE ROM AGREES: arm9_dec.bin 0x0201587c is e59fc000 / e12fff1c / .word
   0x02046b64, a 0xc long-call veneer that passes its registers straight into
   func_02046b64, whose own matched TU (src/func_02046b64.c) takes exactly two.
   No receiver anywhere in the chain.

   THIS USED TO BE A HOST COPY of that one line -- it called func_02046b64
   itself, which made the HAL the implementation of a ROM function src/ already
   carries, and left the matched TU unlinked beside it on the shadow list. A
   static member is __cdecl under MSVC, so the matched TU's symbol
   (?Prepare@TextureTransformer@@SAXAAUBMD_File@@AAUBTA_File@@@Z) has the same
   two-argument, no-`this` call surface this face already had: the body it
   stood in for could simply be called. It forwards to the matched TU now.

   NOT a dropped receiver, and worth spelling out because a two-argument call
   into a `Class::Method` shape is what failure mode 2 looks like from the
   outside. There is no `this` to drop. MSVC emits it SA (plain __cdecl, two
   stack arguments) and the face's emitted bytes are two pushes, no ecx write,
   one call -- checked in the object, which is this file's rule.
   port/slice_w1l3.txt:238 had parked the TU on the opposite reading ("the ROM
   veneer takes (bmd, bta) with no `this`, so the matched METHOD cannot be
   called"); the `static` on that declaration is what refutes it.

   Contrast TextureSequence::Prepare, the sibling veneer 0x100 away, which
   include/TextureSequence.h:49 declares NON-static: that one is a real
   __thiscall method and its callers still need the receiver seam.

   WHICH SLICE SEATS IT. src/_ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_
   File.cpp is seated by slice_w8faces.txt:43, and by that file alone. Lanes
   w8-faces and w8-shadows both mined this symbol, reached the same verdict
   from different evidence (the paragraphs above are both of them), and each
   wrote the same seat line into its own slice. Both slices feed the same three
   targets, so the integration merge kept the w8-faces line and dropped the
   w8-shadows one; slice_w8shadows.txt records the removal and the reason. */
void _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(void *bmd, void *bta)
{ TextureTransformer::Prepare(*(BMD_File *)bmd, *(BTA_File *)bta); }
void _ZN18TextureTransformer6UpdateER15ModelComponents(void *self, void *mc)
{ ((TextureTransformer *)self)->TextureTransformer::Update(
      *(ModelComponents *)mc); }

/* Model::LoadAndSetFile, for the boot spine (gate 24). The definition in
   src/_ZN5Model14LoadAndSetFileEtii.cpp is a real MSVC member -- it lands in
   the map as ?LoadAndSetFile@Model@@QAEXGHH@Z -- and its callers here,
   Stage::LoadModel and Stage::LoadSkybox, are a .cpp inside extern "C" and a
   plain .c respectively. Neither can spell the C++ name, and a linker alias
   would be wrong anyway: the decoration is QAE, __thiscall, so aliasing it
   onto a cdecl declaration would hand the body a `this` that never was one. */
void _ZN5Model14LoadAndSetFileEtii(void *self, unsigned short id, int a, int b)
{ ((Model *)self)->Model::LoadAndSetFile(id, a, b); }
}

/* ---- gate 18: ov085's two classes ---------------------------------------
   Two C-named references onto method definitions, same shape as gate 17's.
   Rabbit::Behavior and both CleanupResources/OnPendingDestroy are already
   C-named in src, so only the two InitResources need a face.
   BOTH RENDERS ARE HOST COPIES, not faces: each dispatches slot 5 of its own
   ModelAnim through a local shadow class, which is ROM numbering where the
   host array is MSVC's -- gate 17's Bird/FLAG case with a body attached.
   port/unmatched/Ov085_Renders.cpp. */
#include "daMip_c.h"
#include "LakituBro.h"
extern "C" {
int _ZN7daMip_c13InitResourcesEv(void *self)
{ return ((daMip_c *)self)->daMip_c::InitResources(); }
int _ZN9LakituBro13InitResourcesEv(void *self)
{ return ((LakituBro *)self)->LakituBro::InitResources(); }
}

/* ---- gate 18: RABBIT_KEY (actor 229) -- the caught rabbit's grant actor ----
   RabbitKey's InitResources and Behavior are real C++ methods (they spell
   qualified member calls), so each needs a C-named face for its vtable slot --
   the gate-18 shape again. Render is NOT here: like the other two ov085
   renders it is a HOST COPY (port/unmatched/Ov085_Renders.cpp). Its local
   shadow-struct dispatch of Model slot 5 lands on the __fastcall mv_render
   through a PLAIN cdecl pointer -- garbage `this` in ECX, measured as a
   frame-1 DEP fault on the spawn-assisted key. The state PMF (this+0x188) is
   handled by the hand-rolled dispatcher host copy
   (port/unmatched/RabbitKey_StateSeat.cpp), not here. */
#include "RabbitKey.h"
extern "C" {
int _ZN9RabbitKey13InitResourcesEv(void *self)
{ return ((RabbitKey *)self)->RabbitKey::InitResources(); }
int _ZN9RabbitKey8BehaviorEv(void *self)
{ return ((RabbitKey *)self)->RabbitKey::Behavior(); }
}

/* ---- gate 40: ov100's STAR_DOOR (the config's _ZN4Door* family) ----------
   Two C-named references onto method definitions, the gate-18 shape. The star
   door's InitResources and Behavior are real C++ methods on `Door` (Door.h);
   its Render, CleanupResources, OnPendingDestroy and D0 are already C-named in
   src, and slot 16 reuses ac_d1_door (the member is a CommonModel at 0xd4, the
   real door's layout), so only these two need a face. */
#include "daStarGate_c.h"
#include "Door.h"
extern "C" {
int _ZN12daStarGate_c13InitResourcesEv(void *self)
{ return ((daStarGate_c *)self)->daStarGate_c::InitResources(); }
int _ZN12daStarGate_c8BehaviorEv(void *self)
{ return ((daStarGate_c *)self)->daStarGate_c::Behavior(); }
}

/* ---- gate 41: ov010's TRAP (shared by LIGHT_BEAM) ------------------------
   Three C-named references onto method definitions, the gate-18 shape. TRAP's
   InitResources, Behavior and Render are real C++ methods; its
   CleanupResources and D0 are already C-named in src, and slots 16/17 trap
   (nothing destroys one on the castle-interior boot -- the gate-17 reading). */
#include "LightBeam.h"
extern "C" {
int _ZN9LightBeam13InitResourcesEv(void *self)
{ return ((LightBeam *)self)->LightBeam::InitResources(); }
int _ZN9LightBeam8BehaviorEv(void *self)
{ return ((LightBeam *)self)->LightBeam::Behavior(); }
int _ZN9LightBeam6RenderEv(void *self)
{ return ((LightBeam *)self)->LightBeam::Render(); }
}

/* ---- gate 42: ov010's PEACH_PAINTING ------------------------------------
   Three C-named references onto method definitions, the gate-18 shape.
   InitResources, Behavior and Render are real C++ methods (the .cpp TUs);
   CleanupResources and D0 are already C-named in src, and slot 16 is a pt_d1
   thunk in hal/actor_classes.cpp (Model at 0xd4 then Actor::~Actor). The class
   was mislabeled as blocked on ov052/RollingRock; the relocs prove both
   references stay inside ov010 -- see slice_gate42.txt. */
#include "PeachPainting.h"
extern "C" {
int _ZN13PeachPainting13InitResourcesEv(void *self)
{ return ((PeachPainting *)self)->PeachPainting::InitResources(); }
int _ZN13PeachPainting8BehaviorEv(void *self)
{ return ((PeachPainting *)self)->PeachPainting::Behavior(); }
int _ZN13PeachPainting6RenderEv(void *self)
{ return ((PeachPainting *)self)->PeachPainting::Render(); }
}

/* Three more C-named references onto method definitions, reached through
   ov085's classes: the two Player talk-state reads the rabbit's caught
   branch consults, and ModelAnim::Render with an explicit scale (which the
   rabbit's Render helper passes -- the +0x80 Vector3, the ROM's own).
   Actor::UpdateCarry is the fourth and it is NOT here: its definition
   declares its own local `class Actor`, so the face has to be built against
   a shadow rather than include/Actor.h. hal/reverse_bridges.cpp. */
#include "ModelAnim.h"
extern "C" {
int _ZN6Player12GetTalkStateEv(void *self)
{ return ((Player *)self)->Player::GetTalkState(); }
int _ZN6Player18HasFinishedTalkingEv(void *self)
{ return ((Player *)self)->Player::HasFinishedTalking(); }
void _ZN9ModelAnim6RenderEPK7Vector3(void *self, const void *scale)
{ ((ModelAnim *)self)->ModelAnim::Render((const Vector3 *)scale); }
}

/* The veneer at arm9 0x0203c178 is `ldr ip,[pc]; bx ip; .word 0x020527e9`
   -- the low bit is the THUMB flag, not part of the address. The ROM symbol
   is func_020527e8 (the 4x3 scale-matrix builder, hosted in
   hal/model_host.cpp); the odd spelling is what the veneer's own src file
   declares. */
#pragma comment(linker, "/alternatename:_func_020527e9=_func_020527e8")

/* Sound::PlaySub reached as a NAMESPACE free function: func_ov085_0212de5c
   declares `namespace Sound { void PlaySub(...); }` where every other caller
   uses the Itanium C name. Both are __cdecl with the same five stack
   arguments and the ov085 caller discards the result, so the matched
   src/_ZN5Sound7PlaySubEjjj5Fix12IiEb.cpp body serves it directly -- the
   same reading cxx_aliases.cpp already takes for the `YAHIIIHH` variant. */
#pragma comment(linker, "/alternatename:?PlaySub@Sound@@YAXIIIH_N@Z=__ZN5Sound7PlaySubEjjj5Fix12IiEb")

/* ---- gate 19: ov098's CANNON --------------------------------------------
   One more, the same shape. Its Render is a host copy for the Model slot-3
   reason and its Behavior for the pointer-to-member one
   (port/unmatched/Cannon_Render.cpp, Cannon_Behavior.cpp). */
#include "Cannon.h"
extern "C" {
int _ZN6Cannon13InitResourcesEv(void *self)
{ return ((Cannon *)self)->Cannon::InitResources(); }
}

/* ---- gate 20: ov002's WATERFALL_MIST -------------------------------------
   One more, the same shape as the cannon's. src's body is a real C++ method
   against include/PoppingLavaBubbles.h (the class the ROM's RTTI calls
   daObjWaterfall_c), so MSVC emits it under ?InitResources@... and the
   vtable fill wants the Itanium name.

   THE CALL MUST BE QUALIFIED, run link100 lane CRASH6. InitResources is
   VIRTUAL, so the unqualified `self->InitResources()` this line used to carry
   was not a call to the body at all: MSVC compiled the whole face down to
   `mov ecx,[ebp+8] / mov eax,[ecx] / jmp dword ptr [eax]`, a dispatch through
   slot 0 of the object's own vtable. Slot 0 is hal/actor_classes.cpp's wm_init,
   whose entire body is `push ecx / call _ZN16daObjWaterfall_c13InitResourcesEv`,
   so the two called each other until the stack ran out. Measured: c00000fd with
   32 identical return words at wm_init+6, eax and ebx both holding the table
   base. It was unreachable until the vptr came back to the address point in the
   commit before this one, which is why it is only being found now.

   Every face in this tree is the qualified shape for exactly this reason
   (1595 qualified against 108 unqualified when the sweep was run). Of the 24
   unqualified ones that name their own class and method, this was the ONLY one
   that compiled to an indirect dispatch: the other 23 call non-virtual members,
   where an unqualified call is already direct. facecycle_guard cannot see this
   shape, because the cycle closes through `jmp dword ptr [eax]` and the guard
   follows direct calls. */
#include "daObjWaterfall_c.h"
extern "C" {
int _ZN16daObjWaterfall_c13InitResourcesEv(void *self)
{ return ((daObjWaterfall_c *)self)->daObjWaterfall_c::InitResources(); }
}

/* ---- gate 21: ov100's BUTTERFLY and FISH ---------------------------------
   Five more of the same shape. Each of these src bodies is a real C++ method
   against its generated header, so MSVC emits it under ?Name@Class@@... and
   the vtable fill (and, for the fish, its own host Behavior) wants the
   Itanium name. Their Renders are NOT faced here: those are host copies in
   port/unmatched/ModelAnim_Renders.cpp, which define the Itanium names
   themselves. */
#include "Butterfly.h"
#include "Fish.h"
extern "C" {
int _ZN9Butterfly13InitResourcesEv(void *self)
{ return ((Butterfly *)self)->Butterfly::InitResources(); }
int _ZN4Fish13InitResourcesEv(void *self)
{ return ((Fish *)self)->Fish::InitResources(); }
int _ZN4Fish16CleanupResourcesEv(void *self)
{ return ((Fish *)self)->Fish::CleanupResources(); }
}

/* Animation::GetFrameCount is a real const method too, and gate 21 is the
   first thing to call it: the butterfly asks its own animation how long it is
   before it will loop, and one of the fish's states does the same. */
#include "Animation.h"
extern "C" {
unsigned _ZNK9Animation13GetFrameCountEv(const void *self)
{ return ((const Animation *)self)->GetFrameCount(); }
}

/* ---- gate 23: ov102's QUESTION_BLOCK -------------------------------------
   Two more of the same shape; its InitResources is already C-named and its
   Render is a host copy (port/unmatched/ModelAnim_Renders.cpp). */
#include "QuestionBlock.h"
extern "C" {
int _ZN13QuestionBlock8BehaviorEv(void *self)
{ return ((QuestionBlock *)self)->QuestionBlock::Behavior(); }
int _ZN13QuestionBlock16CleanupResourcesEv(void *self)
{ return ((QuestionBlock *)self)->QuestionBlock::CleanupResources(); }
}

/* ---- wave 4 lane d: the two FaderBrightness faces Scene::BeforeBehavior needs
   src/_ZN8dScene_c14BeforeBehaviorEv.cpp declares both of these as C-linkage free
   functions TAKING THE RECEIVER:

       extern void _ZN15FaderBrightness14SetForwardTimeEj(FaderBrightness*, u32);
       extern int  _ZN15FaderBrightness7IsAtEndEv(FaderBrightness*);

   and passes &data_0209f5d0 to each. The definitions are real virtual methods
   in src/engine/fader/, so MSVC emits them as ?SetForwardTime@FaderBrightness
   @@UAEHI@Z and ?IsAtEnd@FaderBrightness@@UAEHXZ. Faces and not
   /alternatename aliases, for this file's own reason: both are UAE, virtual
   __thiscall, so an alias would enter them with `this` in whatever ecx held
   and (for SetForwardTime) one stack argument too many. Checklist:
     (a) TARGET  -- qualified `FaderBrightness::` calls, so neither can land on
                    a sibling; IsAtStart is the near-identical one and it is a
                    DIFFERENT predicate (currInterp == 0 vs == 0x1000).
     (b) ARITY   -- IsAtEnd none, SetForwardTime one u32, matching the ROM
                    bodies and the two declarations above.
     (c) RECEIVER-- passed as the first argument and cast, which is what the
                    caller delivers; the qualified call puts it in ecx.

   IsAtEnd's matched TU is ALREADY in the link (slice_w1l3.txt seats it for
   hal/fader_wipes.cpp) -- only the C name was missing, so that face is a name
   and nothing else. SetForwardTime's TU is not, and slice_w1l2.txt adds it.

   Both checks are from the EMITTED BYTES, this file's own rule, not from the
   signatures. dumpbin /disasm on method_faces.cpp.obj:

       __ZN15FaderBrightness14SetForwardTimeEj:
         push ebp / mov ebp,esp
         push  dword ptr [ebp+0Ch]          the frames argument, one of them
         mov   ecx,dword ptr [ebp+8]        the receiver, into ecx
         call  ?SetForwardTime@FaderBrightness@@UAEHI@Z
       __ZN15FaderBrightness7IsAtEndEv:
         push ebp / mov ebp,esp
         mov   ecx,dword ptr [ebp+8]        the receiver, into ecx
         pop ebp
         jmp   ?IsAtEnd@FaderBrightness@@UAEHXZ

   -- so the target is the intended one and not IsAtStart, the arity is right,
   and the receiver reaches ecx in both. IsAtEnd's tail jmp is safe because XZ
   takes no stack argument: the callee's bare `ret` returns to the face's own
   caller with the cdecl argument still on the stack, which that caller cleans.

   ONE HAZARD, INHERITED FROM THE BODY AND NOT FROM THE FACE, recorded because
   slice_w1l3.txt's fader block declined to seat this same body over it. The
   matched FaderBrightness::SetForwardTime ends in `return IsAtEnd();`
   UNQUALIFIED, which is a virtual call through the receiver's vptr -- read out
   of the body's own object rather than predicted from the C:

       ?SetForwardTime@FaderBrightness@@UAEHI@Z:
         ...
         mov   eax,dword ptr [esi]          esi is `this`; eax is the vptr
         call  dword ptr [eax+14h]          MSVC/host slot 5 = IsAtEnd
                                            (MSVC folds the two dtor slots;
                                            ROM numbering puts IsAtEnd at 6)

   so on a null vptr it would fault reading [0x14].

   THE NULL IS GONE, run link100 lane CTOR2 rung C1c, re-landed by lane CTOR3
   rung 1. This paragraph used to end "Do not wake the branch without giving
   data_0209f5d0 a real vptr first", because the only receiver that reaches
   SetForwardTime here is &data_0209f5d0 and its host storage was zeroed BSS --
   the port did not link the ROM's static initialiser for it. It links it now:
   src/__sinit_02074edc.c is on port/slice_ctor.txt and the ROM's own .ctor
   walk runs it at Entry, so the object is constructed the way the DS
   constructs it and its vptr is data_0208eacc, _ZTV15FaderBrightness. That
   table's dtor pair is seated in hal/scene_boot.cpp and every slot from +0x08
   up is that file's named l2_vt_trap, so the worst case at [vptr+0x14] moved
   from an access violation to a line of output that says which table and which
   slot. The [ctor] line of every captured run names the table this object's
   vptr landed on.

   THE FIRST LANDING OF C1c WAS BACKED OUT AND THE REASON MATTERS HERE. The
   same initialiser also moves data_0209f5e8's live vptr onto data_0208eb2c,
   and hal/scene_boot.cpp used to fill that table from scene_fill_title, far
   later in the boot -- so a loopback pair called address 0 through byte +0x14
   of it on the VS menu to level transition. hal/scene_boot.cpp now seats all
   four fader vtables from a C++ static initialiser, before main and therefore
   before the .ctor walk, which is what makes the sentence above true rather
   than hopeful: the table this object's vptr names is live when the vptr is
   written.

   THE BRANCH IS REACHABLE NOW, AND FOR ONE FRAME EACH (run linkfull, lane
   LOOPIN2). Every call site of these two faces on &data_0209f5d0 is
   dScene_c::BeforeBehavior's soft-reset branch, behind `data_0209f1e0 != 0`,
   and src/func_02023498.c -- the only TU that writes that byte non-zero -- is
   in the link and runs every frame at phase 0x17 (tests/walk_window.cpp's
   level loop, hal/scene_boot.cpp's port_scene_tick). So L+R+START+SELECT on a
   level raises the latch; that frame's BeforeBehavior calls SetForwardTime
   here and parks the fader in data_0209f1e4, and the next frame's calls
   IsAtEnd. It goes no further: the fade itself is stepped through
   data_0208eacc slot 2 (FaderBrightness::AdvanceFade, still hal/scene_boot.cpp's
   named trap), so on that second frame the level loop answers the reset by
   starting the game again at the title and ending the run
   (tests/walk_window.cpp, the phase-0x17 block says why). The two counters
   below count exactly those entries -- the latch up and the receiver the reset
   fader -- so a run says whether the branch was entered; every other caller
   (the fader tables' stubs dispatch IsAtEnd on the installed fader) is left
   out of them. They put a compare and an increment ahead of each forward; the
   forward itself -- the receiver into ecx, the qualified call to the intended
   body -- is the one the dumpbin paragraph above reads, whether MSVC ends it
   in a call or a tail jmp. hal/auto_bss.cpp's sizing note still reads "no
   linked writer"; that note is history now. */
#include "FaderBrightness.h"
extern "C" {
extern unsigned char data_0209f1e0[4];   /* hal/auto_bss.cpp: the latch */
extern int data_0209f5d0[3];             /* hal/auto_bss.cpp: the reset fader */
}
static unsigned g_fb_latch_setfwd, g_fb_latch_atend;
extern "C" {
void port_fader_latch_face_hits(unsigned *set_forward, unsigned *is_at_end)
{
    *set_forward = g_fb_latch_setfwd;
    *is_at_end = g_fb_latch_atend;
}
int _ZN15FaderBrightness7IsAtEndEv(void *self)
{
    if (data_0209f1e0[0] && self == (void *)data_0209f5d0) ++g_fb_latch_atend;
    return ((FaderBrightness *)self)->FaderBrightness::IsAtEnd();
}
int _ZN15FaderBrightness14SetForwardTimeEj(void *self, u32 frames)
{
    if (data_0209f1e0[0] && self == (void *)data_0209f5d0) ++g_fb_latch_setfwd;
    return ((FaderBrightness *)self)->FaderBrightness::SetForwardTime(frames);
}
}

/* ---- gate 22: the door ring's Player entry points ------------------------
   These were direct link aliases in cxx_aliases.cpp ("same body, no thunk
   needed"), which on 32-bit MSVC called __thiscall methods with a cdecl
   frame: garbage `this`, and for the one-stack-arg methods a 4-byte ESP
   imbalance -- the 2026-08-07 door-open crash. Real faces, like everything
   else in this file. The Camera pair lives in hal/door_ring_faces.cpp
   because its defining TUs use a local Camera mirror, not include/Camera.h. */
extern "C" {
void _ZN6Player11OpenBigDoorEv(void *self)
{ ((Player *)self)->OpenBigDoor(); }
void _ZN6Player16SetRealCharacterEj(void *self, unsigned int chr_)
{ ((Player *)self)->SetRealCharacter(chr_); }
void _ZN6Player12Unk_020ca488Ev(void *self)
{ ((Player *)self)->Unk_020ca488(); }
int _ZN6Player21IsOpeningDoorWithStarEv(void *self)
{ return ((Player *)self)->IsOpeningDoorWithStar(); }
int _ZN6Player13TryTalkToDoorEh(void *self, unsigned char a)
{ return ((Player *)self)->TryTalkToDoor(a); }
int _ZN6Player16TryTalkToKeyDoorEv(void *self)
{ return ((Player *)self)->TryTalkToKeyDoor(); }
int _ZN6Player24TryExitWhiteDoorWithStarEv(void *self)
{ return ((Player *)self)->TryExitWhiteDoorWithStar(); }
/* gate 200: PushBlock's break branch grants the flower power-up
   (func_ov002_020b979c) -- the daObjPowerUpItem_c inheritance is not just
   RTTI trivia. Both matched src, real C++ methods. */
void _ZN6Player16InitBalloonMarioEv(void *self)
{ ((Player *)self)->InitBalloonMario(); }
void _ZN6Player13InitFireYoshiEv(void *self)
{ ((Player *)self)->InitFireYoshi(); }
}  /* extern "C" */

/* ---- run link100, lane SHADOWS: two ZERO-INDEX FORWARDERS ----------------
 *
 * These are faces in the sense the header at the top of this file means: a
 * one-line forwarder that stands between the ROM's calling convention and the
 * host's. They exist so that two InitResources bodies can be the ROM's own
 * code instead of a host copy of it.
 *
 * THE SEAM. Both classes' InitResources ends by putting the object in state 0
 * through a two-argument state setter, and both matched TUs spell that call
 * with the SECOND ARGUMENT DROPPED -- the r1 ride-through family this file's
 * item 2 describes for a receiver, applied to an ordinary argument. On ARM the
 * call is byte-identical because the caller has already put the value in r1
 * and the `bl` does not disturb it; under cdecl the callee reads the caller's
 * stack instead, so the setter indexes its state table at garbage << 4.
 *
 * THE VALUE IS THE ROM'S, read out of extracted/overlays/ at each overlay's
 * own load base, one instruction before the branch in both:
 *   ov072 (base 0x0211f000), BabyPenguin::InitResources 0x02121e84:
 *     02121F68  mov r1, #0 ... 02121F78  bl #0x2121d50
 *   ov080 (base 0x02123740), CrazedCrate::InitResources 0x021251ec:
 *     021252DC  mov r1, #0 ... 021252E8  bl #0x212513c
 *
 * Each src TU is compiled with a per-source -D that renames its call of the
 * setter onto the forwarder below (port/CMakeLists.txt, the gate-shadows
 * block; the derivation and the full instruction quotes are in
 * port/slice_shadows.txt). Nothing else in the tree is renamed, so every other
 * caller of either setter still reaches it under the ROM's own name.
 *
 * The setters themselves are matched TUs already in the link, and both spell
 * the two-argument shape (src/func_ov072_02121d50.c, src/func_ov080_0212513c.c:
 * `*(char**)(c + OFF) = data_...[i << 4]`), which is why the declarations here
 * are the real ones and not a cast. include/decl_common.h is not reachable
 * from this file (nothing in include/ includes it), so its one-argument
 * declaration of func_ov080_0212513c does not collide with this one. */
extern "C" {
void func_ov072_02121d50(void *c, int i);
void func_ov080_0212513c(void *c, int i);

void port_ov072_bp_state_i0(void *c) { func_ov072_02121d50(c, 0); }
void port_ov080_cc_state_i0(void *c) { func_ov080_0212513c(c, 0); }
}  /* extern "C" */

/* The third forwarder of the same gate, and the one that reads the other way
 * round. LakituBro::InitResources calls TextureSequence::Prepare with the two
 * arguments the ROM passes -- verified in ov085, where 0x0212ec44's branch is
 * preceded only by loads of r0 and r1 and r2 is written on the instruction
 * AFTER it -- while the port's Prepare (hal/player_bridges.cpp) is the three-
 * parameter thiscall face that reshapes r0/r1 into (self, bmd) and then wants a
 * btp the ROM never sent. func_02046d50, which the ROM's Prepare tail-calls,
 * ignores that third value entirely.
 *
 * So the third argument is a HOST value and it is supplied on the host side:
 * the caller's own second argument again, which is a live object, exactly what
 * the retired port/unmatched/TexSeq_Caller_LakituBro.cpp passed. The parameters
 * are void* here and references in the calling TU; at this ABI those are the
 * same word, and both sides are extern "C", so the call is exact. */
extern "C" {
void _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(void *self, void *bmd,
                                                       void *btp);
void port_texseq_prepare_r1(void *self, void *bmd)
{ _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(self, bmd, bmd); }
}  /* extern "C" */

/* ---- RUNG 3 (run link100, lane SHADOWS2): the shadow whose seam is a RETURN
 * TYPE, not an argument -------------------------------------------------------
 *
 * SharedFilePtr::ReallocateModelFile was the tenth MSVC-NAME SHADOW row. The
 * ROM's body is one statement --
 *
 *     unsigned int SharedFilePtr::ReallocateModelFile()
 *     { return func_02017060(file); }
 *
 * (src/_ZN13SharedFilePtr19ReallocateModelFileEv.cpp) -- and func_02017060 has
 * been in this link since gate 16. Nothing was missing. What kept the body out
 * is that the two matched TUs DISAGREE ABOUT THE RETURN TYPE: the definition
 * returns unsigned int, while its one caller
 * (src/_ZN5Model8LoadFileER13SharedFilePtr.cpp:12) and include/SharedFilePtr.h
 * both declare `void ReallocateModelFile();`. On ARM that is invisible --
 * _ZN13SharedFilePtr19ReallocateModelFileEv carries no return type, so both
 * spellings are one symbol and mwcc links them -- but MSVC puts the return type
 * IN the decoration, so the caller asks for ?...@@QAEXXZ and the body publishes
 * ?...@@QAEIXZ. Two names, one function, and the port answered with an empty
 * ?...@@QAEXXZ in hal/gx_upload_bridge.cpp, which is what made it a shadow.
 *
 * THE ALIAS IS ABI-EXACT, which is the whole reason this row is a bridge and
 * not a host copy. Both spellings are `public: __thiscall f(void)`: the
 * receiver rides in ecx, neither pushes a stack argument, both clean zero
 * bytes, and the only difference is an eax the void-spelling caller does not
 * read. Confirmed from the object rather than from the mangling rules --
 * cl /c on the matched TU publishes
 *   ?ReallocateModelFile@SharedFilePtr@@QAEIXZ
 *   (public: unsigned int __thiscall SharedFilePtr::ReallocateModelFile(void))
 * and walk_window.map carried ?ReallocateModelFile@SharedFilePtr@@QAEXXZ from
 * gx_upload_bridge.cpp.obj. It is the same trade hal/cxxname_bridge.cpp's
 * Model::LoadFile note already describes in this exact family: "Same ROM
 * function, two host bodies, picked by how a caller spelled the return type."
 *
 * THE SHRINK IS THE ROM'S AGAIN, said out loud. The empty body did not just
 * stand in for the ROM's body, it DECLINED the work: func_02017060 is the DS
 * heap shrink-to-fit (Heap::_Sizeof, func_020469e0, Heap::Reallocate on
 * Memory::gameHeapPtr). The port's other model path gates the same callee
 * behind port_model_shrink_enabled (hal/level_boot.cpp, applied to
 * src/func_02016ff4.cpp by hostgen), so with this seat the two paths no longer
 * agree: Model::LoadFile's numRefs==1 branch now performs the ROM's shrink and
 * Model::LoadAndSetFile's tail still declines it by default. That is the
 * direction the port is supposed to move -- the ROM's own body doing the ROM's
 * own thing -- and it is gated by measurement, not by assertion: this rung's
 * battery, its eight proofs and its level-1 capture all ran with the shrink
 * live. hostgen's own switch is untouched and is the A/B for the other half. */
#pragma comment(linker, "/alternatename:?ReallocateModelFile@SharedFilePtr@@QAEXXZ=?ReallocateModelFile@SharedFilePtr@@QAEIXZ")

/* ---- gate shadows3 -----------------------------------------------------
   Run link100 wave 14, lane SHADOWS3 seated three rows whose recovered form
   is a REAL C++ METHOD -- daTrs_c::Behavior, daTrsTrap_c::InitResources and
   dEnemyBase_c::UpdateYoshiEat -- so the flat Itanium name their callers and
   registry faces spell stopped being defined when the host copies went away.
   THE FORWARDERS ARE NOT WRITTEN HERE. They were, for one build, and that
   build named the reason not to: port/faces_sync.txt already carried all
   three names as FORWARD rows -- the face DEFINING the member and calling the
   host copy under its flat C name -- so a hand-written reverse face collided
   with the generated one (LNK2005 on ?Behavior@daTrs_c@@UAEHXZ and its two
   siblings, from faces_sync_gen.cpp.obj) and, worse, the pair would have
   forwarded to each other. The ledger owns the direction, so the three rows
   are flipped F -> R there instead: the generated face now defines the flat
   name and makes the qualified call into the matched TU's member, which is
   the same one-line body this file would have held, derived and re-checked
   at every build by facegen.py --sync rather than typed once. */
