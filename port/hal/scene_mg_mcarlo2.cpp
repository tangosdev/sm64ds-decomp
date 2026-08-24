// dScMgMCarlo2_c, "Pair-a-Gone And On" -- actor id 0x17e, scene 382. The
// ENDLESS-MODE sequel to scene 381's Pair-a-Gone, seated by run mg9 lane S381.
// Run mg11, lane PGO.
//
// Read port/slice_pgo.txt for the identity derivation (the ROM's own RTTI, the
// ov005 launch row and the BMG title, all re-derived here rather than inherited
// from the sibling), the five width checks, the attribution finding and the
// closure. Read port/mg_fanout_costs.txt for what the family costs. This file
// is the seat: the three tables' faces, the fill, the factory forwarder and the
// tick witness.
//
// ---- 1. WHY THIS IS A SEPARATE FILE FROM hal/scene_mg.cpp -----------------
//
// scene_mg.cpp is the FAMILY half: the mounts, the thirty-five overlay
// constructors, dScMgBase_c's address-keyed faces and the gate that keeps the
// constructors off a level boot. None of it is re-derived here. This file
// reaches it through the one seam that file exports,
// port_scene_mg_fill_shared(), and adds only what is this class's. The
// alternative -- another face array appended inside scene_mg.cpp -- is what a
// nine-lane fan-out would collide on.
//
// ---- 2. THE HIERARCHY IS THREE DEEP, AND THE ROM SAYS SO THREE TIMES ------
//
//     Scene -> dScMgBase_c         data_ov004_020bc0c0  36 slots
//           -> dScMgSingle3DBase_c data_ov006_0213e448  36 slots
//           -> dScMgMCarlo2_c      data_ov006_0213d7e8  36 slots
//
// The middle edge is stated by the ROM three separate ways, the same three the
// sibling seat had:
//
//   TYPEINFO. The word before this class's vtable, 0x0213d7e4, points at the
//     typeinfo record at 0x0213d714, whose third word is 0x0213bc64 --
//     dScMgSingle3DBase_c's OWN typeinfo. Its name pointer 0x0213d730 reads
//     "14dScMgMCarlo2_c".
//   FACTORY. MgPairAGoneAndOn_Spawn at 0x020fa6ac calls func_ov004_020b2adc
//     (dScMgBase_c's constructor), writes 0x0213e448 into the object's first
//     word (str r1,[r4] at 0x020fa6d4, pool word 0x020fa728), then writes
//     0x0213d7e8 over it (str r1,[r4] at 0x020fa6e8, pool word 0x020fa730).
//   DESTRUCTORS. Slot 17 (func_ov006_020f8f68, the D0) and its unmarked D2
//     sibling slot 16 (func_ov006_020f8ef4) both unwind them in the opposite
//     order, 0x0213d7e8 first and 0x0213e448 second.
//
// ---- 3. THE FILL IS ADDRESS-KEYED, AND THIS IS THE FIFTH CLAIM ON THE
//         MIDDLE TABLE -----------------------------------------------------
//
// port_scene_fill_rom and mc2_apply both key on the ROM WORD a slot holds, not
// on the slot number, so the same face array can be applied to every table in
// the chain and can only land where the ROM parked that exact body. The three
// arrays below are disjoint by construction:
//
//   kSingle3DFaces  the eight bodies dScMgSingle3DBase_c overrides
//                   (slots 2, 5, 7, 10, 16, 17, 26, 33)
//   kMCarlo2Faces   the eight dScMgMCarlo2_c adds or replaces on top
//                   (slots 0, 3, 6, 9, 16, 17, 18, 19)
//   kCard2Faces     dMgMCarlo2CardObj_c's two, section 4b
//
// Slots 16 and 17 appear in the first two and hold DIFFERENT addresses --
// 0x0210a4b0 / 0x0210a4e8 in the middle table, 0x020f8ef4 / 0x020f8f68 in the
// derived one -- so both arrays can be applied to both tables and each writes
// only its own.
//
// FOUR OTHER SEATED FILES DEFINE THEIR OWN COPY OF kSingle3DFaces:
// hal/scene_mg_flower.cpp, hal/scene_mg_memory2.cpp, hal/scene_mg_luckystars.cpp
// and hal/scene_mg_mcarlo.cpp. That is safe for the reason
// hal/scene_mg_memory2.cpp section 3 measures: mg_apply keys on a DS address,
// every row's fill runs on every boot, and the fill that runs FIRST claims the
// table -- the later ones find nothing left to write. This class's registry row
// is appended after all four, so the flower keeps the middle table and every
// earlier witness keeps counting exactly what it counted before this seat
// existed. This seat's own middle copy therefore reports zero on any tree that
// carries the flower row, and that zero is the CHECK rather than a
// disappointment.
//
// THIS IS THE FIFTH COPY. The promotion argument hal/scene_mg_mcarlo.cpp
// section 3 records at four is not taken here either, and for the same
// deliberate reason: it touches four files other mg11 lanes may be editing in
// parallel, and the run's merge plan is a serial merge of nine branches. The
// count is recorded so the lane that does it has a number rather than an
// intention.
//
// ---- 4. SLOT 2 IS NOT src's BODY, AND SLOT 33 IS NOT src's EITHER ---------
//
// Both substitutions are inherited unchanged from the family and neither is
// re-derived here:
//
// func_ov006_0210a6e4 (AfterInitResources) drops the framework's second
// argument -- the ROM never writes r1 before its `bl 0x20b08f0`, so the flags
// ride through in r1 and src spells the call with one argument because that is
// the only way to spell an unnamed value in C. The repair is
// port_mg_flower_after_init in port/unmatched/MgFlower_Slot2.cpp, whose header
// asks the next lane to call it rather than write a second one; this is the
// fifth lane to do so. src/func_ov006_0210a6e4.cpp stays out of
// port/slice_pgo.txt for that reason.
//
// func_ov006_0210a708 (slot 33, the once-per-boot 3D setup) ends with two
// stores to LIGHT_COLOR at 0x040004cc, which a plain src build lands in the
// memory ntr maps across the I/O window without ever telling the geometry
// engine -- the "Yoshi is a black silhouette" defect run mg5 lane YTEX
// measured. It builds from the hostgen'd copy port/CMakeLists.txt's
// FLW_HOSTGEN_SYMS already emits, and src/func_ov006_0210a708.c is out of this
// slice for the same reason.
//
// THIS CLASS'S OWN SLOT 0 WRITES TWO PPU REGISTERS STRAIGHT FROM src AND THAT
// IS CORRECT. func_ov006_020fa56c ends with a read-modify-write of the 16-bit
// words at 0x04000008 and 0x0400100a (BG0CNT on engine A and BG1CNT on engine
// B). Those are plain STATE registers -- the host's ntr memory maps the I/O
// window and a later reader sees the value -- and they are not the
// WRITE-TRIGGERED class hostgen's NTR_MMIO proxy exists for (the geometry
// engine, LIGHT_COLOR). dScMgFlower_c's and dScMgMCarlo_c's own slot 0 write
// the same class of register from src for the same reason. This paragraph
// exists so the next reader does not read the absence of a substitution as an
// oversight.
//
// ---- 4b. THERE IS A THIRD TABLE AND THE VTABLE AXIS CANNOT SEE IT ---------
//
// The factory builds an array of 0x28 records of 0x30 bytes at this+0x51a8
// through func_020733a8 with the element constructor func_ov006_020fa740,
// whose whole body is
//
//     p[0] = (int)&data_ov006_0213d728;  p[1] = 0;  p[2] = 0;
//
// so every one of the FORTY records carries a MOUNTED ROM TABLE as its vptr.
// data_ov006_0213d728 is dMgMCarlo2CardObj_c's vtable -- the typeinfo pointer
// at 0x0213d724 reads 0x0213d704, whose name string at 0x0213d758 is
// "19dMgMCarlo2CardObj_c" -- and it is TWO slots wide:
//
//     [0]  0x020f98dc   the card DRAW
//     [1]  0x020f9994   the card's per-frame update, which takes an event
//
// AND BOTH SLOTS ARE DISPATCHED. Slot 9 (src/func_ov006_020f9ffc.cpp) walks the
// list at data_ov006_02142578 twice and calls `n->m0()` on each node --
// `ldr r1,[r0] / ldr r1,[r1] / blx r1` at 0x020fa0b4 and 0x020fa0f4 in the ROM.
// The pair matcher func_ov006_020f9000, which slot 6 calls on every frame,
// calls `head->m1(i)` -- `ldr r2,[r0] / ldr r2,[r2,#4] / blx r2` at 0x020f9400
// and 0x020f948c. Nothing else in ov006 ever constructs an object with this
// vptr: the only two relocations naming data_ov006_0213d728 in the whole
// overlay are this class's element constructor (0x020fa758) and its element
// reset (0x020f8ffc).
//
// SO A SEAT THAT FILLED ONLY THE TWO SCENE TABLES WOULD LINK, BOOT AND JUMP TO
// A RAW DS ADDRESS ON THE FIRST RENDERED FRAME. It is the check run mg9 lane
// S381 turned into a family rule -- ask what the ROM parks in the first word of
// every object the factory builds -- and this class is the second instance of
// it.
//
// THE WIDTH IS 2, CHECKED THE SAME WAY THE 36s ARE: the span from
// data_ov006_0213d728 to the next config symbol data_ov006_0213d730 is 8 bytes
// = 2 words; exactly 2 load relocations fall inside that span; and the word
// past the end is 0x53643431, the first four bytes of the RTTI string
// "14dScMgMCarlo2_c", not a code address.
//
// ---- 5. THE WIDTH IS 36 ON BOTH SCENE TABLES, CHECKED FIVE WAYS -----------
//
// port/mg_fanout_costs.txt section 11 measured that twelve of twenty-nine
// widths in its own table were phantoms, and that a 37-slot fill writes a host
// thunk over live ROM data belonging to another class. FOR THIS CLASS THE
// PHANTOM IS REAL AND ITS VICTIM IS NAMED, so the checks are not ceremony:
//
//   1. SPAN. data_ov006_0213d7e8 -> next config symbol data_ov006_0213d878.
//      0x90 bytes = 36 words. (An UPPER bound only; it refuses a width and
//      never grants one.)
//   2. TERMINAL SLOT. Slot 35 (0x0213d874) holds 0x020ad660, which every
//      dScMgBase_c-derived table holds.
//   3. WHAT THE WORD PAST THE END IS. Index 36 reads 0x020fb4e0 -- A CODE
//      ADDRESS, which is exactly the shape that fools this check. It is an
//      mwcc {code, 0} member-pointer pair, and section 6 below names what owns
//      it.
//   4. RELOCATION COUNT. Exactly 36 load relocations fall inside
//      [0x0213d7e8, 0x0213d878) and none lands on index 36's second word. A
//      vtable slot is a relocated word by construction.
//   5. RELOCATION STRIDE, run mg9 lane LKY's check. The relocation sources step
//      by 4 for exactly 36 words from 0x0213d7e8 and then jump to 8 at
//      0x0213d878 -- the stride change IS the table ending, and it lands on the
//      same boundary as checks 1 and 4.
//
// ---- 6. THE WORD PAST THE END BELONGS TO A DIFFERENT MINIGAME, AND
//         ADDRESS ADJACENCY WOULD HAVE MIS-ATTRIBUTED IT -------------------
//
// THE LANE'S OWN NEAR MISS, written down because the trap is the family's
// standing one (run mg9 lane LKY caught the same shape one class over). A run
// of TWENTY-FIVE {code, 0} pairs starts at 0x0213d878, immediately after this
// vtable, and __sinit_ov006_02131a38 copies it into five .bss dispatch tables
// at 0x02142604, 0x02142624, 0x02142644, 0x0214266c and 0x02142694. Read by
// address adjacency it is this class's state machine. It is not:
//
//   - EVERY code word in those pairs lies in 0x020fa9c8..0x020fea54, which is
//     PAST this class's code block (0x020f8ef4..0x020fa75c).
//   - The five .bss tables are read from 0x020fad8c, 0x020fb66c, 0x020fc840,
//     0x020fdaec and 0x020fe2b8 -- five dispatchers, all in that same later
//     block.
//   - MgBobOmbSquad's own vtable 0x0213d9cc holds 0x020fa75c and 0x020fa780 in
//     slots 16 and 17, which places the start of ITS code block exactly where
//     this class's ends. The pairs, the tables and the dispatchers are actor
//     0x170's.
//   - AND THE MEASUREMENT THAT SETTLES IT: not one relocation anywhere in
//     ov006 whose source lies in the .init section (0x0212f4c4..0x0213356c)
//     targets this class's code block, and the only eleven .data relocations
//     that point into it are the SpawnInfo word, the element vtable's two and
//     the scene vtable's eight. There is no constructor of this class's for
//     stategen to read.
//
// A SEAT THAT TOOK THE PAIRS WOULD HAVE HOST-COPIED ANOTHER MINIGAME'S
// DISPATCHERS, and a 37-slot fill would have written a host thunk over the
// first word of Bob-omb Squad's state table. sinit is the ground truth for slot
// ownership and the code block is the ground truth for class ownership;
// neither is address order.
//
// ---- 7. THE ROW GOES LAST, AND THE ORDER IS LOAD-BEARING ------------------
//
// port_scene_registry_install walks port_scene_classes[] in table order and
// calls every row's fill on every boot; port_scene_mg_overlay_load runs the
// thirty-five constructors ONCE PER PROCESS from the tail of the first minigame
// row's fill. Appending therefore means the constructors have already run with
// clean ROM words when this fill starts, which is the latent-safe direction
// section 11 names. Here it is a rule obeyed rather than a hazard relied on:
// section 6's measurement says no ov006 constructor reads any word of this
// class's code block, and the width checks say the fill cannot reach past its
// own table into the pairs that DO get copied.
//
// ---- 8. THIS CLASS HAS NO POINTER-TO-MEMBER WALL, MEASURED FIVE WAYS ------
//
// port/mg_fanout_costs.txt section 4 opens "THE WALL, AND IT IS THE SAME ONE
// FOR ALL THIRTY". It is not this one's, and every prescribed detector was run
// rather than one:
//
//   - THE SOURCE, READ BEFORE THE FACE ARRAY WAS WIRED. That ordering is
//     section 14's practical rule. src/func_ov006_020fa13c.c (slot 6) is a
//     plain `switch (*(s16*)(c + 0x5928))` with cases 1, 2 and 3;
//     src/func_ov006_020fa3d0.c (slot 19) continues the same index with cases
//     4, 5 and 6.
//   - THE ROM. 0x020fa148 is add r0,r4,#0x5900 / ldrsh r0,[r0,#0x28] / cmp #1
//     beq / cmp #2 beq / cmp #3 beq / b default, and 0x020fa3d8 is the same
//     shape for 4, 5 and a dead `cmp r0,#6` whose branch is unconditional --
//     the compiler's spelling of `case 6: default:`. Neither is the
//     five-instruction ARM Itanium sequence.
//   - THE ENCODING SCAN, run mg9 lane LKY's detector, over every instruction of
//     all twenty-five bodies in this class's code block: ZERO `tst Rn,#1`
//     sites. The eleven `asr #1x` hits are all Fix12 fractional maths (asr #12)
//     or literal-pool words, and none is the `>> 1` / `& 1` pair decode that is
//     the third shape.
//   - EVERY INDIRECT CALL IS ACCOUNTED FOR. There are exactly five `blx <reg>`
//     sites in the block and all five are ordinary vtable dispatches: two on
//     the card table's slot 1 (0x020f9400, 0x020f948c), two on its slot 0
//     (0x020fa0b4, 0x020fa0f4) and one on this object's own slot 18
//     (0x020fa664, section 9).
//   - A `::*` SWEEP over all twenty-five TUs in this slice is clean.
//   - AND THE RUN IS THE ONE THAT CONVICTS. Section 4 is explicit that a link
//     and a source sweep between them cannot license the word "none". Scene 382
//     boots under SM64DS_FAULTS_FATAL=1 with zero UNHANDLED addresses and no
//     eip on a DS address; port/slice_pgo.txt carries the numbers.
//
// The FRAMEWORK's wall is still the framework's and is still paid: this class
// reaches func_ov004_020b87e0 through dScMgBase_c exactly as curling does, and
// unmatched/MgBase_StateSetter.cpp's eighty routed addresses are inherited.
//
// ---- 9. THE RIDE-THROUGH IS LOAD-BEARING ON SLOTS 18 AND 19 ---------------
//
// Slot 0's own tail dispatches slot 18 through the object's vtable at offset
// 0x48 with `mvn r1,#0` -- one argument, value -1 -- at 0x020fa664, and
// src/func_ov006_020fa56c.cpp spells it `((Base *)c)->m48(-1)` over a
// nineteen-virtual shim so MSVC emits the same slot. The ROM body
// func_ov006_020fa4d4 never reads its r1. A thunk declared (void*, void*)
// compiles to a bare ret, leaks those four bytes, and the caller's own `ret`
// then takes a garbage return address -- the fault lane BASESET repaired across
// the family. runs/mg5/out/baseset/slot18_19_scan.txt is the census: 22 slot-18
// sites and 14 slot-19 sites, argument count ONE at every one of them. Both
// faces below therefore carry the unnamed third parameter.
//
// ---- 10. WHAT THIS SEAT DOES NOT CLAIM ------------------------------------
//
//   - NOBODY HAS LOOKED AT A FRAME NEXT TO A DS. The census below reports what
//     the class does; it cannot say the picture is right. Section 16's "the
//     zeros that are honest and the zeros that are not" applies unchanged.
//   - THE SAVE RECORD IS THE PORT'S USUAL GAP. The ov005 row gives this scene
//     save record 25, and the port has no backup cart, so
//     SaveData::SetDefaultValuesMg zeroes the record on every boot. That is a
//     save-medium gap, not a minigame gap, and it is the same one section 17
//     records for Memory Master.
//   - TOUCH IS UNMEASURED HERE. The class is a stylus card game; nothing in a
//     headless boot drives a pick.

