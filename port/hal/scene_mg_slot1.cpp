// dScMgSlot1_c, actor id 0x16c = scene 364. The ROM gives this id no
// Mg*_SpawnInfo symbol and no naming import has ever named it, so the class
// name here is the ROM's own RTTI. Run mg9, lane S364.
//
// Read port/slice_s364.txt for the identity derivation, the four width checks,
// the ROM adjudications and the floor. This file is the seat: two vtables'
// faces, the fill, the factory forwarder, the one named trap and the census.
//
// ---- 1. WHY THIS IS A SEPARATE FILE FROM hal/scene_mg.cpp -----------------
//
// scene_mg.cpp is the FAMILY half: the mounts, the thirty-five overlay
// constructors, dScMgBase_c's address-keyed faces and the gate that keeps the
// constructors off a level boot. None of it is re-derived here. This file
// reaches it through the one seam that file exports,
// port_scene_mg_fill_shared(), and adds only what is this class's. The
// alternative -- another face array inside scene_mg.cpp -- is what a ten-lane
// fan-out collides on.
//
// ---- 2. WHAT THE CLASS IS ------------------------------------------------
//
// A THREE-REEL, TWENTY-ONE-STOP SLOT MACHINE with six symbols, a payout table
// and a touch-driven bet icon. That is read off the ROM in four places and the
// derivation is in port/slice_s364.txt section 4: the assets it opens are all
// MG/d_2d_mg_slot* and MG/d_2d_mg_bg_slot*, the Render loops three reels with
// a 0x15 stride and wraps at 21, data_ov006_0213e63c places three reels at
// x = 0x38 / 0x80 / 0xc8 on one row, and the Behavior's literal pool carries
// 0x30c30c31 beside the constant 0x15 -- the reciprocal for a divide by 21.
//
// TWO PLAYER-FACING TITLES, BOTH READ OUT OF THE ROM'S BMG BANK:
//
//     ov005 row 17  text 17  message 565  "Mario Slot"
//     ov005 row 33  text 33  message 587  "Super Mario Slot"
//
// The bank is LZ77-compressed and its text is a glyph-index stream rather than
// characters, which is why an ASCII or UTF-16 sweep of the ROM finds no
// minigame titles at all. Message 565's body -- "Touch the spinning slots to
// stop them. Get 3 items in a row to earn coins! The Superstar is wild." --
// confirms every mechanism derived above and was used to confirm none of it.
// 587 repeats that body and adds "Are you feeling super lucky?".
//
// THE INDEX-TO-MESSAGE STEP IS A TABLE AND NOT ARITHMETIC. data_ov004_020bc070
// is 0x0224 + n for indices 0..23 and then stops being: index 24 is 588, 33 is
// 587, 35 is 583. Computing this class's second title instead of reading it
// gives 581, "Luckiest Stars", a card game that is not this one.
// port/slice_s364.txt section 4 has the whole chain and its seven controls.
//
// THE ROW IS STILL NAMED FOR THE CLASS, which is what a two-title class
// already does here: 0x17c is SCENE_MG_PANEL and not SCENE_MG_PUZZLE_PANIC,
// and 0x178 is SCENE_MG_SMARTBALL and not SCENE_MG_SLOTS_SHOT.
//
// ---- 3. TWO TABLES, AND THE SECOND ONE IS NOT AN INHERITANCE EDGE --------
//
//     Scene -> dScMgBase_c    data_ov004_020bc0c0  36 slots
//           -> dScMgSlot1_c   data_ov006_0213eb40  36 slots
//
//     and a NESTED SUB-OBJECT at this+0x4660:
//           dScMgSlot1_c::betIcon_c  data_ov006_0213e5d4   2 slots
//
// The hierarchy is TWO deep, checked section 13's way rather than off the
// type_info alone: the factory writes 0x0213eb40 into [this] once and writes
// no second scene table over it, and slots 16 and 17 each write it back with
// nothing between. None of the six dScMgSingle3DBase_c bodies that appear in
// nine other ov006 vtables appears in this one.
//
// THE betIcon TABLE IS FILLED HERE AND THAT IS NOT OPTIONAL. Its two words are
// mounted ROM addresses, its type_info at 0x0213e594 names it
// "N12dScMgSlot1_c9betIcon_cE", and vtable slot 9 -- the RENDER, which this
// seat does NOT trap -- ends with `((UnkVtbl*)(t + 0x4660))->Virtual4();`.
// Leaving those two words raw is a wild call on the first rendered frame, not
// a latent one.
//
// ---- 4. THE WIDTH IS 36 AND THIS CLASS IS THE ONE SECTION 11 WARNS ABOUT --
//
// port/mg_fanout_costs.txt section 11 names 0x16c by hand: "Index 36 of its
// table lands exactly on MgBingoBallSlotsShot_SpawnInfo (0x0213ebd0), so a
// 37-slot fill there overwrites word 0 of id 0x178's SpawnInfo, which is
// 0x178's factory pointer." Scene 376 IS SEATED in this tree. All four checks
// were re-run on the shipped image before a word was written here:
//
//   1. SPAN     0x0213eb40 to MgBingoBallSlotsShot_SpawnInfo at 0x0213ebd0 is
//               0x90 = 36 words EXACTLY, and the next symbol is a named record
//               rather than padding, so the check is exact rather than an
//               upper bound.
//   2. TERMINAL slot 35 holds 0x020ad660, the family's terminal word.
//   3. THE WORD index 36 is 0x02119824 and index 37 is 0x01780178 -- a factory
//               pointer and a doubled id, i.e. a SpawnInfo record.
//   4. RELOCS   36 relocations have from: inside the span, indices 0..35 with
//               no gap; the next one, from:0x0213ebd0, is 0x178's.
//
// The fill below is called with 36 and the lane's sibling canary is a scene
// 376 boot.
//
// ---- 5. NO POINTER-TO-MEMBER WALL, AND THE REASON IS INSIDE THE FLOOR -----
//
// port/mg_fanout_costs.txt section 4 opens "THE WALL, AND IT IS THE SAME ONE
// FOR ALL THIRTY". It is not this one's. No .data pair table in ov006 holds a
// code word in this class's range, a `::*` sweep over all 23 slice TUs is
// clean, and the class's own state machine is a fourteen-arm JUMP-TABLE SWITCH
// on self+0x46b4 -- `addls pc, pc, r1, lsl #2` at 0x0210c9f8 -- rather than a
// member-pointer table. So there is no address switch here, no
// MgSlot1_StateDispatch.cpp and no stategen output.
//
// SECTION 14'S PRACTICAL RULE WAS FOLLOWED AND COULD NOT BE OBEYED: "before
// wiring slot 6 by name, read the src". Slot 6 HAS no src -- it is this class's
// one floor -- so the read was done on the ROM instead, and what it found is
// the switch above. That is also the answer to the third PMF shape: an
// open-coded Itanium decode would show as `>> 1` / `& 1` on two words read out
// of an object, and the body's only shift-by-one is the jump table's own
// `lsl #2`.
//
// The FRAMEWORK's wall is still the framework's and is still paid once:
// unmatched/MgBase_StateSetter.cpp and MgBase_StateDispatch.cpp are inherited
// unchanged.
//
// ---- 6. THE ONE FLOOR, AND WHAT THE TRAP COSTS ---------------------------
//
//     func_ov006_0210c9e0   vtable slot 6, the Behavior, 0x81c bytes
//
// Config symbol, NO delink block (the block before it ends at 0x0210c9e0 and
// the next starts at 0x0210d1fc, which is its size to the byte), no src file
// in either extension. It is the only hole in the class's code.
//
// It gets a named counting trap and deliberately not a plausible body:
// port/tools/inferred_stub_guard.py exists to refuse the invention that would
// go there. THE RETURN VALUE IS READ, NOT INVENTED -- the body has a single
// exit at 0x0210d148..0x0210d18c which always `mov r0, #1`.
//
// WHAT IT COSTS ON SCREEN, so nobody reads a clean boot as a working game:
// with slot 6 trapped the state index at +0x46b4 never leaves 0, the reels
// never spin, the per-frame tail the ROM body runs after every arm (the
// betIcon slot-0 update at 0x0210d158, two calls to func_ov006_0210c278 and
// one to func_ov006_0210c1a8) never runs, and no sound or fade is ever
// started. The class boots, InitResources lays out the strips and the bet
// icon, Render draws three static reels every frame, and nothing moves.
//
// AND THE RUN LAW'S WARNING DOES NOT BITE HERE. A trap-shaped floor hides its
// callees from static closure, so they have to be budgeted by hand; this one
// hides NONE. Its seven ov006 callees -- 0x0210c180, _c1a8, _c218, _c278,
// _c2c0, _c2d4, _c500 -- are all reached from other slots as well and are
// already in port/slice_s364.txt. Retiring this floor later costs a decomp and
// zero extra slice lines.
//
// ---- 7. THE ROW GOES LAST ------------------------------------------------
//
// port_scene_registry_install walks port_scene_classes[] in table order and
// calls every row's fill on every boot; port_scene_mg_overlay_load runs the
// thirty-five constructors ONCE PER PROCESS from the tail of the first
// minigame row's fill. Appending after every existing row means the
// constructors have already run against clean ROM words when this fill starts,
// which is the latent-safe direction section 11 argues for. It is a rule this
// lane obeys rather than a hazard it relies on: the fill writes 36 words into
// a table whose span is 36 words, so it cannot reach the neighbour at all.

