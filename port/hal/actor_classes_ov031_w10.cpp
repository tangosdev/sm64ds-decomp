// ===========================================================================
// RUN LINKW WAVE 10, LANE w10-shared: ov031's SLIDE DECORATION QUAD.
//
// FOUR actor ids, ONE ROM class, ONE fill, TWELVE instances on level 23.
//
//   302 SLIDE_DECORATION_SILVER_STAR    x3
//   303 SLIDE_DECORATION_YELLOW_STAR    x3
//   304 SLIDE_DECORATION_ORANGE_SMILEY  x3
//   305 SLIDE_DECORATION_BLUE_SMILEY    x3
//
// ---- WHY ONE FILL SERVES FOUR IDS -----------------------------------------
//
// Resolving every one of the 512 spawn-table entries to the table its factory
// stores puts all four of these ids on ONE table, 0x02111984, and the class
// reads its variant out of its OWN actor id rather than out of four classes:
// src/_ZN25SlideDecorationSilverStar13InitResourcesEv.cpp switches on unk_00c
// (the id halfword ActorBase stamps at +0xc) over 0x12e/0x12f/0x130/0x131 =
// 302/303/304/305, writes 0..3 into mVariant, and indexes the four-entry
// SharedFilePtr table data_ov031_02111424 with it. CleanupResources releases
// through the same index. The four factories are byte-identical apart from
// which record they are reached through. So the fill below is written once and
// all four registry rows name it (it is idempotent, so registration order does
// not matter).
//
// port/slice_w10a.txt carries both attribution routes for each of the four,
// the width derivation and the RTTI read; port/ov031_syms.txt carries the
// window-alias check and the mount's size pins. What follows is only what the
// fill itself has to get right.
//
// ---- THE TABLE: 31 SLOTS, TWO NAMES, ONE ADDRESS --------------------------
//
// 0x02111984 is spelled BOTH _ZTV18daObjHsBillboard_c (the RTTI name, which is
// what src/_ZN25SlideDecorationSilverStarD0Ev.c stores) and
// _ZTV25SlideDecorationSilverStar (which is what all four factories store).
// config/arm9/overlays/ov031/symbols.txt carries both labels at that one
// address. The host array is defined under the RTTI name and the factories'
// name is an /alternatename onto it -- the ov022/actor_classes.cpp shape. The
// alias LHS is never defined anywhere else in this link (the four Spawns only
// declare it extern), so port/tools/alternatename_guard.py's rule holds.
//
// Width 31, pinned twice: the `load` reloc run at 0x02111984 is 31 entries and
// ends at 0x02111a00, which is also the end of ov031's .text (base 0x021111a0
// + code_size 0x860). A 32nd slot does not fit in the overlay. Slot 30 is
// Actor::OnAimedAtWithEggReturnVec and there is no slot-31 Platform::Kill --
// the plain-Actor shape, which agrees with the width.
//
// Own overrides, from the reloc run: 0 InitResources, 3 CleanupResources,
// 9 Render, 16 D1, 17 D0. Slot 6 is _ZN9ActorBase8BehaviorEv (arm9 0x02043b24)
// and slot 12 is _ZN9ActorBase16OnPendingDestroyEv (0x02043ac0) -- this class
// has NO Behavior of its own, it is scenery.
//
// ---- SLOT 16 IS SPELLED HERE, AND WHY -------------------------------------
//
// src/_ZN25SlideDecorationSilverStarD1Ev.cpp is the shadow-class MSVC
// destructor shape: the TU declares its own `struct Actor` / `struct Model` /
// `struct SlideDecorationSilverStar : Actor` and defines the destructor, so
// MSVC emits ??1SlideDecorationSilverStar@@UAE@XZ whose body calls
// ??1Model@@QAE@XZ and ??1Actor@@UAE@XZ -- two decorated C++ names that exist
// nowhere in this link, because the port carries those bodies under the ROM's
// C names. Linking that TU is an unresolved external, so slot 16 is spelled
// inline from the ROM D1's own three relocs inside 0x021111a0:
//     0x021111cc load  -> 0x02111984            store the table
//     0x021111b4 call  -> arm9 0x02016d20       _ZN5ModelD1Ev on this+0xd4
//     0x021111bc call  -> arm9 0x020112c8       _ZN5ActorD2Ev
// The D0 is NOT hosted: it is a .c TU carrying the ROM's Itanium name in
// extern "C", it is in the slice, and it already does that chain plus
// Memory::Deallocate (0x021111fc -> 0x0203c1e8) on the game heap.
//
// ---- SLOTS 13/14/30 TRAP ---------------------------------------------------
//
// The ccm/ov064/ov072/jrb/bbh convention. 13/14 are ActorBase::Virtual34/
// Virtual38, two u32 arguments each, bodies not linked into the port at all;
// 30 is Actor::OnAimedAtWithEggReturnVec, an SRET method no __fastcall thunk
// shape here models. Trap by name rather than seat a body with the wrong
// contract.
//
// ---- LANE OWNERSHIP: WHY THE MOUNT BRING-UP IS IN THIS FILE ---------------
//
// The ov045/ov022 arrangement, for the same reason and with the same handoff.
// Every dual-mounted overlay should call port_ovNNN_pack_check /
// port_ovNNN_syms_patch and its sinits from port_actor_overlays_sinits() in
// hal/actor_overlays.cpp; no lane owns that file in this wave, so the bring-up
// rides the registry fill behind a done-guard. The ordering that makes it safe
// is the one lane w4-b measured: port_actor_overlays_sinits() completes before
// port_actor_registry_install(), the registry writes SpawnInfo+0 = host factory
// and THEN calls fill, the generated port_ov031_syms_patch() only rebases
// pointers that land inside its own emission (the +0 word holds a CODE address,
// outside the mount's coverage, so it cannot undo the factory word), and the
// SharedFilePtrs the sinit builds are read by InitResources, which runs at
// spawn -- after registration. Whoever next owns hal/actor_overlays.cpp should
// move the body of ov31_bringup beside the ov013 block and cut the guard here
// to a call.
//
// ---- THE SINIT LINKS, UNLIKE ov045's ONE ----------------------------------
//
// __sinit_ov031_02111434 is in the slice. Its name is not contested: the only
// other src TU whose text contains that string is
// src/_ZN13RacingPenguin8BehaviorEv.cpp, and what that one spells is
// `p__sinit_ov031_02111434` -- dsd's label for the .ctor POINTER at 0x021114ec,
// a different address and a different symbol, and that TU is in no slice. No
// /alternatename anywhere in port/ has this name as its LHS.
// ===========================================================================
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
#include "SlideDecorationSilverStar.h"

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