#include "hal/screen_gap.h"

#include <cstdio>
#include <cstdlib>

extern "C" {

/* the seam into hal/scene_mg.cpp -- mounts, arm9 fill, dScMgBase_c's faces */
unsigned port_scene_mg_fill_shared(void **vt, unsigned n);
void     port_scene_mg_prepare(int id);
int      port_scene_env_want(void);

/* The mount storage the fill writes into. All three tables are the ov006
   mount's own (the base table is ov004's), so the fill replaces the mounted
   words rather than building a fresh host array -- the ov007 and curling
   treatment: a second host array of the same name is a duplicate symbol, and
   leaving the mounted table alone leaves live wild DS pointers in a table the
   factory installs. */
extern unsigned char data_ov004_020bc0c0[];   /* dScMgBase_c,          36 */
extern unsigned char data_ov006_0213e448[];   /* dScMgSingle3DBase_c,  36 */
extern unsigned char data_ov006_0213d7e8[];   /* dScMgMCarlo2_c,       36 */
extern unsigned char data_ov006_0213d728[];   /* dMgMCarlo2CardObj_c,   2 */
extern unsigned char MgPairAGoneAndOn_SpawnInfo[];  /* the SpawnInfo record */

/* dScMgSingle3DBase_c's eight overrides. Slot 2 is NOT src's body and slot 33
   is the hostgen'd copy: see section 4. */
int   port_mg_flower_after_init(void *c, unsigned f);   /* slot  2 */
void  func_ov006_0210a608(void *c, unsigned f);         /* slot  5 */
int   func_ov006_0210a698(void *c);                     /* slot  7 */
int   func_ov006_0210a664(void *c);                     /* slot 10 */
int   func_ov006_0210a4b0(char *c);                     /* slot 16 D2 */
int   func_ov006_0210a4e8(char *c);                     /* slot 17 D0 */
int   func_ov006_0210a600(void);                        /* slot 26 */
void  func_ov006_0210a708(char *c);                     /* slot 33 */

/* dScMgMCarlo2_c's own eight */
int   func_ov006_020fa56c(char *c);           /* slot  0 InitResources     */
int   func_ov006_020f9fe0(void *c);           /* slot  3 CleanupResources  */
int   func_ov006_020fa13c(void *c);           /* slot  6 Behavior          */
int   func_ov006_020f9ffc(char *c);           /* slot  9 Render            */
void *func_ov006_020f8ef4(char *c);           /* slot 16 D2                */
void *func_ov006_020f8f68(char *c);           /* slot 17 D0                */
void  func_ov006_020fa4d4(char *c);           /* slot 18 state reset       */
int   func_ov006_020fa3d0(char *c);           /* slot 19 states 4..6       */

/* dMgMCarlo2CardObj_c's two, section 4b */
void  func_ov006_020f98dc(char *thiz);        /* slot  0 the card draw   */
void  func_ov006_020f9994(char *thiz, int event);  /* slot 1 the update  */

/* the factory */
void *MgPairAGoneAndOn_Spawn(void);

/* ---- THE LOOP TRACE, SM64DS_MC2_TRACE=1 (run mg12 lane CRD) -------------
   Diagnostic only. Every global the class's state-3 loop reads or writes,
   printed once a frame, so a deadlock can be READ rather than reasoned about.
   Names are the plain C spellings the mount defines; the widths are the ones
   the ROM's own ldr/ldrsh/ldrb use at each address. */
extern int           data_ov006_0213d700;     /* the dealt-card sweep, Fix12 */
extern short         data_ov006_0213d6f8;     /* board height (cards)        */
extern short         data_ov006_0213d6f4;     /* cards left                  */
extern int           data_ov006_0213d6fc;     /* the "input armed" flag      */
extern short         data_ov006_0214255c;     /* the clear animation counter */
extern short         data_ov006_02142558;     /* the pair timer              */
extern short         data_ov006_02142560;
extern short         data_ov006_02142564;
extern char         *data_ov006_02142568;
extern char         *data_ov006_0214256c;
extern char         *data_ov006_02142570;     /* first selected card         */
extern char         *data_ov006_02142574;     /* second selected card        */
extern char         *data_ov006_02142578;
extern char         *data_ov006_0214257c;     /* the live card list head     */
extern int           data_ov006_02142580[];   /* the ten face weights        */
extern unsigned char data_020a0de8[];         /* stylus: touched             */
extern unsigned char data_020a0de9[];         /* stylus: edge                */
extern unsigned char data_020a0dea[];         /* stylus: x                   */
extern unsigned char data_020a0deb[];         /* stylus: y                   */
extern int           data_020a0e40[];         /* the local player index      */
int  func_ov006_020f95f0(void);               /* "the board has settled"     */
int  func_ov006_020f96e0(void);               /* "a matching pair exists"    */

/* the framework's dispatch witness, from unmatched/MgBase_StateDispatch.cpp */
void  port_mg_dispatch_counts(unsigned *calls, unsigned *unknown);

/* The persistent minigame record this class's progress is keyed to. Nothing new
   is defined here: the storage is hal/level_boot.cpp's .dsstate$savblk0004 and
   the index function is src/func_ov004_020adc3c.c. Named so the census can
   print the record rather than leave it to be inferred. */
extern void         *data_ov004_020beb68;     /* the live dScMgBase_c `this` */
extern unsigned char data_0209caf4[];         /* 36 records x 20 bytes       */
int   func_ov004_020adc3c(void *c);           /* (self->field_8 >> 8) & 0xff */

}  /* extern "C" */

