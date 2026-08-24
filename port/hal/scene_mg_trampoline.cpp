// dScMgTrampoline_c, the "Trampoline Time" minigame -- actor id 0x180, scene
// 384.  Run mg11, lane TTI.  The twenty-second ov006 minigame class the port
// seats and the FIRST under dScMgD3DBase_c.
//
// Read port/slice_tti.txt for the identity derivation, the five width checks
// and the closure.  Read port/unmatched/MgTrampolineTime_StateDispatch.cpp for
// the wall.  This file is the seat: three vtables' faces, an ELEMENT vtable's
// faces, the fill, the factory forwarder and the run census.
//
// ---- 1. WHY THIS IS A SEPARATE FILE FROM hal/scene_mg.cpp -----------------
//
// The reason hal/scene_mg_flower.cpp, hal/scene_mg_memory2.cpp and
// hal/scene_mg_luckystars.cpp give, unchanged.  scene_mg.cpp is the FAMILY
// half -- the mounts, the thirty-five overlay constructors, dScMgBase_c's
// address-keyed faces and the gate that keeps the constructors off a level
// boot -- and all of it is reached through the one seam that file exports,
// port_scene_mg_fill_shared().  Nothing of it is re-derived here and
// kMgBaseFaces is NOT copied: a second copy would install a different set of
// host thunks over the same DS addresses in the same mounted tables and split
// the per-slot witness in two.
//
// ---- 2. THE HIERARCHY IS THREE DEEP AND THE MIDDLE BASE IS NEW ------------
//
//     Scene -> dScMgBase_c        data_ov004_020bc0c0   36 slots
//           -> dScMgD3DBase_c     data_ov006_0213c62c   36 slots, ovr 17
//           -> dScMgTrampoline_c  data_ov006_0213fb34   36 slots, ovr 25
//
// port/mg_fanout_costs.txt section 3 lists ONE vtable for 0x180 and says
// nothing about an intermediate base -- the same gap section 13's CORRECTION 1
// records and the same one that fired for dScMgSingle3DBase_c at run mg9.  It
// fires here for a FOURTH base, and the arithmetic is exact: of this class's
// 25 overrides against dScMgBase_c, THIRTEEN hold the middle base's body
// unchanged and only TWELVE are its own.  A seat that filled one derived table
// would have left thirteen slots holding raw DS words.
//
//   OWN     slots 0, 3, 6, 9, 16, 17, 18, 19, 23, 24, 25, 34   (12)
//   MIDDLE  slots 1, 2, 5, 7, 10, 11, 26, 27, 28, 29, 30, 31, 33  (13)
//
// THE ROM'S OWN type_info IS THE WITNESS, not a name:
//
//     0x0213fb30 -> 0x0213fad0 +4 -> 0x0213faf0 = "17dScMgTrampoline_c"
//                                +8 -> 0x0213c5c8 +4 -> "14dScMgD3DBase_c"
//
// and the code agrees twice: src/MgTrampolineTime_Spawn.c writes the middle
// table into the object and then data_ov006_0213fb34 over it, and slot 17's
// literal pool (0x02120914 = 0x0213fb34, 0x02120930 = 0x0213c62c) unwinds them
// in the opposite order.
//
// THE MIDDLE TABLE IS MISNAMED IN config AND FOUR CLASSES SHARE IT.
// config/arm9/overlays/ov006/symbols.txt calls 0x0213c62c
// `_ZTV17MgBounceAndPounce`; the ROM's RTTI says dScMgD3DBase_c, and five
// type_info records in ov006 carry 0x0213c5c8 as their base:
// dScMgD3DBase_c itself, dScMgJump_c (0x174), dScMgJump2_c (0x175),
// THIS class (0x180) and dScMgTrampoline2_c (0x181).
// symbols/actor_renames_report.txt already records the consequence as NINE
// "already claimed by MgBounceAndPounce" SKIP lines, and nine more for
// MgTrampolineTerror.  FOUR committed places in this lane said eight; the one
// all four dropped is the FIRST, `ov006:0x213c62c ... (vtable alloc=?)`,
// which is the TABLE rather than a body in it -- the row that put the
// MgBounceAndPounce name on dScMgD3DBase_c's vtable in the first place.  The
// first correction of this said THREE and fixed three; the fourth was
// port/tools/inferred_stub_adjudicated.txt and it took a second review to
// find, because the sweep behind the word "three" was a file list rather
// than a grep over the tree.  This
// lane does not rename config -- three sibling lanes are live on the same base
// in this run -- it records the correction for the decomp side to route.
//
// ---- 3. THE FILL IS ADDRESS-KEYED, SO FOUR TABLES ARE CHEAP ---------------
//
// port_scene_mg_fill_shared and tti_apply both key on the ROM WORD a slot
// holds, not on the slot number, so the same face array can be applied to
// every table in the chain and can only land where the ROM parked that exact
// body.
//
//   kD3DBaseFaces   the SEVENTEEN bodies dScMgD3DBase_c overrides.  Thirteen
//                   of them fill this class's own inherited slots; the other
//                   four (the middle base's slots 16, 17, 24 and 25) exist so
//                   the MIDDLE table closes completely, because that table is
//                   live storage in its own right -- the factory writes it
//                   into the object before the derived table lands and both
//                   destructors write it back on the way down.
//   kTtiFaces       the twelve this class adds or replaces on top.
//   kTrmplnMarioFaces  the FIVE slots of the ELEMENT vtable, section 5.
//
// THE MIDDLE TABLE IS FILLED IN TWO SPANS AND THAT IS NOT COSMETIC.  The ov006
// mount emits one host array per config symbol, and config puts an interior
// symbol inside this table: `_ZTV17MgBounceAndPounce` is 80 bytes (20 slots)
// and `data_ov006_0213c67c` is the next 64 (16 slots).  Writing 36 words
// through the first array would be an out-of-bounds write that happens to land
// correctly only because the pack keeps ROM adjacency.  The fill is
// address-keyed and therefore order-independent, so applying it to the two
// spans separately is exact and needs no pack assumption.
//
// THIS IS THE FIRST CLASS UNDER THIS MIDDLE BASE, so on this tree this seat's
// arrays CLAIM the middle table rather than finding it already hosted.  Three
// sibling lanes (0x174, 0x175, 0x181) are seating the other three classes
// under it in the same run.  When they land, whichever registry row runs FIRST
// claims it and the others find no DS word left and write nothing -- the
// kSingle3DFaces precedent hal/scene_mg_luckystars.cpp section 3 records.  The
// claimed count is PRINTED rather than assumed, so a merged tree says out loud
// which lane's thunks the middle table is carrying.
//
// ---- 4. THE WIDTH IS 36 ON BOTH SCENE TABLES, CHECKED FIVE WAYS -----------
//
// port/mg_fanout_costs.txt section 11 measured that twelve of twenty-nine
// widths in its own table were phantoms and that a 37-slot fill writes a host
// thunk over live ROM data belonging to another class.  Every check was redone
// here before a word was written:
//
//   table      span to next symbol      slot 35     word past the end
//   ---------  ----------------------   ---------   ------------------------
//   0213fb34   0213fbc4, 0x90 = 36      020ad660    0x00000001
//   0213c62c   0213c67c is INTERIOR     020ad660    0x6f6d6168 ("hamo")
//
//   4. RELOCATION COUNT.  Exactly 36 load relocations fall inside each table.
//   5. RELOCATION STRIDE.  ov006's relocs.txt carries 36 CONTIGUOUS load
//      relocations at a FOUR-byte stride from 0x0213fb34 to 0x0213fbc0, and
//      the stride then JUMPS TO EIGHT at 0x0213fbc8.  A vtable is relocated
//      every word; an mwcc member-pointer table every other word.  Same on the
//      middle table: 4-byte stride to 0x0213c6b8, next relocation 0x0213c6c4.
//
// THE SPAN CHECK DOES NOT RESOLVE THE MIDDLE TABLE AND THIS FILE SAYS SO.
// data_ov006_0213c67c is an `ambiguous` config symbol 20 words into the table
// with no relocation pointing at it; the real end is the NEXT symbol,
// data_ov006_0213c6bc, exactly 0x90 on.  port/tools/vtablerows.py refuses
// --width 36 on 0x0213c62c for that reason, which is the tool doing its job:
// it refuses, it does not promote.  The other four checks pin it at 36.
//
// WHOSE BYTES A 37TH SLOT WOULD TAKE, NAMED, because section 11 asks:
// index 36 of THIS class's table is 0x0213fbc4, and that word is
// dScMgTrampoline2_c's own int flag -- 0x181's slot-23 body loads it from
// 0x021230bc, the exact mirror of this class's slot 23 loading data_ov006_
// 0213fa9c.  A 37-slot fill here would write a host thunk over the SIBLING's
// pause flag, and lane TTE is seating that sibling in this same run.
// Index 36 of the MIDDLE table is the first four characters of a string.
// The fills below are called with 36 and with 20+16.
//
// ---- 5. THE ELEMENT VTABLE, AND IT IS DISPATCHED --------------------------
//
// THE SINGLE MOST IMPORTANT THING THIS SEAT DOES THAT THE VTABLE AXIS CANNOT
// SEE, and it is the check run mg9 lane S381 turned into law: check word 0 of
// every factory product.  src/MgTrampolineTime_Spawn.c builds three arrays
// through func_020733a8:
//
//   this+0x500c   4 records of 0xd0    ctor func_ov006_020cd12c   VTABLE
//   this+0x534c   3 records of 0x32c   ctor func_ov006_020d100c   no vtable
//   this+0x5cd0   5 records of 0x24    ctor func_ov006_021225a8   no vtable
//
// The first constructor's body is a call, then `str r1,[r4]` with r1 loaded
// from its own pool at 0x020cd154 = 0x0213b2c4.  So every one of those four
// records carries a MOUNTED ROM TABLE as its vptr.  Its type_info sits at
// 0x0213b2c0 -> 0x0213b244, whose name pointer reads "18dMgTrmpln3DMario_c",
// and the ONLY two relocations naming data_ov006_0213b2c4 in the whole overlay
// are this class's element constructor (0x020cd154) and its element destructor
// (0x020cd0c0, inside func_ov006_020ccfc8).  Nothing else in ov006 builds an
// object with that vptr, and this class's factory is the ONLY caller of
// func_ov006_020cd12c anywhere in the overlay -- so the array is this class's
// alone and no sibling lane can collide on it.
//
// IT IS DISPATCHED, AND THE PROOF IS REACHABILITY RATHER THAN AN EYEBALL.
// Four of its five slot bodies -- func_ov006_020cafac, _020cafa4, _020caf44
// and _020caf4c -- have EXACTLY ONE reference each in all of ov006's
// relocations, and that reference IS the vtable slot.  No arm_call anywhere
// reaches them.  A body that only the vtable names is a body only the vtable
// can call, so a seat that filled the three scene tables and left this one
// would link, boot, and jump to a raw DS address the first time a record was
// asked for its position.  Slot 4 (func_ov006_020cd720) is the exception and
// is called directly as well, from 0x020cd6e0.
//
// THE WIDTH IS 5, and the span check is an upper bound here rather than the
// answer: the next config symbol is data_ov006_0213b2e0, 7 words on, but the
// last two of those words are the NEXT vtable's Itanium header -- 0x0213b2d8
// is a zero offset-to-top with no relocation and 0x0213b2dc is a relocation to
// a type_info, not to code.  Exactly 5 load relocations fall on slots.  The
// mounted array is 28 bytes and the fill writes the first 20 of them.
//
// ---- 6. THE ROW GOES LAST, AND THE ORDER IS LOAD-BEARING ------------------
//
// port_scene_registry_install walks port_scene_classes[] in table order and
// calls every row's fill on every boot; port_scene_mg_overlay_load runs the
// thirty-five constructors ONCE PER PROCESS from the tail of the first
// minigame row's fill.  Appending after every existing row therefore means the
// constructors have already run with clean ROM words when this fill starts,
// which is the latent-safe direction section 11 names -- and section 3 above
// depends on the same ordering for the middle table.
//
// ---- 7. TWO RIDE-THROUGHS ARE REPAIRED HERE, NOT IN src -------------------
//
// The middle base has the flower's slot-2 disease in TWO of its own bodies,
// and both were found by reading the ROM rather than by a run:
//
//   SLOT 2, func_ov006/_ZN17MgBounceAndPounce18AfterInitResourcesEj at
//   0x020e70c0.  The ROM does `mov r4,r0` and then `bl 0x020b08f0` with r1
//   NEVER WRITTEN, so the framework's flags word rides through in r1.  src
//   spells the call with one argument because that is the only way to spell an
//   unnamed value in C.  On the host the second argument would be stack litter.
//
//   SLOT 10, _ZN17MgBounceAndPounce12BeforeRenderEv at 0x020e7040.  The ROM
//   does `bl 0x020b04f4` with r0 UNTOUCHED from entry, so `this` itself rides
//   through.  src takes NO parameter at all and calls func_ov004_020b04f4()
//   with none -- a DROPPED RECEIVER, which is the bin-softlock disease the run
//   law names, and it would hand dScMgBase_c::BeforeRender a garbage `this`.
//
// Both repairs are in port/unmatched/MgD3DBase_Slots.cpp under port_
// names, the port_mg_flower_after_init precedent exactly: this tree does not
// claim a decompilation it does not have, so those two src TUs stay OUT of
// port/slice_tti.txt and the faces below call the repaired bodies.
//
// SLOTS 18 AND 19 DO NOT READ A SECOND ARGUMENT IN THIS CLASS, checked rather
// than assumed because run mg9 lane LKY found a class where they do.  0x02121fa4
// writes r1 (`mov r1,#1`) before any read of it; 0x02121f70's first
// instruction after the prologue is `bl 0x020ad674`, which clobbers r0 and
// never reads r1.  The thunks therefore exist only so __fastcall cleans the
// stack, and they say so.
//
// ---- 8. WHAT THIS SEAT DOES NOT CLAIM -------------------------------------
//
// The state machine is proven to run by the census this file prints, not by
// this comment.  A run that reports slot hits and zero routed dispatches has
// ticked an object without entering its state machine, and that reads as a
// success unless it is printed -- which is why the numbers below are printed
// whether they are zero or not.  This class has no state INDEX field, so the
// only progression witness that exists for it is the chain-link histogram the
// dispatch file keeps, and section 15's finding applies as well: a census that
// measures only dispatch cannot see a board that was never built, so the four
// element records' vptrs are counted too.

