// dScMgRoulette_c, the "Mushroom Roulette" minigame -- actor id 0x17f,
// scene 383. Run mg11, lane RLT. The FIFTH class the port seats under
// dScMgSingle3DBase_c, and the SECOND (and last) whose factory drops the
// receiver on the base constructor.
//
// Read port/slice_rlt.txt for the identity derivation, the four width checks,
// the member-pointer census and the closure. Read port/mg_fanout_costs.txt for
// what the family costs. This file is the seat: the three tables' faces, the
// fill, the factory forwarder and the tick witness.
//
// ---- 1. WHY THIS IS A SEPARATE FILE FROM hal/scene_mg.cpp -----------------
//
// scene_mg.cpp is the FAMILY half: the mounts, the thirty-five overlay
// constructors, dScMgBase_c's address-keyed faces and the gate that keeps the
// constructors off a level boot. None of that is re-derived here. This file
// reaches it through the one seam that file exports,
// port_scene_mg_fill_shared(), and adds only what is this class's. The
// alternative -- another face array appended inside scene_mg.cpp -- is what a
// nine-lane fan-out would collide on.
//
// ---- 2. THE HIERARCHY IS THREE DEEP, AND THE ROM SAYS SO THREE TIMES ------
//
//     Scene -> dScMgBase_c         data_ov004_020bc0c0  36 slots
//           -> dScMgSingle3DBase_c data_ov006_0213e448  36 slots
//           -> dScMgRoulette_c     data_ov006_0213e39c  36 slots
//
// THE CLASS NAME IS THE ROM'S OWN, not a src spelling. The word immediately
// before the vtable, 0x0213e398, carries a load relocation to 0x0213e300, which
// is the type_info record; its second word points at 0x0213e30c, which reads
// "15dScMgRoulette_c" in extracted/overlays/overlay_0006.bin. That is the
// compiler writing the name down, and a relocation cannot be a placeholder.
//
// THE MIDDLE EDGE IS STATED THREE SEPARATE WAYS:
//
//   TYPEINFO. 0x0213e300's THIRD word is 0x0213bc64 -- dScMgSingle3DBase_c's
//     own type_info, whose name string at 0x0213bd00 reads
//     "19dScMgSingle3DBase_c". That is the compiler writing the inheritance
//     down.
//   FACTORY. MgMushroomRoulette_Spawn (0x0210a400) calls func_ov004_020b2adc
//     (dScMgBase_c's constructor), writes 0x0213e448 into the object's first
//     word, then writes 0x0213e39c over it.
//   DESTRUCTORS. Slot 16 (func_ov006_0210788c, the D2) and slot 17
//     (func_ov006_02107920, the D0) both unwind them in the opposite order,
//     0x0213e39c first and 0x0213e448 second.
//
// A hierarchy the type_info states, the constructor writes and the destructor
// unwrites is not a guess.
//
// ---- 3. THE FILL IS ADDRESS-KEYED, AND THIS IS THE FIFTH CLAIM ON THE
//         MIDDLE TABLE -----------------------------------------------------
//
// port_scene_fill_rom and rlt_apply both key on the ROM WORD a slot holds, not
// on the slot number, so the same face array can be applied to every table in
// the chain and can only land where the ROM parked that exact body. The two
// arrays below are disjoint by construction:
//
//   kSingle3DFaces  the eight bodies dScMgSingle3DBase_c overrides
//                   (slots 2, 5, 7, 10, 16, 17, 26, 33)
//   kRouletteFaces  the eight dScMgRoulette_c adds or replaces on top
//                   (slots 0, 3, 6, 9, 16, 17, 18, 19)
//
// Slots 16 and 17 appear in both and hold DIFFERENT addresses -- 0x0210a4b0 /
// 0x0210a4e8 in the middle table, 0x0210788c / 0x02107920 in the derived one --
// so both arrays can be applied to both tables and each writes only its own.
//
// FIVE SEATED FILES NOW DEFINE THEIR OWN COPY OF kSingle3DFaces:
// hal/scene_mg_flower.cpp, hal/scene_mg_memory2.cpp, hal/scene_mg_luckystars.cpp,
// hal/scene_mg_mcarlo.cpp and this one. That is safe for the reason
// hal/scene_mg_memory2.cpp section 3 measures: rlt_apply keys on a DS address,
// every row's fill runs on every boot, and the fill that runs FIRST claims the
// table -- the later ones find nothing left to write. This class's registry row
// is appended after all four, so the flower keeps the middle table and every
// earlier witness keeps counting exactly what it counted before this seat
// existed. This seat's own middle copy therefore reports ZERO on any tree that
// carries the flower row, and that zero is the CHECK rather than a
// disappointment.
//
// PROMOTING kSingle3DFaces TO AN EXPORTED SEAM IS NOW OVERDUE BY TWO CLASSES.
// scene_mg_memory2.cpp says a third would be the moment; scene_mg_mcarlo.cpp
// says a fourth is; this is the fifth. This lane did NOT take the refactor,
// deliberately and for scene_mg_mcarlo.cpp's reason made worse by arithmetic:
// it touches four files that four other mg11 lanes may be editing in parallel,
// and the run's merge plan is a serial merge of nine branches. The argument is
// recorded here with the count -- FIVE copies now -- so the lane that does it
// has the number rather than the intention.
//
// ---- 4. SLOT 2 IS NOT src's BODY, AND SLOT 33 IS NOT src's EITHER ---------
//
// func_ov006_0210a6e4 (AfterInitResources) drops the framework's second
// argument: the ROM never writes r1 before its `bl 0x20b08f0`, so the flags
// ride through in r1 and src spells the call with one argument because that is
// the only way to spell an unnamed value in C. On the host the callee would
// read stack litter, and func_ov004_020b08f0's tail is
// Scene::AfterInitResources(this, flags) where flags == 1 marks the actor for
// destruction. The repair is port_mg_flower_after_init in
// port/unmatched/MgFlower_Slot2.cpp, whose header asks the next lane to call it
// rather than write a second one; memory2 was the second, mcarlo the third,
// luckystars the fourth and this is the fifth. src/func_ov006_0210a6e4.cpp
// stays out of port/slice_rlt.txt for the same reason it is out of the other
// four slices.
//
// func_ov006_0210a708 (slot 33, the once-per-boot 3D setup) ends with two
// stores to LIGHT_COLOR at 0x040004cc, which a plain src build lands in the
// memory ntr maps across the I/O window without ever telling the geometry
// engine -- the "Yoshi is a black silhouette" defect run mg5 lane YTEX
// measured. It builds from the hostgen'd copy port/CMakeLists.txt's
// FLW_HOSTGEN_SYMS already emits, and src/func_ov006_0210a708.c is out of this
// slice for the same reason.
//
// ---- 4b. THERE IS NO FOURTH TABLE, AND THAT IS MEASURED -------------------
//
// The mg9 element-vtable law says to check word 0 of every object a factory
// builds, because dMgMCarloCardObj_c's eighty card records carry a mounted ROM
// vtable that no vtable-axis derivation can see. This factory builds FIVE
// records of 0x34 bytes at this+0x51a8 through func_020733a8, and the element
// constructor it hands over is func_ov006_0210a4ac -- ONE WORD, 0xe12fff1e,
// `bx lr`. The destructor func_ov006_021079c8 is the same instruction. So
// nothing writes word 0 of a record at construction, and the only later writer
// is vtable slot 18's func_ov006_021092a0, which puts the plain fixed-point
// number 0xc000 there. The two Models at +0x530c+0x10 and +0x530c+0x60 are HOST
// C++ objects built by _ZN5ModelC1Ev, so their vptrs are MSVC's and not the
// ROM's. Three tables is the whole set, and the census below prints the raw-DS
// word count for all three.
//
// ---- 5. THE WIDTH IS 36 ON BOTH NEW TABLES, CHECKED FOUR WAYS -------------
//
// port/mg_fanout_costs.txt section 11 measured that twelve of twenty-nine
// widths in its own table were phantoms, and that a 37-slot fill writes a host
// thunk over live ROM data belonging to another class. Both tables went through
// all three of its checks plus the fourth the relocation set gives free:
//
//   table       span to next symbol    slot 35     word past the end
//   ---------   --------------------   ---------   -------------------------
//   0213e448    0213e4d8, 0x90 = 36    020ad660    0x050a0a0f, the first word
//                                                  of data_ov006_0213e4d8
//   0213e39c    0213e42c, 0x90 = 36    020ad660    0x000000c2, the first entry
//                                                  of a small-integer table
//                                                  (0xc2, 0xc4, 0xbf, 0xc0)
//
// FOURTH CHECK: exactly 36 load relocations have a source inside
// [0x0213e39c, 0x0213e42c), the first at 0x0213e39c and the last at 0x0213e428,
// and there is NO relocation at 0x0213e42c. A vtable slot is a relocated word
// by construction, so a 37th slot with no relocation is not a slot.
//
// ---- 6. THE ROW GOES LAST, AND THE ORDER IS LOAD-BEARING ------------------
//
// port_scene_registry_install walks port_scene_classes[] in table order and
// calls every row's fill on every boot; port_scene_mg_overlay_load runs the
// thirty-five overlay constructors ONCE PER PROCESS from the tail of the first
// minigame row's fill. Appending therefore means the constructors have already
// run with clean ROM words when this fill starts, which is the latent-safe
// direction port/mg_fanout_costs.txt section 11 names.
//
// FOR THIS CLASS THE HAZARD IS MEASURED ABSENT RATHER THAN AVOIDED: NOT ONE
// relocation whose source lies in ov006's .init range (0x0212f4c4..0x0213356c)
// lands anywhere in this class's code block (0x0210788c..0x0210a4ac) or its
// data span (0x0213e2dc..0x0213e42c). No overlay constructor reads a word this
// fill writes, and no overlay constructor builds a state table for this class
// either -- which is the same measurement seen from the other side, and is the
// third of the three PMF detectors section 7 runs.
//
// ---- 7. THE POINTER-TO-MEMBER WALL IS ONE TU AND ITS UNIVERSE IS TWO ------
//
// port/mg_fanout_costs.txt section 4 says no static evidence licenses the word
// "none", so all four of its prescribed detectors were run and the answer is
// not none:
//
//   - THE SOURCE, READ BEFORE THE FACE ARRAY WAS WIRED. That ordering is
//     section 14's practical rule, added after dScMgPanel_c's Behavior turned
//     out to open-code the ARM Itanium sequence in plain ints and read clean on
//     every static check. src/func_ov006_02109aac.c (slot 6) is a plain
//     `switch (*(short *)(c + 0x53e6))` over ten cases, and slot 19
//     (src/func_ov006_021096c8.c) is the ROM's own `cmp r0,#9 / addls pc,pc,
//     r0,lsl #2` jump table over the same index. Neither is the wall.
//   - A `::*` SWEEP over all 41 TUs in this class's code block finds EXACTLY
//     ONE: src/func_ov006_02107db8.cpp, the roulette wheel's tick. It is
//     host-copied in port/unmatched/MgRoulette_WheelDispatch.cpp, whose header
//     carries the derivation and the two-address universe.
//   - THE WORD-DECODE SCAN, for the third shape the sweep cannot see. Every
//     word of this class's code block was decoded looking for
//     `add Rd,Rn,Rm,asr #1`: ONE site, 0x02107e30, and it is inside
//     func_ov006_02107db8. There are five `blx Rn` sites in the block and all
//     five are accounted for -- two in func_ov006_02107d80 and one in
//     func_ov006_02107db8 are Model virtual dispatches (section 4b's shadow
//     classes, both host-copied), one at 0x02107e44 is the member-pointer call
//     itself, and one at 0x0210a3c4 is slot 0's own tail dispatch of vtable
//     slot 18.
//   - THE RELOCATION SET, which is a census rather than an absence and is one
//     grep. In this class's whole .data span exactly THREE data words carry a
//     relocation into its own code, on top of the 36 vtable slots: the
//     SpawnInfo's factory word at 0x0213e2f0, and the two mwcc pairs
//     data_ov006_0213e2e8 = {0x021082fc, 0} and data_ov006_0213e2f8 =
//     {0x02107ea8, 0}. Those two ARE the wheel's state machine and both are
//     routed.
//   - AND THE RUN IS THE ONE THAT CONVICTS. Scene 383 boots under
//     SM64DS_FAULTS_FATAL=1 with zero UNHANDLED addresses and no eip on a DS
//     address, and the census below prints the wheel's call and hit counts side
//     by side so a run that never armed the pair cannot read as green.
//
// The FRAMEWORK's wall is still the framework's and is still paid: this class
// reaches func_ov004_020b87e0 through dScMgBase_c exactly as curling does, and
// unmatched/MgBase_StateSetter.cpp's eighty routed addresses are inherited.
// So is the SHARED model sub-object's: slot 6 calls func_ov006_020c19d0 on
// this+0x4f38, whose field pair is routed by
// port/unmatched/MgMemory2_FieldPmf.cpp, already in the build.
//
// ---- 8. WHAT THIS SEAT DOES NOT CLAIM -------------------------------------
//
//   - NOBODY HAS LOOKED AT A FRAME NEXT TO A DS. The census below reports what
//     the class does; it cannot say the picture is right.
//   - THE SAVE RECORD IS THE PORT'S USUAL GAP. The port has no backup cart, so
//     SaveData::SetDefaultValuesMg zeroes data_0209caf4 on every boot. This
//     class's ov005 row is 13, param 0x00090d00, so it keys record 13 and name
//     text 9 -- and text 9 is data_ov004_020bc070[9] = 557, which decodes to
//     "Mushroom Roulette / Guess where the ball will land on the roulette
//     wheel." That is the ROM naming its own scene; it is not a claim about
//     what the port draws.
//   - TOUCH IS UNMEASURED HERE. The ROM's own rules text says the player
//     guesses where the ball lands; nothing in a headless boot places a bet.

