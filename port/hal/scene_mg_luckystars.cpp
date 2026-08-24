// dScMgBSC_c, the "Lucky Stars" minigame -- actor id 0x184, scene 388.
// Run mg9, lane LKY.  The twelfth ov006 minigame class the port seats and the
// THIRD under dScMgSingle3DBase_c.
//
// Read port/slice_lky.txt for the identity derivation, the four width checks
// and the closure.  Read port/unmatched/MgBSC_StateDispatch.cpp for the wall.
// This file is the seat: the two new vtables' faces, the fill, the factory
// forwarder and the run census.
//
// ---- 1. WHY THIS IS A SEPARATE FILE FROM hal/scene_mg.cpp -----------------
//
// The reason hal/scene_mg_flower.cpp and hal/scene_mg_memory2.cpp give,
// unchanged.  scene_mg.cpp is the FAMILY half -- the mounts, the thirty-five
// overlay constructors, dScMgBase_c's address-keyed faces and the gate that
// keeps the constructors off a level boot -- and all of it is reached through
// the one seam that file exports, port_scene_mg_fill_shared().  Nothing of it
// is re-derived here and kMgBaseFaces is NOT copied: a second copy would
// install a different set of host thunks over the same DS addresses in the same
// mounted tables and split the per-slot witness in two.
//
// ---- 2. THE HIERARCHY IS THREE DEEP, AND THE RTTI SAYS SO ----------------
//
//     Scene -> dScMgBase_c         data_ov004_020bc0c0  36 slots
//           -> dScMgSingle3DBase_c data_ov006_0213e448  36 slots
//           -> dScMgBSC_c          data_ov006_0213fec8  36 slots
//
// port/mg_fanout_costs.txt section 3 lists 0x184 as "vtable 0x0213fec8, 36
// slots, 14 overrides" and says nothing about an intermediate base -- the same
// gap section 13's CORRECTION 1 records for 0x16b, and it predicted this:
// "0x169 and 0x186 sit under this same middle table, so the fan-out should
// expect it to fire again".  It fires here too, and a seat that filled one
// derived table would have left SIX slots (2, 5, 7, 10, 26 and 33) holding raw
// DS words.
//
// THIS LANE READ THE ROM'S OWN type_info RATHER THAN INFERRING FROM NAMES,
// which is the chain hal/scene_mg_panel's lane established and the strongest
// witness available.  The word immediately before a vtable is its type_info,
// that record's second word is the name and its third is the BASE's type_info:
//
//     0x0213fec4 -> 0x0213fd8c -> +4 -> 0x0213fd98 = "10dScMgBSC_c"
//                                -> +8 -> 0x0213bc64 -> "19dScMgSingle3DBase_c"
//
// The code agrees twice over: src/MgLuckyStars_Spawn.cpp writes 0x0213e448 into
// the object and then 0x0213fec8 over it, and slots 16 and 17
// (src/func_ov006_02124908.c, src/func_ov006_0212497c.cpp) unwind them in the
// opposite order.  A hierarchy written by the constructor and unwritten by the
// destructor in reverse, with an RTTI base link agreeing, is not a guess.
//
// ---- 3. THE FILL IS ADDRESS-KEYED, SO TWO TABLES ARE CHEAP ----------------
//
// port_scene_fill_rom and bsc_apply both key on the ROM WORD a slot holds, not
// on the slot number, so the same face array can be applied to every table in
// the chain and can only land where the ROM parked that exact body.
//
//   kSingle3DFaces  the eight bodies dScMgSingle3DBase_c overrides
//                   (slots 2, 5, 7, 10, 16, 17, 26, 33)
//   kBscFaces       the eight this class adds or replaces on top
//                   (slots 0, 6, 9, 16, 17, 18, 19, 21)
//
// Slots 16 and 17 appear in both and hold DIFFERENT addresses -- 0x0210a4b0 /
// 0x0210a4e8 in the middle table, 0x02124908 / 0x0212497c in this one -- so
// both arrays can be applied to both tables and each writes only its own.  Six
// of kSingle3DFaces' eight are what fills slots 2, 5, 7, 10, 26 and 33 of the
// DERIVED table, because this class does not override them.
//
// THE MIDDLE TABLE IS NOW SHARED THREE WAYS AND THAT IS STILL SAFE, AND THIS
// IS THE THIRD CLASS scene_mg_memory2.cpp's section 3 SAID TO WATCH FOR.
// hal/scene_mg_flower.cpp and hal/scene_mg_memory2.cpp each define their own
// eight-row array over the same eight DS words; this file makes three.  All
// three fills run on every boot, and the one that runs FIRST claims the middle
// table -- the second and third find no DS word left there and write nothing,
// because bsc_apply keys on a DS address and a slot already holding a host
// thunk matches none.  Row order decides which, and this class's row is
// APPENDED AFTER both, so on this tree:
//
//   - data_ov006_0213e448 keeps hal/scene_mg_flower.cpp's thunks, unchanged
//     from the baseline, and BOTH that file's witness and scene_mg_memory2's
//     keep counting exactly what they counted before this seat existed;
//   - data_ov006_0213fec8 gets THIS file's thunks in its six inherited slots,
//     which is the only table this seat needs to own.
//
// SO THE PROMOTION scene_mg_memory2.cpp ARGUES FOR IS NOW DUE, AND THIS LANE
// DID NOT TAKE IT.  That file says "if a THIRD class under this base is ever
// seated, promoting kSingle3DFaces to an exported seam the way scene_mg.cpp
// exports port_scene_mg_fill_shared is the right move".  This is the third.
// The promotion is not taken here because it means editing two other lanes'
// files in a fan-out where nine other lanes are live on the same base, which is
// exactly the five-way merge the per-class-file rule exists to avoid.  It is
// recorded as owed rather than done, and the measurement below is what says it
// is still only a tidiness debt: this seat's middle-table witness reads ZERO on
// a scene-388 run, which is the flower's fill having claimed the table first.
//
// ---- 4. SLOT 2 IS NOT src's BODY, AND IT IS NOT THIS LANE'S HOST COPY -----
//
// func_ov006_0210a6e4 (AfterInitResources) drops the framework's second
// argument: the ROM never writes r1 before its `bl 0x20b08f0`, so the flags
// ride through in r1, and src spells the call with one argument because that is
// the only way to spell an unnamed value in C.  Disassembled again by this lane
// -- eight instructions plus a one-word pool, r1 untouched -- so the finding is
// re-derived rather than cited.
//
// port/unmatched/MgFlower_Slot2.cpp already carries the repair as
// port_mg_flower_after_init and this file calls it.  src/func_ov006_0210a6e4.cpp
// stays out of port/slice_lky.txt for the same reason it is out of
// port/slice_flw.txt and port/slice_mem.txt: listing it would be an LNK2005
// against that host copy.
//
// ---- 5. THE WIDTH IS 36 ON BOTH TABLES, CHECKED FOUR WAYS -----------------
//
// port/mg_fanout_costs.txt section 11 measured that twelve of twenty-nine
// widths in its own table were phantoms and that a 37-slot fill writes a host
// thunk over live ROM data belonging to another class.  0x184 IS ONE OF THE
// TWELVE it corrected.  Both tables went through all three of its checks, plus
// a fourth this lane adds, before a word was written:
//
//   table       span to next symbol    slot 35     word past the end
//   ---------   --------------------   ---------   -------------------------
//   0213fec8    0213ff58, 0x90 = 36    020ad660    0x0212992c
//   0213e448    0213e4d8, 0x90 = 36    020ad660    0x050a0a0f
//
//   4. RELOCATION COMPLETENESS -- WHICH IS NOT A BOUND ON THE END, AND THIS
//      LINE USED TO PRETEND IT WAS.  It read "36 CONTIGUOUS load relocations at
//      a four-byte stride from 0x0213fec8 to 0x0213ff54, one per slot, and then
//      the stride JUMPS TO EIGHT from 0x0213ff58 on", which invited the reading
//      that the stride-4 run STOPS at slot 35.  It does not: measured over
//      ov006's relocs.txt the contiguous four-byte run starting at 0x0213fec8
//      is THIRTY-SEVEN relocations long and its last is 0x0213ff58, because the
//      first word of the mwcc pair that follows the table is an address too and
//      carries a relocation of its own at exactly +4.  The stride-8 run begins
//      at 0x0213ff60.  What this check really measures is COMPLETENESS: all 36
//      slots carry a load relocation, none is missing, so the fill keys on a
//      table the ROM relocates in full rather than on a run with holes in it.
//      WHERE THE TABLE ENDS IS FIXED BY THE TERMINAL-SLOT CHECK ABOVE
//      (0x020ad660 at index 35) AND BY NOTHING IN THIS ONE.  Measured by lane
//      PKR of run mg11, which hit the same wording on its own table, and
//      re-measured at the mg11 merge.
//
// THE WORD PAST THE END IS A LIVE CODE ADDRESS HERE, which is section 11's
// "where the word happens to look like a code address is exactly where the eye
// fails".  0x0212992c is not padding and not a coincidence: data_ov006_0213ff58
// is an mwcc pair {0x0212992c, 0} and src/__sinit_ov006_021333e0.c line 50
// copies it into data_ov006_02143070 slot 3.  A 37-slot fill of THIS table
// would write a host thunk over the code word of a state belonging to the
// class next door -- and per section 11 that class's identity matters, so this
// lane named it: see port/slice_lky.txt section 4.  The fill below is called
// with 36.
//
// ---- 6. THE ROW GOES LAST, AND THE ORDER IS LOAD-BEARING ------------------
//
// port_scene_registry_install walks port_scene_classes[] in table order and
// calls every row's fill on every boot; port_scene_mg_overlay_load runs the
// thirty-five constructors ONCE PER PROCESS from the tail of the first minigame
// row's fill.  Appending after every existing row therefore means the
// constructors have already run with clean ROM words when this fill starts,
// which is the latent-safe direction section 11 names -- and section 3 above
// depends on it for a second reason.
//
// ---- 7. THE WALL IS ONE TU, AND THE DETECTOR IS THE ROM -------------------
//
// port/unmatched/MgBSC_StateDispatch.cpp carries the single table dispatcher
// and the eleven state addresses.  It is vtable SLOT 6, which is section 14's
// practical rule -- before wiring slot 6 by name, read the src -- and this
// class is a second instance of exactly that: its src declares the table inside
// extern "C", so a link, a compile and a `::*`-free reading all come back clean
// while MSVC strides an eight-byte table by four.
//
// The claim that ONE TU is the whole wall for this class is not made from a
// source sweep.  MgBSC_StateDispatch.cpp section 3 scans the OVERLAY IMAGE for
// the five-instruction ARM Itanium encoding, body by body, each taken at its
// own start and size from ov006's symbols.txt: ONE site across this class's
// twenty own bodies (0x021254dc, slot 6) and ZERO across the eight it inherits
// from dScMgSingle3DBase_c.  That detector is blind to spelling, so it covers
// the plain-int third shape the two prescribed detectors cannot see.
//
// BODY BY BODY, NOT BY ADDRESS RANGE.  An earlier version of this file scanned
// the span 0x02124908..0x0212a3c0 and called it "this class's whole code span".
// It is not: this class's bodies end at 0x0212568c, and that range runs 0x4d34
// bytes further into the NEIGHBOUR, where it picks up four more real dispatch
// sites belonging to MgSnowballSlalom.  MgBSC_StateDispatch.cpp section 3 names
// those four, because port/slice_lky.txt section 4 hands that class's vtable to
// a future lane and a resolved vtable beside a wall priced at zero would be
// worse than handing them nothing.
//
// ---- 8. WHAT THIS SEAT DOES NOT CLAIM -------------------------------------
//
// The state machine is proven to run by the census this file prints, not by
// this comment.  A run that reports slot hits and zero routed dispatches has
// ticked an object without entering its state machine, and that reads as a
// success unless it is printed -- which is why the numbers below are printed
// whether they are zero or not.  Section 15's finding applies here too: a
// census that measures only dispatch cannot see a missing DRAW, so the two
// sprite records slot 9 renders are printed as well.

