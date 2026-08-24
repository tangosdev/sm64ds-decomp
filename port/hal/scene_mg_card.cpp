// dScMgCard_c, the "Picture Poker" minigame -- actor id 0x17b, scene 379.
// Run mg11, lane PKR.  The FOURTH ov006 class the port seats under
// dScMgSingle3DBase_c, after the flower (0x186), Memory Master (0x16b) and
// Lucky Stars (0x184).
//
// Read port/slice_pkr.txt for the identity derivation, the five width checks,
// the attribution ruling and the closure.  This file is the seat: four vtables'
// faces, the fill, the factory forwarder and the run census.
//
// ---- 1. WHY THIS IS A SEPARATE FILE FROM hal/scene_mg.cpp -----------------
//
// The reason hal/scene_mg_flower.cpp, hal/scene_mg_memory2.cpp and
// hal/scene_mg_luckystars.cpp give, unchanged.  scene_mg.cpp is the FAMILY
// half -- the mounts, the thirty-five overlay constructors, dScMgBase_c's
// address-keyed faces and the gate that keeps the constructors off a level boot
// -- and all of it is reached through the one seam that file exports,
// port_scene_mg_fill_shared().  Nothing of it is re-derived here and
// kMgBaseFaces is NOT copied: a second copy would install a different set of
// host thunks over the same DS addresses in the same mounted tables and split
// the per-slot witness in two.
//
// ---- 2. THE HIERARCHY IS THREE DEEP, AND THE RTTI SAYS SO ----------------
//
//     Scene -> dScMgBase_c         data_ov004_020bc0c0  36 slots
//           -> dScMgSingle3DBase_c data_ov006_0213e448  36 slots
//           -> dScMgCard_c         data_ov006_0213bdb4  36 slots
//
// The word immediately before a vtable is its type_info, that record's second
// word is the name and its third is the BASE's type_info.  Read out of
// extracted/overlays/overlay_0006.bin at base 0x020bfec0:
//
//     0x0213bdb0 -> 0x0213bc70                    the type_info
//     0x0213bc70 +4 -> 0x0213bc7c = "11dScMgCard_c"
//                +8 -> 0x0213bc64 +4 -> 0x0213bd00 = "19dScMgSingle3DBase_c"
//                                +8 -> 0x020bbf6c   (ov004, dScMgBase_c's)
//
// The code agrees twice over: src/MgPicturePoker_Spawn.cpp writes 0x0213e448
// into the object and then 0x0213bdb4 over it, and slots 16 and 17
// (src/func_ov006_020d95a4.cpp, src/func_ov006_020d9638.cpp) unwind them in the
// opposite order.  A hierarchy written by the constructor and unwritten by the
// destructor in reverse, with an RTTI base link agreeing, is not a guess.
//
// ---- 3. FOUR TABLES, NOT ONE, AND TWO OF THEM ARE ELEMENT VTABLES --------
//
// This is the first seat in the family whose FACTORY PRODUCTS have their own
// virtuals, which is run mg9 lane S381's element-vtable law arriving with
// teeth.  src/MgPicturePoker_Spawn.cpp builds two five-element arrays through
// the MSL array constructor:
//
//     func_020733a8(p + 0x51a8, 5, 0x30, func_ov006_020dbe30, func_ov006_020d96e0)
//     func_020733a8(p + 0x5298, 5, 0x30, func_ov006_020dbe14, func_ov006_020d96f0)
//
// and each element constructor writes a vtable into the element's word 0:
//
//     +0x51a8[5]  dMgCardObj_c        data_ov006_0213bccc  3 slots  (Mario's)
//     +0x5298[5]  dMgDilarCardObj_c   data_ov006_0213bcf4  3 slots  (Luigi's)
//
// 0x0213bcf4's element ctor writes 0x0213bccc first and 0x0213bcf4 over it, so
// the dealer's card DERIVES from the player's; the RTTI at 0x0213bc58 says the
// same thing ("17dMgDilarCardObj_c" with base 0x0213bc48 = "12dMgCardObj_c").
// The ROM's message 553 says who the dealer is: "Get a better hand than Luigi".
//
// ALL THREE ELEMENT SLOTS ARE DISPATCHED, and this is a `blx` on the loaded
// word rather than an offset that merely matches.  A word-by-word `blx Rm`
// sweep over this class's own 37 bodies (slice section 6) finds NINE indirect
// call sites and every one of them is accounted for:
//
//     slot 0   0x020dab40, 0x020daba4   src/func_ov006_020da9c4.cpp, Render,
//              `ldr r1,[r0] / ldr r1,[r1] / blx r1`, five elements of each array
//     slot 1   0x020db640, 0x020db650   src/func_ov006_020dac34.c's tail,
//              `ldr r1,[r1,#4]`, five elements of each array, EVERY FRAME
//     slot 2   0x020d9854, 0x020d9de0, 0x020da030, 0x020da0e4
//              the two element ticks and the two element seeders
//     +0x48    0x020dbd0c                src/func_ov006_020dbaf0.cpp, the class's
//              OWN vtable slot 18, dispatched with r1 = -1 (`mvn r1,#0`).  The
//              address is the blx itself, which is what the sweep reports; the
//              sequence it ends runs 0x020dbd00..0x020dbd0c.
//
// So a seat that filled the class table alone would leave ten live objects
// jumping to raw DS addresses on the first rendered frame.
//
// ---- 4. THE WIDTH IS 36, CHECKED FIVE WAYS, AND THE 37TH WORD BELONGS TO A
//         CLASS THIS PORT ALREADY SHIPS ---------------------------------
//
//   1. SPAN.  0x0213bdb4 to the next symbol, data_ov006_0213be44, is 0x90 = 36.
//   2. TERMINAL SLOT.  Slot 35 holds 0x020ad660, dScMgBase_c's own
//      `return (this[2] & 0xff) != 0`, which every class in the family ends on.
//   3. WHAT THE WORD PAST THE END IS.  0x0213be44 holds 0x020dc294 and it is
//      not padding: it is the code word of the mwcc member-pointer PAIR
//      {0x020dc294, 0} that src/__sinit_ov006_0213014c.c copies into
//      data_ov006_021417c8[0] -- state slot 0 of dScMgCoin_c, actor id 0x17a,
//      WHICH THIS PORT HAS SHIPPED AS SCENE_MG_COIN SINCE RUN mg5.  A 37-slot
//      fill here would write a host thunk over a live neighbour's state.
//   4. RELOCATION COMPLETENESS, AND IT IS NOT A BOUND ON THE END.  All 36 slots
//      carry a load relocation in ov006's relocs.txt -- none is missing -- so
//      the fill keys on a table the ROM relocates in full rather than on a run
//      with holes.  IT DOES NOT SAY WHERE THE TABLE STOPS, and an earlier
//      version of this line ("36 contiguous load relocations at a four-byte
//      stride, 0x0213bdb4 through 0x0213be40") implied it did.  Measured, the
//      contiguous four-byte run from 0x0213bdb4 is THIRTY-SEVEN long and ends
//      at 0x0213be44, because the first word of the pair that follows is an
//      address too.  Checks 2, 3 and 5 are what bound the end.
//   5. RELOCATION STRIDE.  From 0x0213be44 on the stride is EIGHT
//      (be44, be4c, be54, ...).  A vtable is relocated every word; an mwcc
//      member-pointer table every other word.  The stride change is where the
//      table ends and it needs no symbol to say so.
//
// The fill below is called with 36.
//
// ---- 5. THE ROW GOES LAST, AND THE ORDER IS LOAD-BEARING ------------------
//
// port_scene_registry_install walks port_scene_classes[] in table order and
// calls every row's fill on every boot; port_scene_mg_overlay_load runs the
// thirty-five constructors ONCE PER PROCESS from the tail of the first minigame
// row's fill.  Appending after every existing row therefore means the
// constructors have already run with clean ROM words when this fill starts,
// which is the latent-safe direction port/mg_fanout_costs.txt section 11 names
// -- and section 3 above depends on it for a second reason, because the flower
// row claims the shared dScMgSingle3DBase_c table first and this seat's own
// middle-table pass must find nothing left to claim.
//
// ---- 6. THERE IS NO POINTER-TO-MEMBER WALL FOR THIS CLASS -----------------
//
// This class has no state table and no member pointer of its own.  The state
// machine is an ORDINARY 14-ARM ARM JUMP TABLE in src/func_ov006_020dac34.c:
//
//     020dac44  ldrsh r0,[r1,#0x88]        the state, at +0x5388
//     020dac48  cmp   r0,#0xd
//     020dac4c  addls pc,pc,r0,lsl #2
//
// which is the shape run mg9 lane S364 found for Mario Slot.  The claim is not
// made from a source sweep: the ARM-Itanium encoding scan (slice section 6)
// returns ZERO across this class's 37 bodies, and THE SAME SCAN over the
// neighbouring dScMgCoin_c block returns that class's seven known dispatch
// sites, so the instrument is not reading blind.
//
// The one member pointer anywhere near this class is the +0x4f38 sub-object's
// FIELD-held one, and it is already host-copied family-wide in
// port/unmatched/MgMemory2_FieldPmf.cpp.  Nothing here re-copies it, and
// nothing here rewrites the stored pair -- src/func_ov006_020c0f0c.c, which
// this class's state 1 calls, asks "is this slot idle" BY VALUE against the
// sentinels data_ov006_0213acb0 and _0213aca8, so a seat that wrote host
// addresses over the pair would leave that test permanently false.
//
// ---- 7. WHAT THIS SEAT DOES NOT CLAIM -------------------------------------
//
// NO FLOOR ANY MORE, AND THAT IS THE ONLY THING THAT CHANGED HERE.  Run mg11
// seated this class with one, func_ov006_020da174 (0x2ac at 0x020da174, the
// hand sorter), behind port/unmatched/MgCard_Traps.cpp, and it sat on the
// critical path: state 9 calls it on both hands, it is the only caller of
// src/func_ov006_020d99ec.c, which is what puts every card into state 7 with a
// slide target, and state 10 will not advance until all ten cards reach state
// 8.  So the port reached state 10 and stayed there.  Run mg12 lane SRT
// decompiled the body -- src/func_ov006_020da174.c, byte-identical at mwccarm
// 2004/b56, with the delink block for 0x020da174..0x020da420 enrolled -- and
// the trap is deleted.  port/slice_pkr.txt section 7 is the record.
//
// THE WITNESS MOVED WITH IT.  A count of trap entries is meaningless once there
// is no trap, so this file does not print one; what stands in its place is the
// evidence that only works if the sorter really ran: the state index at +0x5388
// reaching the payout arms 0x0e..0x12, and the ten per-card `state` bytes below
// leaving state 7 for state 8.  Both were already printed and neither could
// move while the trap stood.
//
// The state machine is proven to run by the numbers this file prints, not by
// this comment.  A run that reports slot hits and a state index that never
// moves has ticked an object without entering its machine, and that reads as a
// success unless it is said out loud.

