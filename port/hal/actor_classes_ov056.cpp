// RUN LINKW WAVE 15 (lane w15): OV056's BIG_MOVING_ICE_BLOCK (173).
//
// Level 48, "Wario's key course" (ex_w_map, course 20), spawns 93 actors and
// skips 14. NINE of the fourteen are this one class; the other five are
// MR_BLIZZARD (223), hosted and deliberately unregistered for a measured
// frame-0 register skew. So this file empties level 48's skip list of
// everything that is not already refused on purpose.
//
//   id   class                x on L48  table                          width
//   173  BIG_MOVING_ICE_BLOCK  9        0x02113368 _ZTV17BigMovingIceBlock  32
//
// Both attribution routes agree (the record's word[0] lands inside ov056 and
// the record reads 173 back at +4) and the identity comes from the RTTI
// typeinfo at vtable[-1] -- "18daObjEwmIceBlock_c", EWM being the level prefix
// -- not from the dsd label. The full derivation is in port/ov056_syms.txt.
//
// ov056 is a LEVEL overlay already mounted --whole; this adds the second,
// per-symbol mount, the ov045/ov012/ov013/ov052 dual shape.
//
// ---- THE WIDTH THAT READS 16 AND IS 32 -------------------------------------
//
// The reloc run is 32 and stops on its own. The NEXT-SYMBOL landing reads 16,
// because dsd split the table at 0x021133a8 -- slot 16 exactly, the
// destructor pair, named as if it opened a new object. The ov052 trap in
// mirror image: there the run over-read past the end of a 32-slot table and
// here the landing under-reads to half of one.
//
// The pin is the ROM's own layout and it is the slot-31 Platform check:
//   slot 29  Actor::OnAimedAtWithEgg
//   slot 30  Actor::OnAimedAtWithEggReturnVec
//   slot 31  0x020ee55c  _ZN8Platform4KillEv
// the close every 32-slot platform table in the port ends on (ov045's four,
// ov060's two, ov052's two). The six words after the table (0x021133e8 ..
// 0x02113400) are ROM ZEROS to the .data end -- align-32 padding, not another
// object -- so nothing an oversized span could swallow and nothing an
// undersized one drops. A 16-slot array would have left slots 16..31 off the
// end of host storage: the D1/D0 pair, the whole Actor interaction list and
// Platform::Kill, every one of them dispatched.
//
// ---- THE VTABLE IS HOST STORAGE --------------------------------------------
//
// Excluded from the per-symbol mount and declared here as a host array, the
// ov015/ov016/ov045/ov052/ov060 rule: a mounted vtable hands the factory a
// table of DS code addresses. dsd names 0x02113368 TWICE
// (_ZTV17BigMovingIceBlock and _ZTV18daObjEwmIceBlock_c) -- the Spawn spells
// the first and both destructors spell the second -- so the RTTI spelling is
// aliased onto the one host array. The interior split at 0x021133a8 is
// excluded with it.
//
// ---- WHAT THIS LANE DID NOT NEED -------------------------------------------
//
// No state table to seat: the class has no pointer-to-member dispatcher, its
// Behavior walks a PathPtr node by node inline. No factory wrapper: the Spawn
// names its receiver and stores the table by a real name as its final write.
// No ModelAnim collision: Render's local six-virtual shadow lands on slot 5 of
// a PLAIN Model at +0xd4 (built by Model::LoadFile + ModelBase::SetFile, torn
// down by _ZN5ModelD1Ev, never SetAnim'd) and hal/cxxname_bridge.cpp
// dual-fills _ZTV5Model at BOTH slot 4 and slot 5, so the matched Render
// serves from src unchanged -- the ov052/ov072 reading, not the BabyPenguin
// one. No per-source rename: five of the six bodies sit at addresses ov052
// also uses (both overlays link at 0x021111a0) but dsd gave the two sets
// different names, so they compile side by side untouched.
//
// ---- WHY THE MOUNT BRING-UP IS IN THIS FILE --------------------------------
//
// The ov045/ov052/ov060/ov073 lane-ownership pattern: no lane owns
// hal/actor_overlays.cpp in this wave, so the bring-up rides the registry fill
// behind one done-guard. It runs the pack check, the syms patch,
// hal_fill_platform_vtable (the class's destructors install ov002 0x0210ae38
// as the base vptr on the way out, and that array has to be filled before the
// first teardown, not merely defined) and then ov056's single sinit, which
// constructs the two SharedFilePtrs InitResources loads from.
#include <cstdio>
#include "dsstate_seg.h"
#include <cstdlib>

