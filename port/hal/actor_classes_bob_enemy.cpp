// GATE 32: the vtables of Bob-omb Battlefield's living cast.
//
// Same law as hal/actor_classes.cpp -- ROM slot order, __fastcall thunks that
// call QUALIFIED, unhosted slots trap by name -- with one thing every class
// here has that no class before it did.
//
// ---- THIRTY-ONE SLOTS ------------------------------------------------------
//
// These are Actor subclasses through Enemy, and Actor appends thirteen
// virtuals of its own past the eighteen ActorBase declares. The tables in the
// ROM are 0x7c bytes, not 0x50: read _ZTV6Goomba (ov084 0x02130948) or
// _ZTV6BobOmb (ov102 0x0214e558) with their relocations applied and slot 20 is
// Actor::Virtual50, 21..28 are the eight combat hooks, 29 is the class's own
// OnAimedAtWithEgg and 30 is Actor::OnAimedAtWithEggReturnVec.
//
// Every class the port carried before declared `void *_ZTV<X>[20]`, which was
// true of what those classes needed and is not true here: a Goomba is reached
// through slot 21 the moment Mario ground-pounds one. So the arrays below are
// [31] and ac31_fill_shared writes all of the shared ones.
//
// THE CALL SITES COUNT ROM SLOTS. ov002's combat paths (func_ov002_020b36b4
// and its family) dispatch through LOCAL SHADOW CLASSES declaring thirty-one
// virtuals, so the numbering these arrays are filled in is the numbering the
// caller uses. The header-compiled direction is the one that would diverge --
// MSVC spends one slot on the destructor where Itanium spends two, so its
// OnYoshiTryEat is 17 where the ROM's is 18 -- and no TU in the port's build
// dispatches an Actor virtual that way.
//
// SLOT 30 TRAPS ON EVERY CLASS HERE. Actor::OnAimedAtWithEggReturnVec returns
// a Vector3 BY VALUE, and the ROM's ABI puts the sret pointer in r0 with
// `this` in r1 while MSVC's __thiscall pushes sret on the stack behind ecx.
// A thunk cannot bridge that without a shape the port has no caller for:
// nothing aims a Yoshi egg at anything while the character is Mario.
#include <cstdio>
#include <cstdlib>

#include "Actor.h"
#include "ActorBase.h"
#include "dsstate_seg.h"

extern "C" {
/* the ten shared lifecycle halves, the same functions hal/actor_classes.cpp
   writes into every class it carries */
int _ZN5Actor19BeforeInitResourcesEv(void *self);          /* slot 1  */
void _ZN5Actor18AfterInitResourcesEj(void *self, unsigned a); /* slot 2 */
int _ZN5Actor14BeforeBehaviorEv(void *self);               /* slot 7  */
int _ZN5Actor12BeforeRenderEv(void *self);                 /* slot 10 */
int _ZN5Actor13OnYoshiTryEatEv(void *self);                /* slot 18 */
int _ZN5Actor16OnAimedAtWithEggEv(void *self);             /* slot 29 default */
/* ...and Actor's own tail, slots 20 through 28. Every one is a two-line ROM
   body: Virtual50 answers VS_FAIL and the eight combat hooks do nothing, which
   is what a class that does not care about being hit inherits. */
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

extern int data_02099f24[];          /* the frame phase the lists are in */
extern unsigned char data_020a4b4c;  /* the spawn spine's own step */
const char *port_actor_class_name(unsigned id);   /* hal/actor_registry */
  void port_actor_slot_decline(const char *what);  /* func_02043fdc_hostcopy.cpp */
void port_actor_render_probe(const char *cls, void *model); /* actor_classes */
}

/* ---- the trap -------------------------------------------------------------
   One report for every trapped slot on every class in this gate, saying which
   slot fired on which actor in which phase -- the form hal/actor_classes.cpp
   settled on after a woken Bird's destructor spent a session disguised as a
   slot-13 dispatch. */
static void e31_trap_report(void *self, int slot)
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
#define E31_TRAP(n) \
    static int __fastcall e31_trap##n(void *s, void *) \
    { e31_trap_report(s, n); return 0; }
E31_TRAP(13) E31_TRAP(14) E31_TRAP(16) E31_TRAP(17) E31_TRAP(30)
#undef E31_TRAP

static int __fastcall e31_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall e31_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall e31_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
/* Slots 5, 8 and 11 are ARM tail-call veneers on the ROM: two instructions
   that drop into ActorBase's implementation with the argument still riding in
   r1. A host forward through the veneer's own C face would lose it, so the
   thunk calls the target directly. */
static void __fastcall e31_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall e31_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall e31_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall e31_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall e31_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall e31_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall e31_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall e31_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
static int __fastcall e31_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
/* slot 29, Actor::OnAimedAtWithEgg -- the INHERITED default for every class in
   this gate that has no egg-aim body of its own. Read the ROM tables + 0x74
   with relocs applied (_ZTV10ChainChomp, _ZTV13KoopaTheQuick, _ZTV9KoopaFlag,
   _ZTV11BobOmbBuddy, _ZTV15ChainChompFence, _ZTV11daObjPile_c) and slot 29
   lands on arm9 0x02010124, Actor::OnAimedAtWithEgg -- the same default bbh's
   bbh_aimed and painting's pt_aimed seat. It returns the egg auto-aim lock-on
   radius (81920 == 20.0 in 20.12); the caller reads it thiscall (this in ecx,
   no stack args, ret 0), so this veneer forwards ecx and cleans nothing. The
   three classes with their own body (BobOmb, Goomba, KingBobOmb) overwrite it
   below; the six without inherit this, where a trap used to sit. */
static int __fastcall e31_aimed(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }
static int __fastcall e31_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall e31_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall e31_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall e31_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall e31_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall e31_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall e31_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall e31_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
/* slot 19, OnTurnIntoEgg(Player &player): the caller PUSHES the player, so the
   three-parameter veneer pops it. The ROM reloc at each of these tables + 0x4c
   lands on arm9 0x02010154, Actor::OnTurnIntoEgg (a tail-call veneer to
   KillAndTrackInDeathTable). Seating it lets Yoshi swallow-and-respawn as the
   ROM does; trapping it froze the actor forever. Classes with their OWN egg
   body (BobOmb, Goomba) override this slot below. */
static int __fastcall e31_turn_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }

