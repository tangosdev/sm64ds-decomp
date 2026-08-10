// GATE 200: PUSH_BLOCK (306) + MUGEN_BGM (351), both ov002 (already
// mounted). Four data symbols joined the ov002 mount for this gate
// (PowerFlower_SpawnInfo, _ZTV9PushBlock, MugenBgm_SpawnInfo, _ZTV8MugenBgm);
// everything else the classes touch was already mounted or hosted.
//
// THE ID-306 DECOY, re-derived fresh from relocs (the gate-199 rule: never
// trust the brief's framing): id 306's SpawnInfo is PowerFlower_SpawnInfo
// (0x021097a0), whose spawn-function word relocs to PowerFlower_Spawn
// (0x020b9e0c) -- and THAT matched src news 972 bytes and installs
// _ZTV9PushBlock itself (the SpawnInfo+0x24 vtable rule is ABSENT for this
// id; castle2-scope's correction #3). dsd's "PowerFlower" name lost a
// naming race (actor_renames_report.txt:697); the class is PushBlock,
// RTTI/parent table _ZTV18daObjPowerUpItem_c shares the same address.
//
// BOTH TABLES ARE 31-SLOT ACTOR, the ccm/IceSlideManager base family
// (slot 12 base = 0x02043ac0 ActorBase::OnPendingDestroy, 13/14 traps,
// 15 heap). Own slots, read out of the two tables' relocations:
//   PUSH_BLOCK: 0 Init, 3 Cleanup, 6 Behavior, 9 Render (HOST COPY,
//     plain-Model slot-5 shadow x2, ModelAnim_Renders.cpp), 16 D1, 17 D0
//     (host thunks below), 18 OnYoshiTryEat (own: func_ov002_020b9e04,
//     returns 5 -- the daObjPowerUpItem_c inheritance showing through).
//   MUGEN_BGM: 0 Init, 3 Cleanup, 6 Behavior, 9 Render, 12 OnPendingDestroy
//     (all matched src; Render/Cleanup/OnPendingDestroy are extern-C .c
//     stubs), 16 D1, 17 D0 (host thunks below).
//
// PUSH_BLOCK's 3-state PMF machine dispatches through data_ov002_021097bc;
// the installer/tick host copies and the dest seat live in
// port/unmatched/PushBlock_StateDispatch.cpp (called from the fill below).
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
void port_actor_slot_decline(const char *what);   /* func_02043fdc.cpp */
void port_actor_render_probe(const char *cls, void *model); /* hal/actor_classes */

/* PUSH_BLOCK own bodies */
int _ZN9PushBlock13InitResourcesEv(void *self);    /* slot 0, faced (real C++ method) */
int _ZN9PushBlock16CleanupResourcesEv(void);       /* slot 3, extern-C .c, ignores this */
int _ZN9PushBlock8BehaviorEv(void *self);          /* slot 6, faced (real C++ method) */
int _ZN9PushBlock6RenderEv(void *self);            /* slot 9, HOST COPY
                                                       (ModelAnim_Renders.cpp) */
int func_ov002_020b9e04(void);                     /* slot 18, returns 5 */
void port_pushblock_states_seat(void);             /* PushBlock_StateDispatch.cpp */
extern int _ZTV9PushBlock[];                       /* ov002 mount, 31 slots */

/* MUGEN_BGM own bodies (extern-C .c except the two faced .cpp methods) */
int _ZN8MugenBgm13InitResourcesEv(void *self);     /* slot 0, faced */
int _ZN8MugenBgm16CleanupResourcesEv(void);        /* slot 3, .c, ignores this */
int _ZN8MugenBgm8BehaviorEv(void *self);           /* slot 6, faced */
int _ZN8MugenBgm6RenderEv(void);                   /* slot 9, .c stub, ignores this */
void _ZN8MugenBgm16OnPendingDestroyEv(void);       /* slot 12, .c stub (no arg) */
extern int _ZTV8MugenBgm[];                        /* ov002 mount, 31 slots */

/* D1/D0 teardown members, all hosted seams. PushBlock's chain is read out
   of its OWN matched D0 body (src/_ZN9PushBlockD0Ev.c): WithMeshClsn
   +0x200, MovingCylinderClsn +0x1cc, ShadowModel +0x174, Model +0x124,
   Model +0xd4, then Actor's D2 -- high-address member first, exactly the
   order Spawn constructs in reverse. MugenBgm (212 bytes, no members past
   Actor per its own Spawn) is bare Actor::D2. Neither matched D1/D0 TU is
   compiled: PushBlock's D1 is a real MSVC destructor over local struct
   decls (unlinkable against the host seams) and its D0 stores the parent
   table; host thunks spell the chain instead, the gate-199 treatment. */