/* what the hosted slot 16 has to spell out by hand */
void _ZN5ModelD1Ev(void *self);
void *_ZN5ActorD2Ev(void *self);

const char *port_actor_class_name(unsigned id);   /* hal/actor_registry */
void port_actor_slot_decline(const char *what);   /* func_02043fdc_hostcopy.cpp */

/* the generated ov031 per-symbol mount (build/port/host-src/ov031_syms.c) */
void port_ov031_pack_check(void);
void port_ov031_syms_patch(void);

/* the class's own bodies, all matched src in port/slice_w10a.txt.
   Slot 0/3/9 are real C++ METHODS (MSVC decorates them
   ?InitResources@SlideDecorationSilverStar@@QAEHXZ and friends) and are called
   as methods against include/SlideDecorationSilverStar.h -- never through a C
   name that does not exist. Slot 17 is a .c TU carrying the ROM's Itanium name
   in extern "C" and is called by that name. */
int *_ZN25SlideDecorationSilverStarD0Ev(int *self);   /* slot 17 */

/* the four factories: ActorBase::operator new(296), Actor::Actor(),
   `p[0] = _ZTV25SlideDecorationSilverStar`, Model::Model(this+0xd4). The vptr
   store is the LAST write of each and it is by a real name, so none of the
   four needs a reseat wrapper. */
