// GATE 199: SWITCH_PILLAR (34) + BASEMENT_WATER (35), ov012 (level 4,
// castle basement). Per-symbol ov012 mount (port/ov012_syms.txt). Full
// slot derivation, the mandatory window-alias check (0x02112320/
// 0x021123e4, twelve overlays sharing this DS window) and the SwitchPillar/
// BasementWater class-identity swap are all documented in
// port/ov012_syms.txt's own header.
//
// BOTH CLASSES ARE 32-SLOT PLATFORM, no own Kill (slot 31 uses the shared
// 0x020ee55c default for both -- confirmed by reloc target, neither table
// overrides it).
//
// D1/D0 for BOTH classes are host thunks (store own table, then overwrite
// with the shared _ZTV10dBgActor_c teardown placeholder, the ShipUp/
// RockPillar/SkiLift shape) -- dropped from slice_gate199.txt, byte-locked
// matched-src proof only.
#include <cstdio>
#include "dsstate_seg.h"
#include <cstdlib>

#include "Actor.h"
#include "ActorBase.h"

extern "C" {
int _ZN5Actor19BeforeInitResourcesEv(void *self);             /* slot 1  */
void _ZN5Actor18AfterInitResourcesEj(void *self, unsigned a);  /* slot 2  */
int _ZN5Actor14BeforeBehaviorEv(void *self);                  /* slot 7  */
int _ZN5Actor12BeforeRenderEv(void *self);                    /* slot 10 */
int _ZN5Actor13OnYoshiTryEatEv(void *self);                   /* slot 18 */
void _ZN5Actor13OnTurnIntoEggER6Player(void *self, void *p);  /* slot 19 */
int _ZN5Actor9Virtual50Ev(void *self);                        /* slot 20 */
void _ZN5Actor15OnGroundPoundedERS_(void *self, void *o);     /* slot 21 */
void _ZN5Actor11OnAttacked1ERS_(void *self, void *o);         /* slot 22 */
void _ZN5Actor11OnAttacked2ERS_(void *self, void *o);         /* slot 23 */
void _ZN5Actor8OnKickedERS_(void *self, void *o);             /* slot 24 */
void _ZN5Actor8OnPushedERS_(void *self, void *o);             /* slot 25 */
void _ZN5Actor24OnHitByCannonBlastedCharERS_(void *self, void *o); /* slot 26 */
void _ZN5Actor15OnHitByMegaCharER6Player(void *self, void *p);     /* slot 27 */
void _ZN5Actor19OnHitFromUnderneathERS_(void *self, void *o); /* slot 28 */
int _ZN5Actor16OnAimedAtWithEggEv(void *self);                /* slot 29 */

const char *port_actor_class_name(unsigned id);   /* hal/actor_registry */
void port_actor_slot_decline(const char *what);   /* func_02043fdc_hostcopy.cpp */
void port_actor_render_probe(const char *cls, void *model); /* hal/actor_classes */
void hal_fill_platform_vtable(void);              /* hal/actor_classes.cpp, Platform's
                                                       own base table */
extern int _ZTV8Platform[];

/* SWITCH_PILLAR (daObjC0_Switch_c) own bodies -- unnamed by dsd, matched src */
int func_ov012_02111370(char *self);       /* slot 0, InitResources */
int func_ov012_021112ec(void *self);       /* slot 3, CleanupResources */
int func_ov012_0211134c(void *self);       /* slot 6, Behavior */
int func_ov012_02111324(void *self);       /* slot 9, Render -- HOST COPY,
                                               unmatched/ModelAnim_Renders.cpp */
void func_ov012_0211123c(char *self);      /* slot 21, own OnGroundPounded */
void *SwitchPillar_Spawn(void);            /* installs data_ov012_02112344 */
DSSTATE_BEGIN
int data_ov012_02112344[32];               /* SwitchPillar's own vtable, unnamed by
                                               dsd -- host array */
DSSTATE_END

/* BASEMENT_WATER (daObjC0Water_c) own bodies -- misnamed _ZN12SwitchPillar*
   by dsd (the class-identity swap), matched src under those filenames */
int _ZN12SwitchPillar13InitResourcesEv(void *self);      /* slot 0, faced (real C++ method) */
int *_ZN12SwitchPillarD1Ev(int *self);                   /* slot 16, .c, DTOR-PAIRS seat (0x02111450) */
int *_ZN12SwitchPillarD0Ev(int *self);                   /* slot 17, .c, DTOR-PAIRS seat (0x0211149c) */
int _ZN12SwitchPillar16CleanupResourcesEv(void *self);   /* slot 3 */
int _ZN12SwitchPillar8BehaviorEv(void *self);            /* slot 6, faced (real C++ method) */
int _ZN12SwitchPillar6RenderEv(void *self);              /* slot 9, HOST COPY (the ModelAnim
                                                              slot-5 collision, TWO of them in
                                                              this file -- ModelAnim_Renders.cpp) */
void *BasementWater_Spawn(void);           /* installs _ZTV12SwitchPillar == _ZTV14daObjC0Water_c */
int _ZTV12SwitchPillar[32];

/* D1/D0 teardown bases both classes' host thunks call directly (the
   SkiLift/gate-191 shape): neither matched D1/D0 TU is compiled (both
   store their own table then OVERWRITE it with the shared _ZTV10dBgActor_c
   placeholder -- dropped from slice_gate199.txt, byte-locked proof only).
   The thunks below spell the SAME member-teardown chain the matched src
   bodies read, high-address member first, then Actor's own D2. */
void _ZN18MovingMeshColliderD1Ev(void *self);
void _ZN5ModelD1Ev(void *self);
void *_ZN5ActorD2Ev(void *self);
void _ZN18TextureTransformerD1Ev(void *self);
void _ZN6Memory10DeallocateEPvP4Heap(void *, void *);
extern void *data_020a0eac;   /* the game heap, already hosted (== G0 the
                                  matched src's own D0 bodies extern) */
}
#pragma comment(linker, "/alternatename:__ZTV14daObjC0Water_c=__ZTV12SwitchPillar")

