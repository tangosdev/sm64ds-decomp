// GATE 149: HAUNTED_CHAIR (326), Big Boo's Haunt's rocking chair (ov020).
//
// Same law as hal/actor_classes_bob_enemy.cpp and hal/actor_classes_l7.cpp --
// ROM slot order, __fastcall thunks that call QUALIFIED, unhosted slots trap by
// name -- and HauntedChair is an Actor subclass through Enemy, so its vtable is
// the 31-slot Enemy shape (slots 18..30 are Actor's own tail).
//
// ov020 is level 12's own overlay, mounted WHOLE in PORT_LEVEL_OVERLAYS and per
// symbol in PORT_ACTOR_OVERLAYS so the class reaches its SpawnInfo record and
// its model SharedFilePtr by name (the gate-59 treatment of ov015). Its vtable
// _ZTV12HauntedChair (also _ZTV9daChair_c, the RTTI base spelling dsd names at
// the same address) is LEFT OUT of the mount and is a host array this file
// fills; the alias points the RTTI name at it, the Thwomp treatment.
//
// WHY ONLY THIS ONE OF BIG BOO'S HAUNT'S FOUR ov020 FURNITURE CLASSES. The
// level's object table also names BOOKEND (327) and BOOK_SHOT_SPAWNER (328),
// which spawn BOOK_SHOT (325) at runtime. BookShot's state-9 behaviour
// (func_ov020_0211174c, the book flying home to its shelf) calls
// func_ov063_0211cae8 on the Bookshelf it homes to -- an ov063 function that is
// NOT decompiled and whose overlay is not mounted. Hosting the book classes with
// that call stubbed would not be a ROM-faithful boot, so they stay skipped by
// name at the gate and are documented blockers. HauntedChair reaches nothing
// outside ov020 (its whole helper closure is matched ov020 src and it loads only
// its own model), so it hosts clean on its own.
//
// The id was cross-checked: HauntedChair_SpawnInfo's +4 halfword is 326, the
// census id and ACTOR_SPAWN_TABLE[326] target all agree; HauntedChair_Spawn's
// own vtable-store site (relocs.txt) names _ZTV12HauntedChair.
#include <cstdio>
#include <cstdlib>

#include "Actor.h"
#include "ActorBase.h"
#include "HauntedChair.h"

extern "C" {
/* the shared lifecycle halves, the same functions every enemy fill writes */
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
int _ZN5Actor16OnAimedAtWithEggEv(void *self);                     /* 29 */

extern int data_02099f24[];          /* the frame phase the lists are in */
extern unsigned char data_020a4b4c;  /* the spawn spine's own step */
const char *port_actor_class_name(unsigned id);   /* hal/actor_registry */
  void port_actor_slot_decline(const char *what);  /* func_02043fdc.cpp: per-actor decline */
void port_actor_render_probe(const char *cls, void *model); /* actor_classes */
}

// ---- the trap --------------------------------------------------------------
static void bbh_trap_report(void *self, int slot)
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
#define BBH_TRAP(n) \
    static int __fastcall bbh_trap##n(void *s, void *) \
    { bbh_trap_report(s, n); return 0; }
BBH_TRAP(13) BBH_TRAP(14) BBH_TRAP(16) BBH_TRAP(17) BBH_TRAP(30)
#undef BBH_TRAP

static int __fastcall bbh_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall bbh_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall bbh_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
/* Slots 5/8/11 are ARM tail-call veneers on the ROM; call the target directly
   so the argument riding in r1 is not dropped. */
static void __fastcall bbh_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall bbh_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall bbh_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall bbh_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall bbh_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall bbh_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall bbh_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall bbh_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
static int __fastcall bbh_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
/* Slot 29, Actor::OnAimedAtWithEgg (ov020 0x02114a88 -> 0x02010124). NOT
   Virtual50: the two have the same arity, so nothing checks caught the swap,
   but they return different things. OnAimedAtWithEgg returns the egg auto-aim
   lock-on radius as a Fix12i, 81920 (0x14000, 20.0 in 20.12); Virtual50
   returns VS_FAIL (1), which read as a radius is 1/4096 of a unit. */
