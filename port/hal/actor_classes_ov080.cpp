// RUN LINKFULL (lane VARIANT5): _ZTV13daObjMaruta_c, the rolling logs' base
// table, ov080 0x02128338, THIRTY-TWO slots, as a host array.
//
// daObjMaruta_c is the abstract base of the two rolling logs, daObjFlMaruta_c
// (ROLLING_LOG_LLL, id 70, ov022) and daObjHmMaruta_c (ROLLING_LOG_TTM, id 102,
// ov030); include/daObjMaruta_c.h has the class. It has no actor id and no
// profile of its own. Its table lives in ov080 with its code, and the ROM
// reaches it from nine literal-pool words (config relocs, to:0x02128338): both
// leaves' factories store it before their own table, both leaves' D1 and D0
// store it between their own and dBgActor_c's, and this class's own D1 and D0
// open with it.
//
// IT USED TO BE A RAW MOUNT (port/ov080_syms.txt): 0x88 bytes of ROM data whose
// slot words were DS code addresses, because --pack rebases only a pointer
// whose target lands inside its own emission. Nothing ever dispatched through
// it -- it is installed only between two stores, on the way into and out of a
// leaf -- so the DS addresses never ran, and nothing in the port could name
// this class's own two destructors either. Every other derived vtable in the
// port is a host array ("derived vtables are host arrays, never mounted
// storage": port/ov080_syms.txt's own rule for the four ov080 tables it left
// out), and this one is now too.
//
// THE CONTENTS ARE THE ROM TABLE'S, slot for slot, read off
// config/arm9/overlays/ov080/relocs.txt at 0x02128338 + 4 * slot:
//    0  null (pure: InitResources)      16  _ZN13daObjMaruta_cD1Ev (ov080)
//    1  dActor_c::BeforeInitResources    17  _ZN13daObjMaruta_cD0Ev (ov080)
//    2  dActor_c::AfterInitResources     18  dActor_c::OnYoshiTryEat
//    3  null (pure: CleanupResources)    19  dActor_c::OnTurnIntoEgg
//    4  dActor_c::BeforeCleanupResources 20  dActor_c::Virtual50
//    5  dActor_c::AfterCleanupResources  21  dActor_c::OnGroundPounded
//    6  null (pure: Behavior)            22  dActor_c::OnAttacked1
//    7  dActor_c::BeforeBehavior         23  dActor_c::OnAttacked2
//    8  dActor_c::AfterBehavior          24  dActor_c::OnKicked
//    9  daObjMaruta_c::Render (ov080)    25  dActor_c::OnPushed
//   10  dActor_c::BeforeRender           26  dActor_c::OnHitByCannonBlastedChar
//   11  dActor_c::AfterRender            27  daObjMaruta_c::OnHitByMegaChar (ov080)
//   12  fBase_c::OnPendingDestroy        28  dActor_c::OnHitFromUnderneath
//   13  fBase_c::Virtual34               29  dActor_c::OnAimedAtWithEgg
//   14  fBase_c::Virtual38               30  dActor_c::OnAimedAtWithEggReturnVec
//   15  fBase_c::OnHeapCreated           31  dBgActor_c::Kill
// Every thunk below is the one both leaves' own fills already seat for the same
// slot (hal/actor_classes_ov022_w3e.cpp's rolling-log fill and its shared half,
// the vtable law's two-argument __fastcall, the extra stack parameter where the
// dispatch site pushes one). The three pure slots and 13/14, which no table in
// the port hosts, are declining traps, the base-table convention of
// hal/actor_classes.cpp's Platform and Enemy tables. Slot 16 carries the
// destructor slot's own calling convention (hal/port_d16.h).
//
// CONSTANT-INITIALISED, the shape of _ZTV14ArrowSignRight in
// hal/actor_vtables.cpp, rather than filled by a registry call: the class is
// never registered (no actor id), so there is no fill for the registry to run,
// and the table is correct from the first instruction. It sits inside .dsstate
// with every other hosted vtable.
#include "port_d16.h"

#include <cstdio>

#include "dsstate_seg.h"
#include "dActor_c.h"
#include "fBase_c.h"

extern "C" void *__fastcall port_actor_s30_base(void *self, void *, void *out);

