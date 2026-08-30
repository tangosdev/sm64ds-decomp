// GATE 60-63: the vtables of Whomp's Fortress' mechanical platforms (ov015).
//
// Same law as hal/actor_classes.cpp and hal/actor_classes_bob_enemy.cpp -- ROM
// slot order, __fastcall thunks that call QUALIFIED, unhosted slots trap by
// name.
//
// ---- THE WIDTH: "31 because Platform" WAS THE BUG ---------------------------
//
// This header used to say THIRTY-ONE SLOTS, because these are Actor subclasses
// through Platform. That is backwards and it cost a live crash. Thirty-one is
// the PLAIN ACTOR width: slot 30 is Actor::OnAimedAtWithEggReturnVec and the
// table ends. A Platform subclass has a THIRTY-SECOND slot, Platform::Kill
// (ov002 0x020ee55c), and six of the seven tables here are Platform
// subclasses. Written [31], each left slot 31 reading past the end:
//
//     SM64DS_LEVEL=7 SM64DS_KILL_SLOT31=51@300
//     [kill31] actor 0481D40C id 51 class ROTATING_BRIDGE vptr 00B66600
//              slot31 00000000
//     code c0000005, access 00000008 at 00000000, eip 00000000
//
// The exception is data_ov015_02114360 (id 42 POLE_BILLBOARD), which is a
// plain Actor rather than a Platform and really is 31. CHECK THE CLASS BEFORE
// GROWING ANYTHING: port/tools/vtspan.py reads the width out of the reloc run,
// and the next-symbol bound lies for five of these seven (_ZTV14KnockDownPlank
// reads as 3 words and is 32).
//
// ---- THE ov015 NAME SHIFT --------------------------------------------------
//
// The config's class-to-vtable naming is shifted by one all through this
// overlay, the gate-20 shift again: PoleBillboard_Spawn installs the billboard
// BASE table (data_ov015_02114360), while _ZTV13PoleBillboard (0x02114420)
// carries the _ZN13PoleBillboard* methods and is what KnockDownPlank_Spawn
// installs, and so on down the file. So every row here is wired BY THE VTABLE
// ITS FACTORY ACTUALLY INSTALLS -- read out of relocs.txt at the factory's own
// vtable-store site -- never by the class name the SpawnInfo carries. The seven
// vtables and what each id's factory installs:
//
//   id 42 POLE_BILLBOARD       -> 0x02114360  (billboard base; slot 6 is the
//                                 arm9 Actor no-op, this platform does not tick)
//   id 44 KNOCK_DOWN_PLANK     -> 0x02114420  _ZTV13PoleBillboard / Botaosi_c
//   id 53 MOVING_BAR_BIG       -> 0x0211458c  _ZTV14KnockDownPlank / Dossunbar_c
//   id 54 MOVING_BAR_SMALL     -> 0x0211458c  (same table as BIG)
//   id 52 TOWER_STEP           -> 0x02114650  _ZTV14MovingBarSmall / Lift_c
//   id 51 ROTATING_BRIDGE      -> 0x02114714  _ZTV9TowerStep / Rotebar_c
//   id 50 ROTATING_PLATFORM_WF -> 0x021147e8  (unnamed; slots 6/9 are ov002's
//                                 shared Platform Behavior/Render)
//   id 45 FALL_BLOCK_WF        -> 0x021148dc  _ZTV11FallBlockWf (slots 6/9 are
//                                 ov098 cannon code -- see the gate-63 header)
//
// ---- THE VTABLES ARE HOST STORAGE ------------------------------------------
//
// All seven are excluded from the ov015 per-symbol mount (ov015_syms.txt) and
// declared here as host arrays the registry fills, the ov080 painting rule: a
// mounted vtable would hand a factory DS code addresses. Two are unnamed data
// symbols dsd left as plain data (0x02114360 and 0x021147e8); the rest carry a
// _ZTV name. Each class's own D1/D0 restores its vtable by RTTI name mid-
// teardown, so every _ZTV name that address answers to is aliased onto the one
// host array below.
//
// SLOT 30 does not trap here the way it does for the enemies: these are
// Platform subclasses whose Actor tail past slot 17 is entirely the arm9 base
// bodies (no OnAimedAtWithEggReturnVec override), and the mount leaves those
// pointing at arm9 anyway. The shared fill writes the ten ActorBase halves plus
// Actor's own tail from the same host functions the enemies use; slot 12 is
// ActorBase::OnPendingDestroy for all seven (their vtables' slot 12 is the arm9
// 0x02043ac0), which the shared fill already installs.
#include <cstdio>
#include "dsstate_seg.h"
#include <cstdlib>

#include "Actor.h"
#include "ActorBase.h"

