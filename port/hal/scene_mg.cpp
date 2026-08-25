// THE MINIGAME SCENE: ov004's dScMgBase_c framework, ov006's classes, and the
// two things a minigame needs that no scene before it did -- a THIRTY-SIX slot
// vtable and a set of OVERLAY CONSTRUCTORS that have to run before the scene
// spawns. Run link60, lane MG1 (the pathfinder).
//
// Read port/mg_fanout_costs.txt for what this cost and what the next thirty
// cost after it. Read port/scene_boot_map.txt for the spine this rides, which
// is unchanged: the port still adds nothing to the ROM's own
// Scene::SetSceneToSpawn -> Scene::SpawnIfNecessary -> func_02043098 chain.
//
// THIS FILE IS IN NO TARGET, and every statement below is written in the
// future tense whether or not it reads that way. port/CMakeLists.txt's MG1
// block adds neither this file nor slice_mg1.txt to anything, because the
// minigame framework dispatches mwcc pointer-to-member tables MSVC cannot
// represent, so nothing here is compiled, linked or executed by any build
// today. Read "runs" below as "runs when the dispatch lane wires this", and
// treat any measurement in this file as a measurement of the ROM or of a
// scratch link, never of walk_window.exe.
//
// ---- 1. WHY THIS IS NOT scene_boot.cpp's SHAPE ----------------------------
//
// The star select and the title screen are eighteen-slot _ZTV5Scene tables
// with seven slots overridden. A minigame is not. dScMgBase_c derives from
// Scene and ADDS EIGHTEEN VIRTUALS OF ITS OWN, so its table
// (data_ov004_020bc0c0) is thirty-six slots, and every ov006 minigame class
// inherits that width. vtspan's three routes agree on 36 for the base and for
// dScMgCurling_c's data_ov006_0213c304 alike.
//
// scene_fill_shared() cannot be pointed at one of these, and the reason is
// worth having in front of whoever seats the second minigame: it is not the
// width, it is the INDEX LIST. dScMgBase_c overrides five of the eleven slots
// that function writes (1, 2, 5, 7 and 10), so calling it on a minigame table
// would quietly replace five framework overrides with the base bodies they
// exist to displace, and the tail would be a separate, milder bug. The generic
// answer is port_scene_fill_rom() in hal/scene_boot.cpp, which keys on the ROM
// WORD the slot holds rather than on the slot number and therefore cannot
// write a slot the ROM did not park a shared body in. It works at any width.
//
// ---- 2. THE OVERLAY CONSTRUCTORS, AND WHERE THEY RUN ----------------------
//
// This is the part no lane had done. ov006 has thirty-one .init constructors
// and ov004 four, inventoried by the mount lane (port/ov006_syms.txt,
// port/ov004_syms.txt) and explicitly NOT run: "identified but not wired to
// run". They are wired here.
//
// THE ROM RUNS THEM AT OVERLAY LOAD. func_0201a694, the spawn spine's
// pre-spawn hook, calls GetSceneOverlayID(id); for an id IsMinigameActorID
// accepts (0x169..0x186) that answers ov006, and func_0201a798 then loads
// ov004 FIRST and ov006 second. The DS's LoadOverlay walks each overlay's
// static-init range -- ov006's is 0x0213356c..0x021335ec, thirty-two words,
// thirty-one function pointers and a NULL terminator -- and calls every entry.
// So on the real machine all thirty-five have run before the factory is
// reached, and NONE of them has run during a level, because ov002 and ov006
// are never co-resident.
//
// THE PORT'S PRECEDENT IS PER-MODE, WHICH IS THE SAME STATEMENT. A level run
// calls port_actor_overlays_sinits() out of hal/level_boot.cpp and runs the
// level overlays' constructors there; nothing runs them on any other path. So
// the minigame set runs from port_scene_mg_overlay_load() below, called out of
// port_scene_run BEFORE the spawn and ONLY when the requested id is one
// IsMinigameActorID accepts -- the ROM's own predicate, linked, not a range
// this file re-spells.
//
// IT DELIBERATELY DOES NOT RUN FROM port_scene_registry_install(). That
// function runs on EVERY boot, level runs included, because the ROM's own
// spawn-table edge has to be real for /OPT:REF on every target. Running
// ov006's constructors there would be a divergence with teeth rather than a
// harmless early call: __sinit_ov004_020b948c calls func_020731dc, which
// THREADS A NODE ONTO AN ARM9-GLOBAL DESTRUCTOR LIST, and that list is walked
// on the level path. (The same sinit calls func_020733a8, which an earlier
// version of this block named as a second threader. It is not one -- it is an
// MSL array-construction primitive -- and the argument stands on
// func_020731dc alone.) The fill is safe to run on every boot because
// it only writes ov004/ov006 mount storage nothing else reads; the
// constructors are not, and the split is drawn there for that reason.
//
// ---- 3. WHAT DOES NOT RUN, AND WHY -- NOTHING, ANY MORE -------------------
//
// THE HOLE IS CLOSED. All thirty-five of the ROM's constructors run.
//
// This section used to name two that could not: __sinit_ov004_020b955c
// (0x574 bytes) and __sinit_ov006_0213014c (0x284). Both had a config symbol,
// no delink block and no src file when the minigame lane wrote that, and it
// named them rather than standing in for them, because a plausible
// constructor is exactly the guess port/tools/inferred_stub_guard exists to
// refuse and a constructor's whole job is to leave state behind.
//
// Both were matched on the decomp's main after this branch forked at
// 7b2f913fe -- ov004's as f410b0822 (#1116, 2026-08-06), ov006's as 817be2263
// (#1150, 2026-08-07) -- and run link60's port-catchup lane brought them
// across by address. Neither is a guess and neither is main's word taken on
// trust: both were re-verified in that lane's worktree with tools/match.py at
// mwccarm 2004/b56 with strict relocs, against
// extracted/overlays/overlay_0004.bin at base 0x020ad660 and
// extracted/overlays/overlay_0006.bin at base 0x020bfec0.
//
// WHAT THE TWO ACTUALLY BUILD, now that they are readable:
//
//   __sinit_ov004_020b955c  is the SHARED-FILE-POINTER BUILDER shape of
//     section 4. It fills data_ov004_020bf5d4 and a run of {a, b} pairs, and
//     it registers destructors through func_020731dc / func_020733a8 naming
//     func_ov004_020b4a70, _020b4a7c and _020b4aa0 by address. Those three
//     TUs entered slice_mg1.txt with it; the ROM's own constructor is what
//     references them.
//   __sinit_ov006_0213014c  is the PAIR-TABLE shape: twenty-four eight-byte
//     {code, adj} records copied out of ov006 .data statics into five .bss
//     dispatch tables (02141810, 02141840, 021417b0, 021417e8, 021417c8).
//     Section 4's reading of what those words are -- DS code addresses that
//     only an mwcc pointer-to-member call would dispatch -- applies to these
//     twenty-four exactly as it does to __sinit_ov006_021304ac's twenty-five.
//     Running it is the same act as running the thirty that already ran, and
//     it does not make a DS address dispatchable that was not before.
//
// ---- 4. WHAT THE CONSTRUCTORS ACTUALLY DO ---------------------------------
//
// Two shapes, and the split is clean. Read out of all thirty-five sources
// (thirty-three when this was written; the two that arrived later are one of
// each shape and neither is a third kind).
//
//   SHARED-FILE-POINTER BUILDERS (11 of the 35). They call
//   SharedFilePtr::Construct / func_02017a24 / func_02017acc and register a
//   destructor with func_020731dc, which is the ov085 / ov100 / ov015 shape
//   the level path already runs a dozen of. Nothing about them is new.
//
//   mwcc POINTER-TO-MEMBER PAIR TABLES (the rest). No calls at all: each is a
//   run of struct assignments copying eight-byte {code, adj} records out of
//   .data statics into .bss dispatch tables. dScMgCurling_c's own is
//   __sinit_ov006_021304ac, which copies TWENTY-FIVE pairs into seven tables
//   at 0x021418b0..0x02141950. All twenty-five read {code, 0} in the ROM
//   image, verified word by word.
//   TAKE THE TWENTY-FIVE FROM THE CONSTRUCTOR, NOT FROM AN ADDRESS RANGE.
//   0x0213c1e4 and 0x0213c2bc are the lowest and highest of them, but that
//   range holds twenty-eight slots and three are not pairs: 0x0213c214 is
//   MgShuffleShell_SpawnInfo itself, and 0x0213c264 and 0x0213c2ac are
//   unrelated data. Sweeping the span is how a fan-out lane gets three
//   phantom states with nonzero adjustments.
//
// THE PAIR TABLES ARE THE REASON THE SINITS MATTER AND ALSO THE REASON THIS
// LANE STOPS WHERE IT DOES. The words they copy are DS CODE ADDRESSES, and
// func_ov006_020e3528 (slot 6, the Behavior) dispatches them as
// `(c->*data_ov006_02141950[j].pmf[0])()`. That is an mwcc pointer-to-member
// call, and the port cannot make one: MSVC's member-pointer representation is
// not mwcc's, sizeof differs so the table stride is wrong, and the word is a
// DS address either way. It is the same wall port/unmatched/
// Player_ChangeState.cpp was written for. See section 6.
//
// ---- 5. THE VTABLE, DERIVED --------------------------------------------
//
// THE CLASS IS dScMgCurling_c AND "MgShuffleShell" IS THE LOCALISED NAME OF
// THE SAME MINIGAME. The peer screening picked actor id 0x176 by the symbol
// MgShuffleShell_Spawn; the ROM's own RTTI string at 0x0213c2d0 reads
// "14dScMgCurling_c", and InitResources loads /MG/d_2d_mg_bg_curling1_ncg.bin
// and .../curling2_ncg.bin by name. So src/MgShuffleShell_Spawn.c's
// `_ZTV14dScMgCurling_c` is NOT a per-TU placeholder guess of the ov007 VT0
// kind -- it is the right class name that happens not to be a config symbol
// name. The address it means is settled by the ROM anyway:
//     config/arm9/overlays/ov006/relocs.txt
//     from:0x020e3850 kind:load to:0x0213c304 module:overlay(6)
// and 0x020e3850 is inside MgShuffleShell_Spawn (0x020e3820, 0x34 bytes).
//
// THE SPAWN RECORD, read at (addr - 0x020bfec0) out of
// extracted/overlays/overlay_0006.bin, is EIGHT BYTES and not the ov007 shape:
//     0x0213c214  20 38 0e 02   0x020e3820  the factory
//                 76 01 76 01   374, 374    the id, twice
// The next dsd symbol is at 0x0213c21c, so there is no inline type name after
// it the way dScDSMT_c's record has one. The registry's +4 cross-check reads
// back 374 and passes.
//
// ALL THIRTY-SIX SLOTS, and the fill below writes every one of them. "ruled"
// means the body was disassembled out of the ROM image and compared
// instruction for instruction with src/ before it was seated; the evidence per
// body is in port/tools/inferred_stub_adjudicated.txt.
//
//   slot  ROM word    module  body
//    0   020e3578    ov006   InitResources            ruled REAL_DECOMP
//    1   020b0930    ov004   BeforeInitResources      ruled REAL_DECOMP
//    2   020b08f0    ov004   AfterInitResources       ruled REAL_DECOMP
//    3   02043bf0    arm9    ActorBase::CleanupResources
//    4   0202e5f0    arm9    Scene::BeforeCleanupResources
//    5   020b0840    ov004   AfterCleanupResources    ruled REAL_DECOMP
//    6   020e3528    ov006   Behavior                 ruled REAL_DECOMP
//    7   020b0620    ov004   BeforeBehavior           ruled REAL_DECOMP
//    8   0202e3c8    arm9    Scene::AfterBehavior            VENEER
//    9   020e34ec    ov006   Render                   ruled REAL_DECOMP
//   10   020b04f4    ov004   BeforeRender             ruled REAL_DECOMP
//   11   0202e398    arm9    Scene::AfterRender              VENEER
//   12   020b04e8    ov004   OnPendingDestroy         ruled REAL_DECOMP
//   13   0204357c    arm9    ActorBase::Virtual34
//   14   0204349c    arm9    ActorBase::Virtual38
//   15   02043494    arm9    ActorBase::OnHeapCreated
//   16   020e0638    ov006   D2                       (no marker)
//   17   020e065c    ov006   D0                       ruled REAL_DECOMP
//   18   020e3470    ov006   state reset              ruled REAL_DECOMP
//   19   020b2994    ov004                            ruled REAL_DECOMP
//   20   020b2990    ov004                            ruled REAL_DECOMP
//   21   020b298c    ov004                            ruled REAL_DECOMP
//   22   020ae198    ov004                            ruled REAL_DECOMP
//   23   020ae1a0    ov004                            ruled REAL_DECOMP
//   24   020ae140    ov004                            ruled REAL_DECOMP
//   25   020ae128    ov004                            ruled REAL_DECOMP
//   26   020b04e0    ov004                            ruled REAL_DECOMP
//   27   020af27c    ov004                            ruled REAL_DECOMP
//   28   020af04c    ov004                            ruled REAL_DECOMP
//   29   020af094    ov004                            ruled REAL_DECOMP
//   30   020aeed8    ov004                            ruled REAL_DECOMP
//   31   020b2880    ov004                            ruled REAL_DECOMP
//   32   020b27f4    ov004                            ruled REAL_DECOMP
//   33   020b265c    ov004                            (no marker)
//   34   020ae3b4    ov004                            (no marker)
//   35   020ad660    ov004                            (no marker)
//   --   word 36 reads 2f474d2f, the ASCII "/MG/" of the next symbol's path
//        string, which is what closes the table.
//
// TWENTY-FIVE OF THE THIRTY-SIX BODIES CARRIED THE "recovered from vtable slot
// identity" MARKER and all twenty-five were ruled REAL_DECOMP against the ROM
// before being seated -- twenty ov004, five ov006. TWENTY OF THE TWENTY-FIVE
// ARE dScMgBase_c's OWN and are therefore paid ONCE for all thirty minigames,
// which is the single biggest fact in the cost model.
//
// THE TABLE IS INSIDE THE ov006 MOUNT, so the fill writes host thunks into the
// mount's own storage rather than into a fresh host array -- the ov007
// treatment, for the ov007 reason: a second host array of the same name would
// be a duplicate symbol, and leaving the mounted one alone would leave live
// wild DS pointers in a table the factory installs.
//
// ---- 6. WHERE THIS STOPS ---------------------------------------------------
//
// See port/mg_fanout_costs.txt. The blocker is the mwcc pointer-to-member
// dispatch in section 4 and it is an ABI wall, not a missing body: every one
// of the twenty-five state functions has a matched src TU except
// func_ov006_020e1854, and the dispatch that would reach them cannot be
// compiled by MSVC from the ROM's own source shape.
//
// AMENDED, run link60 lane CT1. The sentence above is still true about the
// DECOMP -- func_ov006_020e1854 still has no src TU and no delink block, and
// stategen.py still reports it under REFUSALS -- but it now reads as though the
// class cannot run that state, and THAT part is out of date. The state is
// transcribed from the ROM in unmatched/MgCurling_State_020e1854.cpp and the
// address switch calls it, so dScMgCurling_c reaches all twenty-five. The host
// body is named port_mg_curling_st_020e1854 precisely so the decomp's own
// accounting keeps saying twenty-four.
//
// TWO HOST-ABI DEFECTS IN THE FRAMEWORK'S OWN SOURCE were found by the
// adjudication and are recorded here because they bite the moment slots 5 and
// 7 are dispatched, and neither is visible to the byte gate:
//
//   include/decl_Scene.h declares `extern int _ZN5Scene14BeforeBehaviorEv();`
//     with EMPTY PARENS inside extern "C", so src/func_ov004_020b0620.cpp's
//     call at line 52 passes no `this`. The real definition
//     (src/_ZN5Scene14BeforeBehaviorEv.cpp:47) takes `char* self` and
//     dereferences it immediately. On ARM this is a ride-through and correct
//     -- r0 already holds self -- and on the host the callee reads the stack.
//     Two lines BELOW it, the same header spells the sibling correctly,
//     `_ZN5Scene19BeforeInitResourcesEv(void*)` at line 25, which is what
//     makes this a defect rather than a convention.
//   src/func_ov004_020b0840.c declares `extern void func_0203cbc0(void);` and
//     calls it with no argument at line 28, while the ROM has the pointer
//     being deleted live in r0. The port's own host body is
//     `func_0203cbc0(void *p) { _ZdlPv(p); }` in
//     port/unmatched/func_02073244_hostcopy.c, so the call hands operator
//     delete whatever is on the stack. Seven other src TUs declare the same
//     function WITH a pointer parameter.
//
// Both are the argument-ride-through class hal/scene_actor_faces.cpp's header
// derives, in the framework rather than in the veneers, and both want the same
// remedy: a PORT_HOST_ABI host copy in port/unmatched/ that places the
// argument, leaving src/ and the byte gate alone. NOT TAKEN HERE, because
// neither can be exercised until the dispatch in section 4 exists, and a host
// copy nobody can call is a count rather than a port.

#include "hal/screen_gap.h"

#include <cstdio>
#include <cstdlib>
#include <chrono>

extern "C" {

/* generated by tools/ovdata.py into build/port/host-src/. This seat is the
   FIRST caller either mount has ever had: port/ov004_ov006_binding_diff.txt
   section 4 records that /OPT:REF strips both pack checks because "a mount
   with no registry rows has no caller". */
void port_ov004_pack_check(void);
void port_ov004_syms_patch(void);
void port_ov006_pack_check(void);
void port_ov006_syms_patch(void);

/* the ROM's own predicate, matched, linked from the slice. Not re-spelled. */
int IsMinigameActorID(unsigned int id);

/* the two arm9 RNG states, for the minigame seed at the foot of this section.
   data_0209d4b8 is hosted in hal/scene_mg_faces.cpp (sized by ROM span) and
   data_0209e650 in hal/auto_bss.cpp as int[8]; both are inside .dsstate, so a
   save state captures whatever the seed left. Spelled with the definitions'
   own types, so neither needs an /alternatename row. */
extern unsigned int data_0209d4b8;
extern int data_0209e650[];

/* ov004's four .init constructors, all four. */
void __sinit_ov004_020b948c(void);
void __sinit_ov004_020b955c(void);
void __sinit_ov004_020b9ad0(void);
void __sinit_ov004_020b9b24(void);

/* ov006's thirty-one, all thirty-one, in the ROM's own .ctor order, which is
   address order. */
void __sinit_ov006_0212f4c4(void);
void __sinit_ov006_0212f52c(void);
void __sinit_ov006_0212f660(void);
void __sinit_ov006_0212f6b4(void);
void __sinit_ov006_0212fc7c(void);
void __sinit_ov006_0212fd48(void);
void __sinit_ov006_021300b0(void);
void __sinit_ov006_0213014c(void);
void __sinit_ov006_021303d0(void);
void __sinit_ov006_021304ac(void);
void __sinit_ov006_02130758(void);
void __sinit_ov006_02130a04(void);
void __sinit_ov006_02130a08(void);
void __sinit_ov006_02130df8(void);
void __sinit_ov006_02130e9c(void);
void __sinit_ov006_02130f00(void);
void __sinit_ov006_02130f64(void);
void __sinit_ov006_021311c8(void);
void __sinit_ov006_021314e4(void);
void __sinit_ov006_021318a0(void);
void __sinit_ov006_0213195c(void);
void __sinit_ov006_02131a38(void);
void __sinit_ov006_02131cd0(void);
void __sinit_ov006_02131fa4(void);
void __sinit_ov006_021322bc(void);
void __sinit_ov006_02132894(void);
void __sinit_ov006_02132970(void);
void __sinit_ov006_02132f68(void);
void __sinit_ov006_0213322c(void);
void __sinit_ov006_0213326c(void);
void __sinit_ov006_021333e0(void);

/* THE BLOCKER'S SUBJECT. arm9 bss, hosted by hal/auto_bss.cpp as
   `int data_0209f61c[0x2c / 4]`. Its first word is the vptr; see the
   pre-flight check in the fill. */
extern int data_0209f61c[];

/* NitroSDK's FSDirPos current directory, which hal/fs_names.cpp fills through
   the ROM's own archive registration; storage in hal/scene_boot.cpp. The
   second pre-flight below reads it as a regression assertion. */
extern int data_020a804c[];

/* The third pre-flight below used to be keyed on
   hal/fdr_arm9_fader_seat.cpp's port_fdr_motion_slots_unseated(). Run link60
   Stage 5 lane SEAT8 seated slot 0x08 on func_0202f428 and retired both the
   predicate and the advisory; the reasoning is kept where the pre-flight was. */

/* the mount storage the fill writes into */
extern unsigned char data_ov006_0213c304[];   /* dScMgCurling_c, 36 slots */
extern unsigned char data_ov004_020bc0c0[];   /* dScMgBase_c,    36 slots */
extern unsigned char MgShuffleShell_SpawnInfo[];

/* the class's own six overrides */
int   func_ov006_020e3578(void *self);        /* slot  0 InitResources */
int   func_ov006_020e3528(void *self);        /* slot  6 Behavior      */
int   func_ov006_020e34ec(void *self);        /* slot  9 Render        */
int   func_ov006_020e0638(void *self);        /* slot 16 D2            */
int   func_ov006_020e065c(void *self);        /* slot 17 D0            */
void  func_ov006_020e3470(void *self);        /* slot 18 state reset   */

/* dScMgBase_c's own twenty-three, in slot order */
int   func_ov004_020b0930(char *c);
void  func_ov004_020b08f0(void *c, unsigned f);
void  func_ov004_020b0840(void *c, unsigned f);
int   func_ov004_020b0620(void *c);
int   func_ov004_020b04f4(void *c);
void  func_ov004_020b04e8(void);
int   func_ov004_020b2994(void);
void  func_ov004_020b2990(void);
void  func_ov004_020b298c(void);
int   func_ov004_020ae198(void);
int   func_ov004_020ae1a0(void);
int   func_ov004_020ae140(void *c);
int   func_ov004_020ae128(void *c);
int   func_ov004_020b04e0(void);
void  func_ov004_020af27c(void *c);
void  func_ov004_020af04c(void *c);
void  func_ov004_020af094(void *c);
void  func_ov004_020aeed8(void *c);
void  func_ov004_020b2880(void);
void  func_ov004_020b27f4(void);
void  func_ov004_020b265c(void *c);
/* SLOT 34 IS THE BOARD'S BRUSH AND TAKES FIVE PARAMETERS. Run mg10, lane F371.
   This line used to read `void func_ov004_020ae3b4(void *c);` while
   src/func_ov004_020ae3b4.c defines
       void func_ov004_020ae3b4(char* obj, int cx, int cy, int val, int n)
   -- an n-by-n block of `val` written at (cx,cy) through MultiCopy_Int. See
   mb_v34 below for what the one-parameter spelling cost and why nothing could
   convict it until this run. */
void  func_ov004_020ae3b4(void *c, int cx, int cy, int val, int n);
/* SLOT 35 IS A PREDICATE ON `this` AND WAS DECLARED AS NEITHER. Run mg6, lane
   PPP. This line used to read `void func_ov004_020ad660(void);` while
   src/func_ov004_020ad660.c defines `int func_ov004_020ad660(int *r0)
   { return (r0[2] & 0xff) != 0; }`. See mb_v35 below for what that cost. */
int   func_ov004_020ad660(int *c);

/* dScMgBase_c's OWN versions of the five slots a derived class overrides.
   Added by run link60 lane MG2; see kMgBaseFaces for why they were missing. */
int   func_ov004_020b0618(void);
int   func_ov004_020b04ec(void);
void *func_ov004_020b2a84(void *c);
void *func_ov004_020b2a18(void *self);
void  func_ov004_020b299c(void);

/* the base ctor the factory calls, and the factory itself */
void *func_ov004_020b2adc(char *self);
int  *MgShuffleShell_Spawn(void);

/* hal/scene_boot.cpp */
unsigned port_scene_fill_rom(void **vt, unsigned n);
int port_scene_env_want(void);

/* the two dispatch host copies, for the run report */
void port_mg_dispatch_counts(unsigned *calls, unsigned *unknown);
unsigned port_mg_curling_state_hits(void);
unsigned port_mg_trap_hits(void);

/* Run mg5 lane BASESET: the framework STATE SETTER's own four numbers.
   port/unmatched/MgBase_StateSetter.cpp. They are printed next to the dispatch
   counts above because a run can have both a nonzero call count and a setter
   that never ran, and the two facts read the same in the old line. */
void port_mg_base_setter_counts(unsigned *calls, unsigned *dispatched,
                                unsigned *states, unsigned *ticks,
                                unsigned *closure);
unsigned port_mg_base_setter_index_hits(unsigned *out, unsigned n);

/* Lane CUR2's two seated collision bodies. They are counted for the same
   reason the dispatch above is: "the shells collide now" has to be a
   measurement, and until this lane both addresses were faces whose only
   witness was a refusal line. A run that ticks a moving shell and reports
   zero entries here has a physics tick that is not reaching them. */
unsigned port_mg_curling_collide_020e1dc8_entries(void);
unsigned port_mg_curling_collide_020e20bc_entries(void);

}  /* extern "C" */

// ---- the tick witness ------------------------------------------------------
//
// One counter per dispatched slot, the same instrument the ov003 and ov007
// seats carry and for the same reason: an object that exists and an object
// that RUNS look identical from outside, and only the counters tell them
// apart. Thirty-six wide, because the table is.
static unsigned g_mg_hits[36];

#define MG_SLOT(n) (++g_mg_hits[(n)])

static int  __fastcall mg_init(void *s, void *)
{ MG_SLOT(0);  const int r = func_ov006_020e3578(s);
  hal_gapless_minigames_latch(); return r; }
