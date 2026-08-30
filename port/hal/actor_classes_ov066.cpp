// run rel0215 wave 2, lane cast-ov066 (W2-ov066): EYEROK (176, 10daIwante_c) --
// ov066, the SSL pyramid boss, wanted by level 17 alone. Fresh per-symbol mount
// (port/ov066_syms.txt), the ov077/ov079/ov080/ov081 convention. Slice is
// port/slice_ov066.txt.
//
// ============================================================================
// ONE CLASS, NOT THREE, AND THE ROM SAYS SO FOUR WAYS.
// ============================================================================
// A boss overlay can carry several cooperating classes (hand left / hand right
// / head) and this one does not. The whole of ov066's .data was dumped word by
// word out of extracted/overlays/overlay_0066.bin (never a dsd export, trap T4)
// and it carries EXACTLY ONE __si_class_type_info record, ONE vtable and ONE
// SpawnInfo:
//
//   0x0211ad24  typeinfo   [0x0209a764 __si_class_type_info][name][base]
//   0x0211ad30  name       "10daIwante_c"   iwa + te = ROCK HAND
//   0x021089ec  base       its own name string reads "10dBgActor_c" = Platform
//   0x0211ad40  SpawnInfo  word[0] Eyerok_Spawn, word[1] low half 176
//   0x0211ad64  vtable     32 slots, slot 31 = ov002 Platform::Kill
//
// The two hands and the head are STATES of one Platform-derived actor. That is
// why .data holds a THIRTY-EIGHT ENTRY pointer-to-member source table instead
// of three class blocks, and why the class's own object is 0x874 bytes with a
// twenty-slot particle ring in it. dsd's _ZTV6Eyerok / _ZN6Eyerok* names and
// the ROM's daIwante_c are two spellings of ONE class -- checked the ov077 way,
// by reading the table's OWN vtable[-1] typeinfo through to its name string.
// There is no ov026/ov030/ov036-style class-identity SHIFT here, because there
// is only one class for a shift to land on.
//
// ============================================================================
// THE SLOT MAP, out of config/arm9/overlays/ov066/relocs.txt with every target
// resolved through config/arm9/symbols.txt, cross-read against the raw bytes.
// ============================================================================
//  slot  ROM word   what it is
//    0   02119ce8   Eyerok::InitResources          own (MSVC method)
//    1   02011268   Actor::BeforeInitResources
//    2   02011244   Actor::AfterInitResources(u32)
//    3   02119654   Eyerok::CleanupResources       own (MSVC method)
//    4   02011220   Actor::BeforeCleanupResources
//    5   02011214   Actor::AfterCleanupResources(u32)
//    6   02119838   Eyerok::Behavior               own -> HOST COPY
//    7   02010fd4   Actor::BeforeBehavior
//    8   02010fc8   Actor::AfterBehavior(u32)
//    9   021197a4   Eyerok::Render                 own -> HOST COPY
//   10   02010f78   Actor::BeforeRender
//   11   02010f6c   Actor::AfterRender(u32)
//   12   021197a0   Eyerok::OnPendingDestroy       own, an EMPTY 4-byte body
//   13   0204357c   ActorBase::Virtual34(u32,u32)  trapped
//   14   0204349c   ActorBase::Virtual38(u32,u32)  trapped
//   15   02043494   ActorBase::OnHeapCreated
//   16   02115ee0   EyerokD1                       own
//   17   02115f84   EyerokD0                       own
//   18   02010160   Actor::OnYoshiTryEat
//   19   02010154   Actor::OnTurnIntoEgg(Player&)
//   20   0201014c   Actor::Virtual50
//   21   02010148   Actor::OnGroundPounded(Actor&)
//   22   02010144   Actor::OnAttacked1(Actor&)
//   23   02010140   Actor::OnAttacked2(Actor&)
//   24   0201013c   Actor::OnKicked(Actor&)
//   25   02010138   Actor::OnPushed(Actor&)
//   26   02010134   Actor::OnHitByCannonBlastedChar(Actor&)
//   27   02010130   Actor::OnHitByMegaChar(Player&)
//   28   0201012c   Actor::OnHitFromUnderneath(Actor&)
//   29   0211a2dc   OWN OnAimedAtWithEgg, overrides the shared 0x02010158
//   30   020100dc   Actor::OnAimedAtWithEggReturnVec  trapped (SRET)
//   31   020ee55c   Platform::Kill                 the 32nd slot
//   32+  00000000   .data tail padding to .bss's align-32 boundary, NOT slots
//
// THIRTY-TWO, NOT THIRTY-ONE. dsd's next-symbol delta says 39 slots (the next
// symbol is data_ov066_0211ae00, the .bss start) and it LIES -- 32..38 are zero
// in the raw image and the reloc run stops at 31. The 32nd slot is real and is
// the whole point: Platform::Kill. Mario stands on the hands.
//
// ============================================================================
// TRAP T1: THIS TABLE STAYS ROM-SHAPED.
// ============================================================================
// Eyerok derives Platform (dBgActor_c), not Model and not ModelAnim, so the
// default applies and the array is indexed exactly as the ROM indexes it, D1 at
// 16 and D0 at 17. The Model/ModelAnim exception in hal/cxxname_bridge.cpp does
// not reach the class table. It DOES reach two of the class's MEMBERS, which is
// a different question and is handled in port/unmatched/Eyerok_Render.cpp.
//
// ============================================================================
// TRAP T5: ONE MARKER-CARRYING BODY, ADJUDICATED BEFORE IT WAS SEATED.
// ============================================================================
// src/func_ov066_0211a2dc.c is the only file in the overlay carrying
// "recovered from vtable slot identity", and this file seats it at slot 29, so
// it needs a pre-seat ROM ruling. It got one:
//
//   ROM 0x0211a2dc, size 0x8:  0a09a0e3  mov r0, #0x28000
//                              1eff2fe1  bx lr
//   src:                       int func_ov066_0211a2dc(void) { return 163840; }
//   163840 == 0x28000. Two instructions, two statements.
//
//   tools/match.py --module ov066 --strict-relocs against
//   extracted/overlays/overlay_0066.bin at base 0x02115ee0:
//   MATCHING VERSIONS: 2004/b56.
//
// T5's FIRST branch: the NAME was recovered from a vtable slot, the BODY is
// real. The row is in port/tools/inferred_stub_adjudicated.txt. Nothing was
// added to inferred_stub_baseline.txt. The body still owes a decomp-side marker
// correction; this lane does not edit src/, so it is owed and not done here.
//
// ============================================================================
// THE NINETEEN-CELL STATE SEAT.
// ============================================================================
// .data 0x0211abe8..0x0211ad18 holds THIRTY-EIGHT {function, 0} source pairs.
// __sinit_ov066_0211a418 copies them into NINETEEN 16-byte .bss records at
// 0x0211afcc..0x0211b0ec as `S16 { S8 x, y; }` -- x the ENTER pair, y the TICK
// pair. The class reaches them two ways and only two:
//
//   func_ov066_02119454(c, &record)  installs the record at c+0x48c and
//                                    immediately calls its ENTER pair
//   Eyerok::Behavior                 reads c+0x48c and calls the TICK pair
//                                    (its own `struct State { char pad[8];
//                                    PMF fn; }` names the +8 offset out loud)
//
// Both are host copies (port/unmatched/Eyerok_StateEnter.cpp and
// Eyerok_Behavior.cpp) because mwcc's pointer-to-member on a deliberately
// incomplete class is the ROM's 8-byte pair and MSVC's is not. The seat below
// rewrites the nineteen records' two function words with HOST addresses, the
// ov030/ov072/ov081 shape, and VERIFIES the ROM values first: a mount pointing
// at the wrong bytes ABORTS instead of silently calling into unmapped memory.
//
// Nineteen states, thirty-eight pairs, thirty-eight distinct ov066 bodies, and
// every ROM address below is a named ov066 function symbol -- checked, not
// assumed. The cell ORDER is __sinit_ov066_0211a418.c's own assignment order,
// not sorted by address.
//
// ============================================================================
// WHAT THIS FILE DOES NOT CLAIM
// ============================================================================
//  - Whether the boss LOOKS right. Nobody has held this next to a DS. The lane
//    reports spawn counts and fault-free frames, not a verdict.
//  - What the nineteen states MEAN. They are seated by address, in the sinit's
//    own field order.
//  - That Eyerok is REACHABLE in normal play from level 17's walk. See the
//    lane's report: registration and a forced spawn are what is proved.
// ============================================================================
#include <cstdio>
#include <cstdlib>