void *SlideDecorationSilverStar_Spawn(void);
void *SlideDecorationYellowStar_Spawn(void);
void *SlideDecorationOrangeSmiley_Spawn(void);
void *SlideDecorationBlueSmiley_Spawn(void);

/* the static initialiser that builds the four SharedFilePtrs the variant
   switch reads (files 1565/1566/1569/1564), in port/slice_w10a.txt */
void __sinit_ov031_02111434(void);

DSSTATE_BEGIN
void *_ZTV18daObjHsBillboard_c[31];
DSSTATE_END
}
/* 0x02111984 answers to both names: the D0 restores it by the RTTI one, the
   four factories store it by the recovered class name. One array, one alias.
   The LHS is declared extern by the four Spawn TUs and DEFINED nowhere, which
   is what port/tools/alternatename_guard.py requires. */
#pragma comment(linker, "/alternatename:__ZTV25SlideDecorationSilverStar=__ZTV18daObjHsBillboard_c")

/* CAPTURED, and the argument is hal/level_boot.cpp's on g_level_mounted: this
   flag says "port_ov31_bringup has run", and everything that pass writes --
   the mount's rebased pointers and the SharedFilePtrs its static initialisers
   construct -- lives in .dsstate. A restore rolls that back. A guard that does
   not roll back with it leaves the pass skipped forever and the overlay
   holding raw DS pointers, which is the defect behind both of the RELOAD
   review's referrals. Bracketed, the pass re-runs exactly when its results
   were rolled away. */
DSSTATE_BEGIN
static int g_ov31_bringup_done;
DSSTATE_END

extern "C" void port_ov31_bringup(void)
{
    if (g_ov31_bringup_done)
        return;
    g_ov31_bringup_done = 1;
    port_ov031_pack_check();
    port_ov031_syms_patch();
    __sinit_ov031_02111434();
}