#include "hal/screen_gap.h"

#include <cstdio>
#include <cstdlib>

#include "MgBounceAndPounce.h"

extern "C" {

/* the seam into hal/scene_mg.cpp -- mounts, arm9 fill, dScMgBase_c's faces */
unsigned port_scene_mg_fill_shared(void **vt, unsigned n);
void     port_scene_mg_prepare(int id);
int      port_scene_env_want(void);
unsigned port_mg_scene_spawn_param(int scene_id);

/* the mount storage the fill writes into.  All of them are the mounts' own, so
   the fill replaces the mounted words rather than building a fresh host array
   -- the ov007, curling, memory2 and luckystars treatment, for the same
   reason: a second host array of the same name is a duplicate symbol, and
   leaving the mounted table alone leaves live wild DS pointers in a table the
   factory installs.

   THE MIDDLE TABLE IS TWO ARRAYS because config puts an interior symbol
   inside it; see section 3. */
extern unsigned char data_ov004_020bc0c0[];        /* dScMgBase_c,        36 */
extern unsigned char _ZTV17MgBounceAndPounce[];    /* dScMgD3DBase_c,  0..19 */
extern unsigned char data_ov006_0213c67c[];        /* dScMgD3DBase_c, 20..35 */
extern unsigned char data_ov006_0213fb34[];        /* dScMgTrampoline_c,  36 */
extern unsigned char data_ov006_0213b2c4[];        /* dMgTrmpln3DMario_c,   5 */
extern unsigned char MgTrampolineTime_SpawnInfo[];

/* dScMgD3DBase_c's seventeen overrides.  Slots 2 and 10 are NOT src's bodies:
   see section 7 and port/unmatched/MgD3DBase_Slots.cpp. */
int  port_mg_d3dbase_after_init(void *c, unsigned f);        /* slot  2 */
int  port_mg_d3dbase_before_render(void *c);                 /* slot 10 */
void port_mg_d3dbase_slot27(void *c);                        /* slot 27 */
void port_mg_d3dbase_slot28(void *c);                        /* slot 28 */

/* the plain-C-named half of the middle base.  The four that src spells as real
   C++ methods (BeforeInitResources, AfterCleanupResources, BeforeBehavior,
   AfterRender) are reached through MgBounceAndPounce.h instead, because MSVC
   compiles them __thiscall and a C declaration of one would be a different
   symbol. */
int  _ZN17MgBounceAndPounceD1Ev(void *self);                 /* slot 16 D1 */
int  _ZN17MgBounceAndPounceD0Ev(void *self);                 /* slot 17 D0 */
int  func_ov006_020e6e78(char *self);                        /* slot 24 */
int  func_ov006_020e6e54(void *t);                           /* slot 25 */
int  func_ov006_020e6e4c(void);                              /* slot 26 */
void func_ov006_020e6d24(char *c);                           /* slot 29 */
void func_ov006_020e6cac(char *c);                           /* slot 30 */
void func_ov006_020e72c0(char *c);                           /* slot 31 */
void func_ov006_020e7124(char *obj);                         /* slot 33 */

/* dScMgTrampoline_c's own twelve.  func_ov006_021214f8 is the HOST COPY in
   unmatched/MgTrampolineTime_StateDispatch.cpp, not the src TU: it is the
   pointer-to-member dispatcher and the port cannot compile the src. */
int   func_ov006_02122198(char *base);        /* slot  0 InitResources     */
int   func_ov006_021212e0(void *t);           /* slot  3 CleanupResources  */
int   func_ov006_021214f8(void *self);        /* slot  6 Behavior, host cp */
int   func_ov006_021212fc(int self);          /* slot  9 Render            */
void *func_ov006_021207dc(void *thiz);        /* slot 16 D2                */
void *func_ov006_02120880(char *thiz);        /* slot 17 D0                */
void  func_ov006_02121fa4(char *o);           /* slot 18 state reset       */
int   func_ov006_02121f70(void);              /* slot 19                   */
int   func_ov006_0212101c(char *self);        /* slot 23                   */
int   func_ov006_021211e0(char *self);        /* slot 24                   */
int   func_ov006_021211bc(void *t);           /* slot 25                   */
void  func_ov006_02120da8(void *obj, int x_base, int y, int val, int n);
                                              /* slot 34, five parameters  */

/* dMgTrmpln3DMario_c's five, section 5 */
int  func_ov006_020cafac(int p);
int  func_ov006_020cafa4(int p);
int  func_ov006_020caf44(int p);
int  func_ov006_020caf4c(int *r0);
void func_ov006_020cd720(short *p);

/* the factory */
void *MgTrampolineTime_Spawn(void);

/* the dispatch file's witnesses */
unsigned port_mg_tti_state_hits(void);
/* the floor file's witness, unmatched/MgTrampolineTime_Floors.cpp */
unsigned port_mg_tti_hittest_calls(void);
/* The DIRECT evidence that a stylus stroke was judged, now that the hit test is
   real code and its trap counter is retired: the three trampoline records
   func_ov006_020d0c38 writes when it accepts a stroke. Defined once in
   hal/trampoline_records.cpp and read by BOTH scenes' censuses. */
void     port_mg_trampoline_record_report(void);
void     port_mg_tti_floor_counts(unsigned *hit, unsigned *f2fc,
                                  unsigned *f1e0);
unsigned port_mg_tti_floor_hits(void);
void     port_mg_tti_link_range(int *lo, int *hi);
void     port_mg_tti_link_hits(unsigned *out5);
void     port_mg_tti_anomalies(unsigned *unset, unsigned *virt, unsigned *last);
/* the framework's, from unmatched/MgBase_StateDispatch.cpp */
void     port_mg_dispatch_counts(unsigned *calls, unsigned *unknown);

/* the persistent minigame record this class's level is READ FROM and WRITTEN
   BACK TO.  Nothing new is defined here: the storage is hal/level_boot.cpp's
   .dsstate$savblk0004 and the index function is src/func_ov004_020adc3c.c. */
extern void         *data_ov004_020beb68;     /* the live dScMgBase_c `this` */
extern unsigned char data_0209caf4[];         /* 36 records x 20 bytes       */
int  func_ov004_020adc3c(void *c);            /* (self->field_8 >> 8) & 0xff */

}  /* extern "C" */

