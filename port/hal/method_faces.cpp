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
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
void _ZN5Actor13SpawnSoundObjEj(void *self, u32 soundObjParam)
{ ((Actor *)self)->Actor::SpawnSoundObj(soundObjParam); }
#else
void _ZN5Actor13SpawnSoundObjEj(void *self, u32 soundObjParam);  /* Linux: real symbol from src/_ZN5Actor13SpawnSoundObjEj */
#endif /* _WIN32 */

/* Same rewrite, same story: a method now, spelled as a free function by every
   ov002 death path that calls it. */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
void _ZN5Actor24KillAndTrackInDeathTableEv(void *self)
{ ((Actor *)self)->Actor::KillAndTrackInDeathTable(); }
#else
void _ZN5Actor24KillAndTrackInDeathTableEv(void *self);  /* Linux: real symbol from src/_ZN5Actor24KillAndTrackInDeathTableEv */
#endif /* _WIN32 */

/* Actor::FindWithID is static -- no `this`, so the face is a plain forward.
   hal/reverse_bridges.cpp used to run this the other way, wrapping a C-form
   src definition into a method; main made the src file the method, so the
   wrapper there is gone and this replaces it. */
Actor *_ZN5Actor10FindWithIDEj(u32 id)
{ return Actor::FindWithID(id); }

#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
void _ZN10ModelAnim24CopyERKS_Pcj(void *self, const void *src, char *nf,
                                  unsigned nof)
{ ((ModelAnim2 *)self)->ModelAnim2::Copy(*(const ModelAnim2 *)src, nf, nof); }
#else
void _ZN10ModelAnim24CopyERKS_Pcj(void *self, const void *src, char *nf,
                                  unsigned nof);  /* Linux: real symbol from src/_ZN10ModelAnim24CopyERKS_Pcj */
#endif /* _WIN32 */


#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
void _ZN12CylinderClsn5ClearEv(void *self)
{ ((CylinderClsn *)self)->CylinderClsn::Clear(); }
#else
void _ZN12CylinderClsn5ClearEv(void *self);  /* Linux: real symbol from src/_ZN12CylinderClsn5ClearEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
void _ZN12CylinderClsn6UpdateEv(void *self)
{ ((CylinderClsn *)self)->CylinderClsn::Update(); }
#else
void _ZN12CylinderClsn6UpdateEv(void *self);  /* Linux: real symbol from src/_ZN12CylinderClsn6UpdateEv */
#endif /* _WIN32 */

#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
void _ZN12WithMeshClsn13SetGroundFlagEv(void *self)
{ ((WithMeshClsn *)self)->WithMeshClsn::SetGroundFlag(); }
#else
void _ZN12WithMeshClsn13SetGroundFlagEv(void *self);  /* Linux: real symbol from src/_ZN12WithMeshClsn13SetGroundFlagEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
void _ZN12WithMeshClsn13SetLimMovFlagEv(void *self)
{ ((WithMeshClsn *)self)->WithMeshClsn::SetLimMovFlag(); }
#else
void _ZN12WithMeshClsn13SetLimMovFlagEv(void *self);  /* Linux: real symbol from src/_ZN12WithMeshClsn13SetLimMovFlagEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
void _ZN12WithMeshClsn15ClearGroundFlagEv(void *self)
{ ((WithMeshClsn *)self)->WithMeshClsn::ClearGroundFlag(); }
#else
void _ZN12WithMeshClsn15ClearGroundFlagEv(void *self);  /* Linux: real symbol from src/_ZN12WithMeshClsn15ClearGroundFlagEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
void _ZN12WithMeshClsn15ClearLimMovFlagEv(void *self)
{ ((WithMeshClsn *)self)->WithMeshClsn::ClearLimMovFlag(); }
#else
void _ZN12WithMeshClsn15ClearLimMovFlagEv(void *self);  /* Linux: real symbol from src/_ZN12WithMeshClsn15ClearLimMovFlagEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
void _ZN12WithMeshClsn18StopDetectingWaterEv(void *self)
{ ((WithMeshClsn *)self)->WithMeshClsn::StopDetectingWater(); }
#else
void _ZN12WithMeshClsn18StopDetectingWaterEv(void *self);  /* Linux: real symbol from src/_ZN12WithMeshClsn18StopDetectingWaterEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
void _ZN12WithMeshClsn19ClearAllGroundFlagsEv(void *self)
{ ((WithMeshClsn *)self)->WithMeshClsn::ClearAllGroundFlags(); }
#else
void _ZN12WithMeshClsn19ClearAllGroundFlagsEv(void *self);  /* Linux: real symbol from src/_ZN12WithMeshClsn19ClearAllGroundFlagsEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
void _ZN12WithMeshClsn19StartDetectingWaterEv(void *self)
{ ((WithMeshClsn *)self)->WithMeshClsn::StartDetectingWater(); }
#else
void _ZN12WithMeshClsn19StartDetectingWaterEv(void *self);  /* Linux: real symbol from src/_ZN12WithMeshClsn19StartDetectingWaterEv */
#endif /* _WIN32 */

