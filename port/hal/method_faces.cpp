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
//      calls _ZN5Actor13ClosestPlayerEv() with no receiver, so ClosestPlayer
//      reads this+0x5c off a null or garbage base. For a direct ClosestPlayer
//      reader that is a crash (the rabbit soft-lock); for FarthestPlayer it is
//      a SILENT wrong result, which is worse.
//
//   3. MIS-BRIDGED RECEIVER. A face entered by a thunk (or tail-called into)
//      with the object pushed on the stack must move it into ECX before the
//      real __thiscall body. Actor::OnTurnIntoEgg is a jmp tail-call into the
//      extern-C _ZN5Actor24KillAndTrackInDeathTableEv face (below), which is
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
// _ZN8PathLift12BaseBehaviorEv is the measured instance: PORT_HOST_ABI tagged
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
#include "Actor.h"
#include "ActorBase.h"
#include "BgCh.h"
#include "Camera.h"
#include "ClsnResult.h"
#include "CylinderClsn.h"
#include "CylinderClsnWithPos.h"
#include "Heap.h"
#include "Message.h"
#include "ModelBase.h"
#include "Model.h"
#include "ModelAnim2.h"
#include "OAM.h"
#include "PathPtr.h"
#include "Player.h"
#include "RaycastLine.h"
#include "SphereClsn.h"
#include "TextureSequence.h"
#include "Timer.h"
#include "WithMeshClsn.h"

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
void _ZN5Actor13SpawnSoundObjEj(void *self, u32 soundObjParam)
{ ((Actor *)self)->Actor::SpawnSoundObj(soundObjParam); }

/* Same rewrite, same story: a method now, spelled as a free function by every
   ov002 death path that calls it. */
void _ZN5Actor24KillAndTrackInDeathTableEv(void *self)
{ ((Actor *)self)->Actor::KillAndTrackInDeathTable(); }

/* Actor::FindWithID is static -- no `this`, so the face is a plain forward.
   hal/reverse_bridges.cpp used to run this the other way, wrapping a C-form
   src definition into a method; main made the src file the method, so the
   wrapper there is gone and this replaces it. */
Actor *_ZN5Actor10FindWithIDEj(u32 id)
{ return Actor::FindWithID(id); }

void _ZN10ModelAnim24CopyERKS_Pcj(void *self, const void *src, char *nf,
                                  unsigned nof)
{ ((ModelAnim2 *)self)->ModelAnim2::Copy(*(const ModelAnim2 *)src, nf, nof); }


void _ZN12CylinderClsn5ClearEv(void *self)
{ ((CylinderClsn *)self)->CylinderClsn::Clear(); }
void _ZN12CylinderClsn6UpdateEv(void *self)
{ ((CylinderClsn *)self)->CylinderClsn::Update(); }

void _ZN12WithMeshClsn13SetGroundFlagEv(void *self)
{ ((WithMeshClsn *)self)->WithMeshClsn::SetGroundFlag(); }
void _ZN12WithMeshClsn13SetLimMovFlagEv(void *self)
{ ((WithMeshClsn *)self)->WithMeshClsn::SetLimMovFlag(); }
void _ZN12WithMeshClsn15ClearGroundFlagEv(void *self)
{ ((WithMeshClsn *)self)->WithMeshClsn::ClearGroundFlag(); }
void _ZN12WithMeshClsn15ClearLimMovFlagEv(void *self)
{ ((WithMeshClsn *)self)->WithMeshClsn::ClearLimMovFlag(); }
void _ZN12WithMeshClsn18StopDetectingWaterEv(void *self)
{ ((WithMeshClsn *)self)->WithMeshClsn::StopDetectingWater(); }
void _ZN12WithMeshClsn19ClearAllGroundFlagsEv(void *self)
{ ((WithMeshClsn *)self)->WithMeshClsn::ClearAllGroundFlags(); }
void _ZN12WithMeshClsn19StartDetectingWaterEv(void *self)
{ ((WithMeshClsn *)self)->WithMeshClsn::StartDetectingWater(); }

void _ZN15TextureSequence6UpdateER15ModelComponents(void *self, void *mc)
{ ((TextureSequence *)self)->TextureSequence::Update(
      *(ModelComponents *)mc); }

