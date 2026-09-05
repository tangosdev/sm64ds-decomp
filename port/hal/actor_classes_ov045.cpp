// RUN LINKW WAVE 4: BOWSER IN THE FIRE SEA'S PLATFORM CAST (ov045, level 37).
// (WAVE 5, lane w5-c: all SIX hosted -- the sqrt unit id 141 was waiting on
// is live, so its section below is the real fill now, and the twenty
// level-37 platform instances are all served.)
//
// Six classes measured, all six hosted (141 landed a wave after the other
// five). Every own vtable slot of all six is backed by matched src; 141's
// wave-4 holdback and its wave-5 release are its own section. Same law as
// hal/actor_classes_wf.cpp, which is the exemplar this file is built on --
// another level overlay's platform cast, another one-class naming shift, the
// same 32-slot Platform tables and the same ov002/ov098 inherited slots.
//
// ---- THE NAMING SHIFT, and the five things that settle it -----------------
//
// dsd lays each mwcc class block out as [bodies..., Spawn] and then names the
// block as if it STARTED at the Spawn, so a body spelled _ZN15FireSeaElevator*
// sits in the block AFTER FireSeaElevator_Spawn, which is PoleLift's. The
// _ZTV names are shifted the same way. The Spawn names are the correct ones.
//
//   id   TRUE class          table       its bodies are spelled     x on L37
//   140  FireSeaElevator     0x02112cf4  func_ov045_021111a0..      x1
//   142  PoleLift            0x02112dbc  _ZN15FireSeaElevator*      x4
//   148  ExtendingPlatform   0x02112e80  _ZN8PoleLift*              x1
//   145  FloatingFloorBfs    0x02112f50  func_ov045_02111b14..      x3
//   141  TiltingPlatformBfs  0x02113020  _ZN16FloatingFloorBfs*     x2
//   139  FallBlockBfs        0x021130f4  _ZN12FallBlockBfs*         x9
//
// FallBlockBfs is the one class whose names are NOT shifted (it is last in
// address order, so its block and its label coincide). "Everything is off by
// one" is wrong; use the table.
//
// Five independent reads agree on all six rows, so this is a measurement:
//   (1) the arm9 SPAWN TABLE, 0x02090864 + id*4, points at each SpawnInfo
//   (2) each SpawnInfo's raw word[1] low half IS the id (raw bytes out of
//       extracted/overlays/overlay_0045.bin -- ov045 is compressed, so the dsd
//       export is the wrong image to read halfwords from)
//   (3) each SpawnInfo's word[0] is that class's Spawn, inside ov045
//   (4) each table's slot 0 lands inside that class's own code block
//   (5) each *_Spawn's own vtable-store reloc names that class's table
// The two RTTI names side with the spawn names and against the _ZTV and method
// names: 0x02112e80 is also _ZTV17daObjKm2_Nobiru_c (nobiru, the one that
// stretches) and its spawn is the one that constructs the
// ExtendingMeshCollider; 0x02112dbc is also _ZTV18daObjKm2_Ami_Bou_c, the pole.
//
// ---- THE WIDTH: THIRTY-TWO, not thirty-one --------------------------------
//
// Five of the six tables are 32 slots wide; only ExtendingPlatform's is 31.
// Four of the wide ones carry Platform::Kill (ov002 0x020ee55c) at slot 31
// (ids 140/141/142/145); FallBlockBfs's slot 31 is func_ov098_0213a17c, per
// its own inherited-slots block below. This
// is the bug hal/actor_classes_wf.cpp's header describes ("31 because Platform
// WAS THE BUG" -- a [31] array left slot 31 reading past the end and took the
// process). Each width here is pinned twice: by the reloc run, and by the
// address of the next dsd data symbol, which lands exactly on the end of every
// one of the six tables.
//
// ---- SLOTS THAT ARE INHERITED BUT NOT arm9 --------------------------------
//
// Three of the six take their Behavior and Render from a base class in ANOTHER
// overlay, so a fill that only writes 0/3/6/9/16/17 out of ov045 would leave
// them null:
//   141  slot 6 func_ov002_020b616c   slot 9 func_ov002_020b6144
//   145  slot 6 func_ov002_020b6494   slot 9 func_ov002_020b646c
//   139  slot 6 func_ov098_0213a36c   slot 9 func_ov098_0213a314
//        slot 27 func_ov098_0213a284  slot 31 func_ov098_0213a17c
// 139's four are the SAME bodies FALL_BLOCK_WF already seats (see
// hal/actor_classes_wf.cpp's gate-63 block, which also settles that the
// "same-id chain faults on a null" worry is a phantom); all four were already
// in walk_window.map, so 139's inherited half costs no new TU. 145's two ov002
// bodies and their two Init/Cleanup veneer targets are new and ride this lane's
// slice. 141's two are what its section below is about.
//
// ---- THE VTABLES ARE HOST STORAGE -----------------------------------------
//
// All six are excluded from the ov045 per-symbol mount (port/ov045_syms.txt) and
// declared here as host arrays -- the ov015/ov016/ov080 rule: a mounted vtable
// hands a factory DS code addresses. Two are unnamed data symbols dsd left as
// plain data (0x02112cf4 and 0x02112f50); the other four carry a _ZTV name. Each
// RTTI name the address also answers to is aliased onto the one host array,
// because the destructors restore their table by the RTTI spelling.
//
// ---- THREE PLACES THE RECOVERED SOURCE CANNOT BE CALLED AS WRITTEN --------
//
// 1. THE `p[0] = (int)VT1` FACTORIES. FloatingFloorBfs_Spawn,
//    TiltingPlatformBfs_Spawn and FallBlockBfs_Spawn each end with a store of
//    the shared-header VT1 placeholder, which is hal/auto_bss.cpp's zeroed [8].
//    Disassembling all three settles what the ROM does and which store is which:
//
//      02111c0c  LDR r1, =0x0210912c   <- the ov002 BASE table
//      02111c10  LDR r0, =0x02112f50   <- FloatingFloorBfs' OWN table
//      02111c14  STR r1, [r4, #0]
//      02111c18  STR r0, [r4, #0]
//
//    base first, own second -- so in `p[0] = _ZTV19daObjKm2_Ukishima_c;
//    p[0] = VT1;` the RTTI name is on the BASE store and VT1 is on the OWN one.
//    A raw spawn therefore leaves a live object dispatching through a zeroed
//    array. The host factories below call the real Spawn and then reseat p[0]
//    onto the class's host table, which is the ROM's final value and exactly
//    the recipe port_factory_rotating_platform_wf uses for the one ov015 factory
//    with the same shape. The other three factories store their table by a real
//    name and are registered directly.
//
// 2. _ZN8PoleLift16CleanupResourcesEv (id 148, slot 3) SPELLS ITS TWO
//    SharedFilePtrs G0/G1, and hal/cxx_aliases.cpp has already aliased
//    ?G0@@3PAHA / ?G1@@3PAHA onto SignPost's ov002 pointers -- single global
//    names, the caveat that file states outright. relocs.txt says the ROM body
//    releases 0x021131d8 then 0x021131d0. Linking this TU would Release
//    SignPost's LIVE SharedFilePtrs on every level-37 teardown, which is not a
//    dying-object write and not survivable. It is the ONE ov045 body this lane
//    replaces with a host thunk (ep_clean below) and the one TU held out of the
//    slice.
//
// 3. _ZN8PoleLiftD1Ev is a real MSVC destructor over structs declared inside its
//    own .cpp, so there is no C name to call and no face to write. ep_d1 below
//    is the chain its sibling D0 spells and relocs.txt confirms: store
//    0x02112e80, ExtendingMeshCollider::~ (this+0x158), Model::~ (this+0xd8),
//    Actor::D2. The gate-199/200/202 treatment.
//
// Every OTHER placeholder store in this cast is a DYING-OBJECT write. The D1/D0
// bodies spell their intermediate base-table restores VT0/VT1/VT2, and
// hal/actor_vtables.cpp keeps those as zeroed storage precisely because they are
// "installed transiently during teardown and never dispatched" -- and here the
// stores sit between two direct calls with no dispatch in between. That is the
// same reading hal/actor_classes_wf.cpp records for FALL_BLOCK_WF's D1/D0, so
// those ten TUs are called directly and linked. `G0` in the six D0 bodies is
// the game heap and is already aliased to _data_020a0eac, which is the word
// relocs.txt has at each of those six literal-pool slots.
//
// ---- LANE OWNERSHIP: WHY THE MOUNT BRING-UP IS IN THIS FILE ---------------
//
// Every other dual-mounted overlay calls port_ovNNN_pack_check /
// port_ovNNN_syms_patch and its sinits from port_actor_overlays_sinits() in
// hal/actor_overlays.cpp. That is the right home and this file is not it; no
// lane owns actor_overlays.cpp in this wave, so the bring-up rides the first
// registry fill instead (ov45_bringup, one done-guard, every fill calls it).
//
// The ordering that makes it safe was measured, not assumed:
//   port_actor_overlays_sinits() runs to completion BEFORE
//   port_actor_registry_install() (hal/level_boot.cpp), so nothing else's sinit
//   can be waiting on ov045's;
//   the registry writes SpawnInfo+0 = host factory and then calls fill, and the
//   generated port_ov045_syms_patch() writes SpawnInfo + 32 and nothing else
//   (build/port/host-src/ov045_syms.c -- the +0 word holds a CODE address, which
//   is not inside the mount's own coverage, so the pointer pass cannot touch
//   it), so a patch that runs after the factory word is written does not undo
//   it;
//   the SharedFilePtrs the sinits build are read by InitResources, which runs at
//   spawn, after registration.
// THE HANDOFF: whoever next owns hal/actor_overlays.cpp should move the body of
// ov45_bringup into port_actor_overlays_sinits beside the ov013 block and cut
// the guard here to a call. The exact block is in this lane's final report.
//
// ---- 0x02112280: THE SINIT THAT CANNOT BE LINKED --------------------------
//
// ov045 has six static initialisers and five of them link. __sinit_ov045_
// 02112280 does not, and the reason is in port/slice_w1l4.txt: three src TUs
// spell that one name with three different types at a shared window address, and
// hal/actor_classes_ov013.cpp resolves ov013's half with an /alternatename that
// fires only while the name is UNDEFINED. Defining it hands ov013's
// CLOCK_PENDULUM an ov045 function address where its SharedFilePtr should be, on
// level 5. The exclusion is forced and it is a standing decision.
//
// That sinit is the one that builds EXTENDING_PLATFORM's two SharedFilePtrs, so
// this file HOSTS THE ONE INITIALISATION BY HAND (ov45_sinit_02112280 below) --
// the gate-17 recipe slice_w1l4.txt names for exactly this case. It is a
// transcription of src/__sinit_ov045_02112280.c, four calls, and the two file
// ids in it (1635 model, 1636 clsn) are the ROM's own, read from that body.
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

