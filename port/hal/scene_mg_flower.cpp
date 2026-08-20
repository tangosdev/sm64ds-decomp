// dScMgFlower_c, the "Loves Me...?" petal minigame -- actor id 0x186, scene
// 390. Run mg5, lane FLW. The SECOND ov006 minigame class the port seats and
// the first with a THREE-DEEP hierarchy.
//
// Read port/slice_flw.txt for the identity derivation, the three width checks
// and the closure. Read port/mg_fanout_costs.txt for what the family costs.
// This file is the seat: the two new vtables' faces, the fill, the factory
// forwarder, the eleven name-spelling aliases the link asked for, and one
// named trap.
//
// ---- 1. WHY THIS IS A SEPARATE FILE FROM hal/scene_mg.cpp -----------------
//
// scene_mg.cpp is the FAMILY half: the mounts, the thirty-five overlay
// constructors, dScMgBase_c's twenty-eight address-keyed faces and the gate
// that keeps the constructors off a level boot. All of that is paid once and
// none of it is re-derived here. This file reaches it through the one seam
// that file exports, port_scene_mg_fill_shared(), and adds only what is this
// class's.
//
// The alternative -- a second kCurlingFaces-shaped block inside scene_mg.cpp
// -- is what the fan-out would collide on: five lanes appending five arrays to
// one file is five merges of the same region, and a copied kMgBaseFaces would
// silently split the per-slot witness in two. The seam is one function.
//
// ---- 2. THE HIERARCHY IS THREE DEEP, AND THE ROM SAYS SO TWICE ------------
//
//     Scene -> dScMgBase_c         data_ov004_020bc0c0  36 slots
//           -> dScMgSingle3DBase_c data_ov006_0213e448  36 slots
//           -> dScMgFlower_c       data_ov006_02140140  36 slots
//
// Both edges are read off code rather than inferred from names. The factory
// func_ov006_0212b7f8 calls func_ov004_020b2adc (dScMgBase_c's constructor),
// then writes 0x0213e448 into the object's first word, then writes 0x02140140
// over it; the deleting destructor func_ov006_0212a5c8 (slot 17) unwinds them
// in the opposite order, 0x02140140 first and 0x0213e448 second. A hierarchy
// written by the constructor and unwritten by the destructor in reverse is not
// a guess.
//
// The middle table is the SAME OBJECT dScMgCup_c (0x169) sits under -- see
// port/slice_mga361.txt, which derived it independently and agrees word for
// word -- so seating it here pays for that class too.
//
// ---- 3. THE FILL IS ADDRESS-KEYED AND THAT IS WHY THREE TABLES ARE CHEAP --
//
// port_scene_fill_rom and mg_apply both key on the ROM WORD a slot holds, not
// on the slot number, so the same face array can be applied to every table in
// the chain and can only land where the ROM parked that exact body. The two
// arrays below are disjoint by construction:
//
//   kSingle3DFaces  the eight bodies dScMgSingle3DBase_c overrides
//                   (slots 2, 5, 7, 10, 16, 17, 26, 33)
//   kFlowerFaces    the six dScMgFlower_c adds or replaces on top
//                   (slots 0, 6, 9, 16, 17, 18)
//
// Slots 16 and 17 appear in both and hold DIFFERENT addresses -- 0x0210a4b0 /
// 0x0210a4e8 in the base, 0x0212a554 / 0x0212a5c8 in the derived -- so both
// arrays can be applied to both tables and each writes only its own.
//
// ---- 4. THE WIDTH IS 36 ON BOTH NEW TABLES, CHECKED THREE WAYS ------------
//
// port/mg_fanout_costs.txt section 11 measured that twelve of twenty-nine
// widths in its own table were phantoms, and that a 37-slot fill writes a host
// thunk over live ROM data belonging to another class. Both tables were put
// through all three of its checks before a word was written:
//
//   table       span to next symbol   slot 35     word past the end
//   ---------   -------------------   ---------   ---------------------------
//   0213e448    0213e4d8, 36 words    020ad660    0x050a0a0f, the first word
//                                                 of data_ov006_0213e4d8
//   02140140    021401d0, 36 words    020ad660    0x2f474d2f, the ASCII "/MG/"
//                                                 of this class's own file
//                                                 path string
//
// AND THE SECTION-11 HAZARD IS MEASURED ABSENT RATHER THAN ASSUMED. Its
// mechanism is a fill that writes into a word an overlay CONSTRUCTOR later
// copies into a .bss dispatch table. Zero relocations leave ov006's .init code
// (0x0212f4c4..0x02133600) for either table's span, so no constructor reads
// either table and the row-order question section 11 raises cannot arise here.
// (It is not vacuous: two words past the END of 0x0213e448, at 0x0213e4e0,
// sits {0x0210adac, 0} -- a real mwcc member-pointer pair belonging to
// something else. A 38-slot fill would have landed on it.)
//
// ---- 5. THE ROW GOES LAST, AND THE ORDER IS LOAD-BEARING ------------------
//
// port_scene_registry_install walks port_scene_classes[] in table order and
// calls every row's fill on every boot; port_scene_mg_overlay_load runs the
// thirty-five constructors ONCE PER PROCESS from the tail of the first
// minigame row's fill. Appending after curling therefore means the
// constructors have already run with clean ROM words when this fill starts,
// which is the safe direction section 11 names.
//
// ---- 6. WHAT THIS CLASS DOES NOT HAVE: A POINTER-TO-MEMBER WALL -----------
//
// port/mg_fanout_costs.txt section 4 opens "THE WALL, AND IT IS THE SAME ONE
// FOR ALL THIRTY". It is not this one's, and the measurement is in
// port/slice_flw.txt: slot 6 is a plain `switch (self->unk_5fe8)` with two
// cases, no overlay constructor builds a pair table for this class, and a
// `::*` sweep over every TU in the slice is clean. So there is no address
// switch here, no MgFlower_StateDispatch.cpp, and no stategen output --
// stategen has no constructor of this class's to read.
//
// The FRAMEWORK's wall is still the framework's and is still paid: this class
// reaches func_ov004_020b87e0 through dScMgBase_c exactly as curling does, and
// unmatched/MgBase_StateDispatch.cpp's address switch is inherited unchanged.
// AMENDED, run mg5 lane BASESET: the trap that used to stand in
// hal/scene_mg_faces.cpp beside that switch is GONE. The setter is host-copied
// in unmatched/MgBase_StateSetter.cpp and its forty addresses are routed, so
// this class inherits a framework state machine that runs.
//
// ---- 7. ONE HARD FLOOR, AND IT IS NOT IN THE VTABLE -----------------------
//
// func_ov006_0212a764 (0x310 bytes) has a config symbol in
// config/arm9/overlays/ov006/symbols.txt, NO delink block in that overlay's
// delinks.txt -- the block before it ends exactly at 0x0212a764 and the next
// starts at 0x0212aa74 -- and no src file anywhere defines it. Two matched TUs
// call it, and both are on this class's live path:
//
//     from:0x0212b73c  inside func_ov006_0212b480, slot 0 InitResources
//     from:0x0212aab8  inside func_ov006_0212aa74, slot 18 the state reset
//
// It is this class's analogue of curling's func_ov006_020e1854 and it is a
// DECOMP gap, not a port one. It gets a named trap below and deliberately not
// a plausible body: port/tools/inferred_stub_guard.py exists to refuse exactly
// the invention that would go here. What the trap costs is measurable rather
// than mysterious -- disassembled at base 0x020bfec0 the body zeroes the
// cursor at +0x5fb8..+0x5fc4, calls func_ov004_020b04d0(0x20), clears all
// twenty-two petal records at +0x4f38 stride 0x20, then lays them out with six
// calls to the arm9 trig helper at 0x0203b990, one ITCM divide and a
// G2x::SetBlendAlpha -- so with the trap in place the petals stay zeroed,
// which reads as "no petal on the board" to both the Behavior and the Render
// loops. The class boots and ticks; it just has no petals.
//
// RETIRING IT IS A TRANSCRIPTION, and the port has done that twice already for
// this exact shape: unmatched/MgCurling_State_020e1854.cpp (lane CT1) and
// unmatched/MgCurling_Collide_020e1dc8.cpp (lane CUR2), both carrying port_
// names so the decomp's own accounting is unchanged. This lane did not take it.