static int  __fastcall mg_beh(void *s, void *)
{
    MG_SLOT(6);
    /* SM64DS_CUR_SHELL_FORCE="x,y": park a shell at a world position without
       the game ever seeing it. The flags are cleared BEFORE the tick so the
       game's own physics never runs on the slot, and set after so only the
       render pass reads it. Probe for the owner's straddle-layering report. */
    static int sforce = -1;
    static int sfx, sfy;
    if (sforce < 0) {
        const char *e = std::getenv("SM64DS_CUR_SHELL_FORCE");
        sforce = (e && std::sscanf(e, "%d,%d", &sfx, &sfy) == 2) ? 1 : 0;
    }
    unsigned char *sc = (unsigned char *)s;
    if (sforce) { *(sc + 0x4689) = 0; *(sc + 0x468a) = 0; }
    const int r = func_ov006_020e3528(s);
    if (sforce) {
        *(sc + 0x4689) = 1;
        *(sc + 0x468a) = 1;
        *(int *)(sc + 0x4660) = sfx << 12;
        *(int *)(sc + 0x4664) = sfy << 12;
    }
    hal_gapless_splice();
    /* SM64DS_CUR_SHELL_TRACE=1: the five shell slots whenever one is near
       the seam, for the owner's straddle-layering report. */
    static int strace = -1;
    if (strace < 0) {
        const char *e = std::getenv("SM64DS_CUR_SHELL_TRACE");
        strace = (e && e[0] && e[0] != '0') ? 1 : 0;
    }
    if (strace) {
        unsigned char *c = (unsigned char *)s;
        for (int i = 0; i < 5; i++) {
            unsigned char *p = c + i * 0x2c;
            if (!*(p + 0x4689) || !*(p + 0x468a)) continue;
            const int wy = *(int *)(p + 0x4664) >> 12;
            if (wy > -80 && wy < 40)
                std::fprintf(stderr, "[shell] beh%u slot%d x=%d wy=%d\n",
                             g_mg_hits[6], i,
                             *(int *)(p + 0x4660) >> 12, wy);
        }
    }
    return r;
}
static int  __fastcall mg_render(void *s, void *)
{ MG_SLOT(9);  return func_ov006_020e34ec(s); }
static void *__fastcall mg_d2(void *s, void *)
{ MG_SLOT(16); return (void *)(size_t)func_ov006_020e0638(s); }
static void *__fastcall mg_d0(void *s, void *)
{ MG_SLOT(17); return (void *)(size_t)func_ov006_020e065c(s); }
/* ---- SLOTS 18 AND 19 TAKE ONE ARGUMENT, AND THE THUNK MUST POP IT ---------
 *
 * Run mg5, lane BASESET. Added because seating the framework state setter made
 * these two slots reachable for the first time and the first run that reached
 * them FAULTED. It is a stack imbalance, not a mis-seated table.
 *
 * A __fastcall thunk stands in for a __thiscall vtable entry: `this` arrives in
 * ecx, the second parameter is the ignored register slot, and EVERY FURTHER
 * PARAMETER IS A STACK PARAMETER THE CALLEE POPS. So a thunk declared
 * `(void *s, void *)` compiles to a bare `ret`, and a ROM caller whose src
 * spells the virtual with an argument pushes four bytes that nobody takes back.
 * Four bytes leak per dispatch, the caller's own `ret` eventually takes a
 * garbage return address, and the fault lands at an address that MOVES WITH THE
 * BUILD because it is stack litter. Measured on scene 378 and scene 366, both
 * through func_ov004_020b8778 -> port_mg_call0 -> a framework state body.
 *
 * BOTH SLOTS WERE SCANNED OUT OF THE TWO OVERLAY IMAGES word by word rather
 * than argued from one call site. runs/mg5/out/baseset/slot18_19_scan.txt is
 * the listing and it REPRODUCES the slot-18 census the dScMgLuigi_c block below
 * already recorded:
 *
 *   offset 0x48, slot 18   22 sites, 21 set r1 first (mvn r1,#0 at fifteen,
 *                          mov r1,#3/#4/#5 at six). The one that does not is
 *                          func_ov004_020b29a0 at 0x020b29ac, and its own src
 *                          spells the virtual `v18(void*)` and forwards its
 *                          second parameter, so the host pushes one there too.
 *   offset 0x4c, slot 19   14 sites, ALL FOURTEEN set r1 first, every one of
 *                          them `ldr r1, [rX, #0x18]`, the message index.
 *
 * So the argument count is ONE at every site of both slots, and there is no
 * caller anywhere in either overlay for which a popping thunk is wrong.
 *
 * mgl_reset below ALREADY declared its stack parameter and its block already
 * carried the slot-18 census, ending "mg_reset and mb_reset_base above do NOT
 * declare it; that is slice_mg1's to look at". This is that look. The four
 * slot-18 thunks that were short and the one slot-19 thunk now all declare the
 * ride-through, and none of them uses it: the parameter exists so __fastcall
 * cleans four bytes, which is the signature repair port/fader_boot_map.txt
 * section 9 audits twelve of.
 */
static int  __fastcall mg_reset(void *s, void *, int /*ridethrough*/)
{ MG_SLOT(18); func_ov006_020e3470(s); return 1; }

/* SM64DS_SCENE_SLOT9=0 and SM64DS_SCENE_SLOT0=0, the two diagnostics the ov003
   and ov007 seats already carry, counted separately so a run can never read a
   no-op as the real body having run. */
static unsigned g_mg_render_skipped, g_mg_init_skipped;
static int __fastcall mg_render_noop(void *, void *)
{ ++g_mg_render_skipped; return 1; }
static int __fastcall mg_init_noop(void *, void *)
{ ++g_mg_init_skipped; return 1; }

// ---- dScMgBase_c's own twenty-three ---------------------------------------
static int  __fastcall mb_binit(void *s, void *)   { MG_SLOT(1);  return func_ov004_020b0930((char *)s); }
static void __fastcall mb_ainit(void *s, void *, unsigned f) { MG_SLOT(2);  func_ov004_020b08f0(s, f); }
static void __fastcall mb_aclean(void *s, void *, unsigned f){ MG_SLOT(5);  func_ov004_020b0840(s, f); }
static int  __fastcall mb_bbeh(void *s, void *)    { MG_SLOT(7);  return func_ov004_020b0620(s); }
static int  __fastcall mb_bren(void *s, void *)    { MG_SLOT(10); return func_ov004_020b04f4(s); }
static int  __fastcall mb_pdes(void *, void *)     { MG_SLOT(12); func_ov004_020b04e8(); return 0; }
static int  __fastcall mb_v19(void *, void *, int) { MG_SLOT(19); return func_ov004_020b2994(); }
static int  __fastcall mb_v20(void *, void *)      { MG_SLOT(20); func_ov004_020b2990(); return 0; }
static int  __fastcall mb_v21(void *, void *)      { MG_SLOT(21); func_ov004_020b298c(); return 0; }
static int  __fastcall mb_v22(void *, void *)      { MG_SLOT(22); return func_ov004_020ae198(); }
static int  __fastcall mb_v23(void *, void *)      { MG_SLOT(23); return func_ov004_020ae1a0(); }
static int  __fastcall mb_v24(void *s, void *)     { MG_SLOT(24); return func_ov004_020ae140(s); }
static int  __fastcall mb_v25(void *s, void *)     { MG_SLOT(25); return func_ov004_020ae128(s); }
static int  __fastcall mb_v26(void *, void *)      { MG_SLOT(26); return func_ov004_020b04e0(); }
static int  __fastcall mb_v27(void *s, void *)     { MG_SLOT(27); func_ov004_020af27c(s); return 0; }
static int  __fastcall mb_v28(void *s, void *)     { MG_SLOT(28); func_ov004_020af04c(s); return 0; }
static int  __fastcall mb_v29(void *s, void *)     { MG_SLOT(29); func_ov004_020af094(s); return 0; }
static int  __fastcall mb_v30(void *s, void *)     { MG_SLOT(30); func_ov004_020aeed8(s); return 0; }
static int  __fastcall mb_v31(void *, void *)      { MG_SLOT(31); func_ov004_020b2880(); return 0; }
static int  __fastcall mb_v32(void *, void *)      { MG_SLOT(32); func_ov004_020b27f4(); return 0; }
static int  __fastcall mb_v33(void *s, void *)     { MG_SLOT(33); func_ov004_020b265c(s); return 0; }
/* SLOT 34 DROPPED FOUR ARGUMENTS AND HAD NO POSSIBLE WITNESS. Run mg10, lane
 * F371, and it is mb_v35's defect one slot over with one extra twist.
 *
 * This thunk used to read
 *
 *     static int __fastcall mb_v34(void *s, void *)
 *     { MG_SLOT(34); func_ov004_020ae3b4(s); return 0; }
 *
 * against a ROM body that takes FIVE. Every slot-34 dispatch site in either
 * overlay image passes five -- r0..r3 plus one word stored at [sp] before the
 * blx -- so a two-parameter __fastcall thunk both fed the brush garbage
 * coordinates and cleaned the wrong number of bytes off the stack.
 *
 * WHY IT SAT THROUGH TWO RUNS UNCONVICTED, which is the part worth keeping.
 * The tree has exactly ONE body that dispatches slot 34, func_ov004_020ae5c4,
 * and until run mg10 that body was TRAPPED in hal/scene_mg_faces.cpp. So slot
 * 34 was unreachable on every scene rather than merely unreached on the ones
 * anybody had booted, and run mg9 lane S371 recorded the mismatch and
 * deliberately did not repair it, on the rule that a repair with no dispatch to
 * witness it is wiring without evidence. Seating the dispatcher is what turned
 * the witness on, and the repair lands with it rather than before it.
 *
 * THE FIVE ARE READ OFF THE DISPATCH SITES, not off the callee. At all eight
 * sites inside 0x020ae5c4 the shape is
 *     str r7,[sp] / ldr Rd,[r0] / ldr r3,[sp,#0x3c] / ldr Rd,[Rd,#0x88]
 *     / mov r1,sb / mov r2,r8 / blx Rd
 * so r1 and r2 are the walked coordinates, r3 is the caller's `val` and the
 * pushed word is its `n`. src/func_ov004_020ae3b4.c declares the same five. */
static int  __fastcall mb_v34(void *s, void *, int cx, int cy, int val, int n)
{ MG_SLOT(34); func_ov004_020ae3b4(s, cx, cy, val, n); return 0; }
/* SLOT 35 DROPPED `this` AND THREW THE ANSWER AWAY. Run mg6, lane PPP.
 *
 * This thunk used to read
 *
 *     static int __fastcall mb_v35(void *, void *)
 *     { MG_SLOT(35); func_ov004_020ad660(); return 0; }
 *
 * against a declaration of `void func_ov004_020ad660(void);` -- while the
 * matched body is `int func_ov004_020ad660(int *r0) { return (r0[2] & 0xff)
 * != 0; }`. So the callee read whatever was on the stack in place of `this`,
 * AND the thunk returned a hardcoded 0 instead of the answer. On ARM both
 * halves ride through and are correct, because r0 already holds `this` and the
 * callee's own return value is the caller's; on the host neither is. It is the
 * same family as lane BASESET's slot-18/19 stack repair and lane FDR2's
 * Scene::BeforeBehavior repair, and like both of those it is invisible to the
 * byte gate.
 *
 * NOTHING HAD EVER CALLED IT, which is why it stood. None of the six classes
 * seated before dScMgPanel_c dispatches slot 35, and the curling canary (scene
 * 374, 300 frames) reproduces its 32557/32557/0 across this repair.
 *
 * WHAT DISPATCHES IT HERE, MEASURED RATHER THAN ASSUMED. An earlier version of
 * this block said "this class dispatches it every frame" through
 * func_ov006_021057f0. THE RUN SAYS ONCE, and says which caller. Four bodies in
 * dScMgPanel_c's closure hold a genuine `ldr rN,[rM,#0x8c]` dispatch --
 * func_ov006_021053a8 (0x02105480), _021057f0 (0x021057fc), _02106168
 * (0x021061ec) and _021063a0 (0x021063b0). THREE of the four now have a
 * compiled body: run mg7 lane L380 decompiled 02106168, so only 021053a8 is
 * still bodiless, and it is never entered. Of the three that remain,
 * func_ov006_021063a0 has exactly TWO callers in the whole of ov006 (relocs
 * from:0x0210779c inside slot 0 and from:0x02107294 inside slot 18) and
 * func_ov006_021057f0 has exactly one (from:0x02106ecc, inside
 * func_ov006_02106eb8). And func_ov006_02106eb8 has NO arm_call anywhere in
 * the overlay at all -- its single reference is a LOAD from 0x0213dc6c, which
 * is pair p5 of data_ov006_02142888 -- so it is reachable only as a state, and
 * a run in which no state 5 is dispatched is a run in which 021057f0 cannot
 * execute.
 *
 * So on the mg6 1200-frame boot, with init 1, state-reset 0 and a slot-35
 * count of exactly 1, the single dispatch was slot 0 -> 021063a0 on the INIT
 * PATH, and 021057f0 never ran at all -- the class settled in its top-level
 * bodiless state long before state 5 came round again. THE REPAIR IS STILL
 * NECESSARY AND ITS REASON MOVES: it is not that a constant 0 would misfire
 * every frame, it is that the calls that do happen decide which FACE SET
 * InitResources deals.
 *
 * THE COUNT IS NOT 1 ANY MORE, run mg7 lane L380, and the reason is the third
 * dispatch site gaining a body. func_ov006_02106168 dispatches slot 35 ONCE
 * PER PANEL, inside its deal loop, and it re-deals until the board passes
 * three tests -- so the per-boot count is now (panels x deals) + 1 rather than
 * 1, and a run that re-deals reads a multiple of the panel count plus one.
 * A slot-35 census of exactly 1 on scene 380 is now the signature of a layout
 * picker that did not run. */
static int  __fastcall mb_v35(void *s, void *)
{ MG_SLOT(35); return func_ov004_020ad660((int *)s); }

/* dScMgBase_c's OWN five, and they are the five the first wired boot found
   missing. Run link60 lane MG2.

   MG1's list below is the twenty-three slots a derived class INHERITS
   UNCHANGED, which is the right set for a derived table and is one short of
   the right set for the BASE table. dScMgBase_c also holds its own bodies in
   the five slots dScMgCurling_c overrides -- 6 Behavior, 9 Render, 16 D2,
   17 D0, 18 state reset -- and nothing keyed them, so the first run that
   reached the fill printed its own warning:

     [scene] MINIGAME FILL INCOMPLETE: dScMgBase_c leaves 5 of 36 raw DS
     words, dScMgCurling_c leaves 0

   which is the diagnostic doing exactly what it was written for. Slot 0's
   base body is arm9 (0x02043c80) and port_scene_fill_rom already had it, so
   five and not six.

   THEY GO IN THE SAME ADDRESS-KEYED ARRAY, which is safe and is also the
   point: dScMgCurling_c's table does not hold any of these five words, so
   they cannot land there, and a FAN-OUT class that leaves one of the five
   un-overridden now gets the base thunk instead of a raw DS word. The
   counters are per SLOT rather than per body, so a base body and a derived
   body both report as that slot being entered, which is what the slot
   counters mean. */
static int  __fastcall mb_beh_base(void *, void *)
{ MG_SLOT(6);  return func_ov004_020b0618(); }
static int  __fastcall mb_ren_base(void *, void *)
{ MG_SLOT(9);  return func_ov004_020b04ec(); }
static void *__fastcall mb_d2_base(void *s, void *)
{ MG_SLOT(16); return func_ov004_020b2a84(s); }
static void *__fastcall mb_d0_base(void *s, void *)
{ MG_SLOT(17); return func_ov004_020b2a18(s); }
static int  __fastcall mb_reset_base(void *, void *, int /*ridethrough*/)
{ MG_SLOT(18); func_ov004_020b299c(); return 1; }

/* The framework's own twenty-three, keyed on the ROM word each slot holds, so
   the same list serves EVERY minigame class: a derived class that overrides
   one of them simply does not hold that word. This is what makes the fan-out
   cheap, and it is the whole reason the fill is address-keyed. */
struct MgFace { unsigned ds; void *host; };
static const MgFace kMgBaseFaces[] = {
    {0x020b0930u, (void *)mb_binit},  {0x020b08f0u, (void *)mb_ainit},
    {0x020b0840u, (void *)mb_aclean}, {0x020b0620u, (void *)mb_bbeh},
    {0x020b04f4u, (void *)mb_bren},   {0x020b04e8u, (void *)mb_pdes},
    {0x020b2994u, (void *)mb_v19},    {0x020b2990u, (void *)mb_v20},
    {0x020b298cu, (void *)mb_v21},    {0x020ae198u, (void *)mb_v22},
    {0x020ae1a0u, (void *)mb_v23},    {0x020ae140u, (void *)mb_v24},
    {0x020ae128u, (void *)mb_v25},    {0x020b04e0u, (void *)mb_v26},
    {0x020af27cu, (void *)mb_v27},    {0x020af04cu, (void *)mb_v28},
    {0x020af094u, (void *)mb_v29},    {0x020aeed8u, (void *)mb_v30},
    {0x020b2880u, (void *)mb_v31},    {0x020b27f4u, (void *)mb_v32},
    {0x020b265cu, (void *)mb_v33},    {0x020ae3b4u, (void *)mb_v34},
    {0x020ad660u, (void *)mb_v35},
    /* the base class's own five, run link60 lane MG2 */
    {0x020b0618u, (void *)mb_beh_base},   {0x020b04ecu, (void *)mb_ren_base},
    {0x020b2a84u, (void *)mb_d2_base},    {0x020b2a18u, (void *)mb_d0_base},
    {0x020b299cu, (void *)mb_reset_base},
};

/* dScMgCurling_c's own six, the per-class half. The fan-out writes one of
   these arrays per minigame and reuses everything above it. */
static const MgFace kCurlingFaces[] = {
    {0x020e3578u, (void *)mg_init},   {0x020e3528u, (void *)mg_beh},
    {0x020e34ecu, (void *)mg_render}, {0x020e0638u, (void *)mg_d2},
    {0x020e065cu, (void *)mg_d0},     {0x020e3470u, (void *)mg_reset},
};

/* ---- THE FAN-OUT SEAM, run mg5 lane FLW ----------------------------------
 *
 * ONE exported function, and it is the whole of what this file gives a
 * fan-out lane. Everything a per-class seat needs from the framework half is
 * behind it -- the mounts, the address-keyed arm9 fill, and kMgBaseFaces --
 * so a lane seating its own class writes its own file and does not copy the
 * twenty-eight-row base array into it.
 *
 * COPYING THAT ARRAY IS THE FAILURE THIS PREVENTS, and it is not a style
 * point. kMgBaseFaces is keyed on the ROM WORD a slot holds, so a second copy
 * in another translation unit would install a DIFFERENT set of host thunks
 * over the same DS addresses in the same mounted tables, and the per-slot
 * counters at the top of this file would then count only whichever copy ran
 * last. The witness would read low and nothing would say so.
 *
 * It deliberately does NOT run the constructors and does not print the
 * raw-word census: the first is gated on the requested id and belongs to the
 * calling fill (port_scene_mg_prepare), and the second has to be reported per
 * TABLE by a caller that knows how many tables its class has. dScMgFlower_c
 * has three. */
extern "C" unsigned port_scene_mg_fill_shared(void **vt, unsigned n);

static unsigned mg_apply(void **vt, unsigned n, const MgFace *f, unsigned nf)
{
    unsigned hit = 0;
    for (unsigned i = 0; i < n; ++i) {
        const unsigned ds = (unsigned)(size_t)vt[i];
        for (unsigned k = 0; k < nf; ++k)
            if (f[k].ds == ds) { vt[i] = f[k].host; ++hit; break; }
    }
    return hit;
}

static void port_scene_mg_mounts(void);

extern "C" unsigned port_scene_mg_fill_shared(void **vt, unsigned n)
{
    port_scene_mg_mounts();
    unsigned hit = port_scene_fill_rom(vt, n);
    hit += mg_apply(vt, n, kMgBaseFaces,
                    sizeof kMgBaseFaces / sizeof kMgBaseFaces[0]);
    return hit;
}

/* Count the words still holding a DS address, which is the only honest check
   that the fill is complete. A minigame table is 36 slots and every one of
   them is dispatched by something, so a nonzero answer here is a wild call
   waiting to happen and the seat says so out loud rather than booting. */
static unsigned mg_raw_left(void **vt, unsigned n)
{
    unsigned left = 0;
    for (unsigned i = 0; i < n; ++i) {
        const unsigned w = (unsigned)(size_t)vt[i];
        if (w >= 0x02000000u && w < 0x02400000u)
            ++left;
    }
    return left;
}

// ---- the overlay load ------------------------------------------------------
//
// The port's stand-in for what func_0201a798 does on the DS: bring ov004 and
// then ov006 up, in that order, which is the ROM's order and not a preference.
/* THE MOUNTS, brought up on EVERY boot, which is the ov007 seat's shape and is
   correct for the same reason: a mount patch only rebases the mount's own
   in-span DATA pointers, it writes nothing a level can observe, and the fill
   below has to run on every boot anyway so that /OPT:REF sees the ROM's own
   spawn-table edge on every target. This is also the FIRST CALLER either mount
   has ever had -- port/ov004_ov006_binding_diff.txt section 4 records that
   /OPT:REF strips both generated pack checks because "a mount with no registry
   rows has no caller", which is why neither appears in the baseline map.
   SPLIT FROM THE CONSTRUCTORS DELIBERATELY. The constructors are NOT safe on
   every boot and the header block says why; keeping them in a separate
   function is what makes the difference visible rather than a comment. */
/* ---- THE MINIGAME GRAPHICS BLOCK, SEATED ------------------------------------
 *
 * Every VBlank, IRQ::VBlankHandler runs func_02019144, whose FIRST beat
 * dispatches the CURRENT GRAPHICS BLOCK's vtable slot 2 and returns before the
 * rest of the function when it answers 0. hal/scene_boot.cpp's
 * port_graph_block_beat is the port's copy of that beat, and it only dispatches
 * a block whose vtable the port has SEATED -- every other block still holds raw
 * DS code addresses, and calling one would be a wild jump.
 *
 * Until this, exactly one block was seated: the title screen's
 * (scene_boot.cpp's one port_graph_block_register call). Every minigame parks
 * dScMgBase_c::graphCallback_c -- ov004's data_ov004_020beb74 -- in
 * data_0209d4a8 through its slot 33, and that block's vtable is
 * data_ov004_020bc03c, four words which the generated mount left as the ROM
 * wrote them:
 *
 *     {0x020ae0d4, 0x020ae0a4, 0x020ae06c, 0x020ae03c}
 *
 * so the beat refused it and slot 2 never ran. All four are FORWARDERS into the
 * scene's own 36-slot table, and the port had none of them compiled:
 *
 *     word 0  func_ov004_020ae0d4 -> scene vtable +0x5c, slot 23
 *     word 1  func_ov004_020ae0a4 -> +0x58, slot 22
 *     word 2  func_ov004_020ae06c -> +0x60, slot 24, AND RETURNS ITS ANSWER
 *     word 3  func_ov004_020ae03c -> +0x64, slot 25
 *
 * WHAT SLOT 24 IS. For the dScMgD3DBase_c family (372, 373, 384, 385) it is the
 * SCREEN SELECTOR, func_ov006_020e6e78: it toggles the live camera between the
 * two screens every frame, flips POWCNT1 bit 15 with it, arms the display
 * capture unit at alternating VRAM banks and moves those banks between LCDC and
 * engine B. That whole path is dark while this beat refuses the block.
 *
 * AND IT IS NOT ONLY THE FLIP. Slot 24 is also the only post-boot updater of
 * the four BG-offset shadows, so with it dead the beat's tail republished FROZEN
 * BOOT VALUES every frame -- measured on 377, whose BG registers sat at HOFS 256
 * / VOFS 432 while its scroll word moved through a whole run.
 *
 * THE ADAPTERS BELOW ARE CALLING-CONVENTION ONLY. Each is one line and calls the
 * ROM's own matched body; not one of them reimplements a forwarder. The port
 * dispatches a graphics-block word as __fastcall (this in ecx), which is what
 * scene_boot.cpp's ti_gc0..ti_gc3 are and what its Slot2 typedef says, while the
 * four src TUs are ordinary cdecl functions taking the block as their argument.
 * That is the whole of the difference and the whole of what these bridge.
 *
 * BLAST RADIUS, and it is why this is the last part of the feature to land.
 * Seating the block gates func_02019144's tail on slot 24's answer for EVERY
 * dScMgBase_c descendant at once, because the block is installed by slot 33
 * which every minigame runs. Non-D3D minigames inherit dScMgBase_c's own slot
 * 24, func_ov004_020ae140, which returns 1 on every path -- so their tail is
 * unchanged by construction rather than by hope. Scenes that never install the
 * block are untouched: installation is slot 33's act, not this function's.
 */
extern "C" void port_graph_block_register(void *vt);
extern "C" unsigned char data_ov004_020bc03c[];
extern "C" int func_ov004_020ae0d4(char *c);
extern "C" int func_ov004_020ae0a4(char *c);
extern "C" int func_ov004_020ae06c(char *c);
extern "C" int func_ov004_020ae03c(char *c);

namespace {
int __fastcall mg_gc0(void *c, void *) { return func_ov004_020ae0d4((char *)c); }
int __fastcall mg_gc1(void *c, void *) { return func_ov004_020ae0a4((char *)c); }
int __fastcall mg_gc2(void *c, void *) { return func_ov004_020ae06c((char *)c); }
int __fastcall mg_gc3(void *c, void *) { return func_ov004_020ae03c((char *)c); }
}  // namespace

