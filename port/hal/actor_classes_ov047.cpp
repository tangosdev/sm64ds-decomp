// RUN REL0215 WAVE 2 (lane cast-sweep2): OV047'S FOUR CLASSES (level 39,
// Bowser in the Sky). Overlay 3 of 4 in this lane.
//
// Level 39 spawns 96 actors and skips 9 across five ids on tip 3b4863d9a.
// SEVEN of the nine are this overlay's own cast; the other two are id 157
// (ArrowPathLift, both-routes owner ov091) and id 192 (PushBlock, ov002).
//
//   id   name                   x on L39  factory                    table
//   151  RICKSHAW_PLATFORM_BS   12*       RickshawPlatformBs_Spawn   0x0211244c
//   152  RICKSHAW_BS             3        RickshawBs_Spawn           0x021122a0
//   153  STAIRS_BS               1        StairsBs_Spawn             0x0211254c
//   156  ROTATING_PLATFORM_BS    3        func_ov047_021113bc        0x0211237c
//
// * id 151 IS NOT IN THE LEVEL'S SKIP LIST. It is a RUNTIME CHILD: the three
// RICKSHAW_BS axles each build four RICKSHAW_PLATFORM_BS carts, so while the
// parent was itself skipped nothing ever asked the pre-spawn gate for id 151.
// It was registered because both attribution routes named it and every part of
// it is this overlay's -- and the seat then measured twelve of them. Left out,
// those twelve would have been declined by name one at a time.
//
// ---- THE DSD CLASS NAMES ARE ATTACHED ONE TABLE LATE ----------------------
//
// dsd names TWO vtables here and both are on the wrong class. In .data address
// order, with the factory install and the RTTI at vtable[-1] read out of the
// image:
//
//   0x021122a0  RickshawBs_Spawn (alloc 0x330)          21daObjKm3_Kurumajiku_c
//   0x0211237c  func_ov047_021113bc (alloc 0x320)       20daObjKm3_Kaitendai_c
//   0x0211244c  RickshawPlatformBs_Spawn (alloc 0x320)  17daObjKm3_Kuruma_c
//   0x0211254c  StairsBs_Spawn (alloc 0xdcc)            17daObjKm3_Dorifu_c
//
// and dsd's _ZTV10RickshawBs is 0x0211237c while _ZTV18RickshawPlatformBs is
// 0x0211254c -- one table LATER than the class each names. So every
// _ZN10RickshawBs* body in src/ is ROTATING_PLATFORM_BS's and every
// _ZN18RickshawPlatformBs* body is STAIRS_BS's. The rows below are wired by
// ADDRESS; the dsd spellings survive only as host array names, which is what
// lets the matched TUs link unchanged. This is the ov036/ov045
// attached-one-class-early shape. Derivation: port/ov047_syms.txt.
//
// tools/actor_names.py saw the same defect from the other side and wrote it
// down: "NOVTABLE actor 153 StairsBs" (the table that is 153's was already
// claimed) and "AMBIG actor 156 ROTATING_PLATFORM_BS: ['ov033','ov047'] ->
// ov033" (it tie-broke to ov033; both attribution routes say ov047). The row
// name for 156 is the one that report derived, and its RTTI agrees.
//
// ---- FOUR COPIES OF ONE SHAPE --------------------------------------------
//
// Every class here is a Platform with a 32-slot table, four own slots
// (0 InitResources, 3 CleanupResources, 16 D1, 17 D0) and slots 6 and 9
// INHERITED from an ov002 intermediate base. So the overlay is four factories,
// four Init/Cleanup pairs, four destructor pairs and four sinits, and nothing
// else. Eight of those inherited bodies are ov002 TUs; four were already in the
// link and four are this lane's measured closure.
//
// ---- T1: ALL FOUR TABLES STAY ROM-SHAPED ---------------------------------
//
// No class here derives from Model or ModelAnim: 151/152/156 HOLD a Model at
// +0xd4 and a MovingMeshCollider at +0x124, and 153 holds an ARRAY of five of
// each at +0x320 and +0x4b0 plus its own pair. So the default applies:
// ROM-shaped, [16] D1, [17] D0, slot 31 Platform::Kill. No slot-5 collision
// anywhere -- the inherited Renders are ov002 bodies that ride from src.
//
// ---- WHY THE MOUNT BRING-UP IS IN THIS FILE -------------------------------
//
// The ov025/ov032/ov033/ov035/ov045/ov052/ov056/ov073 lane-ownership pattern,
// behind one DSSTATE-bracketed done-guard: the pack check, the syms patch,
// hal_fill_platform_vtable (all eight destructors install ov002 0x0210ae38 as
// the base vptr on the way out), the two ov002 intermediate bases this lane
// hosts, and then all FOUR ov047 sinits in ROM order.
#include <cstdio>
#include "dsstate_seg.h"
#include <cstdlib>