void _ZN4BgCh19StartDetectingToxicEv(void *self)
{ ((BgCh *)self)->BgCh::StartDetectingToxic(); }
void _ZN4BgCh21StopDetectingOrdinaryEv(void *self)
{ ((BgCh *)self)->BgCh::StopDetectingOrdinary(); }

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


void _ZN9ActorBase18MarkForDestructionEv(void *self)
{ ((ActorBase *)self)->ActorBase::MarkForDestruction(); }

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
int _ZN9ActorBase14BeforeBehaviorEv(void *self)
{ return ((ActorBase *)self)->ActorBase::BeforeBehavior() ? 1 : 0; }

unsigned _ZNK7PathPtr8NumNodesEv(const void *self)
{ return ((const PathPtr *)self)->PathPtr::NumNodes(); }


}  /* extern "C" */



/* REVERSE faces: these St_ files define the ITANIUM C name; the state
   dispatcher references the MSVC method. Forward method -> C def. */
extern "C" int _ZN6Player19St_GroundPound_MainEv(void *self);
extern "C" int _ZN6Player16St_LongJump_InitEv(void *self);
int Player::St_GroundPound_Main()
{ return _ZN6Player19St_GroundPound_MainEv(this); }
int Player::St_LongJump_Init()
{ return _ZN6Player16St_LongJump_InitEv(this); }

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
extern "C" int _ZN6Player15St_Balloon_MainEv(void *self);
extern "C" int _ZN6Player16St_BurnFire_InitEv(void *self);
extern "C" int _ZN6Player16St_BurnFire_MainEv(void *self);
extern "C" int _ZN6Player18St_CameraZoom_MainEv(void *self);
extern "C" int _ZN6Player18St_DizzyStars_MainEv(void *self);
extern "C" int _ZN6Player19St_Electrocute_MainEv(void *self);
extern "C" int _ZN6Player18St_Grabbed_CleanupEv(void *self);
extern "C" int _ZN6Player12St_Hurt_MainEv(void *self);
extern "C" int _ZN6Player23St_MetalWaterWater_MainEv(void *self);
extern "C" int _ZN6Player15St_Respawn_InitEv(void *self);
extern "C" int _ZN6Player12St_Spin_MainEv(void *self);
extern "C" int _ZN6Player17St_SweepKick_InitEv(void *self);
extern "C" int _ZN6Player12St_Swim_MainEv(void *self);
extern "C" int _ZN6Player15St_Talk_CleanupEv(void *self);
extern "C" int _ZN6Player13St_Throw_InitEv(void *self);
extern "C" int _ZN6Player14St_Thrown_InitEv(void *self);
extern "C" int _ZN6Player19St_TornadoSpin_MainEv(void *self);
int Player::St_Balloon_Main()
{ return _ZN6Player15St_Balloon_MainEv(this); }
int Player::St_BurnFire_Init()
{ return _ZN6Player16St_BurnFire_InitEv(this); }
int Player::St_BurnFire_Main()
{ return _ZN6Player16St_BurnFire_MainEv(this); }
int Player::St_CameraZoom_Main()
{ return _ZN6Player18St_CameraZoom_MainEv(this); }
int Player::St_DizzyStars_Main()
{ return _ZN6Player18St_DizzyStars_MainEv(this); }
int Player::St_Electrocute_Main()
{ return _ZN6Player19St_Electrocute_MainEv(this); }
int Player::St_Grabbed_Cleanup()
{ return _ZN6Player18St_Grabbed_CleanupEv(this); }
int Player::St_Hurt_Main()
{ return _ZN6Player12St_Hurt_MainEv(this); }
int Player::St_MetalWaterWater_Main()
{ return _ZN6Player23St_MetalWaterWater_MainEv(this); }
int Player::St_Respawn_Init()
{ return _ZN6Player15St_Respawn_InitEv(this); }
int Player::St_Spin_Main()
{ return _ZN6Player12St_Spin_MainEv(this); }
int Player::St_SweepKick_Init()
{ return _ZN6Player17St_SweepKick_InitEv(this); }
int Player::St_Swim_Main()
{ return _ZN6Player12St_Swim_MainEv(this); }
int Player::St_Talk_Cleanup()
{ return _ZN6Player15St_Talk_CleanupEv(this); }
int Player::St_Throw_Init()
{ return _ZN6Player13St_Throw_InitEv(this); }
int Player::St_Thrown_Init()
{ return _ZN6Player14St_Thrown_InitEv(this); }
int Player::St_TornadoSpin_Main()
{ return _ZN6Player19St_TornadoSpin_MainEv(this); }
extern "C" int _ZN6Player18St_YoshiPower_MainEv(void *self);
int Player::St_YoshiPower_Main()
{ return _ZN6Player18St_YoshiPower_MainEv(this); }
/* St_Grabbed_Main calls DropActor by its Itanium name; the definition is a
   real method. Forward C name -> method (no face the other way). */
