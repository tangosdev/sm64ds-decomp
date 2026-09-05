// RUN LINKW WAVE 8 (lane w8-packs): LETHAL LAVA LAND'S PLATFORM CAST (ov022,
// level 14). Five of the eleven ids ov022 owns -- VOLCANO_RING (71),
// FLOAT_ON_LAVA_PLATFORM (74), FLOATING_FLOOR_LLL_BIG (75),
// FLOATING_FLOOR_LLL_SMALL (76) and LAVA_PLANK (82) -- on four tables.
//
// Same law as hal/actor_classes_ov045.cpp, the wave-4 exemplar this file is
// built on: another level overlay's platform cast, another one-class naming
// shift, the same 32-slot Platform tables and the same ov002 inherited slots.
// hal/actor_classes_ov065.cpp is the later refinement the bring-up follows.
//
// ov022 was mounted --whole as DATA ONLY since gate 163 (it is level 14's own
// level overlay). NONE of its 83 matched TUs was in any build slice, so the
// whole cast was declined by the pre-spawn gate. This lane adds the second,
// per-symbol mount (port/ov022_syms.txt) and the code slice
// (port/slice_w8packs.txt).
//
// ---- THE CAST MAP WAS RE-DERIVED, NOT CARRIED -----------------------------
//
// Three independent routes, all agreeing, per class. The full tables are in
// port/ov022_syms.txt's header; the short form:
//
//   (1) the arm9 SPAWN TABLE. The words at 0x02090864 + id*4 are RELOCATIONS,
//       so they come from config/arm9/relocs.txt, and dsd's
//       `module:overlays(...)` list is over-broad because ov014/ov015/ov016/
//       ov020/ov021/ov022/ov029/ov034/ov036 share the level-overlay window at
//       base 0x021111a0. BOTH disambiguation routes were required of every id:
//       the SpawnInfo's word[0] spawnFunc lands inside ov022's own image AND
//       the record's word[1] low halfword equals the id the slot came from.
//       Raw bytes from extracted/overlays/overlay_0022.bin (13152 == the
//       overlays.yaml code_size, the DECOMPRESSED image).
//   (2) each Spawn's OWN final vtable-store relocation.
//   (3) the RTTI typeinfo at vtable[-1] -- class identity from the ROM's own
//       Itanium record, NEVER from a dsd label.
//
//   id   TRUE class               table        RTTI at vtable[-1]      x on L14
//   71   VolcanoRing              0x02113d18   14daObjFl_Ring_c          x1
//   74   FloatOnLavaPlatform      0x02113eac   15daObjFl_Block_c         x1
//   75   FloatingFloorLllBig      0x0211412c   17daObjFl_Ukiyuka_c       x1
//   76   FloatingFloorLllSmall    0x0211412c   17daObjFl_Ukiyuka_c       x3
//   82   LavaPlank                0x021141f0   15daObjFl_UkiKi_c         x3
//
// 75 and 76 SHARE one table: FloatingFloorLllBig's block is the Spawn alone
// (no own methods) and both factories store 0x0211412c. That is the
// BOO/BIG_BOO shape hal/actor_classes_ov063.cpp already has -- two ids, one
// fill.
//
// ---- THE NAMING SHIFT ------------------------------------------------------
//
// dsd lays each mwcc class block out as [bodies..., Spawn] and names the block
// from the PRECEDING Spawn label, so a class whose block follows a body-less
// class inherits the wrong name. Two of the four tables filled here are
// shifted and two are not:
//
//   74  FloatOnLavaPlatform  bodies spelled _ZN19RotatingPlatformLll*
//                            table  spelled _ZTV19RotatingPlatformLll
//   82  LavaPlank            bodies spelled _ZN19FloatingFloorLllBig*
//                            table  spelled _ZTV19FloatingFloorLllBig
//   71  VolcanoRing          unnamed func_ov022_* bodies, unnamed table
//   76  FloatingFloorLllSmall its own block, named correctly
//
// The shift is settled by the vtable's own slot-0/3/6/9/16/17 relocations
// landing inside the block that ENDS at that class's Spawn, and by the RTTI
// name siding with the SPAWN name in every row. "Everything is off by one" is
// wrong; use the table.
//
// ---- THE WIDTH: THIRTY-TWO, pinned twice ----------------------------------
//
// All four tables are 32 slots. (1) the reloc run: slot 31 is Platform::Kill
// (ov002 0x020ee55c) on every one of them and there is no slot-32 reloc into
// this overlay. (2) the next dsd symbol lands EXACTLY on each computed end:
// 0x02113d98, 0x02113f2c, 0x021141ac and 0x02114270 are all real symbols. The
// wf slot-31 bug (a [31] array reading past the end and taking the process)
// is why both routes are quoted rather than one.
//
// ---- SLOTS THAT ARE INHERITED BUT NOT arm9 --------------------------------
//
// FLOATING_FLOOR_LLL_SMALL/BIG take Behavior and Render from an ov002 base:
//   slot 6 func_ov002_020b6494   slot 9 func_ov002_020b646c
// Those are the SAME two bodies ov045's FloatingFloorBfs (id 145) already
// seats, and both were in walk_window.map on this lane's own baseline
// (C:\tmp\wl31, 4623/11240), so they cost no new TU.
//
// ---- THE VTABLES ARE HOST STORAGE ------------------------------------------
//
// All ten ov022 tables are excluded from port/ov022_syms.txt and the four
// filled here are declared below as host arrays -- the ov015/ov016/ov045/ov080
// rule: a mounted vtable hands a factory DS code addresses. Two are unnamed
// data symbols dsd left as plain data (0x02113d18) or named for the wrong
// class; each RTTI name the address also answers to is aliased onto the one
// host array, because the destructors restore their table by the RTTI
// spelling.
//
// ---- FOUR PLACES THE RECOVERED SOURCE CANNOT BE CALLED AS WRITTEN ---------
//
// 1. THE `p[0] = VT1` FACTORIES (75 and 76). Both FloatingFloor Spawns end
//    with a store of the shared-header VT1 placeholder, which is
//    hal/auto_bss.cpp's zeroed [8]. The ROM's two stores are, in order:
//
//      021120ec  LDR r1, =0x0210912c   <- the ov002 BASE table
//      021120f0  LDR r0, =0x0211412c   <- the class's OWN table
//
//    base first, own second -- so in `p[0] = _ZTV17daObjFl_Ukiyuka_c;
//    p[0] = VT1;` the RTTI name is on the BASE store and VT1 is on the OWN
//    one. A raw spawn therefore leaves a live object dispatching through a
//    zeroed array. port_factory_floating_floor_lll_{small,big} below call the
//    real Spawn and then reseat p[0] onto the host table, which is the ROM's
//    final value -- the ov045 port_factory / ov065 port_factory_shutter_hmc
//    recipe. The other three factories (71, 74, 82) store their table by a
//    real name as the FINAL write and are registered directly.
//
// 2. _ZN19FloatingFloorLllBig16CleanupResourcesEv (id 82, slot 3) SPELLS ITS
//    TWO SharedFilePtrs G0/G1, and hal/cxx_aliases.cpp has already bound
//    _G0 to the GAME HEAP POINTER (data_020a0eac), ?G0@@3PAHA to SignPost's
//    ov002 model file and _G1 to SignPost's ov002 collision file. Linking this
//    TU would Release SignPost's LIVE SharedFilePtrs (or the heap pointer) on
//    every level-14 teardown, which is not a dying-object write and not
//    survivable. It is the ONE ov022 body this lane replaces with a host thunk
//    (lp_clean below) and the one TU held out of the slice. Run rel0215 wave 3
//    (lane w3-e) tried the per-source -D that works on 73's and 77's
//    identically-shaped CleanupResources and MEASURED the refusal -- see the
//    note above lp_clean; the disassembly below is what named the two targets
//    it would have bound.
//
//    THE THUNK IS INSTRUCTION-VERIFIED against the ROM body (0x021121cc,
//    0x44 bytes), disassembled from extracted/overlays/overlay_0022.bin:
//      021121d4  ADD r0, r4, #0x124
//      021121d8  BL  0x020393dc        MeshColliderBase::IsEnabled
//      021121e0  BEQ 0x021121ec        (skip Disable when disabled)
//      021121e4  ADD r0, r4, #0x124
//      021121e8  BL  0x02039140        MeshColliderBase::Disable
//      021121ec  LDR r0, =0x02114620   (pool 0x02112208)  the MODEL file ptr
//      021121f0  BL  0x02017b64        SharedFilePtr::Release
//      021121f4  LDR r0, =0x02114618   (pool 0x0211220c)  the CLSN file ptr
//      021121f8  BL  0x02017b64        SharedFilePtr::Release
//      021121fc  MOV r0, #1  /  return
//    The 0x02114620-then-0x02114618 order is confirmed independently by
//    __sinit_ov022_02112f78, which constructs 0x02114620 with the MODEL
//    destructor func_02017ab4 and 0x02114618 with the collision one. There is
//    no register-rename or scheduling divergence to classify: the thunk is a
//    statement-for-statement transcription, not a re-derivation.
//
// 3. THE ov059 NAME RACE. func_ov022_0211149c (71's InitResources) hands
//    func_020393c4 the address of ov022's own func_ov022_02111564 under
//    ov059's spelling -- another module's dsd export won the naming race at
//    0x02111564. Both names are the same ov022 body; the alias below is the
//    ov045 data_ov059_021114c8 treatment.
//
// 4. THE FIVE ov064 CLPS BLOCKS. Every class here passes an ov064 CLPS_Block
//    to MovingMeshCollider::SetFile, and none of the five was in
//    port/ov064_syms.txt. This lane appends them there (a five-symbol
//    additive edit, marked in that file). The disambiguation matters:
//    ov064 and ov065 share base 0x02115ee0, and 0x0211bbac is
//    `module:overlays(64,65)` -- ov065 has a FUNCTION there
//    (TtcMovingCubeA_Spawn) while ov064 has data, and the site is a
//    CLPS_Block argument, so it is ov064's. The other four are
//    `module:overlay(64)` outright.
//      71 -> 0x0211bbac (through the table data_ov022_02113cc8)
//      74 -> 0x0211bb0c        75/76 -> 0x0211baec + 0x0211bacc
//      82 -> 0x0211ba6c
//
// Every OTHER placeholder store in this cast is a DYING-OBJECT write. The
// D1/D0 bodies spell their intermediate base-table restores _ZTV10dBgActor_c /
// VT1 / VT2, and hal/actor_vtables.cpp keeps those as zeroed storage precisely
// because they are installed transiently during teardown and never dispatched;
// here the stores sit between two direct calls with no dispatch in between.
// That is the same reading hal/actor_classes_ov045.cpp records. `G0` in the
// five D0 bodies is the game heap and is already aliased to _data_020a0eac,
// which is the word relocs.txt has at each of those five literal-pool slots
// (0x0211123c+, 0x0211175c, 0x0211201c, 0x021121c8 -- checked one by one).
//
// ---- WHAT THIS LANE DOES *NOT* SEAT, AND WHY ------------------------------
//
// ov022 owns eleven ids. Four of them -- LAVA_BRIDGE (73, x2), LAVA_SEESAW
// (77, x1), ROTATING_PLATFORM_LLL (80, x1) and FALL_BLOCK_LLL (83, x9) -- are
// BLOCKED, and the blocker is measured, not aesthetic: sixteen of their
// slot bodies carry the `recovered from vtable slot identity` marker, so they
// are behavioural GUESSES, and port/tools/inferred_stub_guard.py freezes the
// seated-guess set at a two-entry ratchet that may only SHRINK. Naming any of
// them in a fill here would fail the build by design, and its baseline is
// explicitly not to be edited by hand. Per class:
//   73  func_ov022_02111bdc/02111a64/02111ad0/02111aa8/021119c4  (5 guesses)
//   77  func_ov022_02111ea0/02111d90/02111dfc/02111dd4/02111cf0  (5)
//   80  func_ov022_02111670/0211165c/021115f8                    (3)
//   83  func_ov022_02112448/02112434/021123d0                    (3)
// A fifth, ROLLING_LOG_LLL (70, x1), is guess-free but reaches THREE ov080
// bodies (its slot 9 func_ov080_02127124, its slot 27 func_ov080_02127058 and
// the func_ov080_021274ac its InitResources tail-calls, none of them linked on
// this baseline) plus an ov080 base table at data_ov080_02128338 that its
// Spawn stores before its own; that is an ov080 mount edit and a second
// closure, deferred to the next lane rather than half-done here.
// VOLCANO_FIRE (243) is ov022's eleventh id and is placed on NO mounted
// level's object list, so it gets no row (the ov019 rule); its PMF state
// records at 0x02114424/0x0211442c would also need a seat, and one of the two
// bodies (func_ov022_021126ac) is itself a guess.
//
// ---- LANE OWNERSHIP: WHY THE MOUNT BRING-UP IS IN THIS FILE ---------------
//
// The w4-b/w5-B pattern: no lane owns hal/actor_overlays.cpp this wave, so the
// bring-up rides the first registry fill (ov22_bringup, one done-guard, every
// fill calls it). The ordering that makes it safe is the one w4-b measured:
// port_actor_overlays_sinits() runs to completion before
// port_actor_registry_install() (hal/level_boot.cpp); the registry writes
// SpawnInfo+0 = host factory and then calls fill, and the generated
// port_ov022_syms_patch() writes only pointer words inside its own coverage
// (the +0 word holds a CODE address, outside the mount's coverage), so a patch
// that runs after the factory word is written does not undo it; and the
// SharedFilePtrs the sinits build are read by InitResources, which runs at
// spawn, after registration.
// THE HANDOFF: whoever next owns hal/actor_overlays.cpp should move the body
// of ov22_bringup into port_actor_overlays_sinits beside the ov013 block and
// cut the guard here to a call.
//
// FOUR of ov022's ten static initialisers run, the ov019/ov065 rule (only the
// sinits the seated classes need):
//   __sinit_ov022_02112ca8   71's model+clsn pair (0x02114500 / 0x02114508)
//   __sinit_ov022_02112d80   74's              (0x02114558 / 0x02114550)
//   __sinit_ov022_02112ec0   75+76's two pairs (0x021145e0/0x021145d0 and
//                            0x021145d8/0x021145c8)
//   __sinit_ov022_02112f78   82's              (0x02114620 / 0x02114618)
// The other six build file pointers for the five blocked classes and
// VolcanoFire's PMF seat, and stay out for the same reason their classes do.
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
/* Seven of this cast's bodies are REAL C++ METHODS in src/ (MSVC decorates
   them ?InitResources@RotatingPlatformLll@@QAEHXZ and friends), so they are
   called as methods against the generated headers -- never through a C name
   that does not exist, and never through an /alternatename, which would hand
   a __thiscall body a `this` that never was one (hal/method_faces.cpp's
   Model::LoadAndSetFile note). The other bodies of the same two classes ARE
   extern "C" in src/ and are declared as C names below; which is which was
   read off the link, not assumed:
     74  InitResources / CleanupResources / Behavior / Render  -> methods
     74  D1 / D0                                               -> C names
     82  InitResources / Render                                -> methods
     82  Behavior / D1 / D0                                    -> C names */