/* The shared half of a 31-slot table: Actor's four Before/After pairs,
   ActorBase::OnHeapCreated, Actor::OnYoshiTryEat, Virtual50 and the eight
   combat hooks, plus the traps. A caller writes its own 0/3/6/9/12/16/17 and
   whichever of 18/19/29 it overrides. */
static void ac31_fill_shared(void **vt)
{
    vt[1] = (void *)e31_binit;
    vt[2] = (void *)e31_ainit;
    vt[4] = (void *)e31_bclean;
    vt[5] = (void *)e31_aclean;
    vt[7] = (void *)e31_bbeh;
    vt[8] = (void *)e31_abeh;
    vt[10] = (void *)e31_bren;
    vt[11] = (void *)e31_aren;
    vt[12] = (void *)e31_pdes;
    vt[13] = (void *)e31_trap13;
    vt[14] = (void *)e31_trap14;
    vt[15] = (void *)e31_heap;
    vt[16] = (void *)e31_trap16;
    vt[17] = (void *)e31_trap17;
    vt[18] = (void *)e31_yoshi;
    vt[19] = (void *)e31_turn_egg;
    vt[20] = (void *)e31_v50;
    vt[21] = (void *)e31_pounded;
    vt[22] = (void *)e31_atk1;
    vt[23] = (void *)e31_atk2;
    vt[24] = (void *)e31_kicked;
    vt[25] = (void *)e31_pushed;
    vt[26] = (void *)e31_cannon;
    vt[27] = (void *)e31_mega;
    vt[28] = (void *)e31_under;
    vt[29] = (void *)e31_aimed;    /* Actor::OnAimedAtWithEgg, the ROM default;
                                      classes with their own body override it */
    vt[30] = (void *)e31_trap30;
}

// ============================================================================
// BOB_OMB (actor 206, ov102)
// ============================================================================
//
// _ZTV6BobOmb / _ZTV7daBmb_c, ov102 0x0214e558. The walking black bomb, and
// the PINK ONE IS THE SAME CLASS: InitResources reads `param1 & 7` into +0x3f5
// and branches on it, type 2 clearing the solid-collision bit and starting
// inert. The friendly pink character is BOB_OMB_BUDDY, a different class in
// ov084 entirely.
//
// Its SpawnInfo is already in port/ov102_syms.txt (gate 23 mounted the overlay
// for the QUESTION_BLOCK) and its own +4 halfword reads 206, which is the
// registry's cross-check.
//
// Object layout, from its own factory: MovingCylinderClsn at 0x110,
// WithMeshClsn at 0x144, ModelAnim at 0x300, ShadowModel at 0x364, 1024 bytes.
extern "C" {
int _ZN6BobOmb13InitResourcesEv(void *self);       /* face: below */
int _ZN6BobOmb8BehaviorEv(char *self);
int _ZN6BobOmb6RenderEv(void *self);               /* host copy */
int _ZN6BobOmb16CleanupResourcesEv(void);          /* three file releases */
int *_ZN6BobOmbD1Ev(int *self);
int *_ZN6BobOmbD0Ev(int *self);
int func_ov102_0214c6e4(unsigned char *self);      /* slot 18, its own */
void func_ov102_0214adc8(void *self, void *player); /* slot 19, its own */
int func_ov102_0214aa10(void);                     /* slot 29, its own */
void *_ZTV6BobOmb[31];
}
/* The bomb's own D0 spells its table by the RTTI name. */
#pragma comment(linker, "/alternatename:__ZTV7daBmb_c=__ZTV6BobOmb")

static int __fastcall bmb_init(void *s, void *)
{ return _ZN6BobOmb13InitResourcesEv(s); }
static int __fastcall bmb_clean(void *, void *)
{ return _ZN6BobOmb16CleanupResourcesEv(); }
static int __fastcall bmb_behavior(void *s, void *)
{ return _ZN6BobOmb8BehaviorEv((char *)s); }
static int __fastcall bmb_render(void *s, void *)
{ port_actor_render_probe("BOB_OMB", (char *)s + 0x300);
  return _ZN6BobOmb6RenderEv(s); }
/* SLOT 16 IS LIVE. A bomb that is lit reaches its own blast and marks itself
   for destruction, so the D1 the cleanup pass dispatches has to be the class's
   own -- and src/_ZN6BobOmbD1Ev.c is already exactly the D0 body minus the
   final Deallocate, which is what ActorBase::AfterCleanupResources performs
   itself after the dispatch returns. */
static int __fastcall bmb_d1(void *s, void *)
{ return (int)(size_t)_ZN6BobOmbD1Ev((int *)s); }
static int __fastcall bmb_d0(void *s, void *)
{ return (int)(size_t)_ZN6BobOmbD0Ev((int *)s); }
static int __fastcall bmb_yoshi(void *s, void *)
{ return func_ov102_0214c6e4((unsigned char *)s); }
static int __fastcall bmb_egg(void *s, void *, void *p)
{ func_ov102_0214adc8(s, p); return 0; }
static int __fastcall bmb_aimed(void *, void *)
{ return func_ov102_0214aa10(); }

/* The Enemy tier's own eight-entry death table (port/unmatched): its statics
   carry DS code addresses until they are seated, and __sinit_ov002_02100938
   copies them into the bss table the base class dispatches. Seated from the
   first Enemy-family class to register, which is where the registry runs. */
extern "C" void port_enemy_death_states_seat(void);

extern "C" void hal_fill_bob_omb_vtable(void)
{
    void **vt = _ZTV6BobOmb;
    port_enemy_death_states_seat();
    ac31_fill_shared(vt);
    vt[0] = (void *)bmb_init;
    vt[3] = (void *)bmb_clean;
    vt[6] = (void *)bmb_behavior;
    vt[9] = (void *)bmb_render;
    vt[16] = (void *)bmb_d1;
    vt[17] = (void *)bmb_d0;
    vt[18] = (void *)bmb_yoshi;
    vt[19] = (void *)bmb_egg;
    vt[29] = (void *)bmb_aimed;
}

/* ---- method faces ---------------------------------------------------------
   The C-named references the vtables take onto definitions that are real MSVC
   methods against include/. Everything else in this gate's classes is already
   a C-named free function in its own TU. */
#include "BobOmb.h"
extern "C" {
int _ZN6BobOmb13InitResourcesEv(void *self)
{ return ((BobOmb *)self)->BobOmb::InitResources(); }
}