#include "hal/screen_gap.h"
#include "dsstate_seg.h"

#include <cstdio>
#include <cstdlib>

extern "C" {

/* the seam into hal/scene_mg.cpp -- mounts, arm9 fill, dScMgBase_c's faces */
unsigned port_scene_mg_fill_shared(void **vt, unsigned n);
void     port_scene_mg_prepare(int id);
int      port_scene_env_want(void);
int      IsMinigameActorID(unsigned int id);

/* the mount storage the fill writes into. Both tables are inside the ov006
   mount, so the fill replaces the mount's own words rather than building a
   fresh host array -- the ov007, curling, flower and curling2 treatment, for
   the same reason: a second host array of the same name is a duplicate symbol,
   and leaving the mounted table alone would leave live wild DS pointers in a
   table the factory installs. */
extern unsigned char data_ov006_0213eb40[];   /* dScMgSlot1_c,            36 */
extern unsigned char data_ov006_0213e5d4[];   /* dScMgSlot1_c::betIcon_c,  2 */
extern unsigned char data_ov004_020bc0c0[];   /* dScMgBase_c,             36 */
extern unsigned char data_ov006_0213e560[];   /* the SpawnInfo record        */

/* the class's eight overrides, less the floor. Every signature below is the
   src TU's own, not a guess: slot 18 really does take the ride-through int,
   and slot 9's symbol really is the mangled name its config symbol carries. */
int   func_ov006_0210d1fc(void *c);           /* slot  0 InitResources */
/*    func_ov006_0210c9e0                        slot  6 -- THE FLOOR   */
int   _ZN3OAM7SECONDSE(unsigned char *c);     /* slot  9 Render        */
void *func_ov006_0210a8c0(char *c);           /* slot 16 D2            */
void *func_ov006_0210a900(char *c);           /* slot 17 D0            */
void  func_ov006_0210c674(char *c, int i);    /* slot 18 state reset   */
void  func_ov006_0210c4dc(void *c);           /* slot 27               */
void  func_ov006_0210c4b8(void *c);           /* slot 28               */

/* dScMgSlot1_c::betIcon_c's two */
void  func_ov006_0210c410(char *c);           /* betIcon slot 0 Update */
void  func_ov006_0210c374(char *c);           /* betIcon slot 1 Render */

/* and dThIcon_c's two, the sub-object's BASE class in ov001 -- see the hosted
   table at the foot of this file */
void  func_ov001_020ab550(char *c);           /* dThIcon_c slot 0 */
void  func_ov001_020ab54c(void);              /* dThIcon_c slot 1, empty */

/* the factory */
void *func_ov006_0210d6b8(void);

void port_scene_slot1_hits(void);

}  /* extern "C" */