#include "RotatingPlatformLll.h"
#include "FloatingFloorLllBig.h"

extern "C" {
/* the arm9 shared half -- every address read off this lane's own reloc runs
   over the four tables; they agree slot for slot with each other and with the
   six ov045 tables, which is why the shape below is ov045's unchanged. */
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

/* the generated ov022 per-symbol mount (build/host-src/ov022_syms.c) */
void port_ov022_pack_check(void);
void port_ov022_syms_patch(void);
/* the four ov022 sinits the seated classes need */
void __sinit_ov022_02112ca8(void);
void __sinit_ov022_02112d80(void);
void __sinit_ov022_02112ec0(void);
void __sinit_ov022_02112f78(void);

/* what lp_clean has to spell out by hand (id 82, slot 3) */
int _ZN16MeshColliderBase9IsEnabledEv(void *self);
void _ZN16MeshColliderBase7DisableEv(void *self);
void _ZN13SharedFilePtr7ReleaseEv(void *sfp);
extern int data_ov022_02114620[];   /* 82's model SharedFilePtr */
extern int data_ov022_02114618[];   /* 82's collision SharedFilePtr */
/* the one NEW placeholder-name face this lane needs: 74's Behavior reaches
   Platform::IsClsnInRange through a plain name its own TU invented, the
   hal/actor_faces_bob.cpp rule-2 case. Both sides are cdecl, so it is an
   ordinary forward rather than an alias (and a forward keeps the name
   readable in the map). */
int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
}
extern "C" int Platform_IsClsnInRange(void *self, int a, int b)
{ return _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(self, a, b); }

