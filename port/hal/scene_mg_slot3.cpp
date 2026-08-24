// dScMgSlot3_c, the "Mix-a-Mug" slot machine -- actor id 0x16d, scene 365.
// Run mg11, lane MUG. The NINTH class the port seats under
// dScMgSingle3DBase_c (eight were seated at base 98a4c0e8f; the count is
// measured in port/slice_mug.txt, not inherited), and the SECOND of the ROM's
// three dScMgSlotN_c classes after dScMgSlot1_c (0x16c, scene 364, "Mario
// Slot", seated by run mg9).
//
// Read port/slice_mug.txt for the identity derivation (six ROM witnesses, of
// which the player title is one), the five width checks, the state machine and
// the closure. Read port/unmatched/MgSlot3_StateDispatch.cpp for the
// pointer-to-member wall and the one floor. Read port/mg_fanout_costs.txt for
// what the family costs. This file is the seat: the two tables' faces, the
// fill, the factory forwarder and the tick witness.
//
// ---- 1. WHY THIS IS A SEPARATE FILE FROM hal/scene_mg.cpp -----------------
//
// scene_mg.cpp is the FAMILY half: the mounts, the thirty-five overlay
// constructors, dScMgBase_c's address-keyed faces and the gate that keeps the
// constructors off a level boot. All of that is paid once and none of it is
// re-derived here. This file reaches it through the one seam that file exports,
// port_scene_mg_fill_shared(), and adds only what is this class's.
//
// ---- 2. THE HIERARCHY IS THREE DEEP, AND THE ROM SAYS SO THREE TIMES ------
//
//     Scene -> dScMgBase_c         data_ov004_020bc0c0  36 slots
//           -> dScMgSingle3DBase_c data_ov006_0213e448  36 slots
//           -> dScMgSlot3_c        data_ov006_0213eaa8  36 slots
//
//   TYPEINFO. The word before this class's vtable, 0x0213eaa4, points at the
//     typeinfo record at 0x0213e588 = {0x0209a764, 0x0213e5ac, 0x0213bc64}.
//     0x0213e5ac reads "12dScMgSlot3_c" and 0x0213bc64 is
//     dScMgSingle3DBase_c's OWN typeinfo record (its name pointer 0x0213bd00
//     reads "19dScMgSingle3DBase_c"). The compiler wrote the inheritance down.
//   FACTORY. func_ov006_0210c120 calls ActorBase::operator new(0x5044), then
//     func_ov004_020b2adc (dScMgBase_c's constructor), then
//     `str r1,[r4]` with 0x0213e448 at 0x0210c144, then `str r1,[r4]` again
//     with 0x0213eaa8 at 0x0210c158.
//   DESTRUCTORS. Slot 17 (func_ov006_0210a9a8, the D0) and its unmarked D2
//     sibling slot 16 (func_ov006_0210a954) both unwind them in the opposite
//     order, 0x0213eaa8 first and 0x0213e448 second. Both bodies' literal
//     pools hold exactly those two words.
//
// ---- 3. THE FILL IS ADDRESS-KEYED, AND THIS IS THE NINTH CLAIM ON THE
//         MIDDLE TABLE -----------------------------------------------------
//
// port_scene_fill_rom and slot3_apply both key on the ROM WORD a slot holds,
// not on the slot number, so the same face array can be applied to every table
// in the chain and can only land where the ROM parked that exact body. The two
// arrays below are disjoint by construction:
//
//   kSingle3DFaces  the eight bodies dScMgSingle3DBase_c overrides
//                   (slots 2, 5, 7, 10, 16, 17, 26, 33)
//   kSlot3Faces     the nine dScMgSlot3_c adds or replaces on top
//                   (slots 0, 6, 9, 16, 17, 18, 29, 30, 32)
//
// Slots 16 and 17 appear in both and hold DIFFERENT addresses -- 0x0210a4b0 /
// 0x0210a4e8 in the middle table, 0x0210a954 / 0x0210a9a8 in the derived one --
// so both arrays can be applied to both tables and each writes only its own.
//
// EIGHT OTHER SEATED FILES DEFINE THEIR OWN COPY OF kSingle3DFaces, counted
// by grepping the base tree for the face address 0x0210a6e4u:
// hal/scene_mg_boombox.cpp, _cup.cpp, _flower.cpp, _luckystars.cpp,
// _mcarlo.cpp, _memory1.cpp, _memory2.cpp and _psycheout.cpp. That is safe for
// the reason hal/scene_mg_memory2.cpp
// section 3 measures: the fill keys on a DS address, every row's fill runs on
// every boot, and the fill that runs FIRST claims the table -- the later ones
// find nothing left to write. This class's registry row is appended after all
// of them, so the flower keeps the middle table and every earlier witness
// keeps counting exactly what it counted before this seat existed. This seat's
// own middle copy therefore reports ZERO on any tree that carries the flower
// row, and that zero is the CHECK rather than a disappointment.
//
// THE PROMOTION ARGUMENT IS NOW OWED NINE TIMES OVER, and this lane again does
// NOT take it: exporting kSingle3DFaces as a seam touches eight files other
// mg11 lanes may be editing in parallel, and the run's merge plan is a serial
// merge of nine branches. The count is recorded so the lane that does it has
// the number rather than the intention -- and it is recorded as a MEASUREMENT
// for the reason port/slice_mug.txt gives: this file inherited "five" from a
// neighbour's header and it was wrong by four.
//
// ---- 4. SLOT 2 IS NOT src's BODY, AND SLOT 33 IS NOT src's EITHER ---------
//
// Inherited unchanged from the eight earlier seats under this base, and stated
// rather than cross-referenced because a reader of THIS file needs to know why
// two of the eight rows below do not name the src symbol:
//
// func_ov006_0210a6e4 (slot 2, AfterInitResources) drops the framework's
// second argument: the ROM never writes r1 before its `bl 0x20b08f0`, so the
// flags ride through in r1 and src spells the call with one argument because
// that is the only way to spell an unnamed value in C. On the host the callee
// would read stack litter. The repair is port_mg_flower_after_init in
// port/unmatched/MgFlower_Slot2.cpp, whose header asks the next lane to call
// it rather than write a second one; this is the NINTH lane to do so (eight
// call sites in the base tree, one each in the eight hal files listed in
// section 3, counted by grepping 98a4c0e8f for port_mg_flower_after_init), and
// src/func_ov006_0210a6e4.cpp stays out of port/slice_mug.txt for that reason.
//
// func_ov006_0210a708 (slot 33, the once-per-boot 3D setup) ends with two
// stores to LIGHT_COLOR at 0x040004cc, which a plain src build lands in the
// memory ntr maps across the I/O window without ever telling the geometry
// engine -- the "Yoshi is a black silhouette" defect run mg5 lane YTEX
// measured. It builds from the hostgen'd copy port/CMakeLists.txt's
// FLW_HOSTGEN_SYMS already emits, and src/func_ov006_0210a708.c is out of this
// slice for the same reason.
//
// ---- 5. SLOT 18 TAKES A REAL ARGUMENT, AND ON THIS CLASS THAT IS THE ONE
//         THING A COPIED FACE ARRAY WOULD HAVE GOT WRONG ------------------
//
// Every seated minigame before dScMg3DEsp_c had a slot-18 body that ignored
// its second argument, and the family's thunks declare the parameter only so
// __fastcall cleans four bytes. run mg9 lane PSY found the first exception.
// THIS CLASS IS THE SECOND, and it was checked from the ROM rather than
// inherited:
//
//   func_ov006_0210b314 keeps r1 (`mov r8, r1` at 0x0210b31c) and tests it --
//   `cmp r8,#3` at 0x0210b324, `cmp r8,#0x12` at 0x0210b32c, and an `== 4` arm
//   -- so the body has THREE distinct paths through it: 3 and 0x12 are the
//   round START (score 0xc, best-score clamp at 0x270e, level 0), 4 is the
//   round ADVANCE (level += 1 up to 5, best += 1), and any other value skips
//   both of those AND the board set-up and runs only the closing block, which
//   re-arms the three reels and puts the state index back to 0.
//
//   ALL THREE ROM DISPATCH SITES PASS A REAL VALUE, and there are THREE rather
//   than two: 0x0210bfe8 is `mov r1,#3` inside slot 0 InitResources, and state
//   func_ov006_0210ac3c has TWO -- 0x0210ac9c is `mov r1,#4` on the
//   round-cleared arm and 0x0210acd0 is `mov r1,#5` on the coins-remaining
//   arm. So the third path is not hypothetical: 5 is the ordinary "spin again
//   at the same level" reset and the ROM asks for it by name. There is no site
//   that passes -1 the way dScMgMCarlo_c's does.
//
// So slot3_reset below FORWARDS its third parameter. A ride-through thunk here
// would have started every round at level 0 and never advanced the board size,
// and nothing in the byte gate or the link can see that.
//
// ---- 6. SLOT 32 TAKES NO RECEIVER, AND THAT IS THE ROM's DOING ------------
//
// src/func_ov006_0210aa60.cpp declares `void func_ov006_0210aa60(void)` and
// the disassembly agrees: the body's first use of r0 is `mov r0,#0` at
// 0x0210aa68, feeding SetBg1Offset(0,0). It never reads the incoming r0. This
// is the ROM ride-through and NOT the dropped-receiver defect class -- there
// is no receiver in the body for a host thunk to lose. hal/scene_mg.cpp's own
// base face for this slot (mb_v32) is spelled the same way, over
// func_ov004_020b27f4, which is the shape this row inherits.
//
// ---- 7. THE WIDTH IS 36 ON BOTH SCENE TABLES, CHECKED FIVE WAYS -----------
//
// port/mg_fanout_costs.txt section 11 measured that twelve of twenty-nine
// widths in its own table were phantoms, and that a fill one word too wide
// writes a host thunk over live ROM data belonging to another class. For THIS
// table the span check reads 38 and the answer is not "38 is the width" -- the
// costs file resolved that question before this lane opened, and section 3 of
// port/slice_mug.txt carries the mechanism the resolution was missing:
//
//   check                     answer
//   -----------------------   --------------------------------------------
//   1 SPAN                    0x0213eaa8 -> next config symbol 0x0213eb40,
//                             0x98 = 38 words. An UPPER BOUND ONLY.
//   2 TERMINAL SLOT           slot 35 = 0x020ad660, the word every
//                             dScMgBase_c-derived table holds. So 36.
//   3 WHAT THE WORDS PAST     index 36 = 0x00000000 and index 37 =
//     THE END ARE             0x0213e5a0. Those two words are the RTTI
//                             HEADER of dScMgSlot1_c's vtable, which starts
//                             at 0x0213eb40: a {0, &typeinfo} pair, and
//                             0x0213e5a0 is exactly the typeinfo record
//                             whose name string reads "12dScMgSlot1_c".
//                             The config symbol boundary sits at the VTABLE,
//                             not at the header, which is why THIS span reads
//                             two words long. NOT a general rule: all 55 ov006
//                             vtables have an unsymbolised header, but only 13
//                             have a vtable as their PREVIOUS symbol, and this
//                             is the ONLY 0x98 span in the overlay (29 of the
//                             other 30 scene tables span exactly 0x90).
//                             port/slice_mug.txt check 3 has the sweep.
//   4 RELOCATION COUNT        36 relocations at indices 0..35 with NONE at
//                             index 36. A vtable slot is a relocated word by
//                             construction. (The 37th relocation inside the
//                             span is the neighbour's typeinfo pointer.)
//   5 RELOCATION STRIDE       stride 4 for exactly 36 words from 0x0213eaa8,
//     (run mg9 lane LKY)      then a jump of 8 across the zero at index 36.
//                             The jump IS the table ending.
//
// THE VICTIM OF A WRONG WIDTH IS NAMED AND SHIPS TODAY. A 38-slot fill here
// would write a host thunk over 0x0213eb3c, dScMgSlot1_c's typeinfo pointer --
// scene 364, the SCENE_MG_SLOT1 row, seated by run mg9 lane S364. The fill
// below is called with 36 and this lane's sibling canary is a scene-364 boot.
//
// ---- 8. THE ROW GOES LAST, AND THE ORDER IS LOAD-BEARING ------------------
//
// port_scene_registry_install walks port_scene_classes[] in table order and
// calls every row's fill on every boot; port_scene_mg_overlay_load runs the
// thirty-five constructors ONCE PER PROCESS from the tail of the first
// minigame row's fill. Appending therefore means the constructors have already
// run with clean ROM words when this fill starts, which is the latent-safe
// direction port/mg_fanout_costs.txt section 11 names.
//
// IT IS ALSO THE RULE THAT KEEPS THIS CLASS'S OWN STATE TABLE INTACT.
// __sinit_ov006_02132894 copies eight member-pointer pairs out of the .data
// span 0x0213e4e0..0x0213e588 into data_ov006_02142bdc, and this class's own
// SpawnInfo sits inside that run. Nothing this fill writes is inside that span
// -- the fill's whole reach is [0x0213eaa8, 0x0213eb38) -- so the rule is
// obeyed rather than relied on, and the lane's scene-374 canary is the check.
//
// ---- 9. WHAT THIS SEAT DOES NOT CLAIM -------------------------------------
//
//   - THE CLASS'S ONE FLOOR IS RETIRED (run mg12, lane MAM). State 2 of
//     data_ov006_02142bdc is 0x0210adac, the face-evaluation step after the
//     third reel stops; it now has a byte-matched src/func_ov006_0210adac.c and
//     the dispatcher calls it. port/unmatched/MgSlot3_StateDispatch.cpp
//     section 3 has the derivation and what changed. The former floor counter
//     in the census below reads 0 for a structural reason now.
//   - NOBODY HAS LOOKED AT A FRAME NEXT TO A DS. The census reports what the
//     class does; it cannot say the picture is right.
//   - THE SAVE RECORD IS THE PORT'S USUAL GAP. Slot 18 seeds +0xbc from
//     +0x5004 with the same 0x270e clamp the family uses, and the port has no
//     backup cart, so this class restarts at its first board every launch.
//     That is a save-medium gap, not a minigame gap.
//   - TOUCH IS UNMEASURED HERE. The class is a stylus slot machine; nothing in
//     a headless boot stops a reel.

