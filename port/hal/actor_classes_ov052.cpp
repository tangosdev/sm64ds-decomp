// RUN LINKW WAVE 12 (lane w12): OV052's TWO PLATFORM CLASSES (level 44).
//
// Level 44, "Mario's key course" (ex_m_map), placed 95 actors and skipped 17,
// and sixteen of those seventeen are these two classes. ov052 is the level's
// OWN overlay and has been whole-mounted since level 44 landed; this is its
// second, per-symbol mount, the ov045/ov012/ov013 dual shape.
//
//   id   class              x on L44  table                     width
//   174  BOBBING_LOG_EMM    8         0x02112520 (unnamed)        32
//   175  SQUARE_PATH_LIFT   8         0x021125f0 _ZTV14SquarePathLift 32
//
// Both attribution routes agree on both ids and the identities come from the
// RTTI typeinfo at vtable[-1] ("13daObjEmmLog_c" and "14daObjEmmYuka_c", yuka =
// floor), not from dsd labels. The full derivation, and the ONE measurement
// that could have taken the level, are in port/ov052_syms.txt.
//
// ---- THE WIDTH THAT READS 38 AND IS 32 -------------------------------------
//
// id 174's reloc run is THIRTY-EIGHT slots long and its table is THIRTY-TWO.
// The six words past the end are two other objects, and both have an
// independent reader that says so:
//   0x021125a0  SquarePathLift's OWN {model SFP, clsn SFP, CLPS} file table --
//               _ZN14SquarePathLift13InitResourcesEv reads exactly those three
//               words by that name and hands them to Model::LoadFile,
//               MeshCollider::LoadFile and MovingMeshCollider::SetFile.
//   0x021125ac  id 175's Itanium typeinfo and its name string.
// and slot 31 is 0x020ee55c, _ZN8Platform4KillEv, the tail every 32-slot
// platform table in the port ends on. Sizing the host array at 38 would have
// swallowed id 175's file table out of the mount and handed its InitResources
// three zeroed words -- the undersized-global failure class running the other
// way, which is why the width is pinned by the next-symbol landing plus the
// Platform::Kill check and not by the run.
//
// The naive next-symbol read does not work either: dsd split BOTH tables
// (0x02112530 / 0x02112570 / 0x02112590 inside 174's, 0x02112610 inside 175's).
// All five splits are excluded from the mount with their tables.
//
// ---- THE VTABLES ARE HOST STORAGE ------------------------------------------
//
// Both excluded from the per-symbol mount and declared here as host arrays, the
// ov015/ov016/ov045/ov060 rule. dsd names 0x021125f0 TWICE
// (_ZTV14SquarePathLift and _ZTV14daObjEmmYuka_c) and id 174's table not at
// all; the two RTTI spellings the destructors restore by are aliased onto the
// one host array each.
//
// ---- ONE SHARED-WINDOW MISSPELLING -----------------------------------------
//
// src/func_ov052_0211123c.c (id 174's CleanupResources) spells ov052's own file
// table `data_ov056_021124d4` -- a dsd shared-window misname; ov052 and ov056
// link at overlapping bases and the load site's relocs (0x02111260/0x02111268)
// target 0x021124d4 module:overlay(52). Its sibling func_ov052_02111348 spells
// the SAME address `data_ov052_021124d4`, which is the correct one and is what
// this mount emits. The misspelling is routed with a PER-SOURCE RENAME in
// port/CMakeLists.txt rather than an /alternatename, deliberately: ov056 is a
// live target on this board (id 173, nine instances), and the day it is mounted
// an alias with an ov056 LHS would be DEFEATED silently -- the exact failure
// this lane already hit once on ov071/ov073. A -D cannot be defeated.
//
// ---- WHY THE MOUNT BRING-UP IS IN THIS FILE --------------------------------
//
// The ov045/ov060/ov031/ov073 lane-ownership pattern: no lane owns
// hal/actor_overlays.cpp in this wave, so the bring-up rides the first registry
// fill (ov52_bringup, one done-guard, both fills call it). Both ov052 sinits
// link -- neither name is contested at a shared window -- so unlike ov045
// nothing here is hand-hosted, and there are no state tables to seat: neither
// class has a pointer-to-member dispatcher.
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
#include <cstdlib>