#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
void _ZN15TextureSequence6UpdateER15ModelComponents(void *self, void *mc)
{ ((TextureSequence *)self)->TextureSequence::Update(
      *(ModelComponents *)mc); }
#else
void _ZN15TextureSequence6UpdateER15ModelComponents(void *self, void *mc);  /* Linux: real symbol from src/_ZN15TextureSequence6UpdateER15ModelComponents */
#endif /* _WIN32 */

#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
void _ZN4BgCh19StartDetectingToxicEv(void *self)
{ ((BgCh *)self)->BgCh::StartDetectingToxic(); }
#else
void _ZN4BgCh19StartDetectingToxicEv(void *self);  /* Linux: real symbol from src/_ZN4BgCh19StartDetectingToxicEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
void _ZN4BgCh21StopDetectingOrdinaryEv(void *self)
{ ((BgCh *)self)->BgCh::StopDetectingOrdinary(); }
#else
void _ZN4BgCh21StopDetectingOrdinaryEv(void *self);  /* Linux: real symbol from src/_ZN4BgCh21StopDetectingOrdinaryEv */
#endif /* _WIN32 */

#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
void _ZN5Model14SetPolygonModeEi(void *self, int mode)
{ ((Model *)self)->Model::SetPolygonMode(mode); }
#else
void _ZN5Model14SetPolygonModeEi(void *self, int mode);  /* Linux: real symbol from src/_ZN5Model14SetPolygonModeEi */
#endif /* _WIN32 */

#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
void _ZN5Timer10ResetTimerEv(void *self)
{ ((Timer *)self)->Timer::ResetTimer(); }
#else
void _ZN5Timer10ResetTimerEv(void *self);  /* Linux: real symbol from src/_ZN5Timer10ResetTimerEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
void _ZN5Timer10StartTimerEv(void *self)
{ ((Timer *)self)->Timer::StartTimer(); }
#else
void _ZN5Timer10StartTimerEv(void *self);  /* Linux: real symbol from src/_ZN5Timer10StartTimerEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
long long _ZN5Timer7GetTimeEv(void *self)
{ return ((Timer *)self)->Timer::GetTime(); }
#else
long long _ZN5Timer7GetTimeEv(void *self);  /* Linux: real symbol from src/_ZN5Timer7GetTimeEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
void _ZN5Timer9StopTimerEv(void *self)
{ ((Timer *)self)->Timer::StopTimer(); }
#else
void _ZN5Timer9StopTimerEv(void *self);  /* Linux: real symbol from src/_ZN5Timer9StopTimerEv */
#endif /* _WIN32 */

#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN6Player12Unk_020c9e5cEh(void *self, unsigned char h)
{ return ((Player *)self)->Player::Unk_020c9e5c(h); }
#else
int _ZN6Player12Unk_020c9e5cEh(void *self, unsigned char h);  /* Linux: real symbol from src/_ZN6Player12Unk_020c9e5cEh */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN6Player16St_Shell_CleanupEv(void *self)
{ return ((Player *)self)->Player::St_Shell_Cleanup(); }
#else
int _ZN6Player16St_Shell_CleanupEv(void *self);  /* Linux: real symbol from src/_ZN6Player16St_Shell_CleanupEv */
#endif /* _WIN32 */
/* gate 194: func_ov002_020df7ac.c (one of HootTheOwl's ov002 carry-
   handshake leaves) calls this flat name directly -- gate 10 already links
   the matched src/_ZN6Player14St_Owl_CleanupEv.cpp body but nothing faced
   it before now. */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN6Player14St_Owl_CleanupEv(void *self)
{ return ((Player *)self)->Player::St_Owl_Cleanup(); }
#else
int _ZN6Player14St_Owl_CleanupEv(void *self);  /* Linux: real symbol from src/_ZN6Player14St_Owl_CleanupEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
void _ZN6Player18SetNewHatCharacterEjjb(void *self, unsigned a, unsigned b,
                                        unsigned char c)
{ ((Player *)self)->Player::SetNewHatCharacter(a, b, c != 0); }
#else
void _ZN6Player18SetNewHatCharacterEjjb(void *self, unsigned a, unsigned b,
                                        unsigned char c);  /* Linux: real symbol from src/_ZN6Player18SetNewHatCharacterEjjb */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