#include "hal/screen_gap.h"

#include <cstdio>
#include <cstdlib>

extern "C" {

/* the seam into hal/scene_mg.cpp -- mounts, arm9 fill, dScMgBase_c's faces */
unsigned port_scene_mg_fill_shared(void **vt, unsigned n);
void     port_scene_mg_prepare(int id);
int      port_scene_env_want(void);

/* The mount storage the fill writes into. All three tables are the mount's own,
   so the fill replaces the mounted words rather than building a fresh host
   array -- the ov007 and curling treatment, for the same reason: a second host
   array of the same name is a duplicate symbol, and leaving the mounted table
   alone leaves live wild DS pointers in a table the factory installs. */
extern unsigned char data_ov004_020bc0c0[];   /* dScMgBase_c,         36 */
extern unsigned char data_ov006_0213e448[];   /* dScMgSingle3DBase_c, 36 */
extern unsigned char data_ov006_0213e39c[];   /* dScMgRoulette_c,     36 */
extern unsigned char data_ov006_0213e2f0[];   /* the SpawnInfo record     */

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

/* dScMgRoulette_c's own eight */
int   func_ov006_0210a194(char *c);           /* slot  0 InitResources    */
int   func_ov006_0210980c(char *c);           /* slot  3 CleanupResources */
int   func_ov006_02109aac(char *c);           /* slot  6 Behavior         */
int   func_ov006_02109834(char *c);           /* slot  9 Render           */
void *func_ov006_0210788c(char *c);           /* slot 16 D2               */
void *func_ov006_02107920(char *c);           /* slot 17 D0               */
void  func_ov006_021095cc(char *c);           /* slot 18 state reset      */
int   func_ov006_021096c8(char *c);           /* slot 19                  */

/* the factory, host-copied for the dropped receiver (MgRoulette_Factory.cpp) */
void *MgMushroomRoulette_Spawn(void);

/* the wheel's member-pointer witness, from MgRoulette_WheelDispatch.cpp */
void port_mg_roulette_wheel_counts(unsigned *calls, unsigned *hits,
                                   unsigned *spin, unsigned *settle);

/* the framework's dispatch witness, from unmatched/MgBase_StateDispatch.cpp */
void  port_mg_dispatch_counts(unsigned *calls, unsigned *unknown);

/* the shared model sub-object's field-pmf witness, from MgMemory2_FieldPmf.cpp.
   Slot 6 calls func_ov006_020c19d0 on this+0x4f38 every frame, so this class
   runs through a chain another seat owns and the honest thing is to print it
   rather than to imply this seat routed it. */
void  port_mg_memory2_field_counts(unsigned *calls, unsigned *hits);

/* The persistent minigame record. Nothing new is defined here: the storage is
   hal/level_boot.cpp's .dsstate$savblk0004 and the index function is
   src/func_ov004_020adc3c.c. */
extern void         *data_ov004_020beb68;     /* the live dScMgBase_c `this` */
extern unsigned char data_0209caf4[];         /* 36 records x 20 bytes       */
int   func_ov004_020adc3c(void *c);           /* (self->field_8 >> 8) & 0xff */

}  /* extern "C" */