#include "Actor.h"
#include "ActorBase.h"

extern "C" {
/* the arm9 shared half */
int _ZN5Actor19BeforeInitResourcesEv(void *self);              /* slot 1  */
void _ZN5Actor18AfterInitResourcesEj(void *self, unsigned a);  /* slot 2  */
int _ZN5Actor14BeforeBehaviorEv(void *self);                   /* slot 7  */
int _ZN5Actor12BeforeRenderEv(void *self);                     /* slot 10 */
int _ZN5Actor13OnYoshiTryEatEv(void *self);                    /* slot 18 */
void _ZN5Actor13OnTurnIntoEggER6Player(void *self, void *p);   /* slot 19 */
int _ZN5Actor9Virtual50Ev(void *self);                         /* slot 20 */
void _ZN5Actor15OnGroundPoundedERS_(void *self, void *o);      /* slot 21 */
void _ZN5Actor11OnAttacked1ERS_(void *self, void *o);          /* slot 22 */
void _ZN5Actor11OnAttacked2ERS_(void *self, void *o);          /* slot 23 */
void _ZN5Actor8OnKickedERS_(void *self, void *o);              /* slot 24 */
void _ZN5Actor8OnPushedERS_(void *self, void *o);              /* slot 25 */
void _ZN5Actor24OnHitByCannonBlastedCharERS_(void *self, void *o); /* slot 26 */
void _ZN5Actor15OnHitByMegaCharER6Player(void *self, void *p);     /* slot 27 */
void _ZN5Actor19OnHitFromUnderneathERS_(void *self, void *o);      /* slot 28 */
int _ZN5Actor16OnAimedAtWithEggEv(void *self);                     /* slot 29 */
void _ZN8Platform4KillEv(void *self);                              /* slot 31 */

const char *port_actor_class_name(unsigned id);   /* hal/actor_registry */
void port_actor_slot_decline(const char *what);   /* func_02043fdc_hostcopy.cpp */
void port_actor_render_probe(const char *cls, void *model); /* hal/actor_classes */
void hal_fill_platform_vtable(void);              /* hal/actor_classes.cpp */

/* the generated ov047 per-symbol mount (build/port/host-src/ov047_syms.c) */
void port_ov047_pack_check(void);
void port_ov047_syms_patch(void);
/* all four sinits, one per class, in ROM order: 152, 156, 151, 153 */
void __sinit_ov047_021116dc(void);
void __sinit_ov047_02111748(void);
void __sinit_ov047_021117b4(void);
void __sinit_ov047_0211181c(void);

/* ---- id 152 RICKSHAW_BS -- dsd left this class unnamed ------------------ */
int func_ov047_02111268(void *self);       /* slot 0,  InitResources */
int func_ov047_02111254(void *self);       /* slot 3,  CleanupResources */
int *func_ov047_021111a0(int *self);       /* slot 16, D1 */
int *func_ov047_021111f0(int *self);       /* slot 17, D0 */
int *RickshawBs_Spawn(void);               /* id 152 */
int func_ov002_020b6b38(void *self);       /* slot 6,  inherited Behavior */
int func_ov002_020b6b10(void *self);       /* slot 9,  inherited Render */

/* ---- id 156 ROTATING_PLATFORM_BS -- these are the ones dsd spelled
   _ZN10RickshawBs*, and they belong to the ROTATING class ---------------- */
int _ZN10RickshawBs13InitResourcesEv(void *self);      /* slot 0  */
int _ZN10RickshawBs16CleanupResourcesEv(void *self);   /* slot 3  */
int *_ZN10RickshawBsD1Ev(int *self);                   /* slot 16 */
int *_ZN10RickshawBsD0Ev(int *self);                   /* slot 17 */
int *func_ov047_021113bc(void);                        /* id 156 */
int func_ov002_020b6718(void *self);       /* slot 6,  inherited Behavior */
int func_ov002_020b66f0(void *self);       /* slot 9,  inherited Render */

/* ---- id 151 RICKSHAW_PLATFORM_BS -- dsd left this class unnamed too ---- */
int func_ov047_021114c0(void *self);       /* slot 0,  InitResources */
int func_ov047_021114ac(void *self);       /* slot 3,  CleanupResources */
int *func_ov047_021113f8(int *self);       /* slot 16, D1 */
int *func_ov047_02111448(int *self);       /* slot 17, D0 */
int *RickshawPlatformBs_Spawn(void);       /* id 151 */
int func_ov002_020b6920(void *self);       /* slot 6,  inherited Behavior */
int func_ov002_020b68f8(void *self);       /* slot 9,  inherited Render */

/* ---- id 153 STAIRS_BS -- these are the ones dsd spelled
   _ZN18RickshawPlatformBs*, and they belong to the STAIRS class ---------- */
int *_ZN18RickshawPlatformBsD1Ev(void *self);          /* slot 16 */
int *_ZN18RickshawPlatformBsD0Ev(void *self);          /* slot 17 */
void *StairsBs_Spawn(void);                            /* id 153 */
int func_ov002_020b4bfc(void *self);       /* slot 6,  inherited Behavior */
int func_ov002_020b4bc4(void *self);       /* slot 9,  inherited Render */

/* the four host vtables, all excluded from the mount. The two dsd names are
   dsd's for the ADDRESSES, not for the classes -- see this file's header. */
DSSTATE_BEGIN
int data_ov047_021122a0[32];        /* 0x021122a0, id 152 RICKSHAW_BS */
int _ZTV10RickshawBs[32];           /* 0x0211237c, id 156 ROTATING_PLATFORM_BS */
int data_ov047_0211244c[32];        /* 0x0211244c, id 151 RICKSHAW_PLATFORM_BS */
int _ZTV18RickshawPlatformBs[32];   /* 0x0211254c, id 153 STAIRS_BS */
/* THE TWO OV002 INTERMEDIATE BASES THIS LANE HOSTS. The other two this overlay
   needs, data_ov002_021091d4 (daObjKaitendai_c) and data_ov002_02108d94
   (daObjDorifu_c), are already hosted and trap-filled by
   hal/actor_classes_ov036.cpp; these two are the same shape and get the same
   treatment. Both are DS-shaped names, so they sit inside the .dsstate span
   (dsstate_guard fails the build otherwise -- the ov036 reading). */
void *data_ov002_02109320[32];      /* id 152's intermediate base */
void *data_ov002_02109278[32];      /* id 151's intermediate base */
DSSTATE_END
}

