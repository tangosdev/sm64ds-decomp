// dScMgMemory_c, the "Memory Match" minigame -- actor id 0x16a, scene 362.
// Run mg9, lane MMT.  The THIRD seated class under dScMgSingle3DBase_c and the
// immediate sibling of dScMgMemory2_c (0x16b, scene 363, "Memory Master").
//
// Read port/slice_mmt.txt for the identity derivation, the three width checks,
// the twelve marker rulings and the closure.  Read hal/scene_mg_memory2.cpp
// next: this class has the same five-table state machine, the same record
// stride and the same two-index object, and every difference between them is
// silent.  This file is the seat: the two tables' faces, the fill, the factory
// forwarder and the run census.
//
// ---- 1. THE NAME IS dScMgMemory_c AND THE ROM SAYS SO ---------------------
//
// The RTTI string at 0x0213d0b4 reads "13dScMgMemory_c", and the record that
// points at it, data_ov006_0213d090 = {0x0209a764, 0x0213d0b4, 0x0213bc64}, is
// what the word at 0x0213d1b4 -- the relocated pointer immediately before the
// vtable -- loads.  This file and its unmatched/ companions are spelled
// MgMemory1_* rather than MgMemory_* for one reason and it is not the ROM's:
// "MgMemory_" is a prefix of "MgMemory2_", and a reader skimming a slice or a
// link map should not have to check which of two files a name belongs to.  The
// ROM's own name is in this paragraph, in the slice header and in every census
// line this file prints.
//
// ---- 2. THE HIERARCHY IS THREE DEEP, AND THE ROM SAYS SO TWICE ------------
//
//     Scene -> dScMgBase_c         data_ov004_020bc0c0  36 slots
//           -> dScMgSingle3DBase_c data_ov006_0213e448  36 slots
//           -> dScMgMemory_c       data_ov006_0213d1b8  36 slots
//
// port/mg_fanout_costs.txt section 3's row for 0x16a lists ONE vtable and says
// nothing about an intermediate base, exactly as it did for 0x16b before run
// mg6 lane MEM corrected it (section 13, CORRECTION 1).  A seat that filled one
// derived table would leave SIX of its own slots (2, 5, 7, 10, 26 and 33)
// holding raw DS words.  Both edges are read off code rather than inherited
// from the sibling:
//
//   MgMemoryMatch_Spawn (0x020f5504, disassembled) calls func_ov004_020b2adc(p)
//   WITH its argument, writes 0x0213e448 into p[0], constructs the
//   Particle::SysTracker at +0x471c, then writes 0x0213d1b8 over it and
//   constructs the model sub-object at +0x4f38.  The object is 0x5340 bytes.
//   src/MgMemoryMatch_Spawn.c reproduces that instruction for instruction.
//
//   src/func_ov006_020f3888.cpp (slot 17, D0) unwinds them in the OPPOSITE
//   order: 0x0213d1b8 first, then 0x0213e448, then the SysTracker at +0x471c,
//   then func_ov004_020b29c0 and Memory::Deallocate.
//   src/func_ov006_020f3834.cpp (slot 16, D2) does the same without the
//   Deallocate.
//
// THE D1/D2 ORDER WAS CHECKED RATHER THAN ASSUMED, because the config has the
// two dtor addresses swapped elsewhere in this tree and the byte gate cannot
// see it.  0x020f3834 has no Memory::Deallocate and 0x020f3888 does, so slot 16
// is D2 and slot 17 is D0, which is the same way round as the sibling's
// 0x020f5564 / 0x020f55b8.
//
// ---- 3. THE FILL IS ADDRESS-KEYED, SO TWO TABLES ARE CHEAP ----------------
//
// port_scene_fill_rom and mem1_apply both key on the ROM WORD a slot holds, not
// on the slot number, so the same face array can be applied to every table in
// the chain and can only land where the ROM parked that exact body.
//
//   kSingle3DFaces  the eight bodies dScMgSingle3DBase_c overrides
//                   (slots 2, 5, 7, 10, 16, 17, 26, 33)
//   kMemory1Faces   the eight this class adds or replaces on top
//                   (slots 0, 6, 9, 16, 17, 18, 19, 21)
//
// Slots 16 and 17 appear in both and hold DIFFERENT addresses -- 0x0210a4b0 /
// 0x0210a4e8 in the middle table, 0x020f3834 / 0x020f3888 in this one -- so
// both arrays can be applied to both tables and each writes only its own.  Six
// of kSingle3DFaces' eight are what fills slots 2, 5, 7, 10, 26 and 33 of the
// DERIVED table, because this class does not override them.
//
// THIS IS THE THIRD COPY OF kSingle3DFaces AND hal/scene_mg_memory2.cpp
// PREDICTED IT.  That file's section 3 ends "If a THIRD class under this base
// is ever seated, promoting kSingle3DFaces to an exported seam the way
// scene_mg.cpp exports port_scene_mg_fill_shared is the right move, and this
// paragraph is the argument for it."  This is the third class, the promotion is
// now DUE, and THIS LANE DID NOT DO IT: run mg9 fans ten seats out in parallel
// and the promotion edits hal/scene_mg_flower.cpp and hal/scene_mg_memory2.cpp,
// two files this lane does not own.  It is recorded as a merge-time finding
// rather than taken, and the safety argument is unchanged and re-derived here:
//
//   - The middle table data_ov006_0213e448 is claimed by whichever fill runs
//     FIRST, and this row is appended after the flower's and the sibling's, so
//     the flower keeps it and both other copies write nothing into it.
//   - This seat's own table data_ov006_0213d1b8 is named by NO other file, so
//     the six inherited slots in it are this file's alone.  Three copies of the
//     array cannot collide on it because no other fill ever visits it.
//
// AND THE WITNESS BELOW COUNTS THE SECOND OF THOSE, NOT THE FIRST.  A first
// draft of this paragraph said this seat's dScMgSingle3DBase_c counter "reads
// ZERO and that is the correct reading"; the run says 11642 on a 1500-frame
// boot and the run is right.  kSingle3DFaces is applied to BOTH tables, and
// the copy that lands is the one in this class's own derived table -- slots 2,
// 7, 10, 26 and 33 of data_ov006_0213d1b8, which is where every one of those
// dispatches actually goes.  The zero belongs to the OTHER seats' copies of
// the array, which is what hal/scene_mg_flower.cpp's own line reports on a
// scene-362 run.  Two counters, two tables, and the mistake was reading the
// array's NAME as its destination.
//
// ---- 4. SLOT 2 IS NOT src's BODY, AND IT IS NOT THIS LANE'S HOST COPY -----
//
// func_ov006_0210a6e4 (AfterInitResources) drops the framework's second
// argument: the ROM never writes r1 before its `bl 0x20b08f0`, so the flags
// ride through in r1, and src spells the call with one argument because that is
// the only way to spell an unnamed value in C.  On the host the callee reads
// stack litter, and func_ov004_020b08f0's tail is Scene::AfterInitResources
// (this, flags) where flags == 1 marks the actor for destruction -- so the
// wrong value is a coin flip on whether the scene survives frame 0.
//
// port/unmatched/MgFlower_Slot2.cpp already carries the repair as
// port_mg_flower_after_init, and its header ends "THE NEXT LANE TO SEAT 361 OR
// 363 SHOULD CALL THIS RATHER THAN WRITE A SECOND".  This is 362, under the
// same base and through the same slot, and it calls it.
// src/func_ov006_0210a6e4.cpp stays out of port/slice_mmt.txt for the same
// reason it is out of port/slice_flw.txt and port/slice_mem.txt: listing it
// would be an LNK2005 against that host copy.
//
// ---- 5. THE WIDTH IS 36, CHECKED THREE WAYS PLUS THE RELOCATION COUNT -----
//
// port/mg_fanout_costs.txt section 11 measured that twelve of twenty-nine
// widths in its own section 3 table were phantoms, and 0x16a IS ONE OF THE
// TWELVE it names.  All three checks were re-run on this tree before a word was
// written, and the fourth column is this lane's own:
//
//   check                     reading
//   -----------------------   ------------------------------------------------
//   1 SPAN                    next config symbol after data_ov006_0213d1b8 is
//                             data_ov006_0213d248, exactly 0x90 = 36 words on
//   2 TERMINAL SLOT           slot 35 holds 0x020ad660, the word every
//                             dScMgBase_c-derived table holds there
//   3 WHAT THE WORD IS        index 36 reads 0x020f6538 and index 37 reads
//                             0x00000000 -- an mwcc PAIR {code, adj}, and the
//                             code is func_ov006_020f6538, which is
//                             dScMgMemory2_c's round-end state, slot 0 of the
//                             sibling's data_ov006_021423c0 (its constructor
//                             src/__sinit_ov006_021314e4.c copies
//                             data_ov006_0213d248 into it)
//   4 RELOCATION COUNT        exactly THREE loads target 0x0213d1b8 in the
//                             whole overlay: 0x020f387c (D2's pool),
//                             0x020f38e0 (D0's pool) and 0x020f5560 (the
//                             factory's pool).  A 37-slot table would need a
//                             consumer of its extra word and has none.
//
// CHECK 3 IS SECTION 11's HAZARD WITH A NAME ON IT, and the victim is the class
// next door.  A 37-slot fill here would write a host thunk over the code word
// of Memory Master's own state, on a tree where scene 363 is seated and green.
// This fill is called with 36.
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
// ---- 7. THE WALL IS THIS CLASS'S AND IT IS FOUR TUs -----------------------
//
// port/unmatched/MgMemory1_StateDispatch.cpp carries the four TABLE
// dispatchers and the twenty-five state addresses, twenty-two of them routed
// and three carried with the arity marked unknown.  The dispatchers that read a
// member pointer out of an object FIELD are NOT new here: this class reaches
// the same model sub-object at the same +0x4f38 as the sibling, so
// unmatched/MgMemory2_FieldPmf.cpp's three (func_ov006_020c07e8,
// func_ov006_020c19d0, func_ov004_020b52fc) already cover them and are shared
// rather than duplicated.  Nothing in this file dispatches a member pointer.
//
// ---- 8. THE CARD DRAW IS A FLOOR AND THIS SEAT DOES NOT DRAW CARDS --------
//
// func_ov006_020f3e68 -- the sixth call vtable slot 9 (Render) makes, 0xa8, the
// only code in the class that puts a card pixel anywhere -- has NO src TU in
// any overlay and NO delink block.  port/unmatched/MgMemory1_Faces.cpp holds a
// named counting trap for it and carries the ROM read of what it does, and the
// census below prints the trap's count on every run.
//
// SO A GREEN RUN OF THIS SCENE MEANS "the board is dealt and the machine runs",
// NOT "the game looks right".  The sibling's identical floor was exactly this
// shape until run mg7 lane MEMCARDS decompiled it, and section 15 of
// port/mg_fanout_costs.txt records the symptom it produced: every dispatch
// census green, every state count right, a whole board laid out at real screen
// coordinates, and nothing on screen.  The card-record census below is here so
// that reading is available from the numbers instead of from a screenshot.
//
// ---- 9. WHAT THIS SEAT DOES NOT CLAIM -------------------------------------
//
// The state machine is proven to run by the census this file prints, not by
// this comment.  A run that reports slot hits and zero routed dispatches has
// ticked an object without entering its state machine, and that reads as a
// success unless it is printed -- which is why the numbers below are printed
// whether they are zero or not.

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
extern unsigned char data_ov006_0213d1b8[];   /* dScMgMemory_c,       36 */
extern unsigned char MgMemoryMatch_SpawnInfo[];

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