extern "C" {
int _ZN8dActor_c19BeforeInitResourcesEv(void *self);              /* slot 1  */
void _ZN8dActor_c18AfterInitResourcesEj(void *self, unsigned a);  /* slot 2  */
int _ZN8dActor_c14BeforeBehaviorEv(void *self);                   /* slot 7  */
int _ZN8dActor_c12BeforeRenderEv(void *self);                     /* slot 10 */
int _ZN8dActor_c13OnYoshiTryEatEv(void *self);                    /* slot 18 */
void _ZN8dActor_c13OnTurnIntoEggER6Player(void *self, void *p);   /* slot 19 */
int _ZN8dActor_c9Virtual50Ev(void *self);                         /* slot 20 */
void _ZN8dActor_c15OnGroundPoundedERS_(void *self, void *o);      /* slot 21 */
void _ZN8dActor_c11OnAttacked1ERS_(void *self, void *o);          /* slot 22 */
void _ZN8dActor_c11OnAttacked2ERS_(void *self, void *o);          /* slot 23 */
void _ZN8dActor_c8OnKickedERS_(void *self, void *o);              /* slot 24 */
void _ZN8dActor_c8OnPushedERS_(void *self, void *o);              /* slot 25 */
void _ZN8dActor_c24OnHitByCannonBlastedCharERS_(void *self, void *o); /* slot 26 */
void _ZN8dActor_c19OnHitFromUnderneathERS_(void *self, void *o);      /* slot 28 */
int _ZN8dActor_c16OnAimedAtWithEggEv(void *self);                     /* slot 29 */
void _ZN10dBgActor_c4KillEv(void *self);                              /* slot 31 */

int _ZN13daObjMaruta_c6RenderEv(void *self);                            /* slot 9,  ov080 */
void _ZN13daObjMaruta_c15OnHitByMegaCharER6Player(void *self, void *p); /* slot 27, ov080 */
/* slots 16 and 17: the #ifdef _MSC_VER arms of src/_ZN13daObjMaruta_cD1Ev.cpp
   and src/_ZN13daObjMaruta_cD0Ev.cpp, the ROM's own two bodies spelled step
   for step */
void *_ZN13daObjMaruta_cD1Ev(void *self);
void *_ZN13daObjMaruta_cD0Ev(void *self);

const char *port_actor_class_name(unsigned id);   /* hal/actor_registry */
void port_actor_slot_decline(const char *what);   /* func_02043fdc_hostcopy.cpp */
}

