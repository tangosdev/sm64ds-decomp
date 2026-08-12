// GATE 178: two more classes hosted out of ov064, Lethal Lava Land's actor
// overlay -- METAL_NET_LIFT (69) and LAVA_BUBBLE (214). Second mount of ov064
// (gate 177 was the first: BULLY/BIG_BULLY/ROTATING_FIREBAR).
//
// Same law as hal/actor_classes_ov064.cpp (gate 177): ROM slot order, __fastcall
// thunks that call the class's C body or a QUALIFIED base method, the SRET slot
// 30 trapped, the interaction tail bound to the Actor base bodies.
//
// ---- METAL_NET_LIFT (69) IS THE daObjFl_Amilift_c CLASS, NOT MetalNetLift ---
//
// The config naming is a decoy. ACTOR_SPAWN_TABLE[69] (arm9 0x02090978) points at
// MetalNetLift_SpawnInfo (ov064 0x0211bc44, +4 halfword 0x45 = 69); that record's
// +0 word is MetalNetLift_Spawn (0x02117fe8). MetalNetLift_Spawn installs
// _ZTV17daObjFl_Amilift_c -- which resolves to data_ov064_0211bc68 (the load site
// 0x0211801c inside MetalNetLift_Spawn relocates there), a 32-slot PLATFORM table
// whose own methods are the func_ov064_02117* family: Init func_ov064_02117e44,
// Cleanup func_ov064_02117cc4, Behavior func_ov064_02117d24, Render
// func_ov064_02117cfc, D1 func_ov064_02117978, D0 func_ov064_021179bc.
//
// The _ZN12MetalNetLift* symbols (_ZTV12MetalNetLift 0x0211bd2c, InitResources
// the ov002 cross-overlay veneer, D1/D0) belong to a DIFFERENT actor: id 72,
// TiltingPlatformLll_SpawnInfo (0x0211bd08, +4 = 0x48 = 72), whose factory
// TiltingPlatformLll_Spawn installs _ZTV12MetalNetLift. That class and its ov002
// veneer chain (func_ov002_020b6244/60fc/6144/616c/6074/62cc/6374) are NOT this
// gate's target -- a follow-up gate hosts id 72. The prior authoring attempt's
// "MetalNetLift's Render is func_ov064_02117cfc" was right for id 69; its
// "func_ov002_020b6144 is the id-72 table's render" was right too, and id 72 is
// simply not in this gate.
//
// The Amilift vtable data_ov064_0211bc68 is 32 slots (reloc run 0x0211bc68..
// 0x0211bce4, the next symbol data_ov064_0211bce8 begins one word past slot 31),
// the RotatingFirebar shape: slot 31 is _ZN8Platform4KillEv (ov002 0x020ee55c,
// module:overlays(2,7), already hosted), slots 18..29 are the Actor base defaults
// (0x02010160..0x02010124), slot 30 is the SRET OnAimedAtWithEggReturnVec (TRAP).
// It carries the vtable signature (+4 relocates to Actor::BeforeInitResources,
// arm9 0x02011268), so it is a HOST array this file fills, spelled by its RTTI
// name _ZTV17daObjFl_Amilift_c (the montymole reading, int[] to match
// decl_common.h). The factory's LAST vptr write is that table (line 14 of
// MetalNetLift_Spawn.c), then _ZN7PathPtrC1Ev at +0x360 which is not a vptr
// write -- so the factory leaves the REAL table installed and needs NO reseat
// wrapper (unlike gate 177's bullies).
//
// ---- LAVA_BUBBLE (214) IS AN ENEMY, 31 SLOTS ------------------------------
//
// ACTOR_SPAWN_TABLE[214] (arm9 0x02090bbc) -> LavaBubble_SpawnInfo (0x0211bec8,
// +4 = 0xd6 = 214); +0 = LavaBubble_Spawn (0x02118b10), which installs
// _ZTV10LavaBubble (0x0211beec, also spelled _ZTV7daBbl_c). 31 slots (reloc run
// 0x0211beec..0x0211bf64, next symbol one word past slot 30). Its own overrides
// are 0/3/6/9/12/16/17/18; the rest are base defaults, slot 30 the SRET TRAP.
// Its D1 spells _ZTV10LavaBubble and D0 spells _ZTV7daBbl_c (the SAME address,
// the class's own table) plus func_ov002_020aed18 (the Enemy base D2) and
// data_020a0eac (the game heap) -- all hosted -- so BOTH D1/D0 stay in the slice
// and the fill just calls them (the RotatingFirebar treatment). Its factory's
// last vptr write is _ZTV10LavaBubble, then the two collider ctors -- real table
// installed, no reseat wrapper.
//
// ---- THE AMILIFT D1/D0 ARE HOST THUNKS -------------------------------------
//
// func_ov064_02117978 (D1, slot 16) and func_ov064_021179bc (D0, slot 17, whose
// recovered "OnYoshiTryEat" name is wrong -- it is the D0, dtor chain then
// Memory::Deallocate) both spell the SHARED single-global placeholder tables:
// D1 stores _ZTV17daObjFl_Amilift_c then _ZTV10dBgActor_c (196 TUs spell that one
// name for 196 different tables), D0 stores VT0 then VT1 (the shared int[]
// placeholders in decl_common.h). One host definition of any of them would
// satisfy every speller with the wrong bytes and nothing would say so (the
// CastleWater/SphereClsn reading, gate 177's bully treatment). So both are NOT in
// the slice; the thunks below run the matched chain with the derived table stored
// once. Members are destroyed high-address first: MovingMeshCollider +0x124,
// Model +0xd4, then the Actor base D2. D0 also frees on the game heap.
//
// ---- THE TWO PMF STATE MACHINES --------------------------------------------
//
// Both classes drive a source-side PMF table copied into bss by a sinit, then
// dispatched with mwcc's `(obj->*pmf)()` -- the Scuttlebug/MontyMole case. On
// MSVC that call is __thiscall (this in ECX) against a plain extern-C body that
// wants self on the stack: calling-convention corruption. So each dispatcher is a
// host copy (port/unmatched/Ov064Gate178_States.cpp) that reads the record as a
// plain { fn, delta } and calls fn with `this`, and the source PMF blocks are
// seated (their code-address words rewritten to host bodies) BEFORE the sinit
// copies them. The four/three state bodies themselves are plain matched src in
// the slice.
//   - Amilift Behavior func_ov064_02117d24 dispatches data_ov064_0211c750[idx]
//     (3 records), seeded by __sinit_ov064_0211afc0 from data_ov064_0211bc0c/14/1c.
//   - LavaBubble seeder func_ov064_021187ec + Behavior _ZN10LavaBubble8BehaviorEv
//     dispatch data_ov064_0211c7b8/c7c8 (2 records each), seeded by
//     __sinit_ov064_0211b150 from data_ov064_0211be90/98/a0/a8.
//
// ---- THE RENDER COLLISION --------------------------------------------------
//
// Amilift Render func_ov064_02117cfc is the ROM-order model slot-5 dispatch (a
// six-virtual local shadow, m = slot 5, Model at +0xd4) -- the Whomp/Scuttlebug
// collision -- so it is a host copy in unmatched/Ov064Gate178_States.cpp and out
// of the slice. LavaBubble Render (_ZN10LavaBubble6RenderEv) is a bare `return 1`
// (no model, no shadow) and stays in the slice.
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
  void port_actor_slot_decline(const char *what);  /* func_02043fdc.cpp: per-actor decline */