/* The record row this class's object is keyed to, or -1 with no live base. */
static int port_mg_tti_record_index(void)
{
    return data_ov004_020beb68 ? func_ov004_020adc3c(data_ov004_020beb68) : -1;
}

// ---- the tick witness ------------------------------------------------------
//
// One counter per dispatched slot on each of the two class tables this file
// owns faces for, plus one per element-vtable slot.  The instrument every scene
// seat in this port carries, for the reason hal/scene_mg.cpp gives: an object
// that EXISTS and an object that RUNS look identical from outside.  The
// framework slots dScMgBase_c keeps are counted by scene_mg.cpp's own array.
static unsigned g_tti_hits[36];
static unsigned g_d3d_hits[36];    /* the same slots on the MIDDLE table */
static unsigned g_mario_hits[5];   /* the element vtable */

#define TTI(n)   (++g_tti_hits[(n)])
#define D3D(n)   (++g_d3d_hits[(n)])
#define MAR(n)   (++g_mario_hits[(n)])

/* The values slots 18 and 19 were last handed.  Section 7 says neither ROM body
   reads its second argument; the census prints what the thunk received anyway,
   because "the class does not read it" is a claim about the ROM and this is the
   measurement of the host. */
static int g_tti_mode18 = -1, g_tti_mode19 = -1;

