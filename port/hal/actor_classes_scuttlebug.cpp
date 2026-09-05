// GATE 176: SCUTTLEBUG (255, ov071), the spider-legged Scuttlebug daSpd_c that
// skitters after Mario in a few caves and grounds. First class hosted out of
// ov071 (the eyeball MrI, MrI_Projectile and Coffin share the overlay); this is
// its FIRST mount, so ov071 joins PORT_ACTOR_OVERLAYS and its four sinits run.
//
// Same law as hal/actor_classes_flame.cpp and hal/actor_classes_montymole.cpp
// -- ROM slot order, __fastcall thunks that call QUALIFIED or by C name,
// unhosted slots trap by name -- and Scuttlebug is an Enemy subclass, so its
// vtable is the 31-slot shape: the reloc span [0x02122c2c, 0x02122ca8) is 31
// words, D1 slot 16 and D0 slot 17. Every slot below was read out of
// _ZTV10Scuttlebug (= _ZTV7daSpd_c, 0x02122c2c) with its 31 relocations applied
// and cross-checked against BlueFlame's table: the shared Actor/ActorBase halves
// (1/2/4/5/7/8/10/11/13/14/15/20..28/30) hold the identical arm9 addresses.
//
// The class owns 0 (Init), 3 (Cleanup), 6 (Behavior), 9 (Render), 12
// (OnPendingDestroy, an empty body of its OWN -- unlike the flame, which uses
// ActorBase's), 16 (D1), 17 (D0), and the interaction overrides 18
// (OnYoshiTryEat func_ov071_0211f0a4, returns 6), 19 (OnTurnIntoEgg
// func_ov071_02120580) and 29 (OnAimedAtWithEgg func_ov071_0211f0ac, returns
// 0x32000). Slots 13/14 (ActorBase::Virtual34/38, 0x0204357c/0x0204349c) trap
// by name the way every sibling fill traps that pair, and slot 30
// (OnAimedAtWithEggReturnVec, arm9 0x020100dc) traps because its matched body
// is SRET -- a hidden return-slot pointer before self -- which no fill's thunk
// shape models. Everything else binds to Actor/ActorBase's own default half,
// all in the map (the montymole binding set).
//
// BEHAVIOR IS MATCHED SRC, but the two STATE DISPATCHERS it and SetState reach
// are host copies: func_ov071_021202b4 (ENTER) and func_ov071_02120278 (MAIN,
// called by Behavior) form mwcc pointer-to-member reads MSVC mis-strides, so
// they live in port/unmatched/Scuttlebug_StateDispatch.cpp with the state seat.
// Init/Behavior/Render are real MSVC members against include/Scuttlebug.h
// (faces below); Cleanup and D0 are plain C; OnPendingDestroy is an empty .c.
//
// D1 (slot 16) is a HOST THUNK, the flame/HauntedChair treatment. The matched
// D1 (_ZN10ScuttlebugD1Ev.cpp) is the auto-emitted-member-dtor form -- an
// empty ~Scuttlebug() whose compiler-emitted member and base dtor calls resolve
// to MSVC-mangled names that do not exist in this build -- so it is NOT
// compiled; the thunk runs the D0 chain (ScuttlebugD0's own order:
// WithMeshClsn +0x194, MovingCylinderClsn +0x160, ShadowModel +0x138,
// ModelAnim +0xd4, ~Actor) minus the final Memory::Deallocate, because the
// slot-16 caller (ActorBase::AfterCleanupResources) deallocates itself.
//
// The id was cross-checked from the relocated overlay image: Scuttlebug_SpawnInfo
// (0x02122c08) +0 word = Scuttlebug_Spawn (0x02120618), +4 halfword = 0x00ff =
// 255, and ACTOR_SPAWN_TABLE[255] (data_02090864 + 255*4 = 0x02090c60) points at
// that record. Scuttlebug_Spawn's own vtable-store site (Spawn.c: p[0] =
// _ZTV10Scuttlebug) names the table.
#include <cstdio>

/* hal/actor_slot30_seat.cpp -- the shared seat for vtable slot 30,
   Actor::OnAimedAtWithEggReturnVec. The ROM word in slot 30 of every vtable
   this file fills IS the arm9 base body 0x020100dc (checked against
   config/<module>/relocs.txt at vtable+30*4), and that body is now in the
   link from src/_ZN5Actor25OnAimedAtWithEggReturnVecEv.cpp on slice_gate50.
   The three-parameter __fastcall is the sret contract MSVC uses for a
   thiscall member returning a 12-byte struct: this in ecx, the hidden result
   pointer the one (callee-popped) stack argument. Same shape as whomp_s30. */