void port_actor_render_probe(const char *cls, void *model); /* actor_classes */

/* ---- METAL_NET_LIFT (69) / daObjFl_Amilift_c ---- */
int func_ov064_02117e44(void *self);   /* slot 0, Init */
int func_ov064_02117cc4(void *self);   /* slot 3, Cleanup */
int func_ov064_02117cfc(void *self);   /* slot 9, Render -- HOST COPY, see below */
void *MetalNetLift_Spawn(void);        /* the factory, installs the host table */
/* Behavior func_ov064_02117d24 is a HOST COPY (PMF dispatch) -- declared in the
   states file; the fill binds the host copy at slot 6. */
int func_ov064_02117d24(void *self);
/* D1/D0 (func_ov064_02117978/021179bc) are NOT declared -- host thunks below.
   The Amilift dtor chain's sub-object destructors and base D2, all C-linkage. */
void _ZN18MovingMeshColliderD1Ev(void *);   /* MovingMeshCollider at +0x124 */
void _ZN5ModelD1Ev(void *);                 /* Model at +0xd4 */
void _ZN5ActorD2Ev(void *);                 /* the Actor base D2 */
void _ZN6Memory10DeallocateEPvP4Heap(void *, void *);
extern void *data_020a0eac;                 /* Memory::gameHeapPtr (== G0) */
void _ZN8Platform4KillEv(void *self);       /* slot 31, Platform's own */