// ---- the tick witness ------------------------------------------------------
//
// One counter per dispatched slot, the instrument every scene seat in this port
// carries, for hal/scene_mg.cpp's reason: an object that EXISTS and an object
// that RUNS look identical from outside. These count only the slots THIS file's
// faces own; the framework slots dScMgBase_c keeps are counted by scene_mg.cpp's
// own array and printed by its atexit report, which a scene-364 boot also
// registers because 364 is an id IsMinigameActorID accepts.
static unsigned g_s1_hits[36];
static unsigned g_s1_bet_hits[2];      /* the betIcon sub-object's two slots */
static unsigned g_s1_floor_asks;       /* slot 6, the trap below             */
static void    *g_s1_self;

#define S1(n)   (++g_s1_hits[(n)])
#define BET(n)  (++g_s1_bet_hits[(n)])

static int __fastcall s1_init(void *s, void *)
{ S1(0);  const int r = func_ov006_0210d1fc(s);
  /* the GaplessMinigames latch, for hal/scene_mg.cpp's reason: every seated
     minigame calls it so the ones the gapless table does not name can say
     "unsupported" instead of doing nothing quietly. */
  hal_gapless_minigames_latch(); return r; }

/* ---- THE FLOOR ------------------------------------------------------------
 *
 * func_ov006_0210c9e0, vtable slot 6, 0x81c bytes, no delink block and no src
 * file. COUNT AND RETURN ONLY. The 1 is the ROM's own single-exit return value
 * (0x0210d180 `mov r0, #1`) and not a convenient choice; nothing else about
 * the body is reproduced here, and port/tools/inferred_stub_guard.py is the
 * reason a plausible-looking one is not.
 *
 * The census below prints the ask count and the state index side by side,
 * because the two together are the honest statement: N asks with the index
 * pinned at 0 says the tick reached the state machine and the state machine is
 * missing, which is a different claim from either number alone. */