// ============================================================================
// KOOPA_SHELL (actor 285, ov102)
// ============================================================================
//
// _ZTV10KoopaShell / _ZTV7daShl_c, ov102 0x0214e650. The green shell a stomped
// Koopa leaves behind and the item a GREEN_SHELL_BLOCK_TAG cracks open. An Enemy
// subclass on the same 31-slot table shape as BOB_OMB, overriding the six
// lifecycle slots. Slot 18 (OnYoshiTryEat) keeps the shared Actor default:
// the ROM body func_ov102_0214d6a0 is a vtable-slot-identity GUESS, not a named
// decompilation, so inferred_stub_guard keeps it trapped out of the live fill.
//
// Its SpawnInfo is already in port/ov102_syms.txt (gate 23 mounted the overlay)
// and its own +4 halfword reads 285, the registry's cross-check. It is spawned
// dynamically, not placed: BrickBlock::SpawnKoopaShell (GREEN_SHELL_BLOCK_TAG id
// 323, ov002) and a stomped Koopa (id 203, ov062) both call Actor::Spawn(0x11d).
//
// Object layout, from its own factory (KoopaShell_Spawn ov102 0x0214d6b4):
// MovingCylinderClsn at 0x110, WithMeshClsn at 0x144, Model at 0x300,
// ShadowModel at 0x350, MovingCylinderClsn at 0x378, 992 bytes.
extern "C" {
int _ZN10KoopaShell13InitResourcesEv(void *self);      /* C-named in its own TU */
int _ZN10KoopaShell8BehaviorEv(void *self);            /* C-named in its own TU */
int _ZN10KoopaShell6RenderEv(void *self);              /* C-named in its own TU */
int _ZN10KoopaShell16CleanupResourcesEv(void *self);   /* C-named in its own TU */
void _ZN10KoopaShell16OnPendingDestroyEv(void);        /* empty body */
int *_ZN10KoopaShellD1Ev(int *self);
int *_ZN10KoopaShellD0Ev(int *self);
void *_ZTV10KoopaShell[31];
}
/* The shell's own D0 spells its table by the RTTI name. */
#pragma comment(linker, "/alternatename:__ZTV7daShl_c=__ZTV10KoopaShell")
/* The class's .cpp bodies reach the mounted ov102 data under C++ mangled names;
   bridge each to the C mount symbol (the ov064 bowserpuzzle pattern). The model
   file array and the four PMF state statics (ea68 reached under two shadows:
   void* in InitResources, char in Behavior). */
#pragma comment(linker, "/alternatename:?data_ov102_0214d70c@@3PAPAUSharedFilePtr@@A=_data_ov102_0214d70c")
#pragma comment(linker, "/alternatename:?data_ov102_0214ea68@@3PAXA=_data_ov102_0214ea68")
#pragma comment(linker, "/alternatename:?data_ov102_0214ea48@@3DA=_data_ov102_0214ea48")
#pragma comment(linker, "/alternatename:?data_ov102_0214ea58@@3DA=_data_ov102_0214ea58")
#pragma comment(linker, "/alternatename:?data_ov102_0214ea68@@3DA=_data_ov102_0214ea68")
#pragma comment(linker, "/alternatename:?data_ov102_0214ea78@@3DA=_data_ov102_0214ea78")
/* func_ov102_0214ce60 calls Actor::UpdateCarry returning Matrix4x3& (AAU); the
   real matched method src/_ZN5Actor11UpdateCarryER6PlayerRK7Vector3.cpp (gate 18,
   in this target) returns Matrix4x3* (PAU). Same __thiscall ABI, return-mangling
   only; bridge the reference form to the pointer definition. */
#pragma comment(linker, "/alternatename:?UpdateCarry@Actor@@QAEAAUMatrix4x3@@AAUPlayer@@ABUVector3@@@Z=?UpdateCarry@Actor@@QAEPAUMatrix4x3@@AAUPlayer@@ABUVector3@@@Z")

static int __fastcall ksh_init(void *s, void *)
{ return _ZN10KoopaShell13InitResourcesEv(s); }
static int __fastcall ksh_clean(void *s, void *)
{ return _ZN10KoopaShell16CleanupResourcesEv(s); }
static int __fastcall ksh_behavior(void *s, void *)
{ return _ZN10KoopaShell8BehaviorEv(s); }
static int __fastcall ksh_render(void *s, void *)
{ port_actor_render_probe("KOOPA_SHELL", (char *)s + 0x300);
  return _ZN10KoopaShell6RenderEv(s); }
static int __fastcall ksh_pdes(void *, void *)
{ _ZN10KoopaShell16OnPendingDestroyEv(); return 0; }
static int __fastcall ksh_d1(void *s, void *)
{ return (int)(size_t)_ZN10KoopaShellD1Ev((int *)s); }
static int __fastcall ksh_d0(void *s, void *)
{ return (int)(size_t)_ZN10KoopaShellD0Ev((int *)s); }

extern "C" void port_koopa_shell_states_seat(void);   /* port/unmatched */

extern "C" void hal_fill_koopa_shell_vtable(void)
{
    void **vt = _ZTV10KoopaShell;
    port_koopa_shell_states_seat();
    port_enemy_death_states_seat();
    ac31_fill_shared(vt);
    vt[0] = (void *)ksh_init;
    vt[3] = (void *)ksh_clean;
    vt[6] = (void *)ksh_behavior;
    vt[9] = (void *)ksh_render;
    vt[12] = (void *)ksh_pdes;
    vt[16] = (void *)ksh_d1;
    vt[17] = (void *)ksh_d0;
    /* slot 18 (OnYoshiTryEat) stays the shared Actor default: its ROM body
       func_ov102_0214d6a0 is a vtable-slot-identity guess, kept trapped. */
}

