// GATE 205 (run link60, lane A2): MONTY_MOLE_ROCK (311, ov080), the rock the
// burrowing mole throws. daChoro_Rock_c, the second class out of ov080's enemy
// cast after gate 174 seated the mole.
//
// WHY IT IS SEATED. _ZN9MontyMole8BehaviorEv's throw state spawns actor id
// 0x137 = 311. The port declined that spawn and the ROM-faithful thrower
// stores through the returned NULL, so SM64DS_LEVEL=13 SM64DS_SPAWN_ACTOR=310
// took a c0000005 at +0x98 on the first throw. Registering 311 is the whole
// fix; nothing about the thrower is touched.
//
// Same law as hal/actor_classes_montymole.cpp beside it -- ROM slot order,
// __fastcall thunks that call QUALIFIED, unhosted slots trap by name. The
// vtable is the plain 31-slot Actor shape, re-derived by
// `python port/tools/vtspan.py . _ZTV13MontyMoleRock` on this tree rather than
// taken from the brief: tail 31, terminator 31, raw run 31, and the typeinfo
// route's 79 is the documented over-read past the end of the class block. All
// 31 words resolve. Full derivation in port/slice_gate208.txt.
//
// THE CLASS OWNS SIX SLOTS and every one of them is a matched TU on the slice:
// 0 InitResources, 3 CleanupResources, 6 Behavior, 9 Render, 16 D1, 17 D0.
// It does not override slot 29 the way the mole does. This is the first ov080
// class that needs no host copy at all: Behavior is flat code with no
// pointer-to-member dispatch, and D1 is a real C-linkage body rather than the
// auto-emitted-member-dtor .cpp form that forced the mole's slot-16 thunk.
//
// SLOT 9 IS THE WAVE-19 SLOT-5 SHADOW, AND IT IS SAFE. The matched Render
// dispatches slot 5 of a local six-virtual shadow over the member at +0x110.
// Adjudicated from how the class CONSTRUCTS that member and not from any
// header name: MontyMoleRock_Spawn calls _ZN5ModelC1Ev(p + 0x110),
// InitResources feeds Model::LoadFile into ModelBase::SetFile at this + 0x110,
// and both destructors call _ZN5ModelD1Ev(t + 0x110). It is a plain Model, and
// hal/cxxname_bridge.cpp DUAL-FILLS _ZTV5Model at [4] AND [5], so the shadow's
// slot 5 lands on Model::Render. The matched TU is linked.
//
// That is the OPPOSITE ruling from the mole's own Render one class over, and
// the member type is the only difference: _ZN9MontyMole6RenderEv shadows a
// ModelAnim at +0xd4, which the host _ZTV9ModelAnim numbers as Virtual18 at
// slot 5, so it is a live fault and slot 9 there routes to a host copy. Same
// overlay, same shape, same file family, opposite answer.
//
// The id is cross-checked from the relocated overlay image the gate-174 way:
// MontyMoleRock_SpawnInfo (ov080 0x02128008, in the per-symbol mount at the
// real 0x1c stride) +0 word = MontyMoleRock_Spawn 0x02124998, +4 halfword =
// 0x0137 = 311, and MontyMoleRock_Spawn's own vtable-store names 0x0212802c.
#include <cstdio>

/* hal/actor_slot30_seat.cpp -- the shared seat for vtable slot 30,
   Actor::OnAimedAtWithEggReturnVec. The ROM word in slot 30 of every vtable
   this file fills IS the arm9 base body 0x020100dc (checked against
   config/<module>/relocs.txt at vtable+30*4), and that body is now in the
   link from src/_ZN5Actor25OnAimedAtWithEggReturnVecEv.cpp on slice_gate50.
   The three-parameter __fastcall is the sret contract MSVC uses for a
   thiscall member returning a 12-byte struct: this in ecx, the hidden result
   pointer the one (callee-popped) stack argument. Same shape as whomp_s30. */
extern "C" void *__fastcall port_actor_s30_base(void *self, void *, void *out);
#include <cstdlib>

#include "Actor.h"
#include "ActorBase.h"
#include "MontyMoleRock.h"

extern "C" {
/* the shared lifecycle halves, the same functions every sibling fill writes */
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
int _ZN5Actor16OnAimedAtWithEggEv(void *self);                     /* slot 29 */

extern int data_02099f24[];          /* the frame phase the lists are in */
extern unsigned char data_020a4b4c;  /* the spawn spine's own step */
const char *port_actor_class_name(unsigned id);   /* hal/actor_registry */
void port_actor_slot_decline(const char *what);   /* func_02043fdc_hostcopy.cpp */
void port_actor_render_probe(const char *cls, void *model); /* actor_classes */

/* the rock's own C-linkage bodies, all matched src on slice_gate208.txt */
int _ZN13MontyMoleRock16CleanupResourcesEv(void);   /* slot 3,  .c C linkage */
int *_ZN13MontyMoleRockD1Ev(int *self);             /* slot 16, .c C linkage */
int *_ZN13MontyMoleRockD0Ev(int *self);             /* slot 17, .c C linkage */
void *MontyMoleRock_Spawn(void);

/* The one array the ROM factory installs (MontyMoleRock_Spawn does
   `p[0] = (int)_ZTV13MontyMoleRock`); thirty-one slots. Defined here, not just
   declared: the `int` type and C linkage match the
   `extern int _ZTV13MontyMoleRock[]` in include/decl_common.h that the .c
   factory and both destructors read. */
int _ZTV13MontyMoleRock[31];
}

/* D0 stores _ZTV14daChoro_Rock_c, the RTTI base spelling dsd dual-names at the
   same address -- vtspan prints both at ov080 0x0212802c. Both references are
   C linkage, so point the underscore spelling at the one host array, the
   daChoropu_c precedent next door. */
