// RUN LINKFULL (lane VARIANT5): _ZTV16daObjFallBlock_c, the falling blocks'
// base table, ov098 0x0213c5bc, THIRTY-TWO slots, as a host array.
//
// daObjFallBlock_c is the abstract base of the four falling blocks, one per
// level overlay: daObjBk_Fall_Block_c (FALL_BLOCK_WF, id 45, ov015),
// daObjFl_Fall_Block_c (FALL_BLOCK_LLL, id 83, ov022), daObjKm2_Fall_Block_c
// (FALL_BLOCK_BFS, id 139, ov045) and daObjTh_Fall_Block_c (FALL_BLOCK_BBH,
// id 66, ov063); include/daObjFallBlock_c.h has the class. It has no actor id
// and no profile of its own. Its table lives in ov098 with its code, and the
// ROM reaches it from fourteen literal-pool words (config relocs,
// to:0x0213c5bc): every leaf's factory stores it before the leaf's own table,
// every leaf's D1 and D0 store it between their own and dBgActor_c's, and this
// class's own D1 and D0 open with it.
//
// IT USED TO BE A RAW MOUNT (port/ov098_syms.txt): 0x80 bytes of ROM data whose
// slot words were DS code addresses, because --pack rebases only a pointer
// whose target lands inside its own emission. Nothing ever dispatched through
// it -- it is installed only between two stores, on the way into and out of a
// leaf -- so the DS addresses never ran, and nothing in the port could name
// this class's own two destructors either. Every other derived vtable in the
// port is a host array ("the vtables are left out because they are host
// arrays the registry fills": port/ov098_syms.txt's own header), and this one
// is now too.
//
// THE CONTENTS ARE THE ROM TABLE'S, slot for slot, read off
// config/arm9/overlays/ov098/relocs.txt at 0x0213c5bc + 4 * slot:
//    0  null (pure: InitResources)      16  _ZN16daObjFallBlock_cD1Ev (ov098)
//    1  dActor_c::BeforeInitResources    17  _ZN16daObjFallBlock_cD0Ev (ov098)
//    2  dActor_c::AfterInitResources     18  dActor_c::OnYoshiTryEat
//    3  null (pure: CleanupResources)    19  dActor_c::OnTurnIntoEgg
//    4  dActor_c::BeforeCleanupResources 20  dActor_c::Virtual50
//    5  dActor_c::AfterCleanupResources  21  dActor_c::OnGroundPounded
//    6  daObjFallBlock_c::Behavior       22  dActor_c::OnAttacked1
//    7  dActor_c::BeforeBehavior         23  dActor_c::OnAttacked2
//    8  dActor_c::AfterBehavior          24  dActor_c::OnKicked
//    9  daObjFallBlock_c::Render         25  dActor_c::OnPushed
//   10  dActor_c::BeforeRender           26  dActor_c::OnHitByCannonBlastedChar
//   11  dActor_c::AfterRender            27  daObjFallBlock_c::OnHitByMegaChar
//   12  fBase_c::OnPendingDestroy        28  dActor_c::OnHitFromUnderneath
//   13  fBase_c::Virtual34               29  dActor_c::OnAimedAtWithEgg
//   14  fBase_c::Virtual38               30  dActor_c::OnAimedAtWithEggReturnVec
//   15  fBase_c::OnHeapCreated           31  daObjFallBlock_c::Kill
// Every thunk below is the one the leaves' own fills already seat for the same
// slot (hal/actor_classes_ov022_w16.cpp's FALL_BLOCK_LLL fill, whose six ov098
// bodies are these; the vtable law's two-argument __fastcall, the extra stack
// parameter where the dispatch site pushes one). The two pure slots and 13/14,
// which no table in the port hosts, are declining traps, the base-table
// convention of hal/actor_classes.cpp's Platform and Enemy tables. Slot 16
// carries the destructor slot's own calling convention (hal/port_d16.h).
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

/* the class's own four ov098 bodies, each already in the link (the leaves
   seat them) */