#include "hal/screen_gap.h"

#include <cstdio>
#include <cstdlib>

extern "C" {

/* the seam into hal/scene_mg.cpp -- mounts, arm9 fill, dScMgBase_c's faces */
unsigned port_scene_mg_fill_shared(void **vt, unsigned n);
void     port_scene_mg_prepare(int id);
int      port_scene_env_want(void);
int      IsMinigameActorID(unsigned int id);
unsigned port_mg_scene_spawn_param(int scene_id);

/* The mount storage the fill writes into. Both tables are the ov006 mount's
   own, so the fill replaces the mounted words rather than building a fresh
   host array -- the ov007 and curling treatment, for the same reason: a second
   host array of the same name is a duplicate symbol, and leaving the mounted
   table alone leaves live wild DS pointers in a table the factory installs. */
extern unsigned char data_ov004_020bc0c0[];   /* dScMgBase_c,         36 */
extern unsigned char data_ov006_0213e448[];   /* dScMgSingle3DBase_c, 36 */
extern unsigned char data_ov006_0213eaa8[];   /* dScMgSlot3_c,        36 */
extern unsigned char data_ov006_0213e508[];   /* the SpawnInfo record    */
/* dScMgSlot1_c's typeinfo record -- NOT dispatched, NOT filled and NOT this
   class's. It is named for one reason: the census below reads the word past
   this seat's 36 and has to say what that word is supposed to be. */
extern unsigned char data_ov006_0213e5a0[];

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

/* dScMgSlot3_c's own nine. Slot 6 is the HOST COPY in
   port/unmatched/MgSlot3_StateDispatch.cpp -- same symbol, so
   src/func_ov006_0210bcb0.cpp is out of port/slice_mug.txt. */
int   func_ov006_0210bdb0(void *c);           /* slot  0 InitResources */
int   func_ov006_0210bcb0(char *c);           /* slot  6 Behavior, host copy */
int   func_ov006_0210b648(char *c);           /* slot  9 Render        */
void *func_ov006_0210a954(char *c);           /* slot 16 D2            */
void *func_ov006_0210a9a8(char *c);           /* slot 17 D0            */
void  func_ov006_0210b314(char *c, int mode); /* slot 18 round set-up  */
void  func_ov006_0210aa3c(void *c);           /* slot 29               */
void  func_ov006_0210aa10(void *c);           /* slot 30               */
void  func_ov006_0210aa60(void);              /* slot 32, no receiver  */

/* the factory */
void *func_ov006_0210c120(void);

/* the state machine, from unmatched/MgSlot3_StateDispatch.cpp */
unsigned port_mg_slot3_state_hits(void);
unsigned port_mg_slot3_floor_hits(void);
void     port_mg_slot3_state_slots(unsigned *out8);

/* the framework's dispatch witness, from unmatched/MgBase_StateDispatch.cpp */
void  port_mg_dispatch_counts(unsigned *calls, unsigned *unknown);

/* The persistent minigame record this class's board progress is READ FROM and
   WRITTEN BACK TO. Nothing new is defined here: the storage is
   hal/level_boot.cpp's .dsstate$savblk0004 and the index function is
   src/func_ov004_020adc3c.c. */
extern void         *data_ov004_020beb68;     /* the live dScMgBase_c `this` */
extern unsigned char data_0209caf4[];         /* 36 records x 20 bytes       */
int   func_ov004_020adc3c(void *c);           /* (self->field_8 >> 8) & 0xff */

}  /* extern "C" */

