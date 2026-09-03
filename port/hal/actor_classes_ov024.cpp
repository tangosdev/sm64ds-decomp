// RUN REL0215 WAVE 1 (lane cast-sweep1): OV024'S TWO PYRAMID CLASSES (level
// 16, Shifting Sand Land). Overlay 2 of 6 in this lane.
//
// Level 16 spawns 69 actors and skips 19 across seven ids on tip f77f01169.
// FIVE of those instances are this overlay's own cast:
//
//   id  name          x on L16  factory           table       width
//   85  PYRAMID_TOP    1        PyramidTop_Spawn  0x021138c8   32
//   86  PYRAMID_TAG    4        PyramidTag_Spawn  0x02113844   31
//
// The other five ids level 16 skips belong to ov096 (POKEY 232, POKEY_SEGMENT
// 240, TORNADO 308/309 territory) and ov092 (TOX_BOX) -- two unmounted packs
// that are wave 2's yield, not this lane's blocker. They stay skipped and the
// census names them.
//
// ov024 is a LEVEL overlay, whole-mounted in PORT_LEVEL_OVERLAYS. This lane
// adds its per-symbol half, the ov012/ov013/ov025/ov035 dual-mount shape. Both
// attribution routes, both widths, the four T3 pins and the sinit map are in
// port/ov024_syms.txt; the slice and its one held-out TU are in
// port/slice_sweep1_ov024.txt.
//
// ---- THE TWO WIDTHS ARE DIFFERENT, AND THAT IS THE FINDING ----------------
//
// These two classes do not share a base and their tables do not share a
// length. PyramidTop is a PLATFORM (952 bytes, Platform's constructor, Model
// at +0xd4, MovingMeshCollider at +0x124, a second Model at +0x320) and its
// table closes on the usual slot-31 Platform::Kill, width 32. PyramidTag is a
// plain ACTOR (268 bytes, Actor's constructor, a MovingCylinderClsn at +0xd4
// and nothing else) and its table has NO slot 31 -- the word there is a
// literal zero, not a relocation, and the eight bytes at 0x021138c0 are the
// next table's Itanium prologue. Width 31. Reading either width off the other
// would run one fill past its storage or leave the other's tail unfilled.
// dsd's landings under-read both: 25 slots for the tag, FOUR for the top.
//
// PyramidTag also has no Cleanup and no Render of its own: slots 3 and 9 hold
// ActorBase::CleanupResources and ActorBase::Render, the base bodies. Its fill
// writes 0, 6, 16 and 17 and takes the rest from the shared half.
//
// ---- T1: BOTH TABLES STAY ROM-SHAPED --------------------------------------
//
// Neither class derives from Model or ModelAnim -- PyramidTop HOLDS two Models
// as members and PyramidTag holds none -- so the default applies and both
// tables keep the ROM's [16] D1, [17] D0 pair. Every slot was verified against
// config/arm9/overlays/ov024/relocs.txt before a word was replaced.
//
// ---- ONE HELD-OUT TU ------------------------------------------------------
//
// src/_ZN10PyramidTagD1Ev.cpp is the //cpp real-destructor shape and MSVC
// cannot deliver it: it would emit ??1PyramidTag@@UAE@XZ and call
// ??1Actor@@UAE@XZ / ??1MovingCylinderClsn@@QAE@XZ as thiscall, and the port
// defines those two only as cdecl C bodies. pt_tag_d1 below is the ROM body
// transcribed instruction for instruction; the disassembly is in the slice
// file's header. Its sibling D0 is a plain .c and rides from src.
//
// ---- WHY THE MOUNT BRING-UP IS IN THIS FILE -------------------------------
//
// The ov025/ov035/ov045/ov052/ov056/ov073 lane-ownership pattern: no lane owns
// hal/actor_overlays.cpp this wave, so the bring-up rides the first registry
// fill behind one DSSTATE-bracketed done-guard. It runs the pack check, the
// syms patch, hal_fill_platform_vtable (PyramidTop's destructors install ov002
// 0x0210ae38 as the base vptr on the way out, and that array is plain zeroed
// storage until the fill runs) and then ov024's ONE sinit, which is all of
// them -- there is no class here without reach.
#include <cstdio>
#include "dsstate_seg.h"
#include "dtor_faces_cpp.h"
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