/* dScMgMemory_c's own eight.  func_ov006_020f5388 is the HOST COPY in
   unmatched/MgMemory1_StateDispatch.cpp, not the src TU: it is the
   pointer-to-member dispatcher and the port cannot compile the src. */
int   func_ov006_020f53e4(char *self);        /* slot  0 InitResources */
int   func_ov006_020f5388(void *self);        /* slot  6 Behavior, host copy */
int   func_ov006_020f5324(char *c);           /* slot  9 Render */
void *func_ov006_020f3834(char *c);           /* slot 16 D2 */
void *func_ov006_020f3888(char *c);           /* slot 17 D0 */
void  func_ov006_020f52c4(char *c);           /* slot 18 state reset */
int   func_ov006_020f5250(char *c);           /* slot 19 */
int   func_ov006_020f523c(char *c);           /* slot 21 */

/* the factory */
int  *MgMemoryMatch_Spawn(void);

/* this seat's own witnesses */
unsigned port_mg_memory1_state_hits(void);
unsigned port_mg_memory1_floor_hits(void);
unsigned port_mg_memory1_carddraw_hits(void);
/* the framework's, from unmatched/MgBase_StateDispatch.cpp */
void     port_mg_dispatch_counts(unsigned *calls, unsigned *unknown);
/* the sibling's field dispatcher, shared through the model sub-object */
void     port_mg_memory2_field_counts(unsigned *calls, unsigned *hits);