extern "C" {
int _ZN5Actor19BeforeInitResourcesEv(void *self);          /* slot 1  */
void _ZN5Actor18AfterInitResourcesEj(void *self, unsigned a); /* slot 2 */
int _ZN5Actor14BeforeBehaviorEv(void *self);               /* slot 7  */
int _ZN5Actor12BeforeRenderEv(void *self);                 /* slot 10 */
int _ZN5Actor13OnYoshiTryEatEv(void *self);                /* slot 18 */
int _ZN5Actor9Virtual50Ev(void *self);                     /* slot 20 */
void _ZN5Actor15OnGroundPoundedERS_(void *self, void *o);  /* slot 21 */
void _ZN5Actor11OnAttacked1ERS_(void *self, void *o);      /* slot 22 */
void _ZN5Actor11OnAttacked2ERS_(void *self, void *o);      /* slot 23 */
void _ZN5Actor8OnKickedERS_(void *self, void *o);          /* slot 24 */
void _ZN5Actor8OnPushedERS_(void *self, void *o);          /* slot 25 */
void _ZN5Actor24OnHitByCannonBlastedCharERS_(void *self, void *o); /* 26 */
void _ZN5Actor15OnHitByMegaCharER6Player(void *self, void *p);     /* 27 */
void _ZN5Actor19OnHitFromUnderneathERS_(void *self, void *o);      /* 28 */
void _ZN5Actor13OnTurnIntoEggER6Player(void *self, void *p);       /* 19 */
int  _ZN5Actor16OnAimedAtWithEggEv(void *self);                    /* 29 */
void _ZN8Platform4KillEv(void *self);                              /* 31 */

extern int data_02099f24[];          /* the frame phase the lists are in */
extern unsigned char data_020a4b4c;  /* the spawn spine's own step */
const char *port_actor_class_name(unsigned id);   /* hal/actor_registry */
  void port_actor_slot_decline(const char *what);  /* func_02043fdc_hostcopy.cpp */
void port_actor_render_probe(const char *cls, void *model); /* actor_classes */
}

// ---- the trap --------------------------------------------------------------
static void wf_trap_report(void *self, int slot)
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
#define WF_TRAP(n) \
    static int __fastcall wf_trap##n(void *s, void *) \
    { wf_trap_report(s, n); return 0; }
WF_TRAP(13) WF_TRAP(14) WF_TRAP(17)
/* 23/24 are for the two classes below that OVERRIDE one of Actor's tail bodies
   with an ov015 body that is matched in src but in no slice; running Actor's
   shared body there would be the wrong code, not merely less code. 27 and 31
   used to trap here too, but the mega/kill cluster's own ov015 bodies are now
   sliced and seated (kp_mega/ts_mega/mb_mega for 27, ts_kill/mb_kill for 31; see
   below). 30 declines for all seven: its ROM body returns a Vector3 by value and
   the sret contract is unproved. */
WF_TRAP(30)
#undef WF_TRAP
/* Slots 23/24 take the three-parameter shape so they emit `ret 4`: their one
   dispatch site each is now thiscall (Actor_OnAttacked2Dispatch.cpp /
   Actor_OnKickedDispatch.cpp), which pushes one argument the callee must pop.
   Body identical to the WF_TRAP shape; only the pop contract widens. */
static int __fastcall wf_trap23(void *s, void *, void *)
{ wf_trap_report(s, 23); return 0; }
static int __fastcall wf_trap24(void *s, void *, void *)
{ wf_trap_report(s, 24); return 0; }

static int __fastcall wf_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall wf_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall wf_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall wf_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall wf_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall wf_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall wf_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall wf_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall wf_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall wf_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall wf_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
static int __fastcall wf_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall wf_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall wf_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall wf_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall wf_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall wf_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall wf_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall wf_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall wf_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall wf_egg(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }
/* slot 19, OnTurnIntoEgg(Player &player): the caller PUSHES the player, so the
   three-parameter veneer pops it. The ROM reloc at each of these tables + 0x4c
   lands on arm9 0x02010154, Actor::OnTurnIntoEgg (a tail-call veneer to
   KillAndTrackInDeathTable). Seating it lets Yoshi swallow-and-respawn these
   as the ROM does; trapping it froze the actor forever. */
static int __fastcall wf_turn_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
/* slot 31, the Platform tail; the five tables that do not override it */
static int __fastcall wf_kill(void *s, void *)
{ _ZN8Platform4KillEv(s); return 0; }

/* ---- the mega/kill cluster's own ov015 bodies (gate 60/62/76) --------------
   Three of these seven classes override slot 27 (OnHitByMegaChar) and, for two
   of them, slot 31 (Kill) with their own ov015 bodies rather than Actor's /
   Platform's. Each is a real function entry (push {r4,lr}, receiver out of r0),
   matched src, now sliced.

   Slot 27 takes the three-parameter __fastcall shape so it emits `ret 4`: its
   dispatch site pushes the Player argument the callee pops, the wf_mega
   contract. Slot 31's dispatch is a same-object virtual out of the
   slot-27 body (`ldr [r0]; ldr [r1,#0x7c]; blx r1` in the ROM, `c->m()` in the
   recovered C++), so it is thiscall with no argument: the no-arg __fastcall
   veneer reads the receiver from ECX and emits `ret` (no pop), the wf_kill
   convention. The two slot-27 bodies MUST land their same-object slot-31
   dispatch on these real thiscall Kill bodies, which is why each pair is seated
   into the one host table together. */
