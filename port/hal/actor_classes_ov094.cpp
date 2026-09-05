// GATE 194: HOOT_THE_OWL (234), ov094. Fresh per-symbol mount
// (port/ov094_syms.txt), the ov079/ov080/ov081/ov072 convention. A single
// 31-slot Enemy table -- no own Kill (the reloc dump stops at slot 30;
// three trailing zero words at 0x02136ad4/ad8/adc, read directly out of
// extracted/overlays/overlay_0094.bin, are plain non-relocated data, not a
// slot-31 pointer). Full slot derivation, the ov006/ov094 dual-citation
// resolution, the two PMF disease sites and the cross-overlay alias typo
// that needs no bridge are all documented in port/ov094_syms.txt's own
// header.
//
// D1/D0: HootTheOwl's own D1 (0x02135700) and D0 (0x02135748) both store
// their OWN table directly (_ZTV10HootTheOwl / _ZTV7daOwl_c, the same
// address under two RTTI spellings -- a one-speller D0, the MotherPenguin/
// daBgSnwmn_c shape) and STAY IN THE SLICE, faced below -- no shared-
// placeholder overwrite, no host thunk needed.
//
// SLOTS 13/14 (ActorBase::Virtual34/38) and 30 (Actor::OnAimedAtWithEggReturnVec,
// an SRET method) are the repo-wide Enemy/ActorBase traps every other class
// sharing this base already uses (bob_world/bowserpuzzle/flame/jrb/
// koopa_chuckya/ov081) -- neither Virtual34/38 body is linked anywhere in
// this port. Every other shared slot is address-identical to ov081's own
// Enemy shared half (cross-checked target for target against Spindrift's
// own vtable).
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
#include "HootTheOwl.h"

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

/* the two host-copied PMF-dispatch bodies, port/unmatched/HootTheOwl_StateDispatch.cpp */
int func_ov094_02136188(void *self, void *cell);
int _ZN10HootTheOwl8BehaviorEv(void *self);
void port_hoot_the_owl_states_seat(void);   /* same file, seats the 5 state cells */
/* the ModelAnim slot-5 Render host copy, port/unmatched/ModelAnim_Renders.cpp */
int _ZN10HootTheOwl6RenderEv(void *self);

/* plain matched src, own body */
int _ZN10HootTheOwl16CleanupResourcesEv(void);
void _ZN10HootTheOwl16OnPendingDestroyEv(void);
int *_ZN10HootTheOwlD1Ev(int *self);
int *_ZN10HootTheOwlD0Ev(int *self);
void *HootTheOwl_Spawn(void);
extern unsigned char HootTheOwl_SpawnInfo[];
int _ZTV10HootTheOwl[31];                          /* == _ZTV7daOwl_c, host array */
/* the method face, defined at the bottom of this file (real C++ method
   call against HootTheOwl.h -- needs C++ linkage, forward-declared here
   extern "C" so hoot_init below can call it like every other flat name). */
int _ZN10HootTheOwl13InitResourcesEv(void *self);
}

/* D0 (src/_ZN10HootTheOwlD0Ev.c) stores its own table under the RTTI-alias
   spelling _ZTV7daOwl_c (include/decl_common.h's own `extern int
   _ZTV7daOwl_c[];`), the SAME address as _ZTV10HootTheOwl under two names --
   the SkiLift/MotherPenguin (_ZTV7SkiLift == _ZTV10daPgMthr_c) one-speller
   shape. Bridge the second name onto the real host array. */
#pragma comment(linker, "/alternatename:__ZTV7daOwl_c=__ZTV10HootTheOwl")

