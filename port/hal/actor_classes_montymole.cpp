// GATE 174: MONTY_MOLE (310, ov080), the burrowing mole daChoropu_c pops out of
// the ground in a few levels. First fresh class hosted out of ov080's enemy
// cast (the PAINTING gate 50 already mounted ov080 per symbol).
//
// Same law as hal/actor_classes_bbh.cpp and hal/actor_classes_hmc.cpp -- ROM
// slot order, __fastcall thunks that call QUALIFIED, unhosted slots trap by
// name -- and MontyMole is an Actor subclass, so its vtable is the 31-slot Actor
// shape HauntedChair has (slots 18..30 are Actor's own tail). Every slot below
// was read out of _ZTV9MontyMole (ov080 0x021280b0) with its 31 relocations
// applied; the earlier scoping note of a "16-slot vtable with no D1/D0" was
// WRONG -- the reloc span [0x021280b0, 0x0212812c) is 31 words, D1 is slot 16
// and D0 is slot 17.
//
// The class owns 0 (Init), 3 (Cleanup), 6 (Behavior), 9 (Render), 16 (D1),
// 17 (D0) and 29 (func_ov080_02123858, its OnAimedAtWithEgg override which just
// returns 0x28000). Slots 13/14 (ActorBase::Virtual34/38, 0x0204357c/0x0204349c)
// trap by name the way every sibling fill traps that pair, and slot 30
// (OnAimedAtWithEggReturnVec) traps because its matched body is SRET -- a hidden
// return-slot pointer before self -- which no fill's thunk shape models yet.
// Everything else binds to Actor/ActorBase's own default half, all in the map.
//
// BEHAVIOR IS A HOST COPY. _ZN9MontyMole8BehaviorEv dispatches a
// pointer-to-member state table (data_ov080_02128438) inline through an
// incomplete `struct C;`, the WHOMP / OneUpMushroom case, so it is host-copied in
// port/unmatched/MontyMole_StateDispatch.cpp and LEFT OUT of the slice; slot 6
// below calls that host copy. Init and Render are real MSVC members against
// include/MontyMole.h (faces below); Cleanup and D0 are plain C.
//
// D1 (slot 16) is a HOST THUNK, the HauntedChair treatment. The matched D1
// (_ZN9MontyMoleD1Ev.cpp) is an empty-bodied ~MontyMole() whose auto-emitted
// member-dtor calls (ModelAnim at +0xd4, MovingCylinderClsn at +0x138) and base
// ~Actor would resolve to MSVC-mangled names that do not exist in this build, so
// it is NOT compiled; the thunk runs the D0 chain minus the final Deallocate
// (the slot-16 caller, ActorBase::AfterCleanupResources, deallocates itself).
//
// The id was cross-checked from the relocated overlay image: MontyMole_SpawnInfo
// (0x02127fec) +0 word = MontyMole_Spawn (0x021249e0), +4 halfword = 0x0136 =
// 310, and ACTOR_SPAWN_TABLE[310] (data_02090864 + 310*4 = 0x02090d3c) points at
// that record. MontyMole_Spawn's own vtable-store site (relocs 0x02124a1c ->
// 0x021280b0) names _ZTV9MontyMole.
#include <cstdio>
#include <cstdlib>

#include "Actor.h"
#include "dtor_faces_cpp.h"
#include "ActorBase.h"
#include "MontyMole.h"

extern "C" {
/* the shared lifecycle halves, the same functions every fill writes */
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

extern int data_02099f24[];          /* the frame phase the lists are in */
extern unsigned char data_020a4b4c;  /* the spawn spine's own step */
const char *port_actor_class_name(unsigned id);   /* hal/actor_registry */
  void port_actor_slot_decline(const char *what);  /* func_02043fdc_hostcopy.cpp */
void port_actor_render_probe(const char *cls, void *model); /* actor_classes */

/* MontyMole's own C-linkage bodies. Behavior is the HOST COPY in
   port/unmatched/MontyMole_StateDispatch.cpp, not the matched src. */
int _ZN9MontyMole8BehaviorEv(void *self);         /* slot 6, HOST copy */
int _ZN9MontyMole16CleanupResourcesEv(void);      /* slot 3, .c C linkage */
int *_ZN9MontyMoleD0Ev(int *self);                /* slot 17, .c C linkage */
int func_ov080_02123858(void);                    /* slot 29, OnAimedAtWithEgg */
void *MontyMole_Spawn(void);

/* the D1 chain's sub-object destructors, all C-linkage in the build */
void _ZN9ModelAnimD1Ev(void *);            /* the ModelAnim at +0xd4 */
void _ZN18MovingCylinderClsnD1Ev(void *);  /* the MovingCylinderClsn at +0x138 */
void *_ZN5ActorD2Ev(void *);               /* the Actor base */

/* The one array the ROM factory installs (MontyMole_Spawn does
   `p[0] = (int)_ZTV9MontyMole`); thirty-one slots. Defined here, not just
   declared: the `int` type and C linkage match the `extern int _ZTV9MontyMole[]`
   in include/decl_common.h that the .c factory and D0 read. */
int _ZTV9MontyMole[31];
}

/* MontyMoleD0.c stores _ZTV11daChoropu_c (the RTTI base spelling dsd names at
   the same address) into the object's slot 0; both references are C linkage, so
   point the underscore spelling at the one host array -- the daChair_c
   precedent. */
#pragma comment(linker, "/alternatename:__ZTV11daChoropu_c=__ZTV9MontyMole")