static int port_mg_record_index(void)
{
    return data_ov004_020beb68 ? func_ov004_020adc3c(data_ov004_020beb68) : -1;
}

// ---- the tick witness ------------------------------------------------------
//
// One counter per dispatched slot on each of the two tables this file owns
// faces for. The instrument every scene seat in this port carries, for the
// reason hal/scene_mg.cpp gives: an object that EXISTS and an object that RUNS
// look identical from outside. The framework slots dScMgBase_c keeps are
// counted by scene_mg.cpp's own array and printed by its atexit report.
static unsigned g_rlt_hits[36];
static unsigned g_rlt_base_hits[36];   /* the same slots on the MIDDLE table */

#define RLT(n)   (++g_rlt_hits[(n)])
#define R3D(n)   (++g_rlt_base_hits[(n)])

/* ---- dScMgSingle3DBase_c's eight ---------------------------------------- */
static void *__fastcall s3_ainit(void *s, void *, unsigned f)
{ R3D(2);  return (void *)(size_t)port_mg_flower_after_init(s, f); }
static void __fastcall s3_aclean(void *s, void *, unsigned f)
{ R3D(5);  func_ov006_0210a608(s, f); }
static int  __fastcall s3_bbeh(void *s, void *)
{ R3D(7);  return func_ov006_0210a698(s); }
static int  __fastcall s3_bren(void *s, void *)
{ R3D(10); return func_ov006_0210a664(s); }
static void *__fastcall s3_d2(void *s, void *)
{ R3D(16); return (void *)(size_t)func_ov006_0210a4b0((char *)s); }
static void *__fastcall s3_d0(void *s, void *)
{ R3D(17); return (void *)(size_t)func_ov006_0210a4e8((char *)s); }
static int  __fastcall s3_v26(void *, void *)
{ R3D(26); return func_ov006_0210a600(); }
static int  __fastcall s3_v33(void *s, void *)
{ R3D(33); func_ov006_0210a708((char *)s); return 0; }