#include "hal/screen_gap.h"

#include <cstdio>
#include <cstdlib>

extern "C" {

/* the seam into hal/scene_mg.cpp -- mounts, arm9 fill, dScMgBase_c's faces */
unsigned port_scene_mg_fill_shared(void **vt, unsigned n);
void     port_scene_mg_prepare(int id);
int      port_scene_env_want(void);

/* the mount storage the fill writes into.  All five are the mounts' own, so the
   fill replaces the mounted words rather than building a fresh host array --
   the ov007, curling, memory2 and luckystars treatment, for the same reason: a
   second host array of the same name is a duplicate symbol, and leaving the
   mounted table alone leaves live wild DS pointers in a table the factory (and,
   for the two element tables, the array constructor) installs. */
extern unsigned char data_ov004_020bc0c0[];   /* dScMgBase_c,          36 */
extern unsigned char data_ov006_0213e448[];   /* dScMgSingle3DBase_c,  36 */
extern unsigned char data_ov006_0213bdb4[];   /* dScMgCard_c,          36 */
extern unsigned char data_ov006_0213bccc[];   /* dMgCardObj_c,          3 */
extern unsigned char data_ov006_0213bcf4[];   /* dMgDilarCardObj_c,     3 */
extern unsigned char MgPicturePoker_SpawnInfo[];

/* dScMgSingle3DBase_c's eight overrides.  Slot 2 is NOT src's body: see
   hal/scene_mg_luckystars.cpp section 4 and port/unmatched/MgFlower_Slot2.cpp,
   re-checked for this class rather than copied -- func_ov006_0210a6e4 drops the
   framework's second argument at its `bl 0x20b08f0` and src spells the call
   with one argument because that is the only way to spell an unnamed value. */
int   port_mg_flower_after_init(void *c, unsigned f);   /* slot  2 */
void  func_ov006_0210a608(void *c, unsigned f);         /* slot  5 */
int   func_ov006_0210a698(void *c);                     /* slot  7 */
int   func_ov006_0210a664(void *c);                     /* slot 10 */
int   func_ov006_0210a4b0(char *c);                     /* slot 16 D2 */
int   func_ov006_0210a4e8(char *c);                     /* slot 17 D0 */
int   func_ov006_0210a600(void);                        /* slot 26 */
void  func_ov006_0210a708(char *c);                     /* slot 33 */

/* dScMgCard_c's own NINE.  Slot 3 is the one dScMgBSC_c does not have; run mg9
   lane WIG recorded CleanupResources as new to the family and this is the
   second class to carry it. */
int   func_ov006_020dbaf0(char *c);           /* slot  0 InitResources    */
int   func_ov006_020da994(void);              /* slot  3 CleanupResources */
int   func_ov006_020dabec(char *c);           /* slot  6 Behavior         */
int   func_ov006_020da9c4(char *c);           /* slot  9 Render           */
void *func_ov006_020d95a4(char *c);           /* slot 16 D2               */
void *func_ov006_020d9638(char *c);           /* slot 17 D0               */
void  func_ov006_020db9dc(char *c);           /* slot 18 state reset      */
int   func_ov006_020db720(char *c);           /* slot 19                  */
int   func_ov006_020db6ec(void *c);           /* slot 21 difficulty band  */

/* the two element classes' three each */
void  func_ov006_020d9bdc(char *c);           /* dMgCardObj_c      slot 0 */
void  func_ov006_020d9c5c(void *c);           /* dMgCardObj_c      slot 1 */
int   func_ov006_020d9bd0(void);              /* dMgCardObj_c      slot 2 */
void  func_ov006_020d970c(unsigned char *c);  /* dMgDilarCardObj_c slot 0 */
void  func_ov006_020d978c(void *c);           /* dMgDilarCardObj_c slot 1 */
int   func_ov006_020d9998(void);              /* dMgDilarCardObj_c slot 2 */

/* the factory */
void *MgPicturePoker_Spawn(void);

/* the framework's, from unmatched/MgBase_StateDispatch.cpp */
void     port_mg_dispatch_counts(unsigned *calls, unsigned *unknown);
/* the +0x4f38 sub-object's FIELD-held member pointer, from
   unmatched/MgMemory2_FieldPmf.cpp.  This class's state machine WAITS ON that
   machine at states 2, 5 and 7 -- every one of them is
   `if (func_ov006_020c1718(this+0x4f38))` -- so a census that prints this
   class's own state index and not this counter cannot tell a machine that is
   waiting correctly from one that is waiting on a dispatcher that never ran. */
void     port_mg_memory2_field_counts(unsigned *calls, unsigned *hits);
/* the idle sentinel that gate compares against, and the shared-file pair the
   sub-object parks on.  Read, never written. */
extern unsigned char data_ov006_0213ac58[];

/* the persistent minigame record this class's level is READ FROM and WRITTEN
   BACK TO.  Nothing new is defined here: the storage is hal/level_boot.cpp's
   .dsstate$savblk0004 and the index function is src/func_ov004_020adc3c.c. */
extern void         *data_ov004_020beb68;     /* the live dScMgBase_c `this` */
extern unsigned char data_0209caf4[];         /* 36 records x 20 bytes       */
int  func_ov004_020adc3c(void *c);            /* (self->field_8 >> 8) & 0xff */

}  /* extern "C" */