static int __fastcall s1_beh_floor(void *, void *)
{ S1(6); ++g_s1_floor_asks; return 1; }

static int __fastcall s1_render(void *s, void *)
{ S1(9);  return _ZN3OAM7SECONDSE((unsigned char *)s); }
static void *__fastcall s1_d2(void *s, void *)
{ S1(16); return func_ov006_0210a8c0((char *)s); }
static void *__fastcall s1_d0(void *s, void *)
{ S1(17); return func_ov006_0210a900((char *)s); }
/* THE RIDE-THROUGH IS LOAD-BEARING and is not optional on any slot-18 thunk.
   Every slot-18 dispatch site in both overlay images passes one argument (lane
   BASESET's 22-site census, runs/mg5/out/baseset/slot18_19_scan.txt), and the
   round teardown's dispatcher is a __thiscall caller that cleans nothing. A
   thunk declared (void*, void*) compiles to a bare ret, leaks those four
   bytes, and the dispatcher's own `pop ebp; ret` then takes its saved frame
   pointer as a return address.
   THIS CLASS IS THE ONE WHERE THE ARGUMENT IS ALSO READ rather than only
   pushed: src/func_ov006_0210c674.c branches on it (4 copies +0x4709 to
   +0x4706, 3 calls func_ov006_0210c638), and the ROM Behavior dispatches this
   slot with r1 = 4 at 0x0210cf48 and r1 = 5 at 0x0210cf80. So the parameter is
   forwarded, not just absorbed. */
static unsigned g_s1_reset_args[8];   /* 0..6 seen as themselves, 7 = higher */
static int __fastcall s1_reset(void *s, void *, int ridethrough)
{ S1(18);
  ++g_s1_reset_args[(unsigned)ridethrough < 7u ? (unsigned)ridethrough : 7u];
  func_ov006_0210c674((char *)s, ridethrough); return 1; }
static int __fastcall s1_v27(void *s, void *)
{ S1(27); func_ov006_0210c4dc(s); return 0; }
static int __fastcall s1_v28(void *s, void *)
{ S1(28); func_ov006_0210c4b8(s); return 0; }

/* ---- the betIcon sub-object's two ---------------------------------------- */
static void __fastcall s1_bet_update(void *s, void *)
{ BET(0); func_ov006_0210c410((char *)s); }
static void __fastcall s1_bet_render(void *s, void *)
{ BET(1); func_ov006_0210c374((char *)s); }