// ---- the shared 1..30 half, both 32-slot Platform tables share it --------
static void ov12_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: ov012 vtable slot %d is not hosted (actor id %u "
                 "%s)\n", slot, id, port_actor_class_name(id));
    { static char _m[128];
      std::snprintf(_m, sizeof _m, "unhosted ov012 vtable slot %d on id %u %s",
                    slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
static int __fastcall ov12_trap13(void *s, void *) { ov12_trap_report(s, 13); return 0; }
static int __fastcall ov12_trap14(void *s, void *) { ov12_trap_report(s, 14); return 0; }

static int __fastcall ov12_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall ov12_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall ov12_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall ov12_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall ov12_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall ov12_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall ov12_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall ov12_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall ov12_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall ov12_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
static int __fastcall ov12_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall ov12_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall ov12_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall ov12_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall ov12_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall ov12_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall ov12_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall ov12_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall ov12_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall ov12_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall ov12_aimed(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }
static int __fastcall ov12_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }

/* Fills slots 1,2,4,5,7,8,10,11,13,14,15,18..30 -- the standard 32-slot
   Platform shared half. Slot 21 is overwritten again by SwitchPillar (own
   OnGroundPounded); BasementWater uses the shared default. Slot 31 (Kill)
   is NOT filled here -- neither class overrides it, so the caller must
   seat the Platform base's own default separately (hal_fill_platform_vtable
   installs it once into _ZTV8Platform; this file copies that one word). */
static void ov12_fill_shared(void **vt)
{
    vt[1]  = (void *)ov12_binit;
    vt[2]  = (void *)ov12_ainit;
    vt[4]  = (void *)ov12_bclean;
    vt[5]  = (void *)ov12_aclean;
    vt[7]  = (void *)ov12_bbeh;
    vt[8]  = (void *)ov12_abeh;
    vt[10] = (void *)ov12_bren;
    vt[11] = (void *)ov12_aren;
    vt[12] = (void *)ov12_pdes;
    vt[13] = (void *)ov12_trap13;
    vt[14] = (void *)ov12_trap14;
    vt[15] = (void *)ov12_heap;
    vt[18] = (void *)ov12_yoshi;
    vt[19] = (void *)ov12_egg;
    vt[20] = (void *)ov12_v50;
    vt[21] = (void *)ov12_pounded;
    vt[22] = (void *)ov12_atk1;
    vt[23] = (void *)ov12_atk2;
    vt[24] = (void *)ov12_kicked;
    vt[25] = (void *)ov12_pushed;
    vt[26] = (void *)ov12_cannon;
    vt[27] = (void *)ov12_mega;
    vt[28] = (void *)ov12_under;
    vt[29] = (void *)ov12_aimed;
    vt[30] = (void *)port_actor_s30_base;
    /* the shared Platform::Kill default, COPIED out of the base table rather
       than named. ORDERING IS LOAD-BEARING: both callers run
       hal_fill_platform_vtable first, which is what puts a real thunk at
       _ZTV8Platform[31]. It was a [20] array until the vtable-sizing pass, so
       this read was out of bounds and installed whatever followed as slot 31
       of two ov012 tables; it yields the real thunk now. A future reordering
       that fills these before Platform would silently reinstall a null. */
    vt[31] = (void *)_ZTV8Platform[31];
}

// ============================================================================
// SWITCH_PILLAR (34), 32 slots. Own overrides: 0 Init, 3 Cleanup, 6
// Behavior, 9 Render, 16 D1, 17 D0, 21 OnGroundPounded (own, overrides the
// shared default -- reloc-confirmed).
// ============================================================================
static int __fastcall sp_init(void *s, void *)
{ return func_ov012_02111370((char *)s); }
static int __fastcall sp_clean(void *s, void *)
{ return func_ov012_021112ec(s); }
static int __fastcall sp_behavior(void *s, void *)
{ return func_ov012_0211134c(s); }
static int __fastcall sp_render(void *s, void *)
{ port_actor_render_probe("SWITCH_PILLAR", (char *)s + 0xd4);
  return func_ov012_02111324(s); }
/* D1/D0 host thunks (SwitchPillar's own): the matched src
   (func_ov012_021111a0.c/021111e4.c) stores data_ov012_02112344 (own
   table) then OVERWRITES with the shared _ZTV10dBgActor_c placeholder --
   dropped from the slice. Run the member chain high-address first:
   MovingMeshCollider +0x124, Model +0xd4, then Actor's own D2. D0 also
   frees on the game heap (matched src's own `extern void *G0` ==
   data_020a0eac). */
static int __fastcall sp_d1(void *s, void *)
{
    char *t = (char *)s;
    _ZN18MovingMeshColliderD1Ev(t + 0x124);
    _ZN5ModelD1Ev(t + 0xd4);
    _ZN5ActorD2Ev(t);
    return (int)(size_t)s;
}
static int __fastcall sp_d0(void *s, void *)
{
    char *t = (char *)s;
    _ZN18MovingMeshColliderD1Ev(t + 0x124);
    _ZN5ModelD1Ev(t + 0xd4);
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return (int)(size_t)s;
}
static int __fastcall sp_pounded(void *s, void *, void *o)
{ func_ov012_0211123c((char *)s); (void)o; return 0; }

extern "C" void hal_fill_switch_pillar_vtable(void)
{
    hal_fill_platform_vtable();
    void **vt = (void **)data_ov012_02112344;
    ov12_fill_shared(vt);
    vt[0]  = (void *)sp_init;
    vt[3]  = (void *)sp_clean;
    vt[6]  = (void *)sp_behavior;
    vt[9]  = (void *)sp_render;
    vt[16] = (void *)sp_d1;
    vt[17] = (void *)sp_d0;
    vt[21] = (void *)sp_pounded;   /* own OnGroundPounded, overrides the shared default */
}

// ============================================================================
// BASEMENT_WATER (35), 32 slots. Own overrides: 0 Init, 3 Cleanup, 6
// Behavior, 9 Render, 16 D1, 17 D0. No slot-21 override -- confirmed by the
// raw vtable dump (word 21 matches the shared arm9 default byte for byte).
// ============================================================================
static int __fastcall bw_init(void *s, void *)
{ return _ZN12SwitchPillar13InitResourcesEv(s); }
static int __fastcall bw_clean(void *s, void *)
{ return _ZN12SwitchPillar16CleanupResourcesEv(s); }
static int __fastcall bw_behavior(void *s, void *)
{ return _ZN12SwitchPillar8BehaviorEv(s); }
static int __fastcall bw_render(void *s, void *)
{ port_actor_render_probe("BASEMENT_WATER", (char *)s + 0xd4);
  return _ZN12SwitchPillar6RenderEv(s); }
/* D1/D0 (DTOR-PAIRS seat): BasementWater's own matched pair, the src files
   spelled _ZN12SwitchPillarD1Ev.c/D0Ev.c per the class-identity swap, behind
   ecx->arg adapters where host copies of the chain stood. The ROM's table at
   0x02112408 puts them at words 16/17; their first store is that table by its
   RTTI name (_ZTV14daObjC0Water_c, aliased above) and the second is ov002
   0x0210ae38 by relocation, the one Platform base table the port hosts
   (hal/lk2_platform_dtor_seat.cpp) -- not a placeholder. */
static int __fastcall bw_d1(void *s, void *)
{ return (int)(size_t)_ZN12SwitchPillarD1Ev((int *)s); }
static int __fastcall bw_d0(void *s, void *)
{ return (int)(size_t)_ZN12SwitchPillarD0Ev((int *)s); }

extern "C" void hal_fill_basement_water_vtable(void)
{
    hal_fill_platform_vtable();
    void **vt = (void **)_ZTV12SwitchPillar;
    ov12_fill_shared(vt);
    vt[0]  = (void *)bw_init;
    vt[3]  = (void *)bw_clean;
    vt[6]  = (void *)bw_behavior;
    vt[9]  = (void *)bw_render;
    vt[16] = (void *)bw_d1;
    vt[17] = (void *)bw_d0;
    /* no slot 21 override: BasementWater uses the shared default */
}

// ---- method faces --------------------------------------------------------
// _ZN12SwitchPillar13InitResourcesEv/8BehaviorEv (BasementWater's real
// methods, misnamed by dsd) are real MSVC methods against SwitchPillar.h
// (?InitResources@SwitchPillar@@..., not __ZN12SwitchPillar...) -- faced
// here, the IceSheet/OneUpLogo/BabyPenguin recipe. CleanupResources is also
// a real C++ method per its own file (a .cpp), faced the same way. Render
// is NOT faced here: it is a host copy (the ModelAnim slot-5 collision,
// port/unmatched/ModelAnim_Renders.cpp), declared extern "C" above.
#include "SwitchPillar.h"

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
int _ZN12SwitchPillar13InitResourcesEv(void *self)
{ return ((SwitchPillar *)self)->SwitchPillar::InitResources(); }
int _ZN12SwitchPillar16CleanupResourcesEv(void *self)
{ return ((SwitchPillar *)self)->SwitchPillar::CleanupResources(); }
int _ZN12SwitchPillar8BehaviorEv(void *self)
{ return ((SwitchPillar *)self)->SwitchPillar::Behavior(); }
}