/* ---- dScMgD3DBase_c's seventeen ------------------------------------------ */
static int  __fastcall d3_binit(void *s, void *)
{ D3D(1);  return ((MgBounceAndPounce *)s)->BeforeInitResources(); }
static int  __fastcall d3_ainit(void *s, void *, unsigned f)
{ D3D(2);  return port_mg_d3dbase_after_init(s, f); }
static void __fastcall d3_aclean(void *s, void *, unsigned f)
{ D3D(5);  ((MgBounceAndPounce *)s)->AfterCleanupResources(f); }
static int  __fastcall d3_bbeh(void *s, void *)
{ D3D(7);  return ((MgBounceAndPounce *)s)->BeforeBehavior(); }
static int  __fastcall d3_bren(void *s, void *)
{ D3D(10); return port_mg_d3dbase_before_render(s); }
static void __fastcall d3_aren(void *s, void *, unsigned f)
{ D3D(11); ((MgBounceAndPounce *)s)->AfterRender(f); }
static void *__fastcall d3_d1(void *s, void *)
{ D3D(16); return (void *)(size_t)_ZN17MgBounceAndPounceD1Ev(s); }
static void *__fastcall d3_d0(void *s, void *)
{ D3D(17); return (void *)(size_t)_ZN17MgBounceAndPounceD0Ev(s); }
static int  __fastcall d3_v24(void *s, void *)
{ D3D(24); return func_ov006_020e6e78((char *)s); }
static int  __fastcall d3_v25(void *s, void *)
{ D3D(25); return func_ov006_020e6e54(s); }
static int  __fastcall d3_v26(void *, void *)
{ D3D(26); return func_ov006_020e6e4c(); }
/* SLOTS 27 AND 28 TAKE THE RECEIVER AND THIS LANE HAD DROPPED IT.  The two
   ROM bodies are `ldr ip,[pc] / bx ip` veneers onto dScMgBase_c's own slot-27
   and slot-28 bodies, so r0 rides through untouched; their src TUs take no
   parameter and this file used to call them the same way, which handed both
   arm9 bodies stack litter as `this`.  Lanes BNT and BNP found it first and
   port/unmatched/MgD3DBase_Slots.cpp is their repair, adopted verbatim. */