// ============================================================================
// GOOMBA (actor 200, ov084)
// ============================================================================
//
// _ZTV6Goomba / _ZTV7daKrb_c, ov084 0x02130948 (RTTI: daKrb_c -- kuribo is the
// goomba). One class body serves three actor ids: 200 is the ordinary one, 201
// the small one and 202 the large one, and InitResources reads which out of the
// object record's param into mGoombaType at +0x460.
//
// It overrides three of Actor's own tail slots as well as the usual six:
// 18 (OnYoshiTryEat -- Yoshi can swallow one), 19 (OnTurnIntoEgg) and 29
// (OnAimedAtWithEgg).
//
// Object layout, from its own factory: MovingCylinderClsn at 0x180,
// WithMeshClsn at 0x1b4, ModelAnim at 0x370, ShadowModel at 0x3d4,
// MaterialChanger at 0x3fc.
extern "C" {
int _ZN6Goomba13InitResourcesEv(char *self);        /* C-named in its own TU */
int _ZN6Goomba8BehaviorEv(void *self);              /* face: below */
int _ZN6Goomba6RenderEv(void *self);                /* host copy */
int _ZN6Goomba16CleanupResourcesEv(void *self);     /* face: below */
void _ZN6Goomba16OnPendingDestroyEv(void);
int *_ZN6GoombaD1Ev(int *self);
int *_ZN6GoombaD0Ev(int *self);
int func_ov084_0212bfc0(void *self);                /* slot 18, its own */
void func_ov084_0212b344(void *self, void *player); /* slot 19, its own */
int func_ov084_0212b30c(void *self);                /* slot 29, its own */
void *_ZTV6Goomba[31];
}
/* The goomba's own D0 spells its table by the RTTI name. */
#pragma comment(linker, "/alternatename:__ZTV7daKrb_c=__ZTV6Goomba")

static int __fastcall gmb_init(void *s, void *)
{ return _ZN6Goomba13InitResourcesEv((char *)s); }
static int __fastcall gmb_clean(void *s, void *)
{ return _ZN6Goomba16CleanupResourcesEv(s); }
static int __fastcall gmb_behavior(void *s, void *)
{ return _ZN6Goomba8BehaviorEv(s); }
static int __fastcall gmb_render(void *s, void *)
{ port_actor_render_probe("GOOMBA", (char *)s + 0x370);
  return _ZN6Goomba6RenderEv(s); }
static int __fastcall gmb_pdes(void *, void *)
{ _ZN6Goomba16OnPendingDestroyEv(); return 0; }
/* SLOT 16 IS LIVE: a squashed goomba marks itself for destruction and the
   cleanup pass dispatches D1 the next frame. */
static int __fastcall gmb_d1(void *s, void *)
{ return (int)(size_t)_ZN6GoombaD1Ev((int *)s); }
static int __fastcall gmb_d0(void *s, void *)
{ return (int)(size_t)_ZN6GoombaD0Ev((int *)s); }
static int __fastcall gmb_yoshi(void *s, void *)
{ return func_ov084_0212bfc0(s); }
static int __fastcall gmb_egg(void *s, void *, void *p)
{ func_ov084_0212b344(s, p); return 0; }
static int __fastcall gmb_aimed(void *s, void *)
{ return func_ov084_0212b30c(s); }

extern "C" void port_goomba_states_seat(void);          /* port/unmatched */

extern "C" void hal_fill_goomba_vtable(void)
{
    void **vt = _ZTV6Goomba;
    port_goomba_states_seat();
    port_enemy_death_states_seat();
    ac31_fill_shared(vt);
    vt[0] = (void *)gmb_init;
    vt[3] = (void *)gmb_clean;
    vt[6] = (void *)gmb_behavior;
    vt[9] = (void *)gmb_render;
    vt[12] = (void *)gmb_pdes;
    vt[16] = (void *)gmb_d1;
    vt[17] = (void *)gmb_d0;
    vt[18] = (void *)gmb_yoshi;
    vt[19] = (void *)gmb_egg;
    vt[29] = (void *)gmb_aimed;
}

// ============================================================================
// BOB_OMB_BUDDY (actor 181, ov084)
// ============================================================================
//
// _ZTV11BobOmbBuddy / _ZTV14daRedBombhei_c, ov084 0x02130a38 -- the red
// bomb-hei, which is the pink buddy that opens the cannons. It overrides
// nothing past slot 17: the whole class is six slots and a three-state
// machine.
//
// Its Behavior is short because the machine is the class: change state through
// func_ov084_0212c960, run the current one through func_ov084_0212c9a8, both
// host copies for the pointer-to-member reason
// (port/unmatched/BobOmbBuddy_States.cpp).
extern "C" {
int _ZN11BobOmbBuddy13InitResourcesEv(void *self);     /* face: below */
int _ZN11BobOmbBuddy8BehaviorEv(void *self);           /* face: below */
int _ZN11BobOmbBuddy6RenderEv(void *self);             /* host copy */
int _ZN11BobOmbBuddy16CleanupResourcesEv(void);        /* two file releases */
int *_ZN11BobOmbBuddyD0Ev(int *self);
/* the three member teardowns the hosted D1 runs, plus Actor's D2 */
void _ZN11ShadowModelD1Ev(void *);
void _ZN9ModelAnimD1Ev(void *);
void *_ZN18MovingCylinderClsnD1Ev(void *);
void *_ZN5ActorD2Ev(void *);
void *_ZTV11BobOmbBuddy[31];
}
#pragma comment(linker, "/alternatename:__ZTV14daRedBombhei_c=__ZTV11BobOmbBuddy")

static int __fastcall bbud_init(void *s, void *)
{ return _ZN11BobOmbBuddy13InitResourcesEv(s); }
static int __fastcall bbud_clean(void *, void *)
{ return _ZN11BobOmbBuddy16CleanupResourcesEv(); }
static int __fastcall bbud_behavior(void *s, void *)
{ return _ZN11BobOmbBuddy8BehaviorEv(s); }
static int __fastcall bbud_render(void *s, void *)
{ port_actor_render_probe("BOB_OMB_BUDDY", (char *)s + 0x108);
  return _ZN11BobOmbBuddy6RenderEv(s); }
/* SLOT 16 IS HOSTED rather than taken from src. src/_ZN11BobOmbBuddyD1Ev.cpp
   is a REAL C++ DESTRUCTOR -- `BobOmbBuddy::~BobOmbBuddy() {}` over a shadow
   class with three members -- so MSVC emits ??1BobOmbBuddy@@UAE@XZ and calls
   ??1ModelAnim@@QAE@XZ and two more that exist nowhere in this build. The body
   below is src/_ZN11BobOmbBuddyD0Ev.c minus its final Memory::Deallocate,
   which ActorBase::AfterCleanupResources performs itself after the dispatch
   returns -- the Bird's treatment. */