#include "types.h"
#include "Actor.h"
#include "ActorBase.h"
#include "Eyerok.h"

extern "C" {
/* ---- the shared arm9 defaults, slots 1..30 ---- */
int _ZN5Actor19BeforeInitResourcesEv(void *self);
void _ZN5Actor18AfterInitResourcesEj(void *self, unsigned a);
int _ZN5Actor14BeforeBehaviorEv(void *self);
int _ZN5Actor12BeforeRenderEv(void *self);
int _ZN5Actor13OnYoshiTryEatEv(void *self);
void _ZN5Actor13OnTurnIntoEggER6Player(void *self, void *p);
int _ZN5Actor9Virtual50Ev(void *self);
void _ZN5Actor15OnGroundPoundedERS_(void *self, void *o);
void _ZN5Actor11OnAttacked1ERS_(void *self, void *o);
void _ZN5Actor11OnAttacked2ERS_(void *self, void *o);
void _ZN5Actor8OnKickedERS_(void *self, void *o);
void _ZN5Actor8OnPushedERS_(void *self, void *o);
void _ZN5Actor24OnHitByCannonBlastedCharERS_(void *self, void *o);
void _ZN5Actor15OnHitByMegaCharER6Player(void *self, void *p);
void _ZN5Actor19OnHitFromUnderneathERS_(void *self, void *o);
void _ZN8Platform4KillEv(void *self);                    /* slot 31, 0x020ee55c */

extern int data_02099f24[];          /* the frame phase the lists are in */
extern unsigned char data_020a4b4c;  /* the spawn spine's own step */
const char *port_actor_class_name(unsigned id);  /* hal/actor_registry */
void port_actor_slot_decline(const char *what);  /* func_02043fdc_hostcopy.cpp */
void port_actor_render_probe(const char *cls, void *model);

/* ---- Eyerok's own bodies ------------------------------------------------
   Slots 0, 3 and 9 are REAL MSVC METHODS against include/Eyerok.h
   (?InitResources@Eyerok@@..., not __ZN6Eyerok...), so the thunks below reach
   them qualified through the class rather than by an Itanium C name. NOTHING
   ELSE IN THE TREE calls them under the Itanium spelling -- checked across
   src/ and include/ -- so no faces are needed, unlike the ov030 RollingLogTtm
   pair. Slot 9's body is the HOST COPY in port/unmatched/Eyerok_Render.cpp,
   which defines the same method.
   Slot 6 is the HOST COPY in port/unmatched/Eyerok_Behavior.cpp, exported
   under the Itanium C name the matched src used. */
int _ZN6Eyerok8BehaviorEv(char *self);                /* slot 6, HOST COPY */
void _ZN6Eyerok16OnPendingDestroyEv(void);            /* slot 12, .c body, void */
void *_ZN6EyerokD1Ev(void *self);                     /* slot 16 */
void *_ZN6EyerokD0Ev(void *self);                     /* slot 17 */
int func_ov066_0211a2dc(void);                        /* slot 29, own, T5-ruled */

void *Eyerok_Spawn(void);
extern unsigned char Eyerok_SpawnInfo[];

/* ---- the host vtable array. A mounted vtable would hand the factory DS code
        addresses, so its span is excluded from port/ov066_syms.txt and lives
        here (the ov015/ov016/ov022/ov045/ov077/ov080/ov030 rule).
        `int`, not `void *`, because include/decl_common.h:732 already declares
        `extern int _ZTV6Eyerok[];` and src/_ZN6EyerokD1Ev.c includes it. ---- */
int _ZTV6Eyerok[32];    /* vtspan: 0x0211ad64, 32 slots */

/* ---- the nineteen state records, as the seat sees them ---- */
struct PortEyerokCell { unsigned enter_fn, enter_delta, tick_fn, tick_delta; };

/* the thirty-eight ov066 bodies the nineteen cells dispatch */
int func_ov066_02116c6c(void *c);
int func_ov066_02116d14(void *c);
int func_ov066_02116db0(void *c);
int func_ov066_02117190(void *c);
int func_ov066_021171b0(void *c);
int func_ov066_021175bc(void *c);
int func_ov066_021175e8(void *c);
int func_ov066_02117bd0(void *c);
int func_ov066_02117bf0(void *c);
int func_ov066_02118168(void *c);
int func_ov066_02118188(void *c);
int func_ov066_021184c0(void *c);
int func_ov066_021184e0(void *c);
int func_ov066_021185e4(void *c);
int func_ov066_02118604(void *c);
int func_ov066_02118658(void *c);
int func_ov066_02118678(void *c);
int func_ov066_021187c8(void *c);
int func_ov066_021188b0(void *c);
int func_ov066_02118934(void *c);
int func_ov066_02118954(void *c);
int func_ov066_021189a0(void *c);
int func_ov066_021189c0(void *c);
int func_ov066_02118a30(void *c);
int func_ov066_02118a50(void *c);
int func_ov066_02118b08(void *c);
int func_ov066_02118b28(void *c);
int func_ov066_02118be0(void *c);
int func_ov066_02118c00(void *c);
int func_ov066_02118cb8(void *c);
int func_ov066_02118cdc(void *c);
int func_ov066_02118de0(void *c);
int func_ov066_02118e04(void *c);
int func_ov066_0211901c(void *c);
int func_ov066_0211903c(void *c);
int func_ov066_02119348(void *c);
int func_ov066_02119398(void *c);   /* HOST COPY, Eyerok_ClosestPlayerRead.cpp */
int func_ov066_0211944c(void *c);

/* the nineteen .bss records the mount provides */
extern PortEyerokCell data_ov066_0211afcc;
extern PortEyerokCell data_ov066_0211afdc;
extern PortEyerokCell data_ov066_0211afec;
extern PortEyerokCell data_ov066_0211affc;
extern PortEyerokCell data_ov066_0211b00c;
extern PortEyerokCell data_ov066_0211b01c;
extern PortEyerokCell data_ov066_0211b02c;
extern PortEyerokCell data_ov066_0211b03c;
extern PortEyerokCell data_ov066_0211b04c;
extern PortEyerokCell data_ov066_0211b05c;
extern PortEyerokCell data_ov066_0211b06c;
extern PortEyerokCell data_ov066_0211b07c;
extern PortEyerokCell data_ov066_0211b08c;
extern PortEyerokCell data_ov066_0211b09c;
extern PortEyerokCell data_ov066_0211b0ac;
extern PortEyerokCell data_ov066_0211b0bc;
extern PortEyerokCell data_ov066_0211b0cc;
extern PortEyerokCell data_ov066_0211b0dc;
extern PortEyerokCell data_ov066_0211b0ec;
}

