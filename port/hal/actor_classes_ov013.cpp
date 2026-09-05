// GATE 202: CLOCK_HAND_SHORT (292) + CLOCK_HAND_LONG (293) + CLOCK_PENDULUM
// (294), ov013 (level 5, castle second floor -- the clock-painting
// furniture). Per-symbol ov013 mount (port/ov013_syms.txt). Full slot
// derivation, the mandatory window-alias check and the closure verdict
// (castle2-scope's "generic stubs" warning is stale, every own slot is
// matched src) are in port/ov013_syms.txt's own header.
//
// ALL TABLES ARE 31-SLOT ACTOR, the ccm/IceSlideManager/gate-200 base
// family (every base-slot address verified against arm9 symbols.txt:
// slot 12 = 0x02043ac0 ActorBase::OnPendingDestroy, 13/14 Virtual34/38
// traps, 15 OnHeapCreated, 30 OnAimedAtWithEggReturnVec trap). Own slots,
// read out of the tables' relocations -- the SAME set for all three
// classes: 0 Init, 3 Cleanup, 6 Behavior, 9 Render, 16 D1, 17 D0.
//
// TWO TABLES, THREE IDS: both hands share _ZTV22ClockPaintingHandShort
// (== RTTI _ZTV12daObjClock_c, 0x02112200; HandLong_Spawn installs the
// SAME table, reloc-verified). The pendulum's own table is the unnamed
// data_ov013_02112128 (RTTI _ZTV18daObjClockHuriko_c), split five ways by
// dsd -- ONE host array here spans all 31 slots.
//
// D1/D0 for BOTH tables are host thunks (the matched TUs store the own
// table then run the member chain; Pendulum's D0 needs the "VT0"/"G0"
// resolver names, HandShort's D1 is a real MSVC dtor over local structs --
// the gate-199/200 treatment). Chain read from the two matched D0 bodies:
// store own table, Model +0xd4, Actor::D2, D0 frees on the game heap.
//
// Renders ride FROM SRC: plain-Model slot-5 bare-call shadows over +0xd4.
// _ZTV5Model[5] is dual-filled (hal/cxxname_bridge.cpp), the Tree/ov072/l7
// precedent -- NOT the ModelAnim slot-5 collision.
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
#include "dtor_faces_cpp.h"
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
void port_actor_slot_decline(const char *what);   /* func_02043fdc_hostcopy.cpp */
void port_actor_render_probe(const char *cls, void *model); /* hal/actor_classes */

/* CLOCK_PENDULUM (daObjClockHuriko_c) own bodies -- unnamed by dsd,
   matched src, all extern-C */
int func_ov013_0211133c(char *self);       /* slot 0, InitResources */
int func_ov013_02111214(void);             /* slot 3, CleanupResources
                                               (releases the global SFP,
                                               ignores this) */
int func_ov013_021112a8(char *self);       /* slot 6, Behavior (the swing) */
int func_ov013_02111280(void *self);       /* slot 9, Render (from src,
                                               plain-Model slot-5) */
void *ClockPaintingPendulum_Spawn(void);   /* installs data_ov013_02112128 */
DSSTATE_BEGIN
int data_ov013_02112128[31];               /* Pendulum's own vtable, unnamed
                                               and five-way split by dsd --
                                               host array spanning
                                               0x02112128..0x021121a4 */
DSSTATE_END

/* CLOCK_HAND_SHORT/LONG shared own bodies -- real C++ methods against
   ClockPaintingHandShort.h, faced below (Render too: it compiles from src
   as a real method, the Tree recipe) */
