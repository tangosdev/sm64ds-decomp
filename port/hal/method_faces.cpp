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

/* C-name-at-C++-linkage state Init refs (Main TUs call their own Init
   by Itanium name) plus round-3 small faces */
void _ZN6Player14St_OnWall_InitEv(char *self)
{ ((Player *)self)->Player::St_OnWall_Init(); }
void _ZN6Player17St_PunchKick_InitEv(void *self)
{ ((Player *)self)->Player::St_PunchKick_Init(); }
extern "C" int _Z14ApproachLinearRsss(short *x, short target, short step)
{ return ApproachLinear2(*x, target, step); }
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
/* gate 16: the actor teardown is a HOST COPY now (see
   port/unmatched/ActorBase_AfterCleanupResources.cpp -- the matched TU defines
   three engine globals rather than declaring them), so the slot-5 thunks that
   call the method need the method to exist. */
extern "C" void _ZN9ActorBase21AfterCleanupResourcesEj(void *self, unsigned a);
void ActorBase::AfterCleanupResources(u32 a)
{ _ZN9ActorBase21AfterCleanupResourcesEj(this, a); }

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
/* Bird::Render and FLAG's are each one line in src -- dispatch slot 5 of the
   ModelAnim at +0xd4 -- and ROM slot 5 is Render while MSVC slot 5 is
   Virtual18. Call the method the ROM means. */
int _ZN4Bird6RenderEv(void *self)
{ ((ModelAnim *)((char *)self + 0xd4))->ModelAnim::Render(0); return 1; }
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
int _ZN4Flag6RenderEv(void *self)
{ ((ModelAnim *)((char *)self + 0xd4))->ModelAnim::Render(0); return 1; }
}

/* TextureTransformer: two slots (the destructor pair) and nothing else, so
   the vtable the constructor installs is storage. Its Prepare and Update are
   methods; the water reaches both by their Itanium names. */
#include "TextureTransformer.h"
extern "C" {
void *_ZTV18TextureTransformer[4];
void func_02046b64(void *bmdTable, void *btaObj);
/* Two arguments, not three: func_02046b64 resolves the BTA's own material
   NAMES against the BMD's table, and the water's call site passes exactly
   those two with no `this`. */
void _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(void *bmd, void *bta)
{ func_02046b64(bmd, bta); }
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
