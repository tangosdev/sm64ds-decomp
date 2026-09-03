// GATE 211: ENEMY_SWITCH_TAG (357) and ENEMY_SPAWNER (358), both ov002.
//
// Two of the level-designer object classes ov002 carries. ENEMY_SWITCH_TAG is
// the tag block that toggles an Event bit on and off (the switch a "!" enemy
// gate keys off); ENEMY_SPAWNER is the invisible placer that watches an Event
// bit and calls Actor::Spawn once to drop the enemy it is configured for.
// Both are the plain 31-slot Actor table, re-derived on this tree rather than
// taken from the brief:
//     python port/tools/vtspan.py . _ZTV14EnemySwitchTag   ->  31, plain Actor
//     python port/tools/vtspan.py . _ZTV12EnemySpawner     ->  31, plain Actor
// Every one of the 62 words resolves. Each class owns exactly five slots --
// 0 InitResources, 3 CleanupResources, 6 Behavior, 16 D1, 17 D0 -- and
// overrides NOTHING else: slot 9 stays ActorBase::Render (a no-op base body),
// slot 12 stays ActorBase::OnPendingDestroy. Neither Behavior dispatches a
// pointer-to-member, so no state table and no host state seat is needed. Full
// derivation, the id cross-check and the slot map are in port/slice_gate211.txt.
//
// THE 350-vs-357/358 SEPARATION. AMBIENT_SOUND_EFFECTS (350) sits three
// records further along the same ov002 spawn table and used to be NAMED after
// this pair, back when config carried the ov002 method blocks one class late
// (the #1048 shift, hal/actor_classes.cpp:517). That is closed: the three
// tables are distinct addresses in config/arm9/overlays/ov002/symbols.txt --
//     _ZTV12EnemySpawner       0x0210b364
//     _ZTV14EnemySwitchTag     0x0210b3e8
//     _ZTV19AmbientSoundEffects 0x0210b4c8
// -- so the two fills below build their OWN host arrays and never touch
// _ZTV19AmbientSoundEffects, which hal_fill_ambient_sound_vtable owns.
//
// THE LAW, same as every sibling fill: ROM slot order, host __fastcall thunks
// that bridge cdecl/__thiscall to the matched bodies, unhosted slots trapped
// by name. Slots 13/14/30 are ActorBase::Virtual34/38 and the SRET
// Actor::OnAimedAtWithEggReturnVec, the trio every Actor-shaped fill traps.
//
// SLOT 16 IS A HOST THUNK on both, the montymole/gate-31 treatment. The
// matched D1 of each (_ZN14EnemySwitchTagD1Ev.cpp / _ZN12EnemySpawnerD1Ev.cpp)
// is an empty-bodied real C++ destructor whose auto-emitted member-dtor and
// base ~Actor calls would resolve to MSVC-mangled names no TU in this build
// defines, so those two files are NOT enrolled; the chain is spelled here
// instead. EnemySwitchTag's chain is MovingCylinderClsn at +0xd4 then
// Actor::~D2 -- exactly what its own D0 body runs before the deallocate;
// EnemySpawner has no members, so its D1 is Actor::~D2 alone (the
// ac_d1_actor_only shape). Slot 17, the deleting destructor, IS a matched flat
// C body on the slice and both are enrolled.

#include <cstdio>
#include <cstdlib>

#include "Actor.h"
#include "dtor_faces_cpp.h"
#include "ActorBase.h"
#include "EnemySwitchTag.h"
#include "EnemySpawner.h"