/* ---- MSVC-typed spellings of mounted C storage and of the vtable ----------
   src/Eyerok_Spawn.cpp declares `extern void *_ZTV6Eyerok[];` OUTSIDE
   extern "C" -- the ov063 MadPiano shape -- so MSVC decorates the reference
   while this file defines the one real C symbol. The LHS is a decorated
   spelling NOTHING in this link defines, which is what keeps
   port/tools/alternatename_guard.py's rule satisfied by construction (an
   /alternatename whose LHS is also DEFINED is defeated silently). Data only;
   a data alias has no this-register contract to break. */
#pragma comment(linker, "/alternatename:?_ZTV6Eyerok@@3PAPAXA=__ZTV6Eyerok")

/* ---- SIXTY-NINE MSVC-DECORATED REFERENCES ONTO NAMES THE LINK ALREADY HAS -
   Four ov066 TUs are .cpp files that declare mounted storage and arm9 helpers
   with a TYPE, OUTSIDE extern "C", so MSVC decorates the reference while
   ovdata's emission and the arm9 slices carry the plain cdecl name. Each row
   below is a decorated reference onto storage that already exists, NOT a
   rename of anything: every LHS is a decorated spelling nothing in this link
   defines, which is what keeps port/tools/alternatename_guard.py's rule (an
   /alternatename whose LHS is also DEFINED is defeated silently) satisfied by
   construction -- the same rule the six directives THIS LANE HAD TO REMOVE
   broke the moment ov066 landed.

   MEASURED off the first link's own unresolved list with the mangling MSVC
   actually produced, never predicted from the declaration.

   THE ARITY OF EVERY FUNCTION ROW WAS CHECKED BEFORE THE BRIDGE, because a
   function alias CAN lose a receiver where a data alias cannot:
     TextureSequence::Prepare(BMD_File&, BTP_File&)  STATIC, two arguments --
       twelve other TUs across ov002/ov006/ov027/ov060 declare it identically
     TextureSequence::LoadFile / MeshCollider::LoadFile  STATIC, one argument
       (the w4-c LoadFile shape, already in alternatename_baseline's prose)
     Actor::Spawn  a STATIC factory, six arguments, no receiver
     Actor::TrackStar, ShadowModel::InitCylinder,
     MovingCylinderClsnWithPos::Init, MovingMeshCollider::SetFile,
     func_020393d4, func_020393c4  all pass their receiver EXPLICITLY as the
       first argument, so the cdecl body and the decorated reference agree
   Not one row here loses a `this`.

   The two data_ov025_* rows and the func_ov066_0211a35c row are ADDRESS-TAKEN
   ONLY -- a CLPS_Block pointer and a collider callback handed to
   func_020393c4 -- so the @@YAXXZ spelling never becomes a call through the
   alias. The ov046 arena-CLPS precedent (hal/actor_classes_ov060.cpp:201) is
   the same data-under-a-function-spelling shape. ---- */