void _ZN6Player18TurnOffToonShadingEj(void *self, unsigned j)
{ ((Player *)self)->Player::TurnOffToonShading(j); }
#else
void _ZN6Player18TurnOffToonShadingEj(void *self, unsigned j);  /* Linux: real symbol from src/_ZN6Player18TurnOffToonShadingEj */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN6Player22IsBeingShotOutOfCannonEv(void *self)
{ return ((Player *)self)->Player::IsBeingShotOutOfCannon(); }
#else
int _ZN6Player22IsBeingShotOutOfCannonEv(void *self);  /* Linux: real symbol from src/_ZN6Player22IsBeingShotOutOfCannonEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN6Player7IsInAirEv(void *self)
{ return ((Player *)self)->Player::IsInAir(); }
#else
int _ZN6Player7IsInAirEv(void *self);  /* Linux: real symbol from src/_ZN6Player7IsInAirEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
void _ZN6Player4HealEi(void *self, int amt)
{ ((Player *)self)->Player::Heal(amt); }
#else
void _ZN6Player4HealEi(void *self, int amt);  /* Linux: real symbol from src/_ZN6Player4HealEi */
#endif /* _WIN32 */


#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
void _ZN9ActorBase18MarkForDestructionEv(void *self)
{ ((ActorBase *)self)->ActorBase::MarkForDestruction(); }
#else
void _ZN9ActorBase18MarkForDestructionEv(void *self);  /* Linux: real symbol from src/_ZN9ActorBase18MarkForDestructionEv */
#endif /* _WIN32 */

/* Gate 31: the Player's two DESTROY faces. Both definitions are real
   __thiscall methods -- Player::OnPendingDestroy in src, CleanupResources in
   port/unmatched/ -- and hal/level_boot.cpp seats them in vtable slots 12 and
   3, where the ROM's teardown dispatches them. Faces rather than
   /alternatename aliases for the usual reason: a slot thunk entered with the
   object in ecx would hand a cdecl body a `this` it never reads. */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN6Player16CleanupResourcesEv(void *self)
{ return ((Player *)self)->Player::CleanupResources(); }
#else
int _ZN6Player16CleanupResourcesEv(void *self);  /* Linux: real symbol from src/_ZN6Player16CleanupResourcesEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
void _ZN6Player16OnPendingDestroyEv(void *self)
{ ((Player *)self)->Player::OnPendingDestroy(); }
#else
void _ZN6Player16OnPendingDestroyEv(void *self);  /* Linux: real symbol from src/_ZN6Player16OnPendingDestroyEv */
#endif /* _WIN32 */

/* Gate 15: Actor::BeforeBehavior is a .c-style TU that calls its base by
   Itanium name, while the definition is a real __thiscall method. */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN9ActorBase14BeforeBehaviorEv(void *self)
{ return ((ActorBase *)self)->ActorBase::BeforeBehavior() ? 1 : 0; }
#else
int _ZN9ActorBase14BeforeBehaviorEv(void *self);  /* Linux: real symbol from src/_ZN9ActorBase14BeforeBehaviorEv */
#endif /* _WIN32 */

#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the const method it forwards to -> self-recurse on GCC. On Linux bind to the real src/ TU. */
unsigned _ZNK7PathPtr8NumNodesEv(const void *self)
{ return ((const PathPtr *)self)->PathPtr::NumNodes(); }
#else
unsigned _ZNK7PathPtr8NumNodesEv(const void *self);
#endif /* _WIN32 */


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
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
extern "C" void _ZN6Player14St_OnWall_InitEv(char *self)
{ ((Player *)self)->Player::St_OnWall_Init(); }
#else
extern "C" void _ZN6Player14St_OnWall_InitEv(char *self);  /* Linux: real symbol from src/_ZN6Player14St_OnWall_InitEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
extern "C" void _ZN6Player17St_PunchKick_InitEv(void *self)
{ ((Player *)self)->Player::St_PunchKick_Init(); }
#else
extern "C" void _ZN6Player17St_PunchKick_InitEv(void *self);  /* Linux: real symbol from src/_ZN6Player17St_PunchKick_InitEv */
#endif /* _WIN32 */
#pragma comment(linker, "/alternatename:?_ZN6Player14St_OnWall_InitEv@@YAXPAD@Z=__ZN6Player14St_OnWall_InitEv")
#pragma comment(linker, "/alternatename:?_ZN6Player17St_PunchKick_InitEv@@YAXPAX@Z=__ZN6Player17St_PunchKick_InitEv")
extern "C" int _Z14ApproachLinearRsss(short *x, short target, short step)
{ return ApproachLinear2(*x, target, step); }
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
extern "C" int _ZN6Player15IsCollectingCapEv(char *self)
{ return ((Player *)self)->Player::IsCollectingCap(); }
#else
extern "C" int _ZN6Player15IsCollectingCapEv(char *self);  /* Linux: real symbol from src/_ZN6Player15IsCollectingCapEv */
#endif /* _WIN32 */

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
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
extern "C" int _ZN6Player9DropActorEv(void *self)
{ return ((Player *)self)->Player::DropActor(); }
#else
extern "C" int _ZN6Player9DropActorEv(void *self);  /* Linux: real symbol from src/_ZN6Player9DropActorEv */
#endif /* _WIN32 */