/* the persistent minigame record the board size is READ FROM, and the two
   framework bodies that address it.  Nothing new is defined here: the storage
   is hal/level_boot.cpp's .dsstate$savblk0004 and the index function is
   src/func_ov004_020adc3c.c. */
extern void         *data_ov004_020beb68;     /* the live dScMgBase_c `this` */
extern unsigned char data_0209caf4[];         /* 36 records x 20 bytes       */
int  func_ov004_020adc3c(void *c);            /* (self->field_8 >> 8) & 0xff */

}  /* extern "C" */

/* The record row this class's object is keyed to, or -1 with no live base.
   func_ov004_020ad878 -- the ONLY writer of the +0xb4 SEED goes through it --
   computes the same index from the same pointer. */
static int port_mg1_record_index(void)
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
static unsigned g_mem1_hits[36];
static unsigned g_mem1_base_hits[36];   /* the same slots on the MIDDLE table */

#define MEM1(n)  (++g_mem1_hits[(n)])
#define M3D1(n)  (++g_mem1_base_hits[(n)])

/* ---- dScMgSingle3DBase_c's eight ---------------------------------------- */
static void *__fastcall s31_ainit(void *s, void *, unsigned f)
{ M3D1(2);  return (void *)(size_t)port_mg_flower_after_init(s, f); }
static void __fastcall s31_aclean(void *s, void *, unsigned f)
{ M3D1(5);  func_ov006_0210a608(s, f); }
static int  __fastcall s31_bbeh(void *s, void *)
{ M3D1(7);  return func_ov006_0210a698(s); }
static int  __fastcall s31_bren(void *s, void *)
{ M3D1(10); return func_ov006_0210a664(s); }
static void *__fastcall s31_d2(void *s, void *)
{ M3D1(16); return (void *)(size_t)func_ov006_0210a4b0((char *)s); }
static void *__fastcall s31_d0(void *s, void *)
{ M3D1(17); return (void *)(size_t)func_ov006_0210a4e8((char *)s); }
static int  __fastcall s31_v26(void *, void *)
{ M3D1(26); return func_ov006_0210a600(); }
static int  __fastcall s31_v33(void *s, void *)
{ M3D1(33); func_ov006_0210a708((char *)s); return 0; }

