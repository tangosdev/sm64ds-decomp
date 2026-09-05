// run rel0215 wave 1, lane cast-ov036 (W1-D): ov036's RAINBOW CRUISE cast --
// level 28's own seven classes, fresh per-symbol mount (port/ov036_syms.txt),
// the ov072/ov094/ov012/ov013 dual-mount convention. Slice is
// port/slice_ov036cast.txt.
//
// ============================================================================
// THE PAIRING IS BY ADDRESS, NOT BY NAME. READ port/ov036_syms.txt FIRST.
// ============================================================================
// config/arm9/overlays/ov036/symbols.txt attaches its C++ method and _ZTV
// names ONE CLASS EARLY. The Spawn and SpawnInfo names are correct; the
// _ZN.../_ZTV... names are not. Measured three ways in ov036_syms.txt: the
// block layout, block 2's own slots 16/17, and every table's own vtable[-1]
// typeinfo name string. On SIX of the seven that name string is cross-checked
// against the SpawnInfo's word[8] as a fourth, independent read -- but NOT on
// id 123, whose record is seven words, not nine: the word at its SpawnInfo
// +0x20 belongs to the neighbouring data_ov036_02113e88 file record and reads
// one of TRICKY_TRIANGLES' own SharedFilePtrs. 123 is settled by block
// containment instead (its Spawn's own literal pool stores 0x02113ecc, whose
// vtable[-1] names 16daObjRc_Dorifu_c). See ov036_syms.txt's THIRD ROUTE
// section for the per-id table.
//
//   id   registry name            ROM RTTI name          host vtable array
//   127  SWINGING_PLATFORM        16daObjRcBuranko_c      data_ov036_02113a98         (0x02113a98, 32)
//   129  ROTATING_PLATFORM_RR     19daObjRc_Kaitendai_c   data_ov036_02113b74         (0x02113b74, 32)
//   125  SHIP_WING                14daObjRc_Hane_c        _ZTV18RotatingPlatformRr    (0x02113c38, 31)
//   126  DONUT_BLOCK              16daObjRc_Tikuwa_c      _ZTV8ShipWing               (0x02113cf8, 32)
//   132  ARMED_ROTATING_PLATFORM  18daObjRc_Guruguru_c    _ZTV10DonutBlock            (0x02113dcc, 32)
//   123  TRICKY_TRIANGLES         16daObjRc_Dorifu_c      _ZTV21ArmedRotatingPlatform (0x02113ecc, 32)
//   130  FLYING_CARPET            15daObjRcCarpet_c       data_ov036_02113f9c         (0x02113f9c, 33)
//
// So hal_fill_ship_wing_vtable fills _ZTV18RotatingPlatformRr and seats the
// bodies named _ZN18RotatingPlatformRr*, and that is CORRECT: both the array
// name and the body names belong to the class the ROM calls daObjRc_Hane_c.
// Pairing SHIP_WING with _ZTV8ShipWing would hand id 125 DonutBlock's table.
//
// ============================================================================
// THE SLOT MAP, out of config/arm9/overlays/ov036/relocs.txt with every target
// resolved through config/arm9/symbols.txt, cross-read against the raw bytes
// of extracted/overlays/overlay_0036.bin (never dsd's copy, trap T4).
// ============================================================================
//
// slot  127 Buranko    129 Kaitendai  125 Hane      126 Tikuwa    132 Guruguru  123 Dorifu    130 Carpet    what the ROM names
//   0   0211137c own   0211150c own   02111854 own  02111bb8 own  02111eb0 own  021120b4 own  0211244c own  InitResources
//   1   02011268       02011268       02011268      02011268      02011268      02011268      02011268      Actor::BeforeInitResources
//   2   02011244       02011244       02011244      02011244      02011244      02011244      02011244      Actor::AfterInitResources(u32)
//   3   02111284 own   021114f8 own   021115f4 own  021119e8 own  02111db0 own  021120a0 own  02112318 own  CleanupResources
//   4   02011220       02011220       02011220      02011220      02011220      02011220      02011220      Actor::BeforeCleanupResources
//   5   02011214       02011214       02011214      02011214      02011214      02011214      02011214      Actor::AfterCleanupResources(u32)
//   6   021112f0 own   020b6718 ov002 021116c0 own  02111a64 own  02111e20 own  020b4bfc ov002 021123c8 own Behavior
//   7   02010fd4       02010fd4       02010fd4      02010fd4      02010fd4      02010fd4      02010fd4      Actor::BeforeBehavior
//   8   02010fc8       02010fc8       02010fc8      02010fc8      02010fc8      02010fc8      02010fc8      Actor::AfterBehavior(u32)
//   9   021112c8 own   020b66f0 ov002 0211169c own  02111a30 own  02111df8 own  020b4bc4 ov002 02112378 own Render
//  10   02010f78       02010f78       02010f78      02010f78      02010f78      02010f78      02010f78      Actor::BeforeRender
//  11   02010f6c       02010f6c       02010f6c      02010f6c      02010f6c      02010f6c      02010f6c      Actor::AfterRender(u32)
//  12   02043ac0       02043ac0       02043ac0      02111a2c own  02043ac0      02043ac0      02043ac0      OnPendingDestroy (only 126 overrides)
//  13   0204357c       0204357c       0204357c      0204357c      0204357c      0204357c      0204357c      ActorBase::Virtual34(u32,u32)
//  14   0204349c       0204349c       0204349c      0204349c      0204349c      0204349c      0204349c      ActorBase::Virtual38(u32,u32)
//  15   02043494       02043494       02043494      02043494      02043494      02043494      02043494      ActorBase::OnHeapCreated
//  16   021111a0 own   02111444 own   02111580 own  0211193c own  02111d14 own  02111f8c own  02112158 own  D1
//  17   021111e4 own   02111494 own   021115b0 own  02111988 own  02111d58 own  0211200c own  021121c8 own  D0
// 18..30 the arm9 shared half, byte-identical on all seven (02010160 OnYoshiTryEat
//        .. 020100dc OnAimedAtWithEggReturnVec). NOT ONE of the seven overrides
//        anything in 18..30 -- checked word by word against the raw image.
//  31   020ee55c       020ee55c       --            020ee55c      020ee55c      020ee55c      020ee55c      Platform::Kill (125 is a plain Actor, 31 slots)
//  32   --             --             --            --            --            --            020eff18      PathLift::AfterClsn (130 only, 33 slots)
//
// ============================================================================
// TRAP T1: THESE TABLES STAY ROM-SHAPED.
// ============================================================================
// None of the seven derives from Model or ModelAnim -- six derive from
// Platform (dBgActor_c) and one from Actor -- so the default applies and the
// arrays are indexed exactly as the ROM indexes them, D1 at 16 and D0 at 17.
// The Model/ModelAnim exception in hal/cxxname_bridge.cpp does not reach here.
// FLYING_CARPET's MEMBER at +0x450 is a ModelAnim, which is a different
// question and is handled in port/unmatched/FlyingCarpet_Render.cpp.
//
// ============================================================================
// TWO ov002 INTERMEDIATE BASE TABLES ARE HOST STORAGE HERE.
// ============================================================================
// ROTATING_PLATFORM_RR derives daObjKaitendai_c (ov002 0x021091d4) and
// TRICKY_TRIANGLES derives daObjDorifu_c (ov002 0x02108d94). Neither is in
// port/ov002_syms.txt and neither is defined anywhere else in the port, so
// both are declared as host arrays below. Each is an ABSTRACT intermediate --
// the ROM's own words at slots 0 and 3 are ZERO -- and each is installed only
// BETWEEN two member teardowns (or, in the constructor, one instruction before
// the derived table overwrites it), never dispatched through. They get the
// shared half and traps for the rest, the hal_fill_platform_vtable reading
// exactly: if a future class does dispatch, it says which slot.
// dPathLiftActor_c (ov002 0x0210af70), FLYING_CARPET's own intermediate, is
// deliberately NOT in port/ov002_syms.txt (that file says so on its own line
// 251) -- hal/actor_classes_ov100pl.cpp hosts and fills it. This file reaches
// that fill through ov100pl's own exported entry point rather than declaring
// or filling the array a second time.
//
// ============================================================================
// TRAP T5: eight ov036 bodies carry the "recovered from vtable slot identity"
// marker. ALL EIGHT ARE REAL DECOMP AND FIVE OF THEM ARE SEATED HERE.
// ============================================================================
// func_ov036_021111e4, _02111284, _021112c8, _021112f0, _0211137c (all
// SWINGING_PLATFORM) and _02111494, _021114f8, _0211150c (all
// ROTATING_PLATFORM_RR). Adjudicated per body by disassembling the ROM out of
// extracted/overlays/overlay_0036.bin and reading the C against it, which is
// what the marker's overloading requires:
//
//   021111e4  the marker calls it "OnYoshiTryEat" and it is the D0 -- the NAME
//             was guessed from a vtable slot and guessed WRONG (the D1/D0 fold
//             is exactly the mis-read). The BODY is the ROM instruction for
//             instruction: two vptr stores (0x02113a98 then 0x0210ae38),
//             MovingMeshCollider::~ at +0x124, Model::~ at +0xd4, Actor::~D2,
//             Memory::Deallocate with the heap at 0x020a0eac, return this.
//   02111284  CleanupResources: MeshColliderBase::IsEnabled/Disable at +0x124
//             then two SharedFilePtr::Release on 0x02114028 and 0x02114020.
//             Name right, body real.
//   021112c8  Render: `ldr r2,[r0,#0xd4]!; ldr r2,[r2,#0x14]; blx r2` with
//             r1 = 0, which is the six-virtual shadow the C spells.
//   021112f0  Behavior: the +0x31e step, the +0x90 accumulate, the
//             func_ov036_0211123c call and the IsClsnInRange/UpdateClsnPosAndRot
//             pair. Every offset in the ROM listing.
//   0211137c  InitResources: Model::LoadFile/ModelBase::SetFile,
//             MeshCollider::LoadFile/MovingMeshCollider::SetFile,
//             func_020393d4 with the UpdatePosWithTransform callback.
//   02111494  the D0 twin of 021111e4, three vptr stores (own, ov002 0x021091d4,
//             _ZTV8Platform) and the same teardown chain.
//   021114f8  a two-instruction ARM VENEER: ldr ip / ldr r1 / bx ip into
//             func_ov002_020b66a8 with data_ov036_02113b2c in r1. The C spells
//             it as the two-argument call it is.
//   0211150c  the same veneer shape into func_ov002_020b676c with a third
//             argument selected from 02113b18 / 02113b1c on the +8 low byte.
//
// So this is T5's FIRST branch throughout: the NAME was recovered from a
// vtable slot, the BODY is real decompilation. Nothing here is a behavioural
// guess and nothing is de-seated.
//
// The disassembly is not the only read. All eight were also put through the
// repo's own byte gate -- tools/match.py at the canonical 2004/b56 with
// --strict-relocs on, so every relocation SLOT'S DESTINATION is checked
// against config/arm9/overlays/ov036/relocs.txt rather than wildcarded -- and
// all eight report MATCHING. A body that byte-reproduces the ROM at the
// canonical compiler with every reloc destination checked is not a guess.
// The eight are recorded in port/tools/inferred_stub_adjudicated.txt, which is
// the guard's OWN route for a pre-seat ROM ruling and is a tightening rather
// than a loophole: the guard refuses a line that carries no ruling and no
// evidence reference. NOTHING was added to inferred_stub_baseline.txt.
// Each of the eight still OWES a decomp-side marker correction, and its
// adjudicated entry retires when the marker line goes. This lane does not edit
// src/, so that correction is owed and not done here.
//
// ============================================================================
// SHIP_WING's D1 IS NOT COMPILED.
// ============================================================================
// src/_ZN18RotatingPlatformRrD1Ev.cpp is a real MSVC-synthesised destructor
// over a LOCAL shadow class (`struct Actor { char pad[0xd0]; virtual ~Actor();
// }` plus a CommonModel member), so it defines ??1RotatingPlatformRr@@UAE@XZ
// and provides no _ZN18RotatingPlatformRrD1Ev at all -- the MotherPenguin /
// OneUpLogo / BabyPenguin shape (gate-31 PeachPainting recipe). The chain is
// spelled directly as sw_d1 below, HIGH ADDRESS FIRST, from the ROM's own
// listing at 0x02111580: store the own table, CommonModel::~CommonModel at
// +0xd4, Actor's own D2, return this. The matched src stays byte-locked in
// src/ as proof and is dropped from port/slice_ov036cast.txt.
//
// ============================================================================
// FLYING_CARPET IS THE SECOND CUSTOMER FOR THE PathLift MACHINERY.
// ============================================================================
// hal/actor_classes_ov100pl.cpp's own header predicted it: "FLYING_CARPET 130
// IS a real derived class: own 33-slot table 0x02113f9c, slot 32 reloc-pinned
// to the same 0x020eff18, and its Behavior/Init call BaseBehavior/the base
// init. It is the second customer for everything this lane lands." Both halves
// are used unchanged here -- hal_fill_path_lift_base() seats the base table
// and the three-state PMF machine (port/unmatched/PathLift_StateDispatch.cpp),
// and slot 32 gets the same THREE-PARAMETER thunk shape that lane measured:
// the dispatch site pushes the colliding Actor and the callee pops it, so a
// two-parameter thunk would emit a bare `ret` and desync the caller.
#include <cstdio>
#include <cstdlib>

