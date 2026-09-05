// GATE 177: the first three classes hosted out of ov064, Lethal Lava Land's own
// actor overlay -- BULLY (215, daDonketu_c), BIG_BULLY (216, daBDonketu_c) and
// ROTATING_FIREBAR (81, the firebar). First mount of ov064 (ov064_syms.txt).
//
// Same law as hal/actor_classes_montymole.cpp and hal/actor_classes_flame.cpp --
// ROM slot order, __fastcall thunks that call QUALIFIED or the class's C body,
// unhosted slots trap by name -- with one shape no class before this gate had.
//
// ---- THE BULLY VTABLE IS THIRTY-SEVEN SLOTS --------------------------------
//
// _ZTV5Bully (ov064 0x0211b870) and _ZTV8BigBully (0x0211b978) are NOT the plain
// 31-slot Actor shape. Read with their relocations applied, slots 0..30 are the
// exact 31-slot Enemy table every sibling has (slot 1 = Actor::BeforeInit-
// Resources 0x02011268, ..., slot 30 = OnAimedAtWithEggReturnVec 0x020100dc),
// and then SIX MORE class-own virtuals follow, slots 31..36. The scout's
// thirty-seven-slot span was right and it is NOT the DorrieCap trailing-data
// trap: the reloc run is 37 consecutive words 0x0211b870..0x0211b900 and the
// next symbol (data_ov064_0211b904) begins exactly one word past the last slot.
// Bully and BigBully share the extension because they share a base class (its
// own 37-slot table is data_ov064_0211b768, left out of the mount -- its typeinfo
// points into the shared level window and nothing dispatches through it).
//
// ROTATING_FIREBAR is a PLATFORM, thirty-two slots (_ZTV15RotatingFirebar
// 0x0211be10). Slots 0..30 are the standard table with NO overrides past its own
// 0/3/6/9/16/17 (slot 29 keeps Actor's own OnAimedAtWithEgg default 0x02010124),
// and slot 31 is _ZN8Platform4KillEv (ov002 0x020ee55c, already in the build --
// the Thwomp/Stump reading). The reloc names it module:overlays(2,7) because
// ov002 and ov007 share that window; the ov002 body is Platform::Kill.
//
// ---- THE FACTORIES LEAVE THE VPTR ON A PLACEHOLDER -------------------------
//
// Bully_Spawn and BigBully_Spawn are matched .c (in the slice). Each installs
// its derived table by the RTTI wildcard (_ZTV11daDonketu_c / _ZTV12daBDonketu_c,
// aliased below to the host arrays) and then stores VT1 -- auto_bss's shared
// zeroed [8] array -- as its LAST vptr write, so a raw spawn leaves the object on
// nulls. The host registers port_factory_bully / port_factory_big_bully, which
// call the .c factory and reseat slot 0 onto the class's host table: the Thwomp /
// RotatingPlatformWf treatment (hal/actor_classes_l7.cpp). RotatingFirebar_Spawn
// installs _ZTV15RotatingFirebar directly and needs no wrapper.
//
// ---- D1/D0 ARE HOST THUNKS FOR THE BULLIES ---------------------------------
//
// Bully's and BigBully's own D1/D0 (.c) also store the VT1 placeholder in the
// teardown chain, the same single-global name three other TUs spell for three
// different tables -- one host definition would satisfy them all with the wrong
// bytes and nothing would say so (the CastleWater/SphereClsn reading). So they
// are NOT in the slice; the D0/D1 thunks below run the matched chain (member
// dtors high-address first, then the Enemy base D2 func_ov002_020aed18, then --
// for D0 -- Memory::Deallocate on the game heap) with the derived table stored
// once. RotatingFirebar's D1/D0 (.c) spell _ZTV8Platform and data_020a0eac by
// name, both already hosted, so they stay in the slice and the fill just calls
// them.
//
// ---- MID-LIFE SPAWNS -------------------------------------------------------
//
// Bully::Behavior spawns actor 0x120 = 288 = COIN when it is knocked into the
// lava (ret==2), null-checking the result. COIN is hosted and registered
// (hal/actor_classes_bob_world.cpp, _ZTV4Coin, ids 288/289/290), its SpawnInfo
// is in ov002_syms.txt, so the spawn lands on a live class. BigBully::Init-
// Resources spawns 0xd7 = 215 = BULLY (three of them, when its param nibble is 1)
// and null-checks; BULLY is this gate's own class, so that path is hosted too.
#include <cstdio>
#include <cstdlib>

