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
    vt[30] = (void *)port_actor_s30_base;
}


/* SM64DS_RS_PROBE=1: one line per Behavior tick for this lane's classes --
   position and a class-chosen word -- so a headless run can show an actor
   MOVING, not merely surviving (the Klepto lesson: a quarantined actor also
   "survives" a census). Off by default, prints nothing. */
static void rs_probe(const char *cls, void *s, int word)
{
    static int on = -1;
    if (on < 0) on = std::getenv("SM64DS_RS_PROBE") != 0;
    if (!on) return;
    char *c = (char *)s;
    std::printf("[rsprobe] %s %p pos (%d,%d,%d) word %d\n", cls, s,
                *(int *)(c + 0x5c) >> 12, *(int *)(c + 0x60) >> 12,
                *(int *)(c + 0x64) >> 12, word);
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

// ============================================================================
// BOWSER_SHOCKWAVES (actor 281, ov060) -- _ZTV16BowserShockwaves / _ZTV9daFRing_c
// ============================================================================
//
// The two expanding rings Bowser's stomp sends across the arena floor. A
// plain Actor build (ActorC2, 536 bytes) carrying two of everything: ModelAnim
// at 0xd4 and 0x174, TextureSequence at 0x138/0x1d8, MaterialChanger at
// 0x14c/0x1ec, TextureTransformer at 0x160/0x200. Spawned only by
// func_ov060_02115b0c (Bowser's landing state, +0x414 == 2, linked since wave
// 5), which asks for 0x119 and ignores the result.
//
// The table 0x0211ab54 is hosted as an array here (port/ov060_syms.txt
// excludes the seven ov060 vtables). Own slots: 0 Init (real C++ method,
// faced), 3 Cleanup (three SharedFilePtr releases, ignores this), 6 Behavior
// (faced), 9 Render (HOST COPY: the ModelAnim slot-5 collision on two
// ModelAnims, unmatched/Ov060_Renders.cpp), 16/17 the flat-C D1/D0. Slots
// 18..30 are the plain Actor defaults per the ROM's reloc run.
//
// InitResources reaches two BMA files as `func_021115e4` / `func_021115f4`,
// dsd's code-flavoured spelling of two level-overlay-window addresses; for
// the mounted koopa2_boss arena the owner is ov046, so both are aliased onto
// the ov046 per-symbol mount (the arena-CLPS treatment in actor_classes_ov060).
extern "C" {
int _ZN16BowserShockwaves13InitResourcesEv(void *self);   /* face below */
int _ZN16BowserShockwaves16CleanupResourcesEv(void);      /* three releases */
int _ZN16BowserShockwaves8BehaviorEv(void *self);         /* face below */
int _ZN16BowserShockwaves6RenderEv(void *self);           /* host copy */
int *_ZN16BowserShockwavesD1Ev(int *self);
int *_ZN16BowserShockwavesD0Ev(int *self);
void *_ZTV16BowserShockwaves[31];
}
#pragma comment(linker, "/alternatename:__ZTV9daFRing_c=__ZTV16BowserShockwaves")
#pragma comment(linker, "/alternatename:_func_021115e4=_data_ov046_021115e4")
#pragma comment(linker, "/alternatename:_func_021115f4=_data_ov046_021115f4")

static int __fastcall bsw_init(void *s, void *)
{ return _ZN16BowserShockwaves13InitResourcesEv(s); }
static int __fastcall bsw_clean(void *s, void *)
{ (void)s; return _ZN16BowserShockwaves16CleanupResourcesEv(); }
static int __fastcall bsw_behavior(void *s, void *)
{ return _ZN16BowserShockwaves8BehaviorEv(s); }
static int __fastcall bsw_render(void *s, void *)
{ port_actor_render_probe("BOWSER_SHOCKWAVES", (char *)s + 0xd4);
  return _ZN16BowserShockwaves6RenderEv(s); }
static int __fastcall bsw_d1(void *s, void *)
{ return (int)(size_t)_ZN16BowserShockwavesD1Ev((int *)s); }
static int __fastcall bsw_d0(void *s, void *)
{ return (int)(size_t)_ZN16BowserShockwavesD0Ev((int *)s); }

extern "C" void hal_fill_bowser_shockwaves_vtable(void)
{
    void *volatile *vt = (void *volatile *)_ZTV16BowserShockwaves;
    rs_fill_shared(vt);
    vt[0]  = (void *)bsw_init;
    vt[3]  = (void *)bsw_clean;
    vt[6]  = (void *)bsw_behavior;
    vt[9]  = (void *)bsw_render;
    vt[16] = (void *)bsw_d1;
    vt[17] = (void *)bsw_d0;
}

#include "BowserShockwaves.h"
extern "C" {
int _ZN16BowserShockwaves13InitResourcesEv(void *self)
{ return ((BowserShockwaves *)self)->BowserShockwaves::InitResources(); }
int _ZN16BowserShockwaves8BehaviorEv(void *self)
{ return ((BowserShockwaves *)self)->BowserShockwaves::Behavior(); }
}

// ============================================================================
// BUBBLE (actor 291, ov002) -- daObjAbuku_c, table data_ov002_02108964
// ============================================================================
//
// The air bubble a treasure chest releases when it is opened underwater
// (TreasureChest's open state func_ov064_0211a39c: Actor::Spawn(0x123) when
// the player's mIsUnderwater byte is set, then zeroes the bubble's velocity).
// A plain Actor build (ActorC2, 276 bytes) with one MovingCylinderClsn at
// 0xd4. It rises on a sine bob, heals the player it touches (+0xf8 holds the
// id its cylinder hit) and pops on a timer or when it breaks the surface.
//
// dsd named none of its methods (the class shares its base with nothing that
// carries a name), so every own slot is a func_ov002 body: 0 InitResources
// (func_ov002_020b3518), 6 Behavior (func_ov002_020b33dc), 16 D1
// (func_ov002_020b3298). Slot 17, the D0, is a HOST THUNK: the matched body
// func_ov002_020b32c8 spells its table and the heap as the VT0 / G0 shadow
// globals (the PoleLift trap), so it is transcribed here instead: table
// restore, MovingCylinderClsn::D1 at +0xd4, Actor::D2, Deallocate against
// 0x020a0eac. Slots 3 and 9 are ActorBase's own (relocs), 18..30 defaults.
extern "C" {
int func_ov002_020b3518(char *self);     /* slot 0, InitResources */
int func_ov002_020b33dc(char *self);     /* slot 6, Behavior */
int *func_ov002_020b3298(int *self);     /* slot 16, D1 */
extern int data_ov002_02108964[];        /* ov002 mount: the 31-slot table */
}
/* The D1 restores the table by its RTTI name. */
#pragma comment(linker, "/alternatename:__ZTV12daObjAbuku_c=_data_ov002_02108964")

static int __fastcall bub_init(void *s, void *)
{ return func_ov002_020b3518((char *)s); }
static int __fastcall bub_behavior(void *s, void *)
{ rs_probe("BUBBLE", s, *(unsigned short *)((char *)s + 0x10e));
  return func_ov002_020b33dc((char *)s); }
static int __fastcall bub_d1(void *s, void *)
{ return (int)(size_t)func_ov002_020b3298((int *)s); }
static int __fastcall bub_d0(void *s, void *)
{
    char *t = (char *)s;
    *(void **)t = (void *)data_ov002_02108964;
    _ZN18MovingCylinderClsnD1Ev(t + 0xd4);
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return (int)(size_t)s;
}

extern "C" void hal_fill_bubble_vtable(void)
{
    void *volatile *vt = (void *volatile *)data_ov002_02108964;
    rs_fill_shared(vt);
    vt[0]  = (void *)bub_init;
    vt[3]  = (void *)rs_clean_base;    /* ROM slot 3: ActorBase::CleanupResources */
    vt[6]  = (void *)bub_behavior;
    vt[9]  = (void *)rs_render_base;   /* ROM slot 9: ActorBase::Render */
    vt[16] = (void *)bub_d1;
    vt[17] = (void *)bub_d0;
}

// ============================================================================
// YOSHI_EGG (actor 9, ov002) -- _ZTV8YoshiEgg / _ZTV8daYegg_c, 0x0210adb4
// ============================================================================
//
// The egg Yoshi lays after swallowing an enemy, then aims and throws. An Enemy
// build (EnemyC2, 1068 bytes): MovingCylinderClsn 0x110, WithMeshClsn 0x144,
// ModelAnim 0x300, ShadowModel 0x364, the laying player at 0x38c (written by
// the spawner right after Actor::Spawn, before InitResources), the state word
// at 0x3f0 and the up-to-five aim targets at 0x3fc. Spawned only by the
// Player's egg lay (func_ov002_020d6368 / 020d5ab4, slice_gate10, linked).
//
// Own slots: 0 Init (flat C), 3 Cleanup (real C++ method, faced), 6 Behavior
// (faced), 9 Render (HOST COPY: the ModelAnim slot-5 collision,
// unmatched/ModelAnim_Renders.cpp), 16/17 the flat-C D1/D0. Its four-state
// PMF machine is unmatched/YoshiEgg_StateDispatch.cpp; every state body and
// every other helper (26 func_ov002 TUs between 0x020ec610 and 0x020eddc4)
// byte-matches and the ones not already linked are on slice_gate210.txt.
//
// func_02123804 is dsd's code-flavoured spelling of a level-overlay-window
// address the egg's collision helper (func_ov002_020ec670) calls when the
// thing it hit is a WHOMP (id 0xa4) or WHOMP_KING (0xa5): that is ov079's
// func_ov079_02123804 (slice_gate64, linked), the Whomp's hit-by-egg entry,
// so the spelling is aliased onto it -- the same treatment BowserShockwaves'
// two BMA spellings get above.
extern "C" {
int _ZN8YoshiEgg13InitResourcesEv(void *self);     /* flat C TU */
int _ZN8YoshiEgg16CleanupResourcesEv(void *self);  /* face below */
int _ZN8YoshiEgg8BehaviorEv(void *self);           /* face below */
int _ZN8YoshiEgg6RenderEv(void *self);             /* host copy */
int *_ZN8YoshiEggD1Ev(int *self);
int *_ZN8YoshiEggD0Ev(int *self);
extern int _ZTV8YoshiEgg[];                        /* ov002 mount, 31 slots */
}
#pragma comment(linker, "/alternatename:__ZTV8daYegg_c=__ZTV8YoshiEgg")
#pragma comment(linker, "/alternatename:_func_02123804=_func_ov079_02123804")
/* CleanupResources.cpp releases the two animation SharedFilePtrs as C++
   `extern char` objects; the mount defines them under the C spelling. */
#pragma comment(linker, "/alternatename:?data_ov002_0210e6b0@@3DA=_data_ov002_0210e6b0")
#pragma comment(linker, "/alternatename:?data_ov002_0210eb78@@3DA=_data_ov002_0210eb78")

static int __fastcall ye_init(void *s, void *)
{ return _ZN8YoshiEgg13InitResourcesEv(s); }
static int __fastcall ye_clean(void *s, void *)
{ return _ZN8YoshiEgg16CleanupResourcesEv(s); }
static int __fastcall ye_behavior(void *s, void *)
{ rs_probe("YOSHI_EGG", s, *(int *)((char *)s + 0x3f0));
  return _ZN8YoshiEgg8BehaviorEv(s); }
static int __fastcall ye_render(void *s, void *)
{ port_actor_render_probe("YOSHI_EGG", (char *)s + 0x300);
  return _ZN8YoshiEgg6RenderEv(s); }
static int __fastcall ye_d1(void *s, void *)
{ return (int)(size_t)_ZN8YoshiEggD1Ev((int *)s); }
static int __fastcall ye_d0(void *s, void *)
{ return (int)(size_t)_ZN8YoshiEggD0Ev((int *)s); }

extern "C" void hal_fill_yoshi_egg_vtable(void)
{
    port_enemy_death_states_seat();
    void *volatile *vt = (void *volatile *)_ZTV8YoshiEgg;
    rs_fill_shared(vt);
    vt[0]  = (void *)ye_init;
    vt[3]  = (void *)ye_clean;
    vt[6]  = (void *)ye_behavior;
    vt[9]  = (void *)ye_render;
    vt[16] = (void *)ye_d1;
    vt[17] = (void *)ye_d0;
}

#include "YoshiEgg.h"

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
int _ZN8YoshiEgg16CleanupResourcesEv(void *self)
{ return ((YoshiEgg *)self)->YoshiEgg::CleanupResources(); }
int _ZN8YoshiEgg8BehaviorEv(void *self)
{ return ((YoshiEgg *)self)->YoshiEgg::Behavior(); }
}