/* The record row this class's object is keyed to, or -1 with no live base.
   The ov005 launch table's row for 0x017b is row 9, param 0x00050900, so the
   expected answer on a direct scene boot is 9 -- printed rather than asserted. */
static int port_mg_card_record_index(void)
{
    return data_ov004_020beb68 ? func_ov004_020adc3c(data_ov004_020beb68) : -1;
}

// ---- the tick witness ------------------------------------------------------
//
// One counter per dispatched slot on each of the four tables this file owns
// faces for.  The instrument every scene seat in this port carries, for the
// reason hal/scene_mg.cpp gives: an object that EXISTS and an object that RUNS
// look identical from outside.  The framework slots dScMgBase_c keeps are
// counted by scene_mg.cpp's own array.
static unsigned g_card_hits[36];
static unsigned g_card_base_hits[36];   /* the same slots on the MIDDLE table */
static unsigned g_card_elem_hits[3];    /* dMgCardObj_c                       */
static unsigned g_card_dilar_hits[3];   /* dMgDilarCardObj_c                  */

#define CARD(n)  (++g_card_hits[(n)])
#define B3D(n)   (++g_card_base_hits[(n)])
#define ELEM(n)  (++g_card_elem_hits[(n)])
#define DILR(n)  (++g_card_dilar_hits[(n)])