/* gate 14: the init chain the actor spawn spine dispatches. Both are real
   __thiscall methods, so a linker alias onto the Itanium name their .c
   callers use would enter the body with `this` in whatever ecx held. */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
extern "C" int _ZN5Actor18GetBitInDeathTableEv(void *self)
{ return ((Actor *)self)->Actor::GetBitInDeathTable(); }
#else
extern "C" int _ZN5Actor18GetBitInDeathTableEv(void *self);  /* Linux: real symbol from src/_ZN5Actor18GetBitInDeathTableEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
extern "C" void _ZN5Actor18AfterInitResourcesEj(void *self, unsigned a)
{ ((Actor *)self)->Actor::AfterInitResources(a); }
#else
extern "C" void _ZN5Actor18AfterInitResourcesEj(void *self, unsigned a);  /* Linux: real symbol from src/_ZN5Actor18AfterInitResourcesEj */
#endif /* _WIN32 */

/* gate 16: Actor::BeforeRender is the same shape -- a .c TU calling its base
   by Itanium name over a real __thiscall definition. Slot 10 of every actor
   class the registry carries goes through it. */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
extern "C" int _ZN9ActorBase12BeforeRenderEv(void *self)
{ return ((ActorBase *)self)->ActorBase::BeforeRender(); }
#else
extern "C" int _ZN9ActorBase12BeforeRenderEv(void *self);  /* Linux: real symbol from src/_ZN9ActorBase12BeforeRenderEv */
#endif /* _WIN32 */

/* gate 16: ModelBase::ApplyOpacity is a real method whose only caller,
   Tree::Render, spells it as an Itanium C name (and passes a third argument
   the ROM's r2 carried into a two-parameter body; cdecl lets the caller keep
   cleaning it). */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
extern "C" void _ZN9ModelBase12ApplyOpacityEj(void *self, unsigned a)
{ ((ModelBase *)self)->ModelBase::ApplyOpacity(a); }
#else
extern "C" void _ZN9ModelBase12ApplyOpacityEj(void *self, unsigned a);  /* Linux: real symbol from src/_ZN9ModelBase12ApplyOpacityEj */
#endif /* _WIN32 */

/* Model::UpdateFileOffsets is a STATIC member (include/Model.h), which is why
   func_02016ff4 calls it with the file alone and no `this` -- the Itanium name
   is the same either way, so only the header says which. Face, not alias:
   MSVC decorates a static member differently again. */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
extern "C" void _ZN5Model17UpdateFileOffsetsER8BMD_File(BMD_File *f)
{ Model::UpdateFileOffsets(*f); }
#else
extern "C" void _ZN5Model17UpdateFileOffsetsER8BMD_File(BMD_File *f);  /* Linux: real symbol from src/_ZN5Model17UpdateFileOffsetsER8BMD_File */
#endif /* _WIN32 */

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
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
extern "C" void _ZN4Heap10ReallocateEPvj(void *self, void *p, unsigned n)
{ ((Heap *)self)->Heap::Reallocate(p, n); }
#else
extern "C" void _ZN4Heap10ReallocateEPvj(void *self, void *p, unsigned n);  /* Linux: real symbol from src/_ZN4Heap10ReallocateEPvj */
#endif /* _WIN32 */

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
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN12MeshCollider10DetectClsnER10SphereClsn(void *self, void *sph)
{ return ((MeshCollider *)self)->MeshCollider::DetectClsn(*(SphereClsn *)sph); }
#else
int _ZN12MeshCollider10DetectClsnER10SphereClsn(void *self, void *sph);  /* Linux: real symbol from src/_ZN12MeshCollider10DetectClsnER10SphereClsn */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN16MeshColliderBase9IsEnabledEv(void *self)
{ return ((MeshColliderBase *)self)->MeshColliderBase::IsEnabled(); }
#else
int _ZN16MeshColliderBase9IsEnabledEv(void *self);  /* Linux: real symbol from src/_ZN16MeshColliderBase9IsEnabledEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN16MeshColliderBase7DisableEv(void *self)
{ return ((MeshColliderBase *)self)->MeshColliderBase::Disable(); }
#else
int _ZN16MeshColliderBase7DisableEv(void *self);  /* Linux: real symbol from src/_ZN16MeshColliderBase7DisableEv */
#endif /* _WIN32 */
}

/* ---- gate 16: five more C-named references onto method definitions -------
   The 1-up, the sign and the brick block reach these by their Itanium names
   from .c TUs; every one of them is a real __thiscall method in src. */