/* ---- dScMgMemory_c's own eight ------------------------------------------ */
static int  __fastcall mem1_init(void *s, void *)
{ MEM1(0);  const int r = func_ov006_020f53e4((char *)s);
  /* the GaplessMinigames latch, for hal/scene_mg.cpp's reason: every seated
     minigame calls it so the ones the gapless table does not name can say
     "unsupported" instead of doing nothing quietly. */
  hal_gapless_minigames_latch(); return r; }
static int  __fastcall mem1_beh(void *s, void *)
{ MEM1(6);  const int r = func_ov006_020f5388(s); hal_gapless_splice();
  return r; }
static int  __fastcall mem1_render(void *s, void *)
{ MEM1(9);  return func_ov006_020f5324((char *)s); }
static void *__fastcall mem1_d2(void *s, void *)
{ MEM1(16); return func_ov006_020f3834((char *)s); }
static void *__fastcall mem1_d0(void *s, void *)
{ MEM1(17); return func_ov006_020f3888((char *)s); }
/* SLOTS 18 AND 19 TAKE ONE STACK ARGUMENT AND THE THUNK MUST POP IT.  Run mg5
   lane BASESET scanned both offsets out of the two overlay images word by word
   (runs/mg5/out/baseset/slot18_19_scan.txt): offset 0x48 is 22 sites and offset
   0x4c is 14, and the argument count is ONE at every one of them.  A __fastcall
   thunk declared (void*, void*) compiles to a bare `ret`, leaks the caller's
   four bytes, and a later `ret` takes a garbage return address at a fault that
   MOVES WITH THE BUILD.  The parameter exists so __fastcall cleans four bytes.

   WHETHER IT MUST ALSO FORWARD IT IS A PER-CLASS QUESTION and run mg9 lane LKY
   is why: dScMgLuckyStars_c's slot-18 and slot-19 bodies READ the second
   argument and branch on it.  Disassembled here rather than inherited, and the
   test is where r1 is first TOUCHED.  func_ov006_020f52c4's first mention of
   r1 is `ldr r1,[pc,#0x40]` after four instructions, and
   func_ov006_020f5250's is `ldr r1,[r0,#0x314]` after three -- both WRITES, so
   neither body can have read the incoming value and cleaning is enough.
   NOTHING WITNESSES EITHER WAY: both slots read zero hits on every run this
   lane made, at 300, 900 and 1500 frames. */
static int  __fastcall mem1_reset(void *s, void *, int /*ridethrough*/)
{ MEM1(18); func_ov006_020f52c4((char *)s); return 1; }
static int  __fastcall mem1_v19(void *s, void *, int /*ridethrough*/)
{ MEM1(19); return func_ov006_020f5250((char *)s); }
static int  __fastcall mem1_v21(void *s, void *)
{ MEM1(21); return func_ov006_020f523c((char *)s); }

/* SM64DS_SCENE_SLOT0=0 and SM64DS_SCENE_SLOT9=0, the diagnostics every scene
   seat in this port carries, counted separately so a run can never read a no-op
   as the real body having run. */