static int __fastcall bbud_d1(void *s, void *)
{
    *(int *)s = (int)(size_t)_ZTV11BobOmbBuddy;
    _ZN11ShadowModelD1Ev((char *)s + 0x16c);
    _ZN9ModelAnimD1Ev((char *)s + 0x108);
    _ZN18MovingCylinderClsnD1Ev((char *)s + 0xd4);
    _ZN5ActorD2Ev(s);
    return (int)(size_t)s;
}
static int __fastcall bbud_d0(void *s, void *)
{ return (int)(size_t)_ZN11BobOmbBuddyD0Ev((int *)s); }

extern "C" void port_bob_omb_buddy_states_seat(void);   /* port/unmatched */

extern "C" void hal_fill_bob_omb_buddy_vtable(void)
{
    void **vt = _ZTV11BobOmbBuddy;
    port_bob_omb_buddy_states_seat();
    port_enemy_death_states_seat();
    ac31_fill_shared(vt);
    vt[0] = (void *)bbud_init;
    vt[3] = (void *)bbud_clean;
    vt[6] = (void *)bbud_behavior;
    vt[9] = (void *)bbud_render;
    vt[16] = (void *)bbud_d1;
    vt[17] = (void *)bbud_d0;
}

/* ---- ov084's method faces ------------------------------------------------- */
#include "Goomba.h"
#include "BobOmbBuddy.h"
extern "C" {
int _ZN6Goomba8BehaviorEv(void *self)
{ return ((Goomba *)self)->Goomba::Behavior(); }
int _ZN6Goomba16CleanupResourcesEv(void *self)
{ return ((Goomba *)self)->Goomba::CleanupResources(); }
int _ZN11BobOmbBuddy13InitResourcesEv(void *self)
{ return ((BobOmbBuddy *)self)->BobOmbBuddy::InitResources(); }
int _ZN11BobOmbBuddy8BehaviorEv(void *self)
{ return ((BobOmbBuddy *)self)->BobOmbBuddy::Behavior(); }
}

// ============================================================================
// CHAIN_CHOMP (actor 219, ov014) and CHAIN_CHOMP_FENCE (actor 41, ov014)
// ============================================================================
//
// _ZTV10ChainChomp at ov014 0x021147ec and _ZTV15ChainChompFence at
// 0x021148b0. ov014 is Bob-omb Battlefield's OWN LEVEL OVERLAY, so it is
// mounted twice: whole, for the loaders the level boot walks, and per symbol,
// for the SharedFilePtrs and state tables these two classes reach by name.
// That is the gate-17 shape, which did the same to ov009.
//
// Neither overrides anything past slot 17. The chomp's six-state machine is
// its own pair of dispatchers (port/unmatched/ChainChomp_States.cpp).
extern "C" {
int _ZN10ChainChomp13InitResourcesEv(void *self);       /* face: below */
int _ZN10ChainChomp8BehaviorEv(void *self);             /* face: below */
int _ZN10ChainChomp6RenderEv(void *self);               /* host copy */
int _ZN10ChainChomp16CleanupResourcesEv(void);
void *_ZN10ChainChompD1Ev(void *self);
void *_ZN10ChainChompD0Ev(void *self);
void *_ZTV10ChainChomp[31];

void _ZN8Platform4KillEv(void *self);                   /* slot 31 */
int _ZN15ChainChompFence13InitResourcesEv(void *self);  /* face: below */
int _ZN15ChainChompFence8BehaviorEv(void *self);        /* face: below */
int _ZN15ChainChompFence6RenderEv(void *self);          /* host copy */
int _ZN15ChainChompFence16CleanupResourcesEv(void *self); /* face: below */
int *_ZN15ChainChompFenceD1Ev(int *self);
int *_ZN15ChainChompFenceD0Ev(int *self);
/* THIRTY-TWO, not 31: the fence is a Platform subclass (ov014 0x021148b0) and
   its slot 31 is Platform::Kill. Thirty-one is the plain Actor width. dsd left
   an ambiguous symbol at word 1, so the next-symbol bound reads 1 here and the
   reloc run is what says 32. */
void *_ZTV15ChainChompFence[32];
}

static int __fastcall cc_init(void *s, void *)
{ return _ZN10ChainChomp13InitResourcesEv(s); }
static int __fastcall cc_clean(void *, void *)
{ return _ZN10ChainChomp16CleanupResourcesEv(); }
static int __fastcall cc_behavior(void *s, void *)
{ return _ZN10ChainChomp8BehaviorEv(s); }
static int __fastcall cc_render(void *s, void *)
{ port_actor_render_probe("CHAIN_CHOMP", (char *)s + 0x150);
  return _ZN10ChainChomp6RenderEv(s); }
static int __fastcall cc_d1(void *s, void *)
{ return (int)(size_t)_ZN10ChainChompD1Ev(s); }
static int __fastcall cc_d0(void *s, void *)
{ return (int)(size_t)_ZN10ChainChompD0Ev(s); }

extern "C" void port_chain_chomp_states_seat(void);     /* port/unmatched */
/* The fence is a Platform, and its destructor installs the base table between
   the two member teardowns -- hal/actor_classes.cpp owns that fill. */
extern "C" void hal_fill_platform_vtable(void);

extern "C" void hal_fill_chain_chomp_vtable(void)
{
    void **vt = _ZTV10ChainChomp;
    port_chain_chomp_states_seat();
    port_enemy_death_states_seat();
    ac31_fill_shared(vt);
    vt[0] = (void *)cc_init;
    vt[3] = (void *)cc_clean;
    vt[6] = (void *)cc_behavior;
    vt[9] = (void *)cc_render;
    vt[16] = (void *)cc_d1;
    vt[17] = (void *)cc_d0;
}

static int __fastcall ccf_init(void *s, void *)
{ return _ZN15ChainChompFence13InitResourcesEv(s); }
static int __fastcall ccf_clean(void *s, void *)
{ return _ZN15ChainChompFence16CleanupResourcesEv(s); }
static int __fastcall ccf_behavior(void *s, void *)
{ return _ZN15ChainChompFence8BehaviorEv(s); }
static int __fastcall ccf_render(void *s, void *)
{ port_actor_render_probe("CHAIN_CHOMP_FENCE", (char *)s + 0xd4);
  return _ZN15ChainChompFence6RenderEv(s); }