#include "Actor.h"
#include "ActorBase.h"

extern "C" {
/* the shared lifecycle halves, the same functions every 31-slot fill writes */
int _ZN5Actor19BeforeInitResourcesEv(void *self);            /* slot 1  */
void _ZN5Actor18AfterInitResourcesEj(void *self, unsigned a); /* slot 2  */
int _ZN5Actor14BeforeBehaviorEv(void *self);                 /* slot 7  */
int _ZN5Actor12BeforeRenderEv(void *self);                   /* slot 10 */
int _ZN5Actor13OnYoshiTryEatEv(void *self);                  /* slot 18 */
void _ZN5Actor13OnTurnIntoEggER6Player(void *self, void *p); /* slot 19 */
int _ZN5Actor9Virtual50Ev(void *self);                       /* slot 20 */
void _ZN5Actor15OnGroundPoundedERS_(void *self, void *o);    /* slot 21 */
void _ZN5Actor11OnAttacked1ERS_(void *self, void *o);        /* slot 22 */
void _ZN5Actor11OnAttacked2ERS_(void *self, void *o);        /* slot 23 */
void _ZN5Actor8OnKickedERS_(void *self, void *o);            /* slot 24 */
void _ZN5Actor8OnPushedERS_(void *self, void *o);            /* slot 25 */
void _ZN5Actor24OnHitByCannonBlastedCharERS_(void *self, void *o); /* slot 26 */
void _ZN5Actor15OnHitByMegaCharER6Player(void *self, void *p);     /* slot 27 */
void _ZN5Actor19OnHitFromUnderneathERS_(void *self, void *o);      /* slot 28 */
int _ZN5Actor16OnAimedAtWithEggEv(void *self);               /* slot 29 (Actor's) */

extern int data_02099f24[];          /* the frame phase the lists are in */
extern unsigned char data_020a4b4c;  /* the spawn spine's own step */
const char *port_actor_class_name(unsigned id);   /* hal/actor_registry */
  void port_actor_slot_decline(const char *what);  /* func_02043fdc_hostcopy.cpp */
void port_actor_render_probe(const char *cls, void *model); /* actor_classes */

/* Bully's own C-linkage bodies (matched src, slice_gate177.txt). */
int _ZN5Bully13InitResourcesEv(void *self);       /* slot 0, face below */
int *_ZN5BullyD1Ev(int *self);                    /* slot 16, .c, DTOR-PAIRS seat (0x02117070) */
int *_ZN5BullyD0Ev(int *self);                    /* slot 17, .c, DTOR-PAIRS seat (0x021170c4) */
int _ZN5Bully16CleanupResourcesEv(void *self);    /* slot 3, face below */
int _ZN5Bully8BehaviorEv(char *self);             /* slot 6, .c C linkage */
int _ZN5Bully6RenderEv(void *self);               /* slot 9, face below */
int func_ov064_02115f84(void *self);              /* slot 29, OnAimedAtWithEgg */
int func_ov064_021171b0(void *self);              /* slot 31 */
int func_ov064_02117220(void *self);              /* slot 32 */
int func_ov064_02117168(void *self);              /* slot 33 */
int func_ov064_02117154(void *self);              /* slot 34 */
int func_ov064_02117140(void *self);              /* slot 35 */
int func_ov064_0211712c(void *self);              /* slot 36 */
void *Bully_Spawn(void);

/* BigBully's own bodies. Slots 3 (Cleanup) and 29 (OnAimedAtWithEgg) are
   Bully's -- the shared base bodies at 0x02116ca0 / 0x02115f84 -- so BigBully's
   table reuses them. Its six extension slots 31..36 are its own. */
int _ZN8BigBully13InitResourcesEv(void *self);    /* slot 0, face below */
int *_ZN8BigBullyD1Ev(int *self);                 /* slot 16, .c, DTOR-PAIRS seat (0x021174a0) */
int *_ZN8BigBullyD0Ev(int *self);                 /* slot 17, .c, DTOR-PAIRS seat (0x021174f4) */
int _ZN8BigBully8BehaviorEv(void *self);          /* slot 6, face below (.cpp method) */
int _ZN8BigBully6RenderEv(void *self);            /* slot 9, face below */
int func_ov064_0211755c(void *self);              /* slot 31 */
int func_ov064_021175cc(void *self);              /* slot 32 */
int func_ov064_02116374(void *self);              /* slot 33 */
int func_ov064_02116360(void *self);              /* slot 34 */
int func_ov064_0211635c(void *self);              /* slot 35 */
int func_ov064_02116348(void *self);              /* slot 36 */
void *BigBully_Spawn(void);

/* RotatingFirebar's own bodies. D0/D1 stay in the slice (named vtables). */
int _ZN15RotatingFirebar13InitResourcesEv(void *self);    /* slot 0, face below */
int _ZN15RotatingFirebar16CleanupResourcesEv(void *self); /* slot 3, face below */
int _ZN15RotatingFirebar8BehaviorEv(void *self);          /* slot 6, face below */
int _ZN15RotatingFirebar6RenderEv(void *self);            /* slot 9, face below */
int *_ZN15RotatingFirebarD1Ev(void *self);                /* slot 16, .c */
int *_ZN15RotatingFirebarD0Ev(void *self);                /* slot 17, .c */
void *RotatingFirebar_Spawn(void);
void _ZN8Platform4KillEv(void *self);                     /* slot 31 */

/* the Bully/BigBully D-tor chain's sub-object destructors and base D2, all
   C-linkage in the build */
void _ZN11ShadowModelD1Ev(void *);          /* the ShadowModel at +0x370 */
void _ZN18MovingCylinderClsnD1Ev(void *);   /* the MovingCylinderClsn at +0x33c */
void _ZN12WithMeshClsnD1Ev(void *);         /* the WithMeshClsn at +0x174 */
void _ZN9ModelAnimD1Ev(void *);             /* the ModelAnim at +0x110 */
void func_ov002_020aed18(void *);           /* the Enemy base D2 (ov002) */
void _ZN6Memory10DeallocateEPvP4Heap(void *, void *);
extern void *data_020a0eac;                 /* Memory::gameHeapPtr (== G0) */

/* the three derived vtables, HOST arrays this file fills; 37/37/32 slots.
   Defined `int[]`, not `void*[]`: the `int` type and C linkage match the
   `extern int _ZTV15RotatingFirebar[]` in include/decl_common.h that the .c
   factories and D-tors read (the montymole reading). Bully and BigBully spell
   their tables only by the RTTI wildcard, so decl_common.h declares those as
   _ZTV11daDonketu_c / _ZTV12daBDonketu_c (also int[]); the aliases below bridge. */
int _ZTV5Bully[37];
int _ZTV8BigBully[37];
int _ZTV15RotatingFirebar[32];
}