extern "C" {
void func_ov015_021113c0(void *self, void *player);  /* KDP  slot 27 */
void func_ov015_0211233c(void *self);                /* TS   slot 31 Kill */
void func_ov015_021123a0(void *self, int player);    /* TS   slot 27 */
void func_ov015_02111c3c(void *self);                /* MB   slot 31 Kill */
void func_ov015_02111cb8(void *self, int player);    /* MB   slot 27 */
}
static int __fastcall kp_mega(void *s, void *, void *p)
{ func_ov015_021113c0(s, p); return 0; }
static int __fastcall ts_mega(void *s, void *, void *p)
{ func_ov015_021123a0(s, (int)(size_t)p); return 0; }
static int __fastcall ts_kill(void *s, void *)
{ func_ov015_0211233c(s); return 0; }
static int __fastcall mb_mega(void *s, void *, void *p)
{ func_ov015_02111cb8(s, (int)(size_t)p); return 0; }
static int __fastcall mb_kill(void *s, void *)
{ func_ov015_02111c3c(s); return 0; }

/* The shared half, slots 1..30. Six of the seven tables here are 32-slot
   Platform tables and the seventh (data_ov015_02114360, id 42) is a 31-slot
   plain Actor; slot 31 belongs to the caller either way, so it is not written
   here. A caller writes its own 0/3/6/9/16 and its 31, and slot 12 stays
   ActorBase::OnPendingDestroy (the ROM's own slot-12 target for all seven).
   Slot 17 traps: the ROM teardown dispatches
   slot 16 and Memory::Deallocate is done by ActorBase::AfterCleanupResources,
   so a call landing on 17 is worth an abort. */
static void wf_fill_shared(void **vt)
{
    vt[1] = (void *)wf_binit;
    vt[2] = (void *)wf_ainit;
    vt[4] = (void *)wf_bclean;
    vt[5] = (void *)wf_aclean;
    vt[7] = (void *)wf_bbeh;
    vt[8] = (void *)wf_abeh;
    vt[10] = (void *)wf_bren;
    vt[11] = (void *)wf_aren;
    vt[12] = (void *)wf_pdes;
    vt[13] = (void *)wf_trap13;
    vt[14] = (void *)wf_trap14;
    vt[15] = (void *)wf_heap;
    vt[17] = (void *)wf_trap17;
    vt[18] = (void *)wf_yoshi;
    vt[19] = (void *)wf_turn_egg;
    vt[20] = (void *)wf_v50;
    vt[21] = (void *)wf_pounded;
    vt[22] = (void *)wf_atk1;
    vt[23] = (void *)wf_atk2;
    vt[24] = (void *)wf_kicked;
    vt[25] = (void *)wf_pushed;
    vt[26] = (void *)wf_cannon;
    vt[27] = (void *)wf_mega;
    vt[28] = (void *)wf_under;
    /* Slot 29 is Actor::OnAimedAtWithEgg (arm9 0x02010124), the egg lock-on
       radius. It used to forward to Virtual50, which is a different function
       returning a different constant; the ROM tables all name 0x02010124
       here. */
    vt[29] = (void *)wf_egg;
    vt[30] = (void *)wf_trap30;
}

// ============================================================================
// TOWER_STEP (id 52) -- vtable 0x02114650, the _ZN14MovingBarSmall* methods
// ============================================================================
//
// TowerStep_Spawn installs 0x02114650 (config _ZTV14MovingBarSmall /
// _ZTV14daObjBk_Lift_c), so an id-52 object runs MovingBarSmall's lifecycle.
// 916-byte object: Model at +0xd4, MovingMeshCollider at +0x124, ShadowModel at
// +0x320. Whomp's Fortress names several tower steps.
/* Init/Clean/Render are real C++ methods (the .cpp files define
   MovingBarSmall::<name>); Behavior and D1 are plain C in src. The thunks call
   each the way it is defined, the Tree/ArrowSign shape. */
#include "MovingBarSmall.h"
extern "C" {
int _ZN14MovingBarSmall13InitResourcesEv(char *self);  /* .cpp, but extern "C" */
int _ZN14MovingBarSmall8BehaviorEv(char *self);   /* .c, C linkage */
int *_ZN14MovingBarSmallD1Ev(int *self);          /* .c, C linkage */
void *_ZTV14MovingBarSmall[32];
}
/* The address 0x02114650 answers to both _ZTV names; the class D1 restores it
   by its RTTI name. */