static int port_mg2_record_index(void)
{
    return data_ov004_020beb68 ? func_ov004_020adc3c(data_ov004_020beb68) : -1;
}

// ---- the tick witness ------------------------------------------------------
//
// One counter per dispatched slot on each of the two scene tables this file
// owns faces for. The instrument every scene seat in this port carries, for the
// reason hal/scene_mg.cpp gives: an object that EXISTS and an object that RUNS
// look identical from outside. The framework slots dScMgBase_c keeps are
// counted by scene_mg.cpp's own array and printed by its atexit report.
static unsigned g_mc2_hits[36];
static unsigned g_mc2_base_hits[36];   /* the same slots on the MIDDLE table */

#define MC2(n)   (++g_mc2_hits[(n)])
#define M3D(n)   (++g_mc2_base_hits[(n)])

/* ---- dScMgSingle3DBase_c's eight ---------------------------------------- */
static void *__fastcall s3_ainit(void *s, void *, unsigned f)
{ M3D(2);  return (void *)(size_t)port_mg_flower_after_init(s, f); }
static void __fastcall s3_aclean(void *s, void *, unsigned f)
{ M3D(5);  func_ov006_0210a608(s, f); }
static int  __fastcall s3_bbeh(void *s, void *)
{ M3D(7);  return func_ov006_0210a698(s); }
static int  __fastcall s3_bren(void *s, void *)
{ M3D(10); return func_ov006_0210a664(s); }
static void *__fastcall s3_d2(void *s, void *)
{ M3D(16); return (void *)(size_t)func_ov006_0210a4b0((char *)s); }
static void *__fastcall s3_d0(void *s, void *)
{ M3D(17); return (void *)(size_t)func_ov006_0210a4e8((char *)s); }
static int  __fastcall s3_v26(void *, void *)
{ M3D(26); return func_ov006_0210a600(); }
static int  __fastcall s3_v33(void *s, void *)
{ M3D(33); func_ov006_0210a708((char *)s); return 0; }