extern "C" int _ZN6Player9DropActorEv(void *self)
{ return ((Player *)self)->Player::DropActor(); }

/* gate 14: the init chain the actor spawn spine dispatches. Both are real
   __thiscall methods, so a linker alias onto the Itanium name their .c
   callers use would enter the body with `this` in whatever ecx held. */
extern "C" int _ZN5Actor18GetBitInDeathTableEv(void *self)
{ return ((Actor *)self)->Actor::GetBitInDeathTable(); }
extern "C" void _ZN5Actor18AfterInitResourcesEj(void *self, unsigned a)
{ ((Actor *)self)->Actor::AfterInitResources(a); }

/* gate 16: Actor::BeforeRender is the same shape -- a .c TU calling its base
   by Itanium name over a real __thiscall definition. Slot 10 of every actor
   class the registry carries goes through it. */
extern "C" int _ZN9ActorBase12BeforeRenderEv(void *self)
{ return ((ActorBase *)self)->ActorBase::BeforeRender(); }

/* wave 4 lane d: the same shape one slot over, for _ZTV5Scene slot 4.
   src/_ZN5Scene22BeforeCleanupResourcesEv.c is ordinary C and DOES pass its
   receiver (`_ZN9ActorBase22BeforeCleanupResourcesEv(thiz)`); only the
   C-linkage cdecl name was undefined. The matched ActorBase body is already in
   the link as ?BeforeCleanupResources@ActorBase@@UAEHXZ -- UAE, so a face and
   not an alias, and the checklist reads the same as its BeforeRender sibling
   above: qualified target (its near-sibling BeforeInitResources is a different
   slot and a different predicate), no arguments, receiver passed and cast. */
extern "C" int _ZN9ActorBase22BeforeCleanupResourcesEv(void *self)
{ return ((ActorBase *)self)->ActorBase::BeforeCleanupResources(); }

/* ---- wave 4 lane d: _ZTV5Scene slots 8 and 11, the two ROM TAIL VENEERS ----
   A no-op forwarder is exactly the kind of plumbing this file's header says
   gets skimmed, so here is the full reading before the two lines.

   src/_ZN5Scene13AfterBehaviorEj.cpp and src/_ZN5Scene11AfterRenderEj.cpp are
   ARM tail-call veneers -- `ldr ip,[pc]; bx ip; .word <ActorBase body>` -- so
   the ROM never gives them a frame at all: r0 (`this`) and r1 (the
   VirtualFuncSuccess code) ride straight through. The decompilation spells
   that as a void() calling a void(), which is the dropped-receiver shape this
   file lists as failure mode 2, and it is why wave 3 left both slots blocked.

   WHAT SETTLES IT IS THE CALLEE, not the veneer. Both ROM bodies are empty --

       void ActorBase::AfterBehavior(unsigned int vfSuccess_) { u32 v = ...; }

   and MSVC agrees, three bytes each, dumpbin /disasm on their own objects:

       ?AfterBehavior@ActorBase@@UAEXI@Z:   00000000: C2 04 00   ret 4
       ?AfterRender@ActorBase@@UAEXI@Z:     00000000: C2 04 00   ret 4

   -- so identical that /OPT:ICF folds them onto one address in the map
   (both at 0043dd10). Neither reads ecx, neither reads [ebp+8]. A dropped
   receiver cannot be wrong here in the way mode 2 means, because there is no
   read to be wrong: the failure mode needs a `this+0x5c`, and these have no
   loads at all.

   SO THIS DEFINITION IS THE BEHAVIOUR, EXACTLY, and it is correct for every
   caller rather than only for the Scene path. The only other src/ TU that
   spells either C name is the Actor veneer of the same pair
   (src/_ZN5Actor13AfterBehaviorEj.cpp, src/_ZN5Actor11AfterRenderEj.cpp,
   neither in the map today), and the ROM tail-jumps those to the SAME empty
   bodies -- so if a later lane links them, they resolve here and are still
   right.

   THE ONE THING IT IS NOT is a bridge into the matched body: the port's slot-8
   dispatch reaches this, not ?AfterBehavior@ActorBase@@UAEXI@Z. That gap is
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
extern "C" void _ZN9ActorBase13AfterBehaviorEj(void) {}
/* PORT_HOST_ABI: same as AfterBehavior above -- `ret 4`, no receiver read, no
   argument read; /OPT:ICF folds the two matched bodies onto one address. */