/* ---- LAVA_BUBBLE (214) / daBbl_c ---- */
int _ZN10LavaBubble13InitResourcesEv(void *self);    /* slot 0, .cpp method, faced */
int _ZN10LavaBubble16CleanupResourcesEv(void *self); /* slot 3, .c C linkage */
int _ZN10LavaBubble6RenderEv(void *self);            /* slot 9, .c `return 1` */
int _ZN10LavaBubble16OnPendingDestroyEv(void *self); /* slot 12, .c */
int *_ZN10LavaBubbleD1Ev(void *self);                /* slot 16, .c named tables */
int *_ZN10LavaBubbleD0Ev(void *self);                /* slot 17, .c named tables */
int func_ov064_02118b08(void);                       /* slot 18, OnYoshiTryEat, returns 5 */
void *LavaBubble_Spawn(void);                        /* the factory */
/* Behavior _ZN10LavaBubble8BehaviorEv is a HOST COPY (PMF dispatch) -- declared
   in the states file; the fill binds the host copy at slot 6. */
int _ZN10LavaBubble8BehaviorEv(void *self);

/* the two derived vtables, HOST arrays this file fills; 32/31 slots. Defined
   int[] with C linkage to match the extern int _ZTV*[] spellings the factories
   and dtors read (the montymole reading). The Amilift table is spelled only by
   its RTTI wildcard _ZTV17daObjFl_Amilift_c (decl_common.h, int[]). */
int _ZTV17daObjFl_Amilift_c[32];   /* vtspan: data_ov064_0211bc68 */
int _ZTV10LavaBubble[31];
}

/* LavaBubble's D0 spells _ZTV7daBbl_c, the RTTI wildcard for the SAME table as
   _ZTV10LavaBubble (both ov064 0x0211beec). Point it at the host array. */
#pragma comment(linker, "/alternatename:__ZTV7daBbl_c=__ZTV10LavaBubble")

/* The .cpp slice TUs spell five mounted ov064 data symbols as typed C++
   globals (SharedFilePtr / CLPS_Block / char[]), which MSVC mangles; the mount
   defines them under the C names. The standard bridge (montymole/hmc). */
#pragma comment(linker, "/alternatename:?data_ov064_0211c728@@3USharedFilePtr@@A=_data_ov064_0211c728")
#pragma comment(linker, "/alternatename:?data_ov064_0211c730@@3USharedFilePtr@@A=_data_ov064_0211c730")
#pragma comment(linker, "/alternatename:?data_ov064_0211bb6c@@3UCLPS_Block@@A=_data_ov064_0211bb6c")
#pragma comment(linker, "/alternatename:?data_ov064_0211c7b8@@3PADA=_data_ov064_0211c7b8")
#pragma comment(linker, "/alternatename:?data_ov064_0211c7c8@@3PADA=_data_ov064_0211c7c8")

// ---- the trap --------------------------------------------------------------
static void ov64g178_trap_report(void *self, int slot)
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
#define OV64G178_TRAP(n) \
    static int __fastcall ov64g178_trap##n(void *s, void *) \
    { ov64g178_trap_report(s, n); return 0; }
/* 13/14 are ActorBase::Virtual34/38 (not linked, the sibling trap); 30 is the
   SRET OnAimedAtWithEggReturnVec no thunk shape models. */