/* ---- dScMgMCarlo2_c's own eight ----------------------------------------- */
static int  __fastcall mc2_init(void *s, void *)
{ MC2(0);  const int r = func_ov006_020fa56c((char *)s);
  /* the GaplessMinigames latch, for hal/scene_mg.cpp's reason: every seated
     minigame calls it so the ones the gapless table does not name can say
     "unsupported" instead of doing nothing quietly. */
  hal_gapless_minigames_latch(); return r; }
/* SLOT 3 IS NOT IN THE SIBLING'S SET. dScMgMCarlo_c overrides seven slots and
   this class overrides eight; the extra one is CleanupResources, whose base
   word is arm9's 0x02043bf0. The face takes the shape hal/scene_boot.cpp's
   sc_base_clean and hal/scene_mg_wiggler.cpp's wig_clean take for the same slot
   -- (void *, void *), no ride-through -- because that is the shape every
   existing caller of slot 3 in this port already uses. */
static int  __fastcall mc2_clean(void *s, void *)
{ MC2(3);  return func_ov006_020f9fe0(s); }
/* SM64DS_MC2_TRACE=1: the state-3 loop, once a frame. Section CRD. */
static int  g_mc2_trace = -1;
static void mc2_trace(const char *when, const char *self)
{
    if (g_mc2_trace < 0) {
        const char *e = std::getenv("SM64DS_MC2_TRACE");
        g_mc2_trace = (e && *e && *e != '0') ? 1 : 0;
    }
    if (!g_mc2_trace) return;
    const int idx = data_020a0e40 ? (int)((unsigned char *)data_020a0e40)[0] : 0;
    std::printf("[mc2] %-3s tick %u state %d step %d sweep %d (%d) "
                "h6f8 %d left6f4 %d armed6fc %d clr255c %d timer2558 %d "
                "sel %p/%p head %p settled %d pair %d "
                "touch %u/%u @%u,%u\n",
                when, g_mc2_hits[6],
                self ? *(const short *)(self + 0x5928) : -1,
                self ? *(const short *)(self + 0x592e) : -1,
                data_ov006_0213d700, data_ov006_0213d700 >> 12,
                (int)data_ov006_0213d6f8, (int)data_ov006_0213d6f4,
                data_ov006_0213d6fc, (int)data_ov006_0214255c,
                (int)data_ov006_02142558,
                (void *)data_ov006_02142570, (void *)data_ov006_02142574,
                (void *)data_ov006_0214257c,
                func_ov006_020f95f0(), func_ov006_020f96e0(),
                (unsigned)data_020a0de8[idx * 4], (unsigned)data_020a0de9[idx * 4],
                (unsigned)data_020a0dea[idx * 4], (unsigned)data_020a0deb[idx * 4]);
    /* the whole card array, so a tap plan can be computed from a run */
    if (self && when[0] == 'i' && (g_mc2_hits[6] % 5) == 0) {
        for (int i = 0; i < 0x28; ++i) {
            const char *r = self + 0x51a8 + i * 0x30;
            std::printf("[mc2]     card %d state %u key %d face %u pos %d,%d "
                        "vis %u\n", i,
                        (unsigned)(unsigned char)r[0x2d],
                        (int)*(const short *)(r + 0x2a),
                        (unsigned)(unsigned char)r[0x2c],
                        *(const int *)(r + 0x0c) >> 12,
                        *(const int *)(r + 0x10) >> 12,
                        (unsigned)(unsigned char)r[0x2e]);
        }
    }
    std::fflush(stdout);
}