static void port_scene_mg_gc_seat(void)
{
    void **vt = (void **)data_ov004_020bc03c;
    /* SAID ONCE, WITH THE WORDS IT REPLACED, because a seat that silently
       overwrites four pointers is indistinguishable from a seat that
       overwrote the wrong four. The `was` values are the ROM's own addresses
       and a reader can check them against the mount. */
    std::fprintf(stderr,
                 "  [scene] minigame graphics block seated: "
                 "data_ov004_020bc03c at %p was {%08X,%08X,%08X,%08X}\n",
                 (void *)vt, (unsigned)(size_t)vt[0], (unsigned)(size_t)vt[1],
                 (unsigned)(size_t)vt[2], (unsigned)(size_t)vt[3]);
    vt[0] = (void *)mg_gc0;
    vt[1] = (void *)mg_gc1;
    vt[2] = (void *)mg_gc2;
    vt[3] = (void *)mg_gc3;
    port_graph_block_register(vt);
    std::fflush(stderr);
}

static void port_scene_mg_mounts(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    port_ov004_pack_check();
    port_ov004_syms_patch();
    port_ov006_pack_check();
    port_ov006_syms_patch();
    /* AFTER THE MOUNT PATCHES AND NOT BEFORE. port_ov004_syms_patch rebases the
       mount's own data->data pointers, and it walks the same storage this
       writes host code addresses into; seating first would hand it four
       pointers that are not DS addresses and are not meant to be rebased. */
    port_scene_mg_gc_seat();
}

extern "C" void port_scene_mg_overlay_load(void)
{
    static int done;
    if (done)
        return;
    done = 1;

    /* idempotent, and already run by the fill on this boot. Called again so
       this function is correct read on its own. */
    port_scene_mg_mounts();

    /* ov004 FIRST, then ov006, because that is the order func_0201a798 loads
       them in and constructors are order-sensitive by definition. */
    __sinit_ov004_020b948c();
    __sinit_ov004_020b955c();
    __sinit_ov004_020b9ad0();
    __sinit_ov004_020b9b24();

    __sinit_ov006_0212f4c4(); __sinit_ov006_0212f52c();
    __sinit_ov006_0212f660(); __sinit_ov006_0212f6b4();
    __sinit_ov006_0212fc7c(); __sinit_ov006_0212fd48();
    __sinit_ov006_021300b0(); __sinit_ov006_0213014c();
    __sinit_ov006_021303d0();
    __sinit_ov006_021304ac(); __sinit_ov006_02130758();
    __sinit_ov006_02130a04(); __sinit_ov006_02130a08();
    __sinit_ov006_02130df8(); __sinit_ov006_02130e9c();
    __sinit_ov006_02130f00(); __sinit_ov006_02130f64();
    __sinit_ov006_021311c8(); __sinit_ov006_021314e4();
    __sinit_ov006_021318a0(); __sinit_ov006_0213195c();
    __sinit_ov006_02131a38(); __sinit_ov006_02131cd0();
    __sinit_ov006_02131fa4(); __sinit_ov006_021322bc();
    __sinit_ov006_02132894(); __sinit_ov006_02132970();
    __sinit_ov006_02132f68(); __sinit_ov006_0213322c();
    __sinit_ov006_0213326c(); __sinit_ov006_021333e0();

    std::printf("[scene] ov004+ov006 mounted and all 35 overlay "
                "constructors run (ov004 4/4, ov006 31/31)\n");
    std::fflush(stdout);
}

/* Gated on the id, ahead of the spawn. Split from the fill so a reader can see
   that the CONSTRUCTORS are gated and the FILL is not.

   RUN link60 LANE MG2 MOVED THE CALL SITE AND NOT THE RULING. MG1 wrote this
   to be called from port_scene_run; it is called from the fill below instead,
   reading the requested id from port_scene_env_want() rather than being handed
   one. Two reasons, and the first is the smaller:

     OWNERSHIP. port_scene_run is hal/scene_boot.cpp's, and this lane owns
     exactly one region of that file (the port_scene_classes row). A second
     edit there is another lane's to make.

     THE RULING SURVIVES INTACT, WHICH IS WHY THE MOVE IS ALLOWED AT ALL. The
     thing MG1's section 2 forbids is running these constructors on a boot that
     is not a minigame -- __sinit_ov004_020b948c calls func_020731dc, which
     threads a node onto an arm9-global destructor list the LEVEL path walks.
     The gate is what prevents that, not the call site, and the gate is still
     here: on a level run port_scene_env_want() answers -1, IsMinigameActorID
     declines, and not one of the thirty-three runs. The ORDER is preserved as
     well. The fill is reached from port_scene_registry_install at the tail of
     port_stage_a2_seat, which is before port_scene_run reaches the spawn, and
     on the DS LoadOverlay runs an overlay's static-init range before the
     factory is called. Constructors-before-factory is the ROM's order and it
     is still the port's. */
extern "C" void port_scene_mg_prepare(int id)
{
    if (!IsMinigameActorID((unsigned)id))
        return;
    port_scene_mg_overlay_load();
}

/* ---- THE MINIGAME RNG SEED (run mg5, lane RNGSEED) ------------------------
 *
 * THE COMPLAINT. "Loves Me...?" opens with the same number of petals on every
 * launch. It is not the petal code. It is the arm9 RNG state, and on the
 * port's direct scene boot that state is FROZEN.
 *
 * 1. THE ROM DOES NOT SEED FROM A CLOCK, AND THERE IS NO RTC READ ANYWHERE ON
 *    THIS PATH. src/RandomIntInternal.c is a plain LCG,
 *
 *        *seed = *seed * 0x19660d + 0x3c6ef35f
 *
 *    and its seeder src/func_0203b9b4.c is a one-line store. Both arm9 states
 *    are set to CONSTANTS and never to anything else:
 *
 *        __sinit_02074dc4    func_0203b9b4(&data_0209d4b8, 1)
 *        func_020196cc       func_0203b9b4(data_0209d4b8, 0x4d2)
 *                            <- the boot value, called once from
 *                               src/func_0201a054.c after PrepareToSpawnBoot
 *        __sinit_02074e44    func_0203b9b4(&data_0209e650, 1)
 *
 * 2. SO THE ENTROPY IS NOT IN THE SEED, IT IS IN THE DRAW COUNT. The function
 *    this seeder replicates is
 *
 *        func_ov005_020c14a0   dScMiniGm_c::Behavior, at 0x020c14a0
 *
 *    the minigame MENU scene's per-frame tick. Two of its statements are
 *
 *        RandomIntInternal(&data_0209d4b8);
 *        RandomIntInternal(&data_0209e650);
 *
 *    with both results thrown away. That is the whole mechanism. The menu
 *    advances both states once per frame for as long as a player sits in it,
 *    so HOW LONG A HUMAN TOOK TO CHOOSE A GAME is what decides the layout of
 *    the game they chose. A DS that reaches "Loves Me...?" is a DS whose state
 *    is the boot constant advanced N times, N being the frames spent in the
 *    menu. Nothing else on the way in touches data_0209d4b8.
 *
 * 3. WHY THE PORT IS FROZEN. The launcher's F5 relaunches straight into
 *    SM64DS_SCENE=<id>. dScMiniGm_c never runs, so N is zero -- and the port
 *    runs neither of the ROM's two constant seeders on this path either, so
 *    the state is whatever .bss left it: literally 0, since
 *    hal/scene_mg_faces.cpp hosts data_0209d4b8 as zero-initialised storage.
 *    Every boot therefore replays ONE sequence. Modelled against the ROM's own
 *    arithmetic the successive rounds of scene 390 are
 *
 *        12, 20, 9, 10, 15, 14, ...
 *
 *    and the measured first round of the unfixed binary is 12, on every frame
 *    budget from 60 to 300. That is the complaint, exactly.
 *
 *    IT ALSO RETIRES AN EARLIER LANE'S "IT DOES VARY" READING. A lane that saw
 *    the count come out 10 and 12 was not seeing entropy: those are round 1
 *    and round 4 of that one fixed sequence, reached by a run that got further
 *    in. +0x5fd8 is ALSO decremented once per petal plucked
 *    (src/func_ov006_0212ac74.c), so a run with hands on it reports a smaller
 *    number for a third reason. All three are positions in a frozen sequence.
 *
 * 4. WHAT THIS DOES. The faithful equivalent of the journey the launcher
 *    skips: put both states back to the ROM's own boot constants and then run
 *    the menu's two draws N times, N being a menu dwell taken from the host
 *    clock. Every state this can produce is a state the ROM reaches by a
 *    player taking N frames to choose, so this invents no distribution -- it
 *    lands on the ROM's own, which for scene 390 is 8..15 nine times in ten
 *    and 20..21 one time in ten.
 *
 *    GENERIC AT THE MINIGAME LATCH, NOT PER GAME. The gate is the ROM's own
 *    IsMinigameActorID, the same predicate port_scene_mg_prepare uses, so
 *    Bob-omb Squad's spawn patterns, Slots Shot, Coincentration's layouts and
 *    Wanted's rounds all get the same unfreezing from this one place. THE
 *    LEVEL PATH IS NOT TOUCHED: port_scene_begin is the scene path only, and
 *    this refuses any id the predicate declines. (The ROM has nothing to
 *    replicate there anyway -- func_020196cc's 0x4d2 is a boot constant for
 *    the whole game, and the level path has no per-frame stirrer of its own
 *    the way the minigame menu does.)
 */

/* THE GATING RULE, VERBATIM:
 *
 *     The seed varies only when the run is WINDOWED and UNBOUNDED and carries
 *     no scripted-input or capture knob. Windowed means port_scene_begin was
 *     handed a real hwnd. Unbounded means SM64DS_SCENE_FRAMES is unset.
 *     Everything else -- every selftest, every battery row, every BMP gate,
 *     every scripted pad or touch probe, every paced run -- is seeded-fixed,
 *     which means NOT SEEDED AT ALL: the state is left at the .bss zero it has
 *     today and every draw reproduces byte for byte.
 *
 * WHY WINDOWED-AND-UNBOUNDED IS THE RIGHT PAIR, and why neither half alone
 * would do. `windowed` is not re-derived here: port_scene_want_window() in
 * tests/walk_window.cpp already owns that decision and its three gates, and
 * the ANSWER arrives here as port_scene_begin's hwnd argument -- a real window
 * or nullptr. A second copy of that predicate is the thing
 * port/scene_window.txt section 3 warns about, so there is not one.
 *
 * But windowed alone is NOT sufficient, and this is the trap the rule exists
 * for: SM64DS_SCENE_WINDOW=1 with SM64DS_SCENE_FRAMES=N is a REAL WINDOW
 * DRIVEN BY A SCRIPT, and it is how every touch and pad claim on this path is
 * proved without hands. Those runs have an hwnd and must stay byte-identical.
 * The frame budget is what separates them: a run that says how many frames to
 * run is a MEASUREMENT, and a run that does not is a SESSION. Nobody sitting
 * down to play "Loves Me...?" says how many frames to play it for, and no
 * automated run in this tree omits the number.
 *
 * THE NAMED KNOBS BELOW ARE BELT AND BRACES AND ARE MEANT TO BE. Every one of
 * them is already excluded by the frame budget today -- battery.py's scene_env
 * sets SM64DS_SCENE_FRAMES on every scene row, and so does every quoted scene
 * command line in port/ -- so none of these lines changes an answer now. They
 * are here so that the day somebody writes a capture or a scripted probe that
 * forgets to name a count, it comes out fixed rather than random, which is the
 * direction a wrong guess should fail in.
 *
 * SM64DS_FAULTS_FATAL IS DELIBERATELY NOT IN THE LIST. It is set by
 * real-shaped runs as well as by the battery, so gating on it would freeze the
 * very shape this is trying to unfreeze. It needs no line: the battery rows
 * that set it also set SM64DS_SCENE_FRAMES, and they are headless besides. */
static int rng_run_is_real_play(void)
{
    if (std::getenv("SM64DS_WINDOW_SELFTEST"))   return 0;
    if (std::getenv("SM64DS_SCENE_FRAMES"))      return 0;
    if (std::getenv("SM64DS_SCENE_BMP"))         return 0;
    if (std::getenv("SM64DS_SCENE_BMP_STACKED")) return 0;
    if (std::getenv("SM64DS_TOUCH_PROBE"))       return 0;
    if (std::getenv("SM64DS_PAD_TEST"))          return 0;
    if (std::getenv("SM64DS_PROBE_INPUT"))       return 0;
    if (std::getenv("SM64DS_PACE_DIVIDER"))      return 0;
    return 1;
}

/* HOW LONG THE PLAYER STOOD IN THE MENU. One to 3600 frames is a sixtieth of a
 * second to a minute at the DS's 60Hz, which is the honest range for choosing
 * a game off a grid, and the LCG scrambles the low bits hard enough that the
 * resulting counts spread across the ROM's distribution rather than cluster at
 * one end of it.
 *
 * TAKEN FROM THE HOST CLOCK ONCE, AT THE LATCH, which is the only source
 * available: the ROM's entropy is a human's reaction time, and a launcher that
 * skips the menu has no human to time. The counter is mixed with the process
 * id so two launches inside one clock tick -- a relaunch, or a scripted loop
 * -- cannot collide.
 *
 * SM64DS_RNG_MENU_FRAMES=<n> replaces this read entirely; see the seeder. */
static unsigned rng_menu_frames(void)
{
    /* <chrono> rather than windows.h, deliberately: this TU is thousands of
       lines of vtable seats, and pulling the platform header in for two clock
       reads is how a macro collision arrives in a file nobody expects one in.
       The two clocks are mixed because they answer different questions --
       system_clock moves with the wall date, steady_clock with this machine's
       uptime -- so two launches agree on neither. */
    unsigned long long e =
        (unsigned long long)std::chrono::system_clock::now()
            .time_since_epoch().count();
    e ^= (unsigned long long)std::chrono::steady_clock::now()
            .time_since_epoch().count() << 17;
    /* one round of a 64-bit mix, so the low bits are not just the counter's */
    e ^= e >> 33; e *= 0xff51afd7ed558ccdull;
    e ^= e >> 33; e *= 0xc4ceb9fe1a85ec53ull;
    e ^= e >> 33;
    return (unsigned)(e % 3600ull) + 1u;
}

extern "C" void port_scene_mg_seed_rng(int id, int windowed)
{
    if (!IsMinigameActorID((unsigned)id))
        return;

    /* Once per process. The latch is reached from port_scene_begin, which runs
       once, but a second call must never re-roll a state the scene has already
       drawn from. */
    static int done;
    if (done)
        return;
    done = 1;

    /* SM64DS_RNG_MENU_FRAMES=<n> PINS THE DWELL AND FORCES THE SEED. It is the
       only way to ask this code a question from a script, because the shape it
       normally fires in -- windowed and unbounded -- is by definition a run
       that never ends by itself, and a 200-sample sweep of the distribution
       cannot be done by hand two hundred times.

       FORCING IS SAFE HERE AND WOULD NOT BE IF THE KNOB CARRIED ENTROPY. It
       carries a NUMBER: the same n always walks the LCG the same number of
       steps from the same two constants, so a run under it is exactly as
       reproducible as a run without it, just from a different base. That is
       what separates it from SM64DS_TOUCH_PROBE and SM64DS_PAD_TEST, which
       inject something a comparator cannot predict. It still gets a pop in
       both of tools/battery.py's env builders, on the principle that a step
       measuring a baseline should control every knob that can move it rather
       than rely on nobody setting it.

       It is also the REPRO HOOK: a player who reports a layout can have it
       reproduced exactly by pinning the dwell their playlog's seeded-varying
       line recorded. */
    const char *pin = std::getenv("SM64DS_RNG_MENU_FRAMES");

    if (!pin && !(windowed && rng_run_is_real_play())) {
        std::fprintf(stderr, "  [rng] seeded-fixed: data_0209d4b8 left at the "
                     "port's .bss zero, every draw reproducible (windowed=%d)\n",
                     windowed);
        std::fflush(stderr);
        return;
    }

    /* THE ROM'S BOOT CONSTANTS, then N frames of func_ov005_020c14a0's two
       draws. Open-coded rather than calling RandomIntInternal so this file
       takes no link dependency on an ov-scoped body for four lines of
       arithmetic; the constants are src/RandomIntInternal.c's, verbatim. */
    const unsigned n = pin ? (unsigned)std::strtoul(pin, nullptr, 0)
                           : rng_menu_frames();
    unsigned a = 0x4d2u;   /* func_020196cc's value for data_0209d4b8 */
    unsigned b = 1u;       /* __sinit_02074e44's value for data_0209e650 */
    for (unsigned i = 0; i < n; ++i) {
        a = a * 0x19660du + 0x3c6ef35fu;
        b = b * 0x19660du + 0x3c6ef35fu;
    }
    data_0209d4b8    = a;
    data_0209e650[0] = (int)b;

    std::fprintf(stderr, "  [rng] seeded-varying%s: %u menu frames of "
                 "func_ov005_020c14a0 (dScMiniGm_c::Behavior, 0x020c14a0) "
                 "replayed from the ROM's boot constants -- "
                 "data_0209d4b8=0x%08x data_0209e650=0x%08x\n",
                 pin ? " (pinned)" : "", n, a, (unsigned)b);
    std::fflush(stderr);
}

// ---- the fill --------------------------------------------------------------
extern "C" void port_scene_mg_hits(void);   /* defined at the foot of this file */
extern "C" void port_scene_mg_pachinko_report(void);  /* section 8, same */

extern "C" void port_scene_fill_curling(void)
{
    /* THE MOUNTS BEFORE THE FILL, and the order is real rather than
       defensive: port_scene_registry_install calls this fill at the tail of
       port_stage_a2_seat, which is BEFORE port_scene_run reaches
       port_scene_mg_prepare, so if the mounts were only brought up there the
       fill would run first every time. The two do not in fact collide -- the
       patch rebases DATA pointers and every word this fill writes is a CODE
       slot, which the binding diff measured as "left raw, target is ov006's
       own .text" -- but a fill that depends on that not colliding is a fill
       that breaks the day a mount grows. */
    port_scene_mg_mounts();

    void **base = (void **)data_ov004_020bc0c0;
    void **vt   = (void **)data_ov006_0213c304;

    /* THE BASE TABLE IS FILLED TOO, and it is not ceremony. The factory calls
       func_ov004_020b2adc, which writes data_0208e4b8, then _ZTV5Scene, then
       data_ov004_020bc0c0 into self[0] before the factory's own write of the
       derived table lands. Nothing dispatches in that window today -- every
       call the base ctor makes is direct -- but the base table is live
       storage inside a mounted span either way, and leaving thirty-six raw DS
       words in a table the ROM installs is the thing that produced the ov007
       lane's "eip 0x01cccab4 accessing 0x020ccab4" fault. */
    port_scene_fill_rom(base, 36);
    mg_apply(base, 36, kMgBaseFaces,
             sizeof kMgBaseFaces / sizeof kMgBaseFaces[0]);

    /* the derived table: shared arm9 words, then the framework's, then the
       class's own six. Order does not matter -- the three key sets are
       disjoint by construction, since a word is one address -- but it reads
       in inheritance order. */
    port_scene_fill_rom(vt, 36);
    mg_apply(vt, 36, kMgBaseFaces,
             sizeof kMgBaseFaces / sizeof kMgBaseFaces[0]);
    mg_apply(vt, 36, kCurlingFaces,
             sizeof kCurlingFaces / sizeof kCurlingFaces[0]);

    /* the two diagnostics, applied after the fill so they override it */
    {
        const char *s0 = std::getenv("SM64DS_SCENE_SLOT0");
        const char *s9 = std::getenv("SM64DS_SCENE_SLOT9");
        if (s0 && s0[0] == '0') vt[0] = (void *)mg_init_noop;
        if (s9 && s9[0] == '0') vt[9] = (void *)mg_render_noop;
    }

    {
        const unsigned lb = mg_raw_left(base, 36);
        const unsigned lv = mg_raw_left(vt, 36);
        if (lb || lv) {
            std::fprintf(stderr, "  [scene] MINIGAME FILL INCOMPLETE: "
                         "dScMgBase_c leaves %u of 36 raw DS words, "
                         "dScMgCurling_c leaves %u. A dispatch of any of them "
                         "jumps to a DS address as a host one.\n", lb, lv);
            std::fflush(stderr);
        }
    }

    /* THE CONSTRUCTORS, AND ONLY ON A MINIGAME BOOT. The gate is the ROM's own
       predicate on the requested id; see port_scene_mg_prepare's header for
       why the call is here rather than in port_scene_run, and why that does
       not weaken MG1's ruling. The fill runs FIRST so that a constructor which
       ever dispatched a vtable slot would find a host thunk rather than a DS
       word; none of the thirty-three does today (ten build SharedFilePtrs and
       the rest copy pair tables), so the order is a guard rather than a
       requirement, and it is the cheap direction to guard in. */
    port_scene_mg_prepare(port_scene_env_want());

    /* ---- THE PRE-FLIGHT CHECK FOR THIS SEAT'S ONE BLOCKER ----------------
       Run link60 lane MG2. dScMgBase_c's slot 1, BeforeInitResources, calls
       Scene::SetFaders(data_0209f61c) and then func_0202ec9c on the same
       object, and data_0209f61c is an arm9 dWipe_c in bss whose vptr the port
       never installs. (MG2 WROTE FaderBrightness HERE and lane FDR corrected
       it: the ROM's RTTI at 0x020926dc reads "7dWipe_c" over the base chain
       dFdColor_c -> dFdBrightness_c -> dFader_c, so FaderBrightness is the
       grandbase two levels up. It stays the right name for the
       Scene::SetFaders parameter, which is why the call spelling above is
       untouched, and the wrong one for the object.)
       src/__sinit_02074f80.c is the ROM's static initialiser for it --
       `func_0202fc40(data_0209f61c)` then a
       func_020731dc destructor registration -- and NOTHING IN THE PORT RUNS
       IT. func_0202fc40 is not even in the link.

       So the object's first word is zero, and Scene::SetFaders reaches
       `thiz->v24()`, which on the host compiles to
       `call dword ptr [eax+24h]` off a null vptr:

           FAULT c0000005 at _ZN5Scene9SetFadersEP15FaderBrightness+0x24
           accessing 0x00000024

       THE CHECK EXISTS TO GIVE THAT A STABLE NAME. A link offset changes on
       every build and cannot be a battery marker; this line can, and it names
       the CAUSE rather than the address the symptom happened to land on. The
       fault is deliberately not prevented -- installing a plausible vptr is
       the guess port/tools/inferred_stub_guard exists to refuse, and a scene
       that limps past its own blocker is worth less than one that names it.
       The day the fader constructor runs, this line stops printing and
       port/tools/battery.py's SCENE_BLOCKED probe reports BLOCK RETIRED. */
    if (IsMinigameActorID((unsigned)port_scene_env_want()) &&
        data_0209f61c[0] == 0) {
        std::printf("[scene] MINIGAME BLOCKED: the arm9 dWipe_c at "
                    "data_0209f61c has a NULL vptr, because "
                    "__sinit_02074f80 (func_0202fc40) does not run in the "
                    "port. dScMgBase_c slot 1 passes it to Scene::SetFaders, "
                    "which dispatches vtable slot 0x24 off it.\n");
        std::fflush(stdout);
    }

    /* ---- AND THE ONE BEHIND IT ------------------------------------------
       Run link60 lane FDR. The check above stopped printing when
       port/hal/fdr_arm9_fader_seat.cpp made the ROM's own __sinit_02074f80
       run, and the boot walked on to dScMgCurling_c::InitResources, where it
       faults on something with nothing to do with faders:

           FAULT c0000005 at func_0205cdf4+0x22 accessing 0x00000010
           mg_init -> func_ov006_020e3578 -> func_ov004_020adc74
                   -> func_020182bc -> func_02018e3c -> func_0205d644
                   -> func_0205d714 -> func_0205cdf4

       That tail is the ROM's NitroSDK file system opened BY NAME, which is a
       different seam from the id-based one hal/fs.cpp hosts and every other
       port file load goes through. func_0205d714 copies data_020a804c into
       the FSFile as its archive (`out->field_8 = t.field_0`), func_0205cdf4
       reads `list + 0x10` off it, and data_020a804c is zeroed host storage
       (hal/scene_boot.cpp:408, where it is sized 3 words and read as a VRAM
       bank record). Its only ROM writer is func_0205cc80's archive
       registration, which nothing in the port runs, so field_0 is null.

       RETIRED IN TURN, run link60 lane NFS, and it is now the SECOND live
       regression assertion in this block rather than a blocker.
       port/hal/fs_names.cpp runs the ROM's own func_0205cc80 at the ROM's own
       boot point, so data_020a804c holds a real archive and
       dScMgCurling_c::InitResources loads both of its files through the
       cartridge's own name table and RETURNS. The scene does not run yet: the
       check below this one is what stops it now.

       WHAT WAS ALSO WRONG ABOVE, corrected rather than deleted because the
       wrong reading is in three files and a successor will meet it: the
       parenthetical calling data_020a804c "a VRAM bank record" was
       hal/scene_boot.cpp's gloss and it is not what the symbol is. Every
       literal-pool reference to data_020a804c or data_020a8048 in the whole
       arm9 image is inside the FS module. data_020a804c is NitroSDK's
       12-byte FSDirPos current directory and data_020a8048 is the archive
       list head. The sizes hal/scene_boot.cpp gives them are right; only the
       names were wrong. See port/nfs_names_map.txt.

       BOTH CHECKS ARE KEPT IN ONE BLOCK, deliberately. A reader who comes
       here asking "why does scene 374 not boot" wants the whole history in
       one place, and each retired check is a live assertion about the seat
       that retired it: if anything ever de-seats the fader or the archive,
       the matching line starts printing again. */
    if (IsMinigameActorID((unsigned)port_scene_env_want()) &&
        data_020a804c[0] == 0) {
        std::printf("[scene] MINIGAME BLOCKED: the NitroSDK open-by-name "
                    "archive at data_020a804c is null, so "
                    "dScMgCurling_c::InitResources faults in func_0205cdf4. "
                    "hal/fs_names.cpp is supposed to have registered it "
                    "through the ROM's own func_0205cc80 before this runs, so "
                    "seeing this line means that seat came apart.\n");
        std::fflush(stdout);
    }

    /* ---- AND THE ONE BEHIND THAT, WHICH IS NO LONGER A BLOCKER ----------
       Run link60 lane NFS added this as the THIRD blocker in the chain: the
       fader seat let the scene reach InitResources, the file-system seat let
       InitResources finish, and what the scene reached next was the fader
       again -- a defect in that seat's stub ABI.

           func_02043288 -> mb_bbeh -> func_ov004_020b0620 (slot 7)
                         -> Scene::BeforeBehavior

       Scene::BeforeBehavior pushes two arguments into slot 0x0c and cleans
       NOTHING after the call, because MSVC's __thiscall is callee-cleans. The
       trap stub in the slot was `int __fastcall(void *, void *)`, which takes
       both parameters in registers and cleans nothing either, so eight bytes
       leaked and the caller's own epilogue read them back: pop esi took the
       0x1E, pop ebp took the 0, and ret popped the saved esi, which was the
       Scene pointer. Slot 0x10 carried the same defect on the other branch.

       RUN LINK60 LANE FDR2 FIXED THAT, and it was a signature defect rather
       than a missing body: the two stubs now declare the two stack parameters
       their call sites push and clean eight, and the audit of all twelve is
       port/fader_boot_map.txt section 9. The same lane then put the ROM's own
       func_0202f928 and func_0202f708 behind slots 0x0c and 0x10. Scene 374
       runs its 300 frames under SM64DS_FAULTS_FATAL=1 either way, so the
       SCENE_BLOCKED row in port/tools/battery.py is retired rather than
       converted a third time.

       AND THE ADVISORY THAT SAT HERE IS RETIRED, run link60 Stage 5 lane
       SEAT8. It printed "MINIGAME FADE MOTION MISSING" while slot 0x08 was a
       named trap, keyed on port_fdr_motion_slots_unseated() so it could not
       rot into a hardcoded 1. Slot 0x08 is func_0202f428 now, dispatched
       every frame by the ROM's own src/func_02018efc.c off data_0209d4ac, so
       the predicate had nothing left to ask and both it and this print are
       gone rather than left answering over a seated slot. The seat, the
       driver and the acceptance measurement are in port/fader_boot_map.txt
       section 4a and port/irq2_map.txt section 6.

       THE OTHER HALF OF THE OLD WARNING IS ALSO SPENT. It said the setters
       drive the wipe from IRQ 2 and nothing on the host raises it, so seating
       0x08 would not finish the job. Run link60 Stage 5 lane IRQ2 raised the
       HBlank edge out of ntr/rt.cpp's own VCOUNT sweep, and the table the
       setters build is read every scanline; port/irq2_map.txt is the
       derivation. */

    /* The witness has to report itself, because the generic one cannot.
       hal/scene_boot.cpp's end-of-run block chooses between ov003's and
       ov007's counters on `scene == 1` and has no third branch, and that
       block is not this lane's region to widen. std::atexit puts the ov006
       line after the run's own last line rather than inside it, which is a
       worse place for it but an honest one; generalising the block is a
       follow-up for whoever owns that file. Registered only on a minigame
       boot, so a level run prints nothing. */
    if (IsMinigameActorID((unsigned)port_scene_env_want())) {
        static int armed;
        if (!armed) {
            armed = 1;
            std::atexit(port_scene_mg_hits);
            /* Run mg5, lane PCH. Section 8's per-class witness, registered
               beside curling's rather than folded into it: each returns
               immediately when its own slot counters are all zero, so a run
               prints exactly the class it booted and the atexit order between
               the two does not matter. */
            std::atexit(port_scene_mg_pachinko_report);
        }
    }
}