extern "C" {
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN6Player14IsFrontSlidingEv(void *self)
{ return ((Player *)self)->Player::IsFrontSliding(); }
#else
int _ZN6Player14IsFrontSlidingEv(void *self);  /* Linux: real symbol from src/_ZN6Player14IsFrontSlidingEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN6Player15IsEnteringLevelEv(void *self)
{ return ((Player *)self)->Player::IsEnteringLevel(); }
#else
int _ZN6Player15IsEnteringLevelEv(void *self);  /* Linux: real symbol from src/_ZN6Player15IsEnteringLevelEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN6Player17LostGrabbedObjectEv(void *self)
{ return ((Player *)self)->Player::LostGrabbedObject(); }
#else
int _ZN6Player17LostGrabbedObjectEv(void *self);  /* Linux: real symbol from src/_ZN6Player17LostGrabbedObjectEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN6Player20IsStateEnteringLevelEv(void *self)
{ return ((Player *)self)->Player::IsStateEnteringLevel(); }
#else
int _ZN6Player20IsStateEnteringLevelEv(void *self);  /* Linux: real symbol from src/_ZN6Player20IsStateEnteringLevelEv */
#endif /* _WIN32 */
}

#include "PowerStar.h"
extern "C" {
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
void _ZN12WithMeshClsn20UpdateDiscreteNoLavaEv(void *self)
{ ((WithMeshClsn *)self)->WithMeshClsn::UpdateDiscreteNoLava(); }
#else
void _ZN12WithMeshClsn20UpdateDiscreteNoLavaEv(void *self);  /* Linux: real symbol from src/_ZN12WithMeshClsn20UpdateDiscreteNoLavaEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
void _ZN12WithMeshClsn22UpdateDiscreteNoLava_2Ev(void *self)
{ ((WithMeshClsn *)self)->WithMeshClsn::UpdateDiscreteNoLava_2(); }
#else
void _ZN12WithMeshClsn22UpdateDiscreteNoLava_2Ev(void *self);  /* Linux: real symbol from src/_ZN12WithMeshClsn22UpdateDiscreteNoLava_2Ev */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
void _ZN9PowerStar13AddStarMarkerEv(void *self)
{ ((PowerStar *)self)->PowerStar::AddStarMarker(); }
#else
void _ZN9PowerStar13AddStarMarkerEv(void *self);  /* Linux: real symbol from src/_ZN9PowerStar13AddStarMarkerEv */
#endif /* _WIN32 */
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
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN4Bird13InitResourcesEv(void *self)
{ return ((Bird *)self)->Bird::InitResources(); }
#else
int _ZN4Bird13InitResourcesEv(void *self);  /* Linux: real symbol from src/_ZN4Bird13InitResourcesEv */
#endif /* _WIN32 */
/* Bird::Render and FLAG's are each one line in src -- dispatch slot 5 of the
   ModelAnim at +0xd4 -- and ROM slot 5 is Render while MSVC slot 5 is
   Virtual18. Call the method the ROM means. */
int _ZN4Bird6RenderEv(void *self)
{ ((ModelAnim *)((char *)self + 0xd4))->ModelAnim::Render(0); return 1; }
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN8MetalNet13InitResourcesEv(void *self)
{ return ((MetalNet *)self)->MetalNet::InitResources(); }
#else
int _ZN8MetalNet13InitResourcesEv(void *self);  /* Linux: real symbol from src/_ZN8MetalNet13InitResourcesEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN8MetalNet8BehaviorEv(void *self)
{ return ((MetalNet *)self)->MetalNet::Behavior(); }
#else
int _ZN8MetalNet8BehaviorEv(void *self);  /* Linux: real symbol from src/_ZN8MetalNet8BehaviorEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN8MetalNet6RenderEv(void *self)
{ return ((MetalNet *)self)->MetalNet::Render(); }
#else
int _ZN8MetalNet6RenderEv(void *self);  /* Linux: real symbol from src/_ZN8MetalNet6RenderEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN8MetalNet16CleanupResourcesEv(void *self)
{ return ((MetalNet *)self)->MetalNet::CleanupResources(); }
#else
int _ZN8MetalNet16CleanupResourcesEv(void *self);  /* Linux: real symbol from src/_ZN8MetalNet16CleanupResourcesEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN4Flag13InitResourcesEv(void *self)
{ return ((Flag *)self)->Flag::InitResources(); }
#else
int _ZN4Flag13InitResourcesEv(void *self);  /* Linux: real symbol from src/_ZN4Flag13InitResourcesEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN4Flag8BehaviorEv(void *self)
{ return ((Flag *)self)->Flag::Behavior(); }
#else
int _ZN4Flag8BehaviorEv(void *self);  /* Linux: real symbol from src/_ZN4Flag8BehaviorEv */
#endif /* _WIN32 */
int _ZN4Flag6RenderEv(void *self)
{ ((ModelAnim *)((char *)self + 0xd4))->ModelAnim::Render(0); return 1; }
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
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN15IceSlideManager13InitResourcesEv(void *self)
{ return ((IceSlideManager *)self)->IceSlideManager::InitResources(); }
#else
int _ZN15IceSlideManager13InitResourcesEv(void *self);  /* Linux: real symbol from src/_ZN15IceSlideManager13InitResourcesEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN15IceSlideManager8BehaviorEv(void *self)
{ return ((IceSlideManager *)self)->IceSlideManager::Behavior(); }
#else
int _ZN15IceSlideManager8BehaviorEv(void *self);  /* Linux: real symbol from src/_ZN15IceSlideManager8BehaviorEv */
#endif /* _WIN32 */
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
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
void _ZN18TextureTransformer6UpdateER15ModelComponents(void *self, void *mc)
{ ((TextureTransformer *)self)->TextureTransformer::Update(
      *(ModelComponents *)mc); }
#else
void _ZN18TextureTransformer6UpdateER15ModelComponents(void *self, void *mc);  /* Linux: real symbol from src/_ZN18TextureTransformer6UpdateER15ModelComponents */
#endif /* _WIN32 */

/* Model::LoadAndSetFile, for the boot spine (gate 24). The definition in
   src/_ZN5Model14LoadAndSetFileEtii.cpp is a real MSVC member -- it lands in
   the map as ?LoadAndSetFile@Model@@QAEXGHH@Z -- and its callers here,
   Stage::LoadModel and Stage::LoadSkybox, are a .cpp inside extern "C" and a
   plain .c respectively. Neither can spell the C++ name, and a linker alias
   would be wrong anyway: the decoration is QAE, __thiscall, so aliasing it
   onto a cdecl declaration would hand the body a `this` that never was one. */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
void _ZN5Model14LoadAndSetFileEtii(void *self, unsigned short id, int a, int b)
{ ((Model *)self)->Model::LoadAndSetFile(id, a, b); }
#else
void _ZN5Model14LoadAndSetFileEtii(void *self, unsigned short id, int a, int b);  /* Linux: real symbol from src/_ZN5Model14LoadAndSetFileEtii */
#endif /* _WIN32 */
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
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN6Rabbit13InitResourcesEv(void *self)
{ return ((Rabbit *)self)->Rabbit::InitResources(); }
#else
int _ZN6Rabbit13InitResourcesEv(void *self);  /* Linux: real symbol from src/_ZN6Rabbit13InitResourcesEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN9LakituBro13InitResourcesEv(void *self)
{ return ((LakituBro *)self)->LakituBro::InitResources(); }
#else
int _ZN9LakituBro13InitResourcesEv(void *self);  /* Linux: real symbol from src/_ZN9LakituBro13InitResourcesEv */
#endif /* _WIN32 */
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
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN9RabbitKey13InitResourcesEv(void *self)
{ return ((RabbitKey *)self)->RabbitKey::InitResources(); }
#else
int _ZN9RabbitKey13InitResourcesEv(void *self);  /* Linux: real symbol from src/_ZN9RabbitKey13InitResourcesEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN9RabbitKey8BehaviorEv(void *self)
{ return ((RabbitKey *)self)->RabbitKey::Behavior(); }
#else
int _ZN9RabbitKey8BehaviorEv(void *self);  /* Linux: real symbol from src/_ZN9RabbitKey8BehaviorEv */
#endif /* _WIN32 */
}