/* the generated ov024 per-symbol mount (build/port/host-src/ov024_syms.c) */
void port_ov024_pack_check(void);
void port_ov024_syms_patch(void);
/* ov024's ONE sinit: id 85's model (file 1501) and collision (1502)
   SharedFilePtrs and their two chain nodes. id 86 loads no file of its own. */
void __sinit_ov024_02112808(void);

/* the bodies src spells with C names (port/slice_sweep1_ov024.txt) */
int *_ZN10PyramidTopD1Ev(int *self);              /* 0x021138c8 slot 16 */
int *_ZN10PyramidTopD0Ev(int *self);              /* 0x021138c8 slot 17 */
int *_ZN10PyramidTagD0Ev(int *self);              /* 0x02113844 slot 17 */
void *PyramidTop_Spawn(void);                     /* id 85 */
void *PyramidTag_Spawn(void);                     /* id 86 */

/* the two seams the held-out PyramidTag D1 thunk calls, both already linked */
void _ZN18MovingCylinderClsnD1Ev(void *self);     /* arm9 0x020149a4 */
void *_ZN5ActorD2Ev(void *self);                  /* arm9 0x020112c8 */

/* the two host vtables, both excluded from the mount. 31 and 32 -- NOT the
   same length, see this file's header. */
DSSTATE_BEGIN
void *_ZTV10PyramidTag[31];    /* 0x02113844, id 86, Actor close */
void *_ZTV10PyramidTop[32];    /* 0x021138c8, id 85, Platform close */
DSSTATE_END
}

/* The RTTI spellings the destructors restore their tables by. dsd names each
   table twice and the matched destructors reach both through the RTTI name;
   all four names are excluded from the mount, so both LHS are undefined
   everywhere and neither alias can be defeated. The other word PyramidTop's
   destructors store, _ZTV10dBgActor_c, is Platform's base table and is already
   a host array in hal/actor_classes.cpp. */
#pragma comment(linker, "/alternatename:__ZTV16daObjDlPyramid_c=__ZTV10PyramidTop")
#pragma comment(linker, "/alternatename:__ZTV21daObjDlPyramidDummy_c=__ZTV10PyramidTag")

/* The six bodies src defines as real C++ methods rather than extern-"C" free
   functions, faced here -- the ov013/ov025/ov035 recipe. Both Renders ride
   FROM SRC: PyramidTop's is a plain-Model slot-5 bare-call shadow, and it goes
   over the SECOND model at +0x320, not +0xd4 (the one its InitResources gives
   the BMD). PyramidTag has no Render at all. */
#include "PyramidTop.h"
#include "PyramidTag.h"
extern "C" {
int _ZN10PyramidTop13InitResourcesEv(void *self)
{ return ((PyramidTop *)self)->PyramidTop::InitResources(); }
int _ZN10PyramidTop16CleanupResourcesEv(void *self)
{ return ((PyramidTop *)self)->PyramidTop::CleanupResources(); }
int _ZN10PyramidTop8BehaviorEv(void *self)
{ return ((PyramidTop *)self)->PyramidTop::Behavior(); }
int _ZN10PyramidTop6RenderEv(void *self)
{ return ((PyramidTop *)self)->PyramidTop::Render(); }
int _ZN10PyramidTag13InitResourcesEv(void *self)
{ return ((PyramidTag *)self)->PyramidTag::InitResources(); }
int _ZN10PyramidTag8BehaviorEv(void *self)
{ return ((PyramidTag *)self)->PyramidTag::Behavior(); }
}