/* ---- dScMgRoulette_c's own eight ---------------------------------------- */
static int  __fastcall rlt_init(void *s, void *)
{ RLT(0);  const int r = func_ov006_0210a194((char *)s);
  /* the GaplessMinigames latch, for hal/scene_mg.cpp's reason: every seated
     minigame calls it so the ones the gapless table does not name can say
     "unsupported" instead of doing nothing quietly. */
  hal_gapless_minigames_latch(); return r; }
static int  __fastcall rlt_clean(void *s, void *)
{ RLT(3);  return func_ov006_0210980c((char *)s); }
static int  __fastcall rlt_beh(void *s, void *)
{ RLT(6);  return func_ov006_02109aac((char *)s); }
static int  __fastcall rlt_render(void *s, void *)
{ RLT(9);  return func_ov006_02109834((char *)s); }
static void *__fastcall rlt_d2(void *s, void *)
{ RLT(16); return func_ov006_0210788c((char *)s); }
static void *__fastcall rlt_d0(void *s, void *)
{ RLT(17); return func_ov006_02107920((char *)s); }
/* THE RIDE-THROUGH IS LOAD-BEARING ON BOTH OF THESE, and this class is one
   where a lane can watch it happen rather than take it on the family's word:
   slot 0's own tail dispatches slot 18 through the object's vtable at offset
   0x48 with `mvn r1,#0` -- one argument, value -1 -- at 0x0210a3bc..0x0210a3c4.
   Both ROM bodies ignore their r1 (func_ov006_021095cc takes one parameter and
   func_ov006_021096c8 takes one), so the parameter exists so __fastcall cleans
   four bytes and the callee is called without it. A thunk declared
   (void*, void*) compiles to a bare ret, leaks those four bytes, and the
   caller's own `ret` then takes a garbage return address -- which is the fault
   lane BASESET repaired across the family. runs/mg5/out/baseset/
   slot18_19_scan.txt is the census: 22 slot-18 sites and 14 slot-19 sites,
   argument count ONE at every one of them. */