#include "hal/screen_gap.h"

#include <cstdio>
#include <cstdlib>

extern "C" {

/* the seam into hal/scene_mg.cpp -- mounts, arm9 fill, dScMgBase_c's faces */
unsigned port_scene_mg_fill_shared(void **vt, unsigned n);
void     port_scene_mg_prepare(int id);
int      port_scene_env_want(void);
int      IsMinigameActorID(unsigned int id);

/* the mount storage the fill writes into. Both are inside the ov006 mount, so
   the fill replaces the mount's own words rather than building a fresh host
   array -- the ov007 and curling treatment, for the same reason: a second host
   array of the same name is a duplicate symbol, and leaving the mounted table
   alone leaves live wild DS pointers in a table the factory installs. */
extern unsigned char data_ov006_0213e448[];   /* dScMgSingle3DBase_c, 36 */
extern unsigned char data_ov006_02140140[];   /* dScMgFlower_c,       36 */
extern unsigned char data_ov004_020bc0c0[];   /* dScMgBase_c,         36 */
extern unsigned char data_ov006_02140114[];   /* the SpawnInfo record     */


/* dScMgSingle3DBase_c's eight overrides */
void  func_ov006_0210a608(void *c, unsigned f);   /* slot  5 */
int   func_ov006_0210a698(void *c);               /* slot  7 */
int   func_ov006_0210a664(void *c);               /* slot 10 */
int   func_ov006_0210a4b0(char *c);               /* slot 16 D2 */
int   func_ov006_0210a4e8(char *c);               /* slot 17 D0 */
int   func_ov006_0210a600(void);                  /* slot 26 */
void  func_ov006_0210a708(char *c);               /* slot 33 */
/* slot 2 is NOT src's body: see unmatched/MgFlower_Slot2.cpp */
int   port_mg_flower_after_init(void *c, unsigned f);

/* dScMgFlower_c's six own overrides */
int   func_ov006_0212b480(void *c);               /* slot  0 InitResources */
int   func_ov006_0212ac74(char *c);               /* slot  6 Behavior      */
int   func_ov006_0212aacc(char *c);               /* slot  9 Render        */
void *func_ov006_0212a554(char *c);               /* slot 16 D2            */
void *func_ov006_0212a5c8(char *c);               /* slot 17 D0            */
void  func_ov006_0212aa74(char *c);               /* slot 18 state reset   */

/* the factory */
void *func_ov006_0212b7f8(void);

}  /* extern "C" */

