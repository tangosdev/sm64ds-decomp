// LANE SEAT-OV002-RUNTIME: four classes that are never placed by a level's
// object table and are only ever spawned at run time by code ALREADY in the
// link. A boot census never asks for them, which is why no earlier gate
// noticed they were unseated; a played frame does. The scoping doc
// (status/LINKSCOPE.md, lane 1) ranked them by that delta.
//
//   id  class             overlay  spawned by (all linked today)
//   254 FIREBALL          ov002    Actor::SpawnFireball (arm9), called by the
//                                  fire piranhas (func_ov084_0212ddbc), the
//                                  ov070 and ov063 spitters
//   281 BOWSER_SHOCKWAVES ov060    func_ov060_02115b0c, Bowser's stomp landing
//   291 BUBBLE            ov002    func_ov064_0211a39c, the treasure chest's
//                                  open state when the player is underwater
//   9   YOSHI_EGG         ov002    func_ov002_020d6368 / func_ov002_020d5ab4,
//                                  Player::St_Swallow_Main's egg lay
//
// Every own body seated here byte-matches under tools/match.py with strict
// relocs (the per-body table is status/SEATOV002.md). The four marker-carrying
// bodies this lane seats (Fireball's slot 18, Bubble's InitResources and
// Behavior, YoshiEgg's state-0 main) were each ruled REAL_DECOMP on that
// evidence in port/tools/inferred_stub_adjudicated.txt BEFORE being seated;
// the marker on them records how the NAME was recovered (all four are
// misnamed by dsd), not where the body came from.
//
// ALL FOUR TABLES ARE THE STANDARD 31-SLOT ACTOR TABLE, read slot by slot off
// the ROM's own relocations (config/arm9/overlays/ov002/relocs.txt and
// ov060/relocs.txt): slots 1,2,4,5,7,8,10,11 are Actor's Before/After pairs,
// 12 ActorBase::OnPendingDestroy, 13/14 Virtual34/38, 15 OnHeapCreated, and
// 18..30 the plain Actor interaction defaults, with these overrides:
//   FIREBALL          0 Init, 6 Behavior, 9 Render (a `return 1` stub), 16 D1,
//                     17 D0, 18 OnYoshiTryEat (own: func_ov002_020f92e4,
//                     returns 5 for the non-plain variants). Slot 3 is the
//                     inherited ActorBase::CleanupResources.
//   BOWSER_SHOCKWAVES 0 Init, 3 Cleanup, 6 Behavior, 9 Render (HOST COPY:
//                     the ModelAnim slot-5 collision on TWO ModelAnims,
//                     unmatched/Ov060_Renders.cpp), 16 D1, 17 D0.
//   BUBBLE            0 Init, 6 Behavior, 16 D1, 17 D0 (host thunk: the
//                     matched body spells its table and heap as the VT0/G0
//                     shadow globals). 3 and 9 are ActorBase's.
//   YOSHI_EGG         0 Init, 3 Cleanup, 6 Behavior, 9 Render (HOST COPY,
//                     ModelAnim slot-5, unmatched/ModelAnim_Renders.cpp),
//                     16 D1, 17 D0. Its 4-state PMF machine is seated in
//                     unmatched/YoshiEgg_StateDispatch.cpp.
//
// The three ov002 tables are MOUNTED (port/ov002_syms.txt, the gate-200
// shape) and overwritten slot for slot below before any spawn can dispatch
// them; the ov060 table is hosted as an array here (the ov060 mount excludes
// its vtables, port/ov060_syms.txt's rule). Each class's D0 spells its table
// by the RTTI name, aliased onto the seated storage.
#include <cstdio>
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

/* The Enemy tier's eight-entry death table (port/unmatched/Enemy_UpdateDeath):
   its statics carry DS code addresses until seated, and the two Enemy
   subclasses here (Fireball, YoshiEgg) inherit the base's death dispatch. */
void port_enemy_death_states_seat(void);

/* teardown members the host thunks spell */
void _ZN18MovingCylinderClsnD1Ev(void *self);
void *_ZN5ActorD2Ev(void *self);
void _ZN6Memory10DeallocateEPvP4Heap(void *, void *);
extern void *data_020a0eac;   /* the game heap */
}