#include "hal/screen_gap.h"

#include <cstdio>
#include <cstdlib>

extern "C" {

/* the seam into hal/scene_mg.cpp -- mounts, arm9 fill, dScMgBase_c's faces */
unsigned port_scene_mg_fill_shared(void **vt, unsigned n);
void     port_scene_mg_prepare(int id);
int      port_scene_env_want(void);

/* the mount storage the fill writes into.  All three are the mounts' own, so
   the fill replaces the mounted words rather than building a fresh host array
   -- the ov007, curling and memory2 treatment, for the same reason: a second
   host array of the same name is a duplicate symbol, and leaving the mounted
   table alone leaves live wild DS pointers in a table the factory installs. */
extern unsigned char data_ov004_020bc0c0[];   /* dScMgBase_c,         36 */
extern unsigned char data_ov006_0213e448[];   /* dScMgSingle3DBase_c, 36 */
extern unsigned char data_ov006_0213fec8[];   /* dScMgBSC_c,          36 */
extern unsigned char MgLuckyStars_SpawnInfo[];

/* dScMgSingle3DBase_c's eight overrides.  Slot 2 is NOT src's body: see
   section 4 and port/unmatched/MgFlower_Slot2.cpp. */
int   port_mg_flower_after_init(void *c, unsigned f);   /* slot  2 */
void  func_ov006_0210a608(void *c, unsigned f);         /* slot  5 */
int   func_ov006_0210a698(void *c);                     /* slot  7 */
int   func_ov006_0210a664(void *c);                     /* slot 10 */
int   func_ov006_0210a4b0(char *c);                     /* slot 16 D2 */
int   func_ov006_0210a4e8(char *c);                     /* slot 17 D0 */
int   func_ov006_0210a600(void);                        /* slot 26 */
void  func_ov006_0210a708(char *c);                     /* slot 33 */

/* dScMgBSC_c's own eight.  func_ov006_021254c0 is the HOST COPY in
   unmatched/MgBSC_StateDispatch.cpp, not the src TU: it is the
   pointer-to-member dispatcher and the port cannot compile the src.

   SLOTS 18 AND 19 TAKE A SECOND ARGUMENT AND THIS CLASS READS IT, which is
   where it parts company with dScMgMemory2_c.  Both ROM bodies branch on r1:
   0x02125364 compares it against 4, 5 and 3, and 0x0212527c against 4 and 5.
   A thunk that only POPPED the argument -- which is all the seated classes
   before this one needed -- would hand both bodies stack litter and let the
   4/5 arms fire at random.  The declarations below take it and the thunks pass
   it. */
int   func_ov006_0212551c(void *self);        /* slot  0 InitResources */
int   func_ov006_021254c0(void *self);        /* slot  6 Behavior, host copy */
int   func_ov006_021253bc(void *self);        /* slot  9 Render */
void *func_ov006_02124908(char *c);           /* slot 16 D2 */
void *func_ov006_0212497c(char *c);           /* slot 17 D0 */
void  func_ov006_02125364(char *c, int mode); /* slot 18 state reset */
int   func_ov006_0212527c(char *c, int mode); /* slot 19 */
int   func_ov006_02125248(void *self);        /* slot 21 */

/* the factory */
void *MgLuckyStars_Spawn(void);

/* the dispatch file's witnesses */
unsigned port_mg_bsc_state_hits(void);
unsigned port_mg_bsc_floor_hits(void);
void     port_mg_bsc_index_range(int *lo, int *hi);
/* the framework's, from unmatched/MgBase_StateDispatch.cpp */
void     port_mg_dispatch_counts(unsigned *calls, unsigned *unknown);

/* the persistent minigame record this class's level is READ FROM and WRITTEN
   BACK TO.  Nothing new is defined here: the storage is hal/level_boot.cpp's
   .dsstate$savblk0004 and the index function is src/func_ov004_020adc3c.c.
   Named so the census can print the record rather than leave it inferred. */
extern void         *data_ov004_020beb68;     /* the live dScMgBase_c `this` */
extern unsigned char data_0209caf4[];         /* 36 records x 20 bytes       */
int  func_ov004_020adc3c(void *c);            /* (self->field_8 >> 8) & 0xff */

}  /* extern "C" */