OV64G178_TRAP(13) OV64G178_TRAP(14) OV64G178_TRAP(30)
#undef OV64G178_TRAP

// ---- the shared 0..30 half -------------------------------------------------
static int __fastcall ov64g178_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall ov64g178_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall ov64g178_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall ov64g178_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall ov64g178_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall ov64g178_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall ov64g178_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall ov64g178_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall ov64g178_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall ov64g178_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall ov64g178_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
static int __fastcall ov64g178_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall ov64g178_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall ov64g178_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall ov64g178_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall ov64g178_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall ov64g178_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall ov64g178_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall ov64g178_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall ov64g178_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall ov64g178_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall ov64g178_aimed_actor(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }   /* slot 29, Actor's own default */

/* Fill slots 1..30 of a Platform/Enemy 31/32-slot table with the shared bodies.
   The caller writes 0/3/6/9/16/17/(18)/(31). */
static void ov64g178_fill_shared_0_30(void **vt)
{
    vt[1]  = (void *)ov64g178_binit;
    vt[2]  = (void *)ov64g178_ainit;
    vt[4]  = (void *)ov64g178_bclean;
    vt[5]  = (void *)ov64g178_aclean;
    vt[7]  = (void *)ov64g178_bbeh;
    vt[8]  = (void *)ov64g178_abeh;
    vt[10] = (void *)ov64g178_bren;
    vt[11] = (void *)ov64g178_aren;
    vt[12] = (void *)ov64g178_pdes;
    vt[13] = (void *)ov64g178_trap13;
    vt[14] = (void *)ov64g178_trap14;
    vt[15] = (void *)ov64g178_heap;
    vt[18] = (void *)ov64g178_yoshi;
    vt[19] = (void *)ov64g178_egg;
    vt[20] = (void *)ov64g178_v50;
    vt[21] = (void *)ov64g178_pounded;
    vt[22] = (void *)ov64g178_atk1;
    vt[23] = (void *)ov64g178_atk2;
    vt[24] = (void *)ov64g178_kicked;
    vt[25] = (void *)ov64g178_pushed;
    vt[26] = (void *)ov64g178_cannon;
    vt[27] = (void *)ov64g178_mega;
    vt[28] = (void *)ov64g178_under;
    vt[29] = (void *)ov64g178_aimed_actor;
    vt[30] = (void *)ov64g178_trap30;
}

// ============================================================================
// METAL_NET_LIFT (69) -- the daObjFl_Amilift_c Platform, 32 slots
// ============================================================================
//
// The Platform base table has to be filled before MetalNetLift_Spawn calls
// Platform's constructor and before the host D-thunks reseat between the member
// teardowns. hal_fill_platform_vtable owns that fill (gate 177 already relies on
// it for RotatingFirebar).
extern "C" void hal_fill_platform_vtable(void);

static int __fastcall aml_init(void *s, void *)
{ return func_ov064_02117e44(s); }
static int __fastcall aml_clean(void *s, void *)
{ return func_ov064_02117cc4(s); }
static int __fastcall aml_behavior(void *s, void *)
{ return func_ov064_02117d24(s); }        /* HOST COPY (PMF dispatch) */
static int __fastcall aml_render(void *s, void *)
{ port_actor_render_probe("METAL_NET_LIFT", (char *)s + 0xd4);
  return func_ov064_02117cfc(s); }          /* HOST COPY (slot-5 collision) */
static int __fastcall aml_kill(void *s, void *)
{ _ZN8Platform4KillEv(s); return 0; }       /* slot 31, Platform's own */
/* D1/D0 host thunks: run the matched teardown with the derived table stored
   once, the shared placeholder elided. High-address-first: MovingMeshCollider
   +0x124, Model +0xd4, then the Actor base D2. D0 also frees on the game heap
   (D1's caller frees itself after the dispatch, so D1 stops before Deallocate). */