// ---- the trap --------------------------------------------------------------
static void mar_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: _ZTV13daObjMaruta_c slot %d dispatched (actor id "
                 "%u %s); the class is abstract and the ROM word is %s\n",
                 slot, id, port_actor_class_name(id),
                 (slot == 0 || slot == 3 || slot == 6) ? "null" : "unhosted");
    { static char _m[128];
      std::snprintf(_m, sizeof _m, "_ZTV13daObjMaruta_c slot %d on id %u %s",
                    slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
#define MAR_TRAP(n) \
    static int __fastcall mar_trap##n(void *s, void *) \
    { mar_trap_report(s, n); return 0; }
MAR_TRAP(0) MAR_TRAP(3) MAR_TRAP(6) MAR_TRAP(13) MAR_TRAP(14)
#undef MAR_TRAP

// ---- the inherited slots, the thunks both leaves' fills seat ---------------
static int __fastcall mar_binit(void *s, void *)
{ return _ZN8dActor_c19BeforeInitResourcesEv(s); }
static void __fastcall mar_ainit(void *s, void *, unsigned a)
{ _ZN8dActor_c18AfterInitResourcesEj(s, a); }
static int __fastcall mar_bclean(void *s, void *)
{ return ((dActor_c *)s)->dActor_c::BeforeCleanupResources(); }
static void __fastcall mar_aclean(void *s, void *, unsigned a)
{ ((fBase_c *)s)->fBase_c::AfterCleanupResources(a); }
static int __fastcall mar_bbeh(void *s, void *)
{ return _ZN8dActor_c14BeforeBehaviorEv(s); }
static void __fastcall mar_abeh(void *s, void *, unsigned a)
{ ((fBase_c *)s)->fBase_c::AfterBehavior(a); }
static int __fastcall mar_bren(void *s, void *)
{ return _ZN8dActor_c12BeforeRenderEv(s); }
static void __fastcall mar_aren(void *s, void *, unsigned a)
{ ((fBase_c *)s)->fBase_c::AfterRender(a); }
static int __fastcall mar_pdes(void *s, void *)
{ ((fBase_c *)s)->fBase_c::OnPendingDestroy(); return 0; }
static int __fastcall mar_heap(void *s, void *)
{ return ((fBase_c *)s)->fBase_c::OnHeapCreated(); }
static int __fastcall mar_yoshi(void *s, void *)
{ return _ZN8dActor_c13OnYoshiTryEatEv(s); }
/* slot 19 takes the three-parameter shape so it emits `ret 4`: the dispatch
   site pushes the Player the callee pops. The same holds for 21..28. */
static int __fastcall mar_turn_egg(void *s, void *, void *p)
{ _ZN8dActor_c13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall mar_v50(void *s, void *)
{ return _ZN8dActor_c9Virtual50Ev(s); }
static int __fastcall mar_pounded(void *s, void *, void *o)
{ _ZN8dActor_c15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall mar_atk1(void *s, void *, void *o)
{ _ZN8dActor_c11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall mar_atk2(void *s, void *, void *o)
{ _ZN8dActor_c11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall mar_kicked(void *s, void *, void *o)
{ _ZN8dActor_c8OnKickedERS_(s, o); return 0; }
static int __fastcall mar_pushed(void *s, void *, void *o)
{ _ZN8dActor_c8OnPushedERS_(s, o); return 0; }
static int __fastcall mar_cannon(void *s, void *, void *o)
{ _ZN8dActor_c24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall mar_under(void *s, void *, void *o)
{ _ZN8dActor_c19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall mar_egg(void *s, void *)
{ return _ZN8dActor_c16OnAimedAtWithEggEv(s); }
static int __fastcall mar_kill(void *s, void *)
{ _ZN10dBgActor_c4KillEv(s); return 0; }

// ---- the class's own four ---------------------------------------------------
static int __fastcall mar_render(void *s, void *)
{ return _ZN13daObjMaruta_c6RenderEv(s); }
static int __fastcall mar_d1(void *s, void *)
{ return (int)(size_t)_ZN13daObjMaruta_cD1Ev(s); }
static int __fastcall mar_d0(void *s, void *)
{ return (int)(size_t)_ZN13daObjMaruta_cD0Ev(s); }
static int __fastcall mar_mega(void *s, void *, void *p)
{ _ZN13daObjMaruta_c15OnHitByMegaCharER6Player(s, p); return 0; }

DSSTATE_BEGIN
extern "C" {
void *_ZTV13daObjMaruta_c[32] = {
    (void *)mar_trap0,             /*  0 InitResources (pure) */
    (void *)mar_binit,             /*  1 */
    (void *)mar_ainit,             /*  2 */
    (void *)mar_trap3,             /*  3 CleanupResources (pure) */
    (void *)mar_bclean,            /*  4 */
    (void *)mar_aclean,            /*  5 */
    (void *)mar_trap6,             /*  6 Behavior (pure) */
    (void *)mar_bbeh,              /*  7 */
    (void *)mar_abeh,              /*  8 */
    (void *)mar_render,            /*  9 daObjMaruta_c::Render */
    (void *)mar_bren,              /* 10 */
    (void *)mar_aren,              /* 11 */
    (void *)mar_pdes,              /* 12 */
    (void *)mar_trap13,            /* 13 */
    (void *)mar_trap14,            /* 14 */
    (void *)mar_heap,              /* 15 */
    (void *)PORT_D16(mar_d1),      /* 16 D1, 0x02127014 */
    (void *)mar_d0,                /* 17 D0, 0x02126fbc */
    (void *)mar_yoshi,             /* 18 */
    (void *)mar_turn_egg,          /* 19 */
    (void *)mar_v50,               /* 20 */
    (void *)mar_pounded,           /* 21 */
    (void *)mar_atk1,              /* 22 */
    (void *)mar_atk2,              /* 23 */
    (void *)mar_kicked,            /* 24 */
    (void *)mar_pushed,            /* 25 */
    (void *)mar_cannon,            /* 26 */
    (void *)mar_mega,              /* 27 daObjMaruta_c::OnHitByMegaChar */
    (void *)mar_under,             /* 28 */
    (void *)mar_egg,               /* 29 */
    (void *)port_actor_s30_base,   /* 30 */
    (void *)mar_kill,              /* 31 dBgActor_c::Kill */
};
}
DSSTATE_END