/* The record row this class's object is keyed to, or -1 with no live base. */
static int port_mg_bsc_record_index(void)
{
    return data_ov004_020beb68 ? func_ov004_020adc3c(data_ov004_020beb68) : -1;
}

// ---- the tick witness ------------------------------------------------------
//
// One counter per dispatched slot on each of the two tables this file owns
// faces for.  The instrument every scene seat in this port carries, for the
// reason hal/scene_mg.cpp gives: an object that EXISTS and an object that RUNS
// look identical from outside.  The framework slots dScMgBase_c keeps are
// counted by scene_mg.cpp's own array.
static unsigned g_bsc_hits[36];
static unsigned g_bsc_base_hits[36];   /* the same slots on the MIDDLE table */

#define BSC(n)   (++g_bsc_hits[(n)])
#define B3D(n)   (++g_bsc_base_hits[(n)])

/* The two mode values slots 18 and 19 were last handed, kept because section 7
   of this header says the argument is READ here and a census that does not
   print it cannot show the thunk delivered it. */
static int g_bsc_mode18 = -1, g_bsc_mode19 = -1;

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

/* ---- dScMgBSC_c's own eight --------------------------------------------- */
static int  __fastcall bsc_init(void *s, void *)
{ BSC(0);  const int r = func_ov006_0212551c(s);
  /* the GaplessMinigames latch, for hal/scene_mg.cpp's reason: every seated
     minigame calls it so the ones the gapless table does not name can say
     "unsupported" instead of doing nothing quietly.  hal_gapless_splice() is
     deliberately NOT called -- scene 388 has no row in screen_gap.cpp's table
     and gapless behaviour is decided per minigame BY PLAYING IT.  This lane has
     not played it and does not get to vote.  Same ruling as
     hal/scene_mg_bomroom.cpp section 8 and hal/scene_mg_flower.cpp. */
  hal_gapless_minigames_latch(); return r; }