// ---- the tick witness ------------------------------------------------------
//
// One counter per dispatched slot, the instrument every scene seat in this
// port carries, and the reason is the same one hal/scene_mg.cpp gives: an
// object that EXISTS and an object that RUNS look identical from outside.
// These count only the slots THIS file's faces own; the framework slots
// dScMgBase_c keeps are counted by scene_mg.cpp's own array and printed by its
// atexit report, which a scene-390 boot also registers because 390 is an id
// IsMinigameActorID accepts.
static unsigned g_flw_hits[36];
static unsigned g_flw_base_hits[36];   /* the same slots on the MIDDLE table */

#define FLW(n)  (++g_flw_hits[(n)])
#define S3D(n)  (++g_flw_base_hits[(n)])

/* ---- dScMgSingle3DBase_c's eight ---------------------------------------- */
static void *__fastcall s3_ainit(void *s, void *, unsigned f)
{ S3D(2);  return (void *)(size_t)port_mg_flower_after_init(s, f); }
static void __fastcall s3_aclean(void *s, void *, unsigned f)
{ S3D(5);  func_ov006_0210a608(s, f); }
static int  __fastcall s3_bbeh(void *s, void *)
{ S3D(7);  return func_ov006_0210a698(s); }
static int  __fastcall s3_bren(void *s, void *)
{ S3D(10); return func_ov006_0210a664(s); }
static void *__fastcall s3_d2(void *s, void *)
{ S3D(16); return (void *)(size_t)func_ov006_0210a4b0((char *)s); }
static void *__fastcall s3_d0(void *s, void *)
{ S3D(17); return (void *)(size_t)func_ov006_0210a4e8((char *)s); }
static int  __fastcall s3_v26(void *, void *)
{ S3D(26); return func_ov006_0210a600(); }
static int  __fastcall s3_v33(void *s, void *)
{ S3D(33); func_ov006_0210a708((char *)s); return 0; }