/* The factories and D-tors spell the derived tables by the RTTI wildcard names;
   point them at the host arrays -- the daChoropu_c / daBmb_c precedent. */
#pragma comment(linker, "/alternatename:__ZTV11daDonketu_c=__ZTV5Bully")
#pragma comment(linker, "/alternatename:__ZTV12daBDonketu_c=__ZTV8BigBully")

/* The C++/C linkage bridge, the gate-172..176 reading: RotatingFirebar's
   InitResources spells its file table as a C++ void** global, and
   func_ov064_0211616c spells the Particle System::New call at C++ linkage
   over the Itanium name. */
#pragma comment(linker, "/alternatename:?data_ov064_0211adbc@@3PAPAXA=_data_ov064_0211adbc")
/* func_ov064_0211616c.c is forced LANGUAGE CXX (an empty struct in a .c) but
   defines its own symbol unwrapped, so the C spelling its callers use maps
   onto the mangled definition. */
#pragma comment(linker, "/alternatename:_func_ov064_0211616c=?func_ov064_0211616c@@YAHPAD@Z")
#pragma comment(linker, "/alternatename:?_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE@@YAPAXIIHHHPBUVector3_16f@@PAUCallback@@@Z=__ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE")

// ---- the trap --------------------------------------------------------------
static void ov64_trap_report(void *self, int slot)
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
#define OV64_TRAP(n) \
    static int __fastcall ov64_trap##n(void *s, void *) \
    { ov64_trap_report(s, n); return 0; }