// ---- the trap --------------------------------------------------------------
static void ov22_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: ov022 vtable slot %d is not hosted (actor id %u "
                 "%s)\n", slot, id, port_actor_class_name(id));
    { static char _m[128];
      std::snprintf(_m, sizeof _m, "unhosted ov022 vtable slot %d on id %u %s",
                    slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
#define OV22_TRAP(n) \
    static int __fastcall ov22_trap##n(void *s, void *) \
    { ov22_trap_report(s, n); return 0; }
OV22_TRAP(13) OV22_TRAP(14) OV22_TRAP(17)
#undef OV22_TRAP

static int __fastcall ov22_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall ov22_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall ov22_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall ov22_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall ov22_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall ov22_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall ov22_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall ov22_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall ov22_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall ov22_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall ov22_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
/* slot 19 takes the three-parameter shape so it emits `ret 4`: the dispatch
   site pushes the Player the callee pops -- the wf_turn_egg contract. */
static int __fastcall ov22_turn_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall ov22_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall ov22_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall ov22_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall ov22_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall ov22_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall ov22_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall ov22_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall ov22_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall ov22_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall ov22_egg(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }
/* slot 31, the Platform tail; all four tables here take it unchanged. */
static int __fastcall ov22_kill(void *s, void *)
{ _ZN8Platform4KillEv(s); return 0; }

/* The shared half, slots 1..30, every word of it read off the four tables'
   relocations (they agree slot for slot outside each class's own overrides,
   and agree with ov045's six). The caller writes its own 0/3/6/9/16/17 and
   its 31.

   Slots 13/14 are the ActorBase Virtual34/Virtual38 traps and 30 declines,
   both the wf reading (30's ROM body returns a Vector3 by value and the sret
   contract is unproved). Slot 17 traps here and every caller replaces it:
   all four classes have a real deleting destructor.

   THE POINTER IS VOLATILE ON PURPOSE -- the gate-200 elided-stores bug, repro
   and dumpbin proof in hal/actor_classes_ov002g200.cpp: MSVC 19.44 x86 /O2 can
   delete a static filler's stores when it is called with several distinct
   extern-array arguments. This helper has four callers. */
static void ov22_fill_shared(void *volatile *vt)
{
    vt[1]  = (void *)ov22_binit;
    vt[2]  = (void *)ov22_ainit;
    vt[4]  = (void *)ov22_bclean;
    vt[5]  = (void *)ov22_aclean;
    vt[7]  = (void *)ov22_bbeh;
    vt[8]  = (void *)ov22_abeh;
    vt[10] = (void *)ov22_bren;
    vt[11] = (void *)ov22_aren;
    vt[12] = (void *)ov22_pdes;
    vt[13] = (void *)ov22_trap13;
    vt[14] = (void *)ov22_trap14;
    vt[15] = (void *)ov22_heap;
    vt[17] = (void *)ov22_trap17;
    vt[18] = (void *)ov22_yoshi;
    vt[19] = (void *)ov22_turn_egg;
    vt[20] = (void *)ov22_v50;
    vt[21] = (void *)ov22_pounded;
    vt[22] = (void *)ov22_atk1;
    vt[23] = (void *)ov22_atk2;
    vt[24] = (void *)ov22_kicked;
    vt[25] = (void *)ov22_pushed;
    vt[26] = (void *)ov22_cannon;
    vt[27] = (void *)ov22_mega;
    vt[28] = (void *)ov22_under;
    vt[29] = (void *)ov22_egg;
    vt[30] = (void *)port_actor_s30_base;
}

// ---- the mount bring-up ----------------------------------------------------
/* CAPTURED, and the argument is hal/level_boot.cpp's on g_level_mounted: this
   flag says "port_ov22_bringup has run", and everything that pass writes --
   the mount's rebased pointers and the SharedFilePtrs its static initialisers
   construct -- lives in .dsstate. A restore rolls that back. A guard that does
   not roll back with it leaves the pass skipped forever and the overlay
   holding raw DS pointers, which is the defect behind both of the RELOAD
   review's referrals. Bracketed, the pass re-runs exactly when its results
   were rolled away. */
DSSTATE_BEGIN
static int g_ov22_bringup_done;
DSSTATE_END

extern "C" void port_ov22_bringup(void)
{
    if (g_ov22_bringup_done)
        return;
    g_ov22_bringup_done = 1;
    port_ov022_pack_check();
    port_ov022_syms_patch();
    __sinit_ov022_02112ca8();   /* 71's SharedFilePtrs   */
    __sinit_ov022_02112d80();   /* 74's                  */
    __sinit_ov022_02112ec0();   /* 75's and 76's         */
    __sinit_ov022_02112f78();   /* 82's                  */
}

// ============================================================================
// VOLCANO_RING (71) -- table 0x02113d18, the func_ov022_0211* bodies
// ============================================================================
//
// 808-byte object built by Platform's constructor; Model at +0xd4,
// MovingMeshCollider at +0x124. Its InitResources reads its model/collision
// SharedFilePtrs and its CLPS block out of the three-word table
// data_ov022_02113cc8 = { 0x02114500, 0x02114508, ov064 0x0211bbac } rather
// than by direct literal -- which is why the table has to be MOUNTED and the
// ov064 CLPS block with it.
extern "C" {
int func_ov022_0211149c(char *self);   /* slot 0  InitResources */
int func_ov022_0211123c(void *self);   /* slot 3  CleanupResources */
int func_ov022_021112ac(char *self);   /* slot 6  Behavior */
int func_ov022_02111284(void *self);   /* slot 9  Render */
int *func_ov022_021111a0(int *self);   /* slot 16 D1 */
int *func_ov022_021111e4(int *self);   /* slot 17 D0 */
void *VolcanoRing_Spawn(void);
DSSTATE_BEGIN
void *data_ov022_02113d18[32];
DSSTATE_END
}
/* func_ov022_0211149c hands func_020393c4 the address of ov022's own
   func_ov022_02111564 under ov059's spelling -- another module's dsd export
   won the naming race at 0x02111564. Both names are the same ov022 body. */
#pragma comment(linker, "/alternatename:_data_ov059_02111564=_func_ov022_02111564")
/* func_ov022_0211123c.cpp declares the three-word table as a bare `int` while
   the mount defines the plain C name; data has no calling convention. */
#pragma comment(linker, "/alternatename:?data_ov022_02113cc8@@3HA=_data_ov022_02113cc8")
static int __fastcall vr_init(void *s, void *)
{ return func_ov022_0211149c((char *)s); }
static int __fastcall vr_clean(void *s, void *)
{ return func_ov022_0211123c(s); }
static int __fastcall vr_behavior(void *s, void *)
{ return func_ov022_021112ac((char *)s); }
static int __fastcall vr_render(void *s, void *)
{ port_actor_render_probe("VOLCANO_RING", (char *)s + 0xd4);
  return func_ov022_02111284(s); }
static int __fastcall vr_d1(void *s, void *)
{ return (int)(size_t)func_ov022_021111a0((int *)s); }
static int __fastcall vr_d0(void *s, void *)
{ return (int)(size_t)func_ov022_021111e4((int *)s); }
extern "C" void hal_fill_volcano_ring_vtable(void)
{
    port_ov22_bringup();
    void *volatile *vt = (void *volatile *)data_ov022_02113d18;
    ov22_fill_shared(vt);
    vt[0]  = (void *)vr_init;
    vt[3]  = (void *)vr_clean;
    vt[6]  = (void *)vr_behavior;
    vt[9]  = (void *)vr_render;
    vt[16] = (void *)vr_d1;
    vt[17] = (void *)vr_d0;
    vt[31] = (void *)ov22_kill;
}

// ============================================================================
// FLOAT_ON_LAVA_PLATFORM (74) -- table 0x02113eac, the
// _ZN19RotatingPlatformLll* bodies (THE SHIFT)
// ============================================================================
//
// 808-byte object; Model at +0xd4, MovingMeshCollider at +0x124. Its
// InitResources reads 0x02114558 / 0x02114550 by literal and passes ov064's
// CLPS block 0x0211bb0c to MovingMeshCollider::SetFile. RTTI daObjFl_Block_c.
extern "C" {
/* slots 0/3/6/9 are C++ methods -- see the note at the top of the file */
int *_ZN19RotatingPlatformLllD1Ev(int *self);                  /* slot 16 */
int *_ZN19RotatingPlatformLllD0Ev(int *self);                  /* slot 17 */
void *FloatOnLavaPlatform_Spawn(void);
DSSTATE_BEGIN
void *_ZTV19RotatingPlatformLll[32];
DSSTATE_END
}
/* 0x02113eac answers to both names; the D1/D0 restore it by the RTTI one. */
#pragma comment(linker, "/alternatename:__ZTV15daObjFl_Block_c=__ZTV19RotatingPlatformLll")
/* 74's InitResources declares its SharedFilePtr pair as `char *` while the
   ov022 mount defines the plain C name; data has no calling convention, so an
   alias onto the one object is exact (the hal/actor_faces_bob.cpp rule 1). */
#pragma comment(linker, "/alternatename:?data_ov022_02114558@@3PADA=_data_ov022_02114558")
static int __fastcall fl_init(void *s, void *)
{ return ((RotatingPlatformLll *)s)->RotatingPlatformLll::InitResources(); }
static int __fastcall fl_clean(void *s, void *)
{ return ((RotatingPlatformLll *)s)->RotatingPlatformLll::CleanupResources(); }
static int __fastcall fl_behavior(void *s, void *)
{ return ((RotatingPlatformLll *)s)->RotatingPlatformLll::Behavior(); }
static int __fastcall fl_render(void *s, void *)
{ port_actor_render_probe("FLOAT_ON_LAVA_PLATFORM", (char *)s + 0xd4);
  return ((RotatingPlatformLll *)s)->RotatingPlatformLll::Render(); }
static int __fastcall fl_d1(void *s, void *)
{ return (int)(size_t)_ZN19RotatingPlatformLllD1Ev((int *)s); }
static int __fastcall fl_d0(void *s, void *)
{ return (int)(size_t)_ZN19RotatingPlatformLllD0Ev((int *)s); }
extern "C" void hal_fill_float_on_lava_platform_vtable(void)
{
    port_ov22_bringup();
    void *volatile *vt = (void *volatile *)_ZTV19RotatingPlatformLll;
    ov22_fill_shared(vt);
    vt[0]  = (void *)fl_init;
    vt[3]  = (void *)fl_clean;
    vt[6]  = (void *)fl_behavior;
    vt[9]  = (void *)fl_render;
    vt[16] = (void *)fl_d1;
    vt[17] = (void *)fl_d0;
    vt[31] = (void *)ov22_kill;
}

// ============================================================================
// FLOATING_FLOOR_LLL_SMALL (76) and _BIG (75) -- ONE table 0x0211412c
// ============================================================================
//
// 816-byte objects; Model at +0xd4, MovingMeshCollider at +0x124. The only
// class pair in this file: FloatingFloorLllBig's block is its Spawn alone, and
// both factories store the same table after the ov002 base 0x0210912c, so one
// fill serves both ids (the BOO/BIG_BOO shape).
//
// Slots 6 and 9 are ov002's shared bodies -- the same pair ov045's
// FloatingFloorBfs takes -- so this class's own matched set is the four
// _ZN21FloatingFloorLllSmall* bodies and nothing else.
//
// BOTH factories are VT1 wrappers (see the header, item 1).
extern "C" {
int _ZN21FloatingFloorLllSmall13InitResourcesEv(void *self);     /* slot 0  */
int _ZN21FloatingFloorLllSmall16CleanupResourcesEv(void *self);  /* slot 3  */
int func_ov002_020b6494(char *self);                             /* slot 6  */
int func_ov002_020b646c(void *self);                             /* slot 9  */
int *_ZN21FloatingFloorLllSmallD1Ev(int *self);                  /* slot 16 */
int *_ZN21FloatingFloorLllSmallD0Ev(int *self);                  /* slot 17 */
void *FloatingFloorLllSmall_Spawn(void);
void *FloatingFloorLllBig_Spawn(void);
DSSTATE_BEGIN
void *_ZTV21FloatingFloorLllSmall[32];
DSSTATE_END
}
/* uki-yuka = the floating floor; the D1/D0 restore the table by this name
   (their literal at the first store IS 0x0211412c -- checked in the reloc
   run -- while the SPAWNS bind the same name to the ov002 base literal,
   which is what the wrappers below make transient). */
#pragma comment(linker, "/alternatename:__ZTV17daObjFl_Ukiyuka_c=__ZTV21FloatingFloorLllSmall")
/* THE SECOND NAME RACE. 76's InitResources reaches its own overlay's
   six-word file/CLPS table at 0x021140d4 under ov036's spelling -- ov018,
   ov022, ov036 and ov056 all share level-overlay base 0x021111a0, and ov036's
   dsd export won the naming race at that address (the same class of collision
   hal/actor_classes_ccm.cpp records for SkiLift's data_ov036/ov056/ov022
   triple). Both names are the same ov022 object.

   RETIRED, run rel0215 wave 1 (lane cast-ov036), AND THE ROUTING STILL
   APPLIES. The directive here used to be
     /alternatename:_data_ov036_021140d4=_data_ov022_021140d4
   which worked only while nothing DEFINED the LHS. ov036 now has its own
   per-symbol mount (port/ov036_syms.txt) and data_ov036_021140d4 is one of
   TRICKY_TRIANGLES' ten SharedFilePtrs, so the alias became inert and 76 would
   have loaded its models out of Rainbow Cruise's cells -- silently, with no
   link error and no byte-gate signal. port/tools/alternatename_guard.py caught
   it at the link. The routing moved to the guard's own remedy, a per-source -D
   on the ONE reader (src/_ZN21FloatingFloorLllSmall13InitResourcesEv.c) in
   port/CMakeLists.txt beside the ov036 slice block. Nothing about 76 changes;
   what changes is that the binding no longer depends on a name staying
   undefined somewhere else in the tree. */
extern "C" void *port_factory_floating_floor_lll_small(void)
{
    void *p = FloatingFloorLllSmall_Spawn();
    if (p)
        *(void **)p = (void *)_ZTV21FloatingFloorLllSmall;
    return p;
}
extern "C" void *port_factory_floating_floor_lll_big(void)
{
    void *p = FloatingFloorLllBig_Spawn();
    if (p)
        *(void **)p = (void *)_ZTV21FloatingFloorLllSmall;
    return p;
}
static int __fastcall ff_init(void *s, void *)
{ return _ZN21FloatingFloorLllSmall13InitResourcesEv(s); }
static int __fastcall ff_clean(void *s, void *)
{ return _ZN21FloatingFloorLllSmall16CleanupResourcesEv(s); }
static int __fastcall ff_behavior(void *s, void *)
{ return func_ov002_020b6494((char *)s); }
static int __fastcall ff_render(void *s, void *)
{ port_actor_render_probe("FLOATING_FLOOR_LLL", (char *)s + 0xd4);
  return func_ov002_020b646c(s); }
static int __fastcall ff_d1(void *s, void *)
{ return (int)(size_t)_ZN21FloatingFloorLllSmallD1Ev((int *)s); }
static int __fastcall ff_d0(void *s, void *)
{ return (int)(size_t)_ZN21FloatingFloorLllSmallD0Ev((int *)s); }
extern "C" void hal_fill_floating_floor_lll_vtable(void)
{
    port_ov22_bringup();
    void *volatile *vt = (void *volatile *)_ZTV21FloatingFloorLllSmall;
    ov22_fill_shared(vt);
    vt[0]  = (void *)ff_init;
    vt[3]  = (void *)ff_clean;
    vt[6]  = (void *)ff_behavior;
    vt[9]  = (void *)ff_render;
    vt[16] = (void *)ff_d1;
    vt[17] = (void *)ff_d0;
    vt[31] = (void *)ov22_kill;
}

// ============================================================================
// LAVA_PLANK (82) -- table 0x021141f0, the _ZN19FloatingFloorLllBig* bodies
// (THE SHIFT)
// ============================================================================
//
// 808-byte object; Model at +0xd4, MovingMeshCollider at +0x124. RTTI
// daObjFl_UkiKi_c. Its InitResources reads 0x02114620 / 0x02114618 and passes
// ov064's CLPS block 0x0211ba6c. Slot 3 is the ONE host thunk in this file --
// see the header, item 2, for the instruction-level derivation.
extern "C" {
/* slots 0 and 9 are C++ methods; slot 6 is extern "C" in src/ */
int _ZN19FloatingFloorLllBig8BehaviorEv(char *self);           /* slot 6  */
int *_ZN19FloatingFloorLllBigD1Ev(int *self);                  /* slot 16 */
int *_ZN19FloatingFloorLllBigD0Ev(int *self);                  /* slot 17 */
void *LavaPlank_Spawn(void);
DSSTATE_BEGIN
void *_ZTV19FloatingFloorLllBig[32];
DSSTATE_END
}
/* uki-ki = the floating wood; the D1/D0 restore the table by this name. */
#pragma comment(linker, "/alternatename:__ZTV15daObjFl_UkiKi_c=__ZTV19FloatingFloorLllBig")
/* 82's Behavior indexes the arm9 sine-ish table at 0x02082214 through its own
   two-short `SEnt` declaration; the arm9 mount defines the plain C name. Same
   data-alias rule as 74's file pointers above. */
#pragma comment(linker, "/alternatename:?data_02082214@@3PAUSEnt@@A=_data_02082214")
static int __fastcall lp_init(void *s, void *)
{ return ((FloatingFloorLllBig *)s)->FloatingFloorLllBig::InitResources(); }
/* slot 3, HOST THUNK, not the matched TU: the matched
   src/_ZN19FloatingFloorLllBig16CleanupResourcesEv.cpp spells its two
   SharedFilePtrs G0/G1, which hal/cxx_aliases.cpp has bound to the game heap
   pointer and to SignPost's ov002 file pointers. Statement-for-statement
   transcription of the ROM body at 0x021121cc.
   run rel0215 wave 3 (lane w3-e) TRIED to retire this thunk with the
   per-source -D it used on 73's and 77's identically-shaped CleanupResources,
   and MEASURED THE REFUSAL instead: both this body's targets are declared in
   include/decl_common.h (lines 238-239, `extern char data_ov022_02114618[]`
   and `..._02114620[]`) while G1 is declared there too (line 396,
   `extern int G1[]`), so -DG1=data_ov022_02114618 rewrites one into a
   redefinition of the other with a different type -- error C2371, the same
   wall the ov006 Mg3DEsp block at port/CMakeLists.txt:6295 already records.
   73's and 77's targets are NOT in that header, which is why the rename works
   there and not here. The thunk stays and the TU stays out of the slice. */
static int __fastcall lp_clean(void *s, void *)
{
    char *t = (char *)s;
    if (_ZN16MeshColliderBase9IsEnabledEv(t + 0x124))
        _ZN16MeshColliderBase7DisableEv(t + 0x124);
    _ZN13SharedFilePtr7ReleaseEv(data_ov022_02114620);
    _ZN13SharedFilePtr7ReleaseEv(data_ov022_02114618);
    return 1;
}
static int __fastcall lp_behavior(void *s, void *)
{ return _ZN19FloatingFloorLllBig8BehaviorEv((char *)s); }
static int __fastcall lp_render(void *s, void *)
{ port_actor_render_probe("LAVA_PLANK", (char *)s + 0xd4);
  return ((FloatingFloorLllBig *)s)->FloatingFloorLllBig::Render(); }
static int __fastcall lp_d1(void *s, void *)
{ return (int)(size_t)_ZN19FloatingFloorLllBigD1Ev((int *)s); }
static int __fastcall lp_d0(void *s, void *)
{ return (int)(size_t)_ZN19FloatingFloorLllBigD0Ev((int *)s); }
extern "C" void hal_fill_lava_plank_vtable(void)
{
    port_ov22_bringup();
    void *volatile *vt = (void *volatile *)_ZTV19FloatingFloorLllBig;
    ov22_fill_shared(vt);
    vt[0]  = (void *)lp_init;
    vt[3]  = (void *)lp_clean;
    vt[6]  = (void *)lp_behavior;
    vt[9]  = (void *)lp_render;
    vt[16] = (void *)lp_d1;
    vt[17] = (void *)lp_d0;
    vt[31] = (void *)ov22_kill;
}