static int port_mg_slot3_record_index(void)
{
    return data_ov004_020beb68 ? func_ov004_020adc3c(data_ov004_020beb68) : -1;
}

// ---- the tick witness ------------------------------------------------------
//
// One counter per dispatched slot on each of the two tables this file owns
// faces for. The instrument every scene seat in this port carries, for the
// reason hal/scene_mg.cpp gives: an object that EXISTS and an object that RUNS
// look identical from outside.
static unsigned g_s3_hits[36];
static unsigned g_s3_base_hits[36];   /* the same slots on the MIDDLE table */

#define S3(n)    (++g_s3_hits[(n)])
#define M3D(n)   (++g_s3_base_hits[(n)])

/* the last value slot 18 was actually dispatched with, so section 5's finding
   has a runtime witness rather than only a disassembly. -1 means never. */
static int g_s3_slot18_arg = -1;
static unsigned g_s3_slot18_arg3, g_s3_slot18_arg4, g_s3_slot18_arg5,
                g_s3_slot18_argother;

/* ---- dScMgSingle3DBase_c's eight ---------------------------------------- */
static void *__fastcall s3b_ainit(void *s, void *, unsigned f)
{ M3D(2);  return (void *)(size_t)port_mg_flower_after_init(s, f); }
static void __fastcall s3b_aclean(void *s, void *, unsigned f)
{ M3D(5);  func_ov006_0210a608(s, f); }
static int  __fastcall s3b_bbeh(void *s, void *)
{ M3D(7);  return func_ov006_0210a698(s); }
static int  __fastcall s3b_bren(void *s, void *)
{ M3D(10); return func_ov006_0210a664(s); }
static void *__fastcall s3b_d2(void *s, void *)
{ M3D(16); return (void *)(size_t)func_ov006_0210a4b0((char *)s); }
static void *__fastcall s3b_d0(void *s, void *)
{ M3D(17); return (void *)(size_t)func_ov006_0210a4e8((char *)s); }
static int  __fastcall s3b_v26(void *, void *)
{ M3D(26); return func_ov006_0210a600(); }
static int  __fastcall s3b_v33(void *s, void *)
{ M3D(33); func_ov006_0210a708((char *)s); return 0; }