/* The registry's factory column is void *(*)(void) and the matched factory
   returns int *. One typed forwarder rather than a cast through an
   incompatible function pointer, the same shape title_spawn has. */
/* The scene object, kept for the shell census below. Nothing else reads it and
   nothing may: this is an observation hook, not a back door into the class. */
static char *g_mg_curling_self;

extern "C" void *port_mg_curling_spawn(void)
{
    void *p = (void *)MgShuffleShell_Spawn();
    g_mg_curling_self = (char *)p;
    return p;
}

/* THE FIVE SHELLS, AT THE END OF A RUN. Off unless SM64DS_MG_CURLING_TRACE is
   set, so an ordinary run and every selftest frame are unaffected.

   It exists because the two collision bodies lane CUR2 seated are pure
   functions of this array, so "the shells collided" is a statement about these
   twenty numbers and nothing else. Reading them off a frame capture is
   guesswork; reading them here is not. The field offsets are the ones the
   matched physics TUs use (src/func_ov006_020e2868.c, src/func_ov006_020e2c08.c)
   and the two transcriptions in port/unmatched/ tabulate. */
static void port_mg_curling_shell_census(void)
{
    const char *e = std::getenv("SM64DS_MG_CURLING_TRACE");
    if (!e || !*e || *e == '0' || !g_mg_curling_self) return;
    char *c = g_mg_curling_self;
    /* The class fields the shot handler reads, because a shot that did not
       fire is a statement about these and not about the shells. +0x4eb0 and
       +0x4eb4 are the cursor, +0x4ee4 the aim sub-state, +0x4ee6 the next
       shell to place and +0x4ee7 the place-one-now flag; every one of them is
       read or written by src/func_ov006_020e2c08.c or src/func_ov006_020e2dbc.c
       in the matched tree. */
    std::printf("[scene] curling cursor: A %d B %d  substate %u  nextShell %u "
                "placePending %u\n",
                *(int *)(c + 0x4eb0) >> 12, *(int *)(c + 0x4eb4) >> 12,
                *(unsigned char *)(c + 0x4ee4), *(unsigned char *)(c + 0x4ee6),
                *(unsigned char *)(c + 0x4ee7));
    for (int i = 0; i < 5; ++i) {
        char *s = c + i * 0x2c;
        std::printf("[scene] shell %d: onBoard %u state %u fast %u  posA %d "
                    "posB %d  speed %d heading 0x%04x\n", i,
                    *(unsigned char *)(s + 0x4689), *(unsigned char *)(s + 0x4688),
                    *(unsigned char *)(s + 0x468b),
                    *(int *)(s + 0x4660) >> 12, *(int *)(s + 0x4664) >> 12,
                    *(int *)(s + 0x4668), *(unsigned short *)(s + 0x4686));
    }
}

extern "C" void port_scene_mg_hits(void)
{
    unsigned total = 0;
    for (int i = 0; i < 36; ++i) total += g_mg_hits[i];
    std::printf("[scene] ov006 slot hits: init %u, behavior %u, render %u, "
                "cleanup %u, pending-destroy %u%s\n",
                g_mg_hits[0], g_mg_hits[6], g_mg_hits[9], g_mg_hits[3],
                g_mg_hits[12],
                g_mg_render_skipped
                    ? "  [RENDER SLOT NO-OP'd: SM64DS_SCENE_SLOT9=0]" : "");
    if (g_mg_init_skipped)
        std::printf("[scene] INIT SLOT NO-OP'd: SM64DS_SCENE_SLOT0=0, %u "
                    "time(s)\n", g_mg_init_skipped);
    /* the framework half, which is the number the fan-out cares about: it says
       how much of dScMgBase_c a minigame boot actually exercises. */
    std::printf("[scene] 36-slot table, %u total slot entries across all "
                "slots; framework slots entered:", total);
    for (int i = 0; i < 36; ++i)
        if (g_mg_hits[i]) std::printf(" %d(x%u)", i, g_mg_hits[i]);
    std::printf("\n");

    /* THE STATE MACHINE'S OWN WITNESS, which is the number this seat exists to
       produce. The vtable counters above say the class was ticked; these say
       whether the pointer-to-member dispatch the wall was about actually ran,
       and whether it ever met an address the switch does not know. A run with
       slot hits and zero dispatch calls has booted the object without entering
       its state machine, and that reads as a success unless it is printed. */
    {
        unsigned calls = 0, unknown = 0;
        port_mg_dispatch_counts(&calls, &unknown);
        std::printf("[scene] state dispatch: %u call(s) through the address "
                    "switch, %u routed to a dScMgCurling_c state, %u UNHANDLED "
                    "address(es)\n", calls, port_mg_curling_state_hits(),
                    unknown);
    }
    /* Run mg5 lane BASESET. THE SETTER'S OWN WITNESS, and it is a separate
       number from the line above on purpose. func_ov004_020b87e0 is the only
       writer of the message object's +0x18, and the framework's two per-frame
       self-field dispatchers return on their first line while that field reads
       -1. So before this seat every minigame reported dispatch calls and zero
       unhandled addresses while dispatching NONE of the forty framework state
       and tick bodies, because none of them was ever reached. "setter 0" next
       to a nonzero call count is that state of affairs, and it is worth being
       able to see rather than infer. */
    {
        unsigned scalls = 0, sdisp = 0, sstates = 0, sticks = 0, sclos = 0;
        port_mg_base_setter_counts(&scalls, &sdisp, &sstates, &sticks, &sclos);
        std::printf("[scene] framework state setter: %u call(s) into "
                    "func_ov004_020b87e0, %u of them dispatched a state, %u "
                    "state body, %u per-frame tick and %u closure entr(ies) "
                    "routed by MgBase_StateSetter.cpp\n",
                    scalls, sdisp, sstates, sticks, sclos);
        {
            unsigned h[20] = {0};
            unsigned n = port_mg_base_setter_index_hits(h, 20);
            std::printf("[scene] framework message indices asked for:");
            unsigned any = 0;
            for (unsigned i = 0; i < n; ++i)
                if (h[i]) { std::printf(" %u(x%u)", i, h[i]); any = 1; }
            if (!any)
                std::printf(" none (no gameplay asked the framework to change"
                            " state on this run)");
            std::printf("\n");
        }
    }
    port_mg_curling_shell_census();
    std::printf("[scene] curling collision: shot separation 0x020e1dc8 "
                "entered %u time(s), shell-vs-shell 0x020e20bc entered %u "
                "time(s)\n",
                port_mg_curling_collide_020e1dc8_entries(),
                port_mg_curling_collide_020e20bc_entries());
    if (port_mg_trap_hits())
        std::printf("[scene] unmatched ov004/ov006 traps entered: %u\n",
                    port_mg_trap_hits());
    else
        std::printf("[scene] unmatched ov004/ov006 traps entered: 0 (none of "
                    "the 7 trapping sites was reached)\n");
    std::fflush(stdout);
}

// ============================================================================
// LANE LUI, run mg5: dScMgLuigi_c, actor id 0x16e = scene 366, "Wanted!".
// ============================================================================
//
// APPENDED AT THE END OF THE FILE ON PURPOSE. Five lanes of run mg5 are each
// adding one of these blocks to this file at once, and appending is the shape
// that concatenates. Everything above is reused unchanged: kMgBaseFaces,
// mg_apply, mg_raw_left, port_scene_mg_mounts, port_scene_mg_prepare,
// port_scene_fill_rom and the g_mg_hits counters are all shared, which is the
// split port/mg_fanout_costs.txt section 2 calls the whole cost model.
//
// THE CLASS, RE-DERIVED FROM THE ROM RATHER THAN FROM THE COST FILE. The
// derivation and the three width checks are in port/slice_lui.txt section 1;
// the short form is that the doubled-id word 0x016e016e sits at 0x0213ce10, so
// MgWanted_SpawnInfo is 0x0213ce0c and the factory word before the id pair is
// 0x020f3800; the factory's own load relocation (relocs.txt from:0x020f3830)
// names data_ov006_0213cf10; and the ROM's RTTI string at 0x0213ce60 reads
// "12dScMgLuigi_c". The spawn symbol says Wanted and the class says Luigi and
// both are the ROM's, which is curling's MgShuffleShell/dScMgCurling_c shape
// exactly.
//
// TWO TABLES, NOT THREE. Both deleting destructors (func_ov006_020efc0c slot
// 16 and func_ov006_020efc30 slot 17) store data_ov006_0213cf10 and then call
// func_ov004_020b29c0 with nothing in between, so the chain is
// Scene -> dScMgBase_c -> dScMgLuigi_c and the fill does the base's table and
// this one. dScMgCup_c needed a third because its destructors write an
// intermediate base's.
//
// ALL SEVEN OVERRIDE SLOTS, and the fill below writes every one of the
// thirty-six. "ruled" means the body was disassembled out of
// extracted/overlays/overlay_0006.bin at base 0x020bfec0 with
// port/tools/w13_dump.py and compared instruction for instruction with src/
// BEFORE it was seated; the per-body evidence is in
// port/tools/inferred_stub_adjudicated.txt.
//
//   slot  ROM word    module  body
//    0   020f3460    ov006   InitResources            ruled REAL_DECOMP
//    5   020efc68    ov006   AfterCleanupResources    ruled REAL_DECOMP
//    6   020f3414    ov006   Behavior                 ruled REAL_DECOMP
//    9   020f33c0    ov006   Render                   ruled REAL_DECOMP
//   16   020efc0c    ov006   D2                       (no marker, checked)
//   17   020efc30    ov006   D0                       ruled REAL_DECOMP
//   18   020f3294    ov006   state reset              ruled REAL_DECOMP
//   --   the other twenty-nine are dScMgBase_c's or arm9's and are already
//        keyed by address in kMgBaseFaces and port_scene_fill_rom
//   --   word 36 reads 2f474d2f, the ASCII "/MG/" of the path string slot 0
//        loads by name, which is what closes the table
//
// SLOT 18 TAKES AN ARGUMENT AND THE THUNK CLEANS IT. Every dispatch of vtable
// offset 0x48 in ov004 and ov006 was scanned out of the two overlay images
// word by word: twenty-two sites, and every ov006 one sets r1 first (mvn r1,#0
// at most of them, mov r1,#3/#4/#5 at five). The one ov004 site,
// func_ov004_020b29a0, does not set r1 -- and its own src spells the virtual
// as `v18(void*)` and forwards its second parameter, so the host side pushes
// an argument there too. So mgl_reset below declares the stack parameter and
// __fastcall cleans four, which is the signature repair
// port/fader_boot_map.txt section 9 audits twelve of. mg_reset and
// mb_reset_base above do NOT declare it; that is slice_mg1's to look at, and
// this comment is the only thing this lane does about it.
//
// SLOT 5 IS OVERRIDDEN AND STILL REACHES THE FRAMEWORK'S BODY.
// func_ov006_020efc68 tears the IRQ 2 handler down and then tail-calls
// func_ov004_020b0840 with both arguments, which is the ov004 body carrying
// port/mg_fanout_costs.txt section 6c's delete-with-no-pointer defect. The
// defect is inside that ov004 body, not in this class's override, so it is not
// repaired here.

extern "C" {

extern unsigned char data_ov006_0213cf10[];   /* dScMgLuigi_c, 36 slots */
extern unsigned char MgWanted_SpawnInfo[];

/* the seven override bodies. func_ov006_020f3414 is the host copy in
   port/unmatched/MgLuigi_StateDispatch.cpp, not the src TU: it is the class's
   pointer-to-member dispatch and the src cannot be compiled. */
int   func_ov006_020f3460(void *self);          /* slot  0 InitResources     */
int   func_ov006_020efc68(int self, int flag);  /* slot  5 AfterCleanup      */
int   func_ov006_020f3414(void *self);          /* slot  6 Behavior, HOSTED  */
int   func_ov006_020f33c0(void *self);          /* slot  9 Render            */
int   func_ov006_020efc0c(int *self);           /* slot 16 D2                */
int  *func_ov006_020efc30(int *self);           /* slot 17 D0                */
void  func_ov006_020f3294(char *self, int arg); /* slot 18 state reset       */

int *MgWanted_Spawn(void);

/* the class's state-dispatch witness; MgLuigi_StateDispatch.cpp */
void port_mg_luigi_counts(unsigned *hits, unsigned *floor, unsigned *nosrc);
/* the ov004 body with no source that slot 9 reaches; MgLuigi_Faces.cpp */
unsigned port_mg_luigi_ov004_trap_hits(void);

void port_scene_mg_luigi_hits(void);

}  /* extern "C" */

static int  __fastcall mgl_init(void *s, void *)
{ MG_SLOT(0);  const int r = func_ov006_020f3460(s);
  hal_gapless_minigames_latch(); return r; }
static void __fastcall mgl_aclean(void *s, void *, unsigned f)
{ MG_SLOT(5);  func_ov006_020efc68((int)(size_t)s, (int)f); }
static int  __fastcall mgl_beh(void *s, void *)
{ MG_SLOT(6);  return func_ov006_020f3414(s); }
static int  __fastcall mgl_render(void *s, void *)
{ MG_SLOT(9);  return func_ov006_020f33c0(s); }
static void *__fastcall mgl_d2(void *s, void *)
{ MG_SLOT(16); return (void *)(size_t)func_ov006_020efc0c((int *)s); }
static void *__fastcall mgl_d0(void *s, void *)
{ MG_SLOT(17); return (void *)func_ov006_020efc30((int *)s); }
static int  __fastcall mgl_reset(void *s, void *, int a)
{ MG_SLOT(18); func_ov006_020f3294((char *)s, a); return 1; }

/* the two diagnostics, the same pair the curling fill carries */
static int __fastcall mgl_render_noop(void *, void *)
{ ++g_mg_render_skipped; return 1; }
static int __fastcall mgl_init_noop(void *, void *)
{ ++g_mg_init_skipped; return 1; }

/* dScMgLuigi_c's own seven, keyed on the ROM word each slot holds. None of the
   seven appears in kMgBaseFaces or in kCurlingFaces -- a word is one address --
   so the three key sets stay disjoint by construction. */
static const MgFace kLuigiFaces[] = {
    {0x020f3460u, (void *)mgl_init},   {0x020efc68u, (void *)mgl_aclean},
    {0x020f3414u, (void *)mgl_beh},    {0x020f33c0u, (void *)mgl_render},
    {0x020efc0cu, (void *)mgl_d2},     {0x020efc30u, (void *)mgl_d0},
    {0x020f3294u, (void *)mgl_reset},
};

extern "C" void port_scene_fill_luigi(void)
{
    /* the mounts before the fill, the curling fill's reason verbatim */
    port_scene_mg_mounts();

    void **base = (void **)data_ov004_020bc0c0;
    void **vt   = (void **)data_ov006_0213cf10;

    /* The base table is filled here too. It is idempotent -- mg_apply keys on
       the DS word and a slot already holding a host thunk matches nothing --
       so the curling fill having done it on the same boot costs nothing, and
       this fill is correct read on its own. */
    port_scene_fill_rom(base, 36);
    mg_apply(base, 36, kMgBaseFaces,
             sizeof kMgBaseFaces / sizeof kMgBaseFaces[0]);

    port_scene_fill_rom(vt, 36);
    mg_apply(vt, 36, kMgBaseFaces,
             sizeof kMgBaseFaces / sizeof kMgBaseFaces[0]);
    mg_apply(vt, 36, kLuigiFaces,
             sizeof kLuigiFaces / sizeof kLuigiFaces[0]);

    {
        const char *s0 = std::getenv("SM64DS_SCENE_SLOT0");
        const char *s9 = std::getenv("SM64DS_SCENE_SLOT9");
        if (s0 && s0[0] == '0') vt[0] = (void *)mgl_init_noop;
        if (s9 && s9[0] == '0') vt[9] = (void *)mgl_render_noop;
    }

    {
        const unsigned lv = mg_raw_left(vt, 36);
        if (lv) {
            std::fprintf(stderr, "  [scene] MINIGAME FILL INCOMPLETE: "
                         "dScMgLuigi_c leaves %u of 36 raw DS words. A "
                         "dispatch of any of them jumps to a DS address as a "
                         "host one.\n", lv);
            std::fflush(stderr);
        }
    }

    /* THE CONSTRUCTORS, gated on the requested id exactly as the curling fill
       gates them, and idempotent behind port_scene_mg_overlay_load's own
       `static int done`. Whichever minigame row's fill runs first is the one
       that runs them; the rest are no-ops. */
    port_scene_mg_prepare(port_scene_env_want());

    if (port_scene_env_want() == 366) {
        static int armed;
        if (!armed) {
            armed = 1;
            std::atexit(port_scene_mg_luigi_hits);
        }
    }
}

/* The registry's factory column is void *(*)(void) and the matched factory
   returns int *. One typed forwarder, the shape port_mg_curling_spawn has.
   NO ARGUMENT IS DISPLACED HERE: src/MgWanted_Spawn.c calls
   func_ov004_020b2adc(p) WITH the object pointer, where dScMgCup_c's factory
   calls the same base constructor with none. This lane needs no displacement
   ruling and asks for none. */
extern "C" void *port_mg_luigi_spawn(void)
{
    return (void *)MgWanted_Spawn();
}

extern "C" void port_scene_mg_luigi_hits(void)
{
    std::printf("[scene] dScMgLuigi_c slot hits: init %u, behavior %u, "
                "render %u, cleanup %u, state-reset %u\n",
                g_mg_hits[0], g_mg_hits[6], g_mg_hits[9], g_mg_hits[5],
                g_mg_hits[18]);
    {
        unsigned hits = 0, floor = 0, nosrc = 0, calls = 0, unknown = 0;
        port_mg_luigi_counts(&hits, &floor, &nosrc);
        port_mg_dispatch_counts(&calls, &unknown);
        std::printf("[scene] dScMgLuigi_c state dispatch: %u routed to one of "
                    "its 23 reachable states (run mg5 lane INTEG seated 0x020f15ac "
                    "and 0x020f2790); residual floor/trap counters for those two "
                    "are %u/%u and stay 0 now they are real; the framework switch "
                    "saw %u call(s) and %u UNHANDLED address(es)\n",
                    hits, floor, nosrc, calls, unknown);
        /* THE RENDER SLOT'S OWN FLOOR, printed next to the render hit count
           above so the two are read together. A run with 300 render entries
           and 300 trap entries has dispatched the slot and drawn nothing
           through it, which is a different fact from "Render ran" and is the
           one this line exists to keep visible. */
        std::printf("[scene] dScMgLuigi_c render floor: %u entry(ies) into the "
                    "former func_ov004_020b0e84 trap, which slot 9 reaches "
                    "through func_ov004_020b1e34. Run mg5 lane INTEG seated the "
                    "real score HUD, so the counter stays 0; run mg5 lane WTIMER "
                    "closed the veneer's dropped 0xe0/0x14/1 as PROVABLY DEAD "
                    "(port/unmatched/MgLuigi_Faces.cpp section 4)\n",
                    port_mg_luigi_ov004_trap_hits());
    }
    std::fflush(stdout);
}

// ---- 8. dScMgPachinko_c, actor id 0x170, scene 368 -------------------------
//
// Run mg5, lane PCH. THE SECOND MINIGAME CLASS SEATED, and it is deliberately
// additive: nothing above this line changed. The split
// port/mg_fanout_costs.txt section 2 calls the whole cost model is what makes
// that possible -- everything from the mounts to the thirty-five overlay
// constructors to kMgBaseFaces to port_scene_fill_rom is the family's and was
// paid once by lane MG2, and this section is one class's own six overrides,
// one face array, one fill and one factory forwarder.
//
// IDENTITY, RE-DERIVED FROM THE ROM BY THIS LANE rather than inherited:
//   RTTI       0x0213d960  "15dScMgPachinko_c"
//   SpawnInfo  0x0213d910  MgBobOmbSquad_SpawnInfo, doubled id 0x01700170 at +4
//   factory    0x020ff3ec  MgBobOmbSquad_Spawn
//   vtable     0x0213d9cc  data_ov006_0213d9cc, and the factory's only ov006
//              literal-pool load is that word, which is how the two are tied
//              together rather than by name
//
// THE TABLE IS 36 SLOTS AND THE 37th WOULD HIT THE NEIGHBOUR. This class was
// one of the twelve port/mg_fanout_costs.txt section 11 corrected from 37, so
// all three of that section's checks were re-run here before a line of this
// section was written. Span in config/arm9/overlays/ov006/symbols.txt is
// exactly 36 words (data_ov006_0213d9cc -> data_ov006_0213da5c); slot 35 holds
// 0x020ad660, the family terminator; and the word at index 36 is 0x021019e0
// with index 37 zero, which is an mwcc {code, adjustment} pair and not a slot.
//
// AND THE VICTIM HAS A NAME. src/__sinit_ov006_02131cd0.c line 89 reads
// `data_ov006_02142734.p4 = data_ov006_0213da5c;`, and that constructor is
// dScMgPachinko2_c's -- id 0x171, MgLakituLaunch, the SIBLING class. So a
// 37-slot fill here would write a host thunk over the code word of the
// sibling's state 4. 0x171 is not seated by this lane and the corruption would
// have been latent until it was.
//
// port_scene_fill_rom is called with 36 below and mg_raw_left checks 36.