/* ---- dThIcon_c, the betIcon sub-object's BASE class, hosted --------------
 *
 * The factory (0x0210d6ec) and both destructors store 0x020ad494 into
 * this+0x4660 and then store the betIcon table 0x0213e5d4 over it: a
 * base-then-derived vptr pair, which is the second half of the evidence that
 * betIcon_c is a class and not a struct.
 *
 * 0x020ad494 IS AMBIGUOUS IN CONFIG AND THE ROM RESOLVES IT. ov000 and ov001
 * share the base 0x020aa420 and both name a symbol at that address.
 * hal/scene_mg_faces.cpp hosts the ov000 reading -- a 24-byte filename string
 * "thum_shinkei_ncg.bin" -- and that is the WRONG module here. In
 * overlay_0001.bin the address is the second word of a vtable:
 *
 *   0x020ad478  {0x0209a774, 0x020ad480}  a type_info, whose name reads
 *                                         "9dThIcon_c"
 *   0x020ad490  0x020ad478                that type_info, the word before
 *   0x020ad494  0x020ab550                slot 0
 *   0x020ad498  0x020ab54c                slot 1
 *   0x020ad49c  the next config symbol, so the table is exactly 2 words
 *
 * and src/func_ov006_0210c410.c -- the DERIVED betIcon slot 0 -- calls
 * func_ov001_020ab550 directly, which is the base showing through the
 * override. In overlay_0000.bin the same address is the middle of the string
 * ".../togezo.bmd", which is neither a type_info nor a table.
 *
 * WHY IT IS HOSTED AND NOT MOUNTED: the ov001 per-symbol mount is the
 * 0x020ab800..0x020abb00 sprite-template run only (port/ov001_syms.txt says
 * why it is that span and not more), and widening it for two words would still
 * leave two raw DS code addresses in the table, because ovdata rebases data
 * pointers and not code. This is the same treatment and the same argument
 * hal/scene_mg_faces.cpp gives for the ov000 reading of this very address.
 *
 * NOTHING DISPATCHES IT ON THIS CLASS'S PATH -- all three writers overwrite it
 * with the derived table on the next store, so the base vptr is never live
 * across a call. The faces are here anyway, because a table of raw DS words
 * that is currently dead is a wild call the day someone makes it live, and
 * both bodies are already in this binary. */
static void __fastcall s1_thicon_update(void *s, void *)
{ func_ov001_020ab550((char *)s); }
static void __fastcall s1_thicon_render(void *, void *)
{ func_ov001_020ab54c(); }

/* DSSTATE_BEGIN/END because this is HOSTED DS STORAGE and not host bookkeeping:
   port/tools/dsstate_guard.py failed the link over it until the segment was
   opened, which is the check working. A hosted DS symbol outside .dsstate is
   not captured by a save state, and a save state that restores the object but
   not its vtable word is the half-rollback class the guard exists to stop. */
DSSTATE_BEGIN
extern "C" void *data_ov001_020ad494[2] = {
    (void *)s1_thicon_update, (void *)s1_thicon_render,
};
DSSTATE_END

/* SM64DS_SCENE_SLOT0=0 and SM64DS_SCENE_SLOT9=0, the diagnostics the ov003,
   ov007, curling, flower and curling2 seats all carry, counted separately so a
   run can never read a no-op as the real body having run. */
static unsigned g_s1_init_skipped, g_s1_render_skipped;
static int __fastcall s1_init_noop(void *, void *)
{ ++g_s1_init_skipped; return 1; }
static int __fastcall s1_render_noop(void *, void *)
{ ++g_s1_render_skipped; return 1; }

struct S1Face { unsigned ds; void *host; };

static const S1Face kSlot1Faces[] = {
    {0x0210d1fcu, (void *)s1_init},       {0x0210c9e0u, (void *)s1_beh_floor},
    {0x0210c6c0u, (void *)s1_render},     {0x0210a8c0u, (void *)s1_d2},
    {0x0210a900u, (void *)s1_d0},         {0x0210c674u, (void *)s1_reset},
    {0x0210c4dcu, (void *)s1_v27},        {0x0210c4b8u, (void *)s1_v28},
};

static const S1Face kBetIconFaces[] = {
    {0x0210c410u, (void *)s1_bet_update}, {0x0210c374u, (void *)s1_bet_render},
};

static unsigned s1_apply(void **vt, unsigned n, const S1Face *f, unsigned nf)
{
    unsigned hit = 0;
    for (unsigned i = 0; i < n; ++i) {
        const unsigned ds = (unsigned)(size_t)vt[i];
        for (unsigned k = 0; k < nf; ++k)
            if (f[k].ds == ds) { vt[i] = f[k].host; ++hit; break; }
    }
    return hit;
}

/* Words still holding a DS address. Every slot of a minigame table is
   dispatched by something, so a nonzero answer is a wild call waiting to
   happen and the seat says so out loud. */
static unsigned s1_raw_left(void **vt, unsigned n)
{
    unsigned left = 0;
    for (unsigned i = 0; i < n; ++i) {
        const unsigned w = (unsigned)(size_t)vt[i];
        if (w >= 0x02000000u && w < 0x02400000u)
            ++left;
    }
    return left;
}