static int  __fastcall d3_v27(void *s, void *)
{ D3D(27); port_mg_d3dbase_slot27(s); return 1; }
static int  __fastcall d3_v28(void *s, void *)
{ D3D(28); port_mg_d3dbase_slot28(s); return 1; }
static int  __fastcall d3_v29(void *s, void *)
{ D3D(29); func_ov006_020e6d24((char *)s); return 1; }
static int  __fastcall d3_v30(void *s, void *)
{ D3D(30); func_ov006_020e6cac((char *)s); return 1; }
static int  __fastcall d3_v31(void *s, void *)
{ D3D(31); func_ov006_020e72c0((char *)s); return 1; }
static int  __fastcall d3_v33(void *s, void *)
{ D3D(33); func_ov006_020e7124((char *)s); return 0; }

/* ---- dScMgTrampoline_c's own twelve --------------------------------------- */
static int  __fastcall tti_init(void *s, void *)
{ TTI(0);  const int r = func_ov006_02122198((char *)s);
  /* the GaplessMinigames latch, for hal/scene_mg.cpp's reason: every seated
     minigame calls it so the ones the gapless table does not name can say
     "unsupported" instead of doing nothing quietly.  hal_gapless_splice() is
     deliberately NOT called -- scene 384 has no row in screen_gap.cpp's table
     and gapless behaviour is decided per minigame BY PLAYING IT.  This lane has
     not played it and does not get to vote.  Same ruling as
     hal/scene_mg_bomroom.cpp section 8 and hal/scene_mg_luckystars.cpp. */
  hal_gapless_minigames_latch(); return r; }
static int  __fastcall tti_clean(void *s, void *)
{ TTI(3);  return func_ov006_021212e0(s); }
static int  __fastcall tti_beh(void *s, void *)
{ TTI(6);  return func_ov006_021214f8(s); }
static int  __fastcall tti_render(void *s, void *)
{ TTI(9);  return func_ov006_021212fc((int)(size_t)s); }
static void *__fastcall tti_d2(void *s, void *)
{ TTI(16); return func_ov006_021207dc(s); }
static void *__fastcall tti_d0(void *s, void *)
{ TTI(17); return func_ov006_02120880((char *)s); }
/* SLOTS 18 AND 19 TAKE ONE STACK ARGUMENT AND THIS CLASS IGNORES IT -- section
   7, read off the ROM.  The parameter is declared so __fastcall cleans the four
   bytes, and its value is recorded so the census can show what arrived rather
   than assert that it does not matter. */
static int  __fastcall tti_reset(void *s, void *, int mode)
{ TTI(18); g_tti_mode18 = mode; func_ov006_02121fa4((char *)s); return 1; }
static int  __fastcall tti_v19(void *, void *, int mode)
{ TTI(19); g_tti_mode19 = mode; return func_ov006_02121f70(); }
static int  __fastcall tti_v23(void *s, void *)
{ TTI(23); return func_ov006_0212101c((char *)s); }
static int  __fastcall tti_v24(void *s, void *)
{ TTI(24); return func_ov006_021211e0((char *)s); }
static int  __fastcall tti_v25(void *s, void *)
{ TTI(25); return func_ov006_021211bc(s); }
/* SLOT 34 TAKES FOUR ARGUMENTS PAST `this`, which is the mb_v34 shape run mg9
   lane S371 reported as a latent defect and lane LKY closed: slot 34's only
   dispatcher tree-wide is func_ov004_020ae5c4, and no scene reaches it.  This
   is the THIRD class whose slot-34 body has that arity, so the finding
   corroborates rather than contradicts.  The thunk forwards all four anyway --
   an unreachable slot is not a licence to write a wrong one. */
static void __fastcall tti_v34(void *s, void *, int a, int b, int c, int d)
{ TTI(34); func_ov006_02120da8(s, a, b, c, d); }

/* ---- dMgTrmpln3DMario_c's five, section 5 --------------------------------- */
static int  __fastcall mario_v0(void *s, void *)
{ MAR(0); return func_ov006_020cafac((int)(size_t)s); }
static int  __fastcall mario_v1(void *s, void *)
{ MAR(1); return func_ov006_020cafa4((int)(size_t)s); }
static int  __fastcall mario_v2(void *s, void *)
{ MAR(2); return func_ov006_020caf44((int)(size_t)s); }
static int  __fastcall mario_v3(void *s, void *)
{ MAR(3); return func_ov006_020caf4c((int *)s); }
static int  __fastcall mario_v4(void *s, void *)
{ MAR(4); func_ov006_020cd720((short *)s); return 0; }

/* SM64DS_SCENE_SLOT0=0 and SM64DS_SCENE_SLOT9=0, the diagnostics every scene
   seat in this port carries, counted separately so a run can never read a no-op
   as the real body having run. */
static unsigned g_tti_init_skipped, g_tti_render_skipped;
static int __fastcall tti_init_noop(void *, void *)
{ ++g_tti_init_skipped; return 1; }
static int __fastcall tti_render_noop(void *, void *)
{ ++g_tti_render_skipped; return 1; }

struct TtiFace { unsigned ds; void *host; };

/* Keyed on the ROM WORD each slot holds, so the arrays are order-independent
   and cannot land on a slot the ROM did not park that body in. */
static const TtiFace kD3DBaseFaces[] = {
    {0x020e70e4u, (void *)d3_binit},  {0x020e70c0u, (void *)d3_ainit},
    {0x020e6f60u, (void *)d3_aclean}, {0x020e7074u, (void *)d3_bbeh},
    {0x020e7040u, (void *)d3_bren},   {0x020e700cu, (void *)d3_aren},
    {0x020e6c28u, (void *)d3_d1},     {0x020e6c60u, (void *)d3_d0},
    {0x020e6e78u, (void *)d3_v24},    {0x020e6e54u, (void *)d3_v25},
    {0x020e6e4cu, (void *)d3_v26},    {0x020e6d98u, (void *)d3_v27},
    {0x020e6d8cu, (void *)d3_v28},    {0x020e6d24u, (void *)d3_v29},
    {0x020e6cacu, (void *)d3_v30},    {0x020e72c0u, (void *)d3_v31},
    {0x020e7124u, (void *)d3_v33},
};