extern "C" void *__fastcall port_actor_s30_base(void *self, void *, void *out);
#include <cstdlib>

#include "Actor.h"
#include "dtor_faces_cpp.h"
#include "ActorBase.h"
#include "Scuttlebug.h"

extern "C" {
/* the shared lifecycle halves, the same functions every fill writes */
int _ZN5Actor19BeforeInitResourcesEv(void *self);            /* slot 1  */
void _ZN5Actor18AfterInitResourcesEj(void *self, unsigned a); /* slot 2  */
int _ZN5Actor14BeforeBehaviorEv(void *self);                 /* slot 7  */
int _ZN5Actor12BeforeRenderEv(void *self);                   /* slot 10 */
int _ZN5Actor9Virtual50Ev(void *self);                       /* slot 20 */
void _ZN5Actor15OnGroundPoundedERS_(void *self, void *o);    /* slot 21 */
void _ZN5Actor11OnAttacked1ERS_(void *self, void *o);        /* slot 22 */
void _ZN5Actor11OnAttacked2ERS_(void *self, void *o);        /* slot 23 */
void _ZN5Actor8OnKickedERS_(void *self, void *o);            /* slot 24 */
void _ZN5Actor8OnPushedERS_(void *self, void *o);            /* slot 25 */
void _ZN5Actor24OnHitByCannonBlastedCharERS_(void *self, void *o); /* slot 26 */
void _ZN5Actor15OnHitByMegaCharER6Player(void *self, void *p);     /* slot 27 */
void _ZN5Actor19OnHitFromUnderneathERS_(void *self, void *o);      /* slot 28 */

extern int data_02099f24[];          /* the frame phase the lists are in */
extern unsigned char data_020a4b4c;  /* the spawn spine's own step */
const char *port_actor_class_name(unsigned id);   /* hal/actor_registry */
  void port_actor_slot_decline(const char *what);  /* func_02043fdc_hostcopy.cpp */
void port_actor_render_probe(const char *cls, void *model); /* actor_classes */

/* Scuttlebug's own C-linkage bodies. Behavior is matched src; the two state
   dispatchers it reaches are the host copies in
   port/unmatched/Scuttlebug_StateDispatch.cpp. */
int _ZN10Scuttlebug16CleanupResourcesEv(void);    /* slot 3,  .c C linkage */
int _ZN10Scuttlebug16OnPendingDestroyEv(void);    /* slot 12, .c C linkage */
int *_ZN10ScuttlebugD0Ev(int *self);              /* slot 17, .c C linkage */
int func_ov071_0211f0a4(void);                    /* slot 18, OnYoshiTryEat */
void func_ov071_02120580(void *self, void *p);    /* slot 19, OnTurnIntoEgg */
int func_ov071_0211f0ac(void);                    /* slot 29, OnAimedAtWithEgg */
void *Scuttlebug_Spawn(void);

/* the D1 chain's sub-object destructors, all C-linkage in the build (the same
   node dtors ScuttlebugD0 calls) */
void _ZN12WithMeshClsnD1Ev(void *);         /* the WithMeshClsn at +0x194 */
void _ZN18MovingCylinderClsnD1Ev(void *);   /* the MovingCylinderClsn at +0x160 */
void _ZN11ShadowModelD1Ev(void *);          /* the ShadowModel at +0x138 */
void _ZN9ModelAnimD1Ev(void *);             /* the ModelAnim at +0xd4 */
void *_ZN5ActorD2Ev(void *);                /* the Actor base */

/* The array the ROM factory installs (Scuttlebug_Spawn does
   `p[0] = (int)_ZTV10Scuttlebug`); thirty-one slots. Defined here, not just
   declared: the `int` type and C linkage match the `extern int
   _ZTV10Scuttlebug[]` in include/decl_common.h that the .c factory and D0 read. */
int _ZTV10Scuttlebug[31];
}

/* ScuttlebugD0.c stores _ZTV7daSpd_c (the RTTI base spelling dsd names at the
   same address) into the object's slot 0; both references are C linkage, so
   point the underscore spelling at the one host array -- the daChoropu_c /
   daObjFire_c precedent. */
#pragma comment(linker, "/alternatename:__ZTV7daSpd_c=__ZTV10Scuttlebug")