/* 13/14 are ActorBase::Virtual34/38 (not linked, the sibling trap); 30 is the
   SRET OnAimedAtWithEggReturnVec no thunk shape models. */
OV64_TRAP(13) OV64_TRAP(14)
#undef OV64_TRAP

// ---- the shared 0..30 half -------------------------------------------------
static int __fastcall ov64_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall ov64_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall ov64_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall ov64_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall ov64_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall ov64_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall ov64_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall ov64_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall ov64_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall ov64_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall ov64_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
static int __fastcall ov64_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall ov64_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall ov64_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall ov64_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall ov64_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall ov64_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall ov64_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall ov64_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall ov64_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall ov64_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall ov64_aimed_actor(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }   /* slot 29, Actor's own default */

/* Fill slots 1..30 of a Bully/BigBully/Firebar table with the shared bodies.
   The caller writes 0/3/6/9/16/17/29 and (for the bullies) 31..36. */
static void ov64_fill_shared_0_30(void **vt)
{
    vt[1]  = (void *)ov64_binit;
    vt[2]  = (void *)ov64_ainit;
    vt[4]  = (void *)ov64_bclean;
    vt[5]  = (void *)ov64_aclean;
    vt[7]  = (void *)ov64_bbeh;
    vt[8]  = (void *)ov64_abeh;
    vt[10] = (void *)ov64_bren;
    vt[11] = (void *)ov64_aren;
    vt[12] = (void *)ov64_pdes;
    vt[13] = (void *)ov64_trap13;
    vt[14] = (void *)ov64_trap14;
    vt[15] = (void *)ov64_heap;
    vt[18] = (void *)ov64_yoshi;
    vt[19] = (void *)ov64_egg;
    vt[20] = (void *)ov64_v50;
    vt[21] = (void *)ov64_pounded;
    vt[22] = (void *)ov64_atk1;
    vt[23] = (void *)ov64_atk2;
    vt[24] = (void *)ov64_kicked;
    vt[25] = (void *)ov64_pushed;
    vt[26] = (void *)ov64_cannon;
    vt[27] = (void *)ov64_mega;
    vt[28] = (void *)ov64_under;
    vt[30] = (void *)port_actor_s30_base;
}