int _ZN22ClockPaintingHandShort13InitResourcesEv(void *self);    /* slot 0 */
int *_ZN22ClockPaintingHandShortD0Ev(int *self);                 /* slot 17, .c, DTOR-PAIRS seat (0x021113ec) */
int _ZN22ClockPaintingHandShort16CleanupResourcesEv(void *self); /* slot 3 */
int _ZN22ClockPaintingHandShort8BehaviorEv(void *self);          /* slot 6 */
int _ZN22ClockPaintingHandShort6RenderEv(void *self);            /* slot 9 */
void *ClockPaintingHandShort_Spawn(void);
void *ClockPaintingHandLong_Spawn(void);   /* installs the SAME table */
int _ZTV22ClockPaintingHandShort[31];      /* the shared hand vtable,
                                               0x02112200..0x0211227c (dsd
                                               split the 22..30 tail off as
                                               data_ov013_02112258 -- both
                                               left out of the mount) */

/* D1/D0 teardown seams (the chain both matched D0 bodies spell: Model at
   +0xd4, then Actor's own D2; D0 also frees on the game heap) */
void _ZN5ModelD1Ev(void *self);
void *_ZN5ActorD2Ev(void *self);
void _ZN6Memory10DeallocateEPvP4Heap(void *, void *);
extern void *data_020a0eac;   /* the game heap (== the matched D0s' G0) */
}
/* RTTI names the recovered source stores through -- point them at the two
   host arrays */
#pragma comment(linker, "/alternatename:__ZTV18daObjClockHuriko_c=_data_ov013_02112128")
#pragma comment(linker, "/alternatename:__ZTV12daObjClock_c=__ZTV22ClockPaintingHandShort")
/* window-alias names inside the recovered source (other overlays' dsd
   exports won the naming race at these window addresses; all three are
   ov013's own symbols -- see ov013_syms.txt's header) */
#pragma comment(linker, "/alternatename:_data_ov051_021116b0=_data_ov013_021116b0")
#pragma comment(linker, "/alternatename:_data_ov054_02111238=_func_ov013_02111238")
/* The third of the three, ___sinit_ov045_02112280, MOVED to
   hal/cxx_aliases.cpp (lane w8-shadows). It is the only one of them whose name
   is also a matched TU in src/, so it shows up in linkage.py's replacement
   queue, and that tool reads the ruling for a generated ovNNN_syms.c.obj out
   of cxx_aliases.cpp -- the alias registry -- and nowhere else. The pragma had
   to travel with its PORT_HOST_ABI tag for the ruling to be visible. Same
   three targets compile both files (smoke_player, walk_window,
   walk_window_hires), so the alias reaches exactly the links it did before. */
/* HandShort's Behavior TU declares these two hosted globals OUTSIDE its
   extern "C" block, so they mangle as C++ arrays -- the same globals
   cxx_aliases.cpp already carries under other TUs' spellings */
#pragma comment(linker, "/alternatename:?data_02092110@@3PACA=_data_02092110")
#pragma comment(linker, "/alternatename:?data_0209f2c0@@3PAEA=_data_0209f2c0")