#pragma comment(linker, "/alternatename:__ZTV14daObjBk_Lift_c=__ZTV14MovingBarSmall")
static int __fastcall ts_init(void *s, void *)
{ return _ZN14MovingBarSmall13InitResourcesEv((char *)s); }
static int __fastcall ts_clean(void *s, void *)
{ return ((MovingBarSmall *)s)->MovingBarSmall::CleanupResources(); }
static int __fastcall ts_behavior(void *s, void *)
{ return _ZN14MovingBarSmall8BehaviorEv((char *)s); }
static int __fastcall ts_render(void *s, void *)
{
    port_actor_render_probe("TOWER_STEP", (char *)s + 0xd4);
    return ((MovingBarSmall *)s)->MovingBarSmall::Render();
}
static int __fastcall ts_d1(void *s, void *)
{ return (int)(size_t)_ZN14MovingBarSmallD1Ev((int *)s); }
extern "C" void hal_fill_tower_step_vtable(void)
{
    void **vt = _ZTV14MovingBarSmall;
    wf_fill_shared(vt);
    vt[0] = (void *)ts_init;
    vt[3] = (void *)ts_clean;
    vt[6] = (void *)ts_behavior;
    vt[9] = (void *)ts_render;
    vt[16] = (void *)ts_d1;
    /* 32 slots. _ZTV14MovingBarSmall overrides two of the tail with its own
       ov015 bodies, both matched in src and in no slice: 27 OnHitByMegaChar
       (0x021123a0) and 31 Kill (0x0211233c). Seated together (gate 60): the
       slot-27 body dispatches this same object's slot 31 through the vptr, so
       ts_kill must be the thiscall Kill it lands on. */
    vt[27] = (void *)ts_mega;
    vt[31] = (void *)ts_kill;
}

// ============================================================================
// ROTATING_BRIDGE (id 51) -- vtable 0x02114714, the _ZN9TowerStep* methods
// ============================================================================
//
// RotatingBridge_Spawn installs 0x02114714 (config _ZTV9TowerStep /
// _ZTV17daObjBk_Rotebar_c), so an id-51 object runs TowerStep's lifecycle.
// 804-byte object: Model at +0xd4, MovingMeshCollider at +0x124. Whomp's
// Fortress names the rotating bridge.
/* TowerStep's Init/Clean/Behavior/Render are real C++ methods; only D1 is
   plain C in src. */
#include "TowerStep.h"
extern "C" {
int *_ZN9TowerStepD1Ev(int *self);                /* .c, C linkage */
void *_ZTV9TowerStep[32];
}
#pragma comment(linker, "/alternatename:__ZTV17daObjBk_Rotebar_c=__ZTV9TowerStep")
static int __fastcall rb_init(void *s, void *)
{ return ((TowerStep *)s)->TowerStep::InitResources(); }
static int __fastcall rb_clean(void *s, void *)
{ return ((TowerStep *)s)->TowerStep::CleanupResources(); }
static int __fastcall rb_behavior(void *s, void *)
{ return ((TowerStep *)s)->TowerStep::Behavior(); }
static int __fastcall rb_render(void *s, void *)
{
    port_actor_render_probe("ROTATING_BRIDGE", (char *)s + 0xd4);
    return ((TowerStep *)s)->TowerStep::Render();
}
static int __fastcall rb_d1(void *s, void *)
{ return (int)(size_t)_ZN9TowerStepD1Ev((int *)s); }
extern "C" void hal_fill_rotating_bridge_vtable(void)
{
    void **vt = _ZTV9TowerStep;
    wf_fill_shared(vt);
    vt[0] = (void *)rb_init;
    vt[3] = (void *)rb_clean;
    vt[6] = (void *)rb_behavior;
    vt[9] = (void *)rb_render;
    vt[16] = (void *)rb_d1;
    /* 32 slots -- this is ROTATING_BRIDGE's table, the class the slot-31 probe
       crashed on. Slot 31 is Platform::Kill, not overridden. */
    vt[31] = (void *)wf_kill;
}

// ============================================================================
// POLE_BILLBOARD (id 42) -- vtable 0x02114360, the billboard base table
// ============================================================================
//
// PoleBillboard_Spawn installs the unnamed billboard base table
// data_ov015_02114360 (config _ZTV18daObjBkBillboard_c), NOT the
// _ZTV13PoleBillboard at 0x02114420 that carries the _ZN13PoleBillboard*
// methods (which KNOCK_DOWN_PLANK installs below -- the name shift). So an
// id-42 object is a plain billboard: a 292-byte object with a Model at +0xd4
// and NO collider, whose slot-6 Behavior is ActorBase's own no-op and whose own
// slots are the func_ov015_0211* family. Whomp's Fortress names several pole
// billboards.
extern "C" {
int func_ov015_021112a0(char *self);   /* slot 0  InitResources (.cpp extern C) */
int func_ov015_02111254(char *self);   /* slot 3  CleanupResources */
int func_ov015_02111278(char *self);   /* slot 9  Render */
int *func_ov015_021111a0(int *self);   /* slot 16 D1 */
DSSTATE_BEGIN
void *data_ov015_02114360[31];
DSSTATE_END
}
#pragma comment(linker, "/alternatename:__ZTV18daObjBkBillboard_c=_data_ov015_02114360")
static int __fastcall pb_init(void *s, void *)
{ return func_ov015_021112a0((char *)s); }
static int __fastcall pb_clean(void *s, void *)
{ return func_ov015_02111254((char *)s); }
/* slot 6 is ActorBase::Behavior, a base no-op the .cpp defines as a real
   method. */