#include "dsstate_seg.h"
#include "dtor_faces_cpp.h"

#include "Actor.h"
#include "ActorBase.h"
#include "ArmedRotatingPlatform.h"
#include "DonutBlock.h"
#include "PathLift.h"
#include "RotatingPlatformRr.h"
#include "ShipWing.h"

/* hal/actor_slot30_seat.cpp -- the shared seat for vtable slot 30,
   Actor::OnAimedAtWithEggReturnVec. The ROM word in slot 30 of every vtable
   this file fills IS the arm9 base body 0x020100dc (checked against
   config/<module>/relocs.txt at vtable+30*4), and that body is now in the
   link from src/_ZN5Actor25OnAimedAtWithEggReturnVecEv.cpp on slice_gate50.
   The three-parameter __fastcall is the sret contract MSVC uses for a
   thiscall member returning a 12-byte struct: this in ecx, the hidden result
   pointer the one (callee-popped) stack argument. Same shape as whomp_s30. */
extern "C" void *__fastcall port_actor_s30_base(void *self, void *, void *out);

// ---- five decorated references onto mounted cells --------------------------
// Five ov036 TUs in this slice are .cpp files that declare a mounted symbol
// with a TYPE, outside extern "C" -- `extern SharedFilePtr data_ov036_02114028;`
// and friends -- so MSVC decorates the reference while ovdata's emission
// carries the plain cdecl name. Each row below is a decorated reference onto
// the mount, not a rename of anything: the LHS is a decorated spelling nothing
// in this link defines, which is what keeps alternatename_guard's rule (an
// /alternatename whose LHS is also DEFINED is defeated silently) satisfied by
// construction. Measured from the linker's own unresolved list, not guessed.
//   func_ov036_0211137c.cpp                      SwingingPlatform's two
//     ?data_ov036_02114028@@3USharedFilePtr@@A     SharedFilePtrs and the CLPS
//     ?data_ov036_02114020@@3USharedFilePtr@@A     block its collider takes
//     ?data_ov036_02112b68@@3UCLPS_Block@@A
//   _ZN10DonutBlock13InitResourcesEv.cpp         ArmedRotatingPlatform's own
//     ?data_ov036_02113d78@@3PAPAXA                three-word file record
//   _ZN21ArmedRotatingPlatform{13InitResources,   TrickyTriangles' own
//     16CleanupResources}Ev.cpp                     three-word file record
//     ?data_ov036_02113e88@@3UArg@@A
#pragma comment(linker, "/alternatename:?data_ov036_02114028@@3USharedFilePtr@@A=_data_ov036_02114028")
#pragma comment(linker, "/alternatename:?data_ov036_02114020@@3USharedFilePtr@@A=_data_ov036_02114020")
#pragma comment(linker, "/alternatename:?data_ov036_02112b68@@3UCLPS_Block@@A=_data_ov036_02112b68")
#pragma comment(linker, "/alternatename:?data_ov036_02113d78@@3PAPAXA=_data_ov036_02113d78")
#pragma comment(linker, "/alternatename:?data_ov036_02113e88@@3UArg@@A=_data_ov036_02113e88")