static int  __fastcall bsc_beh(void *s, void *)
{ BSC(6);  return func_ov006_021254c0(s); }
static int  __fastcall bsc_render(void *s, void *)
{ BSC(9);  return func_ov006_021253bc(s); }
static void *__fastcall bsc_d2(void *s, void *)
{ BSC(16); return func_ov006_02124908((char *)s); }
static void *__fastcall bsc_d0(void *s, void *)
{ BSC(17); return func_ov006_0212497c((char *)s); }
/* SLOTS 18 AND 19 TAKE ONE STACK ARGUMENT AND THIS CLASS READS IT.  Run mg5
   lane BASESET scanned both offsets out of the two overlay images word by word
   (runs/mg5/out/baseset/slot18_19_scan.txt): offset 0x48 is 22 sites and offset
   0x4c is 14, and the argument count is ONE at every one of them.  Every seated
   class before this one had a ROM body that ignored the value, so the parameter
   existed only so __fastcall would clean four bytes.  Here both bodies BRANCH
   on it -- 0x02125364 against 4, 5 and 3, 0x0212527c against 4 and 5 -- so it
   is forwarded, and the mode is recorded for the census. */
static int  __fastcall bsc_reset(void *s, void *, int mode)
{ BSC(18); g_bsc_mode18 = mode; func_ov006_02125364((char *)s, mode); return 1; }
static int  __fastcall bsc_v19(void *s, void *, int mode)
{ BSC(19); g_bsc_mode19 = mode; return func_ov006_0212527c((char *)s, mode); }
static int  __fastcall bsc_v21(void *s, void *)
{ BSC(21); return func_ov006_02125248(s); }

