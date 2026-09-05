// RUN LINKW WAVE 15 (lane w15): OV025's TWO PYRAMID CLASSES (level 17).
//
// Level 17 spawns 83 actors and skips 11. FIVE of the eleven are these two.
// ov025 is the level's own overlay and has been whole-mounted since lane
// w8-mounts; this is its second, per-symbol mount, the ov045/ov052/ov056 dual
// shape.
//
//   id  class          x on L17  table                             width
//   87  PYRAMID_STEP   4         0x02113914 _ZTV11PyramidStep        32
//   88  PYRAMID_LIFT   1         0x021139d4 _ZTV11PyramidLift        32
//
// Both attribution routes agree on both ids and the identities come from the
// RTTI typeinfo at vtable[-1] -- "14daObjDpBrock_c" and "10daDpLift_c", DP
// being the level's Dry Pyramid prefix -- not from dsd labels. The full
// derivation is in port/ov025_syms.txt.
//
// ---- BOTH WIDTHS ARE 32 AND NEITHER NAIVE READ GETS THERE ------------------
//
// dsd split BOTH tables at slot 8, so the next-symbol landing reads EIGHT on
// each; id 87's reloc run also overruns by three words into id 88's typeinfo
// record. The pin on both is the slot-31 Platform check -- 29
// Actor::OnAimedAtWithEgg, 30 Actor::OnAimedAtWithEggReturnVec, 31
// _ZN8Platform4KillEv -- the close every 32-slot platform table in the port
// ends on. An eight-slot host array would have left Behavior, Render, the
// D1/D0 pair, the whole Actor interaction list and Platform::Kill off the end
// of storage, all of them dispatched.
//
// ---- THE VTABLES ARE HOST STORAGE ------------------------------------------
//
// Both excluded from the per-symbol mount and declared here as host arrays,
// with their five dsd interior splits. dsd names 0x02113914 TWICE
// (_ZTV11PyramidStep, which the Spawn stores, and _ZTV14daObjDpBrock_c, which
// both destructors restore by), so the RTTI spelling is aliased onto the one
// host array; both names are excluded, so the LHS is undefined everywhere and
// the alias cannot be defeated. id 88's table has only ONE ov025 name, and the
// ov027 spelling its three TUs use is routed by a per-source -D instead --
// port/CMakeLists.txt carries the reason.
//
// ---- ONE TU IS HELD OUT: PyramidStep::CleanupResources ---------------------
//
// src/_ZN11PyramidStep16CleanupResourcesEv.cpp spells its two SharedFilePtrs
// G0 and G1, at file scope in a C++ TU, so they mangle to ?G0@@3PAHA and
// ?G1@@3PAHA -- and hal/cxx_aliases.cpp has already bound those two names to
// SIGNPOST's ov002 model and collision files. Linking it would Release
// SignPost's live SharedFilePtrs on every level-17 teardown: the ov022
// FloatingFloorLllBig failure exactly, not a dying-object write and not
// survivable. THE THUNK BELOW IS INSTRUCTION-VERIFIED against the ROM body
// (0x02111e70, 0x38 bytes), disassembled from
// extracted/overlays/overlay_0025.bin:
//     02111e78  ADD r0, r0, #0x124
//     02111e7c  BL  0x02039140        MeshColliderBase::Disable
//     02111e80  LDR r0, =0x02113ab8   (pool 0x02111ea0)  the MODEL file ptr
//     02111e84  BL  0x02017b64        SharedFilePtr::Release
//     02111e88  LDR r0, =0x02113ab0   (pool 0x02111ea4)  the CLSN file ptr
//     02111e8c  BL  0x02017b64        SharedFilePtr::Release
//     02111e90  MOV r0, #1  /  return
// The Disable is UNCONDITIONAL in the ROM -- there is no IsEnabled guard here,
// unlike ov022's and ov056's -- and the 0x02113ab8-then-0x02113ab0 order is
// confirmed independently by __sinit_ov025_02112a44, which constructs
// 0x02113ab8 with the MODEL destructor func_02017ab4 and 0x02113ab0 with the
// collision one. A statement-for-statement transcription, not a
// re-derivation. Every other body of both classes serves from src.
//
// ---- WHY THE MOUNT BRING-UP IS IN THIS FILE --------------------------------
//
// The ov045/ov052/ov056/ov073 lane-ownership pattern: no lane owns
// hal/actor_overlays.cpp in this wave, so the bring-up rides the first registry
// fill behind one done-guard (both fills call it). It runs the pack check, the
// syms patch, hal_fill_platform_vtable (both classes' destructors install
// ov002 0x0210ae38 as the base vptr on the way out, and that array is plain
// zeroed storage until the fill runs) and then the TWO ov025 sinits these
// classes own. The overlay has four; 02112970 and 021129dc belong to
// PyramidTag/PyramidTop, which this lane does not register, and running a
// sinit for a class with no reach is the gate-6 mistake -- so they stay
// unrun, the ov019 rule.
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
/* the arm9 shared half, the same words ov045's six platform tables, ov052's
   two, ov056's one and ov073's two carry -- checked slot for slot against both
   ROM tables before this fill was written. */
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