/* src/func_ov066_021166c8.cpp -- InitResources' resource half. The three
      @@YAXXZ rows are the two ov025 CLPS blocks and ov066's own collider
      callback veneer, all three address-taken only. */
#pragma comment(linker, "/alternatename:?data_ov025_02112cc8@@YAXXZ=_data_ov025_02112cc8")
#pragma comment(linker, "/alternatename:?data_ov025_02112c88@@YAXXZ=_data_ov025_02112c88")
#pragma comment(linker, "/alternatename:?func_ov066_0211a35c@@YAXXZ=_func_ov066_0211a35c")
#pragma comment(linker, "/alternatename:?data_ov066_0211ae54@@3PAPAXA=_data_ov066_0211ae54")
#pragma comment(linker, "/alternatename:?data_ov066_0211ae2c@@3PAPAXA=_data_ov066_0211ae2c")
#pragma comment(linker, "/alternatename:?data_ov066_0211ae94@@3PAPAXA=_data_ov066_0211ae94")
#pragma comment(linker, "/alternatename:?data_ov066_0211ae9c@@3PAPAXA=_data_ov066_0211ae9c")
#pragma comment(linker, "/alternatename:?data_ov066_0211ae34@@3PAPAXA=_data_ov066_0211ae34")
#pragma comment(linker, "/alternatename:?data_ov066_0211ae1c@@3PAPAXA=_data_ov066_0211ae1c")

/* src/func_ov066_02116d14.cpp -- a state body that re-arms the blend anim. */
#pragma comment(linker, "/alternatename:?data_ov066_0211aea4@@3PAPAXA=_data_ov066_0211aea4")
#pragma comment(linker, "/alternatename:?data_ov066_0211ae8c@@3PAPAXA=_data_ov066_0211ae8c")