/* ---- dScMgFlower_c's six ------------------------------------------------ */
/* ---- THE BLOCKER THAT WAS HERE IS RETIRED -------------------------------
   A pre-flight check stood here and printed a named line whenever
   data_ov004_020beb68 was non-zero, because func_ov004_020ad8b8 then called
   func_ov004_020adc3c with no argument and the host callee read the stack.
   Under the coordinator's granted displacement ruling that call now places
   its argument, in port/unmatched/MgFlower_InitScore.cpp, so the condition
   the check was keyed on is the ORDINARY case and a line keyed on it would
   print on every healthy boot.

   SO IT IS DELETED RATHER THAN LEFT ANSWERING OVER A SEATED FIX, which is
   what run link60 Stage 5 lane SEAT8 did with the "MINIGAME FADE MOTION
   MISSING" advisory for the same reason. The derivation, the ROM
   disassembly and the fault it produced travel with the host copy, where a
   reader who wants the history will look for it. The live assertion that
   replaces it is port/tools/battery.py's scene-390 row, which is an ordinary
   selftest now: if the host copy is ever dropped, scene 390 faults in
   InitResources again and that row goes red on the next battery. */
static int  __fastcall flw_init(void *s, void *)
{ FLW(0);  const int r = func_ov006_0212b480(s);
  /* the GaplessMinigames latch, for hal/scene_mg.cpp's reason: every seated
     minigame calls it so the ones the gapless table does not name can say
     "unsupported" instead of doing nothing quietly. 0x180 is not in that
     table -- its G is 16 and nobody has walked its consumers. */
  hal_gapless_minigames_latch(); return r; }
static int  __fastcall flw_beh(void *s, void *)
{ FLW(6);  return func_ov006_0212ac74((char *)s); }
static int  __fastcall flw_render(void *s, void *)
{ FLW(9);  return func_ov006_0212aacc((char *)s); }
static void *__fastcall flw_d2(void *s, void *)
{ FLW(16); return func_ov006_0212a554((char *)s); }
static void *__fastcall flw_d0(void *s, void *)
{ FLW(17); return func_ov006_0212a5c8((char *)s); }
static int  __fastcall flw_reset(void *s, void *)
{ FLW(18); func_ov006_0212aa74((char *)s); return 1; }

/* SM64DS_SCENE_SLOT0=0 and SM64DS_SCENE_SLOT9=0, the diagnostics the ov003,
   ov007 and curling seats all carry, counted separately so a run can never
   read a no-op as the real body having run. */
static unsigned g_flw_init_skipped, g_flw_render_skipped;
static int __fastcall flw_init_noop(void *, void *)
{ ++g_flw_init_skipped; return 1; }
static int __fastcall flw_render_noop(void *, void *)
{ ++g_flw_render_skipped; return 1; }

struct FlwFace { unsigned ds; void *host; };

static const FlwFace kSingle3DFaces[] = {
    {0x0210a6e4u, (void *)s3_ainit},  {0x0210a608u, (void *)s3_aclean},
    {0x0210a698u, (void *)s3_bbeh},   {0x0210a664u, (void *)s3_bren},
    {0x0210a4b0u, (void *)s3_d2},     {0x0210a4e8u, (void *)s3_d0},
    {0x0210a600u, (void *)s3_v26},    {0x0210a708u, (void *)s3_v33},
};

static const FlwFace kFlowerFaces[] = {
    {0x0212b480u, (void *)flw_init},  {0x0212ac74u, (void *)flw_beh},
    {0x0212aaccu, (void *)flw_render},{0x0212a554u, (void *)flw_d2},
    {0x0212a5c8u, (void *)flw_d0},    {0x0212aa74u, (void *)flw_reset},
};

static unsigned flw_apply(void **vt, unsigned n, const FlwFace *f, unsigned nf)
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
static unsigned flw_raw_left(void **vt, unsigned n)
{
    unsigned left = 0;
    for (unsigned i = 0; i < n; ++i) {
        const unsigned w = (unsigned)(size_t)vt[i];
        if (w >= 0x02000000u && w < 0x02400000u)
            ++left;
    }
    return left;
}

extern "C" void port_scene_flower_hits(void);