/* ---- gate 40: ov100's STAR_DOOR (the config's _ZN4Door* family) ----------
   Two C-named references onto method definitions, the gate-18 shape. The star
   door's InitResources and Behavior are real C++ methods on `Door` (Door.h);
   its Render, CleanupResources, OnPendingDestroy and D0 are already C-named in
   src, and slot 16 reuses ac_d1_door (the member is a CommonModel at 0xd4, the
   real door's layout), so only these two need a face. */
#include "Door.h"
extern "C" {
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN4Door13InitResourcesEv(void *self)
{ return ((Door *)self)->Door::InitResources(); }
#else
int _ZN4Door13InitResourcesEv(void *self);  /* Linux: real symbol from src/_ZN4Door13InitResourcesEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN4Door8BehaviorEv(void *self)
{ return ((Door *)self)->Door::Behavior(); }
#else
int _ZN4Door8BehaviorEv(void *self);  /* Linux: real symbol from src/_ZN4Door8BehaviorEv */
#endif /* _WIN32 */
}

/* ---- gate 41: ov010's TRAP (shared by LIGHT_BEAM) ------------------------
   Three C-named references onto method definitions, the gate-18 shape. TRAP's
   InitResources, Behavior and Render are real C++ methods; its
   CleanupResources and D0 are already C-named in src, and slots 16/17 trap
   (nothing destroys one on the castle-interior boot -- the gate-17 reading). */