static int  __fastcall rlt_reset(void *s, void *, int /*ridethrough*/)
{ RLT(18); func_ov006_021095cc((char *)s); return 1; }
static int  __fastcall rlt_v19(void *s, void *, int /*ridethrough*/)
{ RLT(19); return func_ov006_021096c8((char *)s); }

/* SM64DS_SCENE_SLOT0=0 and SM64DS_SCENE_SLOT9=0, the diagnostics every scene
   seat in this port carries, counted separately so a run can never read a
   no-op as the real body having run. */
static unsigned g_rlt_init_skipped, g_rlt_render_skipped;
static int __fastcall rlt_init_noop(void *, void *)
{ ++g_rlt_init_skipped; return 1; }
static int __fastcall rlt_render_noop(void *, void *)
{ ++g_rlt_render_skipped; return 1; }

struct RltFace { unsigned ds; void *host; };

static const RltFace kSingle3DFaces[] = {
    {0x0210a6e4u, (void *)s3_ainit},  {0x0210a608u, (void *)s3_aclean},
    {0x0210a698u, (void *)s3_bbeh},   {0x0210a664u, (void *)s3_bren},
    {0x0210a4b0u, (void *)s3_d2},     {0x0210a4e8u, (void *)s3_d0},
    {0x0210a600u, (void *)s3_v26},    {0x0210a708u, (void *)s3_v33},
};

static const RltFace kRouletteFaces[] = {
    {0x0210a194u, (void *)rlt_init},   {0x0210980cu, (void *)rlt_clean},
    {0x02109aacu, (void *)rlt_beh},    {0x02109834u, (void *)rlt_render},
    {0x0210788cu, (void *)rlt_d2},     {0x02107920u, (void *)rlt_d0},
    {0x021095ccu, (void *)rlt_reset},  {0x021096c8u, (void *)rlt_v19},
};

static unsigned rlt_apply(void **vt, unsigned n, const RltFace *f, unsigned nf)
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
static unsigned rlt_raw_left(void **vt, unsigned n)
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
static unsigned g_rlt_mid_claimed, g_rlt_vt_claimed;

extern "C" void port_scene_roulette_hits(void);