static int  __fastcall mc2_beh(void *s, void *)
{ MC2(6);  mc2_trace("in", (const char *)s);
  const int r = func_ov006_020fa13c(s);
  mc2_trace("out", (const char *)s); return r; }
static int  __fastcall mc2_render(void *s, void *)
{ MC2(9);  return func_ov006_020f9ffc((char *)s); }
static void *__fastcall mc2_d2(void *s, void *)
{ MC2(16); return func_ov006_020f8ef4((char *)s); }
static void *__fastcall mc2_d0(void *s, void *)
{ MC2(17); return func_ov006_020f8f68((char *)s); }
/* the two ride-through faces, section 9 */
static int  __fastcall mc2_reset(void *s, void *, int /*ridethrough*/)
{ MC2(18); func_ov006_020fa4d4((char *)s); return 1; }
static int  __fastcall mc2_v19(void *s, void *, int /*ridethrough*/)
{ MC2(19); return func_ov006_020fa3d0((char *)s); }

/* ---- dMgMCarlo2CardObj_c's two, section 4b ------------------------------ */
/* These are NOT __fastcall thunks over a Scene slot: the ROM dispatches them
   through the card record's own two-slot vtable with `this` in r0 and, for slot
   1, one further value in r1, so the host shape is __thiscall -- one hidden
   receiver plus one stack argument. The counters are here for section 15's
   reason and not for symmetry: the draw is the only code in this class that
   puts a card pixel anywhere, so a run that ticks the board and never enters it
   looks identical from every other census line in this file. */
static unsigned g_mc2_card_draws, g_mc2_card_updates;
static void __fastcall card_draw(void *s, void *)
{ ++g_mc2_card_draws; func_ov006_020f98dc((char *)s); }
static void __fastcall card_update(void *s, void *, int event)
{ ++g_mc2_card_updates; func_ov006_020f9994((char *)s, event); }

/* SM64DS_SCENE_SLOT0=0 and SM64DS_SCENE_SLOT9=0, the diagnostics every scene
   seat in this port carries, counted separately so a run can never read a
   no-op as the real body having run. */
static unsigned g_mc2_init_skipped, g_mc2_render_skipped;
static int __fastcall mc2_init_noop(void *, void *)
{ ++g_mc2_init_skipped; return 1; }
static int __fastcall mc2_render_noop(void *, void *)
{ ++g_mc2_render_skipped; return 1; }

struct Mc2Face { unsigned ds; void *host; };

static const Mc2Face kSingle3DFaces[] = {
    {0x0210a6e4u, (void *)s3_ainit},  {0x0210a608u, (void *)s3_aclean},
    {0x0210a698u, (void *)s3_bbeh},   {0x0210a664u, (void *)s3_bren},
    {0x0210a4b0u, (void *)s3_d2},     {0x0210a4e8u, (void *)s3_d0},
    {0x0210a600u, (void *)s3_v26},    {0x0210a708u, (void *)s3_v33},
};

/* A SEPARATE ARRAY rather than two more rows in kMCarlo2Faces, because it is
   applied to a DIFFERENT table and the address-keyed fill would otherwise be
   free to write a card body into a scene table if the ROM ever parked one
   there. Keeping the arrays per-table is what makes "this array wrote n slots"
   a checkable number. */