static int __fastcall pb_behavior(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::Behavior(); }
static int __fastcall pb_render(void *s, void *)
{
    port_actor_render_probe("POLE_BILLBOARD", (char *)s + 0xd4);
    return func_ov015_02111278((char *)s);
}
static int __fastcall pb_d1(void *s, void *)
{ return (int)(size_t)func_ov015_021111a0((int *)s); }
extern "C" void hal_fill_pole_billboard_vtable(void)
{
    void **vt = data_ov015_02114360;
    wf_fill_shared(vt);
    vt[0] = (void *)pb_init;
    vt[3] = (void *)pb_clean;
    vt[6] = (void *)pb_behavior;
    vt[9] = (void *)pb_render;
    vt[16] = (void *)pb_d1;
    /* THIRTY-ONE and correct: id 42 POLE_BILLBOARD installs the billboard base
       table, which is a plain Actor and not a Platform, so it has no slot 31.
       The only one of the seven that does not grow. */
}

// ============================================================================
// KNOCK_DOWN_PLANK (id 44) -- vtable 0x02114420, the _ZN13PoleBillboard* methods
// ============================================================================
//
// KnockDownPlank_Spawn installs 0x02114420 (config _ZTV13PoleBillboard /
// _ZTV17daObjBk_Botaosi_c), so an id-44 object runs PoleBillboard's lifecycle:
// a 924-byte object with Model at +0xd4, MovingMeshCollider at +0x124,
// ShadowModel at +0x320. Init and Behavior are plain C in src (the .cpp files
// define them extern "C"); Clean and Render are real C++ methods; D1 is C.
#include "PoleBillboard.h"
extern "C" {
int _ZN13PoleBillboard13InitResourcesEv(char *self);  /* .c, C linkage */
int _ZN13PoleBillboard8BehaviorEv(char *self);         /* .cpp extern "C" */
int *_ZN13PoleBillboardD1Ev(int *self);                /* .c, C linkage */
void *_ZTV13PoleBillboard[32];
}
#pragma comment(linker, "/alternatename:__ZTV17daObjBk_Botaosi_c=__ZTV13PoleBillboard")
static int __fastcall kp_init(void *s, void *)
{ return _ZN13PoleBillboard13InitResourcesEv((char *)s); }
static int __fastcall kp_clean(void *s, void *)
{ return ((PoleBillboard *)s)->PoleBillboard::CleanupResources(); }
static int __fastcall kp_behavior(void *s, void *)
{ return _ZN13PoleBillboard8BehaviorEv((char *)s); }
static int __fastcall kp_render(void *s, void *)
{
    port_actor_render_probe("KNOCK_DOWN_PLANK", (char *)s + 0xd4);
    return ((PoleBillboard *)s)->PoleBillboard::Render();
}
static int __fastcall kp_d1(void *s, void *)
{ return (int)(size_t)_ZN13PoleBillboardD1Ev((int *)s); }
extern "C" void hal_fill_knock_down_plank_vtable(void)
{
    void **vt = _ZTV13PoleBillboard;
    wf_fill_shared(vt);
    vt[0] = (void *)kp_init;
    vt[3] = (void *)kp_clean;
    vt[6] = (void *)kp_behavior;
    vt[9] = (void *)kp_render;
    vt[16] = (void *)kp_d1;
    /* 32 slots. _ZTV13PoleBillboard overrides four of the tail with its own
       ov015 bodies, all matched in src and none in a slice: 23 OnAttacked2
       (0x02111408), 24 OnKicked (0x021113fc), 27 OnHitByMegaChar (0x021113c0)
       and 31 Kill, which is Platform's (0x020ee55c). Slot 27 is seated now
       (gate 62, kp_mega -> func_ov015_021113c0); 23/24 stay trapped. */
    vt[23] = (void *)wf_trap23;
    vt[24] = (void *)wf_trap24;
    vt[27] = (void *)kp_mega;
    vt[31] = (void *)wf_kill;
}