extern "C" void port_scene_fill_roulette(void)
{
    void **base = (void **)data_ov004_020bc0c0;
    void **mid  = (void **)data_ov006_0213e448;
    void **vt   = (void **)data_ov006_0213e39c;

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
       arrays here is safe beside the four earlier copies, and why the claimed
       count below is expected to be ZERO on a tree carrying them. */
    port_scene_mg_fill_shared(mid, 36);
    g_rlt_mid_claimed =
        rlt_apply(mid, 36, kSingle3DFaces,
                  sizeof kSingle3DFaces / sizeof kSingle3DFaces[0])
      + rlt_apply(mid, 36, kRouletteFaces,
                  sizeof kRouletteFaces / sizeof kRouletteFaces[0]);

    /* THE DERIVED TABLE. Shared arm9 and dScMgBase_c words first, then the
       intermediate base's eight, then this class's own eight. Order does not
       matter -- the three key sets are disjoint, because a word is one address
       -- but it reads in inheritance order. */
    port_scene_mg_fill_shared(vt, 36);
    g_rlt_vt_claimed =
        rlt_apply(vt, 36, kSingle3DFaces,
                  sizeof kSingle3DFaces / sizeof kSingle3DFaces[0])
      + rlt_apply(vt, 36, kRouletteFaces,
                  sizeof kRouletteFaces / sizeof kRouletteFaces[0]);

    /* the two diagnostics, applied after the fill so they override it */
    {
        const char *s0 = std::getenv("SM64DS_SCENE_SLOT0");
        const char *s9 = std::getenv("SM64DS_SCENE_SLOT9");
        if (s0 && s0[0] == '0') vt[0] = (void *)rlt_init_noop;
        if (s9 && s9[0] == '0') vt[9] = (void *)rlt_render_noop;
    }

    {
        const unsigned lb = rlt_raw_left(base, 36);
        const unsigned lm = rlt_raw_left(mid, 36);
        const unsigned lv = rlt_raw_left(vt, 36);
        if (lb || lm || lv) {
            std::fprintf(stderr, "  [scene] ROULETTE FILL INCOMPLETE: "
                         "dScMgBase_c leaves %u of 36 raw DS words, "
                         "dScMgSingle3DBase_c %u, dScMgRoulette_c %u. A "
                         "dispatch of any of them jumps to a DS address as a "
                         "host one.\n", lb, lm, lv);
            std::fflush(stderr);
        }
    }

    /* The constructors, gated on the requested id, exactly as every other
       minigame fill does it. Idempotent behind scene_mg.cpp's own
       once-per-process guard. */
    port_scene_mg_prepare(port_scene_env_want());

    if (port_scene_env_want() == 383) {
        static int armed;
        if (!armed) {
            armed = 1;
            std::atexit(port_scene_roulette_hits);
        }
    }
}

/* The registry's factory column is void *(*)(void) and so is the factory, but
   the forwarder is kept for the reason title_spawn and port_mg_curling_spawn
   are: it gives the seat one place to observe the object without the registry
   table growing a second column. The factory it calls is the HOST COPY in
   port/unmatched/MgRoulette_Factory.cpp, not src -- see that file's header for
   the displacement ruling. */
static char *g_rlt_self;

extern "C" void *port_mg_roulette_spawn(void)
{
    void *p = MgMushroomRoulette_Spawn();
    g_rlt_self = (char *)p;
    return p;
}