int _ZN16daObjFallBlock_c8BehaviorEv(char *self);                  /* slot 6  */
int _ZN16daObjFallBlock_c6RenderEv(char *self);                    /* slot 9  */
void _ZN16daObjFallBlock_c15OnHitByMegaCharER6Player(char *self);  /* slot 27 */
void _ZN16daObjFallBlock_c4KillEv(char *self);                     /* slot 31 */
/* slots 16 and 17: the #ifdef _MSC_VER arms of src/_ZN16daObjFallBlock_cD1Ev.cpp
   and src/_ZN16daObjFallBlock_cD0Ev.cpp, the ROM's own two bodies spelled
   step for step */
void *_ZN16daObjFallBlock_cD1Ev(void *self);
void *_ZN16daObjFallBlock_cD0Ev(void *self);

const char *port_actor_class_name(unsigned id);   /* hal/actor_registry */
void port_actor_slot_decline(const char *what);   /* func_02043fdc_hostcopy.cpp */
}

// ---- the trap --------------------------------------------------------------
static void fbb_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: _ZTV16daObjFallBlock_c slot %d dispatched (actor "
                 "id %u %s); the class is abstract and the ROM word is %s\n",
                 slot, id, port_actor_class_name(id),
                 (slot == 0 || slot == 3) ? "null" : "unhosted");
    { static char _m[128];
      std::snprintf(_m, sizeof _m, "_ZTV16daObjFallBlock_c slot %d on id %u %s",
                    slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
#define FBB_TRAP(n) \
    static int __fastcall fbb_trap##n(void *s, void *) \
    { fbb_trap_report(s, n); return 0; }
FBB_TRAP(0) FBB_TRAP(3) FBB_TRAP(13) FBB_TRAP(14)
#undef FBB_TRAP

// ---- the inherited slots, the thunks the leaves' fills seat -----------------
static int __fastcall fbb_binit(void *s, void *)
{ return _ZN8dActor_c19BeforeInitResourcesEv(s); }
static void __fastcall fbb_ainit(void *s, void *, unsigned a)
{ _ZN8dActor_c18AfterInitResourcesEj(s, a); }
static int __fastcall fbb_bclean(void *s, void *)
{ return ((dActor_c *)s)->dActor_c::BeforeCleanupResources(); }
static void __fastcall fbb_aclean(void *s, void *, unsigned a)
{ ((fBase_c *)s)->fBase_c::AfterCleanupResources(a); }
static int __fastcall fbb_bbeh(void *s, void *)
{ return _ZN8dActor_c14BeforeBehaviorEv(s); }
static void __fastcall fbb_abeh(void *s, void *, unsigned a)
{ ((fBase_c *)s)->fBase_c::AfterBehavior(a); }
static int __fastcall fbb_bren(void *s, void *)
{ return _ZN8dActor_c12BeforeRenderEv(s); }
static void __fastcall fbb_aren(void *s, void *, unsigned a)
{ ((fBase_c *)s)->fBase_c::AfterRender(a); }
static int __fastcall fbb_pdes(void *s, void *)
{ ((fBase_c *)s)->fBase_c::OnPendingDestroy(); return 0; }
static int __fastcall fbb_heap(void *s, void *)
{ return ((fBase_c *)s)->fBase_c::OnHeapCreated(); }
static int __fastcall fbb_yoshi(void *s, void *)
{ return _ZN8dActor_c13OnYoshiTryEatEv(s); }
/* slot 19 takes the three-parameter shape so it emits `ret 4`: the dispatch
   site pushes the Player the callee pops. The same holds for 21..28. */
static int __fastcall fbb_turn_egg(void *s, void *, void *p)
{ _ZN8dActor_c13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall fbb_v50(void *s, void *)
{ return _ZN8dActor_c9Virtual50Ev(s); }
static int __fastcall fbb_pounded(void *s, void *, void *o)
{ _ZN8dActor_c15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall fbb_atk1(void *s, void *, void *o)
{ _ZN8dActor_c11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall fbb_atk2(void *s, void *, void *o)
{ _ZN8dActor_c11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall fbb_kicked(void *s, void *, void *o)
{ _ZN8dActor_c8OnKickedERS_(s, o); return 0; }
static int __fastcall fbb_pushed(void *s, void *, void *o)
{ _ZN8dActor_c8OnPushedERS_(s, o); return 0; }
static int __fastcall fbb_cannon(void *s, void *, void *o)
{ _ZN8dActor_c24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall fbb_under(void *s, void *, void *o)
{ _ZN8dActor_c19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall fbb_egg(void *s, void *)
{ return _ZN8dActor_c16OnAimedAtWithEggEv(s); }

// ---- the class's own six ----------------------------------------------------
static int __fastcall fbb_behavior(void *s, void *)
{ return _ZN16daObjFallBlock_c8BehaviorEv((char *)s); }
static int __fastcall fbb_render(void *s, void *)
{ return _ZN16daObjFallBlock_c6RenderEv((char *)s); }
static int __fastcall fbb_d1(void *s, void *)
{ return (int)(size_t)_ZN16daObjFallBlock_cD1Ev(s); }
static int __fastcall fbb_d0(void *s, void *)
{ return (int)(size_t)_ZN16daObjFallBlock_cD0Ev(s); }
/* Slot 27 is OnHitByMegaChar(Player &): the caller pushes the player, so the
   thunk takes the third parameter to pop it even though the ov098 body reads
   only the receiver -- the fb_slot27 contract the leaves' fills use. */
static int __fastcall fbb_slot27(void *s, void *, void *)
{ _ZN16daObjFallBlock_c15OnHitByMegaCharER6Player((char *)s); return 0; }
static int __fastcall fbb_slot31(void *s, void *)
{ _ZN16daObjFallBlock_c4KillEv((char *)s); return 0; }

DSSTATE_BEGIN
extern "C" {
void *_ZTV16daObjFallBlock_c[32] = {
    (void *)fbb_trap0,             /*  0 InitResources (pure) */
    (void *)fbb_binit,             /*  1 */
    (void *)fbb_ainit,             /*  2 */
    (void *)fbb_trap3,             /*  3 CleanupResources (pure) */
    (void *)fbb_bclean,            /*  4 */
    (void *)fbb_aclean,            /*  5 */
    (void *)fbb_behavior,          /*  6 daObjFallBlock_c::Behavior */
    (void *)fbb_bbeh,              /*  7 */
    (void *)fbb_abeh,              /*  8 */
    (void *)fbb_render,            /*  9 daObjFallBlock_c::Render */
    (void *)fbb_bren,              /* 10 */
    (void *)fbb_aren,              /* 11 */
    (void *)fbb_pdes,              /* 12 */
    (void *)fbb_trap13,            /* 13 */
    (void *)fbb_trap14,            /* 14 */
    (void *)fbb_heap,              /* 15 */
    (void *)PORT_D16(fbb_d1),      /* 16 D1, 0x02139fc8 */
    (void *)fbb_d0,                /* 17 D0, 0x02139f70 */
    (void *)fbb_yoshi,             /* 18 */
    (void *)fbb_turn_egg,          /* 19 */
    (void *)fbb_v50,               /* 20 */
    (void *)fbb_pounded,           /* 21 */
    (void *)fbb_atk1,              /* 22 */
    (void *)fbb_atk2,              /* 23 */
    (void *)fbb_kicked,            /* 24 */
    (void *)fbb_pushed,            /* 25 */
    (void *)fbb_cannon,            /* 26 */
    (void *)fbb_slot27,            /* 27 daObjFallBlock_c::OnHitByMegaChar */
    (void *)fbb_under,             /* 28 */
    (void *)fbb_egg,               /* 29 */
    (void *)port_actor_s30_base,   /* 30 */
    (void *)fbb_slot31,            /* 31 daObjFallBlock_c::Kill */
};
}
DSSTATE_END
