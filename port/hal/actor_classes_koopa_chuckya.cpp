// GATE 182: KOOPA (203) and CHUCKYA (190), Bob-omb Battlefield's last two
// unregistered NPCs, both in ov062 -- the overlay gate 32 already mounted whole
// per symbol for KOOPA_THE_QUICK (port/ov062_syms.txt). This gate is HAL-only:
// no new mount, no new syms lines, every data symbol both classes reach is
// already in the pack.
//
// Same law as hal/actor_classes_ov064_gate178.cpp: ROM slot order, __fastcall
// thunks that call the class's C body or a QUALIFIED base method, slots 13/14
// (ActorBase::Virtual34/38, not linked) and slot 30 (the SRET
// OnAimedAtWithEggReturnVec) trapped by name, the interaction tail bound to the
// Actor base bodies.
//
// ---- THE SPAWN CHAIN; THE NAMES DO NOT CROSS -------------------------------
//
// ACTOR_SPAWN_TABLE[190] (arm9 0x02090b5c) -> Chuckya_SpawnInfo (ov062
// 0x0211d9bc, +4 halfword 190); its +0 word is Chuckya_Spawn (0x02117478),
// which allocates 0x438 (1080) bytes, runs the Enemy ctor, and installs
// _ZTV7Chuckya (0x0211d9e0) as its ONLY vptr write -- the real table, so NO
// reseat wrapper. Members: MovingCylinderClsn +0x110, WithMeshClsn +0x144,
// ModelAnim +0x300, ShadowModel +0x368 (read out of the matched Chuckya_Spawn.c,
// which the ROM's own ctor-call sequence at 0x0211749c..0x021174b4 confirms).
//
// ACTOR_SPAWN_TABLE[203] (arm9 0x02090b90) -> Koopa_SpawnInfo (0x0211da74,
// +4 halfword 203); +0 = Koopa_Spawn (0x0211970c), 0x3d0 (976) bytes, installs
// _ZTV5Koopa (0x0211dab4) -- the real table, no reseat wrapper. Members:
// MovingCylinderClsn +0x110, WithMeshClsn +0x144, ModelAnim +0x300, ShadowModel
// +0x364. KOOPA_SMALL (204) shares this class (KoopaSmall_Spawn installs the
// SAME _ZTV5Koopa, same 0x3d0) but the level-6 census names no id 204, so it is
// deliberately NOT registered this gate -- one fill-0-style row later if ever
// needed.
//
// ---- BOTH TABLES ARE THE PLAIN 31-SLOT ENEMY/ACTOR SHAPE -------------------
//
// Read off each table's own reloc run (config/arm9/overlays/ov062/relocs.txt),
// slot 0 = InitResources at the vptr word, typeinfo one word before:
//   _ZTV7Chuckya  0x0211d9e0..0x0211da58: overrides 0/3/6/9/12/16/17 and
//     slot 29 (func_ov062_02117470, its own OnAimedAtWithEgg -- returns
//     0xCA000); slots 18..28 the Actor interaction tail (0x02010160..
//     0x0201012c), slot 30 the SRET 0x020100dc.
//   _ZTV5Koopa    0x0211dab4..0x0211db2c: overrides 0/3/6/9/12/16/17 and
//     slots 18 (func_ov062_021196a8, OnYoshiTryEat: 5 or 6 by mModelIndex),
//     19 (func_ov062_02119628, OnTurnIntoEgg: egg-coin bookkeeping + kill) and
//     29 (func_ov062_02119608, OnAimedAtWithEgg: 0x46000/0x25800 by actor id);
//     slot 30 the SRET. The next symbol after 0x0211db2c is data_ov062_0211db30
//     -- KOOPA_THE_QUICK's state static, NOT a 32nd slot.
//
// ---- THE D-TORS STAY IN THE SLICE ------------------------------------------
//
// All four dtors store the class's OWN real table (relocs 0x02115f24/0x02115f7c
// -> 0x0211d9e0; 0x02117510/0x02117568 -> 0x0211dab4), never a shared
// placeholder -- the LavaBubble/FallBlockWf case, no host thunks. Teardown
// high-address-first: ShadowModel +0x368/+0x364, ModelAnim +0x300, WithMeshClsn
// +0x144, MovingCylinderClsn +0x110, then the Enemy base D2 func_ov002_020aed18
// (gate 16); each D0 adds Memory::Deallocate on data_020a0eac. Each D0 spells
// the table by its RTTI alias (_ZTV10daHolhei_c / _ZTV8daNknk_c, the same ROM
// address as the class name) -- aliased below onto the host arrays, the
// daBbl_c/daWater_Hakidasi_c reading.
//
// ---- CHUCKYA'S DISPATCHERS ARE HOST COPIES; KOOPA'S BEHAVIOR IS NOT --------
//
// Chuckya drives a TWENTY-state PMF machine (source statics ov062
// 0x0211d900..0x0211d998, copied by __sinit_ov062_0211cf30 into the ten
// 16-byte State objects at 0x0211de70..0x0211df00). Chuckya_ChangeState and
// Chuckya::Behavior both form the pointer-to-member over a FORWARD-DECLARED
// struct -- the MSVC general-representation fault, the KnockDownPlank/MontyMole
// /Scuttlebug case -- so both are host copies in
// port/unmatched/KoopaChuckya_StateDispatch.cpp, and the twenty source records
// are seated there (port_chuckya_states_seat) BEFORE the sinit copies them.
// Koopa::Behavior dispatches NO pointer-to-member (a plain switch machine) and
// stays in the slice as a real method, faced below.
//
// ---- RENDER: KOOPA IS THE SLOT-5 COLLISION, CHUCKYA IS DIRECT --------------
//
// Koopa::Render dispatches its ModelAnim at +0x300 through a six-virtual local
// shadow (slot 5) -- the Whomp/Scuttlebug/BobOmb collision -- so it is a host
// copy in port/unmatched/Koopa_Render.cpp, the KoopaTheQuick treatment down to
// the +0x300 offset. Chuckya::Render (.c) calls _ZN5Model6RenderEPK7Vector3 by
// its Itanium C name directly, no shadow -- the KingBobOmb case -- and stays in
// the slice.
//
// ---- THE ONE HOLE, STUBBED LOUD --------------------------------------------
//
// func_ov062_02117724 (0x270 bytes, KOOPA's state-3 shell-dust Particle helper)
// is UNMATCHED; three matched Koopa helpers bl into it. A name-and-abort stub
// in the StateDispatch file satisfies the link and names the hole if a fresh
// BoB boot ever reaches it -- the Rabbit-0x0212b8dc/Painting-0x021261f4
// precedent. CHUCKYA never reaches it.
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
void port_enemy_death_states_seat(void);  /* unmatched/Enemy_UpdateDeath.cpp */