/* ---- dScMgSlot3_c's own nine -------------------------------------------- */
static int  __fastcall slot3_init(void *s, void *)
{ S3(0);  const int r = func_ov006_0210bdb0(s);
  /* the GaplessMinigames latch, for hal/scene_mg.cpp's reason: every seated
     minigame calls it so the ones the gapless table does not name can say
     "unsupported" instead of doing nothing quietly. */
  hal_gapless_minigames_latch(); return r; }
static int  __fastcall slot3_beh(void *s, void *)
{ S3(6);  return func_ov006_0210bcb0((char *)s); }
static int  __fastcall slot3_render(void *s, void *)
{ S3(9);  return func_ov006_0210b648((char *)s); }
static void *__fastcall slot3_d2(void *s, void *)
{ S3(16); return func_ov006_0210a954((char *)s); }
static void *__fastcall slot3_d0(void *s, void *)
{ S3(17); return func_ov006_0210a9a8((char *)s); }
/* SECTION 5. THE THIRD PARAMETER IS FORWARDED AND IT IS NOT CEREMONY: the ROM
   body branches on it three ways and the two ROM dispatch sites pass 3 and 4.
   A (void*, void*) thunk here would compile to a body that reads whatever was
   in the slot, and both of this class's rounds would start identically. */
static int  __fastcall slot3_reset(void *s, void *, int mode)
{ S3(18);
  g_s3_slot18_arg = mode;
  if (mode == 3 || mode == 0x12) ++g_s3_slot18_arg3;
  else if (mode == 4)            ++g_s3_slot18_arg4;
  else if (mode == 5)            ++g_s3_slot18_arg5;
  else                           ++g_s3_slot18_argother;
  func_ov006_0210b314((char *)s, mode); return 1; }