/* The two mode values slots 18 and 19 were last handed.  Run mg9 lane LKY's
   rule applies and this lane obeys the half of it that says WITNESS, DO NOT
   ASSUME: both ROM bodies here IGNORE the value -- 0x020db9dc overwrites r1 at
   its second literal load before reading it, and 0x020db720's first act is a
   five-arm jump table on the state at +0x5388 -- so the parameter exists here
   only so __fastcall cleans the four bytes the ROM's callers push.  It is
   recorded anyway, because "the body ignores it" is a claim a census can check
   and a comment cannot. */
static int g_card_mode18 = -1, g_card_mode19 = -1;

/* ---- dScMgSingle3DBase_c's eight ---------------------------------------- */
static void *__fastcall s3_ainit(void *s, void *, unsigned f)
{ B3D(2);  return (void *)(size_t)port_mg_flower_after_init(s, f); }
static void __fastcall s3_aclean(void *s, void *, unsigned f)
{ B3D(5);  func_ov006_0210a608(s, f); }
static int  __fastcall s3_bbeh(void *s, void *)
{ B3D(7);  return func_ov006_0210a698(s); }
static int  __fastcall s3_bren(void *s, void *)
{ B3D(10); return func_ov006_0210a664(s); }
static void *__fastcall s3_d2(void *s, void *)
{ B3D(16); return (void *)(size_t)func_ov006_0210a4b0((char *)s); }
static void *__fastcall s3_d0(void *s, void *)
{ B3D(17); return (void *)(size_t)func_ov006_0210a4e8((char *)s); }
static int  __fastcall s3_v26(void *, void *)
{ B3D(26); return func_ov006_0210a600(); }
static int  __fastcall s3_v33(void *s, void *)
{ B3D(33); func_ov006_0210a708((char *)s); return 0; }

/* ---- dScMgCard_c's own nine --------------------------------------------- */
static int  __fastcall card_init(void *s, void *)
{ CARD(0); const int r = func_ov006_020dbaf0((char *)s);
  /* the GaplessMinigames latch, for hal/scene_mg.cpp's reason: every seated
     minigame calls it so the ones the gapless table does not name can say
     "unsupported" instead of doing nothing quietly.  hal_gapless_splice() is
     deliberately NOT called -- scene 379 has no row in screen_gap.cpp's table
     and gapless behaviour is decided per minigame BY PLAYING IT.  This lane has
     not played it and does not get to vote.  Same ruling as
     hal/scene_mg_bomroom.cpp section 8, hal/scene_mg_flower.cpp and
     hal/scene_mg_luckystars.cpp. */
  hal_gapless_minigames_latch(); return r; }
static int  __fastcall card_clean(void *, void *)
{ CARD(3); return func_ov006_020da994(); }
static int  __fastcall card_beh(void *s, void *)
{ CARD(6); return func_ov006_020dabec((char *)s); }
static int  __fastcall card_render(void *s, void *)
{ CARD(9); return func_ov006_020da9c4((char *)s); }
static void *__fastcall card_d2(void *s, void *)
{ CARD(16); return func_ov006_020d95a4((char *)s); }
static void *__fastcall card_d0(void *s, void *)
{ CARD(17); return func_ov006_020d9638((char *)s); }
/* Slot 18's ROM body sets no return value at its single exit (0x020dbac0:
   `add sp,sp,#4 / pop / bx lr`, r0 untouched) and its ROM caller at 0x020dbd0c
   ignores whatever comes back, so the 1 here is the family convention rather
   than a measurement.  Slot 19's DOES return -- 1 on the ApproachLinear2 arm
   and 0 at its other exit -- so that one is forwarded. */