static const Mc2Face kCard2Faces[] = {
    {0x020f98dcu, (void *)card_draw},  {0x020f9994u, (void *)card_update},
};

static const Mc2Face kMCarlo2Faces[] = {
    {0x020fa56cu, (void *)mc2_init},   {0x020f9fe0u, (void *)mc2_clean},
    {0x020fa13cu, (void *)mc2_beh},    {0x020f9ffcu, (void *)mc2_render},
    {0x020f8ef4u, (void *)mc2_d2},     {0x020f8f68u, (void *)mc2_d0},
    {0x020fa4d4u, (void *)mc2_reset},  {0x020fa3d0u, (void *)mc2_v19},
};

static unsigned mc2_apply(void **vt, unsigned n, const Mc2Face *f, unsigned nf)
{
    unsigned hit = 0;
    for (unsigned i = 0; i < n; ++i) {
        const unsigned ds = (unsigned)(size_t)vt[i];
        for (unsigned k = 0; k < nf; ++k)
            if (f[k].ds == ds) { vt[i] = f[k].host; ++hit; break; }
    }
    return hit;
}

/* Words still holding a DS address. A minigame table is 36 slots and every one
   of them is dispatched by something, so a nonzero answer is a wild call
   waiting to happen and the seat says so out loud. */
static unsigned mc2_raw_left(void **vt, unsigned n)
{
    unsigned left = 0;
    for (unsigned i = 0; i < n; ++i) {
        const unsigned w = (unsigned)(size_t)vt[i];
        if (w >= 0x02000000u && w < 0x02400000u)
            ++left;
    }
    return left;
}

/* How many slots this seat's own copies actually claimed, so section 3's
   "the first fill wins" is a measured statement rather than an assertion. */
static unsigned g_mc2_mid_claimed, g_mc2_vt_claimed, g_mc2_card_claimed;

extern "C" void port_scene_mcarlo2_hits(void);

extern "C" void port_scene_fill_mcarlo2(void)
{
    void **base = (void **)data_ov004_020bc0c0;
    void **mid  = (void **)data_ov006_0213e448;
    void **vt   = (void **)data_ov006_0213d7e8;

    /* THE BASE TABLE IS FILLED HERE TOO AND IT IS NOT CEREMONY. Earlier rows'
       fills already did it and run first, so on a tree carrying them this is a
       second pass over words that are already host pointers and finds nothing.
       It is here so this class does not depend on another class's row
       existing: the factory's first act is func_ov004_020b2adc, which writes
       data_ov004_020bc0c0 into the object's first word before either derived
       table lands, and thirty-six raw DS words in a table the ROM installs is
       what produced the ov007 lane's wild-execute fault. */
    port_scene_mg_fill_shared(base, 36);

    /* THE MIDDLE TABLE. Live storage in its own right -- the destructor chain
       writes it into the object on the way down, so a slot dispatched between
       the two vptr writes reads it. Section 3 is why applying this file's own
       arrays here is safe beside the four earlier seats', and why the claimed
       count below is expected to be ZERO on a tree carrying them. */
    port_scene_mg_fill_shared(mid, 36);
    g_mc2_mid_claimed =
        mc2_apply(mid, 36, kSingle3DFaces,
                  sizeof kSingle3DFaces / sizeof kSingle3DFaces[0])
      + mc2_apply(mid, 36, kMCarlo2Faces,
                  sizeof kMCarlo2Faces / sizeof kMCarlo2Faces[0]);

    /* THE DERIVED TABLE. Shared arm9 and dScMgBase_c words first, then the
       intermediate base's eight, then this class's own eight. Order does not
       matter -- the key sets are disjoint, because a word is one address -- but
       it reads in inheritance order. */
    port_scene_mg_fill_shared(vt, 36);
    g_mc2_vt_claimed =
        mc2_apply(vt, 36, kSingle3DFaces,
                  sizeof kSingle3DFaces / sizeof kSingle3DFaces[0])
      + mc2_apply(vt, 36, kMCarlo2Faces,
                  sizeof kMCarlo2Faces / sizeof kMCarlo2Faces[0]);

    /* THE CARD TABLE, section 4b. Two slots, and it is not a Scene table: no
       arm9 or dScMgBase_c word appears in it, so port_scene_mg_fill_shared is
       NOT called over it -- only this class's own two rows can land, and if
       either fails to, the mounted table keeps a raw DS word that the check
       below reports by name. Filling it here rather than at spawn time is
       correct because the element constructor writes the table's ADDRESS into
       each of the forty records, not its contents, so records built later pick
       the host pointers up on their own. */
    {
        void **card = (void **)data_ov006_0213d728;
        g_mc2_card_claimed =
            mc2_apply(card, 2, kCard2Faces,
                      sizeof kCard2Faces / sizeof kCard2Faces[0]);
    }

    /* the two diagnostics, applied after the fill so they override it */
    {
        const char *s0 = std::getenv("SM64DS_SCENE_SLOT0");
        const char *s9 = std::getenv("SM64DS_SCENE_SLOT9");
        if (s0 && s0[0] == '0') vt[0] = (void *)mc2_init_noop;
        if (s9 && s9[0] == '0') vt[9] = (void *)mc2_render_noop;
    }

    {
        const unsigned lb = mc2_raw_left(base, 36);
        const unsigned lm = mc2_raw_left(mid, 36);
        const unsigned lv = mc2_raw_left(vt, 36);
        const unsigned lc = mc2_raw_left((void **)data_ov006_0213d728, 2);
        if (lb || lm || lv || lc) {
            std::fprintf(stderr, "  [scene] MCARLO2 FILL INCOMPLETE: "
                         "dScMgBase_c leaves %u of 36 raw DS words, "
                         "dScMgSingle3DBase_c %u, dScMgMCarlo2_c %u, "
                         "dMgMCarlo2CardObj_c %u of 2. A dispatch of any of "
                         "them jumps to a DS address as a host one.\n",
                         lb, lm, lv, lc);
            std::fflush(stderr);
        }
    }

    /* The constructors, gated on the requested id, exactly as every other
       minigame fill does it. Idempotent behind scene_mg.cpp's own
       once-per-process guard. */
    port_scene_mg_prepare(port_scene_env_want());

    if (port_scene_env_want() == 382) {
        static int armed;
        if (!armed) {
            armed = 1;
            std::atexit(port_scene_mcarlo2_hits);
        }
    }
}