/* ---- the traps ---------------------------------------------------------- */
static void hsb_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: vtable slot %d is not hosted (actor id %u %s)\n",
                 slot, id, port_actor_class_name(id));
    { static char _m[128];
      std::snprintf(_m, sizeof _m, "unhosted vtable slot %d on id %u %s",
                    slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
static int __fastcall hsb_trap13(void *s, void *) { hsb_trap_report(s, 13); return 0; }
static int __fastcall hsb_trap14(void *s, void *) { hsb_trap_report(s, 14); return 0; }

/* ---- the shared 1..30 half ---------------------------------------------- */
static int __fastcall hsb_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall hsb_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall hsb_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall hsb_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall hsb_behavior(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::Behavior(); }
static int __fastcall hsb_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall hsb_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall hsb_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall hsb_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall hsb_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall hsb_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall hsb_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
static int __fastcall hsb_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall hsb_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall hsb_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall hsb_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall hsb_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall hsb_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall hsb_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall hsb_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall hsb_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall hsb_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall hsb_aimed(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }

/* ---- the class's own five ----------------------------------------------- */
static int __fastcall hsb_init(void *s, void *)
{ return ((SlideDecorationSilverStar *)s)
             ->SlideDecorationSilverStar::InitResources(); }
static int __fastcall hsb_clean(void *s, void *)
{ return ((SlideDecorationSilverStar *)s)
             ->SlideDecorationSilverStar::CleanupResources(); }
/* The ROM slot 9. The matched TU is the ROM-order slot-5 dispatch through a
   local six-virtual shadow over the member at +0xd4, and here that member is a
   plain Model (the factories build it with _ZN5ModelC1Ev, which stores
   _ZTV5Model) -- hal_fill_model_vtable dual-fills _ZTV5Model[4] AND [5] with
   mv_render for exactly this shape, so the ROM's slot 5 lands on Model::Render.
   NOT the Bird/Flag/BabyPenguin/Amilift case: those dispatch over a ModelAnim,
   whose host table numbers slot 5 as Virtual18. */
static int __fastcall hsb_render(void *s, void *)
{ return ((SlideDecorationSilverStar *)s)
             ->SlideDecorationSilverStar::Render(); }
/* Slot 16, the ROM D1's own three relocs; see this file's header. */
/* slot 16 is the matched src D1 through hal/dtor_faces_cpp.cpp (lane DTOR-FACES-CPP);
   the transcribed thunk that stood here (hsb_d1) spelled the same chain by hand. */
static int __fastcall hsb_d0(void *s, void *)
{ return (int)(size_t)_ZN25SlideDecorationSilverStarD0Ev((int *)s); }

extern "C" void hal_fill_slide_decoration_vtable(void)
{
    port_ov31_bringup();

    void **vt = (void **)_ZTV18daObjHsBillboard_c;
    vt[0]  = (void *)hsb_init;      /* SlideDecorationSilverStar::InitResources */
    vt[1]  = (void *)hsb_binit;     /* Actor::BeforeInitResources    */
    vt[2]  = (void *)hsb_ainit;     /* Actor::AfterInitResources(u32) */
    vt[3]  = (void *)hsb_clean;     /* SlideDecorationSilverStar::CleanupResources */
    vt[4]  = (void *)hsb_bclean;    /* Actor::BeforeCleanupResources */
    vt[5]  = (void *)hsb_aclean;    /* Actor::AfterCleanupResources(u32) */
    vt[6]  = (void *)hsb_behavior;  /* ActorBase::Behavior -- no own Behavior */
    vt[7]  = (void *)hsb_bbeh;      /* Actor::BeforeBehavior */
    vt[8]  = (void *)hsb_abeh;      /* Actor::AfterBehavior(u32) */
    vt[9]  = (void *)hsb_render;    /* SlideDecorationSilverStar::Render */
    vt[10] = (void *)hsb_bren;      /* Actor::BeforeRender */
    vt[11] = (void *)hsb_aren;      /* Actor::AfterRender(u32) */
    vt[12] = (void *)hsb_pdes;      /* ActorBase::OnPendingDestroy */
    vt[13] = (void *)hsb_trap13;    /* ActorBase::Virtual34(u32,u32), not linked */
    vt[14] = (void *)hsb_trap14;    /* ActorBase::Virtual38(u32,u32), not linked */
    vt[15] = (void *)hsb_heap;      /* ActorBase::OnHeapCreated */
    vt[16] = (void *)hal_cppd1_SlideDecorationSilverStar;        /* D1, hosted; see the header */
    vt[17] = (void *)hsb_d0;        /* D0, the matched .c TU */
    vt[18] = (void *)hsb_yoshi;     /* Actor::OnYoshiTryEat */
    vt[19] = (void *)hsb_egg;       /* Actor::OnTurnIntoEgg(Player&) */
    vt[20] = (void *)hsb_v50;       /* Actor::Virtual50 */
    vt[21] = (void *)hsb_pounded;   /* Actor::OnGroundPounded(Actor&) */
    vt[22] = (void *)hsb_atk1;      /* Actor::OnAttacked1(Actor&) */
    vt[23] = (void *)hsb_atk2;      /* Actor::OnAttacked2(Actor&) */
    vt[24] = (void *)hsb_kicked;    /* Actor::OnKicked(Actor&) */
    vt[25] = (void *)hsb_pushed;    /* Actor::OnPushed(Actor&) */
    vt[26] = (void *)hsb_cannon;    /* Actor::OnHitByCannonBlastedChar(Actor&) */
    vt[27] = (void *)hsb_mega;      /* Actor::OnHitByMegaChar(Player&) */
    vt[28] = (void *)hsb_under;     /* Actor::OnHitFromUnderneath(Actor&) */
    vt[29] = (void *)hsb_aimed;     /* Actor::OnAimedAtWithEgg */
    vt[30] = (void *)port_actor_s30_base;    /* Actor::OnAimedAtWithEggReturnVec, SRET */
    /* no slot 31: a plain Actor, 31 slots total, ends here */
}