#include "Trap.h"
extern "C" {
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN4Trap13InitResourcesEv(void *self)
{ return ((Trap *)self)->Trap::InitResources(); }
#else
int _ZN4Trap13InitResourcesEv(void *self);  /* Linux: real symbol from src/_ZN4Trap13InitResourcesEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN4Trap8BehaviorEv(void *self)
{ return ((Trap *)self)->Trap::Behavior(); }
#else
int _ZN4Trap8BehaviorEv(void *self);  /* Linux: real symbol from src/_ZN4Trap8BehaviorEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN4Trap6RenderEv(void *self)
{ return ((Trap *)self)->Trap::Render(); }
#else
int _ZN4Trap6RenderEv(void *self);  /* Linux: real symbol from src/_ZN4Trap6RenderEv */
#endif /* _WIN32 */
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
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN13PeachPainting13InitResourcesEv(void *self)
{ return ((PeachPainting *)self)->PeachPainting::InitResources(); }
#else
int _ZN13PeachPainting13InitResourcesEv(void *self);  /* Linux: real symbol from src/_ZN13PeachPainting13InitResourcesEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN13PeachPainting8BehaviorEv(void *self)
{ return ((PeachPainting *)self)->PeachPainting::Behavior(); }
#else
int _ZN13PeachPainting8BehaviorEv(void *self);  /* Linux: real symbol from src/_ZN13PeachPainting8BehaviorEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN13PeachPainting6RenderEv(void *self)
{ return ((PeachPainting *)self)->PeachPainting::Render(); }
#else
int _ZN13PeachPainting6RenderEv(void *self);  /* Linux: real symbol from src/_ZN13PeachPainting6RenderEv */
#endif /* _WIN32 */
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
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN6Player12GetTalkStateEv(void *self)
{ return ((Player *)self)->Player::GetTalkState(); }
#else
int _ZN6Player12GetTalkStateEv(void *self);  /* Linux: real symbol from src/_ZN6Player12GetTalkStateEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN6Player18HasFinishedTalkingEv(void *self)
{ return ((Player *)self)->Player::HasFinishedTalking(); }
#else
int _ZN6Player18HasFinishedTalkingEv(void *self);  /* Linux: real symbol from src/_ZN6Player18HasFinishedTalkingEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
void _ZN9ModelAnim6RenderEPK7Vector3(void *self, const void *scale)
{ ((ModelAnim *)self)->ModelAnim::Render((const Vector3 *)scale); }
#else
void _ZN9ModelAnim6RenderEPK7Vector3(void *self, const void *scale);  /* Linux: real symbol from src/_ZN9ModelAnim6RenderEPK7Vector3 */
#endif /* _WIN32 */
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
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN6Cannon13InitResourcesEv(void *self)
{ return ((Cannon *)self)->Cannon::InitResources(); }
#else
int _ZN6Cannon13InitResourcesEv(void *self);  /* Linux: real symbol from src/_ZN6Cannon13InitResourcesEv */
#endif /* _WIN32 */
}

/* ---- gate 20: ov002's WATERFALL_MIST -------------------------------------
   One more, the same shape as the cannon's. src's body is a real C++ method
   against include/PoppingLavaBubbles.h (the class the ROM's RTTI calls
   daObjWaterfall_c), so MSVC emits it under ?InitResources@... and the
   vtable fill wants the Itanium name. */
#include "PoppingLavaBubbles.h"
extern "C" {
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN18PoppingLavaBubbles13InitResourcesEv(void *self)
{ return ((PoppingLavaBubbles *)self)->InitResources(); }
#else
int _ZN18PoppingLavaBubbles13InitResourcesEv(void *self);  /* Linux: real symbol from src/_ZN18PoppingLavaBubbles13InitResourcesEv */
#endif /* _WIN32 */
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
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN9Butterfly13InitResourcesEv(void *self)
{ return ((Butterfly *)self)->Butterfly::InitResources(); }
#else
int _ZN9Butterfly13InitResourcesEv(void *self);  /* Linux: real symbol from src/_ZN9Butterfly13InitResourcesEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN4Fish13InitResourcesEv(void *self)
{ return ((Fish *)self)->Fish::InitResources(); }
#else
int _ZN4Fish13InitResourcesEv(void *self);  /* Linux: real symbol from src/_ZN4Fish13InitResourcesEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN4Fish16CleanupResourcesEv(void *self)
{ return ((Fish *)self)->Fish::CleanupResources(); }
#else
int _ZN4Fish16CleanupResourcesEv(void *self);  /* Linux: real symbol from src/_ZN4Fish16CleanupResourcesEv */
#endif /* _WIN32 */
}

/* Animation::GetFrameCount is a real const method too, and gate 21 is the
   first thing to call it: the butterfly asks its own animation how long it is
   before it will loop, and one of the fish's states does the same. */