extern "C" {

/* the mount storage this fill writes into */
extern unsigned char data_ov006_0213d9cc[];   /* dScMgPachinko_c, 36 slots */
extern unsigned char MgBobOmbSquad_SpawnInfo[];

/* the class's own six overrides, in slot order. FOUR ARE SLICED AND TWO ARE
   NOT WHAT THEY LOOK LIKE:
     slot  0  func_ov006_020fefc0  NO SRC AT ALL -- the named trap in
              port/unmatched/MgPachinko_Traps.cpp. This is the class's whole
              nosrc column in the fan-out cost table.
     slot  6  func_ov006_020fee24  sliced
     slot  9  func_ov006_020fedc4  sliced, MARKER, ruled REAL_DECOMP
     slot 16  func_ov006_020fa75c  sliced
     slot 17  func_ov006_020fa780  MARKER, ruled REAL_DECOMP, and EXCLUDED from
              the slice: it spells decl_common.h's shared VT/HEAP placeholders
              and bare VT is bound to the ov002 Enemy base table. Hosted as
              port_mg_pachinko_d0 in port/unmatched/MgPachinko_Dtor.cpp.
     slot 18  func_ov006_020fed58  sliced, MARKER, ruled REAL_DECOMP
   The three markers were disassembled out of the shipped overlay image and
   compared instruction for instruction with src before being seated; the
   rulings and their evidence are in port/tools/inferred_stub_adjudicated.txt. */
int   func_ov006_020fefc0(void *self);        /* slot  0 InitResources */
int   func_ov006_020fee24(void *self);        /* slot  6 Behavior      */
int   func_ov006_020fedc4(void *self);        /* slot  9 Render        */
int   func_ov006_020fa75c(void *self);        /* slot 16 D2            */
void *port_mg_pachinko_d0(void *self);        /* slot 17 D0, hosted    */
void  func_ov006_020fed58(void *self, int n); /* slot 18 state reset   */

/* the factory. IT NEEDS NO DISPLACEMENT RULING, and that was checked rather
   than assumed: port/mg_fanout_costs.txt section 12 found 0x169's factory
   calling the base constructor with NO argument where ARM rides r0 through,
   and granted a host copy for it. src/MgBobOmbSquad_Spawn.c does NOT have that
   defect -- it reads `func_ov004_020b2adc(p);`, with the argument -- so the
   matched TU is sliced and called directly. */
int  *MgBobOmbSquad_Spawn(void);

/* the class's state machine, port/unmatched/MgPachinko_StateDispatch.cpp */
void port_mg_pachinko_state_counts(unsigned *hits, unsigned *missing);
unsigned port_mg_pachinko_trap_hits(void);

/* ---- the score-record probe's inputs, all already linked ------------------
   Nothing here is new storage or a new body. These are the exact objects the
   ROM's own HUD path reads, named so the probe can PRINT the number rather
   than leave it to be read off a picture:

     data_ov004_020beb68   the live dScMgBase_c `this`, set by the base ctor
     func_ov004_020adc1c   returns *(u32*)(that + 0x4650)  -- the HIGH SCORE
     func_ov004_020adbc0   returns *(u32*)(that + 0x464c)  -- the SCORE
     func_ov004_020adc3c   (self->field_8 >> 8) & 0xff     -- minigame index
     data_0209caf4         the 36-entry x 20-byte save record table, hosted
                           in hal/level_boot.cpp as .dsstate$savblk0004 */
extern void *data_ov004_020beb68;
extern unsigned char data_0209caf4[];
int func_ov004_020adc1c(void);
int func_ov004_020adbc0(void);
int func_ov004_020adc3c(void *c);

}  /* extern "C" */

/* ---- SM64DS_MG_SCORE_TRACE ------------------------------------------------
   Read-only. Prints the raw stored words behind the two HUD numbers so the
   999999 the owner reported can be attributed to a stored value or to
   func_ov004_020b1ea4's `if (val >= 0xf423f) val = 0xf423f` clamp. Off unless
   the variable is set, so no battery run changes shape. */
static bool mg_score_trace(void)
{
    static int on = -1;
    if (on < 0) {
        const char *e = std::getenv("SM64DS_MG_SCORE_TRACE");
        on = (e && e[0] && e[0] != '0') ? 1 : 0;
    }
    return on != 0;
}

static void mg_score_dump(const char *where)
{
    if (!mg_score_trace()) return;
    unsigned char *g = (unsigned char *)data_ov004_020beb68;
    std::printf("[score:%s] data_ov004_020beb68 = %p\n", where, (void *)g);
    if (!g) { std::fflush(stdout); return; }
    const unsigned f8   = *(unsigned *)(g + 8);
    const int      idx  = func_ov004_020adc3c(g);
    const unsigned cur  = *(unsigned *)(g + 0x464c);
    const unsigned hi   = *(unsigned *)(g + 0x4650);
    const unsigned thd  = *(unsigned *)(g + 0x4654);
    std::printf("[score:%s] field_8 = %08x  minigame index = %d (0x%x)\n",
                where, f8, idx, idx);
    std::printf("[score:%s] +0x464c SCORE      = %u (0x%08x)\n", where, cur, cur);
    std::printf("[score:%s] +0x4650 HIGH SCORE = %u (0x%08x)%s\n", where, hi, hi,
                ((int)hi >= 0xf423f) ? "   <-- OVER THE 999999 CLAMP" : "");
    std::printf("[score:%s] +0x4654 third      = %u (0x%08x)\n", where, thd, thd);
    std::printf("[score:%s] getters: func_ov004_020adc1c()=%d "
                "func_ov004_020adbc0()=%d\n",
                where, func_ov004_020adc1c(), func_ov004_020adbc0());
    std::printf("[score:%s] data_0209caf4 = %p, record[%d][0] = %u\n",
                where, (void *)data_0209caf4, idx,
                (idx >= 0 && idx < 36)
                    ? *(unsigned *)(data_0209caf4 + idx * 20) : 0u);
    std::printf("[score:%s] table head:", where);
    for (int i = 0; i < 8; ++i)
        std::printf(" [%d]=%u", i, *(unsigned *)(data_0209caf4 + i * 20));
    std::printf("\n");
    std::fflush(stdout);
}

/* This class's own tick witness, kept SEPARATE from g_mg_hits rather than
   sharing it. Both tables are 36 slots and most of the slots are the same
   framework bodies, so a shared counter would make "which class ran" a
   question the report could not answer on a build that hosts both. */
static unsigned g_pch_hits[36];
#define PCH_SLOT(n) (++g_pch_hits[(n)])

/* ---- SM64DS_PCH_BALL_TRACE: where the slingshot balls are, per frame -------
 *
 * READ-ONLY, off unless the variable is set, and it exists because the one
 * claim the GaplessMinigames mod makes is a claim about a SEQUENCE OF ROWS that
 * no capture can carry: a still frame cannot show that a ball was on the
 * bottom screen's top row last frame and the top screen's bottom row this one.
 *
 * The two words per ball are the ones hal/gap_continuity.cpp reads and the
 * ones dScMgPachinko_c's own render loop reads a moment later -- slot i at
 * scene + 0x4ed8 + i * 0x38, X then Y, both Fix12, with the visible flag at
 * +0x4f0e of the same slot. The engine column applies the ROM's OWN band test,
 * transcribed from src/func_ov004_020aff38.cpp rather than restated:
 *
 *     world y in [-0x100 - G, -G)   the TOP engine, at y + 0xc0 + G
 *     else world y in [-0x40, 0xc0) the BOTTOM engine, at y
 *     else                          neither, and nothing is drawn
 *
 * so "band" in the column below is not an extra rule -- it is exactly the rows
 * that fail the first test and are then thrown away by OAM::Render's own
 * y + h < 0 cull, which is the hole the mod closes. */
static bool pch_ball_trace(void)
{
    static int on = -1;
    if (on < 0) {
        const char *e = std::getenv("SM64DS_PCH_BALL_TRACE");
        on = (e && e[0] && e[0] != '0') ? 1 : 0;
    }
    return on != 0;
}

static void pch_ball_dump(void *self, unsigned frame)
{
    if (!pch_ball_trace()) return;
    unsigned char *b = (unsigned char *)self;
    const int g = hal_screen_gap_raw();
    for (int i = 0; i < 0x30; ++i) {
        const int at = i * 0x38;
        if (!*(unsigned char *)(b + 0x4f0e + at)) continue;
        const int x = *(int *)(b + 0x4ed8 + at) >> 12;
        const int y = *(int *)(b + 0x4ed8 + 4 + at) >> 12;
        enum { ENG_NONE, ENG_TOP, ENG_BOTTOM };
        int engine, sy;
        if (y >= -0x100 - g && y < -g)   { engine = ENG_TOP;    sy = y + 0xc0 + g; }
        else if (y >= -0x40 && y < 0xc0) { engine = ENG_BOTTOM; sy = y; }
        else                             { engine = ENG_NONE;   sy = 0; }
        /* the sprite is 16x16 with a -8 cel offset, so this is the row the
           first pixel of it actually lands on, and it is what OAM::Render's
           cull is applied to */
        const int top_row = sy - 8;
        /* THE HOLE, named where it happens: the ball reached the bottom engine
           because the top engine's band test rejected it, and then every row of
           it landed above the bottom screen, so OAM::Render's `y + h < 0` threw
           it away and no engine drew it at all. With G at zero the top test
           reaches world -1 and this branch cannot be taken. */
        const int culled = (engine == ENG_BOTTOM && top_row + 16 <= 0);
        /* THE SLOT'S STATE BYTE, because where a ball is only means something
           next to what it is doing. src/func_ov006_020fe2e4.c writes 1 here on
           the stylus press edge (grabbed, and the ball now tracks the stylus at
           a captured offset) and src/func_ov006_020fe394.c writes 2 on the
           release (shot). A trace of positions alone cannot tell a ball that is
           being pulled from one that has just been fired at the same place,
           which is the distinction a drag that leaves the window turns on. */
        const int st = *(unsigned char *)(b + 0x4f0d + at);
        std::fprintf(stderr, "[ball] f%u G=%d slot%d world=(%d,%d) st=%d %s "
                     "sy=%d rows=[%d,%d]%s\n", frame, g, i, x, y, st,
                     engine == ENG_TOP      ? "top"
                     : engine == ENG_BOTTOM ? "bottom"
                                            : "none",
                     sy, top_row, top_row + 15,
                     culled ? "  CULLED (in the hinge)" : "");
    }
    /* THE PARACHUTING BOB-OMBS, the other half of what crosses the seam here,
       and they take a DIFFERENT route: 30 slots at scene + 0x4660, stride the
       same 0x38, live flag at +0x468c and state at +0x468f, drawn through
       func_ov004_020b023c -- which submits to BOTH engines unconditionally and
       lets each one's own clip decide, so there is no engine to pick and both
       rows are reported. They spawn at a flat -0x100000, world row -256, which
       is above the top screen at any G: this line is how far above is measured
       rather than assumed. */
    for (int i = 0; i < 0x1e; ++i) {
        const int at = i * 0x38;
        if (!*(unsigned char *)(b + 0x468c + at)) continue;
        const int x = *(int *)(b + 0x4660 + at) >> 12;
        const int y = *(int *)(b + 0x4664 + at) >> 12;
        std::fprintf(stderr, "[bomb] f%u G=%d slot%d world=(%d,%d) state=%d "
                     "topsy=%d botsy=%d\n", frame, g, i, x, y,
                     (int)*(unsigned char *)(b + 0x468f + at), y + 0xc0 + g, y);
    }
    std::fflush(stderr);
}

static int  __fastcall pch_init(void *s, void *)
{
    PCH_SLOT(0);
    const int r = func_ov006_020fefc0(s);
    /* AFTER THE REAL BODY, NOT INSTEAD OF IT. The class's own setter call is
       inside that body, and this is where the opt-in GaplessMinigames mod
       undoes it. Every seated minigame calls this, not just this one: the
       gapless TABLE decides which of them it engages for, and the ones it does
       not engage for are exactly the ones that have to be able to SAY so.
       hal/screen_gap.cpp carries the table and the consumer audit that says
       this is the right moment. */
    hal_gapless_minigames_latch();
    return r;
}
static int  __fastcall pch_beh(void *s, void *)
{
    PCH_SLOT(6);
    /* SM64DS_PCH_SCROLL_TRACE=1: the pan channels and the three ship slots,
       once per frame, stderr. TEMPORARY probe for the clipped-ships defect:
       ships copy -channel0.y at spawn, so the value here IS their altitude. */
    static int scroll_trace = -1;
    if (scroll_trace < 0) {
        const char *e = std::getenv("SM64DS_PCH_SCROLL_TRACE");
        scroll_trace = (e && e[0] && e[0] != '0') ? 1 : 0;
    }
    if (scroll_trace) {
        unsigned char *b = (unsigned char *)s;
        std::fprintf(stderr, "[pan] beh%u ch0=(%d,%d) ch1=(%d,%d) ch2=(%d,%d)",
                     g_pch_hits[6],
                     *(int *)(b + 0x5bfc) >> 12, *(int *)(b + 0x5c00) >> 12,
                     *(int *)(b + 0x5bfc + 0x14) >> 12, *(int *)(b + 0x5c00 + 0x14) >> 12,
                     *(int *)(b + 0x5bfc + 0x28) >> 12, *(int *)(b + 0x5c00 + 0x28) >> 12);
        for (int i = 0; i < 3; i++) {
            unsigned char *w = b + i * 0x18;
            std::fprintf(stderr, "  ship%d live=%d y=%d", i,
                         (int)*(unsigned char *)(w + 0x4e6d),
                         *(int *)(w + 0x4e5c) >> 12);
        }
        std::fprintf(stderr, "\n");
        /* One shot at frame 120: is the hull actually IN video memory? The
           layer census reads zero opaque; this says whether the map, the
           tiles or the palette is the empty link. */
        if (g_pch_hits[6] == 120) {
            volatile unsigned short *map = (volatile unsigned short *)0x0600a000;
            volatile unsigned char *chr = (volatile unsigned char *)0x06018000;
            volatile unsigned short *plt = (volatile unsigned short *)0x05000000;
            unsigned nzc = 0, nzm = 0, nzp = 0;
            for (unsigned k = 0; k < 0x4000; k++) if (chr[k]) nzc++;
            for (unsigned k = 0; k < 0x1000; k++) if (map[k]) nzm++;
            for (unsigned k = 0; k < 0x100; k++) if (plt[k]) nzp++;
            std::fprintf(stderr, "[hull] map[0..7]=%04x %04x %04x %04x %04x %04x %04x %04x  "
                         "nonzero: map %u/4096 chr %u/16384 pltt %u/256\n",
                         map[0], map[1], map[2], map[3], map[4], map[5], map[6], map[7],
                         nzm, nzc, nzp);
            unsigned lo = 0xffffffff, hi = 0;
            for (unsigned k = 0; k < 0x8000; k++) {
                if (((volatile unsigned char *)0x06018000)[k]) { if (k < lo) lo = k; hi = k; }
            }
            std::fprintf(stderr, "[hull] chr nonzero span 0x%x..0x%x (tiles %u..%u)\n",
                         lo, hi, lo / 32, hi / 32);
            /* which map ROWS carry cells whose tile has any nonzero byte */
            for (unsigned row = 0; row < 32; row++) {
                unsigned live = 0;
                for (unsigned col = 0; col < 64; col++) {
                    unsigned ent = (col < 32) ? row * 32 + col : 0x400 + row * 32 + (col - 32);
                    unsigned t = map[ent] & 0x3ff;
                    volatile unsigned char *tp = chr + t * 32;
                    for (int k2 = 0; k2 < 32; k2++) if (tp[k2]) { live++; break; }
                }
                if (live) std::fprintf(stderr, "[hull] map row %u: %u/64 cells have inked tiles\n", row, live);
            }
        }
    }
    const int r = func_ov006_020fee24(s);
    hal_gapless_splice();
    return r;
}
static int  __fastcall pch_render(void *s, void *)
{
    PCH_SLOT(9);
    if (mg_score_trace() && (g_pch_hits[9] <= 3 || (g_pch_hits[9] % 100) == 0)) {
        char tag[32];
        std::snprintf(tag, sizeof tag, "render%u", g_pch_hits[9]);
        mg_score_dump(tag);
    }
    /* BEFORE the ROM's render loop, so the numbers printed are the ones it is
       about to read, and counted off slot 9 so the frame column is the class's
       own render count rather than a host clock. */
    pch_ball_dump(s, g_pch_hits[9]);
    return func_ov006_020fedc4(s);
}
static void *__fastcall pch_d2(void *s, void *)
{ PCH_SLOT(16); return (void *)(size_t)func_ov006_020fa75c(s); }
static void *__fastcall pch_d0(void *s, void *)
{ PCH_SLOT(17); return port_mg_pachinko_d0(s); }
/* slot 18 takes a SECOND argument in this class where curling's takes none:
   src/func_ov006_020fed58.c is (char *c, int n) and the ROM reads r1 (cmp r1,#9
   at 0x020fed6c). The __fastcall face lands `this` in ecx and the ROM's r1 in
   the first stack slot, which is where the third parameter of this thunk sits. */
static int  __fastcall pch_reset(void *s, void *, int n)
{ PCH_SLOT(18); func_ov006_020fed58(s, n); return 1; }

/* dScMgPachinko_c's own six, the per-class half. */
static const MgFace kPachinkoFaces[] = {
    {0x020fefc0u, (void *)pch_init},   {0x020fee24u, (void *)pch_beh},
    {0x020fedc4u, (void *)pch_render}, {0x020fa75cu, (void *)pch_d2},
    {0x020fa780u, (void *)pch_d0},     {0x020fed58u, (void *)pch_reset},
};

/* ---- the award-delivery self-check ---------------------------------------
 *
 * WHAT IT MEASURES. dScMgPachinko_c's scoring is two functions:
 *
 *     src/func_ov006_020fb7e0.c   the per-item tick that decides an award
 *     src/func_ov006_020fbb2c.c   the award itself, which adds the points to
 *                                 the score through func_ov004_020adb1c
 *
 * and the ROM hands the POINTS from the first to the second in r2, produced by
 * the same ldrh that makes the != 0 test (overlay_0006.bin at base 0x020bfec0):
 *
 *     020fb8bc  add  r0, r8, #0x5900
 *     020fb8c0  ldrh r2, [r0, #0x62]     ; *(u16*)(b + 0x5962), THE AWARD
 *     020fb8c4  cmp  r2, #0
 *     020fb8c8  beq  0x20fb8d8
 *     020fb8cc  mov  r0, sl              ; thiz
 *     020fb8d0  mov  r1, sb              ; i
 *     020fb8d4  bl   0x20fbb2c
 *
 * src spells only the test, declares the callee with TWO parameters and calls
 * it with two. On ARM that is correct and mwccarm accepted it, so the byte gate
 * has never had an opinion. On the host the callee reads its third parameter
 * off a caller stack slot nobody wrote. port/tools/aritycheck.py finds the same
 * split independently:
 *
 *     {"sym":"func_ov006_020fbb2c","def_n":3,
 *      "def_file":"src/func_ov006_020fbb2c.c","decl_n":2,
 *      "decl_file":"src/func_ov006_020fb7e0.c","kind":"DROPS"}
 *
 * HOW IT MEASURES IT WITHOUT PLAYING. The award path needs a stylus drag and a
 * live round, which no headless or scripted-stylus run reaches. So the check
 * drives the two functions directly on a scratch object shaped to take the one
 * branch that awards, with a sentinel in the ROM's own +0x5962 slot, and reads
 * back what the callee stored in +0x4cf8. If the delivery is right the two
 * numbers are equal.
 *
 * IT CANNOT TOUCH THE LIVE SCORE. func_ov004_020adb1c and func_ov004_020adbc0
 * both return immediately when data_ov004_020beb68 is null, so the check parks
 * that global for the duration and restores it. It runs from the fill, before
 * the scene is spawned, where the global is null anyway; the park is belt and
 * braces rather than the mechanism.  */
#define PCH_AWARD_SENTINEL 0x1234u

extern "C" void func_ov006_020fb7e0(char *thiz);

static void pch_award_abi_check(void)
{
    if (!mg_score_trace()) return;

    char *o = (char *)std::calloc(1, 0x5c38);
    if (!o) return;

    /* the one shape that reaches the award on the first loop iteration:
       item live, its 6-frame counter about to roll, its step counter one
       below the limit of 3, no sub-object, and a non-zero award. */
    *(unsigned char  *)(o + 0x5964) = 1;
    *(unsigned short *)(o + 0x5960) = 5;
    *(unsigned char  *)(o + 0x5965) = 2;
    *(unsigned char  *)(o + 0x5967) = 0;
    *(unsigned char  *)(o + 0x5968) = 0;
    *(unsigned short *)(o + 0x5962) = (unsigned short)PCH_AWARD_SENTINEL;

    void *save = data_ov004_020beb68;
    data_ov004_020beb68 = 0;
    func_ov006_020fb7e0(o);
    data_ov004_020beb68 = save;

    /* the award routine takes the first free slot, which on a zeroed object
       is index 0x1d, and stores the points it was handed at +0x4cf8 + n*0xc */
    const unsigned got = *(unsigned short *)(o + 0x4cf8 + 0x1d * 0xc);
    std::printf("[score:award] ROM hands *(u16*)(b+0x5962) = %u in r2; "
                "the callee received %u -> %s\n",
                PCH_AWARD_SENTINEL, got,
                (got == PCH_AWARD_SENTINEL) ? "DELIVERED"
                                            : "DROPPED (score is garbage)");
    std::fflush(stdout);
    std::free(o);
}

extern "C" void *port_mg_pachinko_spawn(void)
{
    void *p = (void *)MgBobOmbSquad_Spawn();
    mg_score_dump("postctor");
    return p;
}

extern "C" void port_scene_fill_pachinko(void)
{
    /* the same order and the same reasons as port_scene_fill_curling; see its
       header for why the mounts come before the fill and why the BASE table is
       filled too rather than only the derived one. */
    port_scene_mg_mounts();

    void **base = (void **)data_ov004_020bc0c0;
    void **vt   = (void **)data_ov006_0213d9cc;

    port_scene_fill_rom(base, 36);
    mg_apply(base, 36, kMgBaseFaces,
             sizeof kMgBaseFaces / sizeof kMgBaseFaces[0]);

    port_scene_fill_rom(vt, 36);
    mg_apply(vt, 36, kMgBaseFaces,
             sizeof kMgBaseFaces / sizeof kMgBaseFaces[0]);
    mg_apply(vt, 36, kPachinkoFaces,
             sizeof kPachinkoFaces / sizeof kPachinkoFaces[0]);

    {
        const unsigned lb = mg_raw_left(base, 36);
        const unsigned lv = mg_raw_left(vt, 36);
        if (lb || lv) {
            std::fprintf(stderr, "  [scene] MINIGAME FILL INCOMPLETE: "
                         "dScMgBase_c leaves %u of 36 raw DS words, "
                         "dScMgPachinko_c leaves %u. A dispatch of any of them "
                         "jumps to a DS address as a host one.\n", lb, lv);
            std::fflush(stderr);
        }
    }

    port_scene_mg_prepare(port_scene_env_want());

    pch_award_abi_check();
}

/* The run report for this class, called from the same place curling's is. */
extern "C" void port_scene_mg_pachinko_report(void)
{
    unsigned total = 0;
    for (int i = 0; i < 36; ++i) total += g_pch_hits[i];
    if (!total) return;               /* not this run's class */

    std::printf("[scene] dScMgPachinko_c slot hits: init %u, behavior %u, "
                "render %u, D2 %u, D0 %u, reset %u\n",
                g_pch_hits[0], g_pch_hits[6], g_pch_hits[9],
                g_pch_hits[16], g_pch_hits[17], g_pch_hits[18]);
    /* AND WHETHER THIS RUN WAS THE GAME. A run with GaplessMinigames engaged
       has different crossing timing from the DS by design, so the report a
       reader takes numbers out of has to say which of the two it is rather
       than leaving it to be found in the settings line further up. */
    std::printf("[scene] dScMgPachinko_c screen gap: %s\n",
                hal_gapless_world()
                    ? "SPLICED (GaplessMinigames -- objects skip the hinge "
                      "rows, NOT the ROM's timing)"
                    : hal_gapless_engaged()
                    ? "GAPLESS (GaplessMinigames engaged -- NOT the ROM's timing)"
                    : "simulated, as the ROM does it");
    if (hal_gapless_world())
        std::printf("[scene] gapless splice: %u carr%s across the hinge rows "
                    "this run\n", hal_gapless_splice_hops(),
                    hal_gapless_splice_hops() == 1 ? "y" : "ies");
    std::printf("[scene] dScMgPachinko_c 36-slot table, %u total slot "
                "entries; slots entered:", total);
    for (int i = 0; i < 36; ++i)
        if (g_pch_hits[i]) std::printf(" %d(x%u)", i, g_pch_hits[i]);
    std::printf("\n");
    {
        unsigned hits = 0, missing = 0;
        port_mg_pachinko_state_counts(&hits, &missing);
        /* Run mg5 lane PCOLL seated the last two floors (0x020fb230 and
           0x020fd2d8), so all 25 of this class's state addresses are real
           dispatches and `missing` is now a measured zero rather than a live
           count. It is still printed: a 0 here is the evidence that the switch
           in unmatched/MgPachinko_StateDispatch.cpp is complete. */
        std::printf("[scene] dScMgPachinko_c state dispatch: %u routed to one "
                    "of its 25 decompiled states, %u reached a state with NO "
                    "decompiled body (there are none left; lanes INTEG and "
                    "PCOLL seated the last three)\n", hits, missing);
    }
    std::printf("[scene] dScMgPachinko_c unmatched-body traps entered: %u\n",
                port_mg_pachinko_trap_hits());
    std::fflush(stdout);
}