/* src/_ZN6Eyerok16CleanupResourcesEv.cpp -- twenty-two SharedFilePtr releases,
   each spelled `extern char data_ov066_*[]`. */
#pragma comment(linker, "/alternatename:?data_ov066_0211ae6c@@3PADA=_data_ov066_0211ae6c")
#pragma comment(linker, "/alternatename:?data_ov066_0211ae4c@@3PADA=_data_ov066_0211ae4c")
#pragma comment(linker, "/alternatename:?data_ov066_0211aeb4@@3PADA=_data_ov066_0211aeb4")
#pragma comment(linker, "/alternatename:?data_ov066_0211aebc@@3PADA=_data_ov066_0211aebc")
#pragma comment(linker, "/alternatename:?data_ov066_0211ae9c@@3PADA=_data_ov066_0211ae9c")
#pragma comment(linker, "/alternatename:?data_ov066_0211ae3c@@3PADA=_data_ov066_0211ae3c")
#pragma comment(linker, "/alternatename:?data_ov066_0211ae2c@@3PADA=_data_ov066_0211ae2c")
#pragma comment(linker, "/alternatename:?data_ov066_0211ae5c@@3PADA=_data_ov066_0211ae5c")
#pragma comment(linker, "/alternatename:?data_ov066_0211ae84@@3PADA=_data_ov066_0211ae84")
#pragma comment(linker, "/alternatename:?data_ov066_0211aea4@@3PADA=_data_ov066_0211aea4")
#pragma comment(linker, "/alternatename:?data_ov066_0211ae8c@@3PADA=_data_ov066_0211ae8c")
#pragma comment(linker, "/alternatename:?data_ov066_0211ae54@@3PADA=_data_ov066_0211ae54")
#pragma comment(linker, "/alternatename:?data_ov066_0211ae94@@3PADA=_data_ov066_0211ae94")
#pragma comment(linker, "/alternatename:?data_ov066_0211ae64@@3PADA=_data_ov066_0211ae64")
#pragma comment(linker, "/alternatename:?data_ov066_0211ae44@@3PADA=_data_ov066_0211ae44")
#pragma comment(linker, "/alternatename:?data_ov066_0211ae74@@3PADA=_data_ov066_0211ae74")
#pragma comment(linker, "/alternatename:?data_ov066_0211ae7c@@3PADA=_data_ov066_0211ae7c")
#pragma comment(linker, "/alternatename:?data_ov066_0211ae24@@3PADA=_data_ov066_0211ae24")
#pragma comment(linker, "/alternatename:?data_ov066_0211aeac@@3PADA=_data_ov066_0211aeac")
#pragma comment(linker, "/alternatename:?data_ov066_0211ae14@@3PADA=_data_ov066_0211ae14")
#pragma comment(linker, "/alternatename:?data_ov066_0211ae1c@@3PADA=_data_ov066_0211ae1c")
#pragma comment(linker, "/alternatename:?data_ov066_0211ae34@@3PADA=_data_ov066_0211ae34")

/* src/_ZN6Eyerok13InitResourcesEv.cpp -- the class's biggest body: the same
      twenty-two file pointers spelled `extern int data_ov066_*[]`, three
      scalars spelled `extern s8`, ten arm9 helpers and the collider
      callback, whose C name is already in the link from slice_gate59. */
