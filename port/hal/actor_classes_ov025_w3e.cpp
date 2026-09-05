// RUN rel0215 WAVE 3 (lane w3-e): ov025's OTHER TWO IDS -- GRINDEL (162) and
// SPINDEL (163), level 17, Shifting Sand Land's pyramid interior. Wave 15
// seated PYRAMID_STEP (87) and PYRAMID_LIFT (88) in
// hal/actor_classes_ov025.cpp; these two close the overlay and take level 17's
// skip list down to the one id nobody in ov025 owns.
//
// Read hal/actor_classes_ov025.cpp first for the mount, the bring-up and the
// two widths it derived. What follows is only what is new.
//
// ---- THE TWO IDS ----------------------------------------------------------
//
//   id   SpawnInfo   spawnFunc                   table       RTTI at [-1]
//   162  0x0211382c  0x02111cf4 Grindel_Spawn    0x02113850  7daDkk_c
//   163  0x0211373c  0x02111898 func_ov025_...   0x02113760  7daDgr_c
//
// Both records pass BOTH attribution routes against overlay_0025.bin. 163 is
// the one worth stating twice, because symbols/actor_renames_report.txt
// resolves it the other way ("AMBIG actor 163 SPINDEL: ['ov021','ov025'] ->
// ov021"): at 0x0211373c ov021's image holds w0=0x021136c8, which IS inside
// ov021 and so passes route 1, but its w1 low halfword is 0. ov025's holds
// w0=0x02111898 inside ov025 AND w1 low halfword 163. That is exactly the case
// requiring both routes was written for. 162 is confirmed a third way: the
// report's own alloc=0x3a0 is the 928 bytes Grindel_Spawn asks for.
//
// ---- THE WIDTHS: BOTH 32, PINNED THE WAY THE wf SLOT-31 BUG DEMANDS -------
//
// Read out of extracted/overlays/overlay_0025.bin. Both tables end
//   slot 29  0x02010124 Actor::OnAimedAtWithEgg   (162 OVERRIDES this one)
//   slot 30  0x020100dc Actor::OnAimedAtWithEggReturnVec
//   slot 31  0x020ee55c Platform::Kill
// and each is followed immediately by the NEXT class's Itanium typeinfo
// record (0x021137e0 after 163's, 0x021138d0 after 162's), which is where a
// reloc-run read would over-read to. Both are Platform-derived, so 32 is also
// what the slot-31 Kill says.
//
//   162 own slots  0 02111c24  3 ov091  6 02111b64  9 ov091
//                 16 021118c8 17 02111928  29 0211199c  31 Platform::Kill
//   163 own slots  0 021117dc  3 02111384  6 021113f0  9 021113c8
//                 16 021111a0 17 021111e4  31 Platform::Kill
//
// GRINDEL'S SLOTS 3 AND 9 ARE THWOMP'S. 162 derives from daDsnBase_c, ov091's
// Thwomp base, so slot 3 is 0x021331b8 (_ZN6Thwomp16CleanupResourcesEv) and
// slot 9 is 0x02133210 (_ZN6Thwomp6RenderEv), and its Behavior and
// InitResources call eight more ov091 bodies. All ten were already in
// walk_window.map on this lane's baseline -- checked, so nothing ov091 is
// added here.
//
// SLOT 29 IS A REAL OVERRIDE and it is read off the table, not off the name:
// 0x021138c4 holds 0x0211199c, an eight-byte body returning 0xce000, where
// every other table in this overlay holds arm9's 0x02010124.
//
// ---- GRINDEL'S FACTORY WRITES ITS TABLE THROUGH `VT1` ---------------------
//
// Grindel_Spawn is the FloatingFloor shape, and the ROM says so:
//   02111d0c  LDR r1, =0x021351fc   (pool 0x02111d38)  the ov091 BASE table
//   02111d14  STR r1, [r4]
//   02111d18  BL  TextureSequence::TextureSequence   (+0x324)
//   02111d20  BL  ShadowModel::ShadowModel           (+0x338)
//   02111d24  LDR r0, =0x02113850   (pool 0x02111d3c)  its OWN table
//   02111d28  STR r0, [r4]
// so the recovered source's `p[0] = _ZTV7daDkk_c` is the BASE store and its
// `p[0] = VT1` is the OWN one. The two member constructors between the stores
// build subobjects at +0x324 and +0x338 and dispatch nothing through p[0], so
// the first store is dead. VT1 is bound per source to the host table
// (port/CMakeLists.txt) and the factory registers directly. 163's Spawn writes
// data_ov025_02113760 by name and needs nothing.
//
// ---- LANE OWNERSHIP -------------------------------------------------------
//
// This file calls wave 15's port_ov25_bringup() (mount pack-check, syms patch,
// hal_fill_platform_vtable and that lane's two sinits) and then runs the two
// sinits THESE classes need, under its own DSSTATE-bracketed guard, for the
// reason that file records: everything the pass writes lives in .dsstate and a
// restore rolls it back, so the guard has to roll back with it.

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
#include "dsstate_seg.h"