static int  __fastcall card_reset(void *s, void *, int mode)
{ CARD(18); g_card_mode18 = mode; func_ov006_020db9dc((char *)s); return 1; }
static int  __fastcall card_v19(void *s, void *, int mode)
{ CARD(19); g_card_mode19 = mode; return func_ov006_020db720((char *)s); }
static int  __fastcall card_v21(void *s, void *)
{ CARD(21); return func_ov006_020db6ec(s); }

/* ---- the two element classes' three each --------------------------------
   These are dispatched off the ELEMENT's own word 0, not off the scene's, and
   the receiver is the element.  Every one of the six is __fastcall for the
   reason every other face in this port is: MSVC reaches a C++ virtual with
   `this` in ecx, and the src bodies that dispatch them
   (src/func_ov006_020da9c4.cpp, _020dac34.c, _020da00c.cpp, _020da0ac.cpp,
   _020d978c.cpp, _020d9c5c.cpp) all spell the object as a real C++ class with
   three virtuals, so MSVC emits the ecx call and reads the word this fill
   replaces. */
static void __fastcall elem_draw(void *s, void *)
{ ELEM(0); func_ov006_020d9bdc((char *)s); }
static void __fastcall elem_tick(void *s, void *)
{ ELEM(1); func_ov006_020d9c5c(s); }
static int  __fastcall elem_home(void *, void *)
{ ELEM(2); return func_ov006_020d9bd0(); }
static void __fastcall dilr_draw(void *s, void *)
{ DILR(0); func_ov006_020d970c((unsigned char *)s); }
static void __fastcall dilr_tick(void *s, void *)
{ DILR(1); func_ov006_020d978c(s); }
static int  __fastcall dilr_home(void *, void *)
{ DILR(2); return func_ov006_020d9998(); }

/* SM64DS_SCENE_SLOT0=0 and SM64DS_SCENE_SLOT9=0, the diagnostics every scene
   seat in this port carries, counted separately so a run can never read a no-op
   as the real body having run. */
static unsigned g_card_init_skipped, g_card_render_skipped;
static int __fastcall card_init_noop(void *, void *)
{ ++g_card_init_skipped; return 1; }
static int __fastcall card_render_noop(void *, void *)
{ ++g_card_render_skipped; return 1; }

struct CardFace { unsigned ds; void *host; };

/* Keyed on the ROM WORD each slot holds, so every array is order-independent
   and cannot land on a slot the ROM did not park that body in. */
static const CardFace kSingle3DFaces[] = {
    {0x0210a6e4u, (void *)s3_ainit},  {0x0210a608u, (void *)s3_aclean},
    {0x0210a698u, (void *)s3_bbeh},   {0x0210a664u, (void *)s3_bren},
    {0x0210a4b0u, (void *)s3_d2},     {0x0210a4e8u, (void *)s3_d0},
    {0x0210a600u, (void *)s3_v26},    {0x0210a708u, (void *)s3_v33},
};

static const CardFace kCardFaces[] = {
    {0x020dbaf0u, (void *)card_init},   {0x020da994u, (void *)card_clean},
    {0x020dabecu, (void *)card_beh},    {0x020da9c4u, (void *)card_render},
    {0x020d95a4u, (void *)card_d2},     {0x020d9638u, (void *)card_d0},
    {0x020db9dcu, (void *)card_reset},  {0x020db720u, (void *)card_v19},
    {0x020db6ecu, (void *)card_v21},
};

static const CardFace kElemFaces[] = {
    {0x020d9bdcu, (void *)elem_draw},   {0x020d9c5cu, (void *)elem_tick},
    {0x020d9bd0u, (void *)elem_home},
};

static const CardFace kDilarFaces[] = {
    {0x020d970cu, (void *)dilr_draw},   {0x020d978cu, (void *)dilr_tick},
    {0x020d9998u, (void *)dilr_home},
};

static unsigned card_apply(void **vt, unsigned n, const CardFace *f, unsigned nf)
{
    unsigned hit = 0;
    for (unsigned i = 0; i < n; ++i) {
        const unsigned ds = (unsigned)(size_t)vt[i];
        for (unsigned k = 0; k < nf; ++k)
            if (f[k].ds == ds) { vt[i] = f[k].host; ++hit; break; }
    }
    return hit;
}

/* Words still holding a DS address.  Every slot of a minigame table is
   dispatched by something, so a nonzero answer is a wild call waiting to happen
   and the seat says so out loud.  The two element tables are three slots each
   and all three are dispatched (section 3), so the same rule applies to them. */
static unsigned card_raw_left(void **vt, unsigned n)
{
    unsigned left = 0;
    for (unsigned i = 0; i < n; ++i) {
        const unsigned w = (unsigned)(size_t)vt[i];
        if (w >= 0x02000000u && w < 0x02400000u)
            ++left;
    }
    return left;
}