#pragma comment(linker, "/alternatename:?_ZN5Actor9TrackStarEjj@@YAEPAXII@Z=__ZN5Actor9TrackStarEjj")
#pragma comment(linker, "/alternatename:?_ZN15TextureSequence8LoadFileER13SharedFilePtr@@YAXPAX@Z=__ZN15TextureSequence8LoadFileER13SharedFilePtr")
#pragma comment(linker, "/alternatename:?_ZN12MeshCollider8LoadFileER13SharedFilePtr@@YAXPAX@Z=__ZN12MeshCollider8LoadFileER13SharedFilePtr")
#pragma comment(linker, "/alternatename:?_ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File@@YAXHH@Z=__ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File")
#pragma comment(linker, "/alternatename:?_ZN11ShadowModel12InitCylinderEv@@YAXPAX@Z=__ZN11ShadowModel12InitCylinderEv")
#pragma comment(linker, "/alternatename:?_ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj@@YAXPAX0PAUVector3@@HHII@Z=__ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj")
#pragma comment(linker, "/alternatename:?_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii@@YAPAXIIPAUVector3@@PAXHH@Z=__ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii")
#pragma comment(linker, "/alternatename:?_ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block@@YAXPAXH0HF0@Z=__ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block")
#pragma comment(linker, "/alternatename:?func_020393d4@@YAXPAX0@Z=_func_020393d4")
#pragma comment(linker, "/alternatename:?func_020393c4@@YAXPAX0@Z=_func_020393c4")
#pragma comment(linker, "/alternatename:?data_ov066_0211ae6c@@3PAHA=_data_ov066_0211ae6c")
#pragma comment(linker, "/alternatename:?data_ov066_0211ae4c@@3PAHA=_data_ov066_0211ae4c")
#pragma comment(linker, "/alternatename:?data_ov066_0211aeb4@@3PAHA=_data_ov066_0211aeb4")
#pragma comment(linker, "/alternatename:?data_ov066_0211aebc@@3PAHA=_data_ov066_0211aebc")
#pragma comment(linker, "/alternatename:?data_ov066_0211ae9c@@3PAHA=_data_ov066_0211ae9c")
#pragma comment(linker, "/alternatename:?data_ov066_0211ae3c@@3PAHA=_data_ov066_0211ae3c")
#pragma comment(linker, "/alternatename:?data_ov066_0211ae2c@@3PAHA=_data_ov066_0211ae2c")
#pragma comment(linker, "/alternatename:?data_ov066_0211ae5c@@3PAHA=_data_ov066_0211ae5c")
#pragma comment(linker, "/alternatename:?data_ov066_0211ae84@@3PAHA=_data_ov066_0211ae84")
#pragma comment(linker, "/alternatename:?data_ov066_0211aea4@@3PAHA=_data_ov066_0211aea4")
#pragma comment(linker, "/alternatename:?data_ov066_0211ae8c@@3PAHA=_data_ov066_0211ae8c")
#pragma comment(linker, "/alternatename:?data_ov066_0211ae54@@3PAHA=_data_ov066_0211ae54")
#pragma comment(linker, "/alternatename:?data_ov066_0211ae94@@3PAHA=_data_ov066_0211ae94")
#pragma comment(linker, "/alternatename:?data_ov066_0211ae64@@3PAHA=_data_ov066_0211ae64")
#pragma comment(linker, "/alternatename:?data_ov066_0211ae44@@3PAHA=_data_ov066_0211ae44")
#pragma comment(linker, "/alternatename:?data_ov066_0211ae74@@3PAHA=_data_ov066_0211ae74")
#pragma comment(linker, "/alternatename:?data_ov066_0211ae7c@@3PAHA=_data_ov066_0211ae7c")
#pragma comment(linker, "/alternatename:?data_ov066_0211ae24@@3PAHA=_data_ov066_0211ae24")
#pragma comment(linker, "/alternatename:?data_ov066_0211aeac@@3PAHA=_data_ov066_0211aeac")
#pragma comment(linker, "/alternatename:?data_ov066_0211ae14@@3PAHA=_data_ov066_0211ae14")
#pragma comment(linker, "/alternatename:?data_ov066_0211ae1c@@3PAHA=_data_ov066_0211ae1c")
#pragma comment(linker, "/alternatename:?data_ov066_0211ae34@@3PAHA=_data_ov066_0211ae34")
#pragma comment(linker, "/alternatename:?data_ov066_0211abe0@@3CA=_data_ov066_0211abe0")
#pragma comment(linker, "/alternatename:?data_ov066_0211ae04@@3CA=_data_ov066_0211ae04")
#pragma comment(linker, "/alternatename:?data_ov066_0211ae0c@@3CA=_data_ov066_0211ae0c")
#pragma comment(linker, "/alternatename:?_ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_@@3DA=__ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_")