// ============================================================================
// ROTATING_PLATFORM_WF (id 50) -- vtable 0x021147e8 (data_ov015_021147e8)
// ============================================================================
//
// RotatingPlatformWf_Spawn installs the unnamed table data_ov015_021147e8
// (config _ZTV17daObjBk_Ukisima_c, ukishima: floating island), an 800-byte
// object with Model at +0xd4 and MovingMeshCollider at +0x124. Its own slots
// are the plain-C func_ov015_02112c* family; its slot-6 Behavior and slot-9
// Render are ov002's SHARED Platform bodies (func_ov002_020b6718 / 020b66f0),
// already in the build, with no pointer-to-member anywhere. Whomp's Fortress
// names the rotating platform.
extern "C" {
int func_ov015_02112c98(char *self);   /* slot 0  InitResources */
int func_ov015_02112c84(char *self);   /* slot 3  CleanupResources */
int func_ov002_020b6718(char *self);   /* slot 6  Behavior (ov002 base) */
int func_ov002_020b66f0(char *self);   /* slot 9  Render (ov002 base) */
int *func_ov015_02112bd0(int *self);   /* slot 16 D1 */
DSSTATE_BEGIN
void *data_ov015_021147e8[32];
DSSTATE_END
void *RotatingPlatformWf_Spawn(void);
}
#pragma comment(linker, "/alternatename:__ZTV17daObjBk_Ukisima_c=_data_ov015_021147e8")
/* RotatingPlatformWf_Spawn is the one ov015 factory whose vtable store is the
   shared-header VT1 placeholder (`p[0] = (int)VT1`), not a named _ZTV -- the
   recovered source's byte-matched wildcard for the relocated word 0x021147e8.
   VT1 is auto_bss's zeroed [8] array, so a raw spawn leaves the object
   dispatching through nulls. The host factory calls the real spawn, then reseats
   slot 0 onto this class's host vtable, the way the registry repoints a
   SpawnInfo's +0 word. */
extern "C" void *port_factory_rotating_platform_wf(void)
{
    void *p = RotatingPlatformWf_Spawn();
    if (p)
        *(void **)p = (void *)data_ov015_021147e8;
    return p;
}
static int __fastcall rp_init(void *s, void *)
{ return func_ov015_02112c98((char *)s); }
static int __fastcall rp_clean(void *s, void *)
{ return func_ov015_02112c84((char *)s); }
static int __fastcall rp_behavior(void *s, void *)
{ return func_ov002_020b6718((char *)s); }
static int __fastcall rp_render(void *s, void *)
{
    port_actor_render_probe("ROTATING_PLATFORM_WF", (char *)s + 0xd4);
    return func_ov002_020b66f0((char *)s);
}
static int __fastcall rp_d1(void *s, void *)
{ return (int)(size_t)func_ov015_02112bd0((int *)s); }
extern "C" void hal_fill_rotating_platform_wf_vtable(void)
{
    void **vt = data_ov015_021147e8;
    wf_fill_shared(vt);
    vt[0] = (void *)rp_init;
    vt[3] = (void *)rp_clean;
    vt[6] = (void *)rp_behavior;
    vt[9] = (void *)rp_render;
    vt[16] = (void *)rp_d1;
    /* 32 slots; slot 31 is Platform::Kill. dsd's bound reads 15 words here. */
    vt[31] = (void *)wf_kill;
}

// ============================================================================
// MOVING_BAR_BIG (id 53) and MOVING_BAR_SMALL (id 54) -- vtable 0x0211458c
// ============================================================================
//
// Both factories (MovingBarBig_Spawn / MovingBarSmall_Spawn) install vtable
// 0x0211458c (config _ZTV14KnockDownPlank / _ZTV19daObjBk_Dossunbar_c), so an
// id-53 or id-54 object runs KnockDownPlank's lifecycle -- an 824-byte object
// (ActorBase::operator new(824)) built by Platform's ctor. The vtable's own
// slots are KnockDownPlank's methods: slot 0 InitResources (0x021120fc, a real
// C++ method), slot 3 CleanupResources (0x02112004, C++), slot 6 Behavior
// (0x02112090), slot 9 Render (0x02112068, C++), slot 16 D1 (0x02111ba0, C).
//
// The one wall this class hit was slot 6. KnockDownPlank::Behavior (the matched
// .cpp, MSVC-compiled) dispatches
//       (((C *)this)->*(data_ov015_021149ec[idx].pmf))();
// over a struct C that is FORWARD-DECLARED (`struct C;`) at the point the PMF
// typedef is formed, so MSVC gives the pointer-to-member its GENERAL
// (worst-case-inheritance) representation -- a multi-word struct plus a this-
// adjust/vptr thunk -- not the single code pointer a complete single-inheritance
// class gets. Seating the bss table with a plain function address makes the call
// read a bogus adjustment (a spawned MOVING_BAR ran 133 behaviours then faulted
// in func_ov015_02111e60 with the arg register turned into the state index).
//
// The port-faithful fix, done: a HOST COPY of KnockDownPlank::Behavior in
// port/unmatched/KnockDownPlank_Behavior.cpp reads the table entry as two plain
// ints and calls the body directly (the Fish/Door/WaterBomb treatment for
// exactly this reason), and seats BOTH of the class's {fn, delta} tables --
// Behavior's data_ov015_021149ec and the table data_ov015_02114a24 that
// func_ov015_02111fb8 drives -- over their host bodies before
// __sinit_ov015_02113048 copies the fourteen source statics in. The seat runs
// from hal/actor_overlays.cpp ahead of that sinit. All fourteen targets are
// matched ov015 src and all deltas are 0, so nothing traps.
//
// The address 0x0211458c answers to both _ZTV names; MovingBar*_Spawn install
// it as _ZTV14KnockDownPlank and the class D1 restores it as
// _ZTV19daObjBk_Dossunbar_c, so both names are aliased onto the one host array.
#include "KnockDownPlank.h"
extern "C" {
int _ZN14KnockDownPlank8BehaviorEv(void *self);          /* host copy, extern C */
int *_ZN14KnockDownPlankD1Ev(int *self);                 /* .c, C linkage */
void port_knock_down_plank_states_seat(void);            /* the two-table seat */
void *_ZTV14KnockDownPlank[32];
}
#pragma comment(linker, "/alternatename:__ZTV19daObjBk_Dossunbar_c=__ZTV14KnockDownPlank")
/* Four spelling bridges the recovered ov015 source needs, none of them a src
   edit and all byte-faithful (same address, same bytes):
   - KnockDownPlank::InitResources reads data_ov034_02114538, a dsd misprefix of
     the ov015 collider-file table at 0x02114538; alias it onto the mounted
     data_ov015_02114538.
   - KnockDownPlank::CleanupResources declares data_ov015_02114534 as a C++
     `extern char[]`, which MSVC mangles to ?data_ov015_02114534@@3PADA; the
     mounted symbol is the C-linkage _data_ov015_02114534.
   - func_ov015_02111d28 calls func_02012664, the plain-address spelling of
     Sound::PlayBank3(unsigned, const Vector3&) at 0x02012664.
   func_ov015_02111d98 also calls Actor::UpdatePosWithOnlySpeed(void*) as a C++
   __thiscall method (?UpdatePosWithOnlySpeed@Actor@@QAEXPAX@Z); that one cannot
   be an alias onto the __cdecl extern-C bridge (calling-convention mismatch, it
   would read this=0), so it gets a real __thiscall shim in
   port/unmatched/KnockDownPlank_Behavior.cpp. */