// ============================================================================
// dScMgSmartball_c, actor id 0x178 (scene 376). Run mg5, lane SMB.
// ============================================================================
//
// THE SECOND MINIGAME CLASS, AND EVERYTHING ABOVE THIS LINE IS REUSED. The
// mounts, the thirty-five overlay constructors, the twenty-eight-entry
// kMgBaseFaces, port_scene_fill_rom, mg_apply, mg_raw_left, the two pre-flight
// assertions and the slot counters are all the family's and are paid. What is
// below is what a fan-out lane actually costs: nine override thunks, one face
// array, one fill and one factory forwarder.
//
// IDENTITY, RE-DERIVED FROM THE ROM. port/slice_smb.txt carries the full
// derivation; the three facts this file depends on are:
//
//   SpawnInfo   0x0213ebd0, found by scanning overlay_0006.bin for the
//               doubled-id word 0x01780178 (one occurrence, at 0x0213ebd4)
//   factory     0x02119824, the word before it, = MgBingoBallSlotsShot_Spawn
//   vtable      0x0213eefc, from the factory's own literal pool, 36 slots
//
// THE RTTI STRING AT 0x0213ec7c READS "16dScMgSmartball_c".
// MgBingoBallSlotsShot is the spawn symbol, after the two menu games the class
// serves, and the files it loads are all /MG/d_2d_mg_smart_ball_*. All three
// names are right and the class name is the ROM's, which is the rule lane MG1
// set for curling.
//
// THE WIDTH IS 36 AND SECTION 11's THREE CHECKS ALL SAY SO: the next config
// symbol after data_ov006_0213eefc is data_ov006_0213ef8c, exactly 36 words
// on; slot 35 holds 0x020ad660, the family's terminal word; and the word past
// it is 0x2f474d2f, ASCII "/MG/", the head of the class's own file-name table.
//
// TWO OF THE NINE OVERRIDES HAVE NO DECOMPILED BODY, and they are the two that
// matter most: slot 0 InitResources and slot 9 Render. Both are named,
// self-reporting traps in port/unmatched/MgSmartball_Traps.cpp, the
// func_ov006_020e1854 precedent. THE CONSEQUENCE IS STATED HERE RATHER THAN
// DISCOVERED AT A FAULT ADDRESS: slot 0 is where the class builds every one of
// its sub-objects, so with it trapped the object's count fields stay zero and
// its sub-object pointers stay null, and slot 6's case 0 dereferences
// self+0x4684 on its first statement without a null check. The pre-flight
// below says so before the spawn.
//
// SLOT 18 IS A HOST COPY AND NOT THE MATCHED TU. src/func_ov006_02118a8c.cpp
// declares func_ov006_02115b0c with no parameters and calls it with none,
// while src/func_ov006_02115b0c.c:113 defines it taking the object; ARM rides
// r0 through and the host does not. port/unmatched/MgSmartball_Slot18.cpp is
// the src body with the argument placed and the src TU is off the slice.
//
// THIS CLASS HAS NO POINTER-TO-MEMBER STATE MACHINE. Its Behavior switches on
// a plain int at self+0x4660, no ov006 overlay constructor copies a pair table
// into its .data neighbourhood, and a member-pointer sweep of all 76 closure
// TUs hits exactly one file, func_ov004_020b87e0.cpp, which is the FRAMEWORK's
// state setter and was a trap above when this block was written. So section 4's
// wall costs this lane nothing, and port/slice_smb.txt records how each of the
// three was measured. AMENDED, run mg5 lane BASESET: that setter is SEATED, in
// port/unmatched/MgBase_StateSetter.cpp, so what this class inherits from the
// framework is a working state machine rather than a reporting trap. The
// measurement about this class's own Behavior is unchanged.

extern "C" {
extern unsigned char data_ov006_0213eefc[];   /* dScMgSmartball_c, 36 slots */
extern unsigned char MgBingoBallSlotsShot_SpawnInfo[];

/* the class's own nine overrides, in slot order. Six carried the "recovered
   from vtable slot identity" marker and all six are ruled REAL_DECOMP against
   the shipped overlay image in port/tools/inferred_stub_adjudicated.txt before
   any of them was seated. */
int   func_ov006_02118b70(void *self);        /* slot  0 InitResources  TRAP */
void  func_ov006_0211944c(char *self, int f); /* slot  5 AfterCleanupResources */
int   func_ov006_02118488(void *self);        /* slot  6 Behavior       */
int   func_ov006_021173c8(void *self);        /* slot  9 Render         TRAP */
void *func_ov006_0210d740(char *self);        /* slot 16 D2             */
void *func_ov006_0210d7e0(void *self);        /* slot 17 D0             */
void  func_ov006_02118a8c(void *self);        /* slot 18 reset, HOST COPY */
int   func_ov006_021147ac(void *self);        /* slot 25                */
void  func_ov006_02118ae4(void);              /* slot 31, takes nothing */

int  *MgBingoBallSlotsShot_Spawn(void);

unsigned port_mg_smartball_trap_hits(void);
unsigned port_mg_smartball_trap_mask(void);
}  /* extern "C" */

/* THE THUNKS. Same shape and the same counters as curling's, because the
   counters are per SLOT rather than per class and only one scene runs in a
   process. Two of the arities are the ROM's rather than the family's and are
   not typos: slot 31's body takes nothing (its prologue is `push {r4,lr}` and
   it never reads r0) and slot 18's host copy takes the object the matched TU
   drops. */
static int  __fastcall smb_init(void *s, void *)
{ MG_SLOT(0);  const int r = func_ov006_02118b70(s);
  hal_gapless_minigames_latch(); return r; }
static void __fastcall smb_aclean(void *s, void *, unsigned f)
{ MG_SLOT(5);  func_ov006_0211944c((char *)s, (int)f); }
/* ---- THE BLOCKER, NAMED BEFORE IT HAPPENS AND NOT PREVENTED ---------------
   The same instrument, and the same ruling, that hal/scene_mg.cpp's fader
   pre-flight used for scene 374: print the CAUSE by name when the condition
   holds, then let the fault happen. A link offset changes on every build and
   cannot be a battery marker; this line can, and it names the field rather
   than the address the symptom lands on.

   WHY THE CONDITION EXISTS. Slot 0 InitResources (func_ov006_02118b70) has no
   decompiled body -- no delink block covers 0x02118b70 -- so it is a trap and
   nothing builds the class's sub-objects. src/func_ov006_02118488.c's case 0
   opens with

       o = *(char**)(c + 0x4684);
       (**(VFunc**)o)(o);

   with no null guard, because on the ROM there is nothing to guard: slot 0 ran.
   The fault is c0000005 accessing 0x00000000 at func_ov006_02118488+0x11b.

   NOT PREVENTED ON PURPOSE. Skipping the call, or seating a plausible object
   in that field, is the guess port/tools/inferred_stub_guard exists to refuse,
   and a scene that limps past its own blocker is worth less than one that
   names it. The day slot 0 is decompiled this line stops printing and
   port/tools/battery.py's SCENE_BLOCKED probe reports BLOCK RETIRED. */
static int  __fastcall smb_beh(void *s, void *)
{
    MG_SLOT(6);
    {
        static int said;
        char *c = (char *)s;
        if (!said && *(int *)(c + 0x4660) == 0 &&
            *(void **)(c + 0x4684) == 0) {
            said = 1;
            /* STDOUT, and that is not a style choice. The flight recorder
               REDIRECTS stderr into playlog/*.log rather than teeing it, so a
               marker written to stderr never reaches the battery's r.stderr
               and the SCENE_BLOCKED row would fail as "a different failure".
               Measured on this lane: the trap lines in
               port/unmatched/MgSmartball_Traps.cpp land in the playlog and
               nowhere else, and a bare run redirected with 2>&1 does not
               contain one. Every "MINIGAME BLOCKED" line already in this file
               uses std::printf, which is why scene 374's row worked. */
            std::printf(
                "[scene] MINIGAME BLOCKED: dScMgSmartball_c slot 6 Behavior "
                "is about to dereference a NULL self+0x4684, because slot 0 "
                "InitResources (func_ov006_02118b70) has no decompiled body "
                "and never built the object. This is a decomp gap, not a port "
                "one.\n");
            std::fflush(stdout);
        }
    }
    {
        const int r = func_ov006_02118488(s);
        hal_gapless_splice();
        return r;
    }
}
static int  __fastcall smb_render(void *s, void *)
{ MG_SLOT(9);  return func_ov006_021173c8(s); }
static void *__fastcall smb_d2(void *s, void *)
{ MG_SLOT(16); return func_ov006_0210d740((char *)s); }
static void *__fastcall smb_d0(void *s, void *)
{ MG_SLOT(17); return func_ov006_0210d7e0(s); }
static int  __fastcall smb_reset(void *s, void *, int /*ridethrough*/)
{ MG_SLOT(18); func_ov006_02118a8c(s); return 1; }
static int  __fastcall smb_v25(void *s, void *)
{ MG_SLOT(25); return func_ov006_021147ac(s); }
static int  __fastcall smb_v31(void *, void *)
{ MG_SLOT(31); func_ov006_02118ae4(); return 0; }

/* dScMgSmartball_c's own nine, the per-class half. Keyed on the ROM word each
   slot holds, exactly like kCurlingFaces, so the three key sets stay disjoint
   by construction. */
static const MgFace kSmartballFaces[] = {
    {0x02118b70u, (void *)smb_init},   {0x0211944cu, (void *)smb_aclean},
    {0x02118488u, (void *)smb_beh},    {0x021173c8u, (void *)smb_render},
    {0x0210d740u, (void *)smb_d2},     {0x0210d7e0u, (void *)smb_d0},
    {0x02118a8cu, (void *)smb_reset},  {0x021147acu, (void *)smb_v25},
    {0x02118ae4u, (void *)smb_v31},
};

// ---- THE SUB-OBJECT FAMILY: TWELVE THREE-SLOT VTABLES ---------------------
//
// Run mg5, lane SMBSEAT. This is the floor the INTEG seat named and could not
// clear: with slot 0 InitResources seated the class BUILDS its sub-objects, and
// the first method call on one of them jumped to 0x02114458 as a host address.
// Nothing about that was a decomp gap. The bodies were already in this tree;
// what was missing was the same address-keyed relocation every scene class
// above this line gets, applied to the tables the sub-objects carry.
//
// ---- IDENTITY, RE-DERIVED FROM THE ROM, AND THE ANNOUNCED ADDRESS IS OFF ---
//
// The 376 SEAT line used to say "a sub-object whose ov006 vtable at 0x0213eca0
// holds raw DS addresses". 0x0213eca0 is not a vtable; it is the WORD that
// holds 0x02114458, and it is the THIRD slot of a table whose vptr is
// 0x0213ec98. The correction matters because the naive reading -- treat
// 0x0213eca0 as a head and fill forward -- walks straight off the end of the
// table it is in.
//
// An Itanium record walk of extracted/overlays/overlay_0006.bin at base
// 0x020bfec0, looking for [0x00000000, ti] pairs whose ti resolves to a name
// string, finds TWELVE of them between 0x0213ec90 and 0x0213eda0, each on a
// 0x14 stride, each naming a cMgSmartball_ class:
//
//   vptr        class                        slots: s0, s1, s2
//   0x0213ec98  19cMgSmartball_ball_c        02113f1c 02113e54 02114458
//   0x0213ecac  20cMgSmartball_dokan_c       02110a20 02110928 02110bb4
//   0x0213ece8  24cMgSmartball_propeller_c   02111144 0211102c 021111f0
//   0x0213ecfc  21cMgSmartball_spring_c      0211134c 02111268 02111560
//   0x0213ed10  21cMgSmartball_object_c      02114724 02114720 02114738
//   0x0213ed24  19cMgSmartball_wing_c        0210e014 0210ddf0 0210e098
//   0x0213ed38  25cMgSmartball_pushswitch_c  0210e3e8 0210e364 0210e460
//   0x0213ed4c  18cMgSmartball_ana_c         021116f0 02111654 02111750
//   0x0213ed60  20cMgSmartball_board_c       0210f564 0210e4f4 0210f914
//   0x0213ed74  19cMgSmartball_slot_c        021100a8 0210ff1c 02110154
//   0x0213ed88  21cMgSmartball_kinoko_c      0211192c 021117bc 02111b20
//   0x0213ed9c  21cMgSmartball_pakkun_c      021106b4 021104c0 02110850
//
// The decomp names those three slots SaveSnapshot, Update and RestoreInitial
// in that order, and the ROM agrees: the sibling headers on main map each
// mangled body to the slot it sits in here.
//
// cMgSmartball_object_c is the ROOT and the other eleven are its direct
// children. Every one of the twelve symbols is already in the ov006 mount
// (port/ov006_syms.txt lines 445-447) and already in the map, so nothing new
// is mounted here; the words are simply relocated.
//
// ---- THE WIDTH IS 3, AND THE SPAN CHECK IS THE ONE THAT LIES --------------
//
// This project has corrupted a neighbouring class once by trusting a width it
// did not derive, so all the checks are written out.
//
//   1. THE SPAN CHECK, WHICH IS WRONG HERE AND IS THE WHOLE WARNING. The next
//      symbol in config/arm9/overlays/ov006/symbols.txt after
//      data_ov006_0213ec98 is data_ov006_0213ecac, 0x14 bytes on, which reads
//      as FIVE words. Five is an overrun into the next class: 0x0213eca4 holds
//      0x00000000 and 0x0213eca8 holds 0x0213ebf8, and the record walk above
//      identifies those two as cMgSmartball_dokan_c's OWN offset-to-top and
//      typeinfo pointer. Filling five would overwrite a neighbouring class's
//      RTTI with a host code address. The span reads 5 because config names the
//      VPTR of each table and not its head, so each symbol span covers three
//      slots plus the next table's two-word header.
//
//   2. THE STRUCTURE CHECK, which is what settles it. Every one of the twelve
//      is [0x00000000, typeinfo, f, f, f] on a 0x14 stride with no gap, and the
//      typeinfo word of record n+1 sits exactly where slot 4 of record n would
//      be. A 4-slot table anywhere in the run would break the stride, and none
//      does.
//
//   3. THE WORD PAST THE END. For eleven of the twelve, vt[3] is 0x00000000 --
//      not a code address, and ov006's .text does not reach address zero. For
//      the twelfth (pakkun, the last), vt[3] is 0x4d633831, which is ASCII
//      "18cM": the head of the RTTI name-string table, not code at all.
//
//   4. A FOURTH WITNESS THAT IS NOT THE ROM. Every cMgSmartball_ class
//      recovered on the decomp's main declares exactly three virtuals --
//      SaveSnapshot, Update, RestoreInitial -- and no virtual destructor.
//      Twelve independent headers, three slots each.
//
// ---- THE CONVENTION IS CDECL, NOT __fastcall, AND THAT IS DELIBERATE ------
//
// Every other thunk in this file is __fastcall (receiver in ECX) because every
// caller of a SCENE vtable in this binary is a host dispatcher that puts it
// there. The sub-object tables are the opposite case: their dominant caller is
// src/func_ov006_02118488.c, this class's own Behavior, which spells
// `typedef void (*VFunc)(void*)` and makes twelve `(**(VFunc**)o)(o)` calls --
// a cdecl call with the receiver PUSHED. A vtable cannot serve both shapes, so
// these thunks take the receiver on the stack, and the one C++ caller
// (dScMgSmartball_c::Render, twelve `->f1()` dispatches) is made to match by
// the __cdecl DispObj declaration in port/unmatched/MgSmartball_HostAbi.h.
// port/CMakeLists.txt already makes the same ruling for func_02021d1c.
//
// ---- TWO SLOTS ARE NOT ORDINARY BODIES -----------------------------------
//
//   dokan RestoreInitial (0x02110bb4) is a 0xc-byte TAIL-CALL VENEER --
//     `ldr ip,[pc]; bx ip; .word 0x02114738` -- so it is the base class's own
//     RestoreInitial reached under a second address. src/func_ov006_02110bb4.c
//     spells both sides `void(void)`, which rides r0 through on ARM and drops
//     the receiver on the host. The thunk dispatches straight to the veneer's
//     TARGET with the receiver, which is exactly what port_scene_fill_rom does
//     for the three Scene veneers it carries.
//
//   object Update (0x02114720) is a genuinely EMPTY body: its single
//     instruction is `bx lr`, and cMgSmartball_object_c.h on main says the
//     same ("empty in this class"). It takes nothing and the thunk passes
//     nothing.
//
// ---- AND TWO OF THE BODIES ARE FILED UNDER WRONG RECOVERED NAMES ----------
//
// Not missing -- misnamed, which reads the same from a symbol search and is
// not the same thing at all. config/arm9/overlays/ov006/delinks.txt in this
// tree files 0x02111144 as src/START_INTRO_MINIMAP_ZOOM.c and 0x02110154 as
// src/_ZN6Player7ST_WAITE.cpp. Both are real, matched ov006 bodies at those
// addresses; the decomp's main has since renamed them
// _ZN24cMgSmartball_propeller_c12SaveSnapshotEv and
// _ZN19cMgSmartball_slot_c14RestoreInitialEv. The rename is a byte-gated-tree
// job and is ROUTED, not taken here: this file calls them by the names this
// tree gives them and says why.
//
// ONE body was genuinely absent: 0x0210f564, cMgSmartball_board_c's
// SaveSnapshot, is a 0x3b0-byte hole between two delink blocks here. It was
// brought across from origin/main by address, re-verified in THIS worktree
// with tools/match.py at mwccarm 2004/b56 against overlay_0006.bin (MATCH),
// given its delink block, and reached through the cdecl forwarder in
// port/unmatched/MgSmartball_Faces.cpp because its recovered form is a
// __thiscall member.

extern "C" {
/* the twelve tables, all already in the ov006 mount */
extern unsigned char data_ov006_0213ec98[];  /* cMgSmartball_ball_c       */
extern unsigned char data_ov006_0213ecac[];  /* cMgSmartball_dokan_c      */
extern unsigned char data_ov006_0213ece8[];  /* cMgSmartball_propeller_c  */
extern unsigned char data_ov006_0213ecfc[];  /* cMgSmartball_spring_c     */
extern unsigned char data_ov006_0213ed10[];  /* cMgSmartball_object_c     */
extern unsigned char data_ov006_0213ed24[];  /* cMgSmartball_wing_c       */
extern unsigned char data_ov006_0213ed38[];  /* cMgSmartball_pushswitch_c */
extern unsigned char data_ov006_0213ed4c[];  /* cMgSmartball_ana_c        */
extern unsigned char data_ov006_0213ed60[];  /* cMgSmartball_board_c      */
extern unsigned char data_ov006_0213ed74[];  /* cMgSmartball_slot_c       */
extern unsigned char data_ov006_0213ed88[];  /* cMgSmartball_kinoko_c     */
extern unsigned char data_ov006_0213ed9c[];  /* cMgSmartball_pakkun_c     */

/* the thirty-six bodies. Every one takes the receiver and nothing else; the
   two exceptions are named in the banner. The parameter types the definitions
   use vary (char *, int *, short *, a per-file struct) and are all one pointer
   wide, so one spelling here is enough and none of these is a C++ name. */
void func_ov006_02113f1c(void *);   void func_ov006_02113e54(void *);
void func_ov006_02114458(void *);
void func_ov006_02110a20(void *);   void func_ov006_02110928(void *);
void START_INTRO_MINIMAP_ZOOM(void *);
void func_ov006_0211102c(void *);   void func_ov006_021111f0(void *);
void func_ov006_0211134c(void *);   void func_ov006_02111268(void *);
void func_ov006_02111560(void *);
void func_ov006_02114724(void *);   void func_ov006_02114720(void);
void func_ov006_02114738(void *);
void func_ov006_0210e014(void *);   void func_ov006_0210ddf0(void *);
void func_ov006_0210e098(void *);
void func_ov006_0210e3e8(void *);   void func_ov006_0210e364(void *);
void func_ov006_0210e460(void *);
void func_ov006_021116f0(void *);   void func_ov006_02111654(void *);
void func_ov006_02111750(void *);
void func_ov006_0210f564(void *);   void func_ov006_0210e4f4(void *);
void func_ov006_0210f914(void *);
void func_ov006_021100a8(void *);   void func_ov006_0210ff1c(void *);
void port_mg_smartball_slot_restore(void *);
void func_ov006_0211192c(void *);   void func_ov006_021117bc(void *);
void func_ov006_02111b20(void *);
void func_ov006_021106b4(void *);   void func_ov006_021104c0(void *);
void func_ov006_02110850(void *);
}  /* extern "C" */

/* One counter for the whole family. Per-class counters would be thirty-six
   numbers nobody reads; what a run needs to know is that the tables are being
   dispatched at all, because a fill that silently never runs and a fill that
   runs look identical from the outside. */
static unsigned g_smb_obj_hits;

#define SMB_OBJ(fn)   { ++g_smb_obj_hits; fn; }

static void smb_ball_s0(void *s)  SMB_OBJ(func_ov006_02113f1c(s))
static void smb_ball_s1(void *s)  SMB_OBJ(func_ov006_02113e54(s))
static void smb_ball_s2(void *s)  SMB_OBJ(func_ov006_02114458(s))
static void smb_dok_s0(void *s)   SMB_OBJ(func_ov006_02110a20(s))
static void smb_dok_s1(void *s)   SMB_OBJ(func_ov006_02110928(s))
/* the veneer: dispatch its target, with the receiver the veneer rides through */
static void smb_dok_s2(void *s)   SMB_OBJ(func_ov006_02114738(s))
static void smb_pro_s0(void *s)   SMB_OBJ(START_INTRO_MINIMAP_ZOOM(s))
static void smb_pro_s1(void *s)   SMB_OBJ(func_ov006_0211102c(s))
static void smb_pro_s2(void *s)   SMB_OBJ(func_ov006_021111f0(s))
static void smb_spr_s0(void *s)   SMB_OBJ(func_ov006_0211134c(s))
static void smb_spr_s1(void *s)   SMB_OBJ(func_ov006_02111268(s))
static void smb_spr_s2(void *s)   SMB_OBJ(func_ov006_02111560(s))
static void smb_obj_s0(void *s)   SMB_OBJ(func_ov006_02114724(s))
/* the empty body: `bx lr`, and it reads nothing */
static void smb_obj_s1(void *)    SMB_OBJ(func_ov006_02114720())
static void smb_obj_s2(void *s)   SMB_OBJ(func_ov006_02114738(s))
static void smb_wng_s0(void *s)   SMB_OBJ(func_ov006_0210e014(s))
static void smb_wng_s1(void *s)   SMB_OBJ(func_ov006_0210ddf0(s))
static void smb_wng_s2(void *s)   SMB_OBJ(func_ov006_0210e098(s))
static void smb_psw_s0(void *s)   SMB_OBJ(func_ov006_0210e3e8(s))
static void smb_psw_s1(void *s)   SMB_OBJ(func_ov006_0210e364(s))
static void smb_psw_s2(void *s)   SMB_OBJ(func_ov006_0210e460(s))
static void smb_ana_s0(void *s)   SMB_OBJ(func_ov006_021116f0(s))
static void smb_ana_s1(void *s)   SMB_OBJ(func_ov006_02111654(s))
static void smb_ana_s2(void *s)   SMB_OBJ(func_ov006_02111750(s))
static void smb_brd_s0(void *s)   SMB_OBJ(func_ov006_0210f564(s))
static void smb_brd_s1(void *s)   SMB_OBJ(func_ov006_0210e4f4(s))
static void smb_brd_s2(void *s)   SMB_OBJ(func_ov006_0210f914(s))
static void smb_slt_s0(void *s)   SMB_OBJ(func_ov006_021100a8(s))
static void smb_slt_s1(void *s)   SMB_OBJ(func_ov006_0210ff1c(s))
static void smb_slt_s2(void *s)   SMB_OBJ(port_mg_smartball_slot_restore(s))
static void smb_kin_s0(void *s)   SMB_OBJ(func_ov006_0211192c(s))
static void smb_kin_s1(void *s)   SMB_OBJ(func_ov006_021117bc(s))
static void smb_kin_s2(void *s)   SMB_OBJ(func_ov006_02111b20(s))
static void smb_pak_s0(void *s)   SMB_OBJ(func_ov006_021106b4(s))
static void smb_pak_s1(void *s)   SMB_OBJ(func_ov006_021104c0(s))
static void smb_pak_s2(void *s)   SMB_OBJ(func_ov006_02110850(s))

#undef SMB_OBJ

/* Address-keyed like every other face array in this file, so the thirty-six
   keys are disjoint from kMgBaseFaces and kSmartballFaces by construction and
   a slot that already holds a host pointer matches nothing. */
static const MgFace kSmartballObjFaces[] = {
    {0x02113f1cu, (void *)smb_ball_s0}, {0x02113e54u, (void *)smb_ball_s1},
    {0x02114458u, (void *)smb_ball_s2},
    {0x02110a20u, (void *)smb_dok_s0},  {0x02110928u, (void *)smb_dok_s1},
    {0x02110bb4u, (void *)smb_dok_s2},
    {0x02111144u, (void *)smb_pro_s0},  {0x0211102cu, (void *)smb_pro_s1},
    {0x021111f0u, (void *)smb_pro_s2},
    {0x0211134cu, (void *)smb_spr_s0},  {0x02111268u, (void *)smb_spr_s1},
    {0x02111560u, (void *)smb_spr_s2},
    {0x02114724u, (void *)smb_obj_s0},  {0x02114720u, (void *)smb_obj_s1},
    {0x02114738u, (void *)smb_obj_s2},
    {0x0210e014u, (void *)smb_wng_s0},  {0x0210ddf0u, (void *)smb_wng_s1},
    {0x0210e098u, (void *)smb_wng_s2},
    {0x0210e3e8u, (void *)smb_psw_s0},  {0x0210e364u, (void *)smb_psw_s1},
    {0x0210e460u, (void *)smb_psw_s2},
    {0x021116f0u, (void *)smb_ana_s0},  {0x02111654u, (void *)smb_ana_s1},
    {0x02111750u, (void *)smb_ana_s2},
    {0x0210f564u, (void *)smb_brd_s0},  {0x0210e4f4u, (void *)smb_brd_s1},
    {0x0210f914u, (void *)smb_brd_s2},
    {0x021100a8u, (void *)smb_slt_s0},  {0x0210ff1cu, (void *)smb_slt_s1},
    {0x02110154u, (void *)smb_slt_s2},
    {0x0211192cu, (void *)smb_kin_s0},  {0x021117bcu, (void *)smb_kin_s1},
    {0x02111b20u, (void *)smb_kin_s2},
    {0x021106b4u, (void *)smb_pak_s0},  {0x021104c0u, (void *)smb_pak_s1},
    {0x02110850u, (void *)smb_pak_s2},
};

static void * const kSmartballObjVts[] = {
    data_ov006_0213ec98, data_ov006_0213ecac, data_ov006_0213ece8,
    data_ov006_0213ecfc, data_ov006_0213ed10, data_ov006_0213ed24,
    data_ov006_0213ed38, data_ov006_0213ed4c, data_ov006_0213ed60,
    data_ov006_0213ed74, data_ov006_0213ed88, data_ov006_0213ed9c,
};

/* THREE per table, never more, and the constant is spelled once so nothing can
   drift it. Returns the number of slots still holding a DS word, so the caller
   can say so out loud rather than a wild call being the first news of it. */