static int __fastcall ccf_kill(void *s, void *)
{ _ZN8Platform4KillEv(s); return 0; }
static int __fastcall ccf_d1(void *s, void *)
{ return (int)(size_t)_ZN15ChainChompFenceD1Ev((int *)s); }
static int __fastcall ccf_d0(void *s, void *)
{ return (int)(size_t)_ZN15ChainChompFenceD0Ev((int *)s); }

extern "C" void hal_fill_chain_chomp_fence_vtable(void)
{
    void **vt = _ZTV15ChainChompFence;
    hal_fill_platform_vtable();
    port_enemy_death_states_seat();
    ac31_fill_shared(vt);
    vt[0] = (void *)ccf_init;
    vt[3] = (void *)ccf_clean;
    vt[6] = (void *)ccf_behavior;
    vt[9] = (void *)ccf_render;
    vt[16] = (void *)ccf_d1;
    vt[17] = (void *)ccf_d0;
    /* slot 31, the Platform tail; the fence does not override it. pile_kill is
       the same one-line forward to _ZN8Platform4KillEv the stump takes. */
    vt[31] = (void *)ccf_kill;
}

// ============================================================================
// STUMP (actor 27, ov091) -- the post the CHAIN_CHOMP is chained to
// ============================================================================
//
// _ZTV11daObjPile_c, ov091 0x021352bc. THE CONFIG NAMES THIS CLASS ON THE
// WRONG TABLE, the ov100 DOOR case for the second time in this port and the
// third naming bug the RTTI has settled this session.
//
//   Stump_SpawnInfo   ov091 0x02135298, +4 halfword 27, factory Stump_Spawn
//   Stump_Spawn       ov091 0x02133938, and its literal pool installs
//                     0x021352bc, whose RTTI reads daObjPile_c
//   _ZTV5Stump        ov091 0x021353ac, RTTI daHyuhyu_c -- and that table is
//                     FWOOSH's, installed by Fwoosh_Spawn for actor 231
//
// So the six _ZN5Stump* bodies in src/ implement FWOOSH, actor 27's own are
// the unnamed func_ov091_* block the real table points at, and the port
// compiles what the vtable points at and renames nothing.
//
// THIRTY-TWO SLOTS, not thirty-one. This is a Platform (Stump_Spawn calls
// Platform's constructor and the D1 installs _ZTV10dBgActor_c between its two
// member teardowns), and Platform appends Kill after Actor's tail: slot 31 of
// the ROM's table is _ZN8Platform4KillEv.
//
// It is the ground-pound target. InitResources sets its hit counter at +0x31e
// to 3; slot 21 (OnGroundPounded) and slot 27 (OnHitByMegaChar) both end in
// func_ov091_021334b8, which takes one hit off it, and Behavior spawns five
// coins when the player has twisted it far enough.
extern "C" {
int func_ov091_021338ac(char *self);            /* InitResources */
int func_ov091_021336cc(void *self);            /* CleanupResources */
int func_ov091_02133738(char *self);            /* Behavior */
int func_ov091_02133710(void *self);            /* Render */
int *func_ov091_021333fc(int *self);            /* D1 */
void func_ov091_02133648(char *self, void *o);  /* slot 21, its own */
void func_ov091_021335d4(char *self, void *o);  /* slot 27, its own */
void _ZN8Platform4KillEv(void *self);           /* slot 31 */
/* A hosted vtable array, not a ROM constant: hal fills its slots at boot, so
   it is mutable hosted state and belongs in the captured section along with
   the /alternatename alias below, which is the same storage under the name
   the config left unnamed. See hal/dsstate_seg.h. */
DSSTATE_BEGIN
void *_ZTV11daObjPile_c[32];
DSSTATE_END
}
/* Stump_Spawn spells the table by the address config left unnamed. */
#pragma comment(linker, "/alternatename:_data_ov091_021352bc=__ZTV11daObjPile_c")

static int __fastcall pile_init(void *s, void *)
{ return func_ov091_021338ac((char *)s); }
static int __fastcall pile_clean(void *s, void *)
{ return func_ov091_021336cc(s); }
static int __fastcall pile_behavior(void *s, void *)
{ return func_ov091_02133738((char *)s); }
static int __fastcall pile_render(void *s, void *)
{ port_actor_render_probe("STUMP", (char *)s + 0xd4);
  return func_ov091_02133710(s); }
/* SLOT 16 IS LIVE: a post that has taken its third pound marks itself for
   destruction and the cleanup pass dispatches D1 the next frame. Slot 17
   keeps the trap -- src/func_ov091_02133440 is the deleting form and it is
   written over the shared-header VT0/VT1/G0 placeholders, which are single
   global names in this build and would resolve to another TU's objects. */
static int __fastcall pile_d1(void *s, void *)
{ return (int)(size_t)func_ov091_021333fc((int *)s); }
static int __fastcall pile_pounded(void *s, void *, void *o)
{ func_ov091_02133648((char *)s, o); return 0; }
static int __fastcall pile_mega(void *s, void *, void *o)
{ func_ov091_021335d4((char *)s, o); return 0; }
static int __fastcall pile_kill(void *s, void *)
{ _ZN8Platform4KillEv(s); return 0; }

extern "C" void hal_fill_stump_vtable(void)
{
    void **vt = _ZTV11daObjPile_c;
    hal_fill_platform_vtable();
    ac31_fill_shared(vt);
    vt[0] = (void *)pile_init;
    vt[3] = (void *)pile_clean;
    vt[6] = (void *)pile_behavior;
    vt[9] = (void *)pile_render;
    vt[16] = (void *)pile_d1;
    vt[21] = (void *)pile_pounded;
    vt[27] = (void *)pile_mega;
    vt[31] = (void *)pile_kill;
}

// ============================================================================
// KOOPA_THE_QUICK (actor 188, ov062)
// ============================================================================
//
// _ZTV13KoopaTheQuick / _ZTV7daRNk_c, ov062 0x0211db9c -- daRNk_c is the
// racing nokonoko. The seventh actor overlay, which also carries CHUCKYA, the
// two koopas, the koopa's flag and KLEPTO.
//
// HE IS NOT IN THE LEVEL'S DEFAULT OBJECT TABLE. Bob-omb Battlefield names him
// in a star group the boot's filter does not load (SM64DS_STAR_FILTER is what
// reads the other halves back), so the way to see him run is the spawn hook.
//
// Nothing here needs a host copy but the Render: his Behavior dispatches no
// pointer-to-member at all, which makes him the simplest class in this gate.
extern "C" {
int _ZN13KoopaTheQuick13InitResourcesEv(void *self);    /* face: below */
int _ZN13KoopaTheQuick8BehaviorEv(void *self);          /* face: below */
int _ZN13KoopaTheQuick6RenderEv(void *self);            /* host copy */
int _ZN13KoopaTheQuick16CleanupResourcesEv(void *self); /* face: below */
int *_ZN13KoopaTheQuickD1Ev(int *self);
int *_ZN13KoopaTheQuickD0Ev(int *self);
void *_ZTV13KoopaTheQuick[31];
}
#pragma comment(linker, "/alternatename:__ZTV7daRNk_c=__ZTV13KoopaTheQuick")