/* RETIRED, run rel0215 wave 2 lane cast-sweep2: this routing is now a
   per-source -D on src/_ZN14KnockDownPlank13InitResourcesEv.cpp in
   port/CMakeLists.txt. The ov034 mount that lane adds DEFINES
   data_ov034_02114538 -- it is the Wiggler's eleven-entry state table --
   and an /alternatename whose LHS is defined is inert, so the alias stopped
   routing and alternatename_guard said so at the link. The wave-5 R1/R2
   recipe: delete the dead directive at its own site, move the routing to
   the reader.
   was: /alternatename:_data_ov034_02114538=_data_ov015_02114538 */
#pragma comment(linker, "/alternatename:?data_ov015_02114534@@3PADA=_data_ov015_02114534")
#pragma comment(linker, "/alternatename:_func_02012664=__ZN5Sound9PlayBank3EjRK7Vector3")
/* Init/Clean/Render are real C++ methods (the .cpp defines KnockDownPlank::<n>),
   so the thunks call them QUALIFIED; Behavior is the host copy (extern "C") and
   D1 is plain C, the TowerStep/ArrowSign shape. */
static int __fastcall mb_init(void *s, void *)
{ return ((KnockDownPlank *)s)->KnockDownPlank::InitResources(); }
static int __fastcall mb_clean(void *s, void *)
{ return ((KnockDownPlank *)s)->KnockDownPlank::CleanupResources(); }
static int __fastcall mb_behavior(void *s, void *)
{ return _ZN14KnockDownPlank8BehaviorEv(s); }
static int __fastcall mb_render(void *s, void *)
{
    port_actor_render_probe("MOVING_BAR", (char *)s + 0xd4);
    return ((KnockDownPlank *)s)->KnockDownPlank::Render();
}
static int __fastcall mb_d1(void *s, void *)
{ return (int)(size_t)_ZN14KnockDownPlankD1Ev((int *)s); }
extern "C" void hal_fill_moving_bar_vtable(void)
{
    void **vt = _ZTV14KnockDownPlank;
    wf_fill_shared(vt);
    vt[0] = (void *)mb_init;
    vt[3] = (void *)mb_clean;
    vt[6] = (void *)mb_behavior;
    vt[9] = (void *)mb_render;
    vt[16] = (void *)mb_d1;
    /* 32 slots. _ZTV14KnockDownPlank overrides two of the tail with its own
       ov015 bodies, both matched in src and in no slice: 27 OnHitByMegaChar
       (0x02111cb8) and 31 Kill (0x02111c3c). dsd's bound reads 3 words. Seated
       together (gate 76): the slot-27 body dispatches this object's own slot 31
       through the vptr, so mb_kill must be the thiscall Kill it lands on. */
    vt[27] = (void *)mb_mega;
    vt[31] = (void *)mb_kill;
}