extern "C" void port_scene_roulette_hits(void)
{
    unsigned total = 0, mtotal = 0;
    for (int i = 0; i < 36; ++i) {
        total  += g_rlt_hits[i];
        mtotal += g_rlt_base_hits[i];
    }

    std::printf("[scene] dScMgRoulette_c slot hits: init %u, cleanup %u, "
                "behavior %u, render %u, D2 %u, D0 %u, state-reset %u, "
                "slot19 %u%s\n",
                g_rlt_hits[0], g_rlt_hits[3], g_rlt_hits[6], g_rlt_hits[9],
                g_rlt_hits[16], g_rlt_hits[17], g_rlt_hits[18], g_rlt_hits[19],
                g_rlt_render_skipped
                    ? "  [RENDER SLOT NO-OP'd: SM64DS_SCENE_SLOT9=0]" : "");
    if (g_rlt_init_skipped)
        std::printf("[scene] INIT SLOT NO-OP'd: SM64DS_SCENE_SLOT0=0, %u "
                    "time(s)\n", g_rlt_init_skipped);

    std::printf("[scene] dScMgRoulette_c own slots entered:");
    for (int i = 0; i < 36; ++i)
        if (g_rlt_hits[i]) std::printf(" %d(x%u)", i, g_rlt_hits[i]);
    std::printf("   (%u total)\n", total);

    std::printf("[scene] dScMgSingle3DBase_c slots entered (this seat's copy):");
    for (int i = 0; i < 36; ++i)
        if (g_rlt_base_hits[i]) std::printf(" %d(x%u)", i, g_rlt_base_hits[i]);
    std::printf("   (%u total)\n", mtotal);

    /* Section 3's claim, measured rather than asserted: on a tree carrying the
       flower, memory2, luckystars and mcarlo rows this seat's middle copy
       should claim ZERO slots and its derived copy should claim FOURTEEN of the
       sixteen face rows -- this class's own eight plus six of the middle base's
       eight. The two that do not land are that base's own D2 and D0
       (0x0210a4b0 and 0x0210a4e8): this class overrides slots 16 and 17 with
       bodies of its own, so those two words are not in the derived table to be
       claimed. Fourteen is also this class's override count in
       port/mg_fanout_costs.txt section 3, which is the cross-check. */
    std::printf("[scene] dScMgRoulette_c fill claims: middle table %u slot(s), "
                "derived table %u slot(s) (of 8 + 8 face rows)\n",
                g_rlt_mid_claimed, g_rlt_vt_claimed);

    /* THE WHEEL'S OWN WITNESS, and it is the number that separates "the class
       ticked" from "the wheel turned". The member pointer at +0x530c is armed
       by slot 18 with 0x021082fc and handed on to 0x02107ea8 by that state
       itself, so a run whose spin count is zero never armed it and a run whose
       settle count is zero never finished a spin. calls-minus-hits is what fell
       through to the framework, which is where an UNHANDLED address would be
       reported. */
    {
        unsigned calls = 0, hits = 0, spin = 0, settle = 0;
        port_mg_roulette_wheel_counts(&calls, &hits, &spin, &settle);
        std::printf("[scene] dScMgRoulette_c wheel dispatch (+0x530c member "
                    "pointer): %u call(s), %u routed, spin 0x021082fc x%u, "
                    "settle 0x02107ea8 x%u\n", calls, hits, spin, settle);
    }

    /* THE STATE MACHINE'S OWN WITNESS. This class dispatches its scene-level
       states through a plain compiled switch on the s16 at +0x53e6 rather than
       through a member-pointer table, so there is no address switch to count
       and the honest instrument is the INDEX ITSELF: slot 18 sets it to 1,
       slot 6 carries it through cases 0 to 4 and slot 19 through 5 to 9. A run
       whose index never moves has ticked the object without running its state
       machine, and that reads as a success unless it is said out loud. */
    if (g_rlt_self) {
        const char *c = g_rlt_self;
        std::printf("[scene] dScMgRoulette_c state index +0x53e6 = %d, timer "
                    "+0x53e8 = %d, board mode +0x53e4 = %d, ball sprite "
                    "+0x53d6 = %d, spawned %d of %d wheel record(s) "
                    "(+0x53f8 / +0x53fc)\n",
                    *(const short *)(c + 0x53e6), *(const short *)(c + 0x53e8),
                    *(const short *)(c + 0x53e4), *(const short *)(c + 0x53d6),
                    *(const int *)(c + 0x53f8), *(const int *)(c + 0x53fc));
        std::printf("[scene] dScMgRoulette_c wheel object +0x530c: angle "
                    "+0xc2 = %d, velocity +0xc8 = %d, target +0xb8 = %d, "
                    "sound gate +0xcc = %d, member pointer = "
                    "{0x%08x, %d}\n",
                    *(const short *)(c + 0x530c + 0xc2),
                    *(const short *)(c + 0x530c + 0xc8),
                    *(const int *)(c + 0x530c + 0xb8),
                    *(const short *)(c + 0x530c + 0xcc),
                    *(const unsigned *)(c + 0x530c),
                    *(const int *)(c + 0x530c + 4));
    }

    /* THE FIVE RECORDS THE FACTORY BUILDS, printed for section 4b's reason and
       for port/mg_fanout_costs.txt section 15's: a census that measures only
       DISPATCH cannot see a board that was never dealt. Slot 18's
       func_ov006_021092a0 writes 0xc000 into words 0 and 1 of each, so a record
       still reading zero was never initialised. */
    if (g_rlt_self) {
        unsigned live = 0;
        for (int i = 0; i < 5; ++i)
            if (*(const int *)(g_rlt_self + 0x51a8 + i * 0x34))
                ++live;
        std::printf("[scene] dScMgRoulette_c record array +0x51a8: %u of 5 "
                    "initialised; [0] = { %d, %d } [1] = { %d, %d }\n", live,
                    *(const int *)(g_rlt_self + 0x51a8 + 0x00),
                    *(const int *)(g_rlt_self + 0x51a8 + 0x04),
                    *(const int *)(g_rlt_self + 0x51a8 + 0x34),
                    *(const int *)(g_rlt_self + 0x51a8 + 0x38));
    }

    /* The two dispatch chains this class runs through that other seats own.
       Printed rather than folded into one number, because a zero in either
       means something different from a zero in the other. */
    {
        unsigned calls = 0, unknown = 0, fcalls = 0, fhits = 0;
        port_mg_dispatch_counts(&calls, &unknown);
        port_mg_memory2_field_counts(&fcalls, &fhits);
        std::printf("[scene] dScMgRoulette_c inherited chains: %u framework "
                    "call(s) with %u UNHANDLED address(es); %u shared model "
                    "sub-object field call(s), %u routed\n",
                    calls, unknown, fcalls, fhits);
    }

    /* THE SAVED PROGRESS, both ends. The port has no backup cart so the record
       is zeroed on every boot; printing it says which of the two moved. */
    {
        const int idx = port_mg_record_index();
        if (idx >= 0 && idx < 36) {
            const int *rec = (const int *)(data_0209caf4 + idx * 20);
            std::printf("[scene] dScMgRoulette_c save record data_0209caf4[%d] "
                        "= { %d, %d, %d, %d, %d }\n", idx,
                        rec[0], rec[1], rec[2], rec[3], rec[4]);
        } else {
            std::printf("[scene] dScMgRoulette_c save record: NO minigame index "
                        "(data_ov004_020beb68 = %p)\n", data_ov004_020beb68);
        }
    }
    std::fflush(stdout);
}