/* dsd mislabeled the anim SharedFilePtr in func_ov071_0211fbf4: the src spells
   data_ov073_02122f88 but the load site's reloc (0x0211fc5c) targets 0x02122f88
   module:overlay(71) -- ov071's own anim pointer, mounted in ov071_syms.txt.
   The config rename goes upstream as its own by-address PR.

   THE ALIAS THAT USED TO SIT HERE IS GONE (run linkw wave 12, lane w12). It
   was
       /alternatename:_data_ov073_02122f88=_data_ov071_02122f88
   and it DIED the moment ov073 was mounted: 0x02122f88 is a real ov073 address
   too -- ChiefChilly's tenth {handler, adj} state record -- so port/ov073_syms.
   txt defines the LHS and a defined LHS defeats /alternatename silently. That
   is the wave-5 R1/R2 arrival shape and tools/alternatename_guard.py caught it
   at the first link. The routing moved to the R1/R2 remedy, a per-source
   rename on the one TU that needs it, in port/CMakeLists.txt:

       set_source_files_properties(src/func_ov071_0211fbf4.c PROPERTIES
           COMPILE_DEFINITIONS "data_ov073_02122f88=data_ov071_02122f88")

   Scuttlebug's behaviour is unchanged: that TU still reads
   data_ov071_02122f88, by name now instead of through the alias. Leaving the
   pragma in place would have been the silent failure -- the reference would
   have bound to ChiefChilly's state record and handed Animation::LoadFile a
   code address. */
/* __sinit_ov071_02122a64 names a second view of data_ov071_02122ecc with a
   dsd _d suffix (a struct overlay on the same address); one storage, two
   spellings. */
#pragma comment(linker, "/alternatename:_data_ov071_02122ecc_d=_data_ov071_02122ecc")

/* The C++/C linkage bridge, the gate-172..175 reading:
   - AnimLoadFile is decl_common's friendly C name for the STATIC
     Animation::LoadFile (cdecl both sides, one arg; the matched method rides
     the slice) -- the UpdatePosWithTransform static-method alias precedent.
   - InitResources spells WithMeshClsn::Init with a void* fourth param; the
     compiled bridge (hal/actor_class_faces.cpp) is the Vector3_16* spelling.
     Same __thiscall body, same widths.
   - func_ov071_0211f7d4 spells GetFloorResult with a void* return; the
     compiled body (slice_gate10) is the int-return spelling. eax either way.
   - func_ov071_0211f524 declares the arm9 identity matrix as a C++ Mtx43;
     romdata.c defines the C symbol. */
#pragma comment(linker, "/alternatename:_AnimLoadFile=?LoadFile@Animation@@SAPADAAUSharedFilePtr@@@Z")
#pragma comment(linker, "/alternatename:?Init@WithMeshClsn@@QAEXPAUActor@@HHPAXH@Z=?Init@WithMeshClsn@@QAEXPAUActor@@HHPAUVector3_16@@H@Z")
#pragma comment(linker, "/alternatename:?GetFloorResult@WithMeshClsn@@QBEPAXXZ=?GetFloorResult@WithMeshClsn@@QBEHXZ")
#pragma comment(linker, "/alternatename:?data_020a0e68@@3UMtx43@@A=_data_020a0e68")

/* func_ov071_0211f7d4 names its second cylinder collider CylinderClsn2 and
   calls Clear/Update as methods; the bodies are the one CylinderClsn's own
   C-linkage forms, already linked. A local shadow bridges the __thiscall
   spellings -- an alias cannot cross the __thiscall/cdecl seam. */
extern "C" void _ZN12CylinderClsn5ClearEv(void *c);
extern "C" void _ZN12CylinderClsn6UpdateEv(void *c);
struct CylinderClsn2 { void Clear(); void Update(); };
void CylinderClsn2::Clear() { _ZN12CylinderClsn5ClearEv(this); }
void CylinderClsn2::Update() { _ZN12CylinderClsn6UpdateEv(this); }