/* The registry's factory column is void *(*)(void) and so is the matched
   factory, but the forwarder is kept for the reason title_spawn and
   port_mg_curling_spawn are: it gives the seat one place to observe the object
   without the registry table growing a second column. */
static char *g_mc2_self;

extern "C" void *port_mg_mcarlo2_spawn(void)
{
    void *p = MgPairAGoneAndOn_Spawn();
    g_mc2_self = (char *)p;
    return p;
}

extern "C" void port_scene_mcarlo2_hits(void)
{
    unsigned total = 0, mtotal = 0;
    for (int i = 0; i < 36; ++i) {
        total += g_mc2_hits[i];
        mtotal += g_mc2_base_hits[i];
    }

    std::printf("[scene] dScMgMCarlo2_c slot hits: init %u, cleanup %u, "
                "behavior %u, render %u, D2 %u, D0 %u, state-reset %u, "
                "slot19 %u%s\n",
                g_mc2_hits[0], g_mc2_hits[3], g_mc2_hits[6], g_mc2_hits[9],
                g_mc2_hits[16], g_mc2_hits[17], g_mc2_hits[18], g_mc2_hits[19],
                g_mc2_render_skipped
                    ? "  [RENDER SLOT NO-OP'd: SM64DS_SCENE_SLOT9=0]" : "");
    if (g_mc2_init_skipped)
        std::printf("[scene] INIT SLOT NO-OP'd: SM64DS_SCENE_SLOT0=0, %u "
                    "time(s)\n", g_mc2_init_skipped);

    std::printf("[scene] dScMgMCarlo2_c own slots entered:");
    for (int i = 0; i < 36; ++i)
        if (g_mc2_hits[i]) std::printf(" %d(x%u)", i, g_mc2_hits[i]);
    std::printf("   (%u total)\n", total);

    std::printf("[scene] dScMgSingle3DBase_c slots entered (this seat's copy):");
    for (int i = 0; i < 36; ++i)
        if (g_mc2_base_hits[i]) std::printf(" %d(x%u)", i, g_mc2_base_hits[i]);
    std::printf("   (%u total)\n", mtotal);

    /* Section 3's claim, measured rather than asserted: on a tree carrying the
       flower, memory2, luckystars and mcarlo rows this seat's middle copy
       should claim ZERO slots and its derived copy should claim FOURTEEN of the
       sixteen face rows -- this class's own eight plus six of the middle base's
       eight. The two that do not land are the middle base's own D2 and D0
       (0x0210a4b0 and 0x0210a4e8): this class overrides slots 16 and 17 with
       bodies of its own, so those two words are not in the derived table to be
       claimed. 14 + 0 + 2 = 16 host writes, and the fill report below prints
       all three. */
    std::printf("[scene] dScMgMCarlo2_c fill claims: middle table %u slot(s), "
                "derived table %u slot(s) (of 8 + 8 face rows), card table %u "
                "of 2\n",
                g_mc2_mid_claimed, g_mc2_vt_claimed, g_mc2_card_claimed);

    /* THE CARD TABLE'S OWN WITNESS, section 4b. The draw is the only code in
       this class that puts a card pixel anywhere, so this pair of numbers is
       the difference between "the board ticked" and "the board was drawn". */
    std::printf("[scene] dMgMCarlo2CardObj_c dispatches: %u draw(s) through "
                "slot 0, %u update(s) through slot 1\n",
                g_mc2_card_draws, g_mc2_card_updates);

    /* THE STATE MACHINE'S OWN WITNESS. This class dispatches its states through
       a plain compiled switch on the s16 at +0x5928 rather than through a
       member-pointer table, so there is no address switch to count and the
       honest instrument is the INDEX ITSELF: slot 18 sets it to 1, slot 6
       carries it through 1, 2 and 3, and slot 19 carries it through 4, 5 and 6.
       A run whose index never moves has ticked the object without running its
       state machine, and that reads as a success unless it is said out loud.
       The framework's own dispatch counters are printed beside it because a
       zero there is a different fact from a zero here. */
    {
        unsigned calls = 0, unknown = 0;
        port_mg_dispatch_counts(&calls, &unknown);
        std::printf("[scene] dScMgMCarlo2_c state index +0x5928 = %d, "
                    "sub-timer +0x592a = %d, board step +0x592e = %d; "
                    "%u framework call(s), %u UNHANDLED address(es)\n",
                    g_mc2_self ? *(const short *)(g_mc2_self + 0x5928) : -1,
                    g_mc2_self ? *(const short *)(g_mc2_self + 0x592a) : -1,
                    g_mc2_self ? *(const short *)(g_mc2_self + 0x592e) : -1,
                    calls, unknown);
    }

    /* THE CARD ARRAY. The factory builds 0x28 records of 0x30 bytes at +0x51a8
       with the element vtable 0x0213d728 ("19dMgMCarlo2CardObj_c"). Section
       15's lesson is why this line exists at all: a census that measures only
       DISPATCH cannot see a board that was never dealt, and Memory Master's
       sixteen cards rendered onto an empty table with every dispatch counter
       reading green. FORTY here rather than the sibling's eighty, which is the
       factory's own count word (`mov r1,#0x28` at 0x020fa700). */
    if (g_mc2_self) {
        unsigned live = 0;
        for (int i = 0; i < 0x28; ++i)
            if (*(const void *const *)(g_mc2_self + 0x51a8 + i * 0x30))
                ++live;
        std::printf("[scene] dScMgMCarlo2_c card array: %u of 40 record(s) "
                    "hold a vptr\n", live);
    }

    /* THE SAVED PROGRESS, both ends. The ov005 row gives this scene save record
       25; printing the object's own fields beside the record says which of them
       moved, which is the question section 17 had to disassemble a whole
       function to answer for Memory Master. */
    {
        const int idx = port_mg2_record_index();
        if (g_mc2_self)
            std::printf("[scene] dScMgMCarlo2_c progress: +0xb4 level = %d, "
                        "+0xb8 best = %d, +0xa8 score = %d\n",
                        *(const int *)(g_mc2_self + 0xb4),
                        *(const int *)(g_mc2_self + 0xb8),
                        *(const int *)(g_mc2_self + 0xa8));
        if (idx >= 0 && idx < 36) {
            const int *rec = (const int *)(data_0209caf4 + idx * 20);
            std::printf("[scene] dScMgMCarlo2_c save record data_0209caf4[%d] "
                        "= { %d, %d, %d, %d, %d }\n", idx,
                        rec[0], rec[1], rec[2], rec[3], rec[4]);
        } else {
            std::printf("[scene] dScMgMCarlo2_c save record: NO minigame index "
                        "(data_ov004_020beb68 = %p)\n", data_ov004_020beb68);
        }
    }
    std::fflush(stdout);
}