/* SM64DS_SCENE_SLOT0=0 and SM64DS_SCENE_SLOT9=0, the diagnostics every scene
   seat in this port carries, counted separately so a run can never read a no-op
   as the real body having run. */
static unsigned g_bsc_init_skipped, g_bsc_render_skipped;
static int __fastcall bsc_init_noop(void *, void *)
{ ++g_bsc_init_skipped; return 1; }
static int __fastcall bsc_render_noop(void *, void *)
{ ++g_bsc_render_skipped; return 1; }

struct BscFace { unsigned ds; void *host; };

/* Keyed on the ROM WORD each slot holds, so the array is order-independent and
   cannot land on a slot the ROM did not park that body in. */
static const BscFace kSingle3DFaces[] = {
    {0x0210a6e4u, (void *)s3_ainit},  {0x0210a608u, (void *)s3_aclean},
    {0x0210a698u, (void *)s3_bbeh},   {0x0210a664u, (void *)s3_bren},
    {0x0210a4b0u, (void *)s3_d2},     {0x0210a4e8u, (void *)s3_d0},
    {0x0210a600u, (void *)s3_v26},    {0x0210a708u, (void *)s3_v33},
};

static const BscFace kBscFaces[] = {
    {0x0212551cu, (void *)bsc_init},  {0x021254c0u, (void *)bsc_beh},
    {0x021253bcu, (void *)bsc_render},{0x02124908u, (void *)bsc_d2},
    {0x0212497cu, (void *)bsc_d0},    {0x02125364u, (void *)bsc_reset},
    {0x0212527cu, (void *)bsc_v19},   {0x02125248u, (void *)bsc_v21},
};