void _ZN12WithMeshClsnD1Ev(void *self);
void _ZN18MovingCylinderClsnD1Ev(void *self);
void _ZN11ShadowModelD1Ev(void *self);
void _ZN5ModelD1Ev(void *self);
void *_ZN5ActorD2Ev(void *self);
void _ZN6Memory10DeallocateEPvP4Heap(void *, void *);
extern void *data_020a0eac;   /* the game heap */
}
#pragma comment(linker, "/alternatename:__ZTV18daObjPowerUpItem_c=__ZTV9PushBlock")

// ---- the shared 1..30 half, both 31-slot Actor tables share it -----------
static void g200_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: ov002g200 vtable slot %d is not hosted (actor id "
                 "%u %s)\n", slot, id, port_actor_class_name(id));
    { static char _m[128];
      std::snprintf(_m, sizeof _m, "unhosted ov002g200 vtable slot %d on id "
                    "%u %s", slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
static int __fastcall g200_trap13(void *s, void *) { g200_trap_report(s, 13); return 0; }
static int __fastcall g200_trap14(void *s, void *) { g200_trap_report(s, 14); return 0; }
static int __fastcall g200_trap30(void *s, void *) { g200_trap_report(s, 30); return 0; }

static int __fastcall g200_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall g200_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall g200_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall g200_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall g200_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall g200_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall g200_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall g200_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall g200_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall g200_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall g200_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
static int __fastcall g200_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall g200_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall g200_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall g200_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall g200_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall g200_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall g200_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall g200_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall g200_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall g200_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall g200_aimed(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }

/* Fills slots 1,2,4,5,7,8,10,11,12,13,14,15,18..30 -- the standard 31-slot
   Actor shared half. The callers overwrite their own 0/3/6/9/16/17;
   PushBlock also overwrites 18 (own OnYoshiTryEat), MugenBgm 12 (own
   OnPendingDestroy).

   THE POINTER IS VOLATILE ON PURPOSE. MSVC 19.44 x86 /O2 (no /GL involved)
   deletes the call to this function from BOTH fill entry points and drops
   all 24 shared-half stores from the object file -- proven by dumpbin on
   the TU's .obj, and reduced to a minimal repro: a static filler storing
   through its pointer parameter, called from two sites with two distinct
   extern-array arguments, loses the calls; one call site compiles
   correctly. The crash was PUSH_BLOCK dispatching slot 1 into a raw DS
   address (0x02011268) the fill never overwrote. Retyping the externs to
   match the defining TU (u8[]) does not cure it; volatile-qualifying the
   stores does, verified in the disassembly. */
static void g200_fill_shared(void *volatile *vt)
{
    vt[1]  = (void *)g200_binit;
    vt[2]  = (void *)g200_ainit;
    vt[4]  = (void *)g200_bclean;
    vt[5]  = (void *)g200_aclean;
    vt[7]  = (void *)g200_bbeh;
    vt[8]  = (void *)g200_abeh;
    vt[10] = (void *)g200_bren;
    vt[11] = (void *)g200_aren;
    vt[12] = (void *)g200_pdes;
    vt[13] = (void *)g200_trap13;
    vt[14] = (void *)g200_trap14;
    vt[15] = (void *)g200_heap;
    vt[18] = (void *)g200_yoshi;
    vt[19] = (void *)g200_egg;
    vt[20] = (void *)g200_v50;
    vt[21] = (void *)g200_pounded;
    vt[22] = (void *)g200_atk1;
    vt[23] = (void *)g200_atk2;
    vt[24] = (void *)g200_kicked;
    vt[25] = (void *)g200_pushed;
    vt[26] = (void *)g200_cannon;
    vt[27] = (void *)g200_mega;
    vt[28] = (void *)g200_under;
    vt[29] = (void *)g200_aimed;
    vt[30] = (void *)g200_trap30;
}

// ============================================================================
// PUSH_BLOCK (306), 31 slots.
// ============================================================================
static int __fastcall pb_init(void *s, void *)
{ return _ZN9PushBlock13InitResourcesEv(s); }
static int __fastcall pb_clean(void *s, void *)
{ (void)s; return _ZN9PushBlock16CleanupResourcesEv(); }
static int __fastcall pb_behavior(void *s, void *)
{ return _ZN9PushBlock8BehaviorEv(s); }
static int __fastcall pb_render(void *s, void *)
{ port_actor_render_probe("PUSH_BLOCK", (char *)s + 0xd4);
  return _ZN9PushBlock6RenderEv(s); }
static int __fastcall pb_yoshi(void *s, void *)
{ (void)s; return func_ov002_020b9e04(); }
static int __fastcall pb_d1(void *s, void *)
{
    char *t = (char *)s;
    *(void **)t = (void *)_ZTV9PushBlock;
    _ZN12WithMeshClsnD1Ev(t + 0x200);
    _ZN18MovingCylinderClsnD1Ev(t + 0x1cc);
    _ZN11ShadowModelD1Ev(t + 0x174);
    _ZN5ModelD1Ev(t + 0x124);
    _ZN5ModelD1Ev(t + 0xd4);
    _ZN5ActorD2Ev(t);
    return (int)(size_t)s;
}
static int __fastcall pb_d0(void *s, void *)
{
    char *t = (char *)s;
    *(void **)t = (void *)_ZTV9PushBlock;
    _ZN12WithMeshClsnD1Ev(t + 0x200);
    _ZN18MovingCylinderClsnD1Ev(t + 0x1cc);
    _ZN11ShadowModelD1Ev(t + 0x174);
    _ZN5ModelD1Ev(t + 0x124);
    _ZN5ModelD1Ev(t + 0xd4);
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return (int)(size_t)s;
}

extern "C" void hal_fill_pushblock_vtable(void)
{
    /* seat the three dispatch cells __sinit_ov002_021014e4 left as DS code
       addresses BEFORE any spawn can dispatch them (the gate-51 order). */
    port_pushblock_states_seat();
    void *volatile *vt = (void *volatile *)_ZTV9PushBlock;
    g200_fill_shared(vt);
    vt[0]  = (void *)pb_init;
    vt[3]  = (void *)pb_clean;
    vt[6]  = (void *)pb_behavior;
    vt[9]  = (void *)pb_render;
    vt[16] = (void *)pb_d1;
    vt[17] = (void *)pb_d0;
    vt[18] = (void *)pb_yoshi;   /* own override: return 5, Yoshi cannot eat it */
}

// ============================================================================
// MUGEN_BGM (351), 31 slots. The endless-stairs music trigger: 212 bytes,
// no members past Actor, all its own slots matched src.
// ============================================================================
static int __fastcall mb_init(void *s, void *)
{ return _ZN8MugenBgm13InitResourcesEv(s); }
static int __fastcall mb_clean(void *s, void *)
{ (void)s; return _ZN8MugenBgm16CleanupResourcesEv(); }
static int __fastcall mb_behavior(void *s, void *)
{ return _ZN8MugenBgm8BehaviorEv(s); }
static int __fastcall mb_render(void *s, void *)
{ (void)s; return _ZN8MugenBgm6RenderEv(); }
static int __fastcall mb_pdes(void *s, void *)
{ (void)s; _ZN8MugenBgm16OnPendingDestroyEv(); return 0; }
static int __fastcall mb_d1(void *s, void *)
{
    char *t = (char *)s;
    *(void **)t = (void *)_ZTV8MugenBgm;
    _ZN5ActorD2Ev(t);
    return (int)(size_t)s;
}
static int __fastcall mb_d0(void *s, void *)
{
    char *t = (char *)s;
    *(void **)t = (void *)_ZTV8MugenBgm;
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return (int)(size_t)s;
}

extern "C" void hal_fill_mugen_bgm_vtable(void)
{
    void *volatile *vt = (void *volatile *)_ZTV8MugenBgm;
    g200_fill_shared(vt);
    vt[0]  = (void *)mb_init;
    vt[3]  = (void *)mb_clean;
    vt[6]  = (void *)mb_behavior;
    vt[9]  = (void *)mb_render;
    vt[12] = (void *)mb_pdes;
    vt[16] = (void *)mb_d1;
    vt[17] = (void *)mb_d0;
}

// ---- method faces --------------------------------------------------------
// PushBlock::InitResources/Behavior and MugenBgm::InitResources/Behavior are
// real MSVC methods against their shared headers (?InitResources@PushBlock@@
// ..., not _ZN9PushBlock...) -- faced here, the IceSheet/SwitchPillar recipe.
// PushBlock::Render is NOT faced: it is a host copy (two plain-Model slot-5
// shadows, port/unmatched/ModelAnim_Renders.cpp), declared extern "C" above.
#include "PushBlock.h"
#include "MugenBgm.h"
extern "C" {
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN9PushBlock13InitResourcesEv(void *self)
{ return ((PushBlock *)self)->PushBlock::InitResources(); }
#else
int _ZN9PushBlock13InitResourcesEv(void *self);  /* Linux: real symbol from src/_ZN9PushBlock13InitResourcesEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN9PushBlock8BehaviorEv(void *self)
{ return ((PushBlock *)self)->PushBlock::Behavior(); }
#else
int _ZN9PushBlock8BehaviorEv(void *self);  /* Linux: real symbol from src/_ZN9PushBlock8BehaviorEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN8MugenBgm13InitResourcesEv(void *self)
{ return ((MugenBgm *)self)->MugenBgm::InitResources(); }
#else
int _ZN8MugenBgm13InitResourcesEv(void *self);  /* Linux: real symbol from src/_ZN8MugenBgm13InitResourcesEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN8MugenBgm8BehaviorEv(void *self)
{ return ((MugenBgm *)self)->MugenBgm::Behavior(); }
#else
int _ZN8MugenBgm8BehaviorEv(void *self);  /* Linux: real symbol from src/_ZN8MugenBgm8BehaviorEv */
#endif /* _WIN32 */
}