static int  __fastcall slot3_v29(void *s, void *)
{ S3(29); func_ov006_0210aa3c(s); return 0; }
static int  __fastcall slot3_v30(void *s, void *)
{ S3(30); func_ov006_0210aa10(s); return 0; }
/* SECTION 6: no receiver, the ROM's own shape, matching hal/scene_mg.cpp's
   mb_v32 over func_ov004_020b27f4. */
static int  __fastcall slot3_v32(void *, void *)
{ S3(32); func_ov006_0210aa60(); return 0; }

/* SM64DS_SCENE_SLOT0=0 and SM64DS_SCENE_SLOT9=0, the diagnostics every scene
   seat in this port carries, counted separately so a run can never read a
   no-op as the real body having run. */
static unsigned g_s3_init_skipped, g_s3_render_skipped;
static int __fastcall slot3_init_noop(void *, void *)
{ ++g_s3_init_skipped; return 1; }
static int __fastcall slot3_render_noop(void *, void *)
{ ++g_s3_render_skipped; return 1; }

struct S3Face { unsigned ds; void *host; };

static const S3Face kSingle3DFaces[] = {
    {0x0210a6e4u, (void *)s3b_ainit},  {0x0210a608u, (void *)s3b_aclean},
    {0x0210a698u, (void *)s3b_bbeh},   {0x0210a664u, (void *)s3b_bren},
    {0x0210a4b0u, (void *)s3b_d2},     {0x0210a4e8u, (void *)s3b_d0},
    {0x0210a600u, (void *)s3b_v26},    {0x0210a708u, (void *)s3b_v33},
};

static const S3Face kSlot3Faces[] = {
    {0x0210bdb0u, (void *)slot3_init},   {0x0210bcb0u, (void *)slot3_beh},
    {0x0210b648u, (void *)slot3_render}, {0x0210a954u, (void *)slot3_d2},
    {0x0210a9a8u, (void *)slot3_d0},     {0x0210b314u, (void *)slot3_reset},
    {0x0210aa3cu, (void *)slot3_v29},    {0x0210aa10u, (void *)slot3_v30},
    {0x0210aa60u, (void *)slot3_v32},
};

static unsigned slot3_apply(void **vt, unsigned n, const S3Face *f, unsigned nf)
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
static unsigned slot3_raw_left(void **vt, unsigned n)
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
static unsigned g_s3_mid_claimed, g_s3_vt_claimed;

extern "C" void port_scene_slot3_hits(void);