static int __fastcall bbh_aimed(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }
/* slot 19, OnTurnIntoEgg(Player &player): the caller PUSHES the player, so the
   three-parameter veneer pops it. The ROM reloc at this table + 0x4c lands on
   arm9 0x02010154, Actor::OnTurnIntoEgg (a tail-call veneer to
   KillAndTrackInDeathTable). Seating it lets Yoshi swallow-and-respawn as the
   ROM does; trapping it froze the actor forever. */
static int __fastcall bbh_turn_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall bbh_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall bbh_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall bbh_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall bbh_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall bbh_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall bbh_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall bbh_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall bbh_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }

/* The shared half of a 31-slot Enemy table. The caller writes its own
   0/3/6/9/16/17. Slots 16/17 are trapped here and overwritten below -- a
   HauntedChair destroys itself on level teardown (gate 31), so both destructors
   are the class's own. */
static void bbh_fill_shared(void **vt)
{
    vt[1] = (void *)bbh_binit;
    vt[2] = (void *)bbh_ainit;
    vt[4] = (void *)bbh_bclean;
    vt[5] = (void *)bbh_aclean;
    vt[7] = (void *)bbh_bbeh;
    vt[8] = (void *)bbh_abeh;
    vt[10] = (void *)bbh_bren;
    vt[11] = (void *)bbh_aren;
    vt[12] = (void *)bbh_pdes;
    vt[13] = (void *)bbh_trap13;
    vt[14] = (void *)bbh_trap14;
    vt[15] = (void *)bbh_heap;
    vt[16] = (void *)bbh_trap16;
    vt[17] = (void *)bbh_trap17;
    vt[18] = (void *)bbh_yoshi;
    vt[19] = (void *)bbh_turn_egg;
    vt[20] = (void *)bbh_v50;
    vt[21] = (void *)bbh_pounded;
    vt[22] = (void *)bbh_atk1;
    vt[23] = (void *)bbh_atk2;
    vt[24] = (void *)bbh_kicked;
    vt[25] = (void *)bbh_pushed;
    vt[26] = (void *)bbh_cannon;
    vt[27] = (void *)bbh_mega;
    vt[28] = (void *)bbh_under;
    vt[29] = (void *)bbh_aimed;  /* Actor::OnAimedAtWithEgg, the ROM's own default */
    vt[30] = (void *)bbh_trap30;
}

// ============================================================================
// HAUNTED_CHAIR (326, ov020) -- _ZTV12HauntedChair / _ZTV9daChair_c (0x02114a14)
// ============================================================================
//
// HauntedChair_Spawn (ov020 0x02113494) installs _ZTV12HauntedChair; its +4
// reads 326. A 0x398-byte object with a Model at +0xd4, a ShadowModel at +0x124,
// a MovingCylinderClsnWithPos at +0x17c and a WithMeshClsn at +0x1bc. Its own
// slots are 0/3/6/9/16/17; everything past 17 is Actor's default. Init/Behavior/
// Render are real C++ methods (faces below); Clean and D0 are plain C.
extern "C" {
int _ZN12HauntedChair13InitResourcesEv(void *self);   /* face: below */
int _ZN12HauntedChair8BehaviorEv(void *self);         /* face: below */
int _ZN12HauntedChair6RenderEv(void *self);           /* face: below */
int _ZN12HauntedChair16CleanupResourcesEv(void *self); /* .c, C linkage */
int *_ZN12HauntedChairD0Ev(int *self);                /* .c, C linkage */
void *_ZTV12HauntedChair[31];
int *HauntedChair_Spawn(void);
/* The Itanium-named sub-object destructors, all in the build with C linkage
   (hal/actor_classes.cpp). The D1 thunk below runs the class's own teardown
   through them -- the D0's chain minus the final Deallocate -- because the
   byte-matched D1 is a .cpp destructor whose auto-emitted member-dtor calls
   would resolve to MSVC-mangled names that do not exist in this build. */
void _ZN5ModelD1Ev(void *);
void _ZN11ShadowModelD1Ev(void *);
void _ZN12WithMeshClsnD1Ev(void *);
void _ZN25MovingCylinderClsnWithPosD1Ev(void *);
void *_ZN5ActorD2Ev(void *);
extern void *_ZTV9daChair_c;   /* aliased to _ZTV12HauntedChair below */
}
static int __fastcall hc_init(void *s, void *)
{ return _ZN12HauntedChair13InitResourcesEv(s); }
static int __fastcall hc_clean(void *s, void *)
{ return _ZN12HauntedChair16CleanupResourcesEv(s); }
static int __fastcall hc_behavior(void *s, void *)
{ return _ZN12HauntedChair8BehaviorEv(s); }
static int __fastcall hc_render(void *s, void *)
{ port_actor_render_probe("HAUNTED_CHAIR", (char *)s + 0xd4);
  return _ZN12HauntedChair6RenderEv(s); }