/* the generated ov045 per-symbol mount (build/host-src/ov045_syms.c) */
void port_ov045_pack_check(void);
void port_ov045_syms_patch(void);
/* five of the six ov045 sinits; 0x02112280 is hosted by hand below */
void __sinit_ov045_021121a8(void);
void __sinit_ov045_02112214(void);
void __sinit_ov045_021122ec(void);
void __sinit_ov045_02112358(void);
void __sinit_ov045_021123c0(void);
/* what a sinit is made of, for the one this file has to host itself */
void func_02017acc(void *sfp, int fileId);           /* SharedFilePtr, model */
void func_02017b4c(void *sfp, int fileId);           /* SharedFilePtr, clsn */
void func_020731dc(void *sfp, void *dtor, void *node);  /* chain the teardown */
extern int func_02017ab4[];                          /* the model dtor */
extern int SharedFilePtr_Destruct_Clsn[];            /* the clsn dtor */
extern int data_ov045_021131d0[], data_ov045_021131d8[];   /* id 148's pair */
extern int data_ov045_021131e0[], data_ov045_021131ec[];   /* ...their nodes */

/* the EMC seat this cast's ExtendingPlatform constructor needs */
void hal_seat_extending_mesh_collider_vtable(void);
}

// ---- the trap --------------------------------------------------------------
static void ov45_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: ov045 vtable slot %d is not hosted (actor id %u "
                 "%s)\n", slot, id, port_actor_class_name(id));
    { static char _m[128];
      std::snprintf(_m, sizeof _m, "unhosted ov045 vtable slot %d on id %u %s",
                    slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
#define OV45_TRAP(n) \
    static int __fastcall ov45_trap##n(void *s, void *) \
    { ov45_trap_report(s, n); return 0; }
OV45_TRAP(13) OV45_TRAP(14) OV45_TRAP(17)
#undef OV45_TRAP

static int __fastcall ov45_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall ov45_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall ov45_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall ov45_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall ov45_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall ov45_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall ov45_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall ov45_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall ov45_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall ov45_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall ov45_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
/* slot 19 takes the three-parameter shape so it emits `ret 4`: the dispatch
   site pushes the Player the callee pops -- the wf_turn_egg contract. */
static int __fastcall ov45_turn_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall ov45_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall ov45_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall ov45_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall ov45_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall ov45_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall ov45_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall ov45_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall ov45_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall ov45_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall ov45_egg(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }
/* slot 31, the Platform tail; four of the six ROM tables take it unchanged
   (140/141/142/145) -- 139's slot 31 is func_ov098_0213a17c and 148 has no
   slot 31 at all. Fills 140/141/142/145. */
static int __fastcall ov45_kill(void *s, void *)
{ _ZN8Platform4KillEv(s); return 0; }

/* The shared half, slots 1..30, every word of it read off the six tables'
   relocations (they agree slot for slot outside each class's own overrides).
   The caller writes its own 0/3/6/9/16/17 and its 31.

   Slot 12 is ActorBase::OnPendingDestroy (arm9 0x02043ac0) in five of the six
   tables and is written here; ExtendingPlatform overrides it and writes its own
   after this returns. Slot 17 traps here and all six callers replace it: every
   one of these classes has a real deleting destructor. Slots 13/14 are the
   ActorBase Virtual34/Virtual38 traps and 30 declines, both the wf reading (30's
   ROM body returns a Vector3 by value and the sret contract is unproved).

   THE POINTER IS VOLATILE ON PURPOSE -- the gate-200 elided-stores bug, repro
   and dumpbin proof in hal/actor_classes_ov002g200.cpp: MSVC 19.44 x86 /O2 can
   delete a static filler's stores when it is called with several distinct
   extern-array arguments. This helper has six callers. */
static void ov45_fill_shared(void *volatile *vt)
{
    vt[1]  = (void *)ov45_binit;
    vt[2]  = (void *)ov45_ainit;
    vt[4]  = (void *)ov45_bclean;
    vt[5]  = (void *)ov45_aclean;
    vt[7]  = (void *)ov45_bbeh;
    vt[8]  = (void *)ov45_abeh;
    vt[10] = (void *)ov45_bren;
    vt[11] = (void *)ov45_aren;
    vt[12] = (void *)ov45_pdes;
    vt[13] = (void *)ov45_trap13;
    vt[14] = (void *)ov45_trap14;
    vt[15] = (void *)ov45_heap;
    vt[17] = (void *)ov45_trap17;
    vt[18] = (void *)ov45_yoshi;
    vt[19] = (void *)ov45_turn_egg;
    vt[20] = (void *)ov45_v50;
    vt[21] = (void *)ov45_pounded;
    vt[22] = (void *)ov45_atk1;
    vt[23] = (void *)ov45_atk2;
    vt[24] = (void *)ov45_kicked;
    vt[25] = (void *)ov45_pushed;
    vt[26] = (void *)ov45_cannon;
    vt[27] = (void *)ov45_mega;
    vt[28] = (void *)ov45_under;
    vt[29] = (void *)ov45_egg;
    vt[30] = (void *)port_actor_s30_base;
}

// ---- the mount bring-up ----------------------------------------------------
//
// The one initialisation this file has to host: __sinit_ov045_02112280's body,
// transcribed from src/__sinit_ov045_02112280.c, which cannot be linked (see the
// header). Model file 1635 into data_ov045_021131d8 with func_02017ab4 as its
// destructor and data_ov045_021131e0 as its chain node; collision file 1636 into
// data_ov045_021131d0 with SharedFilePtr_Destruct_Clsn and data_ov045_021131ec.
static void ov45_sinit_02112280(void)
{
    func_02017acc(data_ov045_021131d8, 1635);
    func_020731dc(data_ov045_021131d8, func_02017ab4, data_ov045_021131e0);
    func_02017b4c(data_ov045_021131d0, 1636);
    func_020731dc(data_ov045_021131d0, SharedFilePtr_Destruct_Clsn,
                  data_ov045_021131ec);
}

/* CAPTURED, and the argument is hal/level_boot.cpp's on g_level_mounted: this
   flag says "port_ov45_bringup has run", and everything that pass writes --
   the mount's rebased pointers and the SharedFilePtrs its static initialisers
   construct -- lives in .dsstate. A restore rolls that back. A guard that does
   not roll back with it leaves the pass skipped forever and the overlay
   holding raw DS pointers, which is the defect behind both of the RELOAD
   review's referrals. Bracketed, the pass re-runs exactly when its results
   were rolled away. */
DSSTATE_BEGIN
static int g_ov45_bringup_done;
DSSTATE_END

extern "C" void port_ov45_bringup(void)
{
    if (g_ov45_bringup_done)
        return;
    g_ov45_bringup_done = 1;
    port_ov045_pack_check();
    port_ov045_syms_patch();
    hal_seat_extending_mesh_collider_vtable();
    __sinit_ov045_021121a8();
    __sinit_ov045_02112214();
    ov45_sinit_02112280();          /* the one that cannot be linked */
    __sinit_ov045_021122ec();
    __sinit_ov045_02112358();
    __sinit_ov045_021123c0();
}

// ============================================================================
// FIRE_SEA_ELEVATOR (id 140) -- table 0x02112cf4, the func_ov045_0211* bodies
// ============================================================================
//
// 808-byte object built by Platform's constructor; Model at +0xd4,
// MovingMeshCollider at +0x124. The rising/falling lift: its Behavior is a
// three-case state machine on +0x327 that walks +0x60 up 0x5dc000 and back.
extern "C" {
int func_ov045_021113ec(char *self);   /* slot 0  InitResources */
int func_ov045_0211123c(void *self);   /* slot 3  CleanupResources */
int func_ov045_0211129c(char *self);   /* slot 6  Behavior */
int func_ov045_02111274(void *self);   /* slot 9  Render (extern-C in a .cpp) */
int *func_ov045_021111a0(int *self);   /* slot 16 D1 */
int *func_ov045_021111e4(int *self);   /* slot 17 D0 */
void *FireSeaElevator_Spawn(void);
DSSTATE_BEGIN
void *data_ov045_02112cf4[32];         /* dsd split it at 0x02112d48 */
DSSTATE_END
}
/* func_ov045_021113ec hands func_020393c4 the address of ov045's own
   func_ov045_021114c8 under ov059's spelling -- another module's dsd export won
   the naming race at 0x021114c8. Both names are the same ov045 body. */
#pragma comment(linker, "/alternatename:_data_ov059_021114c8=_func_ov045_021114c8")
/* the RTTI name the D1 restores; agaru = rises */
#pragma comment(linker, "/alternatename:__ZTV16daObjKm2_Agaru_c=_data_ov045_02112cf4")
static int __fastcall fse_init(void *s, void *)
{ return func_ov045_021113ec((char *)s); }
static int __fastcall fse_clean(void *s, void *)
{ return func_ov045_0211123c(s); }
static int __fastcall fse_behavior(void *s, void *)
{ return func_ov045_0211129c((char *)s); }
static int __fastcall fse_render(void *s, void *)
{ port_actor_render_probe("FIRE_SEA_ELEVATOR", (char *)s + 0xd4);
  return func_ov045_02111274(s); }
static int __fastcall fse_d1(void *s, void *)
{ return (int)(size_t)func_ov045_021111a0((int *)s); }
static int __fastcall fse_d0(void *s, void *)
{ return (int)(size_t)func_ov045_021111e4((int *)s); }
extern "C" void hal_fill_fire_sea_elevator_vtable(void)
{
    port_ov45_bringup();
    void *volatile *vt = (void *volatile *)data_ov045_02112cf4;
    ov45_fill_shared(vt);
    vt[0]  = (void *)fse_init;
    vt[3]  = (void *)fse_clean;
    vt[6]  = (void *)fse_behavior;
    vt[9]  = (void *)fse_render;
    vt[16] = (void *)fse_d1;
    vt[17] = (void *)fse_d0;
    vt[31] = (void *)ov45_kill;
}

// ============================================================================
// POLE_LIFT (id 142) -- table 0x02112dbc, the _ZN15FireSeaElevator* bodies
// ============================================================================
//
// 856-byte object; Model at +0xd4, MovingMeshCollider at +0x124 and a
// MovingCylinderClsn at +0x320 that its factory constructs and its destructors
// tear down first. Four of them on level 37. Init/Clean/Behavior/Render are all
// real C++ methods against FireSeaElevator.h, faced at the bottom of this file.
extern "C" {
int _ZN15FireSeaElevator13InitResourcesEv(void *self);     /* slot 0  */
int _ZN15FireSeaElevator16CleanupResourcesEv(void *self);  /* slot 3  */
int _ZN15FireSeaElevator8BehaviorEv(void *self);           /* slot 6  */
int _ZN15FireSeaElevator6RenderEv(void *self);             /* slot 9  */
int *_ZN15FireSeaElevatorD1Ev(int *self);                  /* slot 16 */
int *_ZN15FireSeaElevatorD0Ev(int *self);                  /* slot 17 */
void *PoleLift_Spawn(void);
DSSTATE_BEGIN
void *_ZTV15FireSeaElevator[32];
DSSTATE_END
}
/* 0x02112dbc answers to both names; the D1/D0 restore it by the RTTI one
   (ami_bou = the net pole). */
#pragma comment(linker, "/alternatename:__ZTV18daObjKm2_Ami_Bou_c=__ZTV15FireSeaElevator")
static int __fastcall pl_init(void *s, void *)
{ return _ZN15FireSeaElevator13InitResourcesEv(s); }
static int __fastcall pl_clean(void *s, void *)
{ return _ZN15FireSeaElevator16CleanupResourcesEv(s); }
static int __fastcall pl_behavior(void *s, void *)
{ return _ZN15FireSeaElevator8BehaviorEv(s); }
static int __fastcall pl_render(void *s, void *)
{ port_actor_render_probe("POLE_LIFT", (char *)s + 0xd4);
  return _ZN15FireSeaElevator6RenderEv(s); }
static int __fastcall pl_d1(void *s, void *)
{ return (int)(size_t)_ZN15FireSeaElevatorD1Ev((int *)s); }
static int __fastcall pl_d0(void *s, void *)
{ return (int)(size_t)_ZN15FireSeaElevatorD0Ev((int *)s); }
extern "C" void hal_fill_pole_lift_vtable(void)
{
    port_ov45_bringup();
    void *volatile *vt = (void *volatile *)_ZTV15FireSeaElevator;
    ov45_fill_shared(vt);
    vt[0]  = (void *)pl_init;
    vt[3]  = (void *)pl_clean;
    vt[6]  = (void *)pl_behavior;
    vt[9]  = (void *)pl_render;
    vt[16] = (void *)pl_d1;
    vt[17] = (void *)pl_d0;
    vt[31] = (void *)ov45_kill;
}

// ============================================================================
// EXTENDING_PLATFORM (id 148) -- table 0x02112e80, the _ZN8PoleLift* bodies
// ============================================================================
//
// THE ONE 31-SLOT TABLE IN THIS FILE. It is also the only class here whose
// factory does not call Platform's constructor: ExtendingPlatform_Spawn runs
// _ZN5ActorC2Ev, then Model's at +0xd8 and _ZN21ExtendingMeshColliderC1Ev at
// +0x158. That EMC constructor is the ROM-wide only caller of the class
// slice_w1l4.txt spent a wave refusing to seat; hal/w2_emc_seat.cpp is the seat
// and ov45_bringup runs it before any of this cast is registered.
//
// 808-byte object, Model at +0xd8 (not +0xd4 -- the EMC subobject moves the
// layout), ExtendingMeshCollider at +0x158. Its Behavior stretches the collider
// between 0x800 and 0x1000 through ExtendingMeshCollider::SetScaleY.
//
// Slot 12 is its own OnPendingDestroy (ov045 0x02111970, a bare `bx lr`), the
// only class in the cast that overrides it; slot 3 is a host thunk and slot 16 a
// host chain, both for the reasons in this file's header.
extern "C" {
int _ZN8PoleLift13InitResourcesEv(void *self);        /* slot 0  */
int _ZN8PoleLift8BehaviorEv(char *self);              /* slot 6, extern-C .cpp */
int _ZN8PoleLift6RenderEv(void *self);                /* slot 9  */
void _ZN8PoleLift16OnPendingDestroyEv(void);          /* slot 12 */
int *_ZN8PoleLiftD0Ev(int *self);                     /* slot 17 */
void *ExtendingPlatform_Spawn(void);
/* what ep_clean and ep_d1 need to spell out by hand */
int _ZN16MeshColliderBase9IsEnabledEv(void *self);
void _ZN16MeshColliderBase7DisableEv(void *self);
void _ZN13SharedFilePtr7ReleaseEv(void *sfp);
void *_ZN21ExtendingMeshColliderD1Ev(void *self);
void _ZN5ModelD1Ev(void *self);
void *_ZN5ActorD2Ev(void *self);
DSSTATE_BEGIN
void *_ZTV8PoleLift[31];
DSSTATE_END
}
/* nobiru = the one that stretches; the D0 restores the table by this name */
#pragma comment(linker, "/alternatename:__ZTV17daObjKm2_Nobiru_c=__ZTV8PoleLift")
static int __fastcall ep_init(void *s, void *)
{ return _ZN8PoleLift13InitResourcesEv(s); }
/* slot 3, HOST THUNK, not the matched TU. src/_ZN8PoleLift16CleanupResourcesEv
   .cpp spells its two SharedFilePtrs G0/G1, which hal/cxx_aliases.cpp has bound
   to SignPost's ov002 pointers; linking it would Release those live. relocs.txt
   (0x02111968/0x0211196c) says the ROM releases 0x021131d8 then 0x021131d0, and
   the body ahead of them is one unconditional MeshColliderBase::Disable
   (func_02039140) on the collider at +0x158, then SharedFilePtr::Release
   (func_02017b64) twice, at 0x02111944/4c/54. The IsEnabled guard below is
   behavior-identical: Disable's own body no-ops on a disabled collider
   (slotIdx 0x18 is exactly IsEnabled's test). */
static int __fastcall ep_clean(void *s, void *)
{
    char *t = (char *)s;
    if (_ZN16MeshColliderBase9IsEnabledEv(t + 0x158))
        _ZN16MeshColliderBase7DisableEv(t + 0x158);
    _ZN13SharedFilePtr7ReleaseEv(data_ov045_021131d8);
    _ZN13SharedFilePtr7ReleaseEv(data_ov045_021131d0);
    return 1;
}
static int __fastcall ep_behavior(void *s, void *)
{ return _ZN8PoleLift8BehaviorEv((char *)s); }
static int __fastcall ep_render(void *s, void *)
{ port_actor_render_probe("EXTENDING_PLATFORM", (char *)s + 0xd8);
  return _ZN8PoleLift6RenderEv(s); }
static int __fastcall ep_pdes(void *s, void *)
{ (void)s; _ZN8PoleLift16OnPendingDestroyEv(); return 0; }
/* slot 16, HOST CHAIN: src/_ZN8PoleLiftD1Ev.cpp is a real MSVC destructor over
   structs local to that TU, so there is no C name to call. The chain is the one
   its D0 sibling spells and relocs.txt confirms at 0x02111854/5c/64/74. */
/* slot 16 is the matched src D1 through hal/dtor_faces_cpp.cpp (lane DTOR-FACES-CPP);
   the transcribed thunk that stood here (ep_d1) spelled the same chain by hand. */
static int __fastcall ep_d0(void *s, void *)
{ return (int)(size_t)_ZN8PoleLiftD0Ev((int *)s); }
extern "C" void hal_fill_extending_platform_vtable(void)
{
    port_ov45_bringup();
    void *volatile *vt = (void *volatile *)_ZTV8PoleLift;
    ov45_fill_shared(vt);
    vt[0]  = (void *)ep_init;
    vt[3]  = (void *)ep_clean;
    vt[6]  = (void *)ep_behavior;
    vt[9]  = (void *)ep_render;
    vt[12] = (void *)ep_pdes;
    vt[16] = (void *)hal_cppd1_PoleLift;
    vt[17] = (void *)ep_d0;
    /* 31 slots: this class is an Actor, not a Platform, and its table really
       does end at 30 -- the next dsd symbol is at 0x02112efc. No slot 31. */
}

// ============================================================================
// FLOATING_FLOOR_BFS (id 145) -- table 0x02112f50, the func_ov045_02111b* bodies
// ============================================================================
//
// 812-byte object; Model at +0xd4, MovingMeshCollider at +0x124. Its own slots
// are only 0/3/16/17 -- Behavior and Render are ov002's shared sinking-platform
// bodies (func_ov002_020b6494 / func_ov002_020b646c) and its Init/Cleanup are
// two-line veneers onto ov002's shared pair through the three-word file table
// data_ov045_02112f08.
extern "C" {
int func_ov045_02111bdc(void *self);   /* slot 0  InitResources veneer */
int func_ov045_02111bc8(void *self);   /* slot 3  CleanupResources veneer */
int func_ov002_020b6494(char *self);   /* slot 6  Behavior  (ov002 base) */
int func_ov002_020b646c(void *self);   /* slot 9  Render    (ov002 base) */
int *func_ov045_02111b14(int *self);   /* slot 16 D1 */
int *func_ov045_02111b64(int *self);   /* slot 17 D0 */
void *FloatingFloorBfs_Spawn(void);
DSSTATE_BEGIN
void *data_ov045_02112f50[32];         /* dsd split it three ways */
DSSTATE_END
}
/* ukishima = floating island. The Spawn spells the ov002 BASE table with this
   name and the D1/D0 spell the OWN one with it (see the header); the alias
   points at the own table, which is what the destructors need, and the factory
   wrapper below fixes the live object. */
#pragma comment(linker, "/alternatename:__ZTV19daObjKm2_Ukishima_c=_data_ov045_02112f50")
/* The two ov002 veneer targets are spelled unprefixed in the ov045 bodies. */
#pragma comment(linker, "/alternatename:_func_020b6584=_func_ov002_020b6584")
#pragma comment(linker, "/alternatename:_func_020b6424=_func_ov002_020b6424")
extern "C" void *port_factory_floating_floor_bfs(void)
{
    void *p = FloatingFloorBfs_Spawn();
    if (p)
        *(void **)p = (void *)data_ov045_02112f50;
    return p;
}
static int __fastcall ff_init(void *s, void *)
{ return func_ov045_02111bdc(s); }
static int __fastcall ff_clean(void *s, void *)
{ return func_ov045_02111bc8(s); }
static int __fastcall ff_behavior(void *s, void *)
{ return func_ov002_020b6494((char *)s); }
static int __fastcall ff_render(void *s, void *)
{ port_actor_render_probe("FLOATING_FLOOR_BFS", (char *)s + 0xd4);
  return func_ov002_020b646c(s); }
static int __fastcall ff_d1(void *s, void *)
{ return (int)(size_t)func_ov045_02111b14((int *)s); }
static int __fastcall ff_d0(void *s, void *)
{ return (int)(size_t)func_ov045_02111b64((int *)s); }
extern "C" void hal_fill_floating_floor_bfs_vtable(void)
{
    port_ov45_bringup();
    void *volatile *vt = (void *volatile *)data_ov045_02112f50;
    ov45_fill_shared(vt);
    vt[0]  = (void *)ff_init;
    vt[3]  = (void *)ff_clean;
    vt[6]  = (void *)ff_behavior;
    vt[9]  = (void *)ff_render;
    vt[16] = (void *)ff_d1;
    vt[17] = (void *)ff_d0;
    vt[31] = (void *)ov45_kill;
}

// ============================================================================
// TILTING_PLATFORM_BFS (id 141) -- table 0x02113020, the _ZN16FloatingFloorBfs*
// bodies. HOSTED IN WAVE 5 (lane w5-c); wave 4 held it back for the one
// port-wide gap its chain crossed, and that gap is closed:
//
//   func_ov002_020b616c  (slot 6) slerps the platform toward the rider's push
//   func_ov002_020b6244  (its base InitResources) installs
//   func_ov002_020b6374  as the collider's beforeClsn callback ->
//   func_ov002_020b62cc  -> Quaternion_FromVector3 -> Quaternion_Normalize
//                        -> func_02053130 -> THE HARDWARE SQRT UNIT.
//
// The unit's four drivers (func_02053008 / func_02053130 / func_020531a4 /
// func_02052fdc) are hostgen NTR_MMIO TUs now, run_sqrt-mediated exactly the
// way gates 8/13 host the divide unit -- ntr/io.cpp's sqrt trigger covers
// both SQRT_PARAM halves (the ROM's drivers write the operand in the HIGH
// half last) and hostgen's MMIO_EXTERN table routes func_02053130's
// role-named SQRTCNT/SQRT_RESULT externs through the proxy. The numeric
// proof against the unit's defined arithmetic is in the commit that landed
// the mechanism.
// ============================================================================
//
// 848-byte object built by Platform's constructor; Model at +0xd4,
// MovingMeshCollider at +0x124 (both straight out of the D1/D0 chains, which
// also settle the member order). Two of them on level 37. Own slots
// 0/3/16/17; its Init and Cleanup are two-line veneers onto ov002's tilting
// base pair (func_020b6244 / func_020b60fc, spelled unprefixed -- the same
// window-race the Ukishima veneers hit, aliased below) through the
// three-word file table data_ov045_02112fdc; Behavior and Render are ov002's
// TILTING pair, and the beforeClsn callback its base Init installs is what
// pushes the tilt toward the rider through the quaternion chain above.
extern "C" {
int _ZN16FloatingFloorBfs13InitResourcesEv(void *self);     /* slot 0  */
int _ZN16FloatingFloorBfs16CleanupResourcesEv(void *self);  /* slot 3  */
int func_ov002_020b616c(char *self);   /* slot 6  Behavior (ov002 base) */
int func_ov002_020b6144(void *self);   /* slot 9  Render   (ov002 base) */
int *_ZN16FloatingFloorBfsD1Ev(int *self);                  /* slot 16 */
int *_ZN16FloatingFloorBfsD0Ev(int *self);                  /* slot 17 */
void *TiltingPlatformBfs_Spawn(void);
DSSTATE_BEGIN
void *_ZTV16FloatingFloorBfs[32];
DSSTATE_END
}
/* gura = the tilting one. The Spawn spells the ov002 BASE table with this
   RTTI name and the D1/D0 open their teardown with it (their FINAL transient
   store is the zeroed VT2, the dying-object reading in this file's header);
   the alias points at the own table, the 145/Ukishima treatment exactly. The
   base-table spelling _ZTV10dBgActor_c and the D0's G0 heap word are already
   defined port-wide (hal/actor_classes.cpp, hal/cxx_aliases.cpp). */
#pragma comment(linker, "/alternatename:__ZTV15daObjKm2_Gura_c=__ZTV16FloatingFloorBfs")
/* The two ov002 veneer targets, spelled unprefixed in the 141 bodies. */
#pragma comment(linker, "/alternatename:_func_020b6244=_func_ov002_020b6244")
#pragma comment(linker, "/alternatename:_func_020b60fc=_func_ov002_020b60fc")
/* The factory ends on `p[0] = Gura; p[0] = VT1;` -- base store then the
   zeroed placeholder, trap (F). The wrapper reseats the ROM's final value. */
extern "C" void *port_factory_tilting_platform_bfs(void)
{
    void *p = TiltingPlatformBfs_Spawn();
    if (p)
        *(void **)p = (void *)_ZTV16FloatingFloorBfs;
    return p;
}
static int __fastcall tp_init(void *s, void *)
{ return _ZN16FloatingFloorBfs13InitResourcesEv(s); }
static int __fastcall tp_clean(void *s, void *)
{ return _ZN16FloatingFloorBfs16CleanupResourcesEv(s); }
static int __fastcall tp_behavior(void *s, void *)
{ return func_ov002_020b616c((char *)s); }
static int __fastcall tp_render(void *s, void *)
{ port_actor_render_probe("TILTING_PLATFORM_BFS", (char *)s + 0xd4);
  return func_ov002_020b6144(s); }
static int __fastcall tp_d1(void *s, void *)
{ return (int)(size_t)_ZN16FloatingFloorBfsD1Ev((int *)s); }
static int __fastcall tp_d0(void *s, void *)
{ return (int)(size_t)_ZN16FloatingFloorBfsD0Ev((int *)s); }
extern "C" void hal_fill_tilting_platform_bfs_vtable(void)
{
    port_ov45_bringup();
    void *volatile *vt = (void *volatile *)_ZTV16FloatingFloorBfs;
    ov45_fill_shared(vt);
    vt[0]  = (void *)tp_init;
    vt[3]  = (void *)tp_clean;
    vt[6]  = (void *)tp_behavior;
    vt[9]  = (void *)tp_render;
    vt[16] = (void *)tp_d1;
    vt[17] = (void *)tp_d0;
    vt[31] = (void *)ov45_kill;
}

// ============================================================================
// FALL_BLOCK_BFS (id 139) -- table 0x021130f4, the _ZN12FallBlockBfs* bodies
// ============================================================================
//
// NINE of them on level 37, the largest population in the cast. 844-byte
// object. This class shares its base with FALL_BLOCK_WF (id 45, ov015): slots
// 6/9/27/31 are the SAME four ov098 bodies hal/actor_classes_wf.cpp seats, all
// four already in walk_window.map, and its Init/Cleanup are veneers onto the
// same ov098 pair through the file table data_ov045_021130ac. Slot 31 is the
// KillOrWhatever poof-dust death that slot 27 arms, not Platform::Kill.
extern "C" {
int _ZN12FallBlockBfs13InitResourcesEv(void *self);     /* slot 0  */
int _ZN12FallBlockBfs16CleanupResourcesEv(void *self);  /* slot 3  */
int func_ov098_0213a36c(char *self);                    /* slot 6  Behavior */
int func_ov098_0213a314(char *self);                    /* slot 9  Render */
int *_ZN12FallBlockBfsD1Ev(int *self);                  /* slot 16 */
int *_ZN12FallBlockBfsD0Ev(int *self);                  /* slot 17 */
void func_ov098_0213a284(char *self);                   /* slot 27 */
void func_ov098_0213a17c(char *self);                   /* slot 31 */
void *FallBlockBfs_Spawn(void);
DSSTATE_BEGIN
void *_ZTV12FallBlockBfs[32];
DSSTATE_END
}
#pragma comment(linker, "/alternatename:__ZTV21daObjKm2_Fall_Block_c=__ZTV12FallBlockBfs")
/* The unprefixed ov098 spellings this class's Init/Cleanup veneers use are
   already aliased by hal/actor_classes_wf.cpp for FALL_BLOCK_WF's identical
   pair, so they are not repeated here. */
extern "C" void *port_factory_fall_block_bfs(void)
{
    void *p = FallBlockBfs_Spawn();
    if (p)
        *(void **)p = (void *)_ZTV12FallBlockBfs;
    return p;
}
static int __fastcall fb_init(void *s, void *)
{ return _ZN12FallBlockBfs13InitResourcesEv(s); }
static int __fastcall fb_clean(void *s, void *)
{ return _ZN12FallBlockBfs16CleanupResourcesEv(s); }
static int __fastcall fb_behavior(void *s, void *)
{ return func_ov098_0213a36c((char *)s); }
static int __fastcall fb_render(void *s, void *)
{ port_actor_render_probe("FALL_BLOCK_BFS", (char *)s + 0xd4);
  return func_ov098_0213a314((char *)s); }
static int __fastcall fb_d1(void *s, void *)
{ return (int)(size_t)_ZN12FallBlockBfsD1Ev((int *)s); }
static int __fastcall fb_d0(void *s, void *)
{ return (int)(size_t)_ZN12FallBlockBfsD0Ev((int *)s); }
/* Slot 27 is OnHitByMegaChar(Player &): the caller pushes the player, so the
   veneer takes the third parameter to pop it even though the ov098 body reads
   only the receiver -- the fb_slot27 contract in hal/actor_classes_wf.cpp. */
static int __fastcall fb_slot27(void *s, void *, void *)
{ func_ov098_0213a284((char *)s); return 0; }
static int __fastcall fb_slot31(void *s, void *)
{ func_ov098_0213a17c((char *)s); return 0; }
extern "C" void hal_fill_fall_block_bfs_vtable(void)
{
    port_ov45_bringup();
    void *volatile *vt = (void *volatile *)_ZTV12FallBlockBfs;
    ov45_fill_shared(vt);
    vt[0]  = (void *)fb_init;
    vt[3]  = (void *)fb_clean;
    vt[6]  = (void *)fb_behavior;
    vt[9]  = (void *)fb_render;
    vt[16] = (void *)fb_d1;
    vt[17] = (void *)fb_d0;
    vt[27] = (void *)fb_slot27;
    vt[31] = (void *)fb_slot31;
}

// ---- method faces ----------------------------------------------------------
// The six bodies src defines as real C++ methods rather than extern-C free
// functions: four of PoleLift's (spelled FireSeaElevator::*, the shift) and two
// of ExtendingPlatform's (spelled PoleLift::*). The IceSheet/SwitchPillar/
// ClockPaintingHandShort recipe -- these compile from src as real methods, so
// the face is the C-name bridge INTO them, not a host copy of them.
#include "FireSeaElevator.h"
#include "PoleLift.h"

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
int _ZN15FireSeaElevator13InitResourcesEv(void *self)
{ return ((FireSeaElevator *)self)->FireSeaElevator::InitResources(); }
int _ZN15FireSeaElevator16CleanupResourcesEv(void *self)
{ return ((FireSeaElevator *)self)->FireSeaElevator::CleanupResources(); }
int _ZN15FireSeaElevator8BehaviorEv(void *self)
{ return ((FireSeaElevator *)self)->FireSeaElevator::Behavior(); }
int _ZN15FireSeaElevator6RenderEv(void *self)
{ return ((FireSeaElevator *)self)->FireSeaElevator::Render(); }
int _ZN8PoleLift13InitResourcesEv(void *self)
{ return ((PoleLift *)self)->PoleLift::InitResources(); }
int _ZN8PoleLift6RenderEv(void *self)
{ return ((PoleLift *)self)->PoleLift::Render(); }
}