extern "C" {
/* the shared lifecycle halves, the same arm9 bodies every sibling fill writes */
int  _ZN5Actor19BeforeInitResourcesEv(void *self);            /* slot 1  */
void _ZN5Actor18AfterInitResourcesEj(void *self, unsigned a); /* slot 2  */
int  _ZN5Actor14BeforeBehaviorEv(void *self);                 /* slot 7  */
int  _ZN5Actor12BeforeRenderEv(void *self);                   /* slot 10 */
int  _ZN5Actor13OnYoshiTryEatEv(void *self);                  /* slot 18 */
void _ZN5Actor13OnTurnIntoEggER6Player(void *self, void *p);  /* slot 19 */
int  _ZN5Actor9Virtual50Ev(void *self);                       /* slot 20 */
void _ZN5Actor15OnGroundPoundedERS_(void *self, void *o);     /* slot 21 */
void _ZN5Actor11OnAttacked1ERS_(void *self, void *o);         /* slot 22 */
void _ZN5Actor11OnAttacked2ERS_(void *self, void *o);         /* slot 23 */
void _ZN5Actor8OnKickedERS_(void *self, void *o);             /* slot 24 */
void _ZN5Actor8OnPushedERS_(void *self, void *o);             /* slot 25 */
void _ZN5Actor24OnHitByCannonBlastedCharERS_(void *self, void *o); /* slot 26 */
void _ZN5Actor15OnHitByMegaCharER6Player(void *self, void *p);     /* slot 27 */
void _ZN5Actor19OnHitFromUnderneathERS_(void *self, void *o);      /* slot 28 */
int  _ZN5Actor16OnAimedAtWithEggEv(void *self);                    /* slot 29 */

extern int data_02099f24[];          /* the frame phase the lists are in */
extern unsigned char data_020a4b4c;  /* the spawn spine's own step */
const char *port_actor_class_name(unsigned id);   /* hal/actor_registry */
void port_actor_slot_decline(const char *what);   /* func_02043fdc_hostcopy.cpp */

/* the two classes' own matched bodies, all on slice_gate211.txt */
int  _ZN14EnemySwitchTag13InitResourcesEv(void *self); /* slot 0, extern-C explicit this */
int *_ZN14EnemySwitchTagD0Ev(int *self);               /* slot 17, .c C linkage */
int  _ZN12EnemySpawner16CleanupResourcesEv(void);      /* slot 3, .c C linkage */
int  _ZN12EnemySpawner8BehaviorEv(char *self);         /* slot 6, extern-C explicit this */
int *_ZN12EnemySpawnerD0Ev(int *self);                 /* slot 17, .c C linkage */

/* the D1 chain's sub-object destructor and the base, both C-linkage here */
void *_ZN18MovingCylinderClsnD1Ev(void *);   /* EnemySwitchTag's member at +0xd4 */
void *_ZN5ActorD2Ev(void *);

/* The two arrays the ROM factories install: EnemySwitchTag_Spawn does
   `p[0] = (int)_ZTV14EnemySwitchTag`, EnemySpawner_Spawn the same for its own.
   Defined here, not just declared -- the `int` type and C linkage match the
   `extern int _ZTV*[]` in include/decl_common.h that the factories, the D0
   bodies and the host thunks all read. Thirty-one slots each. */
int _ZTV14EnemySwitchTag[31];
int _ZTV12EnemySpawner[31];
}

/* EnemySwitchTag's D0 stores _ZTV11daESwitch_c, the RTTI base spelling dsd
   dual-names at the same address (vtspan prints both at ov002 0x0210b3e8).
   Both references are C linkage, so point the RTTI spelling at the one host
   array -- the daChoro_Rock_c/daChoropu_c precedent. EnemySpawner needs no
   such row: its D0 spells the table through the VT placeholder, renamed to
   _ZTV12EnemySpawner per-source in port/CMakeLists.txt, and its Spawn spells
   the array name directly. */
#pragma comment(linker, "/alternatename:__ZTV11daESwitch_c=__ZTV14EnemySwitchTag")

// ---- the trap --------------------------------------------------------------
static void es_trap_report(void *self, int slot)
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
#define ES_TRAP(n) \
    static int __fastcall es_trap##n(void *s, void *) \
    { es_trap_report(s, n); return 0; }
/* 13/14 are ActorBase::Virtual34/38; 30 is Actor::OnAimedAtWithEggReturnVec,
   an SRET body no fill's thunk shape models. */
ES_TRAP(13) ES_TRAP(14) ES_TRAP(30)
#undef ES_TRAP

// ---- the shared half (both classes are plain Actor) ------------------------
// Slots 4/5/8/11 hold Actor::BeforeCleanupResources, then ActorBase's
// AfterCleanupResources / AfterBehavior / AfterRender -- the port-wide standing
// substitution ruled correct in port/actorbase_slots_ruling.txt (the ROM's
// three Actor bodies are `ldr ip,[pc]; bx ip` veneers onto ActorBase's).
static int  __fastcall es_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall es_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int  __fastcall es_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall es_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int  __fastcall es_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall es_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
/* Slot 9: neither class overrides Render, so it is ActorBase::Render, the
   no-op base body (ccm_render_base precedent). */
static int  __fastcall es_render_base(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::Render(); }
static int  __fastcall es_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall es_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int  __fastcall es_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int  __fastcall es_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int  __fastcall es_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
static int  __fastcall es_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int  __fastcall es_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int  __fastcall es_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int  __fastcall es_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int  __fastcall es_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int  __fastcall es_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int  __fastcall es_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int  __fastcall es_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int  __fastcall es_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int  __fastcall es_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int  __fastcall es_aimed(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }

// Fill the eleven shared interaction-tail and lifecycle-half slots that are
// identical on any plain-Actor table. Slots 0/3/6/9/12/16/17 are set per class.
static void es_fill_shared(void **vt)
{
    vt[1]  = (void *)es_binit;
    vt[2]  = (void *)es_ainit;
    vt[4]  = (void *)es_bclean;
    vt[5]  = (void *)es_aclean;
    vt[7]  = (void *)es_bbeh;
    vt[8]  = (void *)es_abeh;
    vt[9]  = (void *)es_render_base;
    vt[10] = (void *)es_bren;
    vt[11] = (void *)es_aren;
    vt[12] = (void *)es_pdes;
    vt[13] = (void *)es_trap13;
    vt[14] = (void *)es_trap14;
    vt[15] = (void *)es_heap;
    vt[18] = (void *)es_yoshi;
    vt[19] = (void *)es_egg;
    vt[20] = (void *)es_v50;
    vt[21] = (void *)es_pounded;
    vt[22] = (void *)es_atk1;
    vt[23] = (void *)es_atk2;
    vt[24] = (void *)es_kicked;
    vt[25] = (void *)es_pushed;
    vt[26] = (void *)es_cannon;
    vt[27] = (void *)es_mega;
    vt[28] = (void *)es_under;
    vt[29] = (void *)es_aimed;
    vt[30] = (void *)es_trap30;
}

// ---- ENEMY_SWITCH_TAG's own slots ------------------------------------------
/* Init is extern-C explicit-this (cdecl); Cleanup and Behavior are real
   __thiscall members against include/EnemySwitchTag.h -- the face bridges the
   __fastcall thunk shape to each. */
static int __fastcall est_init(void *s, void *)
{ return _ZN14EnemySwitchTag13InitResourcesEv(s); }
static int __fastcall est_clean(void *s, void *)
{ return ((EnemySwitchTag *)s)->EnemySwitchTag::CleanupResources(); }
static int __fastcall est_behavior(void *s, void *)
{ return ((EnemySwitchTag *)s)->EnemySwitchTag::Behavior(); }
/* Slot 16: the complete-object destructor chain, spelled by hand because the
   matched .cpp D1 is a shadow-class destructor that does not link on host. The
   member is a MovingCylinderClsn at +0xd4 (its own D0 destroys it there), then
   Actor::~D2. No vtable store: nothing dispatches through the object again
   before the caller deallocates it. */
/* slot 16 is the matched src D1 through hal/dtor_faces_cpp.cpp (lane DTOR-FACES-CPP);
   the transcribed thunk that stood here (est_d1) spelled the same chain by hand. */
static int __fastcall est_d0(void *s, void *)
{ return (int)(size_t)_ZN14EnemySwitchTagD0Ev((int *)s); }

// ---- ENEMY_SPAWNER's own slots ---------------------------------------------
/* Init is a real __thiscall member against include/EnemySpawner.h; Cleanup is
   a flat-C no-this body; Behavior is extern-C explicit-this. */
static int __fastcall esp_init(void *s, void *)
{ return ((EnemySpawner *)s)->EnemySpawner::InitResources(); }
static int __fastcall esp_clean(void *, void *)
{ return _ZN12EnemySpawner16CleanupResourcesEv(); }
static int __fastcall esp_behavior(void *s, void *)
{ return _ZN12EnemySpawner8BehaviorEv((char *)s); }
/* Slot 16: EnemySpawner has no members, so the complete-object destructor is
   Actor::~D2 alone (the ac_d1_actor_only shape). */
/* slot 16 is the matched src D1 through hal/dtor_faces_cpp.cpp (lane DTOR-FACES-CPP);
   the transcribed thunk that stood here (esp_d1) spelled the same chain by hand. */
static int __fastcall esp_d0(void *s, void *)
{ return (int)(size_t)_ZN12EnemySpawnerD0Ev((int *)s); }

extern "C" void hal_fill_enemy_switch_tag_vtable(void)
{
    void **vt = (void **)_ZTV14EnemySwitchTag;
    es_fill_shared(vt);
    vt[0]  = (void *)est_init;
    vt[3]  = (void *)est_clean;
    vt[6]  = (void *)est_behavior;
    vt[16] = (void *)hal_cppd1_EnemySwitchTag;
    vt[17] = (void *)est_d0;
}

extern "C" void hal_fill_enemy_spawner_vtable(void)
{
    void **vt = (void **)_ZTV12EnemySpawner;
    es_fill_shared(vt);
    vt[0]  = (void *)esp_init;
    vt[3]  = (void *)esp_clean;
    vt[6]  = (void *)esp_behavior;
    vt[16] = (void *)hal_cppd1_EnemySpawner;
    vt[17] = (void *)esp_d0;
}