/* the two the held-out CleanupResources thunk calls, both already linked */
int _ZN16MeshColliderBase7DisableEv(void *self);
void _ZN13SharedFilePtr7ReleaseEv(void *self);
/* ...and the two SharedFilePtrs it releases, from the ov025 per-symbol mount */
extern int data_ov025_02113ab8[];   /* model file 1503 */
extern int data_ov025_02113ab0[];   /* collision file 1504 */

const char *port_actor_class_name(unsigned id);   /* hal/actor_registry */
void port_actor_slot_decline(const char *what);   /* func_02043fdc_hostcopy.cpp */
void port_actor_render_probe(const char *cls, void *model); /* hal/actor_classes */
void hal_fill_platform_vtable(void);              /* hal/actor_classes.cpp */

/* the generated ov025 per-symbol mount (build/port/host-src/ov025_syms.c) */
void port_ov025_pack_check(void);
void port_ov025_syms_patch(void);
/* the two sinits these two classes own; the overlay's other two belong to
   PyramidTag/PyramidTop and stay unrun. */
void __sinit_ov025_02112a44(void);   /* id 87's files 1503 + 1504 */
void __sinit_ov025_02112aac(void);   /* id 88's files 1505 + 1506 */

/* the class bodies src spells with C names (port/slice_w15c.txt) */
int *_ZN11PyramidStepD1Ev(int *self);                   /* 87 slot 16 */
int *_ZN11PyramidStepD0Ev(int *self);                   /* 87 slot 17 */
void *PyramidStep_Spawn(void);
int _ZN11PyramidLift16CleanupResourcesEv(void);         /* 88 slot 3  */
void *_ZN11PyramidLiftD1Ev(void *self);                 /* 88 slot 16 */
int *_ZN11PyramidLiftD0Ev(void *self);                  /* 88 slot 17 */
void *PyramidLift_Spawn(void);

DSSTATE_BEGIN
void *_ZTV11PyramidStep[32];
void *_ZTV11PyramidLift[32];
DSSTATE_END
}

/* The RTTI spelling id 87's destructors restore their table by. Both names of
   0x02113914 are excluded from the mount, so this LHS is undefined everywhere
   and the alias cannot be defeated. id 88's table needs no alias: its second
   spelling is another OVERLAY's name and is routed by a per-source -D. */
#pragma comment(linker, "/alternatename:__ZTV14daObjDpBrock_c=__ZTV11PyramidStep")

/* FOUR C++-MANGLED DATA SPELLINGS, the data_02082128 / data_020a0e68 /
   ov052 data_ov052_021124d4 precedent. src/_ZN11PyramidLift13InitResourcesEv
   .cpp declares its two SharedFilePtrs, ov002's second model file and its
   CLPS_Block at FILE SCOPE -- above the file's own extern "C" block rather
   than inside it -- so MSVC decorates all four with their struct types. The
   mounts emit the one C-named array each. Its sibling
   src/_ZN11PyramidLift16CleanupResourcesEv.c reaches two of the same objects
   through the plain C names, which is what these resolve to. All four LHS are
   declared and never defined, so alternatename_guard stays clean. */
#pragma comment(linker, "/alternatename:?data_ov025_02113ae0@@3USharedFilePtr@@A=_data_ov025_02113ae0")
#pragma comment(linker, "/alternatename:?data_ov025_02113ad8@@3USharedFilePtr@@A=_data_ov025_02113ad8")
#pragma comment(linker, "/alternatename:?data_ov002_0210d9f0@@3USharedFilePtr@@A=_data_ov002_0210d9f0")
#pragma comment(linker, "/alternatename:?data_ov025_02112d08@@3UCLPS_Block@@A=_data_ov025_02112d08")

/* The six bodies src defines as real C++ methods rather than extern-"C" free
   functions, three per class. */
#include "PyramidStep.h"
#include "PyramidLift.h"
extern "C" {
int _ZN11PyramidStep13InitResourcesEv(void *self)
{ return ((PyramidStep *)self)->PyramidStep::InitResources(); }
int _ZN11PyramidStep8BehaviorEv(void *self)
{ return ((PyramidStep *)self)->PyramidStep::Behavior(); }
int _ZN11PyramidStep6RenderEv(void *self)
{ return ((PyramidStep *)self)->PyramidStep::Render(); }
int _ZN11PyramidLift13InitResourcesEv(void *self)
{ return ((PyramidLift *)self)->PyramidLift::InitResources(); }
int _ZN11PyramidLift8BehaviorEv(void *self)
{ return ((PyramidLift *)self)->PyramidLift::Behavior(); }
int _ZN11PyramidLift6RenderEv(void *self)
{ return ((PyramidLift *)self)->PyramidLift::Render(); }
}