/* D1, the complete-object destructor slot 16 holds: the ROM's D0 chain
   (_ZN12HauntedChairD0Ev) without the Memory::Deallocate at its tail, because
   the caller of slot 16 (ActorBase::AfterCleanupResources) deallocates itself. */
static int __fastcall hc_d1(void *s, void *)
{
    char *t = (char *)s;
    /* the class's own vtable ADDRESS -- _ZTV9daChair_c is a scalar decl, so
       reading its value would load slot 0, not the table's address */
    *(void **)t = (void *)_ZTV12HauntedChair;
    _ZN12WithMeshClsnD1Ev(t + 0x1bc);
    _ZN25MovingCylinderClsnWithPosD1Ev(t + 0x17c);
    _ZN11ShadowModelD1Ev(t + 0x124);
    _ZN5ModelD1Ev(t + 0xd4);
    _ZN5ActorD2Ev(t);
    return (int)(size_t)s;
}
static int __fastcall hc_d0(void *s, void *)
{ return (int)(size_t)_ZN12HauntedChairD0Ev((int *)s); }

extern "C" void hal_fill_haunted_chair_vtable(void)
{
    void **vt = _ZTV12HauntedChair;
    bbh_fill_shared(vt);
    vt[0] = (void *)hc_init;
    vt[3] = (void *)hc_clean;
    vt[6] = (void *)hc_behavior;
    vt[9] = (void *)hc_render;
    vt[16] = (void *)hc_d1;
    vt[17] = (void *)hc_d0;
}

// ---- method faces ----------------------------------------------------------
// HauntedChair's Init/Behavior/Render are real MSVC members against include/;
// the vtable thunks spell them by Itanium name. Each is a real __thiscall
// method, so a linker alias would hand the body an ecx that never held `this` --
// the face bridges cdecl to __thiscall. Clean and D0 are already C-linkage in
// their .c src, so they get no face.
extern "C" {
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN12HauntedChair13InitResourcesEv(void *self)
{ return ((HauntedChair *)self)->HauntedChair::InitResources(); }
#else
int _ZN12HauntedChair13InitResourcesEv(void *self);  /* Linux: real symbol from src/_ZN12HauntedChair13InitResourcesEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN12HauntedChair8BehaviorEv(void *self)
{ return ((HauntedChair *)self)->HauntedChair::Behavior(); }
#else
int _ZN12HauntedChair8BehaviorEv(void *self);  /* Linux: real symbol from src/_ZN12HauntedChair8BehaviorEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN12HauntedChair6RenderEv(void *self)
{ return ((HauntedChair *)self)->HauntedChair::Render(); }
#else
int _ZN12HauntedChair6RenderEv(void *self);  /* Linux: real symbol from src/_ZN12HauntedChair6RenderEv */
#endif /* _WIN32 */
}

/* _ZTV9daChair_c is the RTTI base spelling dsd names at _ZTV12HauntedChair's
   address; the .c factory/destructor reach it by that name. Point it at the host
   array. */
#pragma comment(linker, "/alternatename:__ZTV9daChair_c=__ZTV12HauntedChair")

/* The C++/C linkage bridge. HauntedChair::InitResources declares the identity
   Matrix4x3 it copies (data_02082128) OUTSIDE its extern "C" block, so MSVC
   mangles the reference as a C++ name while romdata.c defines the one real
   symbol with C linkage. Point the mangled spelling at the C symbol -- the
   "propagate config renames BY ADDRESS" hazard applied to a type. */
#pragma comment(linker, "/alternatename:?data_02082128@@3UM48@@A=_data_02082128")