// ============================================================================
// FALL_BLOCK_WF (id 45) -- vtable 0x021148dc (_ZTV11FallBlockWf)
// ============================================================================
//
// FallBlockWf_Spawn installs 0x021148dc, an 844-byte object. The inherited
// blocker -- "func_ov098_0213a36c walks a same-id chain through +0x348 and
// faults on a null" -- is a PHANTOM. func_ov098_0213a36c IS the Behavior, and it
// is a plain switch state machine with NO pointer-to-member. Its same-id walk is
// null-safe: `p = *(char **)(p + 0x348); if (p == 0) break;` reads the next
// pointer and breaks before dereferencing, and func_ov098_0213a00c only ever
// stores a real Actor* there. Every slot is matched src, so there is no
// undecompiled code and no PMF seat: a plain vtable fill and a slice reach it.
//
// The 32-slot table (this one has a real slot 31, the KillOrWhatever poof-dust
// death effect Behavior case 2 and slot 27 both dispatch) overrides slots
// 0/3/6/9/16/17/27/31 over the arm9 Platform tail the shared fill installs:
//   slot 0  _ZN11FallBlockWf13InitResourcesEv   (ov015 .c, cross-overlay veneer)
//   slot 3  _ZN11FallBlockWf16CleanupResourcesEv (ov015 .c veneer)
//   slot 6  func_ov098_0213a36c                  (Behavior, the switch machine)
//   slot 9  func_ov098_0213a314                  (Render)
//   slot 16 _ZN11FallBlockWfD1Ev                 (ov015 .c)
//   slot 17 _ZN11FallBlockWfD0Ev                 (ov015 .c, the deleting dtor --
//           NOT a trap here, so it overrides wf_fill_shared's slot-17 trap)
//   slot 27 func_ov098_0213a284                  (arms the death via slot 31)
//   slot 31 func_ov098_0213a17c                  (KillOrWhatever)
// The vtable answers to _ZTV11FallBlockWf; D1/D0 restore _ZTV10dBgActor_c and the
// ov006 base data_ov006_0213c5bc mid-teardown (harmless host writes to a dying
// object), so no extra RTTI alias is needed.
extern "C" {
int _ZN11FallBlockWf13InitResourcesEv(void *self);        /* .c, C linkage */
int _ZN11FallBlockWf16CleanupResourcesEv(void *self);     /* .c, C linkage */
int func_ov098_0213a36c(char *self);                      /* Behavior */
int func_ov098_0213a314(char *self);                      /* Render */
int *_ZN11FallBlockWfD1Ev(int *self);                     /* .c, C linkage */
int *_ZN11FallBlockWfD0Ev(int *self);                     /* .c, C linkage */
void func_ov098_0213a284(char *self);                     /* slot 27 */
void func_ov098_0213a17c(char *self);                     /* slot 31 */
void *_ZTV11FallBlockWf[32];
}
/* FallBlockWf's ov015 Init/CleanupResources are cross-overlay veneers that
   tail-call the ov098 bodies by their UNPREFIXED spelling (func_0213a794 /
   func_0213a2cc); alias those onto the real ov098 symbols. */
#pragma comment(linker, "/alternatename:_func_0213a794=_func_ov098_0213a794")
#pragma comment(linker, "/alternatename:_func_0213a2cc=_func_ov098_0213a2cc")
static int __fastcall fb_init(void *s, void *)
{ return _ZN11FallBlockWf13InitResourcesEv(s); }
static int __fastcall fb_clean(void *s, void *)
{ return _ZN11FallBlockWf16CleanupResourcesEv(s); }
static int __fastcall fb_behavior(void *s, void *)
{ return func_ov098_0213a36c((char *)s); }
static int __fastcall fb_render(void *s, void *)
{
    port_actor_render_probe("FALL_BLOCK_WF", (char *)s + 0xd4);
    return func_ov098_0213a314((char *)s);
}
static int __fastcall fb_d1(void *s, void *)
{ return (int)(size_t)_ZN11FallBlockWfD1Ev((int *)s); }
static int __fastcall fb_d0(void *s, void *)
{ return (int)(size_t)_ZN11FallBlockWfD0Ev((int *)s); }
/* Slot 27 is OnHitByMegaChar(Player &player): the caller pushes the player,
   so the thunk needs the third parameter to pop it, even though the ov098
   body reads only r0. Two parameters would leave the pushed word behind and
   send the caller's `ret` to its saved EBP. */
static int __fastcall fb_slot27(void *s, void *, void *)
{ func_ov098_0213a284((char *)s); return 0; }
static int __fastcall fb_slot31(void *s, void *)
{ func_ov098_0213a17c((char *)s); return 0; }
extern "C" void hal_fill_fall_block_wf_vtable(void)
{
    void **vt = _ZTV11FallBlockWf;
    wf_fill_shared(vt);
    vt[0] = (void *)fb_init;
    vt[3] = (void *)fb_clean;
    vt[6] = (void *)fb_behavior;
    vt[9] = (void *)fb_render;
    vt[16] = (void *)fb_d1;
    vt[17] = (void *)fb_d0;    /* the ov098 base has a real deleting dtor here */
    vt[27] = (void *)fb_slot27;
    vt[31] = (void *)fb_slot31;
}