static int __fastcall ktq_init(void *s, void *)
{ return _ZN13KoopaTheQuick13InitResourcesEv(s); }
static int __fastcall ktq_clean(void *s, void *)
{ return _ZN13KoopaTheQuick16CleanupResourcesEv(s); }
static int __fastcall ktq_behavior(void *s, void *)
{ return _ZN13KoopaTheQuick8BehaviorEv(s); }
static int __fastcall ktq_render(void *s, void *)
{ port_actor_render_probe("KOOPA_THE_QUICK", (char *)s + 0x300);
  return _ZN13KoopaTheQuick6RenderEv(s); }
static int __fastcall ktq_d1(void *s, void *)
{ return (int)(size_t)_ZN13KoopaTheQuickD1Ev((int *)s); }
static int __fastcall ktq_d0(void *s, void *)
{ return (int)(size_t)_ZN13KoopaTheQuickD0Ev((int *)s); }

extern "C" void hal_fill_koopa_the_quick_vtable(void)
{
    void **vt = _ZTV13KoopaTheQuick;
    port_enemy_death_states_seat();
    ac31_fill_shared(vt);
    vt[0] = (void *)ktq_init;
    vt[3] = (void *)ktq_clean;
    vt[6] = (void *)ktq_behavior;
    vt[9] = (void *)ktq_render;
    vt[16] = (void *)ktq_d1;
    vt[17] = (void *)ktq_d0;
}

// ============================================================================
// KING_BOB_OMB (actor 189, ov078)
// ============================================================================
//
// _ZTV10KingBobOmb / _ZTV12daBombking_c, ov078 0x02126e4c. LEVEL 6 IS THE ONLY
// LEVEL OF THE FIFTY-TWO THAT LOADS ov078, which is what makes him Bob-omb
// Battlefield's and nobody else's, and the whole overlay is his: fifty-three
// functions, one class, one vtable, one sinit.
//
// He overrides two of Actor's tail slots as well as the usual six: 12
// (OnPendingDestroy, a four-byte body) and 29 (OnAimedAtWithEgg). Slots 18 and
// 19 stay Actor's: the ROM reloc at his table + 0x4c lands on arm9 0x02010154,
// Actor::OnTurnIntoEgg (the swallow-and-track handler), so keeping the default
// is what lets Yoshi swallow him and have him respawn, as the ROM does.
// ac31_fill_shared seats that default (e31_turn_egg).
//
// HIS STATE IS A POINTER, not an index -- +0x420 holds the address of a
// two-PMF record and eighteen of them live in ov078's bss. Both dispatchers
// are host copies (port/unmatched/KingBobOmb_States.cpp) and all thirty-six
// halves are seated in hal/actor_overlays.cpp.
//
// Object layout, from his own factory: WithMeshClsn at 0x110, BlendModelAnim
// at 0x2cc, MovingCylinderClsnWithPos at 0x33c and 0x37c, CommonModel at
// 0x3bc, ShadowModel at 0x3f8, 1292 bytes.
extern "C" {
int _ZN10KingBobOmb13InitResourcesEv(void *self);      /* face: below */
int _ZN10KingBobOmb8BehaviorEv(void *self);            /* host copy */
int _ZN10KingBobOmb6RenderEv(void *self);              /* face: below */
int _ZN10KingBobOmb16CleanupResourcesEv(void);         /* thirteen releases */
void _ZN10KingBobOmb16OnPendingDestroyEv(void);
int *_ZN10KingBobOmbD1Ev(int *self);
int *_ZN10KingBobOmbD0Ev(int *self);
int func_ov078_021265f4(void);                         /* slot 29, his own */
void *_ZTV10KingBobOmb[31];
}
#pragma comment(linker, "/alternatename:__ZTV12daBombking_c=__ZTV10KingBobOmb")

static int __fastcall kbo_init(void *s, void *)
{ return _ZN10KingBobOmb13InitResourcesEv(s); }
static int __fastcall kbo_clean(void *, void *)
{ return _ZN10KingBobOmb16CleanupResourcesEv(); }
static int __fastcall kbo_behavior(void *s, void *)
{ return _ZN10KingBobOmb8BehaviorEv(s); }
static int __fastcall kbo_render(void *s, void *)
{ port_actor_render_probe("KING_BOB_OMB", (char *)s + 0x2cc);
  return _ZN10KingBobOmb6RenderEv(s); }
static int __fastcall kbo_pdes(void *, void *)
{ _ZN10KingBobOmb16OnPendingDestroyEv(); return 0; }
static int __fastcall kbo_d1(void *s, void *)
{ return (int)(size_t)_ZN10KingBobOmbD1Ev((int *)s); }
static int __fastcall kbo_d0(void *s, void *)
{ return (int)(size_t)_ZN10KingBobOmbD0Ev((int *)s); }
static int __fastcall kbo_aimed(void *, void *)
{ return func_ov078_021265f4(); }

/* He is the first actor the port carries that owns a BlendModelAnim, so gate
   24's own table has to be filled before his factory installs it -- and the
   first that owns a CommonModel, whose table the port had never needed at all.
   CommonModel's ROM table is arm9 0x0208e8a4 and it is THREE slots: D1, D0 and
   DoSetFile, and word 3 is already the RTTI name string, which is how the
   length is read rather than guessed. */
extern "C" void hal_fill_blendmodelanim_vtable(void);
/* The CommonModel table is DEFINED AND FILLED in hal/actor_classes_bob_world.cpp,
   which reached it first through the coins. Both gates wrote their own copy and
   the two collided at link. The reasoning for the slot layout, including which
   of the two fills won and why, is in the comment above
   port_fill_common_model_vtable there. This gate keeps only the reference. */