#include "Actor.h"
#include "ActorBase.h"

extern "C" {
/* the arm9 shared half, re-read off this lane's own two reloc runs */
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

const char *port_actor_class_name(unsigned id);
void port_actor_slot_decline(const char *what);
void port_actor_render_probe(const char *cls, void *model);

/* wave 15's bring-up: pack check, syms patch, hal_fill_platform_vtable and
   ids 87/88's two sinits */
void port_ov25_bringup(void);
/* the two sinits THIS lane's classes need */
void __sinit_ov025_02112970(void);   /* 163's model/collision pair */
void __sinit_ov025_021129dc(void);   /* 162's                      */

/* SPINDEL (163) */
int func_ov025_021117dc(char *self);   /* slot 0  InitResources */
int func_ov025_02111384(char *self);   /* slot 3  CleanupResources */
int func_ov025_021113f0(void *self);   /* slot 6  Behavior */
int func_ov025_021113c8(void *self);   /* slot 9  Render */
int *func_ov025_021111a0(int *self);   /* slot 16 D1 */
int *func_ov025_021111e4(int *self);   /* slot 17 D0 */
void *func_ov025_02111898(void);       /* the factory */

/* GRINDEL (162) */
int func_ov025_02111c24(char *self);   /* slot 0  InitResources */
int _ZN6Thwomp16CleanupResourcesEv(void *self);  /* slot 3, ov091 */
int func_ov025_02111b64(void *self);   /* slot 6  Behavior */
int _ZN6Thwomp6RenderEv(void *self);   /* slot 9, ov091 */
int *func_ov025_021118c8(int *self);   /* slot 16 D1 */
int *func_ov025_02111928(int *self);   /* slot 17 D0 */
int func_ov025_0211199c(void);         /* slot 29 OnAimedAtWithEgg override */
void *Grindel_Spawn(void);

DSSTATE_BEGIN
void *data_ov025_02113760[32];   /* 163 */
void *data_ov025_02113850[32];   /* 162 */
DSSTATE_END
}

/* Each table's RTTI spelling, which its D1 restores on the way out. Both
   tables are excluded from the mount, so both LHS are undefined everywhere and
   neither alias can be defeated. `_ZTV7daDkk_c` is ALSO spelled by
   Grindel_Spawn, where it means the ov091 base 0x021351fc rather than this
   table -- that store is the dead first half of the two-store pair the header
   disassembles, so pointing the name at 162's own table costs nothing and the
   live store is the per-source -DVT1. */
#pragma comment(linker, "/alternatename:__ZTV7daDgr_c=_data_ov025_02113760")
#pragma comment(linker, "/alternatename:__ZTV7daDkk_c=_data_ov025_02113850")

/* THREE C++-MANGLED DATA SPELLINGS, the four wave 15 already bridges for
   PyramidLift's InitResources. src/func_ov025_021117dc.cpp declares its two
   SharedFilePtrs and its CLPS_Block with their struct types at file scope, so
   MSVC decorates all three while the mount emits one C-named array each. Data
   has no calling convention, so the alias onto the one object is exact. All
   three LHS are declared and never defined, so alternatename_guard stays
   clean. Taken off the first link's own unresolved list, not guessed. */
#pragma comment(linker, "/alternatename:?data_ov025_02113a68@@3USharedFilePtr@@A=_data_ov025_02113a68")
#pragma comment(linker, "/alternatename:?data_ov025_02113a60@@3USharedFilePtr@@A=_data_ov025_02113a60")
#pragma comment(linker, "/alternatename:?data_ov025_02112c28@@3UCLPS_Block@@A=_data_ov025_02112c28")