extern "C" void port_scene_card_hits(void);

/* How many words this seat's own copy of the middle-table array claimed.  Zero
   is the EXPECTED reading on a tree that also carries the flower row, and
   section 5 is why; it is printed rather than assumed. */
static unsigned g_card_mid_claimed;

extern "C" void port_scene_fill_card(void)
{
    void **base = (void **)data_ov004_020bc0c0;
    void **mid  = (void **)data_ov006_0213e448;
    void **vt   = (void **)data_ov006_0213bdb4;
    void **el   = (void **)data_ov006_0213bccc;
    void **dl   = (void **)data_ov006_0213bcf4;

    /* THE BASE TABLE IS FILLED HERE TOO AND IT IS NOT CEREMONY.  Earlier rows'
       fills already did it and run first, so on a tree carrying them this is a
       second pass over words that are already host pointers and finds nothing.
       It is here so this class does not depend on another class's row existing:
       the factory's first act is func_ov004_020b2adc, which writes
       data_ov004_020bc0c0 into the object's first word before either derived
       table lands. */
    port_scene_mg_fill_shared(base, 36);

    /* THE MIDDLE TABLE.  Live storage in its own right -- the destructor chain
       writes it into the object on the way down, so a slot dispatched between
       the two vptr writes reads it. */
    port_scene_mg_fill_shared(mid, 36);
    g_card_mid_claimed =
        card_apply(mid, 36, kSingle3DFaces,
                   sizeof kSingle3DFaces / sizeof kSingle3DFaces[0]);
    g_card_mid_claimed +=
        card_apply(mid, 36, kCardFaces,
                   sizeof kCardFaces / sizeof kCardFaces[0]);

    /* THE DERIVED TABLE.  Shared arm9 and dScMgBase_c words first, then the
       intermediate base's eight, then this class's own nine.  Order does not
       matter -- the three key sets are disjoint, because a word is one address
       -- but it reads in inheritance order.  THIRTY-SIX, not thirty-seven:
       section 4, and the word a thirty-seventh would take is dScMgCoin_c's
       live state pair. */
    port_scene_mg_fill_shared(vt, 36);
    card_apply(vt, 36, kSingle3DFaces,
               sizeof kSingle3DFaces / sizeof kSingle3DFaces[0]);
    card_apply(vt, 36, kCardFaces,
               sizeof kCardFaces / sizeof kCardFaces[0]);

    /* THE TWO ELEMENT TABLES.  Three slots each, no shared framework words in
       them at all, so port_scene_mg_fill_shared is deliberately NOT called on
       either: there is nothing of dScMgBase_c's in a card object. */
    card_apply(el, 3, kElemFaces, sizeof kElemFaces / sizeof kElemFaces[0]);
    card_apply(dl, 3, kDilarFaces, sizeof kDilarFaces / sizeof kDilarFaces[0]);

    /* the two diagnostics, applied after the fill so they override it */
    {
        const char *s0 = std::getenv("SM64DS_SCENE_SLOT0");
        const char *s9 = std::getenv("SM64DS_SCENE_SLOT9");
        if (s0 && s0[0] == '0') vt[0] = (void *)card_init_noop;
        if (s9 && s9[0] == '0') vt[9] = (void *)card_render_noop;
    }

    {
        const unsigned lb = card_raw_left(base, 36);
        const unsigned lm = card_raw_left(mid, 36);
        const unsigned lv = card_raw_left(vt, 36);
        const unsigned le = card_raw_left(el, 3);
        const unsigned ld = card_raw_left(dl, 3);
        if (lb || lm || lv || le || ld) {
            std::fprintf(stderr, "  [scene] CARD FILL INCOMPLETE: "
                         "dScMgBase_c leaves %u of 36 raw DS words, "
                         "dScMgSingle3DBase_c %u, dScMgCard_c %u, "
                         "dMgCardObj_c %u of 3, dMgDilarCardObj_c %u of 3. "
                         "A dispatch of any of them jumps to a DS address as a "
                         "host one.\n", lb, lm, lv, le, ld);
            std::fflush(stderr);
        }
    }

    /* The constructors, gated on the requested id, exactly as every other
       minigame fill does it.  Idempotent behind scene_mg.cpp's own
       once-per-process guard. */
    port_scene_mg_prepare(port_scene_env_want());

    if (port_scene_env_want() == 379) {
        static int armed;
        if (!armed) {
            armed = 1;
            std::atexit(port_scene_card_hits);
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
   first statement.  src/MgPicturePoker_Spawn.cpp calls func_ov004_020b2adc(p)
   WITH its argument, so this class is on the correct side of it and the factory
   is linked from the slice rather than host-copied. */
static char *g_card_self;

extern "C" void *port_mg_card_spawn(void)
{
    void *p = MgPicturePoker_Spawn();
    g_card_self = (char *)p;
    return p;
}

extern "C" void port_scene_card_hits(void)
{
    unsigned total = 0, mtotal = 0;
    for (int i = 0; i < 36; ++i) {
        total += g_card_hits[i];
        mtotal += g_card_base_hits[i];
    }

    std::printf("[scene] dScMgCard_c slot hits: init %u, cleanup %u, "
                "behavior %u, render %u, D2 %u, D0 %u, state-reset %u, "
                "slot19 %u, slot21 %u%s\n",
                g_card_hits[0], g_card_hits[3], g_card_hits[6], g_card_hits[9],
                g_card_hits[16], g_card_hits[17], g_card_hits[18],
                g_card_hits[19], g_card_hits[21],
                g_card_render_skipped
                    ? "  [RENDER SLOT NO-OP'd: SM64DS_SCENE_SLOT9=0]" : "");
    if (g_card_init_skipped)
        std::printf("[scene] INIT SLOT NO-OP'd: SM64DS_SCENE_SLOT0=0, %u "
                    "time(s)\n", g_card_init_skipped);

    std::printf("[scene] dScMgCard_c own slots entered:");
    for (int i = 0; i < 36; ++i)
        if (g_card_hits[i]) std::printf(" %d(x%u)", i, g_card_hits[i]);
    std::printf("   (%u total)\n", total);

    std::printf("[scene] dScMgSingle3DBase_c slots entered (this seat's copy):");
    for (int i = 0; i < 36; ++i)
        if (g_card_base_hits[i]) std::printf(" %d(x%u)", i, g_card_base_hits[i]);
    std::printf("   (%u total); this seat's middle-table fill claimed %u word(s)"
                " (0 is EXPECTED: the flower row runs first, section 5)\n",
                mtotal, g_card_mid_claimed);

    /* THE ELEMENT VTABLES' OWN WITNESS.  Ten objects, six virtuals, and the
       whole reason run mg9 lane S381's law exists: none of these dispatches is
       visible in any scene-table derivation.  Printed whether they fired or
       not -- zero on slot 0 with a nonzero render count would mean the cards
       are not being drawn at all. */
    std::printf("[scene] dMgCardObj_c (5 at +0x51a8) element vtable "
                "0x0213bccc: draw %u, tick %u, home %u\n",
                g_card_elem_hits[0], g_card_elem_hits[1], g_card_elem_hits[2]);
    std::printf("[scene] dMgDilarCardObj_c (5 at +0x5298) element vtable "
                "0x0213bcf4: draw %u, tick %u, home %u\n",
                g_card_dilar_hits[0], g_card_dilar_hits[1],
                g_card_dilar_hits[2]);

    /* THE STATE MACHINE'S OWN WITNESS.  This class dispatches its states
       through an ARM jump table rather than a member-pointer table (section 6),
       so there is no address switch to count: the honest instrument is the
       state INDEX the ROM's own dispatcher reads, plus the framework's counters
       for the half of the machine that is shared. */
    {
        unsigned calls = 0, unknown = 0;
        port_mg_dispatch_counts(&calls, &unknown);
        std::printf("[scene] dScMgCard_c state machine: no member-pointer table"
                    " (14-arm ARM jump table at 0x020dac4c); %u framework "
                    "call(s), %u UNHANDLED address(es)\n", calls, unknown);
        std::printf("[scene] dScMgCard_c hand sorter func_ov006_020da174: "
                    "SEATED (src/func_ov006_020da174.c, run mg12 lane SRT; run "
                    "mg11 had to trap it). It is the ONLY caller of "
                    "src/func_ov006_020d99ec.c, which is what puts all ten "
                    "cards into state 7 with a slide target, and state 10 waits"
                    " for every card to reach state 8 -- so the card `state` "
                    "bytes below and a state index past 0x0a are what say it "
                    "ran; there is no trap counter left to print\n");
    }

    /* THE SUB-OBJECT'S MACHINE, because THREE of this class's fourteen states
       do nothing but wait on it.  States 2, 5 and 7 all gate on
       func_ov006_020c1718(this+0x4f38), which asks whether the +0x4f38 pair
       equals the idle sentinel data_ov006_0213ac58 BY VALUE -- which is also
       the second reader that makes rewriting the stored pair illegal.  The
       calls counter is unmatched/MgMemory2_FieldPmf.cpp's; anything it cannot
       route falls through to the framework and is reported as UNHANDLED there,
       so calls > hits with zero UNHANDLED means the framework took them. */
    {
        unsigned fcalls = 0, fhits = 0;
        port_mg_memory2_field_counts(&fcalls, &fhits);
        std::printf("[scene] dScMgCard_c +0x4f38 sub-object field dispatch: "
                    "%u call(s), %u routed here", fcalls, fhits);
        if (g_card_self) {
            const unsigned *p = (const unsigned *)(g_card_self + 0x4f38);
            const unsigned *g = (const unsigned *)data_ov006_0213ac58;
            std::printf("; stored pair {%08x, %08x} vs idle sentinel "
                        "{%08x, %08x} -> gate %s", p[0], p[1], g[0], g[1],
                        (p[0] == g[0] && (p[1] == g[1] || p[0] == 0))
                            ? "OPEN (states 2/5/7 may advance)"
                            : "SHUT (states 2/5/7 wait)");
        }
        std::printf("\n");
    }

    /* The state index the ROM's own dispatcher reads, at the offset
       disassembled above: +0x5388 is the jump table's index and +0x538a is the
       per-state countdown five of the fourteen arms run on. */
    if (g_card_self)
        std::printf("[scene] dScMgCard_c object at %p, state index +0x5388 = "
                    "%d, countdown +0x538a = %d, frame counter +0x5396 = %d, "
                    "slot-18 mode last seen %d, slot-19 mode last seen %d\n",
                    (void *)g_card_self,
                    *(short *)(g_card_self + 0x5388),
                    *(short *)(g_card_self + 0x538a),
                    *(short *)(g_card_self + 0x5396),
                    g_card_mode18, g_card_mode19);

    /* THE TEN CARDS, because a run that draws nothing and a run that has
       nothing to draw read the same on every other line -- run mg7 lane
       MEMCARDS' finding, and the reason it is worth the twenty lines.  Each
       0x30-byte record carries, at the offsets src/func_ov006_020d9bdc.c and
       src/func_ov006_020d9c5c.cpp read:
         +0x04  the 20.12 x, shifted down 12 and offset 0x18 for the draw
         +0x08  the 20.12 y
         +0x24  the flip progress; >> 12 indexes the sprite row
         +0x2a  the FACE, 0..5; (face + 1) * 5 + (progress >> 12) is the row
         +0x2b  the card's own state, 0 = not in play
       A card at state 0 draws nothing at all, which is the first thing to look
       at when the screen is empty. */
    if (g_card_self) {
        static const struct { unsigned off; const char *who; } kRows[2] = {
            { 0x51a8u, "dMgCardObj_c     " },
            { 0x5298u, "dMgDilarCardObj_c" },
        };
        for (int r = 0; r < 2; ++r) {
            std::printf("[scene] %s hand:", kRows[r].who);
            for (int i = 0; i < 5; ++i) {
                const char *e = g_card_self + kRows[r].off + i * 0x30;
                const int x = *(const int *)(e + 4) >> 12;
                const int y = *(const int *)(e + 8) >> 12;
                const unsigned face = *(const unsigned char *)(e + 0x2a);
                const unsigned st = *(const unsigned char *)(e + 0x2b);
                const int prog = *(const int *)(e + 0x24) >> 12;
                std::printf(" [%d] @%d,%d face %u prog %d state %u%s", i,
                            x, y, face, prog, st,
                            st == 0 ? " NOT-IN-PLAY" : "");
            }
            std::printf("\n");
        }
    }

    /* ---- THE LEVEL, AND WHERE IT COMES FROM -----------------------------
       Same mechanism run mg8 lane MMD disassembled for Memory Master and run
       mg9 lane LKY found again for Lucky Stars, found once more here rather
       than assumed:

         slot 0   (src/func_ov006_020dbaf0.cpp) seeds +0xa8 and +0xac from
                  func_ov004_020ad8b8(), writes func_ov004_020ad878()'s answer
                  into the LIVE BASE's +0xb4 (not its own), and then dispatches
                  its own vtable slot 18 with -1 through the object's vptr.
         slot 21  (src/func_ov006_020db6ec.c) READS +0xb4: < 5 -> band 1,
                  < 10 -> 2, < 15 -> 3, else 4, handed to func_ov004_020b6324.
                  Four bands, the same shape Lucky Stars has with a different
                  arithmetic (this one is thresholds, that one is /5 capped).
         the machine (src/func_ov006_020dac34.c) WRITES IT BACK: state 13's win
                  arm increments +0xb4 up to 0x270f and raises +0xb8 to match,
                  its lose arm decrements +0xb4 while it is > 0, and state 4
                  calls func_ov004_020ad79c(+0xa8, +0xb4) -- score and level,
                  the same pair Memory Master and Lucky Stars write.

       A fresh record is ZERO (SaveData::SetDefaultValuesMg memsets it), so the
       port starts every launch at level 0, band 1.  That is the save-medium gap
       port/mg_fanout_costs.txt section 17 records and not a minigame one. */
    if (g_card_self) {
        const int idx = port_mg_card_record_index();
        const int lvl = *(const int *)(g_card_self + 0xb4);
        int band;
        if (lvl < 5) band = 1; else if (lvl < 10) band = 2;
        else if (lvl < 15) band = 3; else band = 4;
        std::printf("[scene] dScMgCard_c level: +0xb4 = %d, +0xa8 score = %d, "
                    "+0xac = %d, +0xb8 best = %d  ->  slot 21 band = %d "
                    "(of 1..4)\n", lvl,
                    *(const int *)(g_card_self + 0xa8),
                    *(const int *)(g_card_self + 0xac),
                    *(const int *)(g_card_self + 0xb8), band);
        if (idx >= 0 && idx < 36) {
            const int *rec = (const int *)(data_0209caf4 + idx * 20);
            std::printf("[scene] dScMgCard_c save record data_0209caf4[%d] "
                        "= { %d, %d, %d, %d, %d }  (ov005 row 9 param "
                        "0x00050900 says record 9, name text 5 = message 553 "
                        "\"Picture Poker\")\n", idx,
                        rec[0], rec[1], rec[2], rec[3], rec[4]);
        } else {
            std::printf("[scene] dScMgCard_c save record: NO minigame index "
                        "(data_ov004_020beb68 = %p)\n", data_ov004_020beb68);
        }
    }
    std::fflush(stdout);
}