/* ---- CHUCKYA (190) / daHolhei_c: the C-linkage bodies (matched src) ----
   Behavior is NOT here -- it is the forward-declared-struct PMF dispatcher,
   host-copied in unmatched/KoopaChuckya_StateDispatch.cpp with
   Chuckya_ChangeState. Render IS here: the .c calls Model::Render by its
   Itanium C name, no shadow, and stays in the slice. */
int _ZN7Chuckya13InitResourcesEv(void *self);    /* slot 0,  .c */
int _ZN7Chuckya16CleanupResourcesEv(void *self); /* slot 3,  .c */
int _ZN7Chuckya8BehaviorEv(void *self);          /* slot 6,  HOST COPY */
int _ZN7Chuckya6RenderEv(void *self);            /* slot 9,  .c, direct call */
void _ZN7Chuckya16OnPendingDestroyEv(void);      /* slot 12, .c, empty (void) */
int *_ZN7ChuckyaD1Ev(int *self);                 /* slot 16, .c, own table */
int *_ZN7ChuckyaD0Ev(int *self);                 /* slot 17, .c, own table */
int func_ov062_02117470(void);                   /* slot 29, returns 0xCA000 */
void *Chuckya_Spawn(void);                       /* installs _ZTV7Chuckya itself */

/* ---- KOOPA (203) / daNknk_c: the bodies ----
   Init/Cleanup/Behavior are real MSVC .cpp methods against include/Koopa.h,
   faced below. Render is the ModelAnim slot-5 collision -- host copy in
   unmatched/Koopa_Render.cpp. The rest are C linkage in the slice. */