extern "C" void port_scene_fill_slot1(void)
{
    void **base = (void **)data_ov004_020bc0c0;
    void **vt   = (void **)data_ov006_0213eb40;
    void **bet  = (void **)data_ov006_0213e5d4;

    /* THE BASE TABLE IS FILLED HERE TOO AND IT IS NOT CEREMONY, for
       hal/scene_mg_flower.cpp's reason: on a tree that carries an earlier
       minigame row this is a second pass over words that are already host
       pointers and finds nothing, and it is here so this class does not depend
       on another class's row existing. The factory's first act is
       func_ov004_020b2adc, which writes data_ov004_020bc0c0 into the object's
       first word before the derived table lands. */
    port_scene_mg_fill_shared(base, 36);

    /* THE DERIVED TABLE, 36 AND NOT 37. Section 11 names this exact table: a
       37th write lands on MgBingoBallSlotsShot_SpawnInfo's factory word and
       corrupts a SHIPPED game. The four checks are in the header and in
       port/slice_s364.txt section 3. */
    port_scene_mg_fill_shared(vt, 36);
    s1_apply(vt, 36, kSlot1Faces, sizeof kSlot1Faces / sizeof kSlot1Faces[0]);

    /* THE betIcon TABLE, 2 words. Not an inheritance edge -- a nested
       sub-object at this+0x4660 that vtable slot 9 dispatches every rendered
       frame. port_scene_mg_fill_shared is NOT called on it: that helper's face
       set is dScMgBase_c's and arm9's, none of whose words appear here, and
       calling it would only walk two words for nothing. */
    s1_apply(bet, 2, kBetIconFaces,
             sizeof kBetIconFaces / sizeof kBetIconFaces[0]);

    /* the two diagnostics, applied after the fill so they override it */
    {
        const char *s0 = std::getenv("SM64DS_SCENE_SLOT0");
        const char *s9 = std::getenv("SM64DS_SCENE_SLOT9");
        if (s0 && s0[0] == '0') vt[0] = (void *)s1_init_noop;
        if (s9 && s9[0] == '0') vt[9] = (void *)s1_render_noop;
    }

    {
        const unsigned lb = s1_raw_left(base, 36);
        const unsigned lv = s1_raw_left(vt, 36);
        const unsigned le = s1_raw_left(bet, 2);
        if (lb || lv || le) {
            std::fprintf(stderr, "  [scene] SLOT1 FILL INCOMPLETE: "
                         "dScMgBase_c leaves %u of 36 raw DS words, "
                         "dScMgSlot1_c %u of 36, betIcon_c %u of 2. A dispatch "
                         "of any of them jumps to a DS address as a host one.\n",
                         lb, lv, le);
            std::fflush(stderr);
        }
    }

    /* The constructors, gated on the requested id, exactly as the curling,
       flower and curling2 fills do it. Idempotent behind scene_mg.cpp's own
       once-per-process guard. */
    port_scene_mg_prepare(port_scene_env_want());

    if (IsMinigameActorID((unsigned)port_scene_env_want())) {
        static int armed;
        if (!armed) {
            armed = 1;
            std::atexit(port_scene_slot1_hits);
        }
    }
}

/* The registry's factory column is void *(*)(void) and so is the matched
   factory, but the forwarder is kept anyway for the reason title_spawn,
   port_mg_curling_spawn and port_mg_flower_spawn are: it gives the seat one
   place to observe the object without the registry table growing a column.
   THE FACTORY NEEDS NO DISPLACEMENT RULING. src/func_ov006_0210d6b8.cpp calls
   func_ov004_020b2adc(p) WITH the pointer, where dScMgCup_c's factory calls
   the same base constructor with none and rides r0 through; the ROM agrees --
   `movs r4, r0` leaves r0 holding p across the call at 0x0210d6cc. */
extern "C" void *port_mg_slot1_spawn(void)
{
    void *p = func_ov006_0210d6b8();
    g_s1_self = p;
    return p;
}