// ---- the trap --------------------------------------------------------------
static void sb_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: vtable slot %d is not hosted (actor id %u %s, "
                 "phase %d, spawn step %d)\n",
                 slot, id, port_actor_class_name(id), data_02099f24[0],
                 (int)data_020a4b4c);
    { static char _m[128];
      std::snprintf(_m, sizeof _m, "unhosted vtable slot %d on id %u %s",
                    slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
#define SB_TRAP(n) \
    static int __fastcall sb_trap##n(void *s, void *) \
    { sb_trap_report(s, n); return 0; }
/* 13/14 are ActorBase::Virtual34/38 (not linked, the sibling trap); 30 is the
   SRET OnAimedAtWithEggReturnVec no thunk shape models. */
SB_TRAP(13) SB_TRAP(14)
#undef SB_TRAP

// ---- the shared half -------------------------------------------------------
static int __fastcall sb_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall sb_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall sb_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall sb_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall sb_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall sb_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall sb_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall sb_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall sb_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall sb_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall sb_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall sb_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall sb_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall sb_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall sb_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall sb_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall sb_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall sb_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }

// ---- SCUTTLEBUG's own slots -------------------------------------------------
/* Init/Behavior/Render are real __thiscall members; the face bridges cdecl to
   __thiscall so the ecx that never held `this` is not handed to the body. */
static int __fastcall sb_init(void *s, void *)
{ return ((Scuttlebug *)s)->Scuttlebug::InitResources(); }
static int __fastcall sb_behavior(void *s, void *)
{ return ((Scuttlebug *)s)->Scuttlebug::Behavior(); }
/* Render is the ModelAnim_Renders host copy (ROM-order slot-5 dispatch, the
   Whomp/Fish case), not the matched TU. */
extern "C" int _ZN10Scuttlebug6RenderEv(void *self);
static int __fastcall sb_render(void *s, void *)
{ port_actor_render_probe("SCUTTLEBUG", (char *)s + 0xd4);
  return _ZN10Scuttlebug6RenderEv(s); }
/* Cleanup, OnPendingDestroy and D0 are plain C. */
static int __fastcall sb_clean(void *s, void *)
{ (void)s; return _ZN10Scuttlebug16CleanupResourcesEv(); }
static int __fastcall sb_pdes(void *s, void *)
{ (void)s; return _ZN10Scuttlebug16OnPendingDestroyEv(); }
static int __fastcall sb_d0(void *s, void *)
{ return (int)(size_t)_ZN10ScuttlebugD0Ev((int *)s); }
/* the interaction overrides Scuttlebug carries of its own */
static int __fastcall sb_yoshi(void *s, void *)
{ (void)s; return func_ov071_0211f0a4(); }
static int __fastcall sb_egg(void *s, void *, void *p)
{ func_ov071_02120580(s, p); return 0; }
static int __fastcall sb_aimed(void *s, void *)
{ (void)s; return func_ov071_0211f0ac(); }
/* D1, the complete-object destructor slot 16 holds: the D0 chain
   (_ZN10ScuttlebugD0Ev) without the Memory::Deallocate at its tail, because the
   caller of slot 16 (ActorBase::AfterCleanupResources) deallocates itself.
   Member sub-objects are destroyed high address first, the D0 order. */
/* slot 16 is the matched src D1 through hal/dtor_faces_cpp.cpp (lane DTOR-FACES-CPP);
   the transcribed thunk that stood here (sb_d1) spelled the same chain by hand. */

extern "C" void hal_fill_scuttlebug_vtable(void)
{
    void **vt = (void **)_ZTV10Scuttlebug;
    vt[0]  = (void *)sb_init;
    vt[1]  = (void *)sb_binit;
    vt[2]  = (void *)sb_ainit;
    vt[3]  = (void *)sb_clean;
    vt[4]  = (void *)sb_bclean;
    vt[5]  = (void *)sb_aclean;
    vt[6]  = (void *)sb_behavior;
    vt[7]  = (void *)sb_bbeh;
    vt[8]  = (void *)sb_abeh;
    vt[9]  = (void *)sb_render;
    vt[10] = (void *)sb_bren;
    vt[11] = (void *)sb_aren;
    vt[12] = (void *)sb_pdes;
    vt[13] = (void *)sb_trap13;
    vt[14] = (void *)sb_trap14;
    vt[15] = (void *)sb_heap;
    vt[16] = (void *)hal_cppd1_Scuttlebug;
    vt[17] = (void *)sb_d0;
    /* the Enemy tail (18..30): 18/19/29 are Scuttlebug's own overrides, the
       rest bind Actor/ActorBase's default half, 30 traps (SRET). */
    vt[18] = (void *)sb_yoshi;
    vt[19] = (void *)sb_egg;
    vt[20] = (void *)sb_v50;
    vt[21] = (void *)sb_pounded;
    vt[22] = (void *)sb_atk1;
    vt[23] = (void *)sb_atk2;
    vt[24] = (void *)sb_kicked;
    vt[25] = (void *)sb_pushed;
    vt[26] = (void *)sb_cannon;
    vt[27] = (void *)sb_mega;
    vt[28] = (void *)sb_under;
    vt[29] = (void *)sb_aimed;
    vt[30] = (void *)port_actor_s30_base;
}