int _ZN5Koopa13InitResourcesEv(void *self);      /* slot 0,  face below */
int _ZN5Koopa16CleanupResourcesEv(void *self);   /* slot 3,  face below */
int _ZN5Koopa8BehaviorEv(void *self);            /* slot 6,  face below */
int _ZN5Koopa6RenderEv(void *self);              /* slot 9,  HOST COPY */
void _ZN5Koopa16OnPendingDestroyEv(void);        /* slot 12, .c, empty (void) */
int *_ZN5KoopaD1Ev(int *self);                   /* slot 16, .c, own table */
int *_ZN5KoopaD0Ev(int *self);                   /* slot 17, .c, own table */
int func_ov062_021196a8(void *self);             /* slot 18, OnYoshiTryEat */
void func_ov062_02119628(void *self, void *p);   /* slot 19, OnTurnIntoEgg */
int func_ov062_02119608(void *self);             /* slot 29, OnAimedAtWithEgg */
void *Koopa_Spawn(void);                         /* installs _ZTV5Koopa itself */

/* the two derived vtables, HOST arrays this file fills; 31 slots each. `int[]`
   with C linkage matches the `extern int _ZTV..[]` decls in include/decl_common.h
   that the .c factories and D-tors read (the montymole reading). */
int _ZTV7Chuckya[31];
int _ZTV5Koopa[31];
}

/* Each D0 spells its class's table by the RTTI alias -- the SAME ROM address
   (_ZTV10daHolhei_c = _ZTV7Chuckya = 0x0211d9e0; _ZTV8daNknk_c = _ZTV5Koopa =
   0x0211dab4). Point both at the host arrays -- the daBbl_c reading. */
#pragma comment(linker, "/alternatename:__ZTV10daHolhei_c=__ZTV7Chuckya")
#pragma comment(linker, "/alternatename:__ZTV8daNknk_c=__ZTV5Koopa")

/* Shared-base sibling spellings: three ov062 bodies whose CALLERS spell them
   by the sibling overlays' names (dsd names a shared-image function under the
   overlay that saw it first). Same address family, same cdecl bodies -- alias
   the sibling names onto the ov062 TUs this gate slices. */
#pragma comment(linker, "/alternatename:_func_ov066_02118cdc=_func_ov062_02118cdc")
#pragma comment(linker, "/alternatename:_func_ov066_02118a50=_func_ov062_02118a50")
#pragma comment(linker, "/alternatename:_func_ov065_02117994=_func_ov062_02117994")

/* Particle::RunningSlidingDustAt, spelled as the MSVC static-member mangle by
   func_ov062_02118004.cpp; the body is the C-named matched TU (slice_gate29).
   Static member = plain cdecl, alias is legal. */
#pragma comment(linker, "/alternatename:?RunningSlidingDustAt@Particle@@SAXHHH@Z=__ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_")

/* The C++/C data-linkage bridges, the montymole/painting reading. Koopa's .cpp
   InitResources/CleanupResources spell the three model/animation file tables as
   `SharedFilePtr *[]` OUTSIDE extern "C", which MSVC mangles; the ov062 mount
   (ovdata.py) emits only the C name (_data_...). Same for the two symbols
   func_ov062_021164e8.cpp (a Chuckya state body) spells as `void *[]` / `int`
   outside extern "C". Alias each mangled spelling onto the C symbol. */
#pragma comment(linker, "/alternatename:?data_ov062_0211ced8@@3PAPAUSharedFilePtr@@A=_data_ov062_0211ced8")
#pragma comment(linker, "/alternatename:?data_ov062_0211cee0@@3PAPAUSharedFilePtr@@A=_data_ov062_0211cee0")
#pragma comment(linker, "/alternatename:?data_ov062_0211cee8@@3PAPAUSharedFilePtr@@A=_data_ov062_0211cee8")
#pragma comment(linker, "/alternatename:?data_ov062_0211de00@@3PAPAXA=_data_ov062_0211de00")
#pragma comment(linker, "/alternatename:?data_ov062_0211dec0@@3HA=_data_ov062_0211dec0")
/* Already bridged elsewhere (kept here as a map, NOT re-declared):
   ?data_02082128@@3UMatrix4x3@@A   hal/bob_enemy_bridges.cpp (Chuckya's two
                                    shadow-matrix TUs spell Matrix4x3)
   ?data_0209f2f8@@3CA, ?data_0209d684@@3EA, ?data_0209e650@@3HA,
   ?data_0209d4c8@@3PADA            hal/cxx_aliases.cpp / bob_enemy_bridges.cpp
   ?SetAnim@ModelAnim@@QAEXPAUBCA_File@@HHI@Z
                                    hal/bob_enemy_shadow_faces.cpp (the local
                                    shadow func_ov062_021164e8.cpp declares) */