// ---- the trap --------------------------------------------------------------
static void ov25_trap_report(void *self, int slot)
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
#define OV25_TRAP(n) \
    static int __fastcall ov25_trap##n(void *s, void *) \
    { ov25_trap_report(s, n); return 0; }
OV25_TRAP(13) OV25_TRAP(14)
#undef OV25_TRAP

static int __fastcall ov25_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall ov25_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall ov25_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall ov25_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall ov25_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall ov25_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall ov25_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall ov25_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall ov25_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall ov25_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall ov25_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
/* slot 19 takes the three-parameter shape so it emits `ret 4`: the dispatch
   site pushes the Player the callee pops -- the wf_turn_egg contract. */
static int __fastcall ov25_turn_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall ov25_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall ov25_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall ov25_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall ov25_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall ov25_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall ov25_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall ov25_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall ov25_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall ov25_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall ov25_egg(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }
/* slot 31, the Platform tail, the word both widths are pinned by. */
static int __fastcall ov25_kill(void *s, void *)
{ _ZN8Platform4KillEv(s); return 0; }

/* The shared half of both tables. The caller writes its own 0/3/6/9/16/17
   and 31. */
static void ov25_fill_shared(void *volatile *vt)
{
    vt[1]  = (void *)ov25_binit;
    vt[2]  = (void *)ov25_ainit;
    vt[4]  = (void *)ov25_bclean;
    vt[5]  = (void *)ov25_aclean;
    vt[7]  = (void *)ov25_bbeh;
    vt[8]  = (void *)ov25_abeh;
    vt[10] = (void *)ov25_bren;
    vt[11] = (void *)ov25_aren;
    vt[12] = (void *)ov25_pdes;
    vt[13] = (void *)ov25_trap13;
    vt[14] = (void *)ov25_trap14;
    vt[15] = (void *)ov25_heap;
    vt[18] = (void *)ov25_yoshi;
    vt[19] = (void *)ov25_turn_egg;
    vt[20] = (void *)ov25_v50;
    vt[21] = (void *)ov25_pounded;
    vt[22] = (void *)ov25_atk1;
    vt[23] = (void *)ov25_atk2;
    vt[24] = (void *)ov25_kicked;
    vt[25] = (void *)ov25_pushed;
    vt[26] = (void *)ov25_cannon;
    vt[27] = (void *)ov25_mega;
    vt[28] = (void *)ov25_under;
    vt[29] = (void *)ov25_egg;
    vt[30] = (void *)port_actor_s30_base;
}

// ---- the mount bring-up ----------------------------------------------------
/* CAPTURED, and the argument is hal/level_boot.cpp's on g_level_mounted: this
   flag says "port_ov25_bringup has run", and everything that pass writes --
   the mount's rebased pointers and the SharedFilePtrs its static initialisers
   construct -- lives in .dsstate. A restore rolls that back. A guard that does
   not roll back with it leaves the pass skipped forever and the overlay
   holding raw DS pointers, which is the defect behind both of the RELOAD
   review's referrals. Bracketed, the pass re-runs exactly when its results
   were rolled away. */
DSSTATE_BEGIN
static int g_ov25_bringup_done;
DSSTATE_END

extern "C" void port_ov25_bringup(void)
{
    if (g_ov25_bringup_done)
        return;
    g_ov25_bringup_done = 1;
    port_ov025_pack_check();
    port_ov025_syms_patch();
    /* before the sinits and before any teardown: both classes' destructors
       install ov002 0x0210ae38 as the base vptr on the way out, and that array
       is plain zeroed storage until this fill runs. */
    hal_fill_platform_vtable();
    __sinit_ov025_02112a44();
    __sinit_ov025_02112aac();
}