// ---- the shared 1..30 half, the ov081 Enemy shared half re-derived here
// (address-identical, see this file's own header) under a local name ------
static void ov94_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: ov094 vtable slot %d is not hosted (actor id %u "
                 "%s)\n", slot, id, port_actor_class_name(id));
    { static char _m[128];
      std::snprintf(_m, sizeof _m, "unhosted ov094 vtable slot %d on id %u %s",
                    slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
static int __fastcall ov94_trap13(void *s, void *) { ov94_trap_report(s, 13); return 0; }
static int __fastcall ov94_trap14(void *s, void *) { ov94_trap_report(s, 14); return 0; }

static int __fastcall ov94_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall ov94_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall ov94_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall ov94_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall ov94_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall ov94_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall ov94_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall ov94_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall ov94_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall ov94_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
static int __fastcall ov94_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall ov94_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall ov94_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall ov94_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall ov94_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall ov94_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall ov94_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall ov94_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall ov94_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall ov94_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall ov94_aimed(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }
static int __fastcall ov94_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }

/* Fills slots 1,2,4,5,7,8,10,11,13,14,15,18..30 -- the standard 31/32-slot
   Enemy shared half. HootTheOwl uses the shared default (own body,
   OnPendingDestroy is empty but confirmed OWN by reloc target, not this
   shared one -- overwritten below). No slot 31: this class has none. */
static void ov94_fill_shared(void **vt)
{
    vt[1]  = (void *)ov94_binit;
    vt[2]  = (void *)ov94_ainit;
    vt[4]  = (void *)ov94_bclean;
    vt[5]  = (void *)ov94_aclean;
    vt[7]  = (void *)ov94_bbeh;
    vt[8]  = (void *)ov94_abeh;
    vt[10] = (void *)ov94_bren;
    vt[11] = (void *)ov94_aren;
    vt[12] = (void *)ov94_pdes;
    vt[13] = (void *)ov94_trap13;
    vt[14] = (void *)ov94_trap14;
    vt[15] = (void *)ov94_heap;
    vt[18] = (void *)ov94_yoshi;
    vt[19] = (void *)ov94_egg;
    vt[20] = (void *)ov94_v50;
    vt[21] = (void *)ov94_pounded;
    vt[22] = (void *)ov94_atk1;
    vt[23] = (void *)ov94_atk2;
    vt[24] = (void *)ov94_kicked;
    vt[25] = (void *)ov94_pushed;
    vt[26] = (void *)ov94_cannon;
    vt[27] = (void *)ov94_mega;
    vt[28] = (void *)ov94_under;
    vt[29] = (void *)ov94_aimed;
    vt[30] = (void *)port_actor_s30_base;
}

// ============================================================================
// HOOT_THE_OWL (daOwl_c, 234), an Enemy, 31 slots (no own Kill). Own
// overrides: 0 Init, 3 Cleanup, 6 Behavior, 9 Render, 12 OnPendingDestroy
// (own, empty body -- overrides ov94_pdes above), 16 D1, 17 D0. No other
// own slot -- confirmed by the raw vtable dump (words 18..29 all match the
// shared arm9 defaults byte for byte; slot 30 has no reloc at all, per the
// shared Enemy trap).
// ============================================================================
static int __fastcall hoot_init(void *s, void *)
{ return _ZN10HootTheOwl13InitResourcesEv(s); }
static int __fastcall hoot_clean(void *s, void *)
{ (void)s; return _ZN10HootTheOwl16CleanupResourcesEv(); }
static int __fastcall hoot_behavior(void *s, void *)
{ return _ZN10HootTheOwl8BehaviorEv(s); }
static int __fastcall hoot_render(void *s, void *)
{ port_actor_render_probe("HOOT_THE_OWL", (char *)s + 0x30c);
  return _ZN10HootTheOwl6RenderEv(s); }
static int __fastcall hoot_pdes(void *s, void *)
{ (void)s; _ZN10HootTheOwl16OnPendingDestroyEv(); return 0; }
static int __fastcall hoot_d1(void *s, void *)
{ return (int)(size_t)_ZN10HootTheOwlD1Ev((int *)s); }
static int __fastcall hoot_d0(void *s, void *)
{ return (int)(size_t)_ZN10HootTheOwlD0Ev((int *)s); }

extern "C" void hal_fill_hoot_the_owl_vtable(void)
{
    /* seat the five state cells BEFORE InitResources can dispatch through
       them (func_ov094_02136188 dispatches the "enter" half immediately). */
    port_hoot_the_owl_states_seat();
    void **vt = (void **)_ZTV10HootTheOwl;
    ov94_fill_shared(vt);
    vt[0]  = (void *)hoot_init;
    vt[3]  = (void *)hoot_clean;
    vt[6]  = (void *)hoot_behavior;
    vt[9]  = (void *)hoot_render;
    vt[12] = (void *)hoot_pdes;
    vt[16] = (void *)hoot_d1;
    vt[17] = (void *)hoot_d0;
    /* no slot 31: a 31-slot Enemy, no own Kill, ends here */
}

// ---- method face --------------------------------------------------------
// _ZN10HootTheOwl13InitResourcesEv is a real MSVC method against
// HootTheOwl.h (?InitResources@HootTheOwl@@..., not __ZN10HootTheOwl...) --
// faced here, the BabyPenguin/IceSheet/OneUpLogo recipe. Behavior and
// Render are NOT faced: both are host copies (port/unmatched/
// HootTheOwl_StateDispatch.cpp and port/unmatched/ModelAnim_Renders.cpp).
extern "C" int _ZN10HootTheOwl13InitResourcesEv(void *self)
{ return ((HootTheOwl *)self)->HootTheOwl::InitResources(); }