extern "C" {
/* the arm9 shared half; every address read off this overlay's own reloc runs */
int _ZN5Actor19BeforeInitResourcesEv(void *self);              /* slot 1  */
void _ZN5Actor18AfterInitResourcesEj(void *self, unsigned a);  /* slot 2  */
int _ZN5Actor14BeforeBehaviorEv(void *self);                   /* slot 7  */
int _ZN5Actor12BeforeRenderEv(void *self);                     /* slot 10 */
int _ZN5Actor13OnYoshiTryEatEv(void *self);                    /* slot 18 */
void _ZN5Actor13OnTurnIntoEggER6Player(void *self, void *p);   /* slot 19 */
int _ZN5Actor9Virtual50Ev(void *self);                         /* slot 20 */
void _ZN5Actor15OnGroundPoundedERS_(void *self, void *o);      /* slot 21 */
void _ZN5Actor11OnAttacked1ERS_(void *self, void *o);          /* slot 22 */
void _ZN5Actor11OnAttacked2ERS_(void *self, void *o);          /* slot 23 */
void _ZN5Actor8OnKickedERS_(void *self, void *o);              /* slot 24 */
void _ZN5Actor8OnPushedERS_(void *self, void *o);              /* slot 25 */
void _ZN5Actor24OnHitByCannonBlastedCharERS_(void *self, void *o); /* slot 26 */
void _ZN5Actor15OnHitByMegaCharER6Player(void *self, void *p); /* slot 27 */
void _ZN5Actor19OnHitFromUnderneathERS_(void *self, void *o);  /* slot 28 */
int _ZN5Actor16OnAimedAtWithEggEv(void *self);                 /* slot 29 */
void _ZN8Platform4KillEv(void *self);                          /* slot 31 */
int _ZN16MeshColliderBase9IsEnabledEv(void *self);
int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
void _ZN5ActorD2Ev(void *self);
void _ZN11CommonModelD1Ev(void *self);

extern int data_02099f24[];          /* the frame phase the lists are in */
extern unsigned char data_020a4b4c;  /* the spawn spine's own step */
const char *port_actor_class_name(unsigned id);   /* hal/actor_registry */
void port_actor_slot_decline(const char *what);   /* func_02043fdc_hostcopy */
void port_actor_render_probe(const char *cls, void *model);
/* ov100pl's own entry points. pl_bringup, which fills the shared
   dPathLiftActor_c base table at ov002 0x0210af70, is static in that file and
   is reached through its only exported caller; the state seat is exported
   directly and carries its own done guard. Both are idempotent. */
void hal_fill_path_lift_vtable(void);      /* hal/actor_classes_ov100pl.cpp */
void port_pathlift_states_seat(void);      /* port/unmatched/PathLift_StateDispatch.cpp */
}

// ---- two __thiscall faces onto @@YA bodies, the ov100pl Platform2/Platform3
// shape. src/_ZN10DonutBlock8BehaviorEv.cpp (which is ARMED_ROTATING_PLATFORM's
// Behavior -- the naming shift) declares its OWN local shadows
//     struct MeshColliderBase { bool IsEnabled(); void Disable(); };
//     struct Platform { ...; bool IsClsnInRange(Fix12, Fix12); ... };
// and both of those return BOOL, so MSVC decorates them
// ?IsEnabled@MeshColliderBase@@QAE_NXZ and ?IsClsnInRange@Platform@@QAE_NHH@Z
// -- different symbols from the int-returning pair hal/actor_classes_ov100pl.cpp
// already faces, which is why those two and only those two came back
// unresolved. NOT alias-eligible, for the reason that lane wrote down:
// hal/cxx_aliases.cpp:1059-1060 establishes both bodies as CDECL free
// functions taking `self` explicitly, so an /alternatename from a @@QAE LHS
// would hand the body a `this` that never was one. Their siblings on the same
// two shadows (Disable, UpdateModelPosAndRotY, UpdateClsnPosAndRot) already
// link and are deliberately not redefined here.
struct MeshColliderBase { bool IsEnabled(); };
struct Platform { bool IsClsnInRange(int a, int b); };
bool MeshColliderBase::IsEnabled()
{ return _ZN16MeshColliderBase9IsEnabledEv(this) != 0; }
bool Platform::IsClsnInRange(int a, int b)
{ return _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(this, a, b) != 0; }