static const TtiFace kTtiFaces[] = {
    {0x02122198u, (void *)tti_init},  {0x021212e0u, (void *)tti_clean},
    {0x021214f8u, (void *)tti_beh},   {0x021212fcu, (void *)tti_render},
    {0x021207dcu, (void *)tti_d2},    {0x02120880u, (void *)tti_d0},
    {0x02121fa4u, (void *)tti_reset}, {0x02121f70u, (void *)tti_v19},
    {0x0212101cu, (void *)tti_v23},   {0x021211e0u, (void *)tti_v24},
    {0x021211bcu, (void *)tti_v25},   {0x02120da8u, (void *)tti_v34},
};

static const TtiFace kTrmplnMarioFaces[] = {
    {0x020cafacu, (void *)mario_v0},  {0x020cafa4u, (void *)mario_v1},
    {0x020caf44u, (void *)mario_v2},  {0x020caf4cu, (void *)mario_v3},
    {0x020cd720u, (void *)mario_v4},
};

static unsigned tti_apply(void **vt, unsigned n, const TtiFace *f, unsigned nf)
{
    unsigned hit = 0;
    for (unsigned i = 0; i < n; ++i) {
        const unsigned ds = (unsigned)(size_t)vt[i];
        for (unsigned k = 0; k < nf; ++k)
            if (f[k].ds == ds) { vt[i] = f[k].host; ++hit; break; }
    }
    return hit;
}

/* Words still holding a DS address.  A minigame table is 36 slots and every one
   of them is dispatched by something, so a nonzero answer is a wild call
   waiting to happen and the seat says so out loud. */
static unsigned tti_raw_left(void **vt, unsigned n)
{
    unsigned left = 0;
    for (unsigned i = 0; i < n; ++i) {
        const unsigned w = (unsigned)(size_t)vt[i];
        if (w >= 0x02000000u && w < 0x02400000u)
            ++left;
    }
    return left;
}

extern "C" void port_scene_trampoline_hits(void);

/* How many words this seat's own copy of the middle-table array claimed.  On
   this tree the expected reading is SEVENTEEN (this is the first class seated
   under dScMgD3DBase_c); on a tree that also carries a sibling lane's row it
   drops to whatever that row left, and section 3 is why.  Printed rather than
   assumed either way. */
static unsigned g_tti_mid_claimed;
static unsigned g_tti_mario_claimed;

extern "C" void port_scene_fill_trampoline(void)
{
    void **base = (void **)data_ov004_020bc0c0;
    void **midA = (void **)_ZTV17MgBounceAndPounce;   /* slots  0..19 */
    void **midB = (void **)data_ov006_0213c67c;       /* slots 20..35 */
    void **vt   = (void **)data_ov006_0213fb34;
    void **elem = (void **)data_ov006_0213b2c4;

    /* THE BASE TABLE IS FILLED HERE TOO AND IT IS NOT CEREMONY.  Earlier rows'
       fills already did it and run first, so on a tree carrying them this is a
       second pass over words that are already host pointers and finds nothing.
       It is here so this class does not depend on another class's row existing:
       the factory's first act is func_ov004_020b2adc, which writes
       data_ov004_020bc0c0 into the object's first word before either derived
       table lands. */
    port_scene_mg_fill_shared(base, 36);

    /* THE MIDDLE TABLE, in its two mounted spans -- section 3.  Live storage in
       its own right: the factory writes it into the object before the derived
       table, and both destructors write it back on the way down, so a slot
       dispatched between the two vptr writes reads it. */
    port_scene_mg_fill_shared(midA, 20);
    port_scene_mg_fill_shared(midB, 16);
    g_tti_mid_claimed =
        tti_apply(midA, 20, kD3DBaseFaces,
                  sizeof kD3DBaseFaces / sizeof kD3DBaseFaces[0]);
    g_tti_mid_claimed +=
        tti_apply(midB, 16, kD3DBaseFaces,
                  sizeof kD3DBaseFaces / sizeof kD3DBaseFaces[0]);

    /* THE DERIVED TABLE.  Shared arm9 and dScMgBase_c words first, then the
       middle base's seventeen, then this class's own twelve.  Order does not
       matter -- the three key sets are disjoint, because a word is one address
       -- but it reads in inheritance order.  THIRTY-SIX, not thirty-seven:
       section 4, and the word a thirty-seventh would take is the SIBLING
       class's live pause flag. */
    port_scene_mg_fill_shared(vt, 36);
    tti_apply(vt, 36, kD3DBaseFaces,
              sizeof kD3DBaseFaces / sizeof kD3DBaseFaces[0]);
    tti_apply(vt, 36, kTtiFaces, sizeof kTtiFaces / sizeof kTtiFaces[0]);

    /* THE ELEMENT TABLE, section 5.  Five slots, and it is not a Scene table:
       no arm9 or dScMgBase_c word appears in it, so port_scene_mg_fill_shared
       is NOT called over it -- only this class's own five rows can land, and if
       any fails to, the check below reports the raw word count.  Filling it
       here rather than at spawn time is correct because the element constructor
       writes the table's ADDRESS into each of the four records, not its
       contents, so records built later pick the host pointers up on their
       own. */
    g_tti_mario_claimed =
        tti_apply(elem, 5, kTrmplnMarioFaces,
                  sizeof kTrmplnMarioFaces / sizeof kTrmplnMarioFaces[0]);

    /* the two diagnostics, applied after the fill so they override it */
    {
        const char *s0 = std::getenv("SM64DS_SCENE_SLOT0");
        const char *s9 = std::getenv("SM64DS_SCENE_SLOT9");
        if (s0 && s0[0] == '0') vt[0] = (void *)tti_init_noop;
        if (s9 && s9[0] == '0') vt[9] = (void *)tti_render_noop;
    }

    {
        const unsigned lb = tti_raw_left(base, 36);
        const unsigned lm = tti_raw_left(midA, 20) + tti_raw_left(midB, 16);
        const unsigned lv = tti_raw_left(vt, 36);
        const unsigned le = tti_raw_left(elem, 5);
        if (lb || lm || lv || le) {
            std::fprintf(stderr, "  [scene] TRAMPOLINE FILL INCOMPLETE: "
                         "dScMgBase_c leaves %u of 36 raw DS words, "
                         "dScMgD3DBase_c %u of 36, dScMgTrampoline_c %u of 36, "
                         "dMgTrmpln3DMario_c %u of 5. A dispatch of any of them "
                         "jumps to a DS address as a host one.\n",
                         lb, lm, lv, le);
            std::fflush(stderr);
        }
    }

    /* The constructors, gated on the requested id, exactly as every other
       minigame fill does it.  Idempotent behind scene_mg.cpp's own
       once-per-process guard. */
    port_scene_mg_prepare(port_scene_env_want());

    if (port_scene_env_want() == 384) {
        static int armed;
        if (!armed) {
            armed = 1;
            std::atexit(port_scene_trampoline_hits);
        }
    }
}