extern "C" void _ZN9ActorBase11AfterRenderEj(void) {}

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

/* gate 16, the shrink-to-fit tail of Model::LoadAndSetFile. Both are real
   Heap methods reached by Itanium name from func_02017060; _Sizeof is the ARM
   two-instruction veneer onto Sizeof, so the face calls the target directly
   rather than forwarding through a body that would drop both arguments. */
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
extern "C" void _ZN9ActorBase21AfterCleanupResourcesEj(void *self, unsigned a)
{ ((ActorBase *)self)->ActorBase::AfterCleanupResources(a); }

/* src/_ZN4Heap7_SizeofEPv.cpp exists and stays unlinked, and this is the
   reason, written down so the row stops reading as replacement work. Two lanes
   (w8-faces and w8-shadows) ruled this symbol independently and reached the
   same verdict from different evidence; both halves are kept because each
   closes a hole the other leaves open.

   WHAT THE MATCHED TU IS. The ROM's 0xc long-call veneer at arm9 0x0203c274 --
   `ldr ip, [pc]; bx ip; .word 0x0203c454` -- transcribed the only way a veneer
   can be written in C:

       extern "C" void _ZN4Heap6SizeofEPv(void);
       void _ZN4Heap7_SizeofEPv(void) { _ZN4Heap6SizeofEPv(); }

   -- a void() calling a void(). On ARM that is exact: r0 (`this`) and r1 (the
   pointer) ride through the branch untouched and Sizeof reads them where the
   caller left them.

   WHY IT CANNOT BE LINKED, two independent reasons:
     1. No ride-through under MSVC. Both are __cdecl, the veneer pushes
        nothing, and the callee reads its receiver and argument off stack slots
        that were never written -- concretely, it reads its own return address
        as `this`. Both arguments dropped.
     2. The callee is not reachable as spelled anyway. __ZN4Heap6SizeofEPv is
        not in the link at all: src/_ZN4Heap6SizeofEPv.cpp is METHOD-shaped and
        lands as ?Sizeof@Heap@@QAEHPAX@Z. Slicing the veneer would not even
        link without a second face under it -- one that would then be entered
        with nothing on the stack.

   The host face below delivers both arguments explicitly, which is what the
   ROM's register state means. Flagged as missing a tag at the wave-1 close and
   ruled here. */
/* PORT_HOST_ABI: ARM register ride-through -- the matched TU is the ROM's
   `ldr ip,[pc]; bx ip` veneer, whose void() form carries r0/r1 through in
   registers; the same shape under __cdecl drops both, and the callee is
   method-decorated so the veneer could not link regardless. */
extern "C" int _ZN4Heap7_SizeofEPv(void *self, void *p)
{ return ((Heap *)self)->Heap::Sizeof(p); }
extern "C" void _ZN4Heap10ReallocateEPvj(void *self, void *p, unsigned n)
{ ((Heap *)self)->Heap::Reallocate(p, n); }

/* gate 16, THE OTHER DIRECTION: CylinderClsnWithPos::Init is defined at C
   linkage in its own TU while Tree::InitResources declares it as a method on
   a local class shape and calls it __thiscall. An /alternatename would enter
   the cdecl body with `this` still in ecx, so this is a face. */