// ============================================================================
// THE TRAPS
// ============================================================================
// Slots 13/14 are ActorBase::Virtual34/Virtual38 and slot 30 is
// Actor::OnAimedAtWithEggReturnVec. None of the three can be serviced by a
// __fastcall(void*, void*) thunk: 13/14 take two u32 arguments each and their
// bodies are not linked into the port at all, and 30 is an SRET method whose
// hidden return pointer no thunk shape here models. The ccm/ov064/jrb/bbh/
// ov030/ov036 convention: trap them by name rather than seat a body with the
// wrong contract. Do not read "its body is not linked" as "the slot is not
// reachable" -- dispatch is indirect through the table.
static void ov66_trap_report(void *self, int slot, const char *what)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: %s is not hosted (actor id %u %s, phase %d, "
                 "spawn step %d)\n",
                 what, id, port_actor_class_name(id), data_02099f24[0],
                 (int)data_020a4b4c);
    { static char _m[160];
      std::snprintf(_m, sizeof _m, "unhosted %s on id %u %s",
                    what, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
    (void)slot;
}
static int __fastcall ov66_trap13(void *s, void *)
{ ov66_trap_report(s, 13, "vtable slot 13 ActorBase::Virtual34(u32,u32)"); return 0; }
static int __fastcall ov66_trap14(void *s, void *)
{ ov66_trap_report(s, 14, "vtable slot 14 ActorBase::Virtual38(u32,u32)"); return 0; }
static int __fastcall ov66_trap30(void *s, void *)
{ ov66_trap_report(s, 30, "vtable slot 30 Actor::OnAimedAtWithEggReturnVec (SRET)"); return 0; }

// ============================================================================
// THE SHARED 1..30 HALF
// ============================================================================
static int __fastcall ov66_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall ov66_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall ov66_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall ov66_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall ov66_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall ov66_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall ov66_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall ov66_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall ov66_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall ov66_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
static int __fastcall ov66_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall ov66_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall ov66_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall ov66_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall ov66_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall ov66_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall ov66_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall ov66_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall ov66_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall ov66_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall ov66_kill(void *s, void *)
{ _ZN8Platform4KillEv(s); return 0; }

// ============================================================================
// EYEROK'S OWN SLOTS
// ============================================================================
static int __fastcall ov66_init(void *s, void *)
{ return ((Eyerok *)s)->Eyerok::InitResources(); }
static int __fastcall ov66_clean(void *s, void *)
{ return ((Eyerok *)s)->Eyerok::CleanupResources(); }
static int __fastcall ov66_behavior(void *s, void *)
{ return _ZN6Eyerok8BehaviorEv((char *)s); }
static int __fastcall ov66_render(void *s, void *)
{ port_actor_render_probe("EYEROK", (char *)s + 0x3d0);
  return ((Eyerok *)s)->Eyerok::Render(); }
/* Slot 12 is Eyerok's OWN OnPendingDestroy, not ActorBase's default: the ROM
   word is 0x021197a0, an empty four-byte body inside this overlay, and the .c
   takes void. */
static int __fastcall ov66_pdes(void *s, void *)
{ (void)s; _ZN6Eyerok16OnPendingDestroyEv(); return 0; }
static int __fastcall ov66_d1(void *s, void *)
{ return (int)(size_t)_ZN6EyerokD1Ev(s); }
static int __fastcall ov66_d0(void *s, void *)
{ return (int)(size_t)_ZN6EyerokD0Ev(s); }
/* Slot 29 is Eyerok's OWN OnAimedAtWithEgg, overriding the shared arm9
   0x02010158. Its body takes void and returns the constant 0x28000. */
static int __fastcall ov66_aimed(void *s, void *)
{ (void)s; return func_ov066_0211a2dc(); }

// ============================================================================
// THE NINETEEN-CELL STATE SEAT
// ============================================================================
/* {the record, the ROM addresses the sinit's own source pairs carry, the host
   bodies}. Order is __sinit_ov066_0211a418.c's OWN assignment order, not
   sorted by address, and the ROM column is verified against the mounted bytes
   before the rewrite -- the Ukiki/SoundObject/Cap/MrBlizzard/BabyPenguin
   shape: a mount pointing at the wrong bytes ABORTS instead of silently
   calling into garbage. */
typedef int (*PortEyerokFn)(void *);

static const struct {
    PortEyerokCell *cell;
    unsigned enter_rom, tick_rom;
    PortEyerokFn enter_host, tick_host;
} g_eyerok_cells[19] = {
    { &data_ov066_0211b09c, 0x0211944c, 0x02119398, func_ov066_0211944c, func_ov066_02119398 },
    { &data_ov066_0211b0ac, 0x02119348, 0x0211903c, func_ov066_02119348, func_ov066_0211903c },
    { &data_ov066_0211b0cc, 0x0211901c, 0x02118e04, func_ov066_0211901c, func_ov066_02118e04 },
    { &data_ov066_0211b0dc, 0x02118de0, 0x02118cdc, func_ov066_02118de0, func_ov066_02118cdc },
    { &data_ov066_0211afcc, 0x02118cb8, 0x02118c00, func_ov066_02118cb8, func_ov066_02118c00 },
    { &data_ov066_0211afdc, 0x02118be0, 0x02118b28, func_ov066_02118be0, func_ov066_02118b28 },
    { &data_ov066_0211affc, 0x02118b08, 0x02118a50, func_ov066_02118b08, func_ov066_02118a50 },
    { &data_ov066_0211b00c, 0x02118a30, 0x021189c0, func_ov066_02118a30, func_ov066_021189c0 },
    { &data_ov066_0211b02c, 0x021189a0, 0x02118954, func_ov066_021189a0, func_ov066_02118954 },
    { &data_ov066_0211b03c, 0x02118934, 0x021188b0, func_ov066_02118934, func_ov066_021188b0 },
    { &data_ov066_0211b05c, 0x021187c8, 0x02118678, func_ov066_021187c8, func_ov066_02118678 },
    { &data_ov066_0211b06c, 0x02118658, 0x02118604, func_ov066_02118658, func_ov066_02118604 },
    { &data_ov066_0211b08c, 0x021185e4, 0x021184e0, func_ov066_021185e4, func_ov066_021184e0 },
    { &data_ov066_0211b0bc, 0x021184c0, 0x02118188, func_ov066_021184c0, func_ov066_02118188 },
    { &data_ov066_0211b0ec, 0x02118168, 0x02117bf0, func_ov066_02118168, func_ov066_02117bf0 },
    { &data_ov066_0211afec, 0x02117bd0, 0x021175e8, func_ov066_02117bd0, func_ov066_021175e8 },
    { &data_ov066_0211b01c, 0x021175bc, 0x021171b0, func_ov066_021175bc, func_ov066_021171b0 },
    { &data_ov066_0211b04c, 0x02117190, 0x02116db0, func_ov066_02117190, func_ov066_02116db0 },
    { &data_ov066_0211b07c, 0x02116d14, 0x02116c6c, func_ov066_02116d14, func_ov066_02116c6c },
};

extern "C" void port_eyerok_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (int i = 0; i < 19; ++i) {
        PortEyerokCell *cell = g_eyerok_cells[i].cell;
        if (cell->enter_fn != g_eyerok_cells[i].enter_rom || cell->enter_delta != 0 ||
            cell->tick_fn != g_eyerok_cells[i].tick_rom || cell->tick_delta != 0) {
            std::fprintf(stderr, "FATAL: Eyerok state cell %d: the sinit left "
                         "%08x/%u %08x/%u, the ROM's own records say "
                         "%08x/0 %08x/0 -- WRONG BYTES\n", i,
                         cell->enter_fn, cell->enter_delta, cell->tick_fn, cell->tick_delta,
                         g_eyerok_cells[i].enter_rom, g_eyerok_cells[i].tick_rom);
            std::abort();
        }
        cell->enter_fn = (unsigned)(size_t)g_eyerok_cells[i].enter_host;
        cell->tick_fn = (unsigned)(size_t)g_eyerok_cells[i].tick_host;
    }
}