#include "Actor.h"
#include "ActorBase.h"

extern "C" {
/* the arm9 shared half. Slots 1/2/4/5/7/8/10..15/18..30 are the same arm9
   words ov045's six platform tables, ov052's two and ov073's two carry --
   checked slot for slot against the ROM table before this fill was written. */
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

/* the generated ov056 per-symbol mount (build/port/host-src/ov056_syms.c) */
void port_ov056_pack_check(void);
void port_ov056_syms_patch(void);
/* ov056's single sinit: the two SharedFilePtrs (files 1517 and 1518) and their
   destructor-chain nodes. Matched src, uncontested at the shared window. */
void __sinit_ov056_02112b48(void);
}

/* The RTTI spelling both destructors restore the table by. src's D1 and D0
   write _ZTV18daObjEwmIceBlock_c and then _ZTV10dBgActor_c (ov002 0x0210ae38,
   Platform's base table, filled port-wide by hal_fill_platform_vtable); the
   first is this array under the other of dsd's two names for 0x02113368.
   BOTH names are excluded from the per-symbol mount, so this LHS is undefined
   everywhere and the alias cannot be defeated. */
#pragma comment(linker, "/alternatename:__ZTV18daObjEwmIceBlock_c=__ZTV17BigMovingIceBlock")

// ---- the trap --------------------------------------------------------------
static void ov56_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: ov056 vtable slot %d is not hosted (actor id %u "
                 "%s)\n", slot, id, port_actor_class_name(id));
    { static char _m[128];
      std::snprintf(_m, sizeof _m, "unhosted ov056 vtable slot %d on id %u %s",
                    slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
#define OV56_TRAP(n) \
    static int __fastcall ov56_trap##n(void *s, void *) \
    { ov56_trap_report(s, n); return 0; }
OV56_TRAP(13) OV56_TRAP(14)
#undef OV56_TRAP

static int __fastcall ov56_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall ov56_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall ov56_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall ov56_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall ov56_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall ov56_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall ov56_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall ov56_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall ov56_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall ov56_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall ov56_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
/* slot 19 takes the three-parameter shape so it emits `ret 4`: the dispatch
   site pushes the Player the callee pops -- the wf_turn_egg contract. */
static int __fastcall ov56_turn_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall ov56_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall ov56_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall ov56_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall ov56_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall ov56_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall ov56_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall ov56_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall ov56_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall ov56_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall ov56_egg(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }
/* slot 31, the Platform tail, the word this table's width is pinned by. */
static int __fastcall ov56_kill(void *s, void *)
{ _ZN8Platform4KillEv(s); return 0; }

// ---- BIG_MOVING_ICE_BLOCK (173) --------------------------------------------
//
// 816-byte object built by Platform's constructor; Model at +0xd4,
// MovingMeshCollider at +0x124, PathPtr at +0x320 the factory constructs and
// InitResources binds to the spawn param's low byte. Its Behavior walks the
// path node by node at 0xa000 a frame, reversing at the ends when the path
// does not loop -- the SquarePathLift shape a level over. Its file table is
// data_ov056_02113314, whose third word is a CLPS_Block in ov064.
extern "C" {
int _ZN17BigMovingIceBlock16CleanupResourcesEv(char *self);   /* slot 3  */
int *_ZN17BigMovingIceBlockD1Ev(int *self);                   /* slot 16 */
int *_ZN17BigMovingIceBlockD0Ev(int *self);                   /* slot 17 */
void *BigMovingIceBlock_Spawn(void);
DSSTATE_BEGIN
void *_ZTV17BigMovingIceBlock[32];
DSSTATE_END
}

/* The three bodies src defines as real C++ methods rather than extern-"C" free
   functions. The other three (both destructors and CleanupResources) are
   extern-C in src already. */
#include "BigMovingIceBlock.h"

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
int _ZN17BigMovingIceBlock13InitResourcesEv(void *self)
{ return ((BigMovingIceBlock *)self)->BigMovingIceBlock::InitResources(); }
int _ZN17BigMovingIceBlock8BehaviorEv(void *self)
{ return ((BigMovingIceBlock *)self)->BigMovingIceBlock::Behavior(); }
int _ZN17BigMovingIceBlock6RenderEv(void *self)
{ return ((BigMovingIceBlock *)self)->BigMovingIceBlock::Render(); }
}

static int __fastcall ice_init(void *s, void *)
{ return _ZN17BigMovingIceBlock13InitResourcesEv(s); }
static int __fastcall ice_clean(void *s, void *)
{ return _ZN17BigMovingIceBlock16CleanupResourcesEv((char *)s); }
static int __fastcall ice_behavior(void *s, void *)
{ return _ZN17BigMovingIceBlock8BehaviorEv(s); }
static int __fastcall ice_render(void *s, void *)
{ port_actor_render_probe("BIG_MOVING_ICE_BLOCK", (char *)s + 0xd4);
  return _ZN17BigMovingIceBlock6RenderEv(s); }
static int __fastcall ice_d1(void *s, void *)
{ return (int)(size_t)_ZN17BigMovingIceBlockD1Ev((int *)s); }
static int __fastcall ice_d0(void *s, void *)
{ return (int)(size_t)_ZN17BigMovingIceBlockD0Ev((int *)s); }

// ---- the mount bring-up ----------------------------------------------------
/* CAPTURED, and the argument is hal/level_boot.cpp's on g_level_mounted: this
   flag says "port_ov56_bringup has run", and everything that pass writes --
   the mount's rebased pointers and the SharedFilePtrs its static initialisers
   construct -- lives in .dsstate. A restore rolls that back. A guard that does
   not roll back with it leaves the pass skipped forever and the overlay
   holding raw DS pointers, which is the defect behind both of the RELOAD
   review's referrals. Bracketed, the pass re-runs exactly when its results
   were rolled away. */
DSSTATE_BEGIN
static int g_ov56_bringup_done;
DSSTATE_END

extern "C" void port_ov56_bringup(void)
{
    if (g_ov56_bringup_done)
        return;
    g_ov56_bringup_done = 1;
    port_ov056_pack_check();
    port_ov056_syms_patch();
    /* before the sinit and before any teardown: both destructors install ov002
       0x0210ae38 as the base vptr on the way out, and that array is plain
       zeroed storage until this fill runs. */
    hal_fill_platform_vtable();
    __sinit_ov056_02112b48();
}

extern "C" void hal_fill_big_moving_ice_block_vtable(void)
{
    port_ov56_bringup();
    /* THE POINTER IS VOLATILE ON PURPOSE -- the gate-200 elided-stores bug,
       repro in hal/actor_classes_ov002g200.cpp. */
    void *volatile *vt = (void *volatile *)_ZTV17BigMovingIceBlock;
    vt[0]  = (void *)ice_init;
    vt[1]  = (void *)ov56_binit;
    vt[2]  = (void *)ov56_ainit;
    vt[3]  = (void *)ice_clean;
    vt[4]  = (void *)ov56_bclean;
    vt[5]  = (void *)ov56_aclean;
    vt[6]  = (void *)ice_behavior;
    vt[7]  = (void *)ov56_bbeh;
    vt[8]  = (void *)ov56_abeh;
    vt[9]  = (void *)ice_render;
    vt[10] = (void *)ov56_bren;
    vt[11] = (void *)ov56_aren;
    vt[12] = (void *)ov56_pdes;
    /* 13/14 trap and 30 traps: the ov045/ov052 reading -- 30's ROM body
       returns a Vector3 by value and the sret contract is unproved. */
    vt[13] = (void *)ov56_trap13;
    vt[14] = (void *)ov56_trap14;
    vt[15] = (void *)ov56_heap;
    vt[16] = (void *)ice_d1;
    vt[17] = (void *)ice_d0;
    vt[18] = (void *)ov56_yoshi;
    vt[19] = (void *)ov56_turn_egg;
    vt[20] = (void *)ov56_v50;
    vt[21] = (void *)ov56_pounded;
    vt[22] = (void *)ov56_atk1;
    vt[23] = (void *)ov56_atk2;
    vt[24] = (void *)ov56_kicked;
    vt[25] = (void *)ov56_pushed;
    vt[26] = (void *)ov56_cannon;
    vt[27] = (void *)ov56_mega;
    vt[28] = (void *)ov56_under;
    vt[29] = (void *)ov56_egg;
    vt[30] = (void *)port_actor_s30_base;
    vt[31] = (void *)ov56_kill;
}