extern "C" void port_scene_slot1_hits(void)
{
    unsigned total = 0;
    for (int i = 0; i < 36; ++i) total += g_s1_hits[i];

    std::printf("[scene] dScMgSlot1_c slot hits: init %u, behavior %u, "
                "render %u, state-reset %u%s\n",
                g_s1_hits[0], g_s1_hits[6], g_s1_hits[9], g_s1_hits[18],
                g_s1_render_skipped
                    ? "  [RENDER SLOT NO-OP'd: SM64DS_SCENE_SLOT9=0]" : "");
    if (g_s1_init_skipped)
        std::printf("[scene] INIT SLOT NO-OP'd: SM64DS_SCENE_SLOT0=0, %u "
                    "time(s)\n", g_s1_init_skipped);

    std::printf("[scene] dScMgSlot1_c own slots entered:");
    for (int i = 0; i < 36; ++i)
        if (g_s1_hits[i]) std::printf(" %d(x%u)", i, g_s1_hits[i]);
    std::printf("   (%u total)\n", total);

    std::printf("[scene] dScMgSlot1_c::betIcon_c slots entered: 0(x%u) 1(x%u)"
                "  -- slot 0 is the Behavior's per-frame update and is "
                "unreachable while slot 6 is trapped; slot 1 is the Render's "
                "and is not\n", g_s1_bet_hits[0], g_s1_bet_hits[1]);

    /* ---- THE FLOOR, REPORTED AS A PAIR ----------------------------------
     *
     * The ask count alone reads like an instrument; the state index alone
     * reads like a stuck game. Printed together they are the measurement:
     * N asks with +0x46b4 pinned at 0 says the tick reaches the state machine
     * every frame and the state machine is the missing body. */
    std::printf("[scene] dScMgSlot1_c FLOOR func_ov006_0210c9e0 (slot 6, "
                "0x81c, no src): %u ask(s)\n", g_s1_floor_asks);

    /* THE SLOT-18 SECOND ARGUMENT, CENSUSED RATHER THAN ASSUMED. Run mg9 lane
       LKY found a class whose slot-18 body READS its second argument where
       every earlier seated class ignored it. This one reads it too --
       src/func_ov006_0210c674.c branches on 4 and on 3, and the ROM Behavior
       dispatches the slot with 4 at 0x0210cf48 and with 5 at 0x0210cf80 -- so
       the thunk FORWARDS it rather than only cleaning it. Printing the
       distribution is what turns "forwarded" into a measurement: a run where
       every dispatch carries the same value has not exercised the branch, and
       on this class the two values that would are behind the trapped slot 6.
       This class does NOT override slot 19; that one is dScMgBase_c's. */
    std::printf("[scene] dScMgSlot1_c slot-18 second argument:");
    for (int i = 0; i < 8; ++i)
        if (g_s1_reset_args[i])
            std::printf(" %s%d(x%u)", i == 7 ? ">=" : "", i,
                        g_s1_reset_args[i]);
    if (!g_s1_hits[18]) std::printf(" (never dispatched)");
    std::printf("\n");

    /* ---- THE REEL CENSUS -------------------------------------------------
     *
     * Three reels. +0x46ff+i is the stop index into that reel's 21-entry
     * strip, +0x46a4+i*4 is the 20.12 scroll offset the Render subtracts from
     * the reel's fixed y, and +0x46c0+i*0x15 is the strip itself -- filled by
     * InitResources from data_ov006_0213e9e0 (the ov005 low byte 0 board) or
     * data_ov006_0213ea20 (low byte 1). The port takes ov005 row 17, which is
     * low byte 0.
     *
     * THE MEASUREMENT IS THE STRIP AND THE STOPS, not a look at the screen.
     * A reel that never spins and a reel that spins back to where it started
     * are the same picture and different states, and only the scroll offset
     * separates them. */
    if (g_s1_self) {
        const unsigned char *c = (const unsigned char *)g_s1_self;
        std::printf("[scene] slot1 state: index +0x46b4 = %d, bet +0x46bc = "
                    "%d, mode byte (field_8 & 0xff) = %u, +0x4709 = %u, "
                    "+0x470a = %u, +0x470b = %u\n",
                    *(const int *)(c + 0x46b4), *(const int *)(c + 0x46bc),
                    (unsigned)(*(const unsigned *)(c + 8) & 0xffu),
                    c[0x4709], c[0x470a], c[0x470b]);
        for (int r = 0; r < 3; ++r) {
            std::printf("[scene] slot1 reel %d: stop %u, scroll 0x%08x, strip",
                        r, c[0x46ff + r],
                        (unsigned)*(const int *)(c + 0x46a4 + r * 4));
            for (int i = 0; i < 0x15; ++i)
                std::printf(" %u", c[0x46c0 + r * 0x15 + i]);
            std::printf("\n");
        }
    } else {
        std::printf("[scene] slot1 state: the class never spawned\n");
    }
}