/* THE C++-LINKAGE DATA SPELLING. STAIRS_BS's two //cpp method TUs declare
   `extern struct Arg data_ov047_02112508;` at file scope OUTSIDE decl_common.h's
   `extern "C"` block, so MSVC mangles it with the local struct type. The mount
   emits ONE C-named array, so the mangled spelling is bound onto it -- the
   ov030 / ov032 / ov034 / ov065 recipe. The LHS is a mangled name defined
   nowhere in the link, so the alias cannot be defeated and alternatename_guard
   stays clean. Read off the linker's own LNK2019 line, not derived by hand. */
#pragma comment(linker, "/alternatename:?data_ov047_02112508@@3UArg@@A=_data_ov047_02112508")

/* The two bodies src defines as real C++ methods against
   include/RickshawPlatformBs.h -- they are STAIRS_BS's (id 153), not id 151's,
   for this file's header reason -- faced here, the ov013/ov024/ov025/ov032/
   ov033/ov035 recipe. */
#include "RickshawPlatformBs.h"

/* hal/actor_slot30_seat.cpp -- the shared seat for vtable slot 30,
   Actor::OnAimedAtWithEggReturnVec. The ROM word in slot 30 of every vtable
   this file fills IS the arm9 base body 0x020100dc (checked against
   config/<module>/relocs.txt at vtable+30*4), and that body is now in the
   link from src/_ZN5Actor25OnAimedAtWithEggReturnVecEv.cpp on slice_gate50.
   The three-parameter __fastcall is the sret contract MSVC uses for a
   thiscall member returning a 12-byte struct: this in ecx, the hidden result
   pointer the one (callee-popped) stack argument. Same shape as whomp_s30. */
extern "C" void *__fastcall port_actor_s30_base(void *self, void *, void *out);
extern "C" {
int _ZN18RickshawPlatformBs13InitResourcesEv(void *self)
{ return ((RickshawPlatformBs *)self)->RickshawPlatformBs::InitResources(); }
int _ZN18RickshawPlatformBs16CleanupResourcesEv(void *self)
{ return ((RickshawPlatformBs *)self)->RickshawPlatformBs::CleanupResources(); }
}