extern "C" void port_scene_fill_flower(void)
{
    void **base = (void **)data_ov004_020bc0c0;
    void **mid  = (void **)data_ov006_0213e448;
    void **vt   = (void **)data_ov006_02140140;

    /* THE BASE TABLE IS FILLED HERE TOO AND IT IS NOT CEREMONY. The curling
       row's fill already does it and runs first, so on a tree that carries
       both rows this is a second pass over words that are already host
       pointers and finds nothing -- port_scene_fill_rom and flw_apply both key
       on a DS word, and there are none left. It is here so that this class
       does not depend on another class's row existing: the factory's first act
       is func_ov004_020b2adc, which writes data_ov004_020bc0c0 into the
       object's first word before the derived tables land, and thirty-six raw
       DS words in a table the ROM installs is what produced the ov007 lane's
       "eip 0x01cccab4 accessing 0x020ccab4" fault. */
    port_scene_mg_fill_shared(base, 36);

    /* THE MIDDLE TABLE. Same treatment, and it is live storage in its own
       right: the destructor chain writes it into the object on the way down,
       so a slot dispatched between the two vptr writes reads it. */
    port_scene_mg_fill_shared(mid, 36);
    flw_apply(mid, 36, kSingle3DFaces,
              sizeof kSingle3DFaces / sizeof kSingle3DFaces[0]);
    flw_apply(mid, 36, kFlowerFaces,
              sizeof kFlowerFaces / sizeof kFlowerFaces[0]);

    /* THE DERIVED TABLE. Shared arm9 and dScMgBase_c words first, then the
       intermediate base's eight, then this class's six. Order does not matter
       -- the three key sets are disjoint, because a word is one address -- but
       it reads in inheritance order. */
    port_scene_mg_fill_shared(vt, 36);
    flw_apply(vt, 36, kSingle3DFaces,
              sizeof kSingle3DFaces / sizeof kSingle3DFaces[0]);
    flw_apply(vt, 36, kFlowerFaces,
              sizeof kFlowerFaces / sizeof kFlowerFaces[0]);

    /* the two diagnostics, applied after the fill so they override it */
    {
        const char *s0 = std::getenv("SM64DS_SCENE_SLOT0");
        const char *s9 = std::getenv("SM64DS_SCENE_SLOT9");
        if (s0 && s0[0] == '0') vt[0] = (void *)flw_init_noop;
        if (s9 && s9[0] == '0') vt[9] = (void *)flw_render_noop;
    }

    {
        const unsigned lb = flw_raw_left(base, 36);
        const unsigned lm = flw_raw_left(mid, 36);
        const unsigned lv = flw_raw_left(vt, 36);
        if (lb || lm || lv) {
            std::fprintf(stderr, "  [scene] FLOWER FILL INCOMPLETE: "
                         "dScMgBase_c leaves %u of 36 raw DS words, "
                         "dScMgSingle3DBase_c %u, dScMgFlower_c %u. A dispatch "
                         "of any of them jumps to a DS address as a host one.\n",
                         lb, lm, lv);
            std::fflush(stderr);
        }
    }

    /* The constructors, gated on the requested id, exactly as the curling fill
       does it. Idempotent behind scene_mg.cpp's own once-per-process guard, so
       on a boot that carries both rows this is a no-op and on a tree that ever
       drops the curling row it is still correct. */
    port_scene_mg_prepare(port_scene_env_want());


    if (IsMinigameActorID((unsigned)port_scene_env_want())) {
        static int armed;
        if (!armed) {
            armed = 1;
            std::atexit(port_scene_flower_hits);
        }
    }
}

/* The registry's factory column is void *(*)(void) and so is the matched
   factory, but the forwarder is kept anyway for the reason title_spawn and
   port_mg_curling_spawn are: it gives the seat one place to observe the object
   without the registry table growing a second column. */
static void *g_flw_self;

extern "C" void *port_mg_flower_spawn(void)
{
    void *p = func_ov006_0212b7f8();
    g_flw_self = p;
    return p;
}

extern "C" unsigned port_mg_flower_trap_hits(void);
/* unmatched/MgFlower_SubDispatch.cpp's own counters. The class's slot-6 switch
   is plain C and needs no instrument beyond the state variable below, but the
   SUB-OBJECT at +0x51f8 is dispatched through a member pointer and does, for
   MgBase_StateDispatch's reason: a run with slot hits and zero dispatch calls
   has ticked the object without entering its state machine, and that reads as
   a success unless it is printed. */
extern "C" unsigned port_mg_flower_sub_calls(void);
extern "C" unsigned port_mg_flower_sub_unknown(void);