static unsigned port_scene_fill_smartball_objects(void)
{
    unsigned left = 0;
    const unsigned n = sizeof kSmartballObjVts / sizeof kSmartballObjVts[0];
    for (unsigned i = 0; i < n; ++i) {
        void **vt = (void **)kSmartballObjVts[i];
        mg_apply(vt, 3, kSmartballObjFaces,
                 sizeof kSmartballObjFaces / sizeof kSmartballObjFaces[0]);
        left += mg_raw_left(vt, 3);
    }
    return left;
}

extern "C" unsigned port_mg_smartball_obj_hits(void) { return g_smb_obj_hits; }

/* the run report for this class, registered only on a 376 boot */
extern "C" void port_scene_mg_smartball_hits(void);

extern "C" void port_scene_fill_smartball(void)
{
    /* the mounts first, for port_scene_fill_curling's reason */
    port_scene_mg_mounts();

    void **base = (void **)data_ov004_020bc0c0;
    void **vt   = (void **)data_ov006_0213eefc;

    /* The base table again, and it is not redundant work: the fills run in
       port_scene_classes[] order on every boot and each one is idempotent --
       mg_apply keys on the DS word a slot holds, so a slot already carrying a
       host pointer matches nothing and is left alone. */
    port_scene_fill_rom(base, 36);
    mg_apply(base, 36, kMgBaseFaces,
             sizeof kMgBaseFaces / sizeof kMgBaseFaces[0]);

    port_scene_fill_rom(vt, 36);
    mg_apply(vt, 36, kMgBaseFaces,
             sizeof kMgBaseFaces / sizeof kMgBaseFaces[0]);
    mg_apply(vt, 36, kSmartballFaces,
             sizeof kSmartballFaces / sizeof kSmartballFaces[0]);

    {
        const char *s0 = std::getenv("SM64DS_SCENE_SLOT0");
        const char *s9 = std::getenv("SM64DS_SCENE_SLOT9");
        if (s0 && s0[0] == '0') vt[0] = (void *)mg_init_noop;
        if (s9 && s9[0] == '0') vt[9] = (void *)mg_render_noop;
    }

    /* the twelve sub-object tables, three slots each. Run mg5 lane SMBSEAT;
       the derivation of the width and of the identity is above. */
    {
        const unsigned lo = port_scene_fill_smartball_objects();
        if (lo) {
            std::fprintf(stderr, "  [scene] MINIGAME FILL INCOMPLETE: "
                         "the cMgSmartball_object_c family leaves %u of 36 raw "
                         "DS words across its twelve three-slot tables.\n", lo);
            std::fflush(stderr);
        }
    }

    {
        const unsigned lv = mg_raw_left(vt, 36);
        if (lv) {
            std::fprintf(stderr, "  [scene] MINIGAME FILL INCOMPLETE: "
                         "dScMgSmartball_c leaves %u of 36 raw DS words. A "
                         "dispatch of any of them jumps to a DS address as a "
                         "host one.\n", lv);
            std::fflush(stderr);
        }
    }

    port_scene_mg_prepare(port_scene_env_want());

    /* ---- THE PRE-FLIGHT THIS SEAT OWNS ---------------------------------
       Not a blocker in the fader/archive sense -- nothing here is a seat that
       came apart -- but the same kind of statement, made before the spawn
       rather than after a fault: this class's InitResources has no decompiled
       body, so the boot cannot get past its own first Behavior frame, and the
       reason is a decomp hole rather than anything about the port. Printed
       only on a 376 boot so curling's run is unaffected. */
    if (port_scene_env_want() == 376) {
        std::printf("[scene] dScMgSmartball_c SEATED: vtable 0x0213eefc, 36 "
                    "slots, 9 overrides, and ALL NINE now reach a body. Run mg5 "
                    "lane INTEG seated slot 0 InitResources (func_ov006_02118b70 "
                    "aliased onto the plain-C "
                    "_ZN16dScMgSmartball_c13InitResourcesEv, NONMATCHING) and the "
                    "aux ball-table seeder func_ov006_02114800. Run mg5 lane "
                    "SMBSEAT closed the last two floors: slot 9 Render is "
                    "src/_ZN16dScMgSmartball_c6RenderEv.cpp, main's matched "
                    "__thiscall member, reached through a cdecl forwarder in "
                    "port/unmatched/MgSmartball_Faces.cpp because an alias cannot "
                    "cross the calling convention; and the sub-object family is "
                    "relocated. The line this replaces said 'a sub-object whose "
                    "ov006 vtable at 0x0213eca0 holds raw DS addresses' -- "
                    "0x0213eca0 is the WORD holding 0x02114458 and is slot 2 of "
                    "cMgSmartball_ball_c's table, whose vptr is 0x0213ec98. There "
                    "are TWELVE such tables, one per cMgSmartball_ class, THREE "
                    "slots each (not the five the config symbol span reads), and "
                    "all twelve are filled from kSmartballObjFaces above.\n");
        std::fflush(stdout);
        static int armed;
        if (!armed) {
            armed = 1;
            std::atexit(port_scene_mg_smartball_hits);
        }
    }
}

/* The registry's factory column is void *(*)(void) and the matched factory
   returns int *. One typed forwarder, curling's shape.
   NO DISPLACEMENT RULING IS NEEDED FOR THIS FACTORY, and the reason is in the
   SOURCE rather than in the instructions. Section 12's case is a factory whose
   src calls the base constructor with NO argument while the ROM rides r0
   through. At the instruction level 0x02119824 is the same shape -- `movs
   r4,r0` after the allocator returns, and r0 is never rewritten before
   `bl 0x20b2adc` -- so the object rides through here too. What differs is the
   only thing the host sees: src/MgBingoBallSlotsShot_Spawn.cpp spells the call
   `func_ov004_020b2adc(o)` and 0x169's src/func_ov006_020e0574.cpp spells it
   `func_ov004_020b2adc()`. port/tools/aritycheck.py finds the same split
   independently: it lists func_ov004_020b2adc as DROPS from
   src/func_ov006_020e0574.cpp and src/MgMushroomRoulette_Spawn.cpp, and not
   from this one. */
extern "C" void *port_mg_smartball_spawn(void)
{
    return (void *)MgBingoBallSlotsShot_Spawn();
}

extern "C" void port_scene_mg_smartball_hits(void)
{
    /* ONLY BIT 2 CAN FIRE TODAY, and the other two labels are kept rather
       than deleted. Bits 0 and 1 were slot 0 InitResources and slot 9 Render;
       both bodies are seated now (runs INTEG and SMBSEAT) and their traps are
       gone from port/unmatched/MgSmartball_Traps.cpp, so those two bits are
       structurally unreachable and a reader who sees either label print is
       looking at a re-armed trap, which is exactly what the labels are for. */
    const unsigned m = port_mg_smartball_trap_mask();
    std::printf("[scene] dScMgSmartball_c traps entered: %u total%s%s%s\n",
                port_mg_smartball_trap_hits(),
                (m & 1) ? "  [slot 0 InitResources RE-ARMED]" : "",
                (m & 2) ? "  [slot 9 Render RE-ARMED]" : "",
                (m & 4) ? "  [func_ov006_02115248]" : "");
    /* The sub-object witness. Zero here with a clean run is NOT a pass: it
       means the twelve tables were relocated and then never dispatched, which
       is the one failure a fill cannot tell from a success on its own. */
    std::printf("[scene] cMgSmartball_object_c family dispatches: %u through "
                "the twelve relocated three-slot tables\n",
                port_mg_smartball_obj_hits());
    std::fflush(stdout);
}

// ============================================================================
// RUN mg5, LANE CCN: dScMgCoin_c, THE COINCENTRATION MINIGAME (id 0x17a = 378)
// ============================================================================
//
// The second minigame class seated, and it is the fan-out's cheap shape:
// SIX override slots, five of them marker-carrying and all five ruled
// REAL_DECOMP against the ROM before anything here was written (see
// port/tools/inferred_stub_adjudicated.txt). Everything above this line --
// port_scene_fill_rom, kMgBaseFaces, mg_apply, mg_raw_left, the mounts and the
// thirty-five overlay constructors -- is reused unchanged, which is the split
// port/mg_fanout_costs.txt section 2 calls the whole cost model.
//
// THE HIERARCHY IS TWO DEEP AND THAT IS MEASURED, not assumed from curling.
// Slot 16 (D2, func_ov006_020dbe40) and slot 17 (D0, func_ov006_020dbe64)
// BOTH store data_ov006_0213bf50 into [this] and then call
// func_ov004_020b29c0, dScMgBase_c's teardown, with no table in between. So
// there is no intermediate base of the dScMgSingle3DBase_c kind 0x169 needed,
// and this seat fills ONE derived table plus the shared base one.
//
// THE WIDTH IS 36, CHECKED THREE WAYS. Span to the next config symbol
// (data_ov006_0213bfe0) is exactly 0x90 = 36 words; slot 35 holds 0x020ad660,
// the terminal word every dScMgBase_c-derived table holds; and the word at
// index 36 is 0x01fc00fc, an address in no module. port/slice_ccn.txt states
// all three. A 37-slot fill here would have written a host thunk over the
// first word of data_ov006_0213bfe0, which is the failure
// port/mg_fanout_costs.txt section 11 exists to prevent.

extern "C" {
/* the class's own vtable, in the ov006 mount. 36 slots, span-checked. */
extern unsigned char data_ov006_0213bf50[];   /* dScMgCoin_c,    36 slots */

/* the class's own six vtable bodies. func_ov006_020de69c is the HOST COPY in
   unmatched/MgCoin_StateDispatch.cpp, not the src TU: it is the pointer-to-
   member dispatcher and the port cannot compile the src. */
int   func_ov006_020de704(void *self);          /* slot 0  InitResources */
int   func_ov006_020de69c(void *self);          /* slot 6  Behavior, host copy */
int   func_ov006_020de63c(void *self);          /* slot 9  Render */
int   func_ov006_020dbe40(int *self);           /* slot 16 D2 */
int  *func_ov006_020dbe64(int *self);           /* slot 17 D0 */
void  func_ov006_020de5b0(char *self);          /* slot 18 state reset */

void *MgCoincentration_Spawn(void);

/* the state machine's witnesses, from unmatched/MgCoin_StateDispatch.cpp */
unsigned port_mg_coin_state_hits(void);
unsigned port_mg_coin_floor_hits(void);
unsigned port_mg_coin_touch_calls(void);
/* the render-path floor's witness, from unmatched/MgCoin_Faces.cpp */
unsigned port_mg_coin_trap_hits(void);
}

static int  __fastcall mc_init(void *s, void *)
{ MG_SLOT(0);  const int r = func_ov006_020de704(s);
  hal_gapless_minigames_latch(); return r; }
/* SM64DS_CCN_TRACE=1: dScMgCoin_c's 40-slot object array, once at beh 200. */
static void mc_slots_dump(void *s, unsigned beh)
{
    static int on = -1;
    if (on < 0) {
        const char *e = std::getenv("SM64DS_CCN_TRACE");
        on = (e && e[0] && e[0] != '0') ? 1 : 0;
    }
    if (!on || beh != 200) return;
    unsigned char *c = (unsigned char *)s;
    for (int i = 0; i < 0x28; i++) {
        unsigned char *p = c + i * 0x1c;
        if (!*(p + 0x4676)) continue;
        std::fprintf(stderr, "[ccn] slot%02d kind=%u x=%d y=%d st=%u f15=%u "
                     "vx=%d vy=%d\n", i, (unsigned)*(p + 0x4678),
                     *(int *)(p + 0x4660) >> 12, *(int *)(p + 0x4664) >> 12,
                     (unsigned)*(p + 0x4675), (unsigned)*(p + 0x4677),
                     *(int *)(p + 0x4668) >> 12, *(int *)(p + 0x466c) >> 12);
    }
}
static int  __fastcall mc_beh(void *s, void *)
{ MG_SLOT(6);  mc_slots_dump(s, g_mg_hits[6]); const int r = func_ov006_020de69c(s); hal_gapless_splice(); return r; }
static int  __fastcall mc_render(void *s, void *)
{ MG_SLOT(9);  return func_ov006_020de63c(s); }
static void *__fastcall mc_d2(void *s, void *)
{ MG_SLOT(16); return (void *)(size_t)func_ov006_020dbe40((int *)s); }
static void *__fastcall mc_d0(void *s, void *)
{ MG_SLOT(17); return (void *)func_ov006_020dbe64((int *)s); }
static int  __fastcall mc_reset(void *s, void *, int /*ridethrough*/)
{ MG_SLOT(18); func_ov006_020de5b0((char *)s); return 1; }

/* dScMgCoin_c's own six, the per-class half. Keyed on the ROM WORD each slot
   holds, exactly like kCurlingFaces, so the array is order-independent and
   cannot land on a slot the ROM did not park that body in. */
static const MgFace kCoinFaces[] = {
    {0x020de704u, (void *)mc_init},   {0x020de69cu, (void *)mc_beh},
    {0x020de63cu, (void *)mc_render}, {0x020dbe40u, (void *)mc_d2},
    {0x020dbe64u, (void *)mc_d0},     {0x020de5b0u, (void *)mc_reset},
};

/* The scene object, for the run report below. Nothing else reads it. */
static char *g_mg_coin_self;

extern "C" void *port_mg_coin_spawn(void)
{
    void *p = MgCoincentration_Spawn();
    g_mg_coin_self = (char *)p;
    return p;
}

/* THE FACTORY NEEDS NO DISPLACEMENT RULING, and that is worth recording
   because 0x169's did. src/MgCoincentration_Spawn.cpp calls
   func_ov004_020b2adc(o) WITH its argument, where src/func_ov006_020e0574.cpp
   (0x169's factory) calls the same base constructor with none and rides r0
   through. That callee dereferences on its first statement and then writes
   three vtable words through the pointer, so the difference is a wild write
   versus a correct one. This class's factory is on the correct side of it and
   is linked from the slice rather than host-copied. Verified against the ROM
   at 0x020de940 (0x48 bytes) during this lane's adjudication pass. */

extern "C" void port_scene_mg_coin_hits(void);

extern "C" void port_scene_fill_coin(void)
{
    /* mounts before the fill, for port_scene_fill_curling's reason; both are
       idempotent behind their own static guards. */
    port_scene_mg_mounts();

    void **base = (void **)data_ov004_020bc0c0;
    void **vt   = (void **)data_ov006_0213bf50;

    /* The base table again. port_scene_fill_curling already filled it on this
       boot -- every row's fill runs on every boot -- and doing it here as well
       is idempotent, because the fill keys on the ROM word a slot holds and a
       slot already holding a host thunk matches nothing. It is repeated so this
       fill is correct read on its own, the way port_scene_mg_overlay_load
       re-calls the mounts for the same reason. */
    port_scene_fill_rom(base, 36);
    mg_apply(base, 36, kMgBaseFaces,
             sizeof kMgBaseFaces / sizeof kMgBaseFaces[0]);

    /* the derived table: shared arm9 words, then the framework's twenty-eight,
       then this class's own six. The three key sets are disjoint by
       construction, since a word is one address. */
    port_scene_fill_rom(vt, 36);
    mg_apply(vt, 36, kMgBaseFaces,
             sizeof kMgBaseFaces / sizeof kMgBaseFaces[0]);
    mg_apply(vt, 36, kCoinFaces,
             sizeof kCoinFaces / sizeof kCoinFaces[0]);

    {
        const char *s0 = std::getenv("SM64DS_SCENE_SLOT0");
        const char *s9 = std::getenv("SM64DS_SCENE_SLOT9");
        if (s0 && s0[0] == '0') vt[0] = (void *)mg_init_noop;
        if (s9 && s9[0] == '0') vt[9] = (void *)mg_render_noop;
    }

    {
        const unsigned lv = mg_raw_left(vt, 36);
        if (lv) {
            std::fprintf(stderr, "  [scene] MINIGAME FILL INCOMPLETE: "
                         "dScMgCoin_c leaves %u of 36 raw DS words. A dispatch "
                         "of any of them jumps to a DS address as a host "
                         "one.\n", lv);
            std::fflush(stderr);
        }
    }

    port_scene_mg_prepare(port_scene_env_want());

    if (port_scene_env_want() == 378) {
        static int armed;
        if (!armed) {
            armed = 1;
            std::atexit(port_scene_mg_coin_hits);
        }
    }
}

/* This class's own witness. hal/scene_boot.cpp's end-of-run block still has no
   third branch and is still not this lane's region to widen, so the same
   std::atexit shape port_scene_fill_curling uses is used here. Registered only
   when scene 378 is the requested one, so a curling or level run prints
   nothing extra. */
extern "C" void port_scene_mg_coin_hits(void)
{
    std::printf("[scene] dScMgCoin_c slot hits: init %u, behavior %u, "
                "render %u, D0 %u, state-reset %u\n",
                g_mg_hits[0], g_mg_hits[6], g_mg_hits[9], g_mg_hits[17],
                g_mg_hits[18]);
    {
        unsigned calls = 0, unknown = 0;
        port_mg_dispatch_counts(&calls, &unknown);
        std::printf("[scene] dScMgCoin_c state dispatch: %u routed to a "
                    "dScMgCoin_c state, of which %u ran the touch state "
                    "0x020dd0e0, %u reached the class's ONE bodiless "
                    "state (there is none left, 0x020dd0e0 has a body now), "
                    "%u framework call(s), %u UNHANDLED "
                    "address(es)\n", port_mg_coin_state_hits(),
                    port_mg_coin_touch_calls(),
                    port_mg_coin_floor_hits(), calls, unknown);
    }
    /* THE FLOORS, REPORTED WHETHER OR NOT THEY FIRED. The render floor is the
       only one left and it is a decomp gap; it is silent unless rendering is
       on, so a zero here has to say WHICH zero it is. The state floor is
       CLOSED -- src/func_ov006_020dd0e0.c is the matched body and lane WIRE
       wired the slot to it -- and the count that used to say how often the
       class wanted a state it could not reach now says how often it RAN it. */
    std::printf("[scene] dScMgCoin_c floors: render callee 0x020dbe9c entered "
                "%u time(s); state floor 0x020dd0e0 CLOSED by "
                "src/func_ov006_020dd0e0.c, which ran %u time(s) (bodiless "
                "wants remaining: %u)\n",
                port_mg_coin_trap_hits(), port_mg_coin_touch_calls(),
                port_mg_coin_floor_hits());
    if (g_mg_coin_self)
        std::printf("[scene] dScMgCoin_c object at %p, state index %d\n",
                    (void *)g_mg_coin_self,
                    *(int *)(g_mg_coin_self + 0x51c8));
    std::fflush(stdout);
}

// ============================================================================
// RUN mg6, LANE LKT: dScMgPachinko2_c, THE LAKITU LAUNCH MINIGAME (0x171 = 369)
// ============================================================================
//
// The cheapest whole class on port/mg_fanout_costs.txt section 3's board --
// six override slots, four marker-carrying, no vtable body without a source --
// and it is deliberately additive: nothing above this line changed.
// Everything from the mounts to the thirty-five overlay constructors to
// kMgBaseFaces to port_scene_fill_rom is the family's and was paid once by
// lane MG2; this section is one class's own six overrides, one face array, one
// fill and one factory forwarder.
//
// IDENTITY, RE-DERIVED FROM THE ROM BY THIS LANE rather than inherited:
//   RTTI       0x0213db58  "16dScMgPachinko2_c", 0x64 bytes before the table,
//              which is the same RTTI-then-vtable adjacency the sibling has at
//              0x0213d960 / 0x0213d9cc
//   class      dScMgPachinko2_c, and src/MgLakituLaunch_Spawn.c's own
//              `p[0] = (int)_ZTV16dScMgPachinko2_c;` agrees with the ROM's
//              string. That name is bound to the address by the one
//              /alternatename row in unmatched/MgPachinko2_Faces.cpp
//   SpawnInfo  0x0213da64  MgLakituLaunch_SpawnInfo, doubled id 0x01710171 at
//              +4, and that word occurs exactly ONCE in the whole overlay
//   factory    0x02104258  MgLakituLaunch_Spawn
//   vtable     0x0213dbbc  data_ov006_0213dbbc, and the factory's only ov006
//              literal-pool load is that word (relocs.txt from:0x02104288),
//              which is how the two are tied together rather than by name
//   ctor       __sinit_ov006_02131cd0, attributed by address containment: its
//              copied pairs run 0x0213da5c..0x0213db4c and bracket the
//              SpawnInfo
//
// ---- THE ADJACENCY THIS CLASS SITS IN, IN BOTH DIRECTIONS -----------------
//
// SECTION 8 ABOVE ALREADY NAMED HALF OF IT. dScMgPachinko_c's vtable ends at
// 0x0213da5c, and that word is the pair __sinit_ov006_02131cd0 line 89 copies
// into data_ov006_02142734.p4 -- THIS class's state 4, 0x021019e0. Section 8
// wrote that the corruption "would have been latent until [0x171] was seated".
// This is that commit. From here a 37-slot fill of 0x170 would take out state
// 4 of 0x171 on the same boot rather than quietly; the sibling's fill is
// called with 36 and its mg_raw_left checks 36, so it does not.
//
// AND THE REVERSE IS THIS LANE'S TO PROVE, WITH A DIFFERENT VICTIM. All four
// width checks put this table at 36 slots:
//
//   1. SPAN. config/arm9/overlays/ov006/symbols.txt has data_ov006_0213dbbc
//      followed by data_ov006_0213dc4c -- 0x90 bytes, exactly 36 words. Index
//      36 IS the next symbol.
//   2. TERMINAL SLOT. Slot 35 holds 0x020ad660, the family terminator.
//   3. WHAT THE WORD IS. Index 36 is 0x0210713c and index 37 is 0x00000000,
//      which is an mwcc {code, adjustment} pair and not a slot.
//   4. THE RELOCATION COUNT, free from the ROM: relocs.txt carries exactly 36
//      load relocations inside 0x0213dbbc..0x0213dc4c, one per slot, and the
//      next one is at 0x0213dc4c itself, pointing at 0x0210713c -- the pair's
//      own code word.
//
// THE VICTIM OF A 37th SLOT HERE IS A THIRD CLASS. src/__sinit_ov006_02131fa4.c
// line 64 reads `data_ov006_02142888.p1 = data_ov006_0213dc4c;`, and that
// constructor's copied range brackets MgPuzzlePanelPuzzlePanic_SpawnInfo at
// 0x0213dc64 -- id 0x17c, which nobody has seated. So the write would be a
// wild one into a class with no seat to notice it. port_scene_fill_rom is
// called with 36 below and mg_raw_left checks 36.
//
// ---- THE SIX OVERRIDES ----------------------------------------------------
//
//   slot  0  func_ov006_02103ed0  sliced, MARKER, ruled REAL_DECOMP
//   slot  6  func_ov006_02103d78  sliced. A PLAIN SWITCH on the state index at
//            +0x5660, not a pointer-to-member dispatch -- this class keeps its
//            member-pointer machinery one level down, in the three tables the
//            cases call.
//   slot  9  func_ov006_02103d28  sliced, MARKER, ruled REAL_DECOMP
//   slot 16  func_ov006_020ff420  sliced. Spells the vptr store by the real
//            config symbol data_ov006_0213dbbc, which is why it needs no host
//            copy and its D0 sibling does.
//   slot 17  func_ov006_020ff444  MARKER, ruled REAL_DECOMP, and EXCLUDED from
//            the slice: it spells decl_common.h's shared VT/HEAP placeholders
//            and bare VT is bound to the ov002 Enemy base table. Hosted as
//            port_mg_pachinko2_d0 in port/unmatched/MgPachinko2_Dtor.cpp.
//   slot 18  func_ov006_02103cbc  sliced, MARKER, ruled REAL_DECOMP
//
// The four markers were disassembled out of the shipped overlay image and
// compared instruction for instruction with src before being seated; the
// rulings and their evidence are in port/tools/inferred_stub_adjudicated.txt.
// ZERO of the six is without a source, which is what makes this class the
// cheapest on the board -- and it is a statement about the VTABLE axis only.
// Five ov006 addresses in the state and dispatcher layer have no body at all;
// port/slice_lkt.txt names them and the switch and the trap report them.
//
// ---- THE STATE MACHINE IS THREE TABLES AND IT FIRES ON THE FIRST TICK -----
//
// Slot 0 sets *(int *)(self + 0x5660) = 1, and slot 6's case 1 calls all three
// dispatchers. So this class does not need touch input or a played round to
// reach its own state machine, which is the thing
// port/mg_fanout_costs.txt section 10 records the pathfinder as never having
// proved. The three tables and their twenty-eight states are in
// port/unmatched/MgPachinko2_StateDispatch.cpp.

extern "C" {

/* the mount storage this fill writes into */
extern unsigned char data_ov006_0213dbbc[];   /* dScMgPachinko2_c, 36 slots */
extern unsigned char MgLakituLaunch_SpawnInfo[];

/* the class's own six overrides, in slot order. Five are sliced; slot 17 is
   the host copy, not the src TU. */
int   func_ov006_02103ed0(void *self);        /* slot  0 InitResources */
int   func_ov006_02103d78(void *self);        /* slot  6 Behavior      */
int   func_ov006_02103d28(void *self);        /* slot  9 Render        */
int   func_ov006_020ff420(void *self);        /* slot 16 D2            */
void *port_mg_pachinko2_d0(void *self);       /* slot 17 D0, hosted    */
void  func_ov006_02103cbc(void *self, int n); /* slot 18 state reset   */

/* the factory. IT NEEDS NO DISPLACEMENT RULING, and that was checked rather
   than assumed: port/mg_fanout_costs.txt section 12 found 0x169's factory
   calling the base constructor with NO argument where ARM rides r0 through,
   and granted a host copy for it. src/MgLakituLaunch_Spawn.c does NOT have
   that defect -- it reads `func_ov004_020b2adc(p);`, with the argument -- so
   the matched TU is sliced and called directly. Confirmed against the ROM at
   0x02104258 (0x34 bytes): arm_call to 0x020b2adc at 0x0210426c with r0
   already holding the allocation. */
int  *MgLakituLaunch_Spawn(void);

/* the class's state machine, port/unmatched/MgPachinko2_StateDispatch.cpp */
void port_mg_pachinko2_state_counts(unsigned *hits, unsigned *missing);
/* its per-slot breakdown, same file: a total says the machine dispatched,
   only the breakdown says it MOVED. Run mg7 lane L369. */
void port_mg_pachinko2_state_census(void);

}  /* extern "C" */