// ---- the shared 1..30 half, all three ids share it -----------------------
static void ov13_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: ov013 vtable slot %d is not hosted (actor id %u "
                 "%s)\n", slot, id, port_actor_class_name(id));
    { static char _m[128];
      std::snprintf(_m, sizeof _m, "unhosted ov013 vtable slot %d on id %u %s",
                    slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
static int __fastcall ov13_trap13(void *s, void *) { ov13_trap_report(s, 13); return 0; }
static int __fastcall ov13_trap14(void *s, void *) { ov13_trap_report(s, 14); return 0; }

static int __fastcall ov13_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall ov13_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall ov13_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall ov13_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall ov13_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall ov13_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall ov13_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall ov13_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall ov13_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall ov13_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall ov13_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
static int __fastcall ov13_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall ov13_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall ov13_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall ov13_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall ov13_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall ov13_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall ov13_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall ov13_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall ov13_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall ov13_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall ov13_aimed(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }

/* Fills slots 1,2,4,5,7,8,10,11,12,13,14,15,18..30 -- the standard 31-slot
   Actor shared half. The callers overwrite their own 0/3/6/9/16/17.

   THE POINTER IS VOLATILE ON PURPOSE: MSVC 19.44 x86 /O2 deletes a static
   filler's calls from BOTH call sites when it is called with two distinct
   extern-array arguments, dropping every shared-half store from the object
   file -- the gate-200 elided-stores bug, repro and dumpbin proof in
   hal/actor_classes_ov002g200.cpp. This helper has exactly two callers. */
static void ov13_fill_shared(void *volatile *vt)
{
    vt[1]  = (void *)ov13_binit;
    vt[2]  = (void *)ov13_ainit;
    vt[4]  = (void *)ov13_bclean;
    vt[5]  = (void *)ov13_aclean;
    vt[7]  = (void *)ov13_bbeh;
    vt[8]  = (void *)ov13_abeh;
    vt[10] = (void *)ov13_bren;
    vt[11] = (void *)ov13_aren;
    vt[12] = (void *)ov13_pdes;
    vt[13] = (void *)ov13_trap13;
    vt[14] = (void *)ov13_trap14;
    vt[15] = (void *)ov13_heap;
    vt[18] = (void *)ov13_yoshi;
    vt[19] = (void *)ov13_egg;
    vt[20] = (void *)ov13_v50;
    vt[21] = (void *)ov13_pounded;
    vt[22] = (void *)ov13_atk1;
    vt[23] = (void *)ov13_atk2;
    vt[24] = (void *)ov13_kicked;
    vt[25] = (void *)ov13_pushed;
    vt[26] = (void *)ov13_cannon;
    vt[27] = (void *)ov13_mega;
    vt[28] = (void *)ov13_under;
    vt[29] = (void *)ov13_aimed;
    vt[30] = (void *)port_actor_s30_base;
}

// ============================================================================
// CLOCK_PENDULUM (294), 31 slots.
// ============================================================================
static int __fastcall cp_init(void *s, void *)
{ return func_ov013_0211133c((char *)s); }
static int __fastcall cp_clean(void *s, void *)
{ (void)s; return func_ov013_02111214(); }
static int __fastcall cp_behavior(void *s, void *)
{ return func_ov013_021112a8((char *)s); }
static int __fastcall cp_render(void *s, void *)
{ port_actor_render_probe("CLOCK_PENDULUM", (char *)s + 0xd4);
  return func_ov013_02111280(s); }
/* D1/D0 host thunks: the matched src (func_ov013_021111a0.c/021111d0.c --
   dsd's recovered-name comment calls the D0 "OnYoshiTryEat", the slot-17
   reloc and the body say otherwise) stores the own table then runs Model
   +0xd4 / Actor::D2; D0 frees on the game heap. */
/* SLOT 16 IS THE MATCHED TU NOW, run link100 lane TAIL, and this one is the
   cheapest seat in the campaign: the host thunk it replaces was a line-for-line
   transcription of src/func_ov013_021111a0.c that diverged from it in NOTHING.
   The ROM body's only pooled word is data_ov013_02112128 (reloc 0x021111cc),
   which it spells by that real name, so there was no placeholder to rename and
   no store to reorder -- the same three statements, reached by their own symbol
   instead of copied. port/tools/tail_slots.py --module ov013 --vtable
   0x02112128 --width 32 reads slot 16 -> 0x021111a0.
   SLOT 17 KEEPS ITS THUNK: the matched D0 (func_ov013_021111d0) carries the
   inferred-stub marker, and port/tools/inferred_stub_guard.py refuses new
   seats of those. */
extern "C" int *func_ov013_021111a0(int *t);   /* ov013 0x021111a0, slot 16 */
static int __fastcall cp_d1(void *s, void *)
{ return (int)(size_t)func_ov013_021111a0((int *)s); }
static int __fastcall cp_d0(void *s, void *)
{
    char *t = (char *)s;
    *(void **)t = (void *)data_ov013_02112128;
    _ZN5ModelD1Ev(t + 0xd4);
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return (int)(size_t)s;
}

extern "C" void hal_fill_clock_pendulum_vtable(void)
{
    void *volatile *vt = (void *volatile *)data_ov013_02112128;
    ov13_fill_shared(vt);
    vt[0]  = (void *)cp_init;
    vt[3]  = (void *)cp_clean;
    vt[6]  = (void *)cp_behavior;
    vt[9]  = (void *)cp_render;
    vt[16] = (void *)cp_d1;
    vt[17] = (void *)cp_d0;
}

// ============================================================================
// CLOCK_HAND_SHORT (292) + CLOCK_HAND_LONG (293), ONE shared 31-slot table.
// InitResources reads the actor id itself (0x125 -> hand index 0, the long
// hand; anything else -> 1) so one fill serves both registry rows.
// ============================================================================
static int __fastcall ch_init(void *s, void *)
{ return _ZN22ClockPaintingHandShort13InitResourcesEv(s); }
static int __fastcall ch_clean(void *s, void *)
{ return _ZN22ClockPaintingHandShort16CleanupResourcesEv(s); }
static int __fastcall ch_behavior(void *s, void *)
{ return _ZN22ClockPaintingHandShort8BehaviorEv(s); }
static int __fastcall ch_render(void *s, void *)
{ port_actor_render_probe("CLOCK_HAND", (char *)s + 0xd4);
  return _ZN22ClockPaintingHandShort6RenderEv(s); }
/* D1 host thunk (matched src is a real MSVC destructor over local struct
   decls, unlinkable against the host seams -- lane DTOR-PAIRS-CPP's row). */
/* slot 16 is the matched src D1 through hal/dtor_faces_cpp.cpp (lane DTOR-FACES-CPP);
   the transcribed thunk that stood here (ch_d1) spelled the same chain by hand. */
/* D0 (DTOR-PAIRS seat): the matched flat-C body behind the ecx->arg adapter;
   it stores _ZTV12daObjClock_c, this table by its RTTI name (aliased above),
   and frees on data_020a0eac by name. */
static int __fastcall ch_d0(void *s, void *)
{ return (int)(size_t)_ZN22ClockPaintingHandShortD0Ev((int *)s); }

extern "C" void hal_fill_clock_hand_vtable(void)
{
    void *volatile *vt = (void *volatile *)_ZTV22ClockPaintingHandShort;
    ov13_fill_shared(vt);
    vt[0]  = (void *)ch_init;
    vt[3]  = (void *)ch_clean;
    vt[6]  = (void *)ch_behavior;
    vt[9]  = (void *)ch_render;
    vt[16] = (void *)hal_cppd1_ClockPaintingHandShort;
    vt[17] = (void *)ch_d0;
}

// ---- method faces --------------------------------------------------------
// All four ClockPaintingHandShort methods are real MSVC methods against
// ClockPaintingHandShort.h (?InitResources@ClockPaintingHandShort@@..., not
// _ZN22ClockPaintingHandShort...) -- faced here, the IceSheet/SwitchPillar
// recipe. Render IS faced (it compiles from src, the Tree recipe -- not a
// host copy).
#include "ClockPaintingHandShort.h"
extern "C" {
int _ZN22ClockPaintingHandShort13InitResourcesEv(void *self)
{ return ((ClockPaintingHandShort *)self)->ClockPaintingHandShort::InitResources(); }
int _ZN22ClockPaintingHandShort16CleanupResourcesEv(void *self)
{ return ((ClockPaintingHandShort *)self)->ClockPaintingHandShort::CleanupResources(); }
int _ZN22ClockPaintingHandShort8BehaviorEv(void *self)
{ return ((ClockPaintingHandShort *)self)->ClockPaintingHandShort::Behavior(); }
int _ZN22ClockPaintingHandShort6RenderEv(void *self)
{ return ((ClockPaintingHandShort *)self)->ClockPaintingHandShort::Render(); }
}