extern "C" void port_scene_fill_slot3(void)
{
    void **base = (void **)data_ov004_020bc0c0;
    void **mid  = (void **)data_ov006_0213e448;
    void **vt   = (void **)data_ov006_0213eaa8;

    /* THE BASE TABLE IS FILLED HERE TOO AND IT IS NOT CEREMONY. Earlier rows'
       fills already did it and run first, so on a tree carrying them this is a
       second pass over words that are already host pointers and finds nothing.
       It is here so this class does not depend on another class's row
       existing: the factory's first act after operator new is
       func_ov004_020b2adc, which writes data_ov004_020bc0c0 into the object's
       first word before either derived table lands. */
    port_scene_mg_fill_shared(base, 36);

    /* THE MIDDLE TABLE. Live storage in its own right -- the destructor chain
       writes it into the object on the way down, so a slot dispatched between
       the two vptr writes reads it. Section 3 is why applying this file's own
       arrays here is safe beside the eight earlier seats', and why the claimed
       count below is expected to be ZERO on a tree carrying them. */
    port_scene_mg_fill_shared(mid, 36);
    g_s3_mid_claimed =
        slot3_apply(mid, 36, kSingle3DFaces,
                    sizeof kSingle3DFaces / sizeof kSingle3DFaces[0])
      + slot3_apply(mid, 36, kSlot3Faces,
                    sizeof kSlot3Faces / sizeof kSlot3Faces[0]);

    /* THE DERIVED TABLE. Shared arm9 and dScMgBase_c words first, then the
       intermediate base's eight, then this class's own nine. Order does not
       matter -- the three key sets are disjoint, because a word is one address
       -- but it reads in inheritance order. */
    port_scene_mg_fill_shared(vt, 36);
    g_s3_vt_claimed =
        slot3_apply(vt, 36, kSingle3DFaces,
                    sizeof kSingle3DFaces / sizeof kSingle3DFaces[0])
      + slot3_apply(vt, 36, kSlot3Faces,
                    sizeof kSlot3Faces / sizeof kSlot3Faces[0]);

    /* the two diagnostics, applied after the fill so they override it */
    {
        const char *s0 = std::getenv("SM64DS_SCENE_SLOT0");
        const char *s9 = std::getenv("SM64DS_SCENE_SLOT9");
        if (s0 && s0[0] == '0') vt[0] = (void *)slot3_init_noop;
        if (s9 && s9[0] == '0') vt[9] = (void *)slot3_render_noop;
    }

    {
        const unsigned lb = slot3_raw_left(base, 36);
        const unsigned lm = slot3_raw_left(mid, 36);
        const unsigned lv = slot3_raw_left(vt, 36);
        if (lb || lm || lv) {
            std::fprintf(stderr, "  [scene] SLOT3 FILL INCOMPLETE: "
                         "dScMgBase_c leaves %u of 36 raw DS words, "
                         "dScMgSingle3DBase_c %u, dScMgSlot3_c %u. A dispatch "
                         "of any of them jumps to a DS address as a host "
                         "one.\n", lb, lm, lv);
            std::fflush(stderr);
        }
    }

    /* The constructors, gated on the requested id, exactly as every other
       minigame fill does it. Idempotent behind scene_mg.cpp's own
       once-per-process guard, and it is what fills data_ov006_02142bdc. */
    port_scene_mg_prepare(port_scene_env_want());

    if (port_scene_env_want() == 365) {
        static int armed;
        if (!armed) {
            armed = 1;
            std::atexit(port_scene_slot3_hits);
        }
    }
}

/* The registry's factory column is void *(*)(void) and so is the matched
   factory, but the forwarder is kept for the reason title_spawn and
   port_mg_curling_spawn are: it gives the seat one place to observe the object
   without the registry table growing a second column. */
static char *g_s3_self;

extern "C" void *port_mg_slot3_spawn(void)
{
    void *p = func_ov006_0210c120();
    g_s3_self = (char *)p;
    return p;
}