static unsigned bsc_apply(void **vt, unsigned n, const BscFace *f, unsigned nf)
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
static unsigned bsc_raw_left(void **vt, unsigned n)
{
    unsigned left = 0;
    for (unsigned i = 0; i < n; ++i) {
        const unsigned w = (unsigned)(size_t)vt[i];
        if (w >= 0x02000000u && w < 0x02400000u)
            ++left;
    }
    return left;
}

extern "C" void port_scene_luckystars_hits(void);

/* How many words this seat's own copy of the middle-table array claimed.  Zero
   is the EXPECTED reading on a tree that also carries the flower row, and
   section 3 is why; it is printed rather than assumed. */
static unsigned g_bsc_mid_claimed;

extern "C" void port_scene_fill_luckystars(void)
{
    void **base = (void **)data_ov004_020bc0c0;
    void **mid  = (void **)data_ov006_0213e448;
    void **vt   = (void **)data_ov006_0213fec8;

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
       the two vptr writes reads it.  Section 3 is why applying this file's own
       arrays here is safe next to the flower's and memory2's. */
    port_scene_mg_fill_shared(mid, 36);
    g_bsc_mid_claimed =
        bsc_apply(mid, 36, kSingle3DFaces,
                  sizeof kSingle3DFaces / sizeof kSingle3DFaces[0]);
    g_bsc_mid_claimed +=
        bsc_apply(mid, 36, kBscFaces,
                  sizeof kBscFaces / sizeof kBscFaces[0]);

    /* THE DERIVED TABLE.  Shared arm9 and dScMgBase_c words first, then the
       intermediate base's eight, then this class's own eight.  Order does not
       matter -- the three key sets are disjoint, because a word is one address
       -- but it reads in inheritance order.  THIRTY-SIX, not thirty-seven:
       section 5, and the word a thirty-seventh would take is another class's
       live state pair. */
    port_scene_mg_fill_shared(vt, 36);
    bsc_apply(vt, 36, kSingle3DFaces,
              sizeof kSingle3DFaces / sizeof kSingle3DFaces[0]);
    bsc_apply(vt, 36, kBscFaces,
              sizeof kBscFaces / sizeof kBscFaces[0]);

    /* the two diagnostics, applied after the fill so they override it */
    {
        const char *s0 = std::getenv("SM64DS_SCENE_SLOT0");
        const char *s9 = std::getenv("SM64DS_SCENE_SLOT9");
        if (s0 && s0[0] == '0') vt[0] = (void *)bsc_init_noop;
        if (s9 && s9[0] == '0') vt[9] = (void *)bsc_render_noop;
    }

    {
        const unsigned lb = bsc_raw_left(base, 36);
        const unsigned lm = bsc_raw_left(mid, 36);
        const unsigned lv = bsc_raw_left(vt, 36);
        if (lb || lm || lv) {
            std::fprintf(stderr, "  [scene] LUCKYSTARS FILL INCOMPLETE: "
                         "dScMgBase_c leaves %u of 36 raw DS words, "
                         "dScMgSingle3DBase_c %u, dScMgBSC_c %u. A dispatch of "
                         "any of them jumps to a DS address as a host one.\n",
                         lb, lm, lv);
            std::fflush(stderr);
        }
    }

    /* The constructors, gated on the requested id, exactly as every other
       minigame fill does it.  Idempotent behind scene_mg.cpp's own
       once-per-process guard. */
    port_scene_mg_prepare(port_scene_env_want());

    if (port_scene_env_want() == 388) {
        static int armed;
        if (!armed) {
            armed = 1;
            std::atexit(port_scene_luckystars_hits);
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
   src/MgLuckyStars_Spawn.cpp calls func_ov004_020b2adc(p) WITH its argument, so
   this class is on the correct side of it and the factory is linked from the
   slice rather than host-copied. */
static char *g_bsc_self;

extern "C" void *port_mg_luckystars_spawn(void)
{
    void *p = MgLuckyStars_Spawn();
    g_bsc_self = (char *)p;
    return p;
}

extern "C" void port_scene_luckystars_hits(void)
{
    unsigned total = 0, mtotal = 0;
    for (int i = 0; i < 36; ++i) { total += g_bsc_hits[i]; mtotal += g_bsc_base_hits[i]; }

    std::printf("[scene] dScMgBSC_c slot hits: init %u, behavior %u, render %u, "
                "D2 %u, D0 %u, state-reset %u, slot19 %u, slot21 %u%s\n",
                g_bsc_hits[0], g_bsc_hits[6], g_bsc_hits[9], g_bsc_hits[16],
                g_bsc_hits[17], g_bsc_hits[18], g_bsc_hits[19], g_bsc_hits[21],
                g_bsc_render_skipped
                    ? "  [RENDER SLOT NO-OP'd: SM64DS_SCENE_SLOT9=0]" : "");
    if (g_bsc_init_skipped)
        std::printf("[scene] INIT SLOT NO-OP'd: SM64DS_SCENE_SLOT0=0, %u "
                    "time(s)\n", g_bsc_init_skipped);

    std::printf("[scene] dScMgBSC_c own slots entered:");
    for (int i = 0; i < 36; ++i)
        if (g_bsc_hits[i]) std::printf(" %d(x%u)", i, g_bsc_hits[i]);
    std::printf("   (%u total)\n", total);

    std::printf("[scene] dScMgSingle3DBase_c slots entered (this seat's copy):");
    for (int i = 0; i < 36; ++i)
        if (g_bsc_base_hits[i]) std::printf(" %d(x%u)", i, g_bsc_base_hits[i]);
    std::printf("   (%u total); this seat's middle-table fill claimed %u word(s)"
                " (0 is EXPECTED: the flower row runs first, section 3)\n",
                mtotal, g_bsc_mid_claimed);

    /* THE STATE MACHINE'S OWN WITNESS.  Printed whether or not it fired: a run
       with slot hits and zero routed dispatches has ticked the object without
       entering its state machine, and that reads as a success unless it is said
       out loud. */
    {
        unsigned calls = 0, unknown = 0;
        int lo = -1, hi = -1;
        port_mg_dispatch_counts(&calls, &unknown);
        port_mg_bsc_index_range(&lo, &hi);
        std::printf("[scene] dScMgBSC_c state dispatch: %u routed to one of the "
                    "class's 14 table slots (11 distinct bodies), %u bodiless "
                    "want(s); state index range entered %d..%d; %u framework "
                    "call(s), %u UNHANDLED address(es)\n",
                    port_mg_bsc_state_hits(), port_mg_bsc_floor_hits(),
                    lo, hi, calls, unknown);
        std::printf("[scene] dScMgBSC_c state floor: NONE. All eleven addresses "
                    "in data_ov006_02142f94 reach a matched src TU and none "
                    "carries a NONMATCHING banner\n");
    }

    /* The state index the ROM's own dispatcher reads, at the offset
       disassembled in unmatched/MgBSC_StateDispatch.cpp section 4. +0x51b8 is
       slot 6's, it is the one slot 18 zeroes, and slot 9 gates its whole draw
       on it being >= 1 -- three independent readers of one field. */
    if (g_bsc_self)
        std::printf("[scene] dScMgBSC_c object at %p, state index +0x51b8 = %d, "
                    "slot-18 mode last seen %d, slot-19 mode last seen %d\n",
                    (void *)g_bsc_self, *(int *)(g_bsc_self + 0x51b8),
                    g_bsc_mode18, g_bsc_mode19);

    /* THE TWO SPRITE RECORDS, because a run that renders nothing and a run that
       has nothing to render read the same on every other line -- run mg7 lane
       MEMCARDS' finding, section 15, and the reason it is worth the twenty
       lines.  src/func_ov006_021253bc.c (slot 9, Render) draws exactly two
       sprites through Hud_RenderSprite (0x020af68c), gated on the state index
       being >= 1, and their fields are read off the ROM at 0x021253f0..0x0212545c:
         +0x51a8 + i*8   the 20.12 x, shifted down by 12 for the draw
         +0x51ac + i*8   the 20.12 y
         +0x51ca + i     a selector, doubled and added when the frame is >= 3
         +0x51cc + i     the frame; < 3 indexes data_ov006_0213fe8c directly
       So a count here is the difference between "nothing was drawn" and
       "something was drawn off screen". */
    if (g_bsc_self) {
        const int st = *(const int *)(g_bsc_self + 0x51b8);
        std::printf("[scene] dScMgBSC_c sprites (draw gate: state %d >= 1 is %s):",
                    st, st >= 1 ? "OPEN" : "SHUT");
        for (int i = 0; i < 2; ++i) {
            const int x = *(const int *)(g_bsc_self + 0x51a8 + i * 8) >> 12;
            const int y = *(const int *)(g_bsc_self + 0x51ac + i * 8) >> 12;
            const unsigned b0 = *(const unsigned char *)(g_bsc_self + 0x51ca + i);
            const unsigned b1 = *(const unsigned char *)(g_bsc_self + 0x51cc + i);
            const unsigned idx = (b1 < 3) ? b1 : (b1 + b0 * 2);
            std::printf(" [%d] @%d,%d sel %u frame %u -> sprite row %u%s", i,
                        x, y, b0, b1, idx,
                        (x > -32 && x < 288 && y > -44 && y < 236)
                            ? " ON-SCREEN" : " off-screen");
        }
        std::printf("\n");
    }

    /* ---- THE LEVEL, AND WHERE IT COMES FROM -----------------------------
       Same mechanism run mg8 lane MMD disassembled for Memory Master, found
       again here rather than assumed: this class keeps its progress in the
       PERSISTENT per-minigame record, not in the scene.

         slot 18  (src/func_ov006_02125364.c) SEEDS it, and only on mode 3:
                    if (mode != 4 && mode != 5 && mode == 3)
                        self->+0xb4 = func_ov004_020ad878();
                        func_ov004_020adb1c(self->+0xb4);
                  -- func_ov004_020ad878 is data_0209caf4[minigame index][1],
                  and func_ov004_020adb1c is the star badge on the top screen.
         slot 19  (src/func_ov006_0212527c.cpp) WRITES IT BACK, on state 0xc
                  with data_ov004_020bf9e4 == 1:
                        func_ov004_020ad79c(self->+0xa8, self->+0xb4)
                  -- score and level, the same pair Memory Master writes.
         slot 21  (src/func_ov006_02125248.c) READS it: (+0xb4)/5 capped at 3,
                  plus one, handed to func_ov004_020b6324. Four bands.
         slot 0   (src/func_ov006_0212551c.cpp) seeds +0xa8 and +0xac from
                  func_ov004_020ad8b8() and then dispatches slot 18 with mode 3
                  through the object's own vtable -- which is how the seed above
                  is reached on a direct scene boot.

       A fresh record is ZERO (SaveData::SetDefaultValuesMg memsets it), so the
       port starts every launch at level 0, band 1. That is the save-medium gap
       section 17 records and not a minigame one. */
    if (g_bsc_self) {
        const int idx = port_mg_bsc_record_index();
        const int lvl = *(const int *)(g_bsc_self + 0xb4);
        int band = lvl / 5; if (band > 3) band = 3;
        std::printf("[scene] dScMgBSC_c level: +0xb4 = %d, +0xa8 score = %d, "
                    "+0xac = %d  ->  slot 21 band = %d (of 1..4)\n",
                    lvl, *(const int *)(g_bsc_self + 0xa8),
                    *(const int *)(g_bsc_self + 0xac), band + 1);
        if (idx >= 0 && idx < 36) {
            const int *rec = (const int *)(data_0209caf4 + idx * 20);
            std::printf("[scene] dScMgBSC_c save record data_0209caf4[%d] "
                        "= { %d, %d, %d, %d, %d }  (field 1 IS the level +0xb4 "
                        "is seeded from)\n", idx,
                        rec[0], rec[1], rec[2], rec[3], rec[4]);
        } else {
            std::printf("[scene] dScMgBSC_c save record: NO minigame index "
                        "(data_ov004_020beb68 = %p)\n", data_ov004_020beb68);
        }
    }
    std::fflush(stdout);
}