// ---- the trap --------------------------------------------------------------
static void ov24_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: ov024 vtable slot %d is not hosted (actor id %u "
                 "%s)\n", slot, id, port_actor_class_name(id));
    { static char _m[128];
      std::snprintf(_m, sizeof _m, "unhosted ov024 vtable slot %d on id %u %s",
                    slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
#define OV24_TRAP(n) \
    static int __fastcall ov24_trap##n(void *s, void *) \
    { ov24_trap_report(s, n); return 0; }
OV24_TRAP(13) OV24_TRAP(14) OV24_TRAP(30)
#undef OV24_TRAP

static int __fastcall ov24_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall ov24_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall ov24_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall ov24_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall ov24_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall ov24_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall ov24_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall ov24_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall ov24_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall ov24_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall ov24_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
/* slot 19 takes the three-parameter shape so it emits `ret 4` -- the
   wf_turn_egg contract. */
static int __fastcall ov24_turn_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall ov24_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall ov24_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall ov24_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall ov24_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall ov24_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall ov24_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall ov24_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall ov24_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall ov24_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall ov24_egg(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }
static int __fastcall ov24_kill(void *s, void *)
{ _ZN8Platform4KillEv(s); return 0; }

/* Slots 1..30 -- the half BOTH tables share, which is every slot up to the one
   where they diverge. The 31-slot table simply has no slot 31 to write.

   THE POINTER IS VOLATILE ON PURPOSE -- the gate-200 elided-stores bug, repro
   and dumpbin proof in hal/actor_classes_ov002g200.cpp. This helper has
   exactly two callers. */
static void ov24_fill_shared(void *volatile *vt)
{
    vt[1]  = (void *)ov24_binit;
    vt[2]  = (void *)ov24_ainit;
    vt[4]  = (void *)ov24_bclean;
    vt[5]  = (void *)ov24_aclean;
    vt[7]  = (void *)ov24_bbeh;
    vt[8]  = (void *)ov24_abeh;
    vt[10] = (void *)ov24_bren;
    vt[11] = (void *)ov24_aren;
    vt[12] = (void *)ov24_pdes;
    vt[13] = (void *)ov24_trap13;
    vt[14] = (void *)ov24_trap14;
    vt[15] = (void *)ov24_heap;
    vt[18] = (void *)ov24_yoshi;
    vt[19] = (void *)ov24_turn_egg;
    vt[20] = (void *)ov24_v50;
    vt[21] = (void *)ov24_pounded;
    vt[22] = (void *)ov24_atk1;
    vt[23] = (void *)ov24_atk2;
    vt[24] = (void *)ov24_kicked;
    vt[25] = (void *)ov24_pushed;
    vt[26] = (void *)ov24_cannon;
    vt[27] = (void *)ov24_mega;
    vt[28] = (void *)ov24_under;
    vt[29] = (void *)ov24_egg;
    vt[30] = (void *)ov24_trap30;
}

// ---- the mount bring-up ----------------------------------------------------
/* CAPTURED, the hal/level_boot.cpp g_level_mounted argument: everything this
   pass writes lives in .dsstate, so a restore rolls it back and the guard has
   to roll back with it or the pass stays skipped forever. */
DSSTATE_BEGIN
static int g_ov24_bringup_done;
DSSTATE_END

extern "C" void port_ov24_bringup(void)
{
    if (g_ov24_bringup_done)
        return;
    g_ov24_bringup_done = 1;
    port_ov024_pack_check();
    port_ov024_syms_patch();
    hal_fill_platform_vtable();
    __sinit_ov024_02112808();
}

// ============================================================================
// PYRAMID_TOP (85) -- table 0x021138c8, 32 slots, RTTI 16daObjDlPyramid_c.
// ============================================================================
//
// The pyramid's rising top block. 952-byte Platform: Model at +0xd4,
// MovingMeshCollider at +0x124, a SECOND Model at +0x320 (the one its
// InitResources actually gives the BMD, file 1501), the collider transform at
// +0x370 and a saved spawn position at +0x3a0. InitResources loads model 1501
// and KCL 1502, hands the collider CLPS 0x021129f0 and installs
// MeshColliderBase::UpdatePosAndAngs as the BeforeClsn callback, then ENABLES
// the collider against itself. +0x3b6 is the counter PyramidTag increments,
// which is how the four tags drive the one top.
static int __fastcall pt_top_init(void *s, void *)
{ return _ZN10PyramidTop13InitResourcesEv(s); }
static int __fastcall pt_top_clean(void *s, void *)
{ return _ZN10PyramidTop16CleanupResourcesEv(s); }
static int __fastcall pt_top_behavior(void *s, void *)
{ return _ZN10PyramidTop8BehaviorEv(s); }
static int __fastcall pt_top_render(void *s, void *)
{ port_actor_render_probe("PYRAMID_TOP", (char *)s + 0x320);
  return _ZN10PyramidTop6RenderEv(s); }
static int __fastcall pt_top_d1(void *s, void *)
{ return (int)(size_t)_ZN10PyramidTopD1Ev((int *)s); }
static int __fastcall pt_top_d0(void *s, void *)
{ return (int)(size_t)_ZN10PyramidTopD0Ev((int *)s); }

extern "C" void hal_fill_pyramid_top_vtable(void)
{
    port_ov24_bringup();
    void *volatile *vt = (void *volatile *)_ZTV10PyramidTop;
    ov24_fill_shared(vt);
    vt[0]  = (void *)pt_top_init;
    vt[3]  = (void *)pt_top_clean;
    vt[6]  = (void *)pt_top_behavior;
    vt[9]  = (void *)pt_top_render;
    vt[16] = (void *)pt_top_d1;
    vt[17] = (void *)pt_top_d0;
    vt[31] = (void *)ov24_kill;
}

// ============================================================================
// PYRAMID_TAG (86) -- table 0x02113844, 31 slots, RTTI 21daObjDlPyramidDummy_c.
// ============================================================================
//
// The four invisible trigger cylinders that drive the top. 268-byte ACTOR with
// a MovingCylinderClsn at +0xd4 and no model at all: no InitResources file
// load, no Cleanup and no Render, which is why slots 3 and 9 are the base
// bodies and this fill writes only 0/6/16/17. InitResources finds actor id
// 0x55 (85, the top), keeps its unique id in +0x108 and sizes the cylinder;
// Behavior clears and updates the cylinder every frame until +0xf8 is set, at
// which point it bumps the top's +0x3b6 counter and marks itself for
// destruction. If there is no id-85 actor it destroys itself on frame one --
// which is why registering 86 without 85 would be a silent no-op rather than
// a fault, and why the two land together.
static int __fastcall pt_tag_init(void *s, void *)
{ return _ZN10PyramidTag13InitResourcesEv(s); }
static int __fastcall pt_tag_behavior(void *s, void *)
{ return _ZN10PyramidTag8BehaviorEv(s); }
/* Slot 16, HELD OUT of the slice and transcribed; see the slice file's header
   for the disassembly and the receiver-shape reason. */
/* slot 16 is the matched src D1 through hal/dtor_faces_cpp.cpp (lane DTOR-FACES-CPP);
   the transcribed thunk that stood here (pt_tag_d1) spelled the same chain by hand. */
static int __fastcall pt_tag_d0(void *s, void *)
{ return (int)(size_t)_ZN10PyramidTagD0Ev((int *)s); }
/* The two base slots this class carries in its OWN table rather than an
   override -- filled explicitly so the 31-slot array is complete. */
static int __fastcall pt_tag_clean(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::CleanupResources(); }
static int __fastcall pt_tag_render(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::Render(); }

extern "C" void hal_fill_pyramid_tag_vtable(void)
{
    port_ov24_bringup();
    void *volatile *vt = (void *volatile *)_ZTV10PyramidTag;
    ov24_fill_shared(vt);
    vt[0]  = (void *)pt_tag_init;
    vt[3]  = (void *)pt_tag_clean;
    vt[6]  = (void *)pt_tag_behavior;
    vt[9]  = (void *)pt_tag_render;
    vt[16] = (void *)hal_cppd1_PyramidTag;
    vt[17] = (void *)pt_tag_d0;
    /* NO slot 31: this table is 31 slots and the array is 31 long. Writing one
       would run past its own storage. */
}