#include "Animation.h"
extern "C" {
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. On Linux bind to the real src/_ZNK9Animation13GetFrameCountEv TU. */
unsigned _ZNK9Animation13GetFrameCountEv(const void *self)
{ return ((const Animation *)self)->GetFrameCount(); }
#else
unsigned _ZNK9Animation13GetFrameCountEv(const void *self);
#endif /* _WIN32 */
}

/* ---- gate 23: ov102's QUESTION_BLOCK -------------------------------------
   Two more of the same shape; its InitResources is already C-named and its
   Render is a host copy (port/unmatched/ModelAnim_Renders.cpp). */
#include "QuestionBlock.h"
extern "C" {
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN13QuestionBlock8BehaviorEv(void *self)
{ return ((QuestionBlock *)self)->QuestionBlock::Behavior(); }
#else
int _ZN13QuestionBlock8BehaviorEv(void *self);  /* Linux: real symbol from src/_ZN13QuestionBlock8BehaviorEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN13QuestionBlock16CleanupResourcesEv(void *self)
{ return ((QuestionBlock *)self)->QuestionBlock::CleanupResources(); }
#else
int _ZN13QuestionBlock16CleanupResourcesEv(void *self);  /* Linux: real symbol from src/_ZN13QuestionBlock16CleanupResourcesEv */
#endif /* _WIN32 */
}

/* ---- gate 22: the door ring's Player entry points ------------------------
   These were direct link aliases in cxx_aliases.cpp ("same body, no thunk
   needed"), which on 32-bit MSVC called __thiscall methods with a cdecl
   frame: garbage `this`, and for the one-stack-arg methods a 4-byte ESP
   imbalance -- the 2026-08-07 door-open crash. Real faces, like everything
   else in this file. The Camera pair lives in hal/door_ring_faces.cpp
   because its defining TUs use a local Camera mirror, not include/Camera.h. */
extern "C" {
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
void _ZN6Player11OpenBigDoorEv(void *self)
{ ((Player *)self)->OpenBigDoor(); }
#else
void _ZN6Player11OpenBigDoorEv(void *self);  /* Linux: real symbol from src/_ZN6Player11OpenBigDoorEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
void _ZN6Player16SetRealCharacterEj(void *self, unsigned int chr_)
{ ((Player *)self)->SetRealCharacter(chr_); }
#else
void _ZN6Player16SetRealCharacterEj(void *self, unsigned int chr_);  /* Linux: real symbol from src/_ZN6Player16SetRealCharacterEj */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
void _ZN6Player12Unk_020ca488Ev(void *self)
{ ((Player *)self)->Unk_020ca488(); }
#else
void _ZN6Player12Unk_020ca488Ev(void *self);  /* Linux: real symbol from src/_ZN6Player12Unk_020ca488Ev */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN6Player21IsOpeningDoorWithStarEv(void *self)
{ return ((Player *)self)->IsOpeningDoorWithStar(); }
#else
int _ZN6Player21IsOpeningDoorWithStarEv(void *self);  /* Linux: real symbol from src/_ZN6Player21IsOpeningDoorWithStarEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN6Player13TryTalkToDoorEh(void *self, unsigned char a)
{ return ((Player *)self)->TryTalkToDoor(a); }
#else
int _ZN6Player13TryTalkToDoorEh(void *self, unsigned char a);  /* Linux: real symbol from src/_ZN6Player13TryTalkToDoorEh */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN6Player16TryTalkToKeyDoorEv(void *self)
{ return ((Player *)self)->TryTalkToKeyDoor(); }
#else
int _ZN6Player16TryTalkToKeyDoorEv(void *self);  /* Linux: real symbol from src/_ZN6Player16TryTalkToKeyDoorEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN6Player24TryExitWhiteDoorWithStarEv(void *self)
{ return ((Player *)self)->TryExitWhiteDoorWithStar(); }
#else
int _ZN6Player24TryExitWhiteDoorWithStarEv(void *self);  /* Linux: real symbol from src/_ZN6Player24TryExitWhiteDoorWithStarEv */
#endif /* _WIN32 */
/* gate 200: PushBlock's break branch grants the flower power-up
   (func_ov002_020b979c) -- the daObjPowerUpItem_c inheritance is not just
   RTTI trivia. Both matched src, real C++ methods. */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
void _ZN6Player16InitBalloonMarioEv(void *self)
{ ((Player *)self)->InitBalloonMario(); }
#else
void _ZN6Player16InitBalloonMarioEv(void *self);  /* Linux: real symbol from src/_ZN6Player16InitBalloonMarioEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
void _ZN6Player13InitFireYoshiEv(void *self)
{ ((Player *)self)->InitFireYoshi(); }
#else
void _ZN6Player13InitFireYoshiEv(void *self);  /* Linux: real symbol from src/_ZN6Player13InitFireYoshiEv */
#endif /* _WIN32 */
}  /* extern "C" */