// ============================================================================
// PYRAMID_STEP (87) -- table 0x02113914 == _ZTV14daObjDpBrock_c
// ============================================================================
//
// 932-byte object built by Platform's constructor; MovingMeshCollider at
// +0x124 and a SECOND Model at +0x320 (the only one it has -- there is nothing
// at +0xd4). Its InitResources reads the low two bits of the spawn param to
// pick one of three starting offsets and phases, and its Behavior is a
// two-state up/down counter on +0x370/+0x372 that flips direction every 0x64
// frames, rebuilding the collider transform whenever Platform::IsClsnInRange
// says the player is near. Its files are 1503 (model) and 1504 (collision).
static int __fastcall ps_init(void *s, void *)
{ return _ZN11PyramidStep13InitResourcesEv(s); }
/* Slot 3, THE MATCHED TU -- run rel0215 wave 3 (lane w3-e) retired the
   transcription this was. The ruling that held it out stands: the body spells
   its two SharedFilePtrs as the shared placeholders G0/G1, and
   hal/cxx_aliases.cpp binds those to the game heap and to SignPost's ov002
   file pointers, so linking it as written Released live pointers. What changed
   is the binding. Its own pool names the two objects --
   0x02111ea0 -> 0x02113ab8 and 0x02111ea4 -> 0x02113ab0, both
   module:overlay(25), both already mounted above -- and neither is declared in
   include/decl_common.h, so the per-source -D compiles here. (The identical
   rename on ov022's LAVA_PLANK body does NOT compile, because ITS targets are
   in that header; the W4/W9 blocks in port/CMakeLists.txt carry both halves of
   the test.) The transcription below is what proved the two addresses.
     _ZN16MeshColliderBase7DisableEv(this + 0x124);
     SharedFilePtr::Release(0x02113ab8);  SharedFilePtr::Release(0x02113ab0); */
static int __fastcall ps_clean(void *s, void *)
{ return ((PyramidStep *)s)->PyramidStep::CleanupResources(); }
static int __fastcall ps_behavior(void *s, void *)
{ return _ZN11PyramidStep8BehaviorEv(s); }
static int __fastcall ps_render(void *s, void *)
{ port_actor_render_probe("PYRAMID_STEP", (char *)s + 0x320);
  return _ZN11PyramidStep6RenderEv(s); }
static int __fastcall ps_d1(void *s, void *)
{ return (int)(size_t)_ZN11PyramidStepD1Ev((int *)s); }
static int __fastcall ps_d0(void *s, void *)
{ return (int)(size_t)_ZN11PyramidStepD0Ev((int *)s); }

extern "C" void hal_fill_pyramid_step_vtable(void)
{
    port_ov25_bringup();
    /* THE POINTER IS VOLATILE ON PURPOSE -- the gate-200 elided-stores bug,
       repro in hal/actor_classes_ov002g200.cpp. */
    void *volatile *vt = (void *volatile *)_ZTV11PyramidStep;
    ov25_fill_shared(vt);
    vt[0]  = (void *)ps_init;
    vt[3]  = (void *)ps_clean;
    vt[6]  = (void *)ps_behavior;
    vt[9]  = (void *)ps_render;
    vt[16] = (void *)ps_d1;
    vt[17] = (void *)ps_d0;
    vt[31] = (void *)ov25_kill;
}

// ============================================================================
// PYRAMID_LIFT (88) -- table 0x021139d4
// ============================================================================
//
// 1020-byte object; Model at +0xd4, MovingMeshCollider at +0x124, a second
// Model at +0x320 and a TEN-ELEMENT array of 12-byte position records at
// +0x37c that its factory builds with func_020733a8 and its destructors tear
// down with __destroy_arr. Its Render draws the first model once and then the
// second model once per array element, translating between draws. Its files
// are 1505 (model) and 1506 (collision), plus ov002's data_ov002_0210d9f0 for
// the second model. func_ov025_021125dc is the collision callback its
// InitResources hands func_020393c4; it forwards to func_ov025_021125bc, which
// sets +0x3f7 when the colliding actor's id reads 0xbf (the Player).
static int __fastcall pl_init(void *s, void *)
{ return _ZN11PyramidLift13InitResourcesEv(s); }
static int __fastcall pl_clean(void *s, void *)
{ return _ZN11PyramidLift16CleanupResourcesEv(); }
static int __fastcall pl_behavior(void *s, void *)
{ return _ZN11PyramidLift8BehaviorEv(s); }
static int __fastcall pl_render(void *s, void *)
{ port_actor_render_probe("PYRAMID_LIFT", (char *)s + 0xd4);
  return _ZN11PyramidLift6RenderEv(s); }
static int __fastcall pl_d1(void *s, void *)
{ return (int)(size_t)_ZN11PyramidLiftD1Ev(s); }
static int __fastcall pl_d0(void *s, void *)
{ return (int)(size_t)_ZN11PyramidLiftD0Ev(s); }

extern "C" void hal_fill_pyramid_lift_vtable(void)
{
    port_ov25_bringup();
    void *volatile *vt = (void *volatile *)_ZTV11PyramidLift;
    ov25_fill_shared(vt);
    vt[0]  = (void *)pl_init;
    vt[3]  = (void *)pl_clean;
    vt[6]  = (void *)pl_behavior;
    vt[9]  = (void *)pl_render;
    vt[16] = (void *)pl_d1;
    vt[17] = (void *)pl_d0;
    vt[31] = (void *)ov25_kill;
}