// ============================================================================
// BULLY (215) and BIG_BULLY (216)
// ============================================================================
static int __fastcall bly_behavior(void *s, void *)
{ return _ZN5Bully8BehaviorEv((char *)s); }
static int __fastcall bly_clean(void *s, void *)
{ return _ZN5Bully16CleanupResourcesEv(s); }
static int __fastcall bly_aimed(void *s, void *)
{ return func_ov064_02115f84(s); }              /* slot 29, OnAimedAtWithEgg: derefs [this+0x330] */
/* slots 31..36: the six extension virtuals, Bully's own.

   SLOT 32 STAYS THE TWO-PARAMETER SHAPE, AND THE REASON IS MEASURED. It was
   briefly widened to three parameters on the theory that slot 32 means
   AfterClsn everywhere and AfterClsn is always dispatched with a pushed Actor.
   That theory is FALSE, and the widening was the PathLift bug mirrored: a
   callee popping four bytes nobody pushed.

   THE CENSUS. Every call through a slot-32 vtable word in the linked binary,
   swept over `dumpbin /disasm:nobytes walk_window.exe` for
   `call dword ptr [reg+00000080h]`. There are exactly THREE, and they do not
   agree:

     func_ov004_020b08f0 +0xb    mov ecx,esi / call [eax+80h]     pushes 0
     func_ov002_020eff90 +0xb    push [ebp+10h] / call [eax+80h]  pushes 1
     func_ov064_02116d1c +0x129  mov ecx,esi / call [eax+80h]     pushes 0

   Only the middle one pushes, and that one is the PathLift collider veneer.
   BULLY IS DISPATCHED BY THE THIRD, which pushes nothing, so its slot-32
   callee must emit a bare `ret`. Widening it made the callee pop four bytes
   the caller never pushed, and func_ov064_02116d1c's epilogue cannot absorb
   that -- it unwinds by `add esp,0Ch` and four explicit pops with no
   `mov esp,ebp`, so every pop reads one slot high and its own `ret` takes the
   wrong word. Same wild execute as report 7447e46c, pointing the other way.

   SO SLOT NUMBER IS NOT A CONTRACT. An extension virtual is per-class by
   definition, and three classes' slot 32 are three different methods. Only the
   dispatch SITE fixes a pop, which is why abicheck's authority is keyed on the
   site and why slot 32 as a whole is left UNJUDGED there. */
static int __fastcall bly_v31(void *s, void *) { return func_ov064_021171b0(s); }
static int __fastcall bly_v32(void *s, void *) { return func_ov064_02117220(s); }
static int __fastcall bly_v33(void *s, void *) { return func_ov064_02117168(s); }
static int __fastcall bly_v34(void *s, void *) { return func_ov064_02117154(s); }
static int __fastcall bly_v35(void *s, void *) { return func_ov064_02117140(s); }
static int __fastcall bly_v36(void *s, void *) { return func_ov064_0211712c(s); }
/* D1/D0 (DTOR-PAIRS seat): each class's own matched flat-C pair behind
   ecx->arg adapters, where host copies of the chain stood. The VT1 the bodies
   spell is settled by the ROM's own relocation, not by the shared auto_bss
   dummy: every one of the four bodies' second store relocates to ov064
   0x0211b768 (data_ov064_0211b768, the bullies' 37-slot base table, hosted in
   hal/actor_classes_ov027.cpp), so port/CMakeLists.txt compiles the four TUs
   with VT1=data_ov064_0211b768 (the minigame-scene D0 treatment). The store is
   dead anyway -- the Enemy base D2 (func_ov002_020aed18) writes its own table
   next -- and it is the ROM's word. G0 is the game heap by the standing alias. */
static int __fastcall bly_d1(void *s, void *)
{ return (int)(size_t)_ZN5BullyD1Ev((int *)s); }
static int __fastcall bly_d0(void *s, void *)
{ return (int)(size_t)_ZN5BullyD0Ev((int *)s); }
static int __fastcall bbly_d1(void *s, void *)
{ return (int)(size_t)_ZN8BigBullyD1Ev((int *)s); }
static int __fastcall bbly_d0(void *s, void *)
{ return (int)(size_t)_ZN8BigBullyD0Ev((int *)s); }
static int __fastcall bly_init(void *s, void *)
{ return _ZN5Bully13InitResourcesEv(s); }
static int __fastcall bly_render(void *s, void *)
{ port_actor_render_probe("BULLY", (char *)s + 0x110);
  return _ZN5Bully6RenderEv(s); }

extern "C" void hal_fill_bully_vtable(void)
{
    void **vt = (void **)_ZTV5Bully;
    ov64_fill_shared_0_30(vt);
    vt[0]  = (void *)bly_init;
    vt[3]  = (void *)bly_clean;
    vt[6]  = (void *)bly_behavior;
    vt[9]  = (void *)bly_render;
    vt[16] = (void *)bly_d1;
    vt[17] = (void *)bly_d0;
    vt[29] = (void *)bly_aimed;
    vt[31] = (void *)bly_v31;
    vt[32] = (void *)bly_v32;
    vt[33] = (void *)bly_v33;
    vt[34] = (void *)bly_v34;
    vt[35] = (void *)bly_v35;
    vt[36] = (void *)bly_v36;
}