// ============================================================================
// THE FILL
// ============================================================================
extern "C" void hal_fill_eyerok_vtable(void)
{
    /* Seat and verify the nineteen PMF cells BEFORE anything can dispatch
       through them: Eyerok::InitResources reaches func_ov066_02119454 on the
       first frame, and that body installs a cell and enters it in the same
       three statements. */
    port_eyerok_states_seat();

    void **vt = (void **)_ZTV6Eyerok;
    vt[0]  = (void *)ov66_init;      /* Eyerok::InitResources */
    vt[1]  = (void *)ov66_binit;     /* Actor::BeforeInitResources */
    vt[2]  = (void *)ov66_ainit;     /* Actor::AfterInitResources(u32) */
    vt[3]  = (void *)ov66_clean;     /* Eyerok::CleanupResources */
    vt[4]  = (void *)ov66_bclean;    /* Actor::BeforeCleanupResources */
    vt[5]  = (void *)ov66_aclean;    /* Actor::AfterCleanupResources(u32) */
    vt[6]  = (void *)ov66_behavior;  /* Eyerok::Behavior, HOST COPY */
    vt[7]  = (void *)ov66_bbeh;      /* Actor::BeforeBehavior */
    vt[8]  = (void *)ov66_abeh;      /* Actor::AfterBehavior(u32) */
    vt[9]  = (void *)ov66_render;    /* Eyerok::Render, HOST COPY */
    vt[10] = (void *)ov66_bren;      /* Actor::BeforeRender */
    vt[11] = (void *)ov66_aren;      /* Actor::AfterRender(u32) */
    vt[12] = (void *)ov66_pdes;      /* own OnPendingDestroy, empty body */
    vt[13] = (void *)ov66_trap13;    /* ActorBase::Virtual34(u32,u32), not linked */
    vt[14] = (void *)ov66_trap14;    /* ActorBase::Virtual38(u32,u32), not linked */
    vt[15] = (void *)ov66_heap;      /* ActorBase::OnHeapCreated */
    vt[16] = (void *)ov66_d1;        /* D1 */
    vt[17] = (void *)ov66_d0;        /* D0 */
    vt[18] = (void *)ov66_yoshi;     /* Actor::OnYoshiTryEat */
    vt[19] = (void *)ov66_egg;       /* Actor::OnTurnIntoEgg(Player&) */
    vt[20] = (void *)ov66_v50;       /* Actor::Virtual50 */
    vt[21] = (void *)ov66_pounded;   /* Actor::OnGroundPounded(Actor&) */
    vt[22] = (void *)ov66_atk1;      /* Actor::OnAttacked1(Actor&) */
    vt[23] = (void *)ov66_atk2;      /* Actor::OnAttacked2(Actor&) */
    vt[24] = (void *)ov66_kicked;    /* Actor::OnKicked(Actor&) */
    vt[25] = (void *)ov66_pushed;    /* Actor::OnPushed(Actor&) */
    vt[26] = (void *)ov66_cannon;    /* Actor::OnHitByCannonBlastedChar(Actor&) */
    vt[27] = (void *)ov66_mega;      /* Actor::OnHitByMegaChar(Player&) */
    vt[28] = (void *)ov66_under;     /* Actor::OnHitFromUnderneath(Actor&) */
    vt[29] = (void *)ov66_aimed;     /* OWN OnAimedAtWithEgg, T5-ruled body */
    vt[30] = (void *)ov66_trap30;    /* Actor::OnAimedAtWithEggReturnVec, SRET */
    vt[31] = (void *)ov66_kill;      /* Platform::Kill, the 32nd slot */
}