/* The C++/C linkage bridge, the gate-172/173 reading. InitResources declares
   the two model SharedFilePtrs and the anim-table pointer array as typed C++
   globals outside its extern "C" block -- three MSVC data spellings of the
   mounted C symbols. func_ov080_02123ecc spells Actor::ClosestPlayer with an
   Actor* return while the compiled bridge (hal/reverse_bridges.cpp) is the
   Player*-return spelling -- same __thiscall body, the pointer rides eax
   either way. */
#pragma comment(linker, "/alternatename:?data_ov080_0212766c@@3PAPAUSharedFilePtr@@A=_data_ov080_0212766c")
#pragma comment(linker, "/alternatename:?data_ov080_021283c0@@3USharedFilePtr@@A=_data_ov080_021283c0")
#pragma comment(linker, "/alternatename:?data_ov080_021283c8@@3USharedFilePtr@@A=_data_ov080_021283c8")
#pragma comment(linker, "/alternatename:?ClosestPlayer@Actor@@QAEPAU1@XZ=?ClosestPlayer@Actor@@QAEPAUPlayer@@XZ")

// ---- the trap --------------------------------------------------------------
static void mm_trap_report(void *self, int slot)
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
#define MM_TRAP(n) \
    static int __fastcall mm_trap##n(void *s, void *) \
    { mm_trap_report(s, n); return 0; }
/* 30 is Actor::OnAimedAtWithEggReturnVec, an SRET body (the matched src takes
   a hidden return-slot pointer BEFORE self); no sibling fill models that
   convention and every one traps the slot, so this fill does too. */
MM_TRAP(13) MM_TRAP(14) MM_TRAP(30)
#undef MM_TRAP

// ---- the shared half -------------------------------------------------------
static int __fastcall mm_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall mm_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall mm_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall mm_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall mm_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall mm_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall mm_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall mm_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall mm_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall mm_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall mm_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
static int __fastcall mm_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall mm_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall mm_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall mm_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall mm_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall mm_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall mm_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall mm_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall mm_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall mm_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }

// ---- MONTY_MOLE's own slots -------------------------------------------------
/* Init and Render are real __thiscall members; the face bridges cdecl to
   __thiscall so the ecx that never held `this` is not handed to the body. */
static int __fastcall mm_init(void *s, void *)
{ return ((MontyMole *)s)->MontyMole::InitResources(); }
/* RUN LINKW WAVE 19: was `((MontyMole *)s)->MontyMole::Render()`. That body
   dispatches slot 5 of a local six-virtual shadow over the ModelAnim at 0xd4,
   which the host _ZTV9ModelAnim numbers as Virtual18 -- a live c0000005 the
   moment a MONTY_MOLE is drawn (SM64DS_SPAWN_ACTOR=310 on level 13). See
   port/unmatched/W19_Slot5_Renders.cpp for the fault and the cost. */
extern "C" int port_w19_montymole_render(void *self);
static int __fastcall mm_render(void *s, void *)
{ port_actor_render_probe("MONTY_MOLE", (char *)s + 0xd4);
  return port_w19_montymole_render(s); }
/* Behavior is the HOST copy (C linkage), Cleanup and D0 are plain C. */
static int __fastcall mm_behavior(void *s, void *)
{ return _ZN9MontyMole8BehaviorEv(s); }
static int __fastcall mm_clean(void *s, void *)
{ return _ZN9MontyMole16CleanupResourcesEv(); }
static int __fastcall mm_d0(void *s, void *)
{ return (int)(size_t)_ZN9MontyMoleD0Ev((int *)s); }
static int __fastcall mm_aimed(void *s, void *)
{ (void)s; return func_ov080_02123858(); }
/* D1, the complete-object destructor slot 16 holds: the D0 chain
   (_ZN9MontyMoleD0Ev) without the Memory::Deallocate at its tail, because the
   caller of slot 16 (ActorBase::AfterCleanupResources) deallocates itself.
   Member sub-objects are destroyed high address first, the D0 order. */
/* slot 16 is the matched src D1 through hal/dtor_faces_cpp.cpp (lane DTOR-FACES-CPP);
   the transcribed thunk that stood here (mm_d1) spelled the same chain by hand. */

extern "C" void hal_fill_monty_mole_vtable(void)
{
    void **vt = (void **)_ZTV9MontyMole;
    vt[0]  = (void *)mm_init;
    vt[1]  = (void *)mm_binit;
    vt[2]  = (void *)mm_ainit;
    vt[3]  = (void *)mm_clean;
    vt[4]  = (void *)mm_bclean;
    vt[5]  = (void *)mm_aclean;
    vt[6]  = (void *)mm_behavior;
    vt[7]  = (void *)mm_bbeh;
    vt[8]  = (void *)mm_abeh;
    vt[9]  = (void *)mm_render;
    vt[10] = (void *)mm_bren;
    vt[11] = (void *)mm_aren;
    vt[12] = (void *)mm_pdes;
    vt[13] = (void *)mm_trap13;
    vt[14] = (void *)mm_trap14;
    vt[15] = (void *)mm_heap;
    vt[16] = (void *)hal_cppd1_MontyMole;
    vt[17] = (void *)mm_d0;
    /* the Actor tail (18..30): all bind to Actor's own default, except 29 which
       MontyMole overrides with func_ov080_02123858. */
    vt[18] = (void *)mm_yoshi;
    vt[19] = (void *)mm_egg;
    vt[20] = (void *)mm_v50;
    vt[21] = (void *)mm_pounded;
    vt[22] = (void *)mm_atk1;
    vt[23] = (void *)mm_atk2;
    vt[24] = (void *)mm_kicked;
    vt[25] = (void *)mm_pushed;
    vt[26] = (void *)mm_cannon;
    vt[27] = (void *)mm_mega;
    vt[28] = (void *)mm_under;
    vt[29] = (void *)mm_aimed;
    vt[30] = (void *)mm_trap30;
}