// ---- the shared 1..30 half, the standard 31-slot Actor table --------------
static void rs_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: runtime_spawns vtable slot %d is not hosted (actor "
                 "id %u %s)\n", slot, id, port_actor_class_name(id));
    { static char _m[128];
      std::snprintf(_m, sizeof _m, "unhosted runtime_spawns vtable slot %d on "
                    "id %u %s", slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
static int __fastcall rs_trap13(void *s, void *) { rs_trap_report(s, 13); return 0; }
static int __fastcall rs_trap14(void *s, void *) { rs_trap_report(s, 14); return 0; }
static int __fastcall rs_trap30(void *s, void *) { rs_trap_report(s, 30); return 0; }

static int __fastcall rs_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall rs_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall rs_clean_base(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::CleanupResources(); }
static int __fastcall rs_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall rs_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall rs_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall rs_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall rs_render_base(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::Render(); }
static int __fastcall rs_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall rs_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall rs_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall rs_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall rs_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
static int __fastcall rs_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall rs_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall rs_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall rs_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall rs_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall rs_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall rs_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall rs_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall rs_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall rs_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall rs_aimed(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }

/* Fills slots 1,2,4,5,7,8,10,11,12,13,14,15,18..30 -- the shared half. The
   callers write their own 0/3/6/9/16/17 (and 18 where the class overrides
   it). THE POINTER IS VOLATILE ON PURPOSE: hal/actor_classes_ov002g200.cpp
   records the MSVC 19.44 /O2 miscompile that dropped every shared-half store
   from a static filler called from two sites; volatile stores are the cure it
   verified in the disassembly. */
static void rs_fill_shared(void *volatile *vt)
{
    vt[1]  = (void *)rs_binit;
    vt[2]  = (void *)rs_ainit;
    vt[4]  = (void *)rs_bclean;
    vt[5]  = (void *)rs_aclean;
    vt[7]  = (void *)rs_bbeh;
    vt[8]  = (void *)rs_abeh;
    vt[10] = (void *)rs_bren;
    vt[11] = (void *)rs_aren;
    vt[12] = (void *)rs_pdes;
    vt[13] = (void *)rs_trap13;
    vt[14] = (void *)rs_trap14;
    vt[15] = (void *)rs_heap;
    vt[18] = (void *)rs_yoshi;
    vt[19] = (void *)rs_egg;
    vt[20] = (void *)rs_v50;
    vt[21] = (void *)rs_pounded;
    vt[22] = (void *)rs_atk1;
    vt[23] = (void *)rs_atk2;
    vt[24] = (void *)rs_kicked;
    vt[25] = (void *)rs_pushed;
    vt[26] = (void *)rs_cannon;
    vt[27] = (void *)rs_mega;
    vt[28] = (void *)rs_under;
    vt[29] = (void *)rs_aimed;
    vt[30] = (void *)rs_trap30;
}

// ============================================================================
// FIREBALL (actor 254, ov002) -- _ZTV8Fireball / _ZTV12daFPknBall_c, 0x0210bf94
// ============================================================================
//
// The fireball a fire piranha spits (and the ov070/ov063 spitters). An Enemy
// build (EnemyC2, 888 bytes): MovingCylinderClsn at 0x110, WithMeshClsn at
// 0x144, ShadowModel at 0x300. Spawned only through Actor::SpawnFireball
// (arm9 0x02014... , src/_ZN5Actor13SpawnFireball*.c, linked since gate 10),
// which asks for 0xfe and writes the caller's params through the result.
//
// Every own slot is matched src: InitResources is a real C++ method (faced
// below), Behavior is C-named in its own TU, Render is the ROM's `return 1`
// stub, D1/D0 are flat C. Slot 18 is the class's own OnYoshiTryEat
// (func_ov002_020f92e4: 5 for the variants that burn, 0 for the plain one),
// ruled REAL_DECOMP by strict-reloc byte match. Its Behavior's three helpers
// (func_ov002_020f8b24 shadow/particles, 020f88ec coin drop, 020f897c the
// cap/coin drop on a wall hit) are on the slice; the three particle tables
// they index (data_ov002_02100320/334/348) joined the ov002 mount.
extern "C" {
int _ZN8Fireball13InitResourcesEv(void *self);    /* face below */
int _ZN8Fireball8BehaviorEv(void *self);          /* C-named in its own TU */
int _ZN8Fireball6RenderEv(void);                  /* `return 1` stub */
int *_ZN8FireballD1Ev(int *self);
int *_ZN8FireballD0Ev(int *self);
int func_ov002_020f92e4(char *self);              /* slot 18, own */
extern int _ZTV8Fireball[];                       /* ov002 mount, 31 slots */
}
/* The fireball's own D0 spells its table by the RTTI name. */
#pragma comment(linker, "/alternatename:__ZTV12daFPknBall_c=__ZTV8Fireball")

static int __fastcall fb_init(void *s, void *)
{ return _ZN8Fireball13InitResourcesEv(s); }
static int __fastcall fb_behavior(void *s, void *)
{ return _ZN8Fireball8BehaviorEv(s); }
static int __fastcall fb_render(void *s, void *)
{ (void)s; return _ZN8Fireball6RenderEv(); }
static int __fastcall fb_d1(void *s, void *)
{ return (int)(size_t)_ZN8FireballD1Ev((int *)s); }
static int __fastcall fb_d0(void *s, void *)
{ return (int)(size_t)_ZN8FireballD0Ev((int *)s); }
static int __fastcall fb_yoshi(void *s, void *)
{ return func_ov002_020f92e4((char *)s); }

extern "C" void hal_fill_fireball_vtable(void)
{
    port_enemy_death_states_seat();
    void *volatile *vt = (void *volatile *)_ZTV8Fireball;
    rs_fill_shared(vt);
    vt[0]  = (void *)fb_init;
    vt[3]  = (void *)rs_clean_base;   /* ROM slot 3: ActorBase::CleanupResources */
    vt[6]  = (void *)fb_behavior;
    vt[9]  = (void *)fb_render;
    vt[16] = (void *)fb_d1;
    vt[17] = (void *)fb_d0;
    vt[18] = (void *)fb_yoshi;
}

/* ---- method faces ---------------------------------------------------------
   The C-named references the tables take onto definitions that are real MSVC
   methods against include/. */
#include "Fireball.h"
extern "C" {
int _ZN8Fireball13InitResourcesEv(void *self)
{ return ((Fireball *)self)->Fireball::InitResources(); }
}