/* Bully_Spawn's last vptr write is the VT1 placeholder; reseat onto the host
   table (the Thwomp treatment). */
extern "C" void *port_factory_bully(void)
{
    void *p = Bully_Spawn();
    if (p)
        *(void **)p = (void *)_ZTV5Bully;
    return p;
}

// ---- BIG_BULLY -------------------------------------------------------------
static int __fastcall bbly_init(void *s, void *)
{ return _ZN8BigBully13InitResourcesEv(s); }
static int __fastcall bbly_behavior(void *s, void *)
{ return _ZN8BigBully8BehaviorEv(s); }
static int __fastcall bbly_render(void *s, void *)
{ port_actor_render_probe("BIG_BULLY", (char *)s + 0x110);
  return _ZN8BigBully6RenderEv(s); }
/* BigBully's own six extension virtuals. Slot 32 stays the two-parameter shape
   for bly_v32's measured reason: its dispatch site pushes nothing. */
static int __fastcall bbly_v31(void *s, void *) { return func_ov064_0211755c(s); }
static int __fastcall bbly_v32(void *s, void *) { return func_ov064_021175cc(s); }
static int __fastcall bbly_v33(void *s, void *) { return func_ov064_02116374(s); }
static int __fastcall bbly_v34(void *s, void *) { return func_ov064_02116360(s); }
static int __fastcall bbly_v35(void *s, void *) { return func_ov064_0211635c(s); }
static int __fastcall bbly_v36(void *s, void *) { return func_ov064_02116348(s); }
/* BigBully shares Bully's Cleanup (slot 3 = 0x02116ca0) and OnAimedAtWithEgg
   (slot 29 = 0x02115f84), so its own object shape is identical -- reuse the
   bly_clean/bly_aimed thunks. Its D1/D0 are its OWN ROM bodies (same member
   layout, its own table stored): bbly_d1/bbly_d0. */
extern "C" void hal_fill_big_bully_vtable(void)
{
    void **vt = (void **)_ZTV8BigBully;
    ov64_fill_shared_0_30(vt);
    vt[0]  = (void *)bbly_init;
    vt[3]  = (void *)bly_clean;
    vt[6]  = (void *)bbly_behavior;
    vt[9]  = (void *)bbly_render;
    vt[16] = (void *)bbly_d1;
    vt[17] = (void *)bbly_d0;
    vt[29] = (void *)bly_aimed;
    vt[31] = (void *)bbly_v31;
    vt[32] = (void *)bbly_v32;
    vt[33] = (void *)bbly_v33;
    vt[34] = (void *)bbly_v34;
    vt[35] = (void *)bbly_v35;
    vt[36] = (void *)bbly_v36;
}

extern "C" void *port_factory_big_bully(void)
{
    void *p = BigBully_Spawn();
    if (p)
        *(void **)p = (void *)_ZTV8BigBully;
    return p;
}

// ============================================================================
// ROTATING_FIREBAR (81) -- a Platform, 32 slots
// ============================================================================
//
// The Platform base table has to be filled before RotatingFirebar_Spawn calls
// Platform's constructor and before the D-tors reseat _ZTV8Platform between the
// two member teardowns. hal_fill_platform_vtable owns that fill.
extern "C" void hal_fill_platform_vtable(void);

static int __fastcall fb_init(void *s, void *)
{ return _ZN15RotatingFirebar13InitResourcesEv(s); }
static int __fastcall fb_clean(void *s, void *)
{ return _ZN15RotatingFirebar16CleanupResourcesEv(s); }
static int __fastcall fb_behavior(void *s, void *)
{ return _ZN15RotatingFirebar8BehaviorEv(s); }
static int __fastcall fb_render(void *s, void *)
{ port_actor_render_probe("ROTATING_FIREBAR", (char *)s + 0xd4);
  return _ZN15RotatingFirebar6RenderEv(s); }