static unsigned g_mem1_init_skipped, g_mem1_render_skipped;
static int __fastcall mem1_init_noop(void *, void *)
{ ++g_mem1_init_skipped; return 1; }
static int __fastcall mem1_render_noop(void *, void *)
{ ++g_mem1_render_skipped; return 1; }

struct Mem1Face { unsigned ds; void *host; };

/* Keyed on the ROM WORD each slot holds, so the array is order-independent and
   cannot land on a slot the ROM did not park that body in. */
static const Mem1Face kSingle3DFaces[] = {
    {0x0210a6e4u, (void *)s31_ainit},  {0x0210a608u, (void *)s31_aclean},
    {0x0210a698u, (void *)s31_bbeh},   {0x0210a664u, (void *)s31_bren},
    {0x0210a4b0u, (void *)s31_d2},     {0x0210a4e8u, (void *)s31_d0},
    {0x0210a600u, (void *)s31_v26},    {0x0210a708u, (void *)s31_v33},
};

static const Mem1Face kMemory1Faces[] = {
    {0x020f53e4u, (void *)mem1_init},  {0x020f5388u, (void *)mem1_beh},
    {0x020f5324u, (void *)mem1_render},{0x020f3834u, (void *)mem1_d2},
    {0x020f3888u, (void *)mem1_d0},    {0x020f52c4u, (void *)mem1_reset},
    {0x020f5250u, (void *)mem1_v19},   {0x020f523cu, (void *)mem1_v21},
};

static unsigned mem1_apply(void **vt, unsigned n, const Mem1Face *f, unsigned nf)
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
static unsigned mem1_raw_left(void **vt, unsigned n)
{
    unsigned left = 0;
    for (unsigned i = 0; i < n; ++i) {
        const unsigned w = (unsigned)(size_t)vt[i];
        if (w >= 0x02000000u && w < 0x02400000u)
            ++left;
    }
    return left;
}

extern "C" void port_scene_memory1_hits(void);

extern "C" void port_scene_fill_memory1(void)
{
    void **base = (void **)data_ov004_020bc0c0;
    void **mid  = (void **)data_ov006_0213e448;
    void **vt   = (void **)data_ov006_0213d1b8;

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
       arrays here is safe next to the flower's and the sibling's, and why this
       seat's middle-table witness is expected to read ZERO. */
    port_scene_mg_fill_shared(mid, 36);
    mem1_apply(mid, 36, kSingle3DFaces,
               sizeof kSingle3DFaces / sizeof kSingle3DFaces[0]);
    mem1_apply(mid, 36, kMemory1Faces,
               sizeof kMemory1Faces / sizeof kMemory1Faces[0]);

    /* THE DERIVED TABLE.  Shared arm9 and dScMgBase_c words first, then the
       intermediate base's eight, then this class's own eight.  Order does not
       matter -- the three key sets are disjoint, because a word is one address
       -- but it reads in inheritance order.  THIRTY-SIX, never thirty-seven:
       section 5's check 3 names what index 36 would clobber. */
    port_scene_mg_fill_shared(vt, 36);
    mem1_apply(vt, 36, kSingle3DFaces,
               sizeof kSingle3DFaces / sizeof kSingle3DFaces[0]);
    mem1_apply(vt, 36, kMemory1Faces,
               sizeof kMemory1Faces / sizeof kMemory1Faces[0]);

    /* the two diagnostics, applied after the fill so they override it */
    {
        const char *s0 = std::getenv("SM64DS_SCENE_SLOT0");
        const char *s9 = std::getenv("SM64DS_SCENE_SLOT9");
        if (s0 && s0[0] == '0') vt[0] = (void *)mem1_init_noop;
        if (s9 && s9[0] == '0') vt[9] = (void *)mem1_render_noop;
    }

    {
        const unsigned lb = mem1_raw_left(base, 36);
        const unsigned lm = mem1_raw_left(mid, 36);
        const unsigned lv = mem1_raw_left(vt, 36);
        if (lb || lm || lv) {
            std::fprintf(stderr, "  [scene] MEMORY1 FILL INCOMPLETE: "
                         "dScMgBase_c leaves %u of 36 raw DS words, "
                         "dScMgSingle3DBase_c %u, dScMgMemory_c %u. A dispatch "
                         "of any of them jumps to a DS address as a host one.\n",
                         lb, lm, lv);
            std::fflush(stderr);
        }
    }

    /* The constructors, gated on the requested id, exactly as every other
       minigame fill does it.  Idempotent behind scene_mg.cpp's own
       once-per-process guard. */
    port_scene_mg_prepare(port_scene_env_want());

    if (port_scene_env_want() == 362) {
        static int armed;
        if (!armed) {
            armed = 1;
            std::atexit(port_scene_memory1_hits);
        }
    }
}

