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

#include <cstdio>
#include <cstdlib>

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
void  func_ov004_020ae3b4(void *c);
void  func_ov004_020ad660(void);

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
{ MG_SLOT(0);  return func_ov006_020e3578(s); }
static int  __fastcall mg_beh(void *s, void *)
{ MG_SLOT(6);  return func_ov006_020e3528(s); }
static int  __fastcall mg_render(void *s, void *)
{ MG_SLOT(9);  return func_ov006_020e34ec(s); }
static void *__fastcall mg_d2(void *s, void *)
{ MG_SLOT(16); return (void *)(size_t)func_ov006_020e0638(s); }
static void *__fastcall mg_d0(void *s, void *)
{ MG_SLOT(17); return (void *)(size_t)func_ov006_020e065c(s); }
static int  __fastcall mg_reset(void *s, void *)
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
static int  __fastcall mb_v19(void *, void *)      { MG_SLOT(19); return func_ov004_020b2994(); }
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
static int  __fastcall mb_v34(void *s, void *)     { MG_SLOT(34); func_ov004_020ae3b4(s); return 0; }
static int  __fastcall mb_v35(void *, void *)      { MG_SLOT(35); func_ov004_020ad660(); return 0; }

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
static int  __fastcall mb_reset_base(void *, void *)
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

// ---- the fill --------------------------------------------------------------
extern "C" void port_scene_mg_hits(void);   /* defined at the foot of this file */

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