/* This class's own tick witness, kept separate from g_mg_hits for the reason
   section 8 keeps g_pch_hits separate: the report below returns early on an
   all-zero array, so a run prints exactly the class it booted and no other
   seat's counters can make this one look busy. */
static unsigned g_lkt_hits[36];
#define LKT_SLOT(n) (++g_lkt_hits[(n)])

static int  __fastcall lkt_init(void *s, void *)
{ LKT_SLOT(0);  const int r = func_ov006_02103ed0(s);
  hal_gapless_minigames_latch(); return r; }
static int  __fastcall lkt_beh(void *s, void *)
{ LKT_SLOT(6);  const int r = func_ov006_02103d78(s); hal_gapless_splice();
  return r; }
static int  __fastcall lkt_render(void *s, void *)
{ LKT_SLOT(9);  return func_ov006_02103d28(s); }
static void *__fastcall lkt_d2(void *s, void *)
{ LKT_SLOT(16); return (void *)(size_t)func_ov006_020ff420(s); }
static void *__fastcall lkt_d0(void *s, void *)
{ LKT_SLOT(17); return port_mg_pachinko2_d0(s); }
/* slot 18 takes a SECOND argument in this class, exactly as the sibling's
   does: src/func_ov006_02103cbc.c is (char *c, int n) and the ROM reads r1
   (cmp r1,#0x10 at 0x02103cd0). The __fastcall face lands `this` in ecx and
   the ROM's r1 in the first stack slot, which is where the third parameter of
   this thunk sits. Declaring it without the parameter is the four-byte stack
   leak run mg5 lane BASESET measured. */
static int  __fastcall lkt_reset(void *s, void *, int n)
{ LKT_SLOT(18); func_ov006_02103cbc(s, n); return 1; }

/* dScMgPachinko2_c's own six, the per-class half. Keyed on the ROM WORD each
   slot holds, exactly like kCurlingFaces and kPachinkoFaces, so the array is
   order-independent and cannot land on a slot the ROM did not park that body
   in. The six keys are disjoint from kMgBaseFaces by construction -- a word is
   one address, and none of these six appears in the base table. */
static const MgFace kPachinko2Faces[] = {
    {0x02103ed0u, (void *)lkt_init},   {0x02103d78u, (void *)lkt_beh},
    {0x02103d28u, (void *)lkt_render}, {0x020ff420u, (void *)lkt_d2},
    {0x020ff444u, (void *)lkt_d0},     {0x02103cbcu, (void *)lkt_reset},
};

/* The scene object, for the run report below. Nothing else reads it and
   nothing may: this is an observation hook, not a back door into the class. */
static char *g_mg_pachinko2_self;

extern "C" void *port_mg_pachinko2_spawn(void)
{
    void *p = (void *)MgLakituLaunch_Spawn();
    g_mg_pachinko2_self = (char *)p;
    return p;
}

extern "C" void port_scene_mg_pachinko2_hits(void);

extern "C" void port_scene_fill_pachinko2(void)
{
    /* the same order and the same reasons as port_scene_fill_curling; see its
       header for why the mounts come before the fill and why the BASE table is
       filled too rather than only the derived one. Both are idempotent behind
       their own static guards. */
    port_scene_mg_mounts();

    void **base = (void **)data_ov004_020bc0c0;
    void **vt   = (void **)data_ov006_0213dbbc;

    port_scene_fill_rom(base, 36);
    mg_apply(base, 36, kMgBaseFaces,
             sizeof kMgBaseFaces / sizeof kMgBaseFaces[0]);

    /* THE DERIVED TABLE, AND THE 36 IS THE WHOLE OF SECTION 11's LESSON. Three
       key sets, disjoint by construction: the shared arm9 words, the
       framework's twenty-eight, then this class's own six. */
    port_scene_fill_rom(vt, 36);
    mg_apply(vt, 36, kMgBaseFaces,
             sizeof kMgBaseFaces / sizeof kMgBaseFaces[0]);
    mg_apply(vt, 36, kPachinko2Faces,
             sizeof kPachinko2Faces / sizeof kPachinko2Faces[0]);

    {
        const char *s0 = std::getenv("SM64DS_SCENE_SLOT0");
        const char *s9 = std::getenv("SM64DS_SCENE_SLOT9");
        if (s0 && s0[0] == '0') vt[0] = (void *)mg_init_noop;
        if (s9 && s9[0] == '0') vt[9] = (void *)mg_render_noop;
    }

    {
        const unsigned lb = mg_raw_left(base, 36);
        const unsigned lv = mg_raw_left(vt, 36);
        if (lb || lv) {
            std::fprintf(stderr, "  [scene] MINIGAME FILL INCOMPLETE: "
                         "dScMgBase_c leaves %u of 36 raw DS words, "
                         "dScMgPachinko2_c leaves %u. A dispatch of any of "
                         "them jumps to a DS address as a host one.\n",
                         lb, lv);
            std::fflush(stderr);
        }
    }

    port_scene_mg_prepare(port_scene_env_want());

    if (port_scene_env_want() == 369) {
        static int armed;
        if (!armed) {
            armed = 1;
            std::atexit(port_scene_mg_pachinko2_hits);
        }
    }
}

/* This class's own witness. hal/scene_boot.cpp's end-of-run block still has no
   branch for a fifth minigame and is still not this lane's region to widen, so
   the same std::atexit shape port_scene_fill_coin uses is used here.
   Registered only when scene 369 is the requested one, so no other scene's run
   prints anything extra.

   THE DISPATCH LINE IS THE ONE THAT MATTERS. A minigame that spawns, ticks and
   renders looks identical from outside to one whose state machine never fires,
   and port/mg_fanout_costs.txt section 10 records the pathfinder shipping in
   exactly that condition -- "the dispatch fix is proven to COMPILE and to LINK
   and is NOT yet proven to route a single state". A zero on the routed count
   below means this seat is in that condition and the run says so. */
extern "C" void port_scene_mg_pachinko2_hits(void)
{
    unsigned total = 0;
    for (int i = 0; i < 36; ++i) total += g_lkt_hits[i];
    if (!total) return;               /* not this run's class */

    std::printf("[scene] dScMgPachinko2_c slot hits: init %u, behavior %u, "
                "render %u, D2 %u, D0 %u, state-reset %u\n",
                g_lkt_hits[0], g_lkt_hits[6], g_lkt_hits[9],
                g_lkt_hits[16], g_lkt_hits[17], g_lkt_hits[18]);
    {
        unsigned hits = 0, missing = 0, calls = 0, unknown = 0;
        port_mg_pachinko2_state_counts(&hits, &missing);
        port_mg_dispatch_counts(&calls, &unknown);
        /* Run mg7 lane L369 closed all four of the addresses that used to
           land in the reporting case, so `missing` is write-never now and
           printing it is proof rather than a warning. The per-entry follow-up
           line goes with the trap that produced it: func_ov006_0210076c is a
           real src TU on slice_lkt.txt and has nothing of its own to say. */
        std::printf("[scene] dScMgPachinko2_c state dispatch: %u routed to one "
                    "of this class's 28 states, %u of those reached an address "
                    "with no decompiled body (all 28 have one since run mg7), "
                    "%u framework call(s), %u UNHANDLED address(es)\n",
                    hits, missing, calls, unknown);
        port_mg_pachinko2_state_census();
    }
    if (g_mg_pachinko2_self)
        std::printf("[scene] dScMgPachinko2_c object at %p, state index %d\n",
                    (void *)g_mg_pachinko2_self,
                    *(int *)(g_mg_pachinko2_self + 0x5660));
    std::fflush(stdout);
}

// RUN mg6, LANE PPP: dScMgPanel_c, actor id 0x17c = scene 380
// ============================================================================
//
// APPENDED AT THE END OF THE FILE, the shape every mg5 lane used and for the
// reason they gave: several lanes append one of these blocks at once and
// appending is what concatenates. Everything above is reused unchanged --
// kMgBaseFaces, mg_apply, mg_raw_left, port_scene_mg_mounts,
// port_scene_mg_prepare, port_scene_fill_rom and the g_mg_hits counters.
//
// ---- THE CLASS, RE-DERIVED FROM THE ROM AND NOT FROM THE COST FILE --------
//
// THE RTTI CHAIN IS COMPLETE AND IT IS BETTER EVIDENCE THAN THE SRC SPELLING.
// The word immediately BEFORE the vtable is the type_info pointer:
//
//     config/arm9/overlays/ov006/relocs.txt
//       from:0x0213dd44 kind:load to:0x0213dd84 module:overlay(6)
//       from:0x0213e248 kind:load to:0x0213dd40 module:overlay(6)
//
// so data_ov006_0213e24c[-1] = 0x0213dd40, whose second word points at
// 0x0213dd84, which reads "12dScMgPanel_c" in the shipped image. The class is
// dScMgPanel_c and the two player-facing titles the spawn symbol carries are
// localised names, exactly the MgShuffleShell / dScMgCurling_c and MgWanted /
// dScMgLuigi_c shape. src/MgPuzzlePanelPuzzlePanic_Spawn.c spelling
// _ZTV12dScMgPanel_c agrees, and is the weaker of the two witnesses.
//
// ONE ACTOR ID, TWO PLAYER TITLES, AND THE SELECTOR IS INSIDE THE CLASS. A
// scan of extracted/overlays/overlay_0006.bin for every doubled-id word
// 0xNNNN NNNN in 0x169..0x186 finds THIRTY records and exactly one for 0x17c;
// the arm9 spawn table at 0x02090e54 has one row for it, in id order between
// 0x17b's and 0x17d's. So "Puzzle Panel" and "Puzzle Panic" are not two ids.
// What the ROM does have is a two-way branch inside the class:
// func_ov006_02106168 deals each panel's pair of faces from ONE OF TWO
// face-set tables, data_ov006_0213ded0 when the gate answers nonzero and
// data_ov006_0213e070 when it answers zero, and func_ov006_021057f0 skips its
// whole state dispatch when vtable SLOT 35 answers nonzero. Slot 35 is
// dScMgBase_c's func_ov004_020ad660, `return (this[2] & 0xff) != 0` -- a mode
// byte the framework puts in the object's third word. WHICH VALUE IS WHICH
// TITLE IS NOT DERIVED HERE and this block does not guess it; what is derived
// is that the split is a mode flag on one class and not two classes.
//
// AN EARLIER VERSION OF THIS PARAGRAPH NAMED THE WRONG PAIR OF TABLES. It said
// the gate chose between data_ov006_0213dd4c and data_ov006_0213dd58. Run mg7
// lane L380 read the body: those two are loaded UNCONDITIONALLY, both indexed
// by [this+0x4cbc]-4, and they hold the panel x and y positions as u16 that
// the body shifts left twelve into +0x4cc4+i*4 and +0x4d54+i*4. They have
// nothing to do with the mode.
//
//   SpawnInfo      0x0213dc64  MgPuzzlePanelPuzzlePanic_SpawnInfo. The doubled
//                  id word 0x017c017c sits at 0x0213dc68 and the word before
//                  it is 0x02107858, which is the factory -- the mechanical
//                  derivation port/mg_fanout_costs.txt section 3 prescribes.
//   factory        0x02107858  MgPuzzlePanelPuzzlePanic_Spawn, 0x34 bytes
//   vtable         0x0213e24c  named by the factory's OWN load relocation,
//                  relocs.txt from:0x02107888 kind:load to:0x0213e24c
//   width          36          all three of section 11's checks; see
//                              port/slice_ppp.txt section 1
//   overrides      6           slots 0, 6, 9, 16, 17, 18
//   markers        5           all five ruled REAL_DECOMP before seating
//   nosrc          0           among the override bodies
//
// TWO TABLES, NOT THREE. Slot 16 (func_ov006_0210428c) and slot 17
// (func_ov006_021042b0) BOTH store data_ov006_0213e24c into [this] and then
// call func_ov004_020b29c0 with nothing in between, so the chain is
// Scene -> dScMgBase_c -> dScMgPanel_c and this fill does the base's table and
// this one. dScMgCup_c needed a third because its destructors write an
// intermediate base's.
//
// ALL SIX OVERRIDE SLOTS. "ruled" means the body was disassembled out of
// extracted/overlays/overlay_0006.bin at base 0x020bfec0 with
// port/tools/w13_dump.py and compared instruction for instruction with src/
// BEFORE it was seated; the per-body evidence is in
// port/tools/inferred_stub_adjudicated.txt.
//
//   slot  ROM word    module  body
//    0   021073b0    ov006   InitResources            ruled REAL_DECOMP
//    6   02107358    ov006   Behavior, HOST COPY      ruled REAL_DECOMP
//    9   0210730c    ov006   Render                   ruled REAL_DECOMP
//   16   0210428c    ov006   D2                       (no marker, checked)
//   17   021042b0    ov006   D0                       ruled REAL_DECOMP
//   18   021071fc    ov006   state reset              ruled REAL_DECOMP
//   --   the other thirty are dScMgBase_c's or arm9's and are already keyed by
//        address in kMgBaseFaces and port_scene_fill_rom
//   --   word 36 reads ffffffff, which is its own config symbol
//        data_ov006_0213e2dc and not a code address
//
// SLOT 18 TAKES AN ARGUMENT AND THE THUNK CLEANS IT, the repair lane BASESET
// audited twenty-two slot-18 sites for. src/func_ov006_021071fc.c declares
// `(char *self, int flag)` and the ROM branches on r1 at 0x02107208, so the
// argument is real here rather than a ride-through, and mp_reset declares it.
//
// SLOT 6 IS A HOST COPY AND THE REASON IS NEW. It is the third pointer-to-
// member shape port/mg_fanout_costs.txt section 4's FLW amendment describes:
// the src reads the pair as two plain ints and open-codes the ARM Itanium
// decode, so it compiles, it links, and both prescribed detectors read clean
// over it. THE ONLY THING THAT CONVICTS IT IS A RUN, and the symptom is eip on
// a raw DS address. port/unmatched/MgPanel_StateDispatch.cpp carries it and
// nine more; this class has TEN dispatching TUs against curling's five,
// because its state machine is two levels deep.
//
// ---- ONE SHARED FACE WAS REPAIRED, AND IT IS NOT COSMETIC ------------------
//
// mb_v35 above used to read
//
//     static int __fastcall mb_v35(void *, void *)
//     { MG_SLOT(35); func_ov004_020ad660(); return 0; }
//
// against a declaration of `void func_ov004_020ad660(void);` -- while
// src/func_ov004_020ad660.c defines `int func_ov004_020ad660(int *r0)
// { return (r0[2] & 0xff) != 0; }`. So the thunk dropped `this` (the callee
// read whatever was on the stack) AND discarded the answer, returning a
// constant 0. On ARM both halves ride through and are correct; on the host
// neither is. It is the same family as the slot-18/19 stack repair lane
// BASESET made and the Scene::BeforeBehavior repair lane FDR2 made.
//
// NOTHING HAD EVER CALLED IT, which is why it survived: the six classes seated
// before this one never dispatch slot 35, and the curling canary (scene 374)
// reproduces byte for byte across the repair. dScMgPanel_c dispatches it ONCE
// PER BOOT, from func_ov006_021063a0 on the init path -- an earlier version of
// this paragraph said "every frame, through func_ov006_021057f0" and the run
// refutes both halves; the mb_v35 block above carries the measurement and the
// four dispatch sites. The gate decides WHICH OF TWO BOARD LAYOUTS
// InitResources builds, so a constant 0 silently picked one, which is a worse
// failure to own than a per-frame predicate because no slot census shows it.

extern "C" {
/* the class's own vtable, in the ov006 mount. 36 slots, span-checked. */
extern unsigned char data_ov006_0213e24c[];   /* dScMgPanel_c,   36 slots */
extern unsigned char MgPuzzlePanelPuzzlePanic_SpawnInfo[];

/* the class's own six vtable bodies. func_ov006_02107358 is the HOST COPY in
   unmatched/MgPanel_StateDispatch.cpp, not the src TU. */
int   func_ov006_021073b0(void *self);          /* slot 0  InitResources */
int   func_ov006_02107358(char *self);          /* slot 6  Behavior, host copy */
int   func_ov006_0210730c(void *self);          /* slot 9  Render */
int   func_ov006_0210428c(int *self);           /* slot 16 D2 */
int  *func_ov006_021042b0(int *self);           /* slot 17 D0 */
void  func_ov006_021071fc(char *self, int flag);/* slot 18 state reset */

int  *MgPuzzlePanelPuzzlePanic_Spawn(void);

/* the state machine's witness, from unmatched/MgPanel_StateDispatch.cpp */
void port_mg_panel_counts(unsigned *hits, unsigned *floor, unsigned *unknown);

void port_scene_mg_panel_hits(void);
}

static int  __fastcall mp_init(void *s, void *)
{ MG_SLOT(0);  const int r = func_ov006_021073b0(s);
  hal_gapless_minigames_latch(); return r; }
static int  __fastcall mp_beh(void *s, void *)
{ MG_SLOT(6);  const int r = func_ov006_02107358((char *)s);
  hal_gapless_splice(); return r; }
static int  __fastcall mp_render(void *s, void *)
{ MG_SLOT(9);  return func_ov006_0210730c(s); }
static void *__fastcall mp_d2(void *s, void *)
{ MG_SLOT(16); return (void *)(size_t)func_ov006_0210428c((int *)s); }
static void *__fastcall mp_d0(void *s, void *)
{ MG_SLOT(17); return (void *)func_ov006_021042b0((int *)s); }
static int  __fastcall mp_reset(void *s, void *, int flag)
{ MG_SLOT(18); func_ov006_021071fc((char *)s, flag); return 1; }

/* dScMgPanel_c's own six, keyed on the ROM word each slot holds. None of the
   six appears in kMgBaseFaces or in any earlier class's array -- a word is one
   address -- so the key sets stay disjoint by construction. */
static const MgFace kPanelFaces[] = {
    {0x021073b0u, (void *)mp_init},   {0x02107358u, (void *)mp_beh},
    {0x0210730cu, (void *)mp_render}, {0x0210428cu, (void *)mp_d2},
    {0x021042b0u, (void *)mp_d0},     {0x021071fcu, (void *)mp_reset},
};

/* The scene object, for the run report below. Nothing else reads it. */
static char *g_mg_panel_self;

/* The registry's factory column is void *(*)(void) and the matched factory
   returns int *. One typed forwarder, the shape port_mg_curling_spawn has.
   NO DISPLACEMENT RULING IS NEEDED: src/MgPuzzlePanelPuzzlePanic_Spawn.c calls
   func_ov004_020b2adc(p) WITH the object pointer, where dScMgCup_c's factory
   calls the same base constructor with none and rides r0 through. Confirmed
   against the ROM at 0x0210786c (arm_call to 0x020b2adc) with r0 = p live. */
extern "C" void *port_mg_panel_spawn(void)
{
    void *p = (void *)MgPuzzlePanelPuzzlePanic_Spawn();
    g_mg_panel_self = (char *)p;
    return p;
}

extern "C" void port_scene_fill_panel(void)
{
    /* mounts before the fill, for port_scene_fill_curling's reason; both are
       idempotent behind their own static guards. */
    port_scene_mg_mounts();

    void **base = (void **)data_ov004_020bc0c0;
    void **vt   = (void **)data_ov006_0213e24c;

    /* The base table again, idempotent for the reason the coin fill states:
       mg_apply keys on the ROM word and a slot already holding a host thunk
       matches nothing, so an earlier row's fill costs this one nothing and
       this fill is correct read on its own. */
    port_scene_fill_rom(base, 36);
    mg_apply(base, 36, kMgBaseFaces,
             sizeof kMgBaseFaces / sizeof kMgBaseFaces[0]);

    port_scene_fill_rom(vt, 36);
    mg_apply(vt, 36, kMgBaseFaces,
             sizeof kMgBaseFaces / sizeof kMgBaseFaces[0]);
    mg_apply(vt, 36, kPanelFaces,
             sizeof kPanelFaces / sizeof kPanelFaces[0]);

    {
        const char *s0 = std::getenv("SM64DS_SCENE_SLOT0");
        const char *s9 = std::getenv("SM64DS_SCENE_SLOT9");
        if (s0 && s0[0] == '0') vt[0] = (void *)mg_init_noop;
        if (s9 && s9[0] == '0') vt[9] = (void *)mg_render_noop;
    }

    {
        const unsigned lv = mg_raw_left(vt, 36);
        if (lv) {
            std::fprintf(stderr, "  [scene] MINIGAME FILL INCOMPLETE: "
                         "dScMgPanel_c leaves %u of 36 raw DS words. A "
                         "dispatch of any of them jumps to a DS address as a "
                         "host one.\n", lv);
            std::fflush(stderr);
        }
    }

    port_scene_mg_prepare(port_scene_env_want());

    if (port_scene_env_want() == 380) {
        static int armed;
        if (!armed) {
            armed = 1;
            std::atexit(port_scene_mg_panel_hits);
        }
    }
}

/* This class's own witness, registered only when scene 380 is the requested
   one, so a curling or level run prints nothing extra. hal/scene_boot.cpp's
   end-of-run block still has no third branch and is still not this lane's
   region to widen, which is why this is an atexit and not a line there. */
extern "C" void port_scene_mg_panel_hits(void)
{
    std::printf("[scene] dScMgPanel_c slot hits: init %u, behavior %u, "
                "render %u, D0 %u, state-reset %u, slot35 gate %u\n",
                g_mg_hits[0], g_mg_hits[6], g_mg_hits[9], g_mg_hits[17],
                g_mg_hits[18], g_mg_hits[35]);
    {
        unsigned hits = 0, floor = 0, unknown = 0, calls = 0, fwk = 0;
        port_mg_panel_counts(&hits, &floor, &unknown);
        port_mg_dispatch_counts(&calls, &fwk);
        /* THE TWO NUMBERS THAT READ ALIKE AND ARE NOT THE SAME. `routed` is
           this class's own two-level machine; `UNHANDLED` is a code word
           neither of its switches knows, and a nonzero one is the number that
           says a dispatching TU was missed rather than that a state is
           missing. Printed together on purpose. */
        std::printf("[scene] dScMgPanel_c state dispatch: %u routed to one of "
                    "its 26 reachable states across six tables, %u code "
                    "word(s) this class did not know; the framework switch saw "
                    "%u call(s) and %u UNHANDLED address(es)\n",
                    hits, unknown, calls, fwk);
        /* ONE FLOOR IS LEFT AND IT IS A SUB-STATE. 0x021053a8 is slot 2 of
           data_ov006_02142820: a class that asks for it loses one tick and
           carries on. It is not counted as a routed hit -- the switch returns
           -1 for a floor, so `routed` above is states that reached a real
           body.
           THE SECOND FLOOR IS RETIRED. This line used to carry 0x02106ca4 as
           well, and on the mg6 1200-frame run that number was 951 -- the
           class had settled in a TOP-LEVEL state with no body and asked for it
           every frame afterwards. Run mg7 lane L380 decompiled it; it is a
           real case in the address switch now, so it can only appear in
           `routed`. */
        std::printf("[scene] dScMgPanel_c floor: %u ask(s) for the sub-state "
                    "0x021053a8 (slot 2 of data_ov006_02142820), which has a "
                    "config symbol, no delink block and no src file, so it is "
                    "not entered (total bodiless asks %u)\n",
                    floor, floor);
    }
    /* THE LAYOUT IS THE PROOF LINE. func_ov006_02106168 is the sole writer of
       the two panel arrays -- the CURRENT face at +0x4f1e and the TARGET face
       at +0x4f42 -- and func_ov006_021067a4 zeroes both just before it runs.
       Before it had a body they stayed zero, so the round-over test compared
       zero to zero and passed on the first frame. The first bytes of each are
       printed so a run says whether a board was actually dealt. */
    if (g_mg_panel_self) {
        const int n = *(int *)(g_mg_panel_self + 0x4cb8);
        const unsigned char *cur = (const unsigned char *)(g_mg_panel_self + 0x4f1e);
        const unsigned char *tgt = (const unsigned char *)(g_mg_panel_self + 0x4f42);
        int i;
        int lim = n;
        int differ = 0;
        if (lim < 0)  lim = 0;
        if (lim > 36) lim = 36;
        std::printf("[scene] dScMgPanel_c board: %d panel(s), side %d, moves "
                    "%d, face set %d\n",
                    n, *(int *)(g_mg_panel_self + 0x4cbc),
                    *(int *)(g_mg_panel_self + 0x4cc0),
                    *(int *)(g_mg_panel_self + 0x4cb4));
        std::printf("[scene]   +0x4f1e:");
        for (i = 0; i < lim; i++) std::printf(" %02x", cur[i]);
        std::printf("\n[scene]   +0x4f42:");
        for (i = 0; i < lim; i++) std::printf(" %02x", tgt[i]);
        for (i = 0; i < lim; i++) if (cur[i] != tgt[i]) differ++;
        std::printf("\n[scene]   %d of %d panel(s) differ -- a zero here on a "
                    "dealt board is the round-over test comparing zero to "
                    "zero\n", differ, lim);
        std::printf("[scene] dScMgPanel_c object at %p, state index %d, "
                    "mode byte %u\n", (void *)g_mg_panel_self,
                    *(int *)(g_mg_panel_self + 0x4ca8),
                    (unsigned)(*(unsigned char *)(g_mg_panel_self + 8)));
    }
    std::fflush(stdout);
}