/* The registry's factory column is void *(*)(void) and so is the matched
   factory; the forwarder is kept for the reason title_spawn and
   port_mg_memory2_spawn are, so the seat has one place to observe the object
   without the registry table growing a second column.

   THE FACTORY NEEDS NO DISPLACEMENT RULING, and the check was run rather than
   inherited from the sibling.  MgMemoryMatch_Spawn's disassembly at 0x020f5504
   is `ldr r0,[pc,#0x40] / bl 0x2043444 / movs r4,r0 / beq / bl 0x20b2adc` --
   r0 holds `this` at the call, so func_ov004_020b2adc gets its argument.  0x169
   needed a ruling because src/func_ov006_020e0574.cpp calls the same base
   constructor with NO argument and rides r0 through, and that callee
   dereferences on its first statement.  This class is on the correct side of
   it and is linked from the slice rather than host-copied. */
static char *g_mem1_self;

extern "C" void *port_mg_memory1_spawn(void)
{
    void *p = (void *)MgMemoryMatch_Spawn();
    g_mem1_self = (char *)p;
    return p;
}

extern "C" void port_scene_memory1_hits(void)
{
    unsigned total = 0, mtotal = 0;
    for (int i = 0; i < 36; ++i) { total += g_mem1_hits[i]; mtotal += g_mem1_base_hits[i]; }

    std::printf("[scene] dScMgMemory_c slot hits: init %u, behavior %u, "
                "render %u, D2 %u, D0 %u, state-reset %u%s\n",
                g_mem1_hits[0], g_mem1_hits[6], g_mem1_hits[9], g_mem1_hits[16],
                g_mem1_hits[17], g_mem1_hits[18],
                g_mem1_render_skipped
                    ? "  [RENDER SLOT NO-OP'd: SM64DS_SCENE_SLOT9=0]" : "");
    if (g_mem1_init_skipped)
        std::printf("[scene] INIT SLOT NO-OP'd: SM64DS_SCENE_SLOT0=0, %u "
                    "time(s)\n", g_mem1_init_skipped);

    std::printf("[scene] dScMgMemory_c own slots entered:");
    for (int i = 0; i < 36; ++i)
        if (g_mem1_hits[i]) std::printf(" %d(x%u)", i, g_mem1_hits[i]);
    std::printf("   (%u total)\n", total);

    std::printf("[scene] dScMgSingle3DBase_c slots entered (this seat's copy, "
                "which lands in this class's OWN derived table -- the flower "
                "row claims the middle table itself, so its copy reads 0 and "
                "this one does not):");
    for (int i = 0; i < 36; ++i)
        if (g_mem1_base_hits[i]) std::printf(" %d(x%u)", i, g_mem1_base_hits[i]);
    std::printf("   (%u total)\n", mtotal);

    /* THE STATE MACHINE'S OWN WITNESS.  Printed whether or not it fired: a run
       with slot hits and zero routed dispatches has ticked the object without
       entering its state machine, and that reads as a success unless it is
       said out loud. */
    {
        unsigned calls = 0, unknown = 0, fcalls = 0, fhits = 0;
        port_mg_dispatch_counts(&calls, &unknown);
        port_mg_memory2_field_counts(&fcalls, &fhits);
        std::printf("[scene] dScMgMemory_c state dispatch: %u routed to one of "
                    "the class's 22 routed table states (of 25 pairs; the "
                    "three in data_ov006_021422a4 have no dispatcher), %u "
                    "bodiless want(s); shared FIELD dispatch %u call(s), %u "
                    "routed; %u framework call(s), %u UNHANDLED address(es)\n",
                    port_mg_memory1_state_hits(),
                    port_mg_memory1_floor_hits(),
                    fcalls, fhits, calls, unknown);
    }

    /* THE FLOOR, PRINTED AS A NUMBER.  func_ov006_020f3e68 is the sixth call
       vtable slot 9 (Render) makes and the only card draw in the class; it has
       no src TU and no delink block, so unmatched/MgMemory1_Faces.cpp holds a
       counting trap in its place.  One entry per render frame is the reading
       that says the seat REACHED the draw and the draw is the hole. */
    std::printf("[scene] dScMgMemory_c CARD DRAW FLOOR: the Render callee "
                "0x020f3e68 is TRAPPED (no src TU, no delink block) and was "
                "entered %u time(s). NO CARD IS DRAWN ON ANY FRAME.\n",
                port_mg_memory1_carddraw_hits());

    /* The two state indexes the ROM's own dispatchers read, at the offsets
       disassembled in unmatched/MgMemory1_StateDispatch.cpp section 2. +0x5314
       is slot 6's and is the one slot 18 zeroes, which cross-confirms both. */
    if (g_mem1_self)
        std::printf("[scene] dScMgMemory_c object at %p, state index +0x5314 "
                    "= %d, +0x5318 = %d\n",
                    (void *)g_mem1_self,
                    *(int *)(g_mem1_self + 0x5314),
                    *(int *)(g_mem1_self + 0x5318));

    /* THE CARD RECORDS, because a run that renders no cards and a run that has
       no cards to render read the same on every other line -- which is exactly
       the lesson port/mg_fanout_costs.txt section 15 paid for on the sibling.
       func_ov006_020f3e68 walks TWELVE records at +0x51a8 with stride 0x18 and
       would draw one sprite per record whose +0x12 byte is nonzero.  The fields
       are read off the ROM: +0x00/+0x04 are the 20.12 screen x/y the draw
       shifts down by 12, +0x10 is the card identity (the row of the seven-by-
       five halfword table at 0x0213d168), +0x15 is the flip frame (its column),
       +0x13 is the in-play gate func_ov006_020f3f10's per-card dispatch reads
       and +0x14 is the per-card state it dispatches ON.
       src/func_ov006_020f4cd8.c is what populates them: records 4..11 for the
       eight-card board, 2..11 for the ten and 0..11 for the twelve, each with
       an identity in 1..4/5/6 twice over, x = 128.0 and y = -128.0 -- off the
       top of the screen, which is where the deal animates them in from. */
    if (g_mem1_self) {
        int live = 0, on_screen = 0, settled = 0;
        int minx = 0x7fffffff, maxx = -0x7fffffff;
        int miny = 0x7fffffff, maxy = -0x7fffffff;
        for (int i = 0; i < 12; ++i) {
            const char *r = g_mem1_self + 0x51a8 + i * 0x18;
            if (*(const unsigned char *)(r + 0x12) == 0) continue;
            ++live;
            int x = *(const int *)(r + 0x00) >> 12;
            int y = *(const int *)(r + 0x04) >> 12;
            if (x < minx) minx = x;
            if (x > maxx) maxx = x;
            if (y < miny) miny = y;
            if (y > maxy) maxy = y;
            if (x > -32 && x < 288 && y > -44 && y < 236) ++on_screen;
            if (*(const unsigned char *)(r + 0x14) == 1) ++settled;
        }
        std::printf("[scene] dScMgMemory_c cards: %d of 12 record(s) live, "
                    "%d inside the screen box, %d settled on a slot", live,
                    on_screen, settled);
        if (live)
            std::printf(", x %d..%d y %d..%d", minx, maxx, miny, maxy);
        std::printf("  (board byte +0x533c = %u)\n",
                    *(const unsigned char *)(g_mem1_self + 0x533c));

        std::printf("[scene] dScMgMemory_c card type.frame.state:");
        for (int i = 0; i < 12; ++i) {
            const char *r = g_mem1_self + 0x51a8 + i * 0x18;
            if (*(const unsigned char *)(r + 0x12) == 0) continue;
            std::printf(" %u.%u.%u", *(const unsigned char *)(r + 0x10),
                        *(const unsigned char *)(r + 0x15),
                        *(const unsigned char *)(r + 0x14));
        }
        std::printf("\n");

        /* WHERE EACH RECORD IS, in DS pixels, so a lane that wants to TAP one
           does not have to infer the record-to-slot mapping from a picture.
           src/func_ov006_020f43c4.c flies record i toward
           data_ov006_0213d09c[+0x533c][i], so record index IS slot index for
           this class -- unlike the sibling, whose dealer walks slots
           separately.  Same >>12 the draw does. */
        std::printf("[scene] dScMgMemory_c card slots (rec:type@x,y):");
        for (int i = 0; i < 12; ++i) {
            const char *r = g_mem1_self + 0x51a8 + i * 0x18;
            if (*(const unsigned char *)(r + 0x12) == 0) continue;
            std::printf(" %d:%u@%d,%d", i, *(const unsigned char *)(r + 0x10),
                        *(const int *)(r + 0x00) >> 12,
                        *(const int *)(r + 0x04) >> 12);
        }
        std::printf("\n");

        /* THE TOUCH STATE.  src/func_ov006_020f4248.c is slot 2 of the arity-1
           table: it compares data_020a0dea/deb against a record's own x/y in a
           +-0x10 by +-0x16 box, refuses once +0x5338 reaches 2, and on a hit
           writes the picked identity to +0x532c+s, the picked record index to
           +0x532e+s and sets that record's state to 3. */
        std::printf("[scene] dScMgMemory_c touch: %u card(s) turned over "
                    "(+0x5338), picks +0x532c/d = %u/%u on records "
                    "+0x532e/f = %u/%u\n",
                    *(const unsigned char *)(g_mem1_self + 0x5338),
                    *(const unsigned char *)(g_mem1_self + 0x532c),
                    *(const unsigned char *)(g_mem1_self + 0x532d),
                    *(const unsigned char *)(g_mem1_self + 0x532e),
                    *(const unsigned char *)(g_mem1_self + 0x532f));
    }

    /* ---- THE BOARD LADDER, AND WHERE IT COMES FROM ------------------------
       The card count is not a property of the scene.  func_ov006_020f51f0
       reads ONE int -- the object's +0xb4 -- and picks 8/10/12:

           add r1,r0,#0x5000 / mov r2,#0 / strb r2,[r1,#0x33c]
           add r2,r0,#0x5300 / mov r3,#4 / strh r3,[r2,#0x2a]   default 0 / 4
           ldr r0,[r0,#0xb4]                THE ONLY INPUT
           cmp r0,#0xa / movge r0,#2 strbge [+0x533c]
                         movge r0,#6 strhge [+0x532a]   >= 10 -> 6 pairs, 12
           cmp r0,#5   / movge r0,#1 strbge [+0x533c]
                         movge r0,#5 strhge [+0x532a]   >=  5 -> 5 pairs, 10

       and +0xb4 is SEEDED in exactly one place, InitResources
       (src/func_ov006_020f53e4.cpp):  *(int*)(self+0xb4) = func_ov004_020ad878()
       which is data_0209caf4[minigame index][1] -- the PERSISTENT per-minigame
       record.  (Vtable slot 1, func_ov004_020b0930, ZEROES +0xb4 before slot 0
       runs; say "the only writer of the seed" rather than "no other writer",
       which is the correction port/mg_fanout_costs.txt section 17 records.)
       So the board size is saved progress, and a fresh record -- zero, what
       SaveData::SetDefaultValuesMg leaves -- is the eight-card board.

       The round end writes it back: src/func_ov006_020f4888.c does
       +0xb4 +/- 1 on a cleared or failed board and mirrors the maximum into
       +0xb8, and src/func_ov006_020f3c90.c calls
       func_ov004_020ad79c(score, +0xb4 +/- 1) through to the record.  This
       block prints both ends so a run can say which of them moved.

       THE THRESHOLDS ARE THE SIBLING'S, THE BOARDS ARE NOT.  Memory Master
       steps 16 -> 18 -> 20 at levels 5 and 10; this class steps 8 -> 10 -> 12
       at the same two levels, through the same save-record mechanism. */
    if (g_mem1_self) {
        const int idx = port_mg1_record_index();
        const unsigned pairs = *(const unsigned short *)(g_mem1_self + 0x532a);
        std::printf("[scene] dScMgMemory_c difficulty: +0xb4 LEVEL = %d, "
                    "+0xb8 best = %d, +0xa8 score = %d  ->  +0x533c = %u, "
                    "+0x532a pairs needed = %u (%u cards)\n",
                    *(const int *)(g_mem1_self + 0xb4),
                    *(const int *)(g_mem1_self + 0xb8),
                    *(const int *)(g_mem1_self + 0xa8),
                    *(const unsigned char *)(g_mem1_self + 0x533c),
                    pairs, pairs * 2u);
        std::printf("[scene] dScMgMemory_c progress: %u pair(s) found "
                    "(+0x5337), %u miss(es) of %u (+0x533a/+0x533b), timer "
                    "+0x5324 = %u\n",
                    *(const unsigned char *)(g_mem1_self + 0x5337),
                    *(const unsigned char *)(g_mem1_self + 0x533a),
                    *(const unsigned char *)(g_mem1_self + 0x533b),
                    *(const unsigned short *)(g_mem1_self + 0x5324));
        if (idx >= 0 && idx < 36) {
            const int *rec = (const int *)(data_0209caf4 + idx * 20);
            std::printf("[scene] dScMgMemory_c save record data_0209caf4[%d] "
                        "= { %d, %d, %d, %d, %d }  (field 1 IS the level "
                        "+0xb4 was seeded from; Memory Master is record 29 and "
                        "this class is record 1)\n", idx,
                        rec[0], rec[1], rec[2], rec[3], rec[4]);
        } else {
            std::printf("[scene] dScMgMemory_c save record: NO minigame index "
                        "(data_ov004_020beb68 = %p)\n", data_ov004_020beb68);
        }
    }
    std::fflush(stdout);
}