extern "C" {
void *_ZTV11CommonModel[];
void port_fill_common_model_vtable(void);
}

static void hal_fill_common_model_vtable(void)
{ port_fill_common_model_vtable(); }

extern "C" void hal_fill_king_bob_omb_vtable(void)
{
    void **vt = _ZTV10KingBobOmb;
    hal_fill_blendmodelanim_vtable();
    hal_fill_common_model_vtable();
    port_enemy_death_states_seat();
    ac31_fill_shared(vt);
    vt[0] = (void *)kbo_init;
    vt[3] = (void *)kbo_clean;
    vt[6] = (void *)kbo_behavior;
    vt[9] = (void *)kbo_render;
    vt[12] = (void *)kbo_pdes;
    vt[16] = (void *)kbo_d1;
    vt[17] = (void *)kbo_d0;
    vt[29] = (void *)kbo_aimed;
}

// ============================================================================
// KOOPA_FLAG (actor 205, ov062)
// ============================================================================
//
// _ZTV9KoopaFlag / _ZTV9daRFlag_c, ov062 0x0211dc54 -- the racing flag, and
// the other half of the koopa's race. It is a plain Actor rather than an
// Enemy (KoopaFlag_Spawn calls Actor's constructor, not Enemy's), but its
// table is still the thirty-one-slot shape, so ac31_fill_shared serves it.
//
// Its Behavior is the finish line: it asks Actor::FindWithID for the id it
// was handed and watches for that actor's own id word to read 0xbf, PLAYER.
// When it does it stops the level timer and starts a ninety-frame sound.
extern "C" {
int _ZN9KoopaFlag13InitResourcesEv(void *self);        /* face: below */
int _ZN9KoopaFlag8BehaviorEv(void *self);              /* face: below */
int _ZN9KoopaFlag6RenderEv(void *self);                /* host copy */
int _ZN9KoopaFlag16CleanupResourcesEv(void);
int *_ZN9KoopaFlagD0Ev(int *self);
void *_ZTV9KoopaFlag[31];
}
#pragma comment(linker, "/alternatename:__ZTV9daRFlag_c=__ZTV9KoopaFlag")

static int __fastcall kfl_init(void *s, void *)
{ return _ZN9KoopaFlag13InitResourcesEv(s); }
static int __fastcall kfl_clean(void *, void *)
{ return _ZN9KoopaFlag16CleanupResourcesEv(); }
static int __fastcall kfl_behavior(void *s, void *)
{ return _ZN9KoopaFlag8BehaviorEv(s); }
static int __fastcall kfl_render(void *s, void *)
{ port_actor_render_probe("KOOPA_FLAG", (char *)s + 0x108);
  return _ZN9KoopaFlag6RenderEv(s); }
/* SLOT 16 IS HOSTED rather than taken from src, the bob-omb buddy's case for
   the second time: src/_ZN9KoopaFlagD1Ev.cpp is a REAL C++ DESTRUCTOR over a
   shadow class with two members, so MSVC emits ??1KoopaFlag@@UAE@XZ and calls
   two member dtors that exist nowhere in this build. The body below is
   src/_ZN9KoopaFlagD0Ev.c minus its final Memory::Deallocate, which
   ActorBase::AfterCleanupResources performs itself after the dispatch. */
static int __fastcall kfl_d1(void *s, void *)
{
    *(int *)s = (int)(size_t)_ZTV9KoopaFlag;
    _ZN9ModelAnimD1Ev((char *)s + 0x108);
    _ZN18MovingCylinderClsnD1Ev((char *)s + 0xd4);
    _ZN5ActorD2Ev(s);
    return (int)(size_t)s;
}
static int __fastcall kfl_d0(void *s, void *)
{ return (int)(size_t)_ZN9KoopaFlagD0Ev((int *)s); }

extern "C" void hal_fill_koopa_flag_vtable(void)
{
    void **vt = _ZTV9KoopaFlag;
    ac31_fill_shared(vt);
    vt[0] = (void *)kfl_init;
    vt[3] = (void *)kfl_clean;
    vt[6] = (void *)kfl_behavior;
    vt[9] = (void *)kfl_render;
    vt[16] = (void *)kfl_d1;
    vt[17] = (void *)kfl_d0;
}

/* ---- the last four classes' method faces ---------------------------------- */
#include "ChainChomp.h"
#include "ChainChompFence.h"
#include "KoopaTheQuick.h"
extern "C" {
int _ZN10ChainChomp13InitResourcesEv(void *self)
{ return ((ChainChomp *)self)->ChainChomp::InitResources(); }
int _ZN10ChainChomp8BehaviorEv(void *self)
{ return ((ChainChomp *)self)->ChainChomp::Behavior(); }
int _ZN15ChainChompFence13InitResourcesEv(void *self)
{ return ((ChainChompFence *)self)->ChainChompFence::InitResources(); }
int _ZN15ChainChompFence8BehaviorEv(void *self)
{ return ((ChainChompFence *)self)->ChainChompFence::Behavior(); }
int _ZN15ChainChompFence16CleanupResourcesEv(void *self)
{ return ((ChainChompFence *)self)->ChainChompFence::CleanupResources(); }
int _ZN13KoopaTheQuick13InitResourcesEv(void *self)
{ return ((KoopaTheQuick *)self)->KoopaTheQuick::InitResources(); }
int _ZN13KoopaTheQuick8BehaviorEv(void *self)
{ return ((KoopaTheQuick *)self)->KoopaTheQuick::Behavior(); }
int _ZN13KoopaTheQuick16CleanupResourcesEv(void *self)
{ return ((KoopaTheQuick *)self)->KoopaTheQuick::CleanupResources(); }
}
#include "KoopaFlag.h"
extern "C" {
int _ZN9KoopaFlag13InitResourcesEv(void *self)
{ return ((KoopaFlag *)self)->KoopaFlag::InitResources(); }
int _ZN9KoopaFlag8BehaviorEv(void *self)
{ return ((KoopaFlag *)self)->KoopaFlag::Behavior(); }
}
#include "KingBobOmb.h"
extern "C" {
int _ZN10KingBobOmb13InitResourcesEv(void *self)
{ return ((KingBobOmb *)self)->KingBobOmb::InitResources(); }
int _ZN10KingBobOmb6RenderEv(void *self)
{ return ((KingBobOmb *)self)->KingBobOmb::Render(); }
}