#include "Actor.h"
#include "ActorBase.h"

extern "C" {
/* the arm9 shared half. Slots 1/2/4/5/7/8/10..15/18..30 of BOTH tables are the
   same arm9 words, and they are the same words ov045's six platform tables and
   ov073's two carry -- checked slot for slot before this fill was written. */
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

/* the generated ov052 per-symbol mount (build/port/host-src/ov052_syms.c) */
void port_ov052_pack_check(void);
void port_ov052_syms_patch(void);
/* both ov052 sinits, both linkable */
void __sinit_ov052_02111b64(void);
void __sinit_ov052_02111bd0(void);
}

/* The two RTTI spellings the destructors restore their tables by. id 174's D1
   (func_ov052_021111a0) writes _ZTV13daObjEmmLog_c and then the zeroed
   _ZTV10dBgActor_c placeholder hal/actor_classes.cpp already defines port-wide;
   id 175's D1 does the same with _ZTV14daObjEmmYuka_c. Both are dying-object
   writes -- the two stores sit between direct calls with no dispatch in between
   -- and the alias makes the FIRST one land on the real host array anyway.
   0x021125f0 carries the Yuka name as a dsd symbol of its own and it is
   excluded from the mount with the table, so this LHS is undefined everywhere
   and the alias cannot be defeated. */
#pragma comment(linker, "/alternatename:__ZTV13daObjEmmLog_c=_data_ov052_02112520")
#pragma comment(linker, "/alternatename:__ZTV14daObjEmmYuka_c=__ZTV14SquarePathLift")

/* ONE C++-MANGLED DATA SPELLING, the data_02082128 / data_020a0e68 precedent.
   src/func_ov052_02111348.cpp declares id 174's file table at FILE SCOPE as
   `struct DataT { SharedFilePtr *a, *b; CLPS_Block *c; }` rather than inside an
   extern "C" block, so it emits a decorated data name; the mount emits the one
   C-named array. Its sibling func_ov052_0211123c reaches the SAME twelve bytes
   through the ov056 misspelling this lane renames per source. LHS declared and
   never defined, so alternatename_guard stays clean. */
#pragma comment(linker, "/alternatename:?data_ov052_021124d4@@3UDataT@@A=_data_ov052_021124d4")