/* ---- THE NAME-SPELLING ALIASES -------------------------------------------
 *
 * The whole of this class's link wall, and it is four rows in one wave. The
 * "C-named symbols declared at C++ linkage" case hal/scene_boot.cpp section 1
 * carries twenty-three of for the star select and port/mg_fanout_costs.txt
 * section 4 counts twenty-two of in the family's own wall.
 *
 * THEY ARE ALIASES AND NOT FACES, and the test is section 4's rulebook
 * corollary rather than habit: an alias is safe when the two spellings describe
 * the same bytes at the same address, and unsafe when either names a
 * member-pointer type, where mwcc's eight-byte pair and MSVC's four-byte one
 * disagree on stride. NONE OF THESE FOUR IS A MEMBER POINTER. This class HAS a
 * member pointer -- the pair at offset 0 of the wheel object at +0x530c -- and
 * it is host-copied in port/unmatched/MgRoulette_WheelDispatch.cpp rather than
 * aliased, which is the distinction the corollary exists to make.
 *
 * THE FIRST THREE are src/func_ov006_02107a6c.cpp's, a //cpp TU that declares
 * them at NAMESPACE scope rather than inside its extern "C" block, so MSVC puts
 * the TYPE into the symbol name and the ov006 mount's plain C definition cannot
 * satisfy it. What each one IS, checked before the row was written:
 *
 *   data_ov006_021428d0  a .bss int, the retry countdown this body approaches
 *                        with _Z14ApproachLinearRiii and resets to 0x78
 *   data_ov006_0213e2dc  a .data int reading 0xffffffff, this class's ROUND
 *                        COUNTER -- the body increments it and returns
 *                        (counter >= 4). AND IT IS A NAMED VICTIM OF THE WIDTH
 *                        HAZARD: hal/scene_mg.cpp's dScMgPanel_c block records
 *                        that "word 36 reads ffffffff, which is its own config
 *                        symbol data_ov006_0213e2dc and not a code address" --
 *                        0x0213e24c + 36 words IS 0x0213e2dc, so a 37-slot fill
 *                        of dScMgPanel_c would write a host thunk over this
 *                        class's live round counter. port/mg_fanout_costs.txt
 *                        section 11 names 0x169's and 0x16c's victims; this is
 *                        a third, found from the other end.
 *   data_ov006_02142aa4  a .bss array of FOUR pointers, which
 *                        func_ov006_02107b70 fills with this+0x52ac, +0x52c4,
 *                        +0x52dc and +0x52f4 -- four 0x18-byte sub-objects
 *                        inside the scene object. Four is the count the body
 *                        loops to and the span to the next config symbol
 *                        (0x02142ab4) is exactly 0x10, so the declared bound
 *                        and the ROM agree.
 *
 * THE FOURTH IS THE BARE-NAME SHAPE, the one MgCoin_Faces.cpp and
 * MgMemory2_Faces.cpp already carry two of. src/func_ov006_02108f2c.c declares
 * `extern s32 func_020b9488;` -- a DATA word under a func_ name that exists in
 * no config. Its own relocation says what it is: from inside this class's code
 * block, kind:load to:0x020b9488 module:overlays(0,4), and ov004's symbols.txt
 * has data_ov004_020b9488 at exactly that address.
 */
#pragma comment(linker, "/alternatename:?data_ov006_021428d0@@3HA=_data_ov006_021428d0")
#pragma comment(linker, "/alternatename:?data_ov006_0213e2dc@@3HA=_data_ov006_0213e2dc")
#pragma comment(linker, "/alternatename:?data_ov006_02142aa4@@3PAPADA=_data_ov006_02142aa4")
#pragma comment(linker, "/alternatename:_func_020b9488=_data_ov004_020b9488")