extern "C" void port_scene_slot3_hits(void)
{
    unsigned total = 0, mtotal = 0;
    for (int i = 0; i < 36; ++i) {
        total  += g_s3_hits[i];
        mtotal += g_s3_base_hits[i];
    }

    std::printf("[scene] dScMgSlot3_c slot hits: init %u, behavior %u, "
                "render %u, D2 %u, D0 %u, round set-up %u, v29 %u, v30 %u, "
                "v32 %u%s\n",
                g_s3_hits[0], g_s3_hits[6], g_s3_hits[9], g_s3_hits[16],
                g_s3_hits[17], g_s3_hits[18], g_s3_hits[29], g_s3_hits[30],
                g_s3_hits[32],
                g_s3_render_skipped
                    ? "  [RENDER SLOT NO-OP'd: SM64DS_SCENE_SLOT9=0]" : "");
    if (g_s3_init_skipped)
        std::printf("[scene] INIT SLOT NO-OP'd: SM64DS_SCENE_SLOT0=0, %u "
                    "time(s)\n", g_s3_init_skipped);

    std::printf("[scene] dScMgSlot3_c own slots entered:");
    for (int i = 0; i < 36; ++i)
        if (g_s3_hits[i]) std::printf(" %d(x%u)", i, g_s3_hits[i]);
    std::printf("   (%u total)\n", total);

    std::printf("[scene] dScMgSingle3DBase_c slots entered (this seat's copy):");
    for (int i = 0; i < 36; ++i)
        if (g_s3_base_hits[i]) std::printf(" %d(x%u)", i, g_s3_base_hits[i]);
    std::printf("   (%u total)\n", mtotal);

    /* Section 3's claim, measured rather than asserted: on a tree carrying the
       flower, memory2, boombox, mcarlo and cup rows this seat's middle copy
       should claim ZERO slots and its derived copy should claim FIFTEEN of the
       seventeen face rows -- this class's own nine plus six of the middle
       base's eight. The two that do not land are the middle base's own D2 and
       D0 (0x0210a4b0 and 0x0210a4e8): this class overrides slots 16 and 17
       with bodies of its own, so those two words are not in the derived table
       to be claimed. */
    std::printf("[scene] dScMgSlot3_c fill claims: middle table %u slot(s), "
                "derived table %u slot(s) (of 8 + 9 face rows)\n",
                g_s3_mid_claimed, g_s3_vt_claimed);

    /* SECTION 7's WIDTH CHECK, ASKED OF THE RUNNING PROCESS. The five static
       checks say this table is 36 words and that the two words past it are
       dScMgSlot1_c's RTTI header -- {0, &typeinfo} in front of the scene-364
       vtable at 0x0213eb40, which the SCENE_MG_SLOT1 row ships. A fill one or
       two words too wide would put a host thunk there and NOTHING WOULD
       FAULT: the port never dereferences a typeinfo pointer, so scene 364
       would keep booting and keep printing an unchanged census. That is the
       one hazard a runtime census cannot see, which is why this line reads
       the two words back instead.

       THE EXPECTED VALUE IS NOT THE ROM's LITERAL, and getting that wrong the
       first time is worth the sentence: the ov006 mount is a PACKED,
       CONTIGUOUS blob (port/ov006_syms.txt: "--pack fills the space BETWEEN
       mounted symbols ... makes the run contiguous"), so the two words really
       are the neighbour's -- but the mount REBASES relocated words, so the
       typeinfo pointer reads as the HOST address of data_ov006_0213e5a0 and
       not as 0x0213e5a0. Comparing against the mount's own symbol is the check
       that survives that, and it is the stronger comparison anyway: it asks
       "is this still the word the mount emitted", which is exactly the
       question. */
    {
        const unsigned *past = (const unsigned *)(data_ov006_0213eaa8 + 36 * 4);
        const unsigned want = (unsigned)(size_t)data_ov006_0213e5a0;
        std::printf("[scene] dScMgSlot3_c words past its 36: [36] = 0x%08x, "
                    "[37] = 0x%08x (the mount's own &data_ov006_0213e5a0 = "
                    "0x%08x, dScMgSlot1_c's typeinfo, scene 364)%s\n",
                    past[0], past[1], want,
                    (past[0] == 0u && past[1] == want)
                        ? "  UNTOUCHED" : "  *** CLOBBERED ***");
    }

    /* SECTION 5's FINDING, WITNESSED AT RUN TIME. Every seated class before
       dScMg3DEsp_c has a slot-18 body that ignores this argument; this one
       branches on it three ways. Printing the split says the forward happened
       and which arms ran, rather than leaving it to the disassembly. */
    std::printf("[scene] dScMgSlot3_c slot-18 second argument: last %d; "
                "round-start(3/0x12) %u, round-advance(4) %u, respin(5) %u, "
                "other %u\n",
                g_s3_slot18_arg, g_s3_slot18_arg3, g_s3_slot18_arg4,
                g_s3_slot18_arg5, g_s3_slot18_argother);

    /* THE STATE MACHINE'S OWN WITNESS. This class dispatches its states
       through an mwcc member-pointer table, so the honest instrument is BOTH
       the per-slot routing counts AND the index itself: a run whose index
       never moves has ticked the object without running its state machine, and
       that reads as a success unless it is said out loud. */
    {
        unsigned calls = 0, unknown = 0, slots[8];
        port_mg_dispatch_counts(&calls, &unknown);
        port_mg_slot3_state_slots(slots);
        std::printf("[scene] dScMgSlot3_c state dispatch: %u call(s) through "
                    "the address switch, %u to the RETIRED floor at state 2 "
                    "(0x0210adac, matched, expect 0); %u framework call(s), "
                    "%u UNHANDLED address(es)\n",
                    port_mg_slot3_state_hits(), port_mg_slot3_floor_hits(),
                    calls, unknown);
        std::printf("[scene] dScMgSlot3_c state slots (table order):");
        for (int i = 0; i < 8; ++i) std::printf(" %d(x%u)", i, slots[i]);
        std::printf("\n");
    }

    std::printf("[scene] dScMgSlot3_c state index +0x5000 = %d, level +0x5004 "
                "= %d, open timer +0x500c = %d, reels stopped +0x503d = %d, "
                "symbols per reel +0x503a = %d\n",
                g_s3_self ? *(const int *)(g_s3_self + 0x5000) : -1,
                g_s3_self ? *(const int *)(g_s3_self + 0x5004) : -1,
                g_s3_self ? *(const int *)(g_s3_self + 0x500c) : -1,
                g_s3_self ? *(const unsigned char *)(g_s3_self + 0x503d) : -1,
                g_s3_self ? *(const unsigned char *)(g_s3_self + 0x503a) : -1);

    /* THE THREE REEL POSITIONS, in the units the ROM keeps them in. Slot 0
       seeds them from RandomIntInternal and state 1 walks them; a run whose
       reels never move has drawn a slot machine that is not spinning, which is
       exactly the shape run mg9 lane S364 found on dScMgSlot1_c and no
       dispatch counter can see. */
    if (g_s3_self) {
        const int *r = (const int *)(g_s3_self + 0x4fe4);
        std::printf("[scene] dScMgSlot3_c reels +0x4fe4 = { 0x%x, 0x%x, 0x%x },"
                    " strip length +0x4ff0 = 0x%x\n",
                    r[0], r[1], r[2], *(const int *)(g_s3_self + 0x4ff0));
    }

    /* THE SPAWN PARAMETER, both halves, and the save record it selects. */
    {
        const unsigned param = port_mg_scene_spawn_param(365);
        const int idx = port_mg_slot3_record_index();
        std::printf("[scene] dScMgSlot3_c spawn param 0x%08x -> save record "
                    "%u, name text %u, mode %u (ov005 row 31, the only row for "
                    "scene 365)\n", param, (param >> 8) & 0xff,
                    (param >> 16) & 0xff, param & 0xff);
        if (g_s3_self)
            std::printf("[scene] dScMgSlot3_c progress: +0xb4 score = %d, "
                        "+0xb8 = %d, +0xa8 = %d, +0xbc best = %d\n",
                        *(const int *)(g_s3_self + 0xb4),
                        *(const int *)(g_s3_self + 0xb8),
                        *(const int *)(g_s3_self + 0xa8),
                        *(const int *)(g_s3_self + 0xbc));
        if (idx >= 0 && idx < 36) {
            const int *rec = (const int *)(data_0209caf4 + idx * 20);
            std::printf("[scene] dScMgSlot3_c save record data_0209caf4[%d] "
                        "= { %d, %d, %d, %d, %d }\n", idx,
                        rec[0], rec[1], rec[2], rec[3], rec[4]);
        } else {
            std::printf("[scene] dScMgSlot3_c save record: NO minigame index "
                        "(data_ov004_020beb68 = %p)\n", data_ov004_020beb68);
        }
    }
    std::fflush(stdout);
}