// ---- the trap --------------------------------------------------------------
static void ov52_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: ov052 vtable slot %d is not hosted (actor id %u "
                 "%s)\n", slot, id, port_actor_class_name(id));
    { static char _m[128];
      std::snprintf(_m, sizeof _m, "unhosted ov052 vtable slot %d on id %u %s",
                    slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
#define OV52_TRAP(n) \
    static int __fastcall ov52_trap##n(void *s, void *) \
    { ov52_trap_report(s, n); return 0; }
OV52_TRAP(13) OV52_TRAP(14) OV52_TRAP(17)
#undef OV52_TRAP

static int __fastcall ov52_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall ov52_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall ov52_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall ov52_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall ov52_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall ov52_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall ov52_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall ov52_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall ov52_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall ov52_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall ov52_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
/* slot 19 takes the three-parameter shape so it emits `ret 4`: the dispatch
   site pushes the Player the callee pops -- the wf_turn_egg contract. */
static int __fastcall ov52_turn_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall ov52_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall ov52_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall ov52_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall ov52_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall ov52_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall ov52_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall ov52_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall ov52_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall ov52_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall ov52_egg(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }
/* slot 31, the Platform tail. BOTH tables take it unchanged. */
static int __fastcall ov52_kill(void *s, void *)
{ _ZN8Platform4KillEv(s); return 0; }

/* The shared half, slots 1..30. The caller writes its own 0/3/6/9/16/17 and 31.
   Slots 13/14 trap and 30 declines (the wf/ov045 reading: 30's ROM body returns
   a Vector3 by value and the sret contract is unproved); 17 traps here and both
   callers replace it. THE POINTER IS VOLATILE ON PURPOSE -- the gate-200
   elided-stores bug, repro in hal/actor_classes_ov002g200.cpp. */
static void ov52_fill_shared(void *volatile *vt)
{
    vt[1]  = (void *)ov52_binit;
    vt[2]  = (void *)ov52_ainit;
    vt[4]  = (void *)ov52_bclean;
    vt[5]  = (void *)ov52_aclean;
    vt[7]  = (void *)ov52_bbeh;
    vt[8]  = (void *)ov52_abeh;
    vt[10] = (void *)ov52_bren;
    vt[11] = (void *)ov52_aren;
    vt[12] = (void *)ov52_pdes;
    vt[13] = (void *)ov52_trap13;
    vt[14] = (void *)ov52_trap14;
    vt[15] = (void *)ov52_heap;
    vt[17] = (void *)ov52_trap17;
    vt[18] = (void *)ov52_yoshi;
    vt[19] = (void *)ov52_turn_egg;
    vt[20] = (void *)ov52_v50;
    vt[21] = (void *)ov52_pounded;
    vt[22] = (void *)ov52_atk1;
    vt[23] = (void *)ov52_atk2;
    vt[24] = (void *)ov52_kicked;
    vt[25] = (void *)ov52_pushed;
    vt[26] = (void *)ov52_cannon;
    vt[27] = (void *)ov52_mega;
    vt[28] = (void *)ov52_under;
    vt[29] = (void *)ov52_egg;
    vt[30] = (void *)port_actor_s30_base;
}

// ---- the mount bring-up ----------------------------------------------------
/* CAPTURED, and the argument is hal/level_boot.cpp's on g_level_mounted: this
   flag says "port_ov52_bringup has run", and everything that pass writes --
   the mount's rebased pointers and the SharedFilePtrs its static initialisers
   construct -- lives in .dsstate. A restore rolls that back. A guard that does
   not roll back with it leaves the pass skipped forever and the overlay
   holding raw DS pointers, which is the defect behind both of the RELOAD
   review's referrals. Bracketed, the pass re-runs exactly when its results
   were rolled away. */
DSSTATE_BEGIN
static int g_ov52_bringup_done;
DSSTATE_END

extern "C" void port_ov52_bringup(void)
{
    if (g_ov52_bringup_done)
        return;
    g_ov52_bringup_done = 1;
    port_ov052_pack_check();
    port_ov052_syms_patch();
    __sinit_ov052_02111b64();
    __sinit_ov052_02111bd0();
}

// ============================================================================
// BOBBING_LOG_EMM (174) -- table 0x02112520, the func_ov052_0211* bodies
// ============================================================================
//
// 808-byte object built by Platform's constructor; Model at +0xd4,
// MovingMeshCollider at +0x124. Its Behavior rides the arm9 sine table
// data_02082214 on Y (amplitude at +0x324, base at +0x320, both set by its
// InitResources from the spawn param) and spins +0x31e by 0x200 a frame -- a
// bobbing, rolling log. Its file table is data_ov052_021124d4.
extern "C" {
int func_ov052_02111348(char *self);   /* slot 0  InitResources */
int func_ov052_0211123c(char *self);   /* slot 3  CleanupResources */
int func_ov052_021112ac(char *self);   /* slot 6  Behavior */
int func_ov052_02111284(void *self);   /* slot 9  Render */
int *func_ov052_021111a0(int *self);   /* slot 16 D1 */
int *func_ov052_021111e4(int *self);   /* slot 17 D0 */
void *func_ov052_02111410(void);       /* the factory */
DSSTATE_BEGIN
void *data_ov052_02112520[32];
DSSTATE_END
}
static int __fastcall log_init(void *s, void *)
{ return func_ov052_02111348((char *)s); }
static int __fastcall log_clean(void *s, void *)
{ return func_ov052_0211123c((char *)s); }
static int __fastcall log_behavior(void *s, void *)
{ return func_ov052_021112ac((char *)s); }
static int __fastcall log_render(void *s, void *)
{ port_actor_render_probe("BOBBING_LOG_EMM", (char *)s + 0xd4);
  return func_ov052_02111284(s); }
static int __fastcall log_d1(void *s, void *)
{ return (int)(size_t)func_ov052_021111a0((int *)s); }
static int __fastcall log_d0(void *s, void *)
{ return (int)(size_t)func_ov052_021111e4((int *)s); }
extern "C" void hal_fill_bobbing_log_emm_vtable(void)
{
    port_ov52_bringup();
    void *volatile *vt = (void *volatile *)data_ov052_02112520;
    ov52_fill_shared(vt);
    vt[0]  = (void *)log_init;
    vt[3]  = (void *)log_clean;
    vt[6]  = (void *)log_behavior;
    vt[9]  = (void *)log_render;
    vt[16] = (void *)log_d1;
    vt[17] = (void *)log_d0;
    vt[31] = (void *)ov52_kill;
}

// ============================================================================
// SQUARE_PATH_LIFT (175) -- table 0x021125f0 == _ZTV14daObjEmmYuka_c
// ============================================================================
//
// 816-byte object; Model at +0xd4, MovingMeshCollider at +0x124 and a PathPtr
// at +0x320 its factory constructs and its InitResources binds to the spawn
// param's path id. Its Behavior walks the path node by node at 0xa000 a frame,
// reversing at the ends when the path does not loop. Its file table is
// data_ov052_021125a0 -- the three words id 174's reloc run overran into.
extern "C" {
int _ZN14SquarePathLift13InitResourcesEv(void *self);     /* slot 0  */
int _ZN14SquarePathLift16CleanupResourcesEv(void *self);  /* slot 3  */
int _ZN14SquarePathLift8BehaviorEv(void *self);           /* slot 6  */
int _ZN14SquarePathLift6RenderEv(void *self);             /* slot 9  */
int *_ZN14SquarePathLiftD1Ev(int *self);                  /* slot 16 */
int *_ZN14SquarePathLiftD0Ev(int *self);                  /* slot 17 */
void *SquarePathLift_Spawn(void);
DSSTATE_BEGIN
void *_ZTV14SquarePathLift[32];
DSSTATE_END
}
static int __fastcall spl_init(void *s, void *)
{ return _ZN14SquarePathLift13InitResourcesEv(s); }
static int __fastcall spl_clean(void *s, void *)
{ return _ZN14SquarePathLift16CleanupResourcesEv(s); }
static int __fastcall spl_behavior(void *s, void *)
{ return _ZN14SquarePathLift8BehaviorEv(s); }
static int __fastcall spl_render(void *s, void *)
{ port_actor_render_probe("SQUARE_PATH_LIFT", (char *)s + 0xd4);
  return _ZN14SquarePathLift6RenderEv(s); }
static int __fastcall spl_d1(void *s, void *)
{ return (int)(size_t)_ZN14SquarePathLiftD1Ev((int *)s); }
static int __fastcall spl_d0(void *s, void *)
{ return (int)(size_t)_ZN14SquarePathLiftD0Ev((int *)s); }
extern "C" void hal_fill_square_path_lift_vtable(void)
{
    port_ov52_bringup();
    void *volatile *vt = (void *volatile *)_ZTV14SquarePathLift;
    ov52_fill_shared(vt);
    vt[0]  = (void *)spl_init;
    vt[3]  = (void *)spl_clean;
    vt[6]  = (void *)spl_behavior;
    vt[9]  = (void *)spl_render;
    vt[16] = (void *)spl_d1;
    vt[17] = (void *)spl_d0;
    vt[31] = (void *)ov52_kill;
}

// ---- method faces ----------------------------------------------------------
// The four bodies src defines as real C++ methods rather than extern-C free
// functions -- all four of SquarePathLift's. id 174's six are extern-C already.
#include "SquarePathLift.h"
extern "C" {
int _ZN14SquarePathLift13InitResourcesEv(void *self)
{ return ((SquarePathLift *)self)->SquarePathLift::InitResources(); }
int _ZN14SquarePathLift16CleanupResourcesEv(void *self)
{ return ((SquarePathLift *)self)->SquarePathLift::CleanupResources(); }
int _ZN14SquarePathLift8BehaviorEv(void *self)
{ return ((SquarePathLift *)self)->SquarePathLift::Behavior(); }
int _ZN14SquarePathLift6RenderEv(void *self)
{ return ((SquarePathLift *)self)->SquarePathLift::Render(); }
}