/* If the link reports an unresolved ?data_0209e650@@3PAHA (the int[] spelling
   two Chuckya state .cpp TUs use -- both looked extern-"C"-wrapped, so this
   should stay dead), uncomment: */
/* #pragma comment(linker, "/alternatename:?data_0209e650@@3PAHA=_data_0209e650") */

// ---- the trap --------------------------------------------------------------
static void kc182_trap_report(void *self, int slot)
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
#define KC182_TRAP(n) \
    static int __fastcall kc182_trap##n(void *s, void *) \
    { kc182_trap_report(s, n); return 0; }
/* 13/14 are ActorBase::Virtual34/38 (not linked, the sibling trap); 30 is the
   SRET OnAimedAtWithEggReturnVec no thunk shape models. */
KC182_TRAP(13) KC182_TRAP(14) KC182_TRAP(30)
#undef KC182_TRAP

// ---- the shared 0..30 half -------------------------------------------------
static int __fastcall kc182_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall kc182_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall kc182_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall kc182_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall kc182_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall kc182_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall kc182_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall kc182_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall kc182_pdes_base(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall kc182_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall kc182_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
static int __fastcall kc182_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall kc182_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall kc182_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall kc182_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall kc182_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall kc182_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall kc182_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall kc182_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall kc182_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall kc182_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall kc182_aimed_actor(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }   /* slot 29, Actor's own default */

/* Fill slots 1..30 with the shared bodies. The callers write 0/3/6/9/12/16/17
   and their own interaction overrides (Chuckya 29; Koopa 18/19/29). */
static void kc182_fill_shared_0_30(void **vt)
{
    vt[1]  = (void *)kc182_binit;
    vt[2]  = (void *)kc182_ainit;
    vt[4]  = (void *)kc182_bclean;
    vt[5]  = (void *)kc182_aclean;
    vt[7]  = (void *)kc182_bbeh;
    vt[8]  = (void *)kc182_abeh;
    vt[10] = (void *)kc182_bren;
    vt[11] = (void *)kc182_aren;
    vt[12] = (void *)kc182_pdes_base;
    vt[13] = (void *)kc182_trap13;
    vt[14] = (void *)kc182_trap14;
    vt[15] = (void *)kc182_heap;
    vt[18] = (void *)kc182_yoshi;
    vt[19] = (void *)kc182_egg;
    vt[20] = (void *)kc182_v50;
    vt[21] = (void *)kc182_pounded;
    vt[22] = (void *)kc182_atk1;
    vt[23] = (void *)kc182_atk2;
    vt[24] = (void *)kc182_kicked;
    vt[25] = (void *)kc182_pushed;
    vt[26] = (void *)kc182_cannon;
    vt[27] = (void *)kc182_mega;
    vt[28] = (void *)kc182_under;
    vt[29] = (void *)kc182_aimed_actor;
    vt[30] = (void *)kc182_trap30;
}

// ============================================================================
// CHUCKYA (190) -- daHolhei_c, 31 slots
// ============================================================================
static int __fastcall chk_init(void *s, void *)
{ return _ZN7Chuckya13InitResourcesEv(s); }
static int __fastcall chk_clean(void *s, void *)
{ return _ZN7Chuckya16CleanupResourcesEv(s); }
static int __fastcall chk_behavior(void *s, void *)
{ return _ZN7Chuckya8BehaviorEv(s); }        /* HOST COPY (PMF dispatch) */
static int __fastcall chk_render(void *s, void *)
{ port_actor_render_probe("CHUCKYA", (char *)s + 0x300);
  return _ZN7Chuckya6RenderEv(s); }           /* .c, direct Model::Render call */
static int __fastcall chk_pdes(void *s, void *)
{ (void)s; _ZN7Chuckya16OnPendingDestroyEv(); return 0; }
static int __fastcall chk_d1(void *s, void *)
{ return (int)(size_t)_ZN7ChuckyaD1Ev((int *)s); }
static int __fastcall chk_d0(void *s, void *)
{ return (int)(size_t)_ZN7ChuckyaD0Ev((int *)s); }
static int __fastcall chk_aimed(void *s, void *)
{ (void)s; return func_ov062_02117470(); }    /* slot 29, its own, no args */

extern "C" void hal_fill_chuckya_vtable(void)
{
    void **vt = (void **)_ZTV7Chuckya;
    port_enemy_death_states_seat();
    kc182_fill_shared_0_30(vt);
    vt[0]  = (void *)chk_init;
    vt[3]  = (void *)chk_clean;
    vt[6]  = (void *)chk_behavior;
    vt[9]  = (void *)chk_render;
    vt[12] = (void *)chk_pdes;
    vt[16] = (void *)chk_d1;
    vt[17] = (void *)chk_d0;
    vt[29] = (void *)chk_aimed;   /* Chuckya's own OnAimedAtWithEgg override */
}

// ============================================================================
// KOOPA (203) -- daNknk_c, 31 slots (KOOPA_SMALL 204 shares this table;
// not registered this gate)
// ============================================================================
static int __fastcall kp_init(void *s, void *)
{ return _ZN5Koopa13InitResourcesEv(s); }
static int __fastcall kp_clean(void *s, void *)
{ return _ZN5Koopa16CleanupResourcesEv(s); }
static int __fastcall kp_behavior(void *s, void *)
{ return _ZN5Koopa8BehaviorEv(s); }
static int __fastcall kp_render(void *s, void *)
{ port_actor_render_probe("KOOPA", (char *)s + 0x300);
  return _ZN5Koopa6RenderEv(s); }             /* HOST COPY (slot-5 collision) */
static int __fastcall kp_pdes(void *s, void *)
{ (void)s; _ZN5Koopa16OnPendingDestroyEv(); return 0; }
static int __fastcall kp_d1(void *s, void *)
{ return (int)(size_t)_ZN5KoopaD1Ev((int *)s); }
static int __fastcall kp_d0(void *s, void *)
{ return (int)(size_t)_ZN5KoopaD0Ev((int *)s); }
static int __fastcall kp_yoshi(void *s, void *)
{ return func_ov062_021196a8(s); }            /* slot 18, its own: 5 or 6 */
static int __fastcall kp_egg(void *s, void *, void *p)
{ func_ov062_02119628(s, p); return 0; }      /* slot 19, its own */
static int __fastcall kp_aimed(void *s, void *)
{ return func_ov062_02119608(s); }            /* slot 29, its own */

extern "C" void hal_fill_koopa_vtable(void)
{
    void **vt = (void **)_ZTV5Koopa;
    port_enemy_death_states_seat();
    kc182_fill_shared_0_30(vt);
    vt[0]  = (void *)kp_init;
    vt[3]  = (void *)kp_clean;
    vt[6]  = (void *)kp_behavior;
    vt[9]  = (void *)kp_render;
    vt[12] = (void *)kp_pdes;
    vt[16] = (void *)kp_d1;
    vt[17] = (void *)kp_d0;
    vt[18] = (void *)kp_yoshi;
    vt[19] = (void *)kp_egg;
    vt[29] = (void *)kp_aimed;
}

// ---- method faces ----------------------------------------------------------
// The C-named references the vtable thunks take onto definitions that are real
// MSVC methods against include/Koopa.h (the Goomba/BowserPuzzle reading).
// Koopa::Render is NOT faced -- it is a host copy (unmatched/Koopa_Render.cpp).
// Chuckya has no .cpp methods: its lifecycle bodies are .c C linkage, its
// Behavior/ChangeState are host copies (extern "C" in the StateDispatch file).
#include "Koopa.h"
extern "C" {
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN5Koopa13InitResourcesEv(void *self)
{ return ((Koopa *)self)->Koopa::InitResources(); }
#else
int _ZN5Koopa13InitResourcesEv(void *self);  /* Linux: real symbol from src/_ZN5Koopa13InitResourcesEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN5Koopa16CleanupResourcesEv(void *self)
{ return ((Koopa *)self)->Koopa::CleanupResources(); }
#else
int _ZN5Koopa16CleanupResourcesEv(void *self);  /* Linux: real symbol from src/_ZN5Koopa16CleanupResourcesEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN5Koopa8BehaviorEv(void *self)
{ return ((Koopa *)self)->Koopa::Behavior(); }
#else
int _ZN5Koopa8BehaviorEv(void *self);  /* Linux: real symbol from src/_ZN5Koopa8BehaviorEv */
#endif /* _WIN32 */
}