/* The registry's factory column is void *(*)(void) and so is the matched
   factory; the forwarder is kept for the reason title_spawn and
   port_mg_curling_spawn are, so the seat has one place to observe the object
   without the registry table growing a second column.

   THE FACTORY NEEDS NO DISPLACEMENT RULING, re-checked rather than assumed.
   port/mg_fanout_costs.txt section 12 grants one to 0x169 because
   src/func_ov006_020e0574.cpp calls the base constructor func_ov004_020b2adc
   with NO argument and rides r0 through, and that callee dereferences on its
   first statement and then writes three vtable words through the pointer.
   src/MgTrampolineTime_Spawn.c calls func_ov004_020b2adc(p) WITH its argument,
   so this class is on the correct side of it and the factory is linked from the
   slice rather than host-copied. */
static char *g_tti_self;

extern "C" void *port_mg_trampoline_spawn(void)
{
    void *p = MgTrampolineTime_Spawn();
    g_tti_self = (char *)p;
    return p;
}

extern "C" void port_scene_trampoline_hits(void)
{
    unsigned total = 0, mtotal = 0, etotal = 0;
    for (int i = 0; i < 36; ++i) { total += g_tti_hits[i]; mtotal += g_d3d_hits[i]; }
    for (int i = 0; i < 5; ++i)  etotal += g_mario_hits[i];

    std::printf("[scene] dScMgTrampoline_c slot hits: init %u, cleanup %u, "
                "behavior %u, render %u, D2 %u, D0 %u, state-reset %u, "
                "slot19 %u, slot23 %u, slot24 %u, slot25 %u, slot34 %u%s\n",
                g_tti_hits[0], g_tti_hits[3], g_tti_hits[6], g_tti_hits[9],
                g_tti_hits[16], g_tti_hits[17], g_tti_hits[18], g_tti_hits[19],
                g_tti_hits[23], g_tti_hits[24], g_tti_hits[25], g_tti_hits[34],
                g_tti_render_skipped
                    ? "  [RENDER SLOT NO-OP'd: SM64DS_SCENE_SLOT9=0]" : "");
    if (g_tti_init_skipped)
        std::printf("[scene] INIT SLOT NO-OP'd: SM64DS_SCENE_SLOT0=0, %u "
                    "time(s)\n", g_tti_init_skipped);

    std::printf("[scene] dScMgTrampoline_c own slots entered:");
    for (int i = 0; i < 36; ++i)
        if (g_tti_hits[i]) std::printf(" %d(x%u)", i, g_tti_hits[i]);
    std::printf("   (%u total)\n", total);

    std::printf("[scene] dScMgD3DBase_c slots entered (this seat's copy):");
    for (int i = 0; i < 36; ++i)
        if (g_d3d_hits[i]) std::printf(" %d(x%u)", i, g_d3d_hits[i]);
    std::printf("   (%u total); this seat's middle-table fill claimed %u word(s)"
                " (17 is EXPECTED on a tree with no sibling row, section 3)\n",
                mtotal, g_tti_mid_claimed);

    std::printf("[scene] dMgTrmpln3DMario_c element slots entered:");
    for (int i = 0; i < 5; ++i)
        if (g_mario_hits[i]) std::printf(" %d(x%u)", i, g_mario_hits[i]);
    std::printf("   (%u total); element-table fill claimed %u of 5\n",
                etotal, g_tti_mario_claimed);

    /* THE STATE MACHINE'S OWN WITNESS.  Printed whether or not it fired: a run
       with slot hits and zero routed dispatches has ticked the object without
       entering its state machine, and that reads as a success unless it is said
       out loud.  This class has NO state index field -- its member pointer is
       the state -- so the chain-link histogram below is the whole progression
       witness. */
    {
        unsigned calls = 0, unknown = 0, links[5] = {0,0,0,0,0};
        unsigned unset = 0, virt = 0, last = 0;
        int lo = -1, hi = -1;
        port_mg_dispatch_counts(&calls, &unknown);
        port_mg_tti_link_range(&lo, &hi);
        port_mg_tti_link_hits(links);
        port_mg_tti_anomalies(&unset, &virt, &last);
        std::printf("[scene] dScMgTrampoline_c state dispatch: %u routed to one "
                    "of the class's 5 chain links, %u bodiless want(s); link "
                    "range entered %d..%d; %u framework call(s), %u UNHANDLED "
                    "address(es)\n",
                    port_mg_tti_state_hits(), port_mg_tti_floor_hits(),
                    lo, hi, calls, unknown);
        std::printf("[scene] dScMgTrampoline_c chain links: "
                    "0 intro x%u -> 1 play x%u -> 2 settle x%u -> "
                    "3 result x%u -> 4 idle x%u\n",
                    links[0], links[1], links[2], links[3], links[4]);
        if (unset || virt || last)
            std::printf("[scene] dScMgTrampoline_c dispatch anomalies: %u tick(s)"
                        " with the member pointer still zero, %u with a nonzero "
                        "adjustment (the VIRTUAL arm; every ROM pair reads "
                        "{code,0}), last unrouted code word 0x%08x\n",
                        unset, virt, last);
        std::printf("[scene] dScMgTrampoline_c state floor: NONE. All five "
                    "addresses in the chain reach a matched src TU and none "
                    "carries a NONMATCHING banner\n");
        std::printf("[scene] dScMgTrampoline_c hit test: func_ov006_020d0c38 "
                    "is DECOMPILED AND SEATED as of run mg12 lane TRM. The "
                    "retired trap counter reads %u and can only ever read 0 "
                    "now -- the evidence is the record readout below, not "
                    "this number\n", port_mg_tti_hittest_calls());
        {
            unsigned h = 0, f2 = 0, f1 = 0;
            port_mg_tti_floor_counts(&h, &f2, &f1);
            std::printf("[scene] dScMgTrampoline_c closure floors REMAINING: "
                        "020cf2fc (0x45c) x%u, 020d01e0 (0x800) x%u -- two "
                        "ov006 bodies still with no decompilation, trapped "
                        "count-and-return. 020d0c38 is retired (its slot "
                        "reads %u)\n", f2, f1, h);
        }
        port_mg_trampoline_record_report();
    }

    /* The member pointer itself, at the offset disassembled in
       unmatched/MgTrampolineTime_StateDispatch.cpp section 4.  +0x5004 is the
       code word and +0x5008 the adjustment; slot 6 is the only reader and the
       five setter bodies are the only writers. */
    if (g_tti_self)
        std::printf("[scene] dScMgTrampoline_c object at %p (0x5dc8 bytes), "
                    "member pointer +0x5004 = {0x%08x, %d}, slot-18 arg last "
                    "seen %d, slot-19 arg last seen %d\n",
                    (void *)g_tti_self,
                    *(unsigned *)(g_tti_self + 0x5004),
                    *(int *)(g_tti_self + 0x5008),
                    g_tti_mode18, g_tti_mode19);

    /* THE STYLUS CHAIN. This class's object is 0x5dc8 bytes and its stroke
       block sits at +0x5db0..+0x5db6 rather than 0x181's +0x7b9c..+0x7ba2 --
       src/func_ov006_0212101c.c (this class's slot 23) is what says so, and it
       is the caller that hands the two Vec2s to the hit test. The release latch
       it keys on is +0x5dc4/+0x5dc5, which slot 23 clears on both arms. */
    if (g_tti_self)
        std::printf("[scene] dScMgTrampoline_c stylus chain: +0x5dc4=%u, "
                    "+0x5dc5=%u, stroke block +0x5db0..+0x5db6 = "
                    "(%d,%d)->(%d,%d)\n",
                    (unsigned)*(const unsigned char *)(g_tti_self + 0x5dc4),
                    (unsigned)*(const unsigned char *)(g_tti_self + 0x5dc5),
                    (int)*(const short *)(g_tti_self + 0x5db0),
                    (int)*(const short *)(g_tti_self + 0x5db2),
                    (int)*(const short *)(g_tti_self + 0x5db4),
                    (int)*(const short *)(g_tti_self + 0x5db6));

    /* THE THREE FACTORY ARRAYS, because a run that draws nothing and a run that
       has nothing to draw read the same on every other line -- run mg7 lane
       MEMCARDS' finding, and the reason section 5 exists at all.  The first
       array's records carry a vptr (the element vtable); the other two do not,
       so only the first can be counted this way and the other two are reported
       by their first words instead. */
    if (g_tti_self) {
        unsigned live = 0;
        for (int i = 0; i < 4; ++i)
            if (*(const void *const *)(g_tti_self + 0x500c + i * 0xd0))
                ++live;
        std::printf("[scene] dScMgTrampoline_c element array +0x500c: %u of 4 "
                    "record(s) hold a vptr; +0x534c[3 x 0x32c] first words "
                    "{0x%08x, 0x%08x, 0x%08x}; +0x5cd0[5 x 0x24] first word "
                    "0x%08x\n", live,
                    *(const unsigned *)(g_tti_self + 0x534c + 0 * 0x32c),
                    *(const unsigned *)(g_tti_self + 0x534c + 1 * 0x32c),
                    *(const unsigned *)(g_tti_self + 0x534c + 2 * 0x32c),
                    *(const unsigned *)(g_tti_self + 0x5cd0));
    }

    /* ---- THE ov005 ROW, AND WHAT IT DECIDES ------------------------------
       hal/scene_mg_sound.cpp's port_mg_scene_spawn_param takes the FIRST row
       whose id matches.  0x0180 appears on exactly ONE of the thirty-six rows
       (row 12), so there is no first-match ambiguity for this scene, and its
       +0x04 word is 0x000c0c00 -- save record 12, name-text index 12. */
    {
        const unsigned param = port_mg_scene_spawn_param(384);
        const int idx = port_mg_tti_record_index();
        std::printf("[scene] dScMgTrampoline_c ov005 row 12: param 0x%08x -> "
                    "save record %u, name text %u; live record index %d\n",
                    param, (param >> 8) & 0xff, (param >> 16) & 0xff, idx);
        if (g_tti_self) {
            std::printf("[scene] dScMgTrampoline_c progress: +0xa8 score = %d, "
                        "+0xb4 level = %d, +0x4664 screen = %u, "
                        "+0x5d90 timer = %d\n",
                        *(const int *)(g_tti_self + 0xa8),
                        *(const int *)(g_tti_self + 0xb4),
                        *(const unsigned short *)(g_tti_self + 0x4664),
                        *(const int *)(g_tti_self + 0x5d90));
        }
        if (idx >= 0 && idx < 36) {
            const int *rec = (const int *)(data_0209caf4 + idx * 20);
            std::printf("[scene] dScMgTrampoline_c save record "
                        "data_0209caf4[%d] = { %d, %d, %d, %d, %d }\n", idx,
                        rec[0], rec[1], rec[2], rec[3], rec[4]);
        } else {
            std::printf("[scene] dScMgTrampoline_c save record: NO minigame "
                        "index (data_ov004_020beb68 = %p)\n",
                        data_ov004_020beb68);
        }
    }
    std::fflush(stdout);
}