// ---- the traps -------------------------------------------------------------
// Slots 13/14 are ActorBase::Virtual34/Virtual38 and slot 30 is
// Actor::OnAimedAtWithEggReturnVec. The ov072/ov100pl/ccm/jrb convention: none
// of the three can be serviced by a __fastcall(void*, void*) thunk -- 13/14
// take two u32 arguments each and their solid-heap bodies are not linked into
// the port at all, and 30 is an SRET method whose hidden return pointer no
// thunk shape here models. Trap them by name rather than seat a body with the
// wrong contract.
static void ov36_trap_report(void *self, int slot, const char *which)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: vtable slot %d (%s) is not hosted (actor id %u %s, "
                 "phase %d, spawn step %d)\n",
                 slot, which, id, port_actor_class_name(id), data_02099f24[0],
                 (int)data_020a4b4c);
    { static char _m[160];
      std::snprintf(_m, sizeof _m, "unhosted vtable slot %d (%s) on id %u %s",
                    slot, which, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
static int __fastcall ov36_trap13(void *s, void *)
{ ov36_trap_report(s, 13, "ActorBase::Virtual34"); return 0; }
static int __fastcall ov36_trap14(void *s, void *)
{ ov36_trap_report(s, 14, "ActorBase::Virtual38"); return 0; }
static int __fastcall ov36_trap30(void *s, void *)
{ ov36_trap_report(s, 30, "Actor::OnAimedAtWithEggReturnVec"); return 0; }
/* The abstract intermediates' own 0/3/6/9. The ROM parks ZERO in those words
   on both tables, so a dispatch through them was never legal on the DS
   either; this says which slot instead of calling address zero. */
static int __fastcall ov36_base_trap0(void *s, void *)
{ ov36_trap_report(s, 0, "abstract base InitResources"); return 0; }
static int __fastcall ov36_base_trap3(void *s, void *)
{ ov36_trap_report(s, 3, "abstract base CleanupResources"); return 0; }
static int __fastcall ov36_base_trap6(void *s, void *)
{ ov36_trap_report(s, 6, "abstract base Behavior"); return 0; }
static int __fastcall ov36_base_trap9(void *s, void *)
{ ov36_trap_report(s, 9, "abstract base Render"); return 0; }

// ---- the shared 1..31 half -------------------------------------------------
static int __fastcall ov36_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall ov36_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall ov36_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall ov36_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall ov36_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall ov36_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall ov36_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall ov36_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall ov36_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall ov36_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall ov36_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
/* slot 19 takes the three-parameter shape so it emits `ret 4`: the dispatch
   site pushes the Player the callee pops -- the wf/ov072/ov100pl contract. */
static int __fastcall ov36_turn_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall ov36_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall ov36_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall ov36_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall ov36_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall ov36_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall ov36_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall ov36_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall ov36_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall ov36_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall ov36_aimed(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }
static int __fastcall ov36_kill(void *s, void *)
{ _ZN8Platform4KillEv(s); return 0; }
/* slot 32, FLYING_CARPET only. THE THIRD PARAMETER IS THE POP, NOT A VALUE --
   the ov100pl lane measured this: the dispatch site is a thiscall over a
   33-virtual class that PUSHES the colliding Actor with no caller cleanup, so
   a two-parameter thunk emits a bare `ret`, and the caller's own `pop ebp`
   then takes the pushed Actor into EBP. Same word (0x020eff18), same shape. */
static int __fastcall ov36_after_clsn(void *s, void *, void * /*clsnActor*/)
{ ((PathLift *)s)->PathLift::AfterClsn(); return 0; }

/* Fills every slot the seven tables share: 1,2,4,5,7,8,10,11,12,13,14,15 and
   18..30. Each caller writes its own 0/3/6/9/16/17 afterwards (and 126 its own
   12, and the six Platform-derived their 31, and 130 its 32).

   SLOT NUMBERING IS THE ROM'S, taken from the seven reloc spans quoted in this
   file's header. Slots 18..30 are byte-identical across all seven tables and
   across ov072's and ov100pl's reviewed fills, which is what makes one shared
   fill legitimate; verified per table against the raw image, not assumed. */
static void ov36_fill_shared(void *volatile *vt)
{
    vt[1]  = (void *)ov36_binit;    /* Actor::BeforeInitResources */
    vt[2]  = (void *)ov36_ainit;    /* Actor::AfterInitResources(u32) */
    vt[4]  = (void *)ov36_bclean;   /* Actor::BeforeCleanupResources */
    vt[5]  = (void *)ov36_aclean;   /* Actor::AfterCleanupResources(u32) */
    vt[7]  = (void *)ov36_bbeh;     /* Actor::BeforeBehavior */
    vt[8]  = (void *)ov36_abeh;     /* Actor::AfterBehavior(u32) */
    vt[10] = (void *)ov36_bren;     /* Actor::BeforeRender */
    vt[11] = (void *)ov36_aren;     /* Actor::AfterRender(u32) */
    vt[12] = (void *)ov36_pdes;     /* ActorBase::OnPendingDestroy */
    vt[13] = (void *)ov36_trap13;   /* ActorBase::Virtual34(u32,u32), not linked */
    vt[14] = (void *)ov36_trap14;   /* ActorBase::Virtual38(u32,u32), not linked */
    vt[15] = (void *)ov36_heap;     /* ActorBase::OnHeapCreated */
    vt[18] = (void *)ov36_yoshi;    /* Actor::OnYoshiTryEat */
    vt[19] = (void *)ov36_turn_egg; /* Actor::OnTurnIntoEgg(Player&) */
    vt[20] = (void *)ov36_v50;      /* Actor::Virtual50 */
    vt[21] = (void *)ov36_pounded;  /* Actor::OnGroundPounded(Actor&) */
    vt[22] = (void *)ov36_atk1;     /* Actor::OnAttacked1(Actor&) */
    vt[23] = (void *)ov36_atk2;     /* Actor::OnAttacked2(Actor&) */
    vt[24] = (void *)ov36_kicked;   /* Actor::OnKicked(Actor&) */
    vt[25] = (void *)ov36_pushed;   /* Actor::OnPushed(Actor&) */
    vt[26] = (void *)ov36_cannon;   /* Actor::OnHitByCannonBlastedChar(Actor&) */
    vt[27] = (void *)ov36_mega;     /* Actor::OnHitByMegaChar(Player&) */
    vt[28] = (void *)ov36_under;    /* Actor::OnHitFromUnderneath(Actor&) */
    vt[29] = (void *)ov36_aimed;    /* Actor::OnAimedAtWithEgg */
    vt[30] = (void *)port_actor_s30_base;   /* Actor::OnAimedAtWithEggReturnVec, SRET */
}

// ============================================================================
// The two ov002 abstract intermediates, filled once.
// ============================================================================
// FOUR OF THE HOST ARRAYS CARRY DS-SHAPED NAMES, so they have to sit inside
// the .dsstate span a save state captures (hal/dsstate_seg.h; dsstate_guard
// fails the build otherwise, which is how these four were found rather than
// reasoned about). The three _ZTV*-named tables are not DS-shaped names and
// the guard does not ask for them. Grouped here rather than beside each class
// so one DSSTATE_BEGIN/END pair covers the set.
extern "C" {
DSSTATE_BEGIN
void *data_ov036_02113a98[32];  /* vtspan: data_ov036_02113a98, SWINGING_PLATFORM */
void *data_ov002_021091d4[32];  /* vtspan: daObjKaitendai_c, ov002 0x021091d4 */
void *data_ov002_02108d94[32];  /* vtspan: daObjDorifu_c, ov002 0x02108d94 */
int data_ov036_02113b74[32];   /* vtspan: data_ov036_02113b74, ROTATING_PLATFORM_RR */
int data_ov036_02113f9c[33];   /* vtspan: data_ov036_02113f9c, FLYING_CARPET */
DSSTATE_END
}

/* THE `void **tabs[2]` SHAPE IS LOAD-BEARING, not stylistic. Both tables are
   filled identically, so any spelling would run the same -- but
   port/tools/vtspan.py --fills resolves a fill to the table it writes by
   reading the binding, and it recognises exactly two shapes: one
   `void **vt = <named array>;` and this `void **tabs[N] = { A, B };` loop
   (hal_fill_platform_vtable uses the same one for the Platform/dBgActor pair).
   A helper taking the array as a parameter, or a cast in the initialiser list,
   reports as "table binding not modelled" -- a GAP IN THE CHECK, not a pass,
   which would leave these two 32-slot arrays unsized by the one tool that
   sizes them against the ROM. Measured: the parameterised spelling was tried
   first and vtspan refused to resolve it. The arrays are declared `void *`
   above for the same reason -- an `int` array needs a cast here, and a cast
   defeats MULTIBIND. */
static void ov36_base_bringup(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    void **tabs[2] = { data_ov002_021091d4, data_ov002_02108d94 };
    for (int k = 0; k < 2; ++k) {
        void *volatile *vt = (void *volatile *)tabs[k];
        ov36_fill_shared(vt);
        /* the ROM parks ZERO in slots 0 and 3 on both tables; 6 and 9 are the
           derived class's business on both customers too. */
        vt[0]  = (void *)ov36_base_trap0;
        vt[3]  = (void *)ov36_base_trap3;
        vt[6]  = (void *)ov36_base_trap6;
        vt[9]  = (void *)ov36_base_trap9;
        /* 16/17: an abstract intermediate installed only between two member
           teardowns never reaches its own destructors either. */
        vt[16] = (void *)ov36_base_trap0;
        vt[17] = (void *)ov36_base_trap0;
        vt[31] = (void *)ov36_kill;    /* both are Platform-derived */
    }
}

// ============================================================================
// SWINGING_PLATFORM (127) -- 16daObjRcBuranko_c, 0x02113a98, 32 slots,
// Platform-derived. Two on level 28. Own 0/3/6/9/16/17.
// ============================================================================
extern "C" {
int func_ov036_0211137c(char *self);       /* slot 0  */
int func_ov036_02111284(void *self);       /* slot 3  */
int func_ov036_021112f0(char *self);       /* slot 6  */
int func_ov036_021112c8(void *self);       /* slot 9, +0xd4 plain Model draw */
int *func_ov036_021111a0(int *self);       /* slot 16, D1 */
int *func_ov036_021111e4(int *self);       /* slot 17, D0 */
void *SwingingPlatform_Spawn(void);
/* data_ov036_02113a98[32] is declared in the DSSTATE block above. THE ROM NAME
   IS THE CANONICAL ONE HERE, not _ZTV16daObjRcBuranko_c, and that is not
   cosmetic: port/tools/vtspan.py --sweep sizes a host array against the ROM by
   looking its name up in config, and config names 0x02113a98 only
   data_ov036_02113a98 -- _ZTV16daObjRcBuranko_c is a host-only spelling that
   comes from include/decl_common.h. Declared under the host-only name, this
   32-slot array would not have been sized by the one tool that sizes them, the
   _ZTV13daObjWakame_c / _ZTV20daObjFl_Fall_Block_c shape. The two TUs that
   spell the host-only name get a per-source -D onto this one. */
}

static int __fastcall swp_init(void *s, void *)
{ return func_ov036_0211137c((char *)s); }
static int __fastcall swp_clean(void *s, void *)
{ return func_ov036_02111284(s); }
static int __fastcall swp_behavior(void *s, void *)
{ return func_ov036_021112f0((char *)s); }
static int __fastcall swp_render(void *s, void *)
{ port_actor_render_probe("SWINGING_PLATFORM", (char *)s + 0xd4);
  return func_ov036_021112c8(s); }
static int __fastcall swp_d1(void *s, void *)
{ return (int)(size_t)func_ov036_021111a0((int *)s); }
static int __fastcall swp_d0(void *s, void *)
{ return (int)(size_t)func_ov036_021111e4((int *)s); }

extern "C" void hal_fill_swinging_platform_vtable(void)
{
    void *volatile *vt = (void *volatile *)data_ov036_02113a98;
    ov36_fill_shared(vt);
    vt[0]  = (void *)swp_init;
    vt[3]  = (void *)swp_clean;
    vt[6]  = (void *)swp_behavior;
    vt[9]  = (void *)swp_render;
    vt[16] = (void *)swp_d1;
    vt[17] = (void *)swp_d0;
    vt[31] = (void *)ov36_kill;
}

// ============================================================================
// ROTATING_PLATFORM_RR (129) -- 19daObjRc_Kaitendai_c, 0x02113b74, 32 slots,
// daObjKaitendai_c <- dBgActor_c. Four on level 28. Own 0/3/16/17; slots 6 and
// 9 are ov002's shared bodies, which the ROM parks directly in the table.
// ============================================================================
extern "C" {
int func_ov036_0211150c(int *self);        /* slot 0, veneer into ov002 */
int func_ov036_021114f8(void *self);       /* slot 3, veneer into ov002 */
int func_ov002_020b6718(char *self);       /* slot 6, ov002 shared */
int func_ov002_020b66f0(void *self);       /* slot 9, ov002 shared */
int *func_ov036_02111444(int *self);       /* slot 16, D1 */
int *func_ov036_02111494(int *self);       /* slot 17, D0 */
void *RotatingPlatformRr_Spawn(void);
/* data_ov036_02113b74[32] is declared in the DSSTATE block above */
}

static int __fastcall rpr_init(void *s, void *)
{ return func_ov036_0211150c((int *)s); }
static int __fastcall rpr_clean(void *s, void *)
{ return func_ov036_021114f8(s); }
static int __fastcall rpr_behavior(void *s, void *)
{ return func_ov002_020b6718((char *)s); }
static int __fastcall rpr_render(void *s, void *)
{ port_actor_render_probe("ROTATING_PLATFORM_RR", (char *)s + 0xd4);
  return func_ov002_020b66f0(s); }
static int __fastcall rpr_d1(void *s, void *)
{ return (int)(size_t)func_ov036_02111444((int *)s); }
static int __fastcall rpr_d0(void *s, void *)
{ return (int)(size_t)func_ov036_02111494((int *)s); }

extern "C" void hal_fill_rotating_platform_rr_vtable(void)
{
    ov36_base_bringup();
    void *volatile *vt = (void *volatile *)data_ov036_02113b74;
    ov36_fill_shared(vt);
    vt[0]  = (void *)rpr_init;
    vt[3]  = (void *)rpr_clean;
    vt[6]  = (void *)rpr_behavior;
    vt[9]  = (void *)rpr_render;
    vt[16] = (void *)rpr_d1;
    vt[17] = (void *)rpr_d0;
    vt[31] = (void *)ov36_kill;
}

// ============================================================================
// SHIP_WING (125) -- 14daObjRc_Hane_c, 0x02113c38, THIRTY-ONE slots, the one
// plain Actor of the seven. Six on level 28. Own 0/3/6/9/16/17, no Kill.
// ============================================================================
extern "C" {
int _ZN18RotatingPlatformRr13InitResourcesEv(void *self);      /* slot 0, faced */
int _ZN18RotatingPlatformRr16CleanupResourcesEv(void);         /* slot 3, .c body takes void */
int _ZN18RotatingPlatformRr8BehaviorEv(void *self);            /* slot 6, faced */
int _ZN18RotatingPlatformRr6RenderEv(char *self);              /* slot 9, extern "C" already */
int *_ZN18RotatingPlatformRrD0Ev(int *self);                   /* slot 17, D0 */
void *ShipWing_Spawn(void);
int _ZTV18RotatingPlatformRr[31];  /* vtspan: _ZTV18RotatingPlatformRr */
}

static int __fastcall sw_init(void *s, void *)
{ return _ZN18RotatingPlatformRr13InitResourcesEv(s); }
static int __fastcall sw_clean(void *s, void *)
{ (void)s; return _ZN18RotatingPlatformRr16CleanupResourcesEv(); }
static int __fastcall sw_behavior(void *s, void *)
{ return _ZN18RotatingPlatformRr8BehaviorEv(s); }
/* NO port_actor_render_probe HERE, and that is deliberate. SHIP_WING's member
   at +0xd4 is a CommonModel (its Spawn constructs it with _ZN11CommonModelC1Ev
   and its Render calls CommonModel::Render), not a Model, and the shared probe
   reads Model's layout -- file at +0x0c, transforms at +0x14, the matrix at
   +0x1c. On a CommonModel (vtable +0x00, modelFile +0x04, data +0x08,
   mat4x3 +0x0c; include/CommonModel.h) those offsets land inside the matrix,
   so the probe printed `file 00001000 transforms 00000000` -- 0x1000 is
   mat4x3[0][0], the identity's own 1.0 in fix12, not a file pointer, and the
   null is a matrix cell, not a failed load. A diagnostic that reports a load
   problem where there is none is worse than no diagnostic, so this class does
   not take it. Its Init does load: the faced InitResources calls
   Model::LoadFile(data_ov036_02114070) then ModelBase::SetFile(+0xd4, ..., 1, -1). */
static int __fastcall sw_render(void *s, void *)
{ return _ZN18RotatingPlatformRr6RenderEv((char *)s); }
/* D1: the matched TU is an MSVC-synthesised destructor over a LOCAL shadow
   class and provides no C name (see this file's header). Chain spelled
   directly from the ROM's own listing at 0x02111580: the class's own vtable,
   then the CommonModel member at +0xd4, then Actor's D2. */
/* slot 16 is the matched src D1 through hal/dtor_faces_cpp.cpp (lane DTOR-FACES-CPP);
   the transcribed thunk that stood here (sw_d1) spelled the same chain by hand. */
static int __fastcall sw_d0(void *s, void *)
{ return (int)(size_t)_ZN18RotatingPlatformRrD0Ev((int *)s); }

extern "C" void hal_fill_ship_wing_vtable(void)
{
    void *volatile *vt = (void *volatile *)_ZTV18RotatingPlatformRr;
    ov36_fill_shared(vt);
    vt[0]  = (void *)sw_init;
    vt[3]  = (void *)sw_clean;
    vt[6]  = (void *)sw_behavior;
    vt[9]  = (void *)sw_render;
    vt[16] = (void *)hal_cppd1_RotatingPlatformRr;
    vt[17] = (void *)sw_d0;
    /* no slot 31: a plain Actor, 31 slots total, ends at 30 */
}

// ============================================================================
// DONUT_BLOCK (126) -- 16daObjRc_Tikuwa_c, 0x02113cf8, 32 slots,
// Platform-derived. THIRTY-ONE on level 28, the largest single id in the
// level's skip list. The only one of the seven with its own slot 12.
// ============================================================================
extern "C" {
int _ZN8ShipWing13InitResourcesEv(void *self);      /* slot 0,  faced */
int _ZN8ShipWing16CleanupResourcesEv(void *self);   /* slot 3,  faced */
int _ZN8ShipWing8BehaviorEv(void *self);            /* slot 6,  faced */
int _ZN8ShipWing6RenderEv(void *self);              /* slot 9,  faced */
void _ZN8ShipWing16OnPendingDestroyEv(void);        /* slot 12, own, .c body takes void */
int *_ZN8ShipWingD1Ev(int *self);                   /* slot 16, D1 */
int *_ZN8ShipWingD0Ev(int *self);                   /* slot 17, D0 */
void *DonutBlock_Spawn(void);
int _ZTV8ShipWing[32];  /* vtspan: _ZTV8ShipWing */
}

static int __fastcall db_init(void *s, void *)
{ return _ZN8ShipWing13InitResourcesEv(s); }
static int __fastcall db_clean(void *s, void *)
{ return _ZN8ShipWing16CleanupResourcesEv(s); }
static int __fastcall db_behavior(void *s, void *)
{ return _ZN8ShipWing8BehaviorEv(s); }
static int __fastcall db_render(void *s, void *)
{ port_actor_render_probe("DONUT_BLOCK", (char *)s + 0xd4);
  return _ZN8ShipWing6RenderEv(s); }
static int __fastcall db_pdes(void *s, void *)
{ (void)s; _ZN8ShipWing16OnPendingDestroyEv(); return 0; }
static int __fastcall db_d1(void *s, void *)
{ return (int)(size_t)_ZN8ShipWingD1Ev((int *)s); }
static int __fastcall db_d0(void *s, void *)
{ return (int)(size_t)_ZN8ShipWingD0Ev((int *)s); }

extern "C" void hal_fill_donut_block_vtable(void)
{
    void *volatile *vt = (void *volatile *)_ZTV8ShipWing;
    ov36_fill_shared(vt);
    vt[0]  = (void *)db_init;
    vt[3]  = (void *)db_clean;
    vt[6]  = (void *)db_behavior;
    vt[9]  = (void *)db_render;
    vt[12] = (void *)db_pdes;   /* own, overrides the shared ActorBase default */
    vt[16] = (void *)db_d1;
    vt[17] = (void *)db_d0;
    vt[31] = (void *)ov36_kill;
}

// ============================================================================
// ARMED_ROTATING_PLATFORM (132) -- 18daObjRc_Guruguru_c, 0x02113dcc, 32 slots,
// Platform-derived. One on level 28. Own 0/3/6/9/16/17.
// ============================================================================
extern "C" {
int _ZN10DonutBlock13InitResourcesEv(void *self);      /* slot 0, faced */
int _ZN10DonutBlock16CleanupResourcesEv(void *self);   /* slot 3, faced */
int _ZN10DonutBlock8BehaviorEv(void *self);            /* slot 6, faced */
int _ZN10DonutBlock6RenderEv(void *self);              /* slot 9, faced */
int *_ZN10DonutBlockD1Ev(int *self);                   /* slot 16, D1 */
int *_ZN10DonutBlockD0Ev(int *self);                   /* slot 17, D0 */
void *ArmedRotatingPlatform_Spawn(void);
int _ZTV10DonutBlock[32];  /* vtspan: _ZTV10DonutBlock */
}

static int __fastcall arp_init(void *s, void *)
{ return _ZN10DonutBlock13InitResourcesEv(s); }
static int __fastcall arp_clean(void *s, void *)
{ return _ZN10DonutBlock16CleanupResourcesEv(s); }
static int __fastcall arp_behavior(void *s, void *)
{ return _ZN10DonutBlock8BehaviorEv(s); }
static int __fastcall arp_render(void *s, void *)
{ port_actor_render_probe("ARMED_ROTATING_PLATFORM", (char *)s + 0xd4);
  return _ZN10DonutBlock6RenderEv(s); }
static int __fastcall arp_d1(void *s, void *)
{ return (int)(size_t)_ZN10DonutBlockD1Ev((int *)s); }
static int __fastcall arp_d0(void *s, void *)
{ return (int)(size_t)_ZN10DonutBlockD0Ev((int *)s); }

extern "C" void hal_fill_armed_rotating_platform_vtable(void)
{
    void *volatile *vt = (void *volatile *)_ZTV10DonutBlock;
    ov36_fill_shared(vt);
    vt[0]  = (void *)arp_init;
    vt[3]  = (void *)arp_clean;
    vt[6]  = (void *)arp_behavior;
    vt[9]  = (void *)arp_render;
    vt[16] = (void *)arp_d1;
    vt[17] = (void *)arp_d0;
    vt[31] = (void *)ov36_kill;
}

// ============================================================================
// TRICKY_TRIANGLES (123) -- 16daObjRc_Dorifu_c, 0x02113ecc, 32 slots,
// daObjDorifu_c <- dBgActor_c. One on level 28. Own 0/3/16/17; slots 6 and 9
// are ov002's shared bodies, parked directly in the table by the ROM. The
// class carries FIVE Models at +0x320 (stride 0x50) and FIVE
// MovingMeshColliders at +0x4b0 (stride 0x1c8) -- the drifting triangle set --
// and its own Init/Cleanup are veneers into ov002 with the class's three-word
// record data_ov036_02113e88 as the second argument.
// ============================================================================
extern "C" {
int _ZN21ArmedRotatingPlatform13InitResourcesEv(void *self);      /* slot 0, faced */
int _ZN21ArmedRotatingPlatform16CleanupResourcesEv(void *self);   /* slot 3, faced */
int func_ov002_020b4bfc(char *self);                              /* slot 6, ov002 shared */
int func_ov002_020b4bc4(char *self);                              /* slot 9, ov002 shared */
void *_ZN21ArmedRotatingPlatformD1Ev(void *self);                 /* slot 16, D1 */
int _ZN21ArmedRotatingPlatformD0Ev(void *self);                   /* slot 17, D0 */
void *TrickyTriangles_Spawn(void);
int _ZTV21ArmedRotatingPlatform[32];  /* vtspan: _ZTV21ArmedRotatingPlatform */
}

static int __fastcall tt_init(void *s, void *)
{ return _ZN21ArmedRotatingPlatform13InitResourcesEv(s); }
static int __fastcall tt_clean(void *s, void *)
{ return _ZN21ArmedRotatingPlatform16CleanupResourcesEv(s); }
static int __fastcall tt_behavior(void *s, void *)
{ return func_ov002_020b4bfc((char *)s); }
static int __fastcall tt_render(void *s, void *)
{ port_actor_render_probe("TRICKY_TRIANGLES", (char *)s + 0x320);
  return func_ov002_020b4bc4((char *)s); }
static int __fastcall tt_d1(void *s, void *)
{ return (int)(size_t)_ZN21ArmedRotatingPlatformD1Ev(s); }
static int __fastcall tt_d0(void *s, void *)
{ return _ZN21ArmedRotatingPlatformD0Ev(s); }

extern "C" void hal_fill_tricky_triangles_vtable(void)
{
    ov36_base_bringup();
    void *volatile *vt = (void *volatile *)_ZTV21ArmedRotatingPlatform;
    ov36_fill_shared(vt);
    vt[0]  = (void *)tt_init;
    vt[3]  = (void *)tt_clean;
    vt[6]  = (void *)tt_behavior;
    vt[9]  = (void *)tt_render;
    vt[16] = (void *)tt_d1;
    vt[17] = (void *)tt_d0;
    vt[31] = (void *)ov36_kill;
}

// ============================================================================
// FLYING_CARPET (130) -- 15daObjRcCarpet_c, 0x02113f9c, THIRTY-THREE slots,
// dPathLiftActor_c <- dBgActor_c. Four on level 28. Own 0/3/6/9/16/17 plus the
// INHERITED slot 32 (PathLift::AfterClsn, ov002 0x020eff18 -- the same word
// PATH_LIFT's own table carries).
//
// Layout, from its own Spawn and D1: Model at +0xd4, MovingMeshCollider at
// +0x124, a THREE-element Model array at +0x320 (stride 0x50), PathPtr at
// +0x430, the state gate at +0x428 and a ModelAnim at +0x450. Size 0x4c0.
// The Render is the host copy (port/unmatched/FlyingCarpet_Render.cpp), the
// ModelAnim slot-5 collision.
// ============================================================================
extern "C" {
int func_ov036_0211244c(char *self);       /* slot 0  */
int func_ov036_02112318(char *self);       /* slot 3  */
int func_ov036_021123c8(char *self);       /* slot 6, calls PathLift::BaseBehavior */
int func_ov036_02112378(void *self);       /* slot 9, HOST COPY */
int func_ov036_02112158(char *self);       /* slot 16, D1 */
void *func_ov036_021121c8(char *self);     /* slot 17, D0 */
void *FlyingCarpet_Spawn(void);
/* data_ov036_02113f9c[33] is declared in the DSSTATE block above */
}

static int __fastcall fc_init(void *s, void *)
{ return func_ov036_0211244c((char *)s); }
static int __fastcall fc_clean(void *s, void *)
{ return func_ov036_02112318((char *)s); }
static int __fastcall fc_behavior(void *s, void *)
{ return func_ov036_021123c8((char *)s); }
static int __fastcall fc_render(void *s, void *)
{ port_actor_render_probe("FLYING_CARPET", (char *)s + 0x450);
  return func_ov036_02112378(s); }
static int __fastcall fc_d1(void *s, void *)
{ return func_ov036_02112158((char *)s); }
static int __fastcall fc_d0(void *s, void *)
{ return (int)(size_t)func_ov036_021121c8((char *)s); }

extern "C" void hal_fill_flying_carpet_vtable(void)
{
    /* THE BASE MACHINERY IS OV100PL'S AND IS REUSED, NOT REBUILT.
       port_pathlift_states_seat rewrites the three PathLift state records'
       enter/tick words from DS code addresses to host ones, and
       func_ov036_021123c8 (this class's Behavior) dispatches through them via
       _ZN8PathLift12BaseBehaviorEv on its FIRST tick -- without the seat that
       is a call into unmapped DS memory. It is mandatory here and it carries
       its own done guard.
       hal_fill_path_lift_vtable is called for its side effect: it is the only
       exported caller of ov100pl's static pl_bringup, which fills the shared
       dPathLiftActor_c base table at ov002 0x0210af70 -- FLYING_CARPET's own
       intermediate, which its Spawn and D1 install between member teardowns,
       and which ov100pl hosts because port/ov002_syms.txt deliberately does
       not mount it.
       PATH_LIFT (31) is not placed on level 28, so on this level that call
       writes ov100's own table with the same fixed values it would get on
       levels 13 and 15 and nothing instantiates it; the write is inert and
       idempotent, and re-filling is cheaper and safer than a second fill of
       another lane's array from this file. */
    port_pathlift_states_seat();
    hal_fill_path_lift_vtable();
    void *volatile *vt = (void *volatile *)data_ov036_02113f9c;
    ov36_fill_shared(vt);
    vt[0]  = (void *)fc_init;
    vt[3]  = (void *)fc_clean;
    vt[6]  = (void *)fc_behavior;
    vt[9]  = (void *)fc_render;
    vt[16] = (void *)fc_d1;
    vt[17] = (void *)fc_d0;
    vt[31] = (void *)ov36_kill;
    vt[32] = (void *)ov36_after_clsn;   /* the INHERITED extra virtual */
}

// ---- method faces ----------------------------------------------------------
// Twelve ov036 TUs define a REAL C++ METHOD against their class header
// (int ShipWing::Behavior() and friends), so MSVC mangles them
// ?Behavior@ShipWing@@QAEHXZ and the ROM's C name _ZN8ShipWing8BehaviorEv is
// not provided by the object at all. Faced here, the IceSheet/OneUpLogo/
// BabyPenguin recipe: a thin extern "C" wrapper that calls the qualified
// method. The four bodies that are already extern "C" in src
// (_ZN18RotatingPlatformRr6RenderEv, _ZN18RotatingPlatformRr16CleanupResourcesEv,
// _ZN8ShipWing16OnPendingDestroyEv, _ZN21ArmedRotatingPlatformD1Ev) need no
// face and are declared above and called directly.
extern "C" {
int _ZN18RotatingPlatformRr13InitResourcesEv(void *self)
{ return ((RotatingPlatformRr *)self)->RotatingPlatformRr::InitResources(); }
int _ZN18RotatingPlatformRr8BehaviorEv(void *self)
{ return ((RotatingPlatformRr *)self)->RotatingPlatformRr::Behavior(); }

int _ZN8ShipWing13InitResourcesEv(void *self)
{ return ((ShipWing *)self)->ShipWing::InitResources(); }
int _ZN8ShipWing16CleanupResourcesEv(void *self)
{ return ((ShipWing *)self)->ShipWing::CleanupResources(); }
int _ZN8ShipWing8BehaviorEv(void *self)
{ return ((ShipWing *)self)->ShipWing::Behavior(); }
int _ZN8ShipWing6RenderEv(void *self)
{ return ((ShipWing *)self)->ShipWing::Render(); }

int _ZN10DonutBlock13InitResourcesEv(void *self)
{ return ((DonutBlock *)self)->DonutBlock::InitResources(); }
int _ZN10DonutBlock16CleanupResourcesEv(void *self)
{ return ((DonutBlock *)self)->DonutBlock::CleanupResources(); }
int _ZN10DonutBlock8BehaviorEv(void *self)
{ return ((DonutBlock *)self)->DonutBlock::Behavior(); }
int _ZN10DonutBlock6RenderEv(void *self)
{ return ((DonutBlock *)self)->DonutBlock::Render(); }

int _ZN21ArmedRotatingPlatform13InitResourcesEv(void *self)
{ return ((ArmedRotatingPlatform *)self)->ArmedRotatingPlatform::InitResources(); }
int _ZN21ArmedRotatingPlatform16CleanupResourcesEv(void *self)
{ return ((ArmedRotatingPlatform *)self)->ArmedRotatingPlatform::CleanupResources(); }
}