// ---- the trap --------------------------------------------------------------
static void ov47_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: ov047 vtable slot %d is not hosted (actor id %u "
                 "%s)\n", slot, id, port_actor_class_name(id));
    { static char _m[128];
      std::snprintf(_m, sizeof _m, "unhosted ov047 vtable slot %d on id %u %s",
                    slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
#define OV47_TRAP(n) \
    static int __fastcall ov47_trap##n(void *s, void *) \
    { ov47_trap_report(s, n); return 0; }
OV47_TRAP(13) OV47_TRAP(14)
#undef OV47_TRAP

/* The two intermediate bases' own 0/3/6/9/16/17: the ROM parks ZERO in 0 and 3,
   6 and 9 are the derived class's business, and an abstract intermediate
   installed only between two member teardowns never reaches its own
   destructors. Trap-filled, the ov036 base treatment exactly. */
static void ov47_base_trap(void *s, int slot)
{
    unsigned id = s ? *(unsigned short *)((char *)s + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: ov047 intermediate base slot %d entered (actor id "
                 "%u %s) -- the ROM parks zero there\n", slot, id,
                 port_actor_class_name(id));
    { static char _m[128];
      std::snprintf(_m, sizeof _m,
                    "ov047 intermediate base slot %d on id %u", slot, id);
      port_actor_slot_decline(_m); }
}
#define OV47_BASE_TRAP(n) \
    static int __fastcall ov47_base_trap##n(void *s, void *) \
    { ov47_base_trap(s, n); return 0; }
OV47_BASE_TRAP(0) OV47_BASE_TRAP(3) OV47_BASE_TRAP(6) OV47_BASE_TRAP(9)
#undef OV47_BASE_TRAP

static int __fastcall ov47_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall ov47_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall ov47_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall ov47_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall ov47_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall ov47_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall ov47_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall ov47_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall ov47_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall ov47_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall ov47_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
static int __fastcall ov47_turn_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall ov47_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall ov47_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall ov47_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall ov47_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall ov47_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall ov47_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall ov47_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall ov47_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall ov47_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall ov47_egg(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }
static int __fastcall ov47_kill(void *s, void *)
{ _ZN8Platform4KillEv(s); return 0; }

/* The shared half of all six tables (four classes plus the two intermediate
   bases). Each caller writes its own 0/3/6/9/16/17 and, for the four class
   tables, 31.

   THE POINTER IS VOLATILE ON PURPOSE -- the gate-200 elided-stores bug, repro
   in hal/actor_classes_ov002g200.cpp. */
static void ov47_fill_shared(void *volatile *vt)
{
    vt[1]  = (void *)ov47_binit;
    vt[2]  = (void *)ov47_ainit;
    vt[4]  = (void *)ov47_bclean;
    vt[5]  = (void *)ov47_aclean;
    vt[7]  = (void *)ov47_bbeh;
    vt[8]  = (void *)ov47_abeh;
    vt[10] = (void *)ov47_bren;
    vt[11] = (void *)ov47_aren;
    vt[12] = (void *)ov47_pdes;
    vt[13] = (void *)ov47_trap13;
    vt[14] = (void *)ov47_trap14;
    vt[15] = (void *)ov47_heap;
    vt[18] = (void *)ov47_yoshi;
    vt[19] = (void *)ov47_turn_egg;
    vt[20] = (void *)ov47_v50;
    vt[21] = (void *)ov47_pounded;
    vt[22] = (void *)ov47_atk1;
    vt[23] = (void *)ov47_atk2;
    vt[24] = (void *)ov47_kicked;
    vt[25] = (void *)ov47_pushed;
    vt[26] = (void *)ov47_cannon;
    vt[27] = (void *)ov47_mega;
    vt[28] = (void *)ov47_under;
    vt[29] = (void *)ov47_egg;
    vt[30] = (void *)port_actor_s30_base;
}

/* THE `void **tabs[2]` SHAPE IS LOAD-BEARING, not stylistic: port/tools/
   vtspan.py --fills resolves a fill to the table it writes by reading the
   binding, and it recognises exactly two shapes -- one `void **vt = <named
   array>;` and this two-element loop. The ov036 base bringup carries the same
   note and the same reason. */
static void ov47_base_bringup(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    void **tabs[2] = { data_ov002_02109320, data_ov002_02109278 };
    for (int k = 0; k < 2; ++k) {
        void *volatile *vt = (void *volatile *)tabs[k];
        ov47_fill_shared(vt);
        vt[0]  = (void *)ov47_base_trap0;
        vt[3]  = (void *)ov47_base_trap3;
        vt[6]  = (void *)ov47_base_trap6;
        vt[9]  = (void *)ov47_base_trap9;
        vt[16] = (void *)ov47_base_trap0;
        vt[17] = (void *)ov47_base_trap0;
        vt[31] = (void *)ov47_kill;    /* both are Platform-derived */
    }
}

// ---- the mount bring-up ----------------------------------------------------
DSSTATE_BEGIN
static int g_ov47_bringup_done;
DSSTATE_END

extern "C" void port_ov47_bringup(void)
{
    if (g_ov47_bringup_done)
        return;
    g_ov47_bringup_done = 1;
    port_ov047_pack_check();
    port_ov047_syms_patch();
    hal_fill_platform_vtable();
    ov47_base_bringup();
    __sinit_ov047_021116dc();
    __sinit_ov047_02111748();
    __sinit_ov047_021117b4();
    __sinit_ov047_0211181c();
}

// ============================================================================
// RICKSHAW_BS (152) -- table 0x021122a0, RTTI 21daObjKm3_Kurumajiku_c, 32 slots
// ============================================================================
//
// The rickshaw's AXLE (kurumajiku). An 816-byte Platform: Model at +0xd4,
// MovingMeshCollider at +0x124, intermediate base ov002 0x02109320, from which
// it inherits Behavior and Render. Three on level 39.
static int __fastcall rb_init(void *s, void *)
{ return func_ov047_02111268(s); }
static int __fastcall rb_clean(void *s, void *)
{ return func_ov047_02111254(s); }
static int __fastcall rb_behavior(void *s, void *)
{ return func_ov002_020b6b38(s); }
static int __fastcall rb_render(void *s, void *)
{ port_actor_render_probe("RICKSHAW_BS", (char *)s + 0xd4);
  return func_ov002_020b6b10(s); }
static int __fastcall rb_d1(void *s, void *)
{ return (int)(size_t)func_ov047_021111a0((int *)s); }
static int __fastcall rb_d0(void *s, void *)
{ return (int)(size_t)func_ov047_021111f0((int *)s); }

extern "C" void hal_fill_rickshaw_bs_vtable(void)
{
    port_ov47_bringup();
    void *volatile *vt = (void *volatile *)data_ov047_021122a0;
    ov47_fill_shared(vt);
    vt[0]  = (void *)rb_init;
    vt[3]  = (void *)rb_clean;
    vt[6]  = (void *)rb_behavior;
    vt[9]  = (void *)rb_render;
    vt[16] = (void *)rb_d1;
    vt[17] = (void *)rb_d0;
    vt[31] = (void *)ov47_kill;
}

// ============================================================================
// ROTATING_PLATFORM_BS (156) -- table 0x0211237c (dsd _ZTV10RickshawBs),
// RTTI 20daObjKm3_Kaitendai_c, 32 slots
// ============================================================================
//
// The rotating stand (kaitendai). An 800-byte Platform of the same shape,
// intermediate base ov002 0x021091d4 (daObjKaitendai_c, already hosted by
// hal/actor_classes_ov036.cpp). Three on level 39. Its four own bodies are the
// ones dsd spelled _ZN10RickshawBs*.
static int __fastcall rp_init(void *s, void *)
{ return _ZN10RickshawBs13InitResourcesEv(s); }
static int __fastcall rp_clean(void *s, void *)
{ return _ZN10RickshawBs16CleanupResourcesEv(s); }
static int __fastcall rp_behavior(void *s, void *)
{ return func_ov002_020b6718(s); }
static int __fastcall rp_render(void *s, void *)
{ port_actor_render_probe("ROTATING_PLATFORM_BS", (char *)s + 0xd4);
  return func_ov002_020b66f0(s); }
static int __fastcall rp_d1(void *s, void *)
{ return (int)(size_t)_ZN10RickshawBsD1Ev((int *)s); }
static int __fastcall rp_d0(void *s, void *)
{ return (int)(size_t)_ZN10RickshawBsD0Ev((int *)s); }

extern "C" void hal_fill_rotating_platform_bs_vtable(void)
{
    port_ov47_bringup();
    void *volatile *vt = (void *volatile *)_ZTV10RickshawBs;
    ov47_fill_shared(vt);
    vt[0]  = (void *)rp_init;
    vt[3]  = (void *)rp_clean;
    vt[6]  = (void *)rp_behavior;
    vt[9]  = (void *)rp_render;
    vt[16] = (void *)rp_d1;
    vt[17] = (void *)rp_d0;
    vt[31] = (void *)ov47_kill;
}

// ============================================================================
// RICKSHAW_PLATFORM_BS (151) -- table 0x0211244c, RTTI 17daObjKm3_Kuruma_c,
// 32 slots
// ============================================================================
//
// The rickshaw's CART (kuruma). An 800-byte Platform, intermediate base ov002
// 0x02109278. NOT IN LEVEL 39's SKIP LIST and placed TWELVE times on it: each
// of the three RICKSHAW_BS axles builds four of these at run time, so the
// pre-spawn gate never saw the id while the parent was skipped.
static int __fastcall rk_init(void *s, void *)
{ return func_ov047_021114c0(s); }
static int __fastcall rk_clean(void *s, void *)
{ return func_ov047_021114ac(s); }
static int __fastcall rk_behavior(void *s, void *)
{ return func_ov002_020b6920(s); }
static int __fastcall rk_render(void *s, void *)
{ port_actor_render_probe("RICKSHAW_PLATFORM_BS", (char *)s + 0xd4);
  return func_ov002_020b68f8(s); }
static int __fastcall rk_d1(void *s, void *)
{ return (int)(size_t)func_ov047_021113f8((int *)s); }
static int __fastcall rk_d0(void *s, void *)
{ return (int)(size_t)func_ov047_02111448((int *)s); }

extern "C" void hal_fill_rickshaw_platform_bs_vtable(void)
{
    port_ov47_bringup();
    void *volatile *vt = (void *volatile *)data_ov047_0211244c;
    ov47_fill_shared(vt);
    vt[0]  = (void *)rk_init;
    vt[3]  = (void *)rk_clean;
    vt[6]  = (void *)rk_behavior;
    vt[9]  = (void *)rk_render;
    vt[16] = (void *)rk_d1;
    vt[17] = (void *)rk_d0;
    vt[31] = (void *)ov47_kill;
}

// ============================================================================
// STAIRS_BS (153) -- table 0x0211254c (dsd _ZTV18RickshawPlatformBs),
// RTTI 17daObjKm3_Dorifu_c, 32 slots
// ============================================================================
//
// The staircase that assembles itself out of the sky. A 3532-byte Platform: an
// ARRAY of five Models at +0x320 (stride 0x50) and five MovingMeshColliders at
// +0x4b0 (stride 0x1c8) on top of its own pair at +0xd4/+0x124, and a
// fifteen-word resource argument at 0x02112508 -- five {model file, clsn file,
// CLPS block} triples, one per stair. Intermediate base ov002 0x02108d94
// (daObjDorifu_c, already hosted by hal/actor_classes_ov036.cpp). One on level
// 39. Its four own bodies are the ones dsd spelled _ZN18RickshawPlatformBs*.
static int __fastcall sb_init(void *s, void *)
{ return _ZN18RickshawPlatformBs13InitResourcesEv(s); }
static int __fastcall sb_clean(void *s, void *)
{ return _ZN18RickshawPlatformBs16CleanupResourcesEv(s); }
static int __fastcall sb_behavior(void *s, void *)
{ return func_ov002_020b4bfc(s); }
static int __fastcall sb_render(void *s, void *)
{ port_actor_render_probe("STAIRS_BS", (char *)s + 0xd4);
  return func_ov002_020b4bc4(s); }
static int __fastcall sb_d1(void *s, void *)
{ return (int)(size_t)_ZN18RickshawPlatformBsD1Ev(s); }
static int __fastcall sb_d0(void *s, void *)
{ return (int)(size_t)_ZN18RickshawPlatformBsD0Ev(s); }

extern "C" void hal_fill_stairs_bs_vtable(void)
{
    port_ov47_bringup();
    void *volatile *vt = (void *volatile *)_ZTV18RickshawPlatformBs;
    ov47_fill_shared(vt);
    vt[0]  = (void *)sb_init;
    vt[3]  = (void *)sb_clean;
    vt[6]  = (void *)sb_behavior;
    vt[9]  = (void *)sb_render;
    vt[16] = (void *)sb_d1;
    vt[17] = (void *)sb_d0;
    vt[31] = (void *)ov47_kill;
}