/* ---- THE NAME-SPELLING ALIASES -------------------------------------------
 *
 * The "C-named symbols declared at C++ linkage" case hal/scene_boot.cpp
 * section 1 carries twenty-three of for the star select. TWO TUs in this slice
 * reach a symbol through a namespace-scope declaration rather than through an
 * extern "C" block, so MSVC puts the TYPE into the symbol name and the plain C
 * definition can never satisfy it. Both were found by the link, both are one
 * row, and neither is a member pointer -- which is the rulebook corollary that
 * makes an alias safe: the two spellings have to describe the same bytes at
 * the same address, and mwcc's eight-byte member-pointer pair against MSVC's
 * four-byte one is the case where they do not.
 *
 * ?data_ov006_0213eaa8@@3PAXA. src/func_ov006_0210a954.cpp (slot 16, the D2)
 * declares `extern void* data_ov006_0213eaa8;` outside its extern "C" block
 * and stores its ADDRESS into the object's first word. Its D0 sibling
 * src/func_ov006_0210a9a8.cpp declares the same table inside extern "C" and
 * needs no row, which is the two spellings of one table sitting three lines
 * apart in the same class. The right-hand side is the ov006 mount's own
 * definition of the vtable this seat fills.
 *
 * ?GetBG1ScrPtr@G2@@SAPAXXZ. src/func_ov006_0210aa60.cpp (slot 32) declares a
 * local `struct G2 { static void* GetBG1ScrPtr(); };` to spell the arm9 static,
 * so MSVC mangles the call while the port defines the Itanium name at C
 * linkage. The double underscore on the right is the same shape
 * hal/actor_faces_bob.cpp's Model::LoadFile row and eight others in
 * hal/actor_classes_*.cpp use.
 *
 * NO ROW IS OWED FOR data_ov006_0213e448: src/func_ov006_0210a954.cpp declares
 * it the same way, and hal/scene_mg_flower.cpp already carries
 * ?data_ov006_0213e448@@3HA while port/unmatched/MgMemory2_Faces.cpp carries
 * ?data_ov006_0213e448@@3PAXA. A second /alternatename for the same LHS is
 * noise a reader has to diff, and the link is satisfied.
 */
#pragma comment(linker, "/alternatename:?data_ov006_0213eaa8@@3PAXA=_data_ov006_0213eaa8")
#pragma comment(linker, "/alternatename:?GetBG1ScrPtr@G2@@SAPAXXZ=__ZN2G212GetBG1ScrPtrEv")