extern "C" void port_scene_flower_hits(void)
{
    unsigned total = 0, mtotal = 0;
    for (int i = 0; i < 36; ++i) { total += g_flw_hits[i]; mtotal += g_flw_base_hits[i]; }

    std::printf("[scene] dScMgFlower_c slot hits: init %u, behavior %u, "
                "render %u, state-reset %u%s\n",
                g_flw_hits[0], g_flw_hits[6], g_flw_hits[9], g_flw_hits[18],
                g_flw_render_skipped
                    ? "  [RENDER SLOT NO-OP'd: SM64DS_SCENE_SLOT9=0]" : "");
    if (g_flw_init_skipped)
        std::printf("[scene] INIT SLOT NO-OP'd: SM64DS_SCENE_SLOT0=0, %u "
                    "time(s)\n", g_flw_init_skipped);

    std::printf("[scene] dScMgFlower_c own slots entered:");
    for (int i = 0; i < 36; ++i)
        if (g_flw_hits[i]) std::printf(" %d(x%u)", i, g_flw_hits[i]);
    std::printf("   (%u total)\n", total);

    std::printf("[scene] dScMgSingle3DBase_c slots entered:");
    for (int i = 0; i < 36; ++i)
        if (g_flw_base_hits[i]) std::printf(" %d(x%u)", i, g_flw_base_hits[i]);
    std::printf("   (%u total)\n", mtotal);

    /* THE STATE MACHINE'S OWN WITNESS. This class dispatches its states from a
       plain switch inside slot 6 rather than through a pointer-to-member
       table, so there is no address switch to count calls through and the
       honest instrument is the state variable itself: +0x5fe8 selects the
       switch arm and +0x5fec is the display state the arms write. Printing
       them is what makes "the state machine ran" a measurement rather than an
       inference from the behavior counter. */
    if (g_flw_self) {
        char *c = (char *)g_flw_self;
        std::printf("[scene] flower state: switch var +0x5fe8 = %d, display "
                    "state +0x5fec = %d, petals live +0x5fd8 = %d, timer "
                    "+0x5fd0 = %d, held petal +0x5fc8 = %d, score +0x5ff0 = "
                    "%d\n",
                    *(int *)(c + 0x5fe8), *(int *)(c + 0x5fec),
                    *(int *)(c + 0x5fd8), *(int *)(c + 0x5fd0),
                    *(int *)(c + 0x5fc8), *(int *)(c + 0x5ff0));
    } else {
        std::printf("[scene] flower state: the class never spawned\n");
    }

    /* ---- THE PETAL CENSUS ------------------------------------------------
     *
     * Twenty-two records at +0x4f38, stride 0x20. func_ov006_0212a764 lays
     * the live ones out on a ring: +0x4f54 is the angle it picks, +0x4f3c and
     * +0x4f40 are the position that angle produces through func_0203d388,
     * +0x4f38 is the live flag.
     *
     * THE MEASUREMENT IS THE COUNT OF DISTINCT POSITIONS, not a look at the
     * screen. "Every petal draws in one place" has two completely different
     * causes -- a layout that computed one position N times, or a layout that
     * computed N positions and a draw that ignores them -- and the only thing
     * that separates them is reading the records the layout wrote. A run with
     * N live petals and 1 distinct position is a LAYOUT bug; N live petals
     * and N distinct positions with one blob on screen is a DRAW bug. */
    if (g_flw_self) {
        char *c = (char *)g_flw_self;
        int live = 0, distinct_pos = 0, distinct_ang = 0;
        int px[22], py[22], pa[22];
        std::printf("[scene] petal records at +0x4f38 stride 0x20:\n");
        for (int i = 0; i < 22; ++i) {
            char *r = c + 0x4f38 + i * 0x20;
            const int flag = *(unsigned char *)r;
            const int x = *(int *)(r + 4);          /* +0x4f3c */
            const int y = *(int *)(r + 8);          /* +0x4f40 */
            const int a = *(short *)(r + 0x1c);     /* +0x4f54 */
            std::printf("[scene]   %2d live=%d angle=%6d x=%08x y=%08x\n",
                        i, flag, a, (unsigned)x, (unsigned)y);
            if (!flag) continue;
            int seen = 0;
            for (int k = 0; k < live; ++k) if (px[k] == x && py[k] == y) seen = 1;
            if (!seen) ++distinct_pos;
            seen = 0;
            for (int k = 0; k < live; ++k) if (pa[k] == a) seen = 1;
            if (!seen) ++distinct_ang;
            px[live] = x; py[live] = y; pa[live] = a;
            ++live;
        }
        std::printf("[scene] petals live %d, DISTINCT POSITIONS %d, distinct "
                    "angles %d%s\n", live, distinct_pos, distinct_ang,
                    (live > 1 && distinct_pos == 1)
                        ? "   <-- EVERY LIVE PETAL IS AT ONE POSITION" : "");
    }

    std::printf("[scene] sub-object state dispatch (+0x51f8, the member "
                "pointer func_ov006_020c3d18 open-codes): %u call(s) through "
                "the address switch, %u UNHANDLED address(es)\n",
                port_mg_flower_sub_calls(), port_mg_flower_sub_unknown());

    if (port_mg_flower_trap_hits())
        std::printf("[scene] func_ov006_0212a764 (undecompiled, no delink "
                    "block, no src) entered %u time(s) -- the petal layout did "
                    "NOT run\n", port_mg_flower_trap_hits());
    else
        std::printf("[scene] func_ov006_0212a764 trap entered: 0\n");
    std::fflush(stdout);
}