/* SLOT 16 IS LIVE: the D1/D0 are matched .c that spell _ZTV8Platform and
   data_020a0eac by name (both hosted), so the fill just calls them. */
static int __fastcall fb_d1(void *s, void *)
{ return (int)(size_t)_ZN15RotatingFirebarD1Ev(s); }
static int __fastcall fb_d0(void *s, void *)
{ return (int)(size_t)_ZN15RotatingFirebarD0Ev(s); }
static int __fastcall fb_aimed(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }   /* slot 29, Actor's own */
static int __fastcall fb_kill(void *s, void *)
{ _ZN8Platform4KillEv(s); return 0; }           /* slot 31, Platform's own */

extern "C" void hal_fill_rotating_firebar_vtable(void)
{
    void **vt = (void **)_ZTV15RotatingFirebar;
    hal_fill_platform_vtable();
    ov64_fill_shared_0_30(vt);
    vt[0]  = (void *)fb_init;
    vt[3]  = (void *)fb_clean;
    vt[6]  = (void *)fb_behavior;
    vt[9]  = (void *)fb_render;
    vt[16] = (void *)fb_d1;
    vt[17] = (void *)fb_d0;
    vt[29] = (void *)fb_aimed;
    vt[31] = (void *)fb_kill;
}

// ---- method faces ----------------------------------------------------------
// The C-named references the vtables take onto definitions that are real MSVC
// methods against include/. _ZN6Player4BurnEv (RotatingFirebar::Behavior's
// contact path) is NOT faced here -- gate 175's flame already defines it
// (hal/actor_classes_flame.cpp); this gate only relies on it.
#include "Bully.h"
#include "BigBully.h"
#include "RotatingFirebar.h"

/* hal/actor_slot30_seat.cpp -- the shared seat for vtable slot 30,
   Actor::OnAimedAtWithEggReturnVec. The ROM word in slot 30 of every vtable
   this file fills IS the arm9 base body 0x020100dc (checked against
   config/<module>/relocs.txt at vtable+30*4), and that body is now in the
   link from src/_ZN5Actor25OnAimedAtWithEggReturnVecEv.cpp on slice_gate50.
   The three-parameter __fastcall is the sret contract MSVC uses for a
   thiscall member returning a 12-byte struct: this in ecx, the hidden result
   pointer the one (callee-popped) stack argument. Same shape as whomp_s30. */
extern "C" void *__fastcall port_actor_s30_base(void *self, void *, void *out);
extern "C" {
int _ZN5Bully13InitResourcesEv(void *self)
{ ((Bully *)self)->Bully::InitResources(); return 1; }
int _ZN5Bully16CleanupResourcesEv(void *self)
{ return ((Bully *)self)->Bully::CleanupResources(); }
/* The three Renders are NOT faced here: each dispatches its model's slot 5
   through a ROM-order local shadow (the Whomp/Scuttlebug case), so the C
   names are host copies in port/unmatched/ModelAnim_Renders.cpp and the
   matched TUs are dropped from slice_gate177.txt. */
int _ZN5Bully6RenderEv(void *self);
int _ZN8BigBully13InitResourcesEv(void *self)
{ return ((BigBully *)self)->BigBully::InitResources(); }
int _ZN8BigBully8BehaviorEv(void *self)
{ return ((BigBully *)self)->BigBully::Behavior(); }
int _ZN8BigBully6RenderEv(void *self);
int _ZN15RotatingFirebar13InitResourcesEv(void *self)
{ return ((RotatingFirebar *)self)->RotatingFirebar::InitResources(); }
int _ZN15RotatingFirebar16CleanupResourcesEv(void *self)
{ return ((RotatingFirebar *)self)->RotatingFirebar::CleanupResources(); }
int _ZN15RotatingFirebar8BehaviorEv(void *self)
{ return ((RotatingFirebar *)self)->RotatingFirebar::Behavior(); }
int _ZN15RotatingFirebar6RenderEv(void *self);
}