#pragma comment(linker, "/alternatename:__ZTV14daChoro_Rock_c=__ZTV13MontyMoleRock")

/* InitResources declares the model SharedFilePtr as a typed C++ global outside
   its extern "C" block -- an MSVC data spelling of the mounted C symbol. The
   same cell hal/actor_classes_montymole.cpp already routes for the mole's own
   InitResources, so the directive is a duplicate of a live one rather than a
   new claim; MSVC folds identical /alternatename directives. */
#pragma comment(linker, "/alternatename:?data_ov080_021283c8@@3USharedFilePtr@@A=_data_ov080_021283c8")

// ---- the trap --------------------------------------------------------------
static void mr_trap_report(void *self, int slot)
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
#define MR_TRAP(n) \
    static int __fastcall mr_trap##n(void *s, void *) \
    { mr_trap_report(s, n); return 0; }
/* 13/14 are ActorBase::Virtual34/38, the pair every sibling fill traps. 30 is
   Actor::OnAimedAtWithEggReturnVec, an SRET body (the matched src takes a
   hidden return-slot pointer BEFORE self) that no fill's thunk shape models. */
MR_TRAP(13) MR_TRAP(14)
#undef MR_TRAP

// ---- the shared half -------------------------------------------------------
// Slots 4/5/8/11 hold Actor::BeforeCleanupResources, Actor::AfterCleanup-
// Resources, Actor::AfterBehavior and Actor::AfterRender in the ROM. Of those
// four the port has only Actor::BeforeCleanupResources; the other three exist
// in this build as ActorBase's, which is what every Actor-shaped fill in the
// port binds and what this one binds too. It is a port-wide standing
// substitution, not a choice made here, and it is RULED CORRECT: the ROM's
// three Actor bodies are `ldr ip,[pc]; bx ip` veneers onto exactly those
// ActorBase bodies. Evidence, scope sweep and the two caveats are in
// port/actorbase_slots_ruling.txt.
static int __fastcall mr_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall mr_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall mr_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall mr_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall mr_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall mr_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall mr_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall mr_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall mr_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall mr_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall mr_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
static int __fastcall mr_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall mr_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall mr_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall mr_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall mr_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall mr_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall mr_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall mr_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall mr_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall mr_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall mr_aimed(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }

// ---- MONTY_MOLE_ROCK's own slots -------------------------------------------
/* Init, Behavior and Render are real __thiscall members against
   include/MontyMoleRock.h; the face bridges cdecl to __thiscall so the ecx that
   never held `this` is not handed to the body. */
static int __fastcall mr_init(void *s, void *)
{ return ((MontyMoleRock *)s)->MontyMoleRock::InitResources(); }
static int __fastcall mr_behavior(void *s, void *)
{ return ((MontyMoleRock *)s)->MontyMoleRock::Behavior(); }
/* Slot 9 is the matched Render, on the adjudication in the header comment: the
   member at +0x110 is a plain Model and _ZTV5Model[5] is dual-filled. The probe
   is the sibling-fill convention and reports the model the shadow dispatches. */
static int __fastcall mr_render(void *s, void *)
{ port_actor_render_probe("MONTY_MOLE_ROCK", (char *)s + 0x110);
  return ((MontyMoleRock *)s)->MontyMoleRock::Render(); }
/* Cleanup, D1 and D0 are plain C bodies, all matched, all on the slice. D1 is
   the ROM's own complete-object destructor -- no host thunk stands in for it
   here, unlike the mole's slot 16. */
static int __fastcall mr_clean(void *s, void *)
{ (void)s; return _ZN13MontyMoleRock16CleanupResourcesEv(); }
static int __fastcall mr_d1(void *s, void *)
{ return (int)(size_t)_ZN13MontyMoleRockD1Ev((int *)s); }
static int __fastcall mr_d0(void *s, void *)
{ return (int)(size_t)_ZN13MontyMoleRockD0Ev((int *)s); }

extern "C" void hal_fill_monty_mole_rock_vtable(void)
{
    void **vt = (void **)_ZTV13MontyMoleRock;
    vt[0]  = (void *)mr_init;
    vt[1]  = (void *)mr_binit;
    vt[2]  = (void *)mr_ainit;
    vt[3]  = (void *)mr_clean;
    vt[4]  = (void *)mr_bclean;
    vt[5]  = (void *)mr_aclean;
    vt[6]  = (void *)mr_behavior;
    vt[7]  = (void *)mr_bbeh;
    vt[8]  = (void *)mr_abeh;
    vt[9]  = (void *)mr_render;
    vt[10] = (void *)mr_bren;
    vt[11] = (void *)mr_aren;
    vt[12] = (void *)mr_pdes;
    vt[13] = (void *)mr_trap13;
    vt[14] = (void *)mr_trap14;
    vt[15] = (void *)mr_heap;
    vt[16] = (void *)mr_d1;
    vt[17] = (void *)mr_d0;
    /* the Actor tail (18..30). The rock overrides none of it -- slot 29 is
       Actor's own OnAimedAtWithEgg, where the mole substitutes
       func_ov080_02123858. */
    vt[18] = (void *)mr_yoshi;
    vt[19] = (void *)mr_egg;
    vt[20] = (void *)mr_v50;
    vt[21] = (void *)mr_pounded;
    vt[22] = (void *)mr_atk1;
    vt[23] = (void *)mr_atk2;
    vt[24] = (void *)mr_kicked;
    vt[25] = (void *)mr_pushed;
    vt[26] = (void *)mr_cannon;
    vt[27] = (void *)mr_mega;
    vt[28] = (void *)mr_under;
    vt[29] = (void *)mr_aimed;
    vt[30] = (void *)port_actor_s30_base;
}