/* ---- THE NAMED TRAP ------------------------------------------------------
 *
 * Section 7 of the header is the whole ruling. A config symbol, no delink
 * block, no src file, two live callers. It returns without doing anything and
 * counts itself, so "the petal layout never ran" is a printed number rather
 * than an absence somebody has to notice.
 *
 * ITS ARITY IS THE ROM'S. include/decl_common.h:2628 declares
 * `extern void func_ov006_0212a764(void*);` and both matched callers pass the
 * object, which is also what the ROM does (`mov sb,r0` on entry, then every
 * access through sb). A trap that took void would compile and would silently
 * become a different function the day somebody transcribes the body.
 */
/* Run mg5, lane INTEG: the petal-array layout was recovered on branch
   decomp/yoshi-petal as src/func_ov006_0212a764.c (NONMATCHING regperm floor)
   and is now sliced in port/slice_flw.txt, so the trap that stood in for it is
   gone and the real body lays out the petals. The counter is kept because the
   census above prints port_mg_flower_trap_hits(); it now stays 0, the honest
   report that the petal layout is no longer stubbed. */
static unsigned g_flw_trap_hits;

extern "C" unsigned port_mg_flower_trap_hits(void) { return g_flw_trap_hits; }

/* ---- THE NAME-SPELLING FACES --------------------------------------------
 *
 * Eleven rows, every one of them an /alternatename and none of them a stand-in
 * for a body. They are hand-written rather than generated because each carries
 * a ruling that a generator cannot make, and because port/mg_fanout_costs.txt
 * section 10 finding 1 records what happens when a generated row is taken at
 * face value: facegen aliased dScMgCurling_c's twenty-five-entry pointer-to-
 * member STATE TABLE, because its wall test is `"P8" in sym` and a struct
 * wrapper hides the P8. Every row below was checked against that failure mode
 * first -- none of the eleven targets is a pair table, and this class has no
 * pointer-to-member table at all (port/slice_flw.txt).
 *
 * GROUP A -- THE C-LINKAGE FLIP, TWO ROWS. src/func_ov006_0212b7f8.cpp (the
 * factory) declares
 *
 *     extern int data_ov006_0213e448;
 *     extern int data_ov006_02140140;
 *
 * OUTSIDE its extern "C" block, so MSVC mangles both as C++ globals of type
 * int while the ov006 mount defines them as the plain C symbols. The address
 * is what the code uses (`*(int*)p = (int)&data_ov006_02140140`), and the
 * address is the same one either spelling names, so an alias is exactly right
 * and no storage is involved. Same shape as hal/scene_mg_faces.cpp's
 * _ZTV14dScMgCurling_c row; lane MGA predicted two rows here for 0x169 with
 * the @@3PAXA spelling, and this class's TU spells them `int`, so the mangle
 * is @@3HA. THE MANGLE IS NOT GUESSABLE FROM THE DECLARATION ALONE and both
 * were read off the link, not derived.
 *
 * GROUP B -- A FUNCTION DECLARED AS DATA, ONE ROW.
 * src/func_ov006_020c3e70.cpp:9 declares `extern void* func_ov006_020c3e54;`
 * outside extern "C" and passes `&func_ov006_020c3e54` to __destroy_arr as the
 * element destructor. The declaration's TYPE is wrong and its ADDRESS is
 * right: taking the address of that "variable" is taking the function's entry
 * point, which is what the ROM's literal pool holds. Its sibling
 * src/func_ov006_020c3f54.cpp declares the same symbol as a function and needs
 * no row, which is what identifies this as a spelling rather than a body.
 *
 * GROUP C -- FIVE ov004 GLOBALS SPELLED func_*, AND ONE SPELLED ov000.
 *     func_020bc86c  func_020bc898  func_020bc8a4  func_020bc8a8
 *         include/decl_common.h:1358..1367 declares all four `extern int`,
 *         and src/func_ov006_0212b480.c (InitResources) assigns 0xc0, 0xa0,
 *         0xa0 and 0x40 into them. The ROM stores those four constants into
 *         0x020bc86c, 0x020bc898, 0x020bc8a4 and 0x020bc8a8 -- read out of the
 *         disassembly at 0x0212b6f8..0x0212b720 -- and all four are
 *         `data_ov004_*` in config/arm9/overlays/ov004/symbols.txt.
 *     func_020beb6c  and  data_ov000_020beb74
 *         src/func_ov006_0210a708.c spells ov004's two bss words this way.
 *         Both are kind:bss in ov004's symbols.txt.
 *
 * LANE MGA CALLED THE LAST TWO "HOSTED WORDS" AND THAT IS THE ONE THING IN ITS
 * FRONTIER LIST THIS LANE CORRECTS. Its note reads "the probe prints them as
 * data_ov000_* / func_020beb6c; that is the probe naming an address it has no
 * mount for". There IS a mount: build/port/host-src/ov004_syms.c defines
 * data_ov004_020beb6c and data_ov004_020beb74 as real packed storage (the
 * latter eight bytes, with a PACK BROKEN check at +22312). Hosting them again
 * would have put a SECOND copy of two DS words beside the mount's own, and the
 * ROM code that reads them through the mount and the src TU that writes them
 * through the host global would then have disagreed silently. They are
 * aliases.
 *
 * GROUP D -- TWO ov006/arm9 FUNCTIONS UNDER A SHORTER NAME.
 *     func_020c3adc   src/func_ov006_020c3bc8.c:1 declares
 *                     `extern void func_020c3adc(void *);` -- ov006's own
 *                     func_ov006_020c3adc, one block away in the same file's
 *                     own overlay. The same defect port/mg_fanout_costs.txt
 *                     section 6d records for src/func_ov006_020e3578.c.
 *     func_02012754   src/func_ov006_0212ac74.c calls it nine times.
 *                     config/arm9/symbols.txt:382 names 0x02012754
 *                     _ZN5Sound12PlayBank2_2DEj, and the matched TU
 *                     src/_ZN5Sound12PlayBank2_2DEj.cpp is ALREADY in the
 *                     image, so this is a name and not a slice line. The nine
 *                     call sites are the minigame's sound cues (0x103..0x10d),
 *                     so getting it wrong would be silent rather than fatal --
 *                     which is why it is written down.
 */
#pragma comment(linker, "/alternatename:?data_ov006_0213e448@@3HA=_data_ov006_0213e448")
#pragma comment(linker, "/alternatename:?data_ov006_02140140@@3HA=_data_ov006_02140140")
#pragma comment(linker, "/alternatename:?func_ov006_020c3e54@@3PAXA=_func_ov006_020c3e54")
#pragma comment(linker, "/alternatename:_func_020c3adc=_func_ov006_020c3adc")
#pragma comment(linker, "/alternatename:_func_02012754=__ZN5Sound12PlayBank2_2DEj")
#pragma comment(linker, "/alternatename:_func_020bc86c=_data_ov004_020bc86c")
#pragma comment(linker, "/alternatename:_func_020bc898=_data_ov004_020bc898")
#pragma comment(linker, "/alternatename:_func_020bc8a4=_data_ov004_020bc8a4")
#pragma comment(linker, "/alternatename:_func_020bc8a8=_data_ov004_020bc8a8")
#pragma comment(linker, "/alternatename:_func_020beb6c=_data_ov004_020beb6c")
#pragma comment(linker, "/alternatename:_data_ov000_020beb74=_data_ov004_020beb74")