/* And the FOURTH, from the reclaimed PYRAMID_STEP slot 3 (W9 in
   port/CMakeLists.txt): that body declares G0 as `extern int G0[]` at C++ file
   scope, so the -D renames it into a C++-decorated `int *`. Its G1 comes from
   decl_common.h inside that header's extern "C" and lands on the plain C name
   with no bridge. */
#pragma comment(linker, "/alternatename:?data_ov025_02113ab8@@3PAHA=_data_ov025_02113ab8")

// ---- the trap --------------------------------------------------------------
static void ov25e_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: ov025 vtable slot %d is not hosted (actor id %u "
                 "%s)\n", slot, id, port_actor_class_name(id));
    { static char _m[128];
      std::snprintf(_m, sizeof _m, "unhosted ov025 vtable slot %d on id %u %s",
                    slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
#define OV25E_TRAP(n) \
    static int __fastcall ov25e_trap##n(void *s, void *) \
    { ov25e_trap_report(s, n); return 0; }
OV25E_TRAP(13) OV25E_TRAP(14)
#undef OV25E_TRAP

static int __fastcall ov25e_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall ov25e_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall ov25e_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall ov25e_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall ov25e_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall ov25e_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall ov25e_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall ov25e_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall ov25e_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall ov25e_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall ov25e_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
static int __fastcall ov25e_turn_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall ov25e_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall ov25e_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall ov25e_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall ov25e_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall ov25e_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall ov25e_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall ov25e_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall ov25e_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall ov25e_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall ov25e_egg(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }
static int __fastcall ov25e_kill(void *s, void *)
{ _ZN8Platform4KillEv(s); return 0; }

/* Slots 1..30. The caller writes its own 0/3/6/9/16/17 and 31, and 162 also
   writes its 29. THE POINTER IS VOLATILE ON PURPOSE -- the gate-200
   elided-stores bug, repro in hal/actor_classes_ov002g200.cpp. */
static void ov25e_fill_shared(void *volatile *vt)
{
    vt[1]  = (void *)ov25e_binit;
    vt[2]  = (void *)ov25e_ainit;
    vt[4]  = (void *)ov25e_bclean;
    vt[5]  = (void *)ov25e_aclean;
    vt[7]  = (void *)ov25e_bbeh;
    vt[8]  = (void *)ov25e_abeh;
    vt[10] = (void *)ov25e_bren;
    vt[11] = (void *)ov25e_aren;
    vt[12] = (void *)ov25e_pdes;
    vt[13] = (void *)ov25e_trap13;
    vt[14] = (void *)ov25e_trap14;
    vt[15] = (void *)ov25e_heap;
    vt[18] = (void *)ov25e_yoshi;
    vt[19] = (void *)ov25e_turn_egg;
    vt[20] = (void *)ov25e_v50;
    vt[21] = (void *)ov25e_pounded;
    vt[22] = (void *)ov25e_atk1;
    vt[23] = (void *)ov25e_atk2;
    vt[24] = (void *)ov25e_kicked;
    vt[25] = (void *)ov25e_pushed;
    vt[26] = (void *)ov25e_cannon;
    vt[27] = (void *)ov25e_mega;
    vt[28] = (void *)ov25e_under;
    vt[29] = (void *)ov25e_egg;
    vt[30] = (void *)port_actor_s30_base;
}

// ---- the bring-up ----------------------------------------------------------
DSSTATE_BEGIN
static int g_ov25e_bringup_done;
DSSTATE_END

static void ov25e_bringup(void)
{
    port_ov25_bringup();          /* wave 15's */
    if (g_ov25e_bringup_done)
        return;
    g_ov25e_bringup_done = 1;
    __sinit_ov025_02112970();     /* 163 */
    __sinit_ov025_021129dc();     /* 162 */
}

// ============================================================================
// SPINDEL (163) -- table 0x02113760, RTTI 7daDgr_c
// ============================================================================
//
// 820-byte object built by Platform's constructor: Model at +0xd4,
// MovingMeshCollider at +0x124, and a Matrix4x3 at +0x2ec its InitResources
// hands MovingMeshCollider::SetFile along with the CLPS block at 0x02112c28.
// Files 730 (model) and 731 (collision), built by __sinit_ov025_02112970.
// Slot 9 is the plain-Model slot-5 dispatch (a six-virtual shadow calling
// m(0)); its D1 runs Model::~Model at +0xd4, so it is a Model and not a
// ModelAnim, and hal/cxxname_bridge.cpp's dual fill of _ZTV5Model[5] makes
// index 5 Render on the host -- the same ruling slice_ov030cast.txt records
// for UkikiCage and RollingLogTtm.
static int __fastcall sp_init(void *s, void *)
{ return func_ov025_021117dc((char *)s); }
static int __fastcall sp_clean(void *s, void *)
{ return func_ov025_02111384((char *)s); }
static int __fastcall sp_behavior(void *s, void *)
{ return func_ov025_021113f0(s); }
static int __fastcall sp_render(void *s, void *)
{ port_actor_render_probe("SPINDEL", (char *)s + 0xd4);
  return func_ov025_021113c8(s); }
static int __fastcall sp_d1(void *s, void *)
{ return (int)(size_t)func_ov025_021111a0((int *)s); }
static int __fastcall sp_d0(void *s, void *)
{ return (int)(size_t)func_ov025_021111e4((int *)s); }
extern "C" void hal_fill_spindel_vtable(void)
{
    ov25e_bringup();
    void *volatile *vt = (void *volatile *)data_ov025_02113760;
    ov25e_fill_shared(vt);
    vt[0]  = (void *)sp_init;
    vt[3]  = (void *)sp_clean;
    vt[6]  = (void *)sp_behavior;
    vt[9]  = (void *)sp_render;
    vt[16] = (void *)sp_d1;
    vt[17] = (void *)sp_d0;
    vt[31] = (void *)ov25e_kill;
}

// ============================================================================
// GRINDEL (162) -- table 0x02113850, RTTI 7daDkk_c over 11daDsnBase_c
// ============================================================================
//
// 928-byte object: Model at +0xd4, MovingMeshCollider at +0x124, a
// TextureSequence at +0x324 and a ShadowModel at +0x338, with the Thwomp base
// state at +0x398. Its InitResources seats the three-word resource table
// data_ov025_02113814 = { 0x02113a88 model SFP, 0x02113a90 collision SFP,
// 0x02112d28 CLPS } at +0x320 and then runs ov091's own init tail, and drops a
// RaycastLine to find the floor it slams onto. Files 0x2dc and 0x2dd, built by
// __sinit_ov025_021129dc.
static int __fastcall gr_init(void *s, void *)
{ return func_ov025_02111c24((char *)s); }
static int __fastcall gr_clean(void *s, void *)
{ return _ZN6Thwomp16CleanupResourcesEv(s); }
static int __fastcall gr_behavior(void *s, void *)
{ return func_ov025_02111b64(s); }
static int __fastcall gr_render(void *s, void *)
{ port_actor_render_probe("GRINDEL", (char *)s + 0xd4);
  return _ZN6Thwomp6RenderEv(s); }
static int __fastcall gr_d1(void *s, void *)
{ return (int)(size_t)func_ov025_021118c8((int *)s); }
static int __fastcall gr_d0(void *s, void *)
{ return (int)(size_t)func_ov025_02111928((int *)s); }
/* slot 29, the one override: an eight-byte body that takes no receiver on the
   ROM either (it is `mov r0, #0xce000; bx lr`), so the thunk drops `this`. */
static int __fastcall gr_egg(void *s, void *)
{ (void)s; return func_ov025_0211199c(); }
extern "C" void hal_fill_grindel_vtable(void)
{
    ov25e_bringup();
    void *volatile *vt = (void *volatile *)data_ov025_02113850;
    ov25e_fill_shared(vt);
    vt[0]  = (void *)gr_init;
    vt[3]  = (void *)gr_clean;
    vt[6]  = (void *)gr_behavior;
    vt[9]  = (void *)gr_render;
    vt[16] = (void *)gr_d1;
    vt[17] = (void *)gr_d0;
    vt[29] = (void *)gr_egg;
    vt[31] = (void *)ov25e_kill;
}