static int __fastcall aml_d1(void *s, void *)
{
    char *t = (char *)s;
    *(void **)t = (void *)_ZTV17daObjFl_Amilift_c;
    _ZN18MovingMeshColliderD1Ev(t + 0x124);
    _ZN5ModelD1Ev(t + 0xd4);
    _ZN5ActorD2Ev(t);
    return (int)(size_t)s;
}
static int __fastcall aml_d0(void *s, void *)
{
    char *t = (char *)s;
    *(void **)t = (void *)_ZTV17daObjFl_Amilift_c;
    _ZN18MovingMeshColliderD1Ev(t + 0x124);
    _ZN5ModelD1Ev(t + 0xd4);
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return (int)(size_t)s;
}

extern "C" void hal_fill_metal_net_lift_vtable(void)
{
    void **vt = (void **)_ZTV17daObjFl_Amilift_c;
    hal_fill_platform_vtable();
    ov64g178_fill_shared_0_30(vt);
    vt[0]  = (void *)aml_init;
    vt[3]  = (void *)aml_clean;
    vt[6]  = (void *)aml_behavior;
    vt[9]  = (void *)aml_render;
    vt[16] = (void *)aml_d1;
    vt[17] = (void *)aml_d0;
    vt[31] = (void *)aml_kill;
}

// ============================================================================
// LAVA_BUBBLE (214) -- an Enemy, 31 slots
// ============================================================================
static int __fastcall lb_init(void *s, void *)
{ return _ZN10LavaBubble13InitResourcesEv(s); }
static int __fastcall lb_clean(void *s, void *)
{ return _ZN10LavaBubble16CleanupResourcesEv(s); }
static int __fastcall lb_behavior(void *s, void *)
{ return _ZN10LavaBubble8BehaviorEv(s); }   /* HOST COPY (PMF dispatch) */
static int __fastcall lb_render(void *s, void *)
{ return _ZN10LavaBubble6RenderEv(s); }      /* bare return 1, no model */
static int __fastcall lb_pdes(void *s, void *)
{ _ZN10LavaBubble16OnPendingDestroyEv(s); return 0; }
static int __fastcall lb_yoshi(void *s, void *)
{ return func_ov064_02118b08(); }            /* slot 18, LavaBubble's own, returns 5 */
/* D1/D0 stay in the slice: they spell _ZTV10LavaBubble / _ZTV7daBbl_c and
   func_ov002_020aed18 / data_020a0eac, all hosted. The fill just calls them. */
static int __fastcall lb_d1(void *s, void *)
{ return (int)(size_t)_ZN10LavaBubbleD1Ev(s); }
static int __fastcall lb_d0(void *s, void *)
{ return (int)(size_t)_ZN10LavaBubbleD0Ev(s); }

extern "C" void hal_fill_lava_bubble_vtable(void)
{
    void **vt = (void **)_ZTV10LavaBubble;
    ov64g178_fill_shared_0_30(vt);
    vt[0]  = (void *)lb_init;
    vt[3]  = (void *)lb_clean;
    vt[6]  = (void *)lb_behavior;
    vt[9]  = (void *)lb_render;
    vt[12] = (void *)lb_pdes;
    vt[16] = (void *)lb_d1;
    vt[17] = (void *)lb_d0;
    vt[18] = (void *)lb_yoshi;   /* LavaBubble's own OnYoshiTryEat override */
}

// ---- method faces ----------------------------------------------------------
// LavaBubble::InitResources is a real MSVC .cpp method; face its C name onto the
// qualified method against include/LavaBubble.h. Its Behavior is a host copy (in
// the states file) and its Render/Cleanup/OnPendingDestroy/D1/D0 are .c bodies
// with C linkage already, so no face for those. The Amilift methods are all
// func_ov064_* C bodies -- no face. func_ov064_02117cfc (Render) is a host copy.
#include "LavaBubble.h"
extern "C" {
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN10LavaBubble13InitResourcesEv(void *self)
{ return ((LavaBubble *)self)->LavaBubble::InitResources(); }
#else
int _ZN10LavaBubble13InitResourcesEv(void *self);  /* Linux: real symbol from src/_ZN10LavaBubble13InitResourcesEv */
#endif /* _WIN32 */
}