/* ---- THE NAME-SPELLING ALIASES -------------------------------------------
 *
 * The "C-named symbols declared at C++ linkage" case hal/scene_boot.cpp
 * section 1 carries twenty-three of for the star select and
 * port/mg_fanout_costs.txt section 4 counts twenty-two of in the family's own
 * wall. Three TUs in this slice reach mounted DS data through a namespace-scope
 * `extern` rather than through an extern "C" block, so MSVC puts the TYPE into
 * the symbol name and the mount's plain C definition can never satisfy it.
 * Each row below is the mangling of one such declaration, and all eighteen were
 * read off the LINK rather than predicted -- the link is the only thing that
 * knows which spellings a TU actually emits.
 *
 * THEY ARE ALIASES AND NOT FACES, and the test is section 4's rulebook
 * corollary rather than habit: an alias is safe when the two spellings describe
 * the same bytes at the same address, and unsafe when one of them is a
 * member-pointer type (where mwcc's eight-byte pair and MSVC's four-byte one
 * disagree on stride). NONE OF THESE IS A MEMBER POINTER, checked one by one:
 *
 *   0x0213d6f4, 0x0213d6f8   .data halfwords the ROM reads with ldrsh, at the
 *                            same two addresses this class's slot 6 reads.
 *   0x0213d700               a .data word the ROM reads with ldr (the board's
 *                            Fix12 sweep position, written as `x * 5 << 12`).
 *   0x0213d7e8               this class's own vtable, spelled `int` by the
 *                            factory and `void *` by the D2, both by ADDRESS.
 *   0x02142558/60/64         .bss halfwords (the round timer, a counter, and
 *                            the score slot 9 clamps at 0x270f).
 *   0x02142568..0x0214257c   six .bss WORDS each holding a pointer to a card
 *                            record. This is the spelling section 10's tool
 *                            finding warns about, @@3PAU hiding a member-
 *                            pointer TABLE behind a struct wrapper -- and the
 *                            guard's shape matches while its reason does not,
 *                            for run mg9 lane S381's reason: `Node` is an
 *                            ordinary object type with a vptr at 0, prev/next
 *                            at +4/+8 and an int at +0x20, every one of those
 *                            offsets the same width on both machines, and the
 *                            disassembly at 0x020fa0ac and 0x020f93f4 reads
 *                            exactly those fields. A member-pointer table would
 *                            stride by 8 in the ROM and 4 on the host; these are
 *                            single words, not a table at all.
 *   0x02142580               a .bss int array indexed [i] with a 4-byte stride
 *                            on both machines.
 *   0x0212e97c               a .rodata table of 6+ rows of ten ints. The src
 *                            declares it `int[][10]` but INDEXES IT BY BYTES
 *                            (`(char *)base + idx * 0x28`), so the declared
 *                            shape reaches only the address.
 *
 * AND TWO OF THE EIGHTEEN ARE FUNCTIONS, WHICH IS A DIFFERENT DEFECT WITH THE
 * SAME REMEDY. src/func_ov006_020f9000.cpp declares
 *
 *     void ApproachLinear2(s16& r, s16 a, s16 b);
 *     void ApproachLinear(int& r, int a, int b);
 *
 * at namespace scope, while the port defines both as extern "C" bodies under
 * their mwcc names -- _Z14ApproachLinearRiii in hal/player_bridges.cpp and
 * _Z15ApproachLinear2Rsss in hal/method_faces.cpp -- and both of those RETURN
 * int. So the disagreement is the RETURN TYPE and the reference-versus-pointer
 * spelling, neither of which changes a single byte of the 32-bit cdecl frame: a
 * reference is passed as an address, the arguments are identical, cleanup is
 * caller-side, and EAX is simply not read. THAT LAST CLAUSE WAS CHECKED RATHER
 * THAN ASSUMED -- all four call sites (lines 103, 104, 146 and 165 of that TU)
 * are plain statements that discard the result. The same three-row shape
 * hal/cxx_aliases.cpp already carries for _Z14ApproachLinearRiii itself.
 */
#pragma comment(linker, "/alternatename:?data_ov006_0213d6f4@@3FA=_data_ov006_0213d6f4")
#pragma comment(linker, "/alternatename:?data_ov006_0213d6f8@@3FA=_data_ov006_0213d6f8")
#pragma comment(linker, "/alternatename:?data_ov006_0213d700@@3HA=_data_ov006_0213d700")
#pragma comment(linker, "/alternatename:?data_ov006_0213d7e8@@3HA=_data_ov006_0213d7e8")
#pragma comment(linker, "/alternatename:?data_ov006_0213d7e8@@3PAXA=_data_ov006_0213d7e8")
#pragma comment(linker, "/alternatename:?data_ov006_0212e97c@@3PAY09HA=_data_ov006_0212e97c")
#pragma comment(linker, "/alternatename:?data_ov006_02142558@@3FA=_data_ov006_02142558")
#pragma comment(linker, "/alternatename:?data_ov006_02142560@@3FA=_data_ov006_02142560")
#pragma comment(linker, "/alternatename:?data_ov006_02142564@@3FA=_data_ov006_02142564")
#pragma comment(linker, "/alternatename:?data_ov006_02142568@@3PAUNode@@A=_data_ov006_02142568")
#pragma comment(linker, "/alternatename:?data_ov006_0214256c@@3PAUNode@@A=_data_ov006_0214256c")
#pragma comment(linker, "/alternatename:?data_ov006_02142570@@3PAUNode@@A=_data_ov006_02142570")
#pragma comment(linker, "/alternatename:?data_ov006_02142574@@3PAUNode@@A=_data_ov006_02142574")
#pragma comment(linker, "/alternatename:?data_ov006_02142578@@3PAUNode@@A=_data_ov006_02142578")
#pragma comment(linker, "/alternatename:?data_ov006_0214257c@@3PAUNode@@A=_data_ov006_0214257c")
#pragma comment(linker, "/alternatename:?data_ov006_02142580@@3PAHA=_data_ov006_02142580")
#pragma comment(linker, "/alternatename:?ApproachLinear@@YAXAAHHH@Z=__Z14ApproachLinearRiii")
#pragma comment(linker, "/alternatename:?ApproachLinear2@@YAXAAFFF@Z=__Z15ApproachLinear2Rsss")