extern "C" void _ZN19CylinderClsnWithPos4InitERK7Vector35Fix12IiES4_jj(
    void *self, const void *pos, int radius, int height, unsigned flags,
    unsigned vulnFlags);
void CylinderClsnWithPos::Init(const Vector3 &pos, Fix12i radius,
                               Fix12i height, u32 flags, u32 vulnFlags)
{
    _ZN19CylinderClsnWithPos4InitERK7Vector35Fix12IiES4_jj(
        this, &pos, radius, height, flags, vulnFlags);
}


/* ---- gate 16: the collider faces the actor colliders need ---------------
   Three C-named references onto MSVC method definitions. The first two are
   the port's own transcriptions of the ITCM octree walks (port/unmatched/),
   which MovingMeshCollider's DetectClsn overrides call DIRECTLY -- not
   through a vtable -- after transforming the ray or sphere into the
   collider's own space. func_01ffb0fc is the line walk's ROM address, which
   is how the matched source spells it. */
#include "MeshCollider.h"
#include "SphereClsn.h"
extern "C" {
int func_01ffb0fc(void *self, void *ray)
{ return ((MeshCollider *)self)->MeshCollider::DetectClsn(*(RaycastLine *)ray); }
int _ZN12MeshCollider10DetectClsnER10SphereClsn(void *self, void *sph)
{ return ((MeshCollider *)self)->MeshCollider::DetectClsn(*(SphereClsn *)sph); }
int _ZN16MeshColliderBase9IsEnabledEv(void *self)
{ return ((MeshColliderBase *)self)->MeshColliderBase::IsEnabled(); }
int _ZN16MeshColliderBase7DisableEv(void *self)
{ return ((MeshColliderBase *)self)->MeshColliderBase::Disable(); }
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
void _ZN12WithMeshClsn20UpdateDiscreteNoLavaEv(void *self)
{ ((WithMeshClsn *)self)->WithMeshClsn::UpdateDiscreteNoLava(); }
void _ZN12WithMeshClsn22UpdateDiscreteNoLava_2Ev(void *self)
{ ((WithMeshClsn *)self)->WithMeshClsn::UpdateDiscreteNoLava_2(); }
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
#include "MetalNet.h"
#include "Flag.h"
extern "C" {
int _ZN4Bird13InitResourcesEv(void *self)
{ return ((Bird *)self)->Bird::InitResources(); }
/* BIRD::RENDER AND FLAG::RENDER ARE RETIRED (run link100, lane PMFB2). Both
   are on port/slice_pmfb2.txt and compile from src/_ZN4Bird6RenderEv.cpp and
   src/_ZN4Flag6RenderEv.cpp; each matched TU recovered as a real C++ method
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
     0211211c _ZN4Flag6RenderEv   ldr r2,[r2,#0x14]    blx r2    mov r0,#1
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
int _ZN8MetalNet13InitResourcesEv(void *self)
{ return ((MetalNet *)self)->MetalNet::InitResources(); }
int _ZN8MetalNet8BehaviorEv(void *self)
{ return ((MetalNet *)self)->MetalNet::Behavior(); }
int _ZN8MetalNet6RenderEv(void *self)
{ return ((MetalNet *)self)->MetalNet::Render(); }
int _ZN8MetalNet16CleanupResourcesEv(void *self)
{ return ((MetalNet *)self)->MetalNet::CleanupResources(); }
int _ZN4Flag13InitResourcesEv(void *self)
{ return ((Flag *)self)->Flag::InitResources(); }
int _ZN4Flag8BehaviorEv(void *self)
{ return ((Flag *)self)->Flag::Behavior(); }
/* _ZN4Flag6RenderEv is retired with Bird's, on the same measurement.
   include/Flag.h:17 puts its ModelAnim at +0xd4 too and
   src/_ZN4Flag6RenderEv.cpp is byte-for-byte the same shape -- the same local
   six-virtual shadow, the same `b->m(0)` at index 5. Its face is in
   hal/except_faces.cpp with Bird's. */
}

/* ---- gate 143: level 11's own class, IceSlideManager (ov019) -------------
   The Cool Cool Mountain slide manager (actor 356). Its InitResources and
   Behavior are real __thiscall methods in src (.cpp against the generated
   IceSlideManager.h), dispatched through vtable slots 0 and 6 by the
   host-filled _ZTV15IceSlideManager (hal/actor_classes_ccm.cpp). Its D1/D0 are
   not faced: slot 16 is the empty-~Actor form the vtable fill spells inline and
   slot 17 traps. */
#include "IceSlideManager.h"
extern "C" {
int _ZN15IceSlideManager13InitResourcesEv(void *self)
{ return ((IceSlideManager *)self)->IceSlideManager::InitResources(); }
int _ZN15IceSlideManager8BehaviorEv(void *self)
{ return ((IceSlideManager *)self)->IceSlideManager::Behavior(); }
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
#include "Rabbit.h"
#include "LakituBro.h"
extern "C" {
int _ZN6Rabbit13InitResourcesEv(void *self)
{ return ((Rabbit *)self)->Rabbit::InitResources(); }
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
#include "Door.h"
extern "C" {
int _ZN4Door13InitResourcesEv(void *self)
{ return ((Door *)self)->Door::InitResources(); }
int _ZN4Door8BehaviorEv(void *self)
{ return ((Door *)self)->Door::Behavior(); }
}

/* ---- gate 41: ov010's TRAP (shared by LIGHT_BEAM) ------------------------
   Three C-named references onto method definitions, the gate-18 shape. TRAP's
   InitResources, Behavior and Render are real C++ methods; its
   CleanupResources and D0 are already C-named in src, and slots 16/17 trap
   (nothing destroys one on the castle-interior boot -- the gate-17 reading). */
#include "Trap.h"
extern "C" {
int _ZN4Trap13InitResourcesEv(void *self)
{ return ((Trap *)self)->Trap::InitResources(); }
int _ZN4Trap8BehaviorEv(void *self)
{ return ((Trap *)self)->Trap::Behavior(); }
int _ZN4Trap6RenderEv(void *self)
{ return ((Trap *)self)->Trap::Render(); }
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
   vtable fill wants the Itanium name. */
#include "PoppingLavaBubbles.h"
extern "C" {
int _ZN18PoppingLavaBubbles13InitResourcesEv(void *self)
{ return ((PoppingLavaBubbles *)self)->InitResources(); }
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
   src/_ZN5Scene14BeforeBehaviorEv.cpp declares both of these as C-linkage free
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

   so on a null vptr it faults reading [0x14]. The only
   receiver that reaches it here is &data_0209f5d0, whose host storage is
   zeroed BSS because the port does not link the ROM's static initialiser for
   it (src/__sinit_02074edc.c) -- so that dispatch would fault on a null vptr.
   It cannot run: every call site is behind `data_0209f1e0 != 0`, and the only
   TU that ever writes that byte non-zero (src/func_02023498.c) is not in the
   link. The sizing note in hal/auto_bss.cpp carries the same constraint at the
   storage, and slice_w1l2.txt's blocked list names the job that would lift it
   (the ROM-class swap slice_w1l3.txt already scoped). Do not wake the branch
   without giving data_0209f5d0 a real vptr first.

   RESTORED, run link100 lane CTOR3 rung 0. Lane CTOR2's rung C1c retired the
   sentence above by linking src/__sinit_02074edc.c, which does give this
   object a real vptr (data_0208eacc). That rung is out again -- it also moved
   data_0209f5e8's live vptr onto data_0208eb2c, which hal/scene_boot.cpp does
   not fill until scene_fill_title, and a loopback pair died calling address 0
   through it on the VS menu -> level transition. The paragraph stands as it
   was written until that seat is unconditional. */
#include "FaderBrightness.h"
extern "C" {
int _ZN15FaderBrightness7IsAtEndEv(void *self)
{ return ((FaderBrightness *)self)->FaderBrightness::IsAtEnd(); }
int _ZN15FaderBrightness14SetForwardTimeEj(void *self, u32 frames)
{ return ((FaderBrightness *)self)->FaderBrightness::SetForwardTime(frames); }
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
