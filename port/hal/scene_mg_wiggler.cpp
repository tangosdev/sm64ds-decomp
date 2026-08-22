// dScMgHanachan_c, the "Which Wiggler" minigame -- actor id 0x182, scene 386.
// Run mg9, lane WIG.
//
// Read port/slice_wig.txt for the identity derivation, the four width checks,
// the closure and the two floors. Read
// port/unmatched/MgWiggler_StateDispatch.cpp for the state machine, which is a
// shape no seated class has had before. This file is the seat: one face array,
// one fill, one factory forwarder and the run census.
//
// ---- 1. WHY THIS IS A SEPARATE FILE FROM hal/scene_mg.cpp -----------------
//
// The reason hal/scene_mg_flower.cpp, _bomroom.cpp, _curling2.cpp and
// _memory2.cpp are separate files: scene_mg.cpp is the FAMILY half -- the
// mounts, the thirty-five overlay constructors, dScMgBase_c's address-keyed
// faces and the gate that keeps the constructors off a level boot. All of it is
// paid once and none of it is re-derived here. This file reaches it through the
// one seam that file exports, port_scene_mg_fill_shared(), and adds only what is
// this class's. Ten lanes of one run appending ten face arrays to one file is
// ten merges of the same region.
//
// ---- 2. THE NAME, AND IT IS THE ROM'S ------------------------------------
//
// The spawn symbol says MgWhichWiggler and the CLASS says dScMgHanachan_c --
// exactly the MgShuffleShell / dScMgCurling_c and MgSortOrSplode /
// dScMgBomroom_c shape, and the class name is the one this seat's files are
// named after, per the rule lane MG1 set. "Hanachan" is the Japanese name for
// the Wiggler; the localised title is the guessing game.
//
// The name is read through the type_info the word BEFORE the vtable points at,
// which is the chain lane PPP established and which is stronger than a src
// spelling because a per-TU placeholder can be wrong and a relocation cannot:
//
//     relocs.txt  from:0x0213cab4 kind:load to:0x0213ca7c module:overlay(6)
//     relocs.txt  from:0x0213ca80 kind:load to:0x0213ca88 module:overlay(6)
//
// and 0x0213ca88 reads "15dScMgHanachan_c" in
// extracted/overlays/overlay_0006.bin. The third word of that record,
// 0x0213ca84 -> 0x020bbf6c, is ov004's "11dScMgBase_c", so the hierarchy is TWO
// deep with no intermediate table.
//
// ---- 3. THE HIERARCHY IS TWO DEEP AND THE CODE SAYS SO TOO ---------------
//
//     Scene -> dScMgBase_c        data_ov004_020bc0c0  36 slots
//           -> dScMgHanachan_c    data_ov006_0213cab8  36 slots
//
// The RTTI chain above is one witness; section 13's CORRECTION 1 prescribes two
// reads of CODE as the cheap test, and both were taken. The FACTORY
// (MgWhichWiggler_Spawn, 0x020ede18) does one `str r1,[r4]` from a pool word
// that relocs.txt resolves to 0x0213cab8 and no second store. Slot 16 (D2,
// 0x020ea280) and slot 17 (D0, 0x020ea2c8) each do one `str ip,[r4]` from their
// own pool -- 0x020ea2c0 and 0x020ea318, both 0213CAB8 -- and then call
// func_ov004_020b29c0 with nothing in between. So there is no
// dScMgSingle3DBase_c-shaped middle table here and this seat fills ONE derived
// table plus the shared base one.
//
// ---- 4. THE WIDTH IS 36, CHECKED FOUR WAYS -------------------------------
//
//   1. SPAN. The next config symbol after data_ov006_0213cab8 is
//      data_ov006_0213cb48, exactly 0x90 = 36 words on, so a 37th slot cannot
//      exist inside the table.
//   2. TERMINAL SLOT. Slot 35 holds 0x020ad660, the word every dScMgBase_c-
//      derived table holds there.
//   3. WHAT THE WORD PAST IT IS. Index 36 reads 0xffffffff -- an address in no
//      module, and the first word of data_ov006_0213cb48.
//   4. RELOCATION COUNT, which section 11's three checks do not include and
//      which is the one that cannot be argued with. Exactly THIRTY-SIX
//      relocation rows in config/arm9/overlays/ov006/relocs.txt have a source
//      inside [0x0213cab8, 0x0213cb48), one per slot, the last at 0x0213cb44;
//      there is NO row at 0x0213cb48. A vtable word is a code address and is
//      therefore always relocated, so a 37th slot would have a 37th row.
//
// A 37-slot fill here would have written a host thunk over 0xffffffff's word,
// which is the failure port/mg_fanout_costs.txt section 11 exists to prevent.
//
// ---- 5. ALL THIRTY-SIX SLOTS, AND THE SEVEN THIS FILE OWNS ---------------
//
// A fresh word-for-word diff against dScMgBase_c's own table (read out of
// extracted/overlays/overlay_0004.bin at base 0x020ad660) finds SEVEN slots
// differing and no others. "ruled" means the body was disassembled out of
// extracted/overlays/overlay_0006.bin at base 0x020bfec0 and compared
// instruction for instruction with src/ BEFORE it was seated; the per-body
// evidence is in port/tools/inferred_stub_adjudicated.txt.
//
//   slot  ROM word    body
//    0   020edb04    InitResources         ruled REAL_DECOMP
//    3   020ecec8    CleanupResources      ruled REAL_DECOMP
//    6   020ed18c    Behavior              ruled REAL_DECOMP, HOST COPY
//    9   020ecee4    Render                ruled REAL_DECOMP
//   16   020ea280    D2                    (no marker)
//   17   020ea2c8    D0                    ruled REAL_DECOMP
//   18   020eda48    state reset           ruled REAL_DECOMP
//
// SLOT 3 IS THE NEW ONE. No seated minigame class before this overrides
// CleanupResources; the six seated at six or seven overrides all sit on
// 0, 6, 9, 16, 17, 18. dScMgHanachan_c adds slot 3, whose base word is arm9's
// 0x02043bf0, and the body is three statements: func_ov004_020ad90c(this) then
// return 1. The face below takes the shape hal/scene_boot.cpp's sc_base_clean
// takes for the same slot -- (void *, void *), no ride-through -- because that
// is the shape every existing caller of slot 3 in this port already uses.
//
// The other twenty-nine are dScMgBase_c's and arm9's, and
// port_scene_mg_fill_shared writes every one of them, which is why this file's
// array is seven rows long.
//
// ---- 6. THE STATE MACHINE HAS NO TABLE, AND THAT IS THE LANE'S FINDING ----
//
// Every seated minigame before this one dispatches an INDEXED table its overlay
// constructor builds in .bss. This class has neither: no overlay constructor
// (measured -- zero relocations from any __sinit block land anywhere in this
// class's .data) and no table. It has twenty-nine individually symbolised
// {code, 0} pairs in .data, each installed by name into a member-pointer FIELD,
// and three fields across two objects read them: the scene's own +0x4660 and
// each wiggler sub-object's +0x10 and +0x00. Fourteen distinct code words, all
// fourteen routed, all fourteen with a matched src TU.
//
// FIVE DISPATCHERS, AND THE FIFTH IS INVISIBLE TO BOTH PRESCRIBED DETECTORS.
// func_ov006_020eb610 open-codes the ARM Itanium sequence in plain ints inside
// extern "C", so it emits no unresolvable symbol and spells no member-pointer
// operator. It was found by a third detector this lane wrote: decode every word
// of ov006's text and look for `add Rd,Rn,Rm,asr #1` immediately followed by
// `ands Rm,Rm,#1`. That is the sequence's own fingerprint; over all of ov006 it
// finds 114 sites and five are in this class, which is exactly the five.
//
// SO THE CENSUS BELOW COUNTS IT SEPARATELY, for the reason
// hal/scene_mg_bomroom.cpp gives: on a build that compiled the src TU instead,
// that count would read zero AND the run would fault with eip on a DS address.
// A nonzero count beside a clean run is the positive evidence.
//
// ---- 7. STATE MOVEMENT IS REPORTED AS AN ADDRESS, NOT AN INDEX -----------
//
// There is no state INDEX on this class to watch. What moves is the code word
// in the pair at scene+0x4660, so the report below prints the first and last
// code word it saw there and how many distinct ones passed through. That is the
// honest analogue of "state index moved from 1 to 2" for a class built this
// way, and it is measured at the seat rather than inferred from dispatch
// counts.
//
// ---- 8. THE TWO FLOORS -----------------------------------------------------
//
//   func_ov006_020ea914  0x324  ON THE RENDER PATH. Slot 9 calls it
//       unconditionally and is its only caller in all of ov006.
//   func_ov006_020ec4dc  0x20c  wiggler set-up variant 3 of 4, selected by
//       data_ov006_02141fd8 in src/func_ov006_020ecdb8.c's switch. Variants
//       0, 1 and 2 are matched.
//
// Both are trapped in port/unmatched/MgWiggler_Faces.cpp and both are reported
// below whether or not they fire, because a silent zero and an absent
// instrument look the same in a log -- and the two zeros mean different things.
//
// ---- 9. THE GAPLESS LATCH IS CALLED AND THE SPLICE IS NOT ----------------
//
// hal_gapless_minigames_latch() runs from slot 0 the way every seated
// minigame's does, because a scene that does not latch inherits the previous
// scene's answer. hal_gapless_splice() is deliberately NOT called: scene 386 has
// no row in screen_gap.cpp's table, gapless behaviour is decided per minigame by
// playing it, and this lane has not played it and does not get to vote.
// hal/scene_mg_flower.cpp and hal/scene_mg_bomroom.cpp make the same call for
// the same reason.

#include "hal/screen_gap.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {

/* the seam into hal/scene_mg.cpp -- mounts, arm9 fill, dScMgBase_c's faces */
unsigned port_scene_mg_fill_shared(void **vt, unsigned n);
void     port_scene_mg_prepare(int id);
int      port_scene_env_want(void);

/* the framework's own dispatch census, from unmatched/MgBase_StateDispatch.cpp */
void port_mg_dispatch_counts(unsigned *calls, unsigned *unknown);

/* the mount storage the fill writes into. Both tables are inside their
   overlay's mount, so the fill replaces the mount's own words rather than
   building a fresh host array -- the ov007 and curling treatment, for the same
   reason: a second host array of the same name is a duplicate symbol, and
   leaving the mounted table alone leaves live wild DS pointers in a table the
   factory installs. */
extern unsigned char data_ov004_020bc0c0[];   /* dScMgBase_c,       36 slots */
extern unsigned char data_ov006_0213cab8[];   /* dScMgHanachan_c,   36 slots */

/* the wiggler-kind selector src/func_ov006_020ecdb8.c switches on */
extern short data_ov006_02141fd8;
/* the wiggler count slot 6 and slot 9 both loop on */
extern int   data_ov006_0213c958;

/* the class's own seven vtable bodies. func_ov006_020ed18c is the HOST COPY in
   unmatched/MgWiggler_StateDispatch.cpp, not the src TU: it is the
   pointer-to-member dispatcher and the port cannot compile the src. */
int   func_ov006_020edb04(void *self);          /* slot 0  InitResources */
int   func_ov006_020ecec8(void *self);          /* slot 3  CleanupResources */
int   func_ov006_020ed18c(char *self);          /* slot 6  Behavior, host copy */
int   func_ov006_020ecee4(void *self);          /* slot 9  Render */
void *func_ov006_020ea280(char *self);          /* slot 16 D2 */
void *func_ov006_020ea2c8(void *self);          /* slot 17 D0 */
void  func_ov006_020eda48(char *self, int st);  /* slot 18 state reset */

/* the factory */
void *MgWhichWiggler_Spawn(void);

/* this class's own witnesses */
void port_mg_wiggler_counts(unsigned *calls, unsigned *hits, unsigned *scene,
                            unsigned *opencoded);
void port_mg_wiggler_trap_counts(unsigned *render, unsigned *setup);

}  /* extern "C" */

// ---- the tick witness ------------------------------------------------------
//
// One counter per dispatched slot, the instrument every scene seat in this port
// carries, for the reason hal/scene_mg.cpp gives: an object that EXISTS and an
// object that RUNS look identical from outside. These count only the seven
// slots THIS file's faces own; the framework slots dScMgBase_c keeps are
// counted by scene_mg.cpp's own array and printed by its atexit report, which a
// scene-386 boot also registers because 386 is an id IsMinigameActorID accepts.
static unsigned g_wig_hits[36];

#define WIG(n)  (++g_wig_hits[(n)])

/* THE STATE WITNESS. Sampled at the top of every Behavior tick, before the ROM
   body reads the field, so it records what the class was about to dispatch
   rather than what it left behind. */
static unsigned g_wig_state_first;
static unsigned g_wig_state_last;
static unsigned g_wig_state_changes;
static unsigned g_wig_state_seen[8];
static unsigned g_wig_state_nseen;

static void wig_sample_state(const char *s)
{
    const unsigned code = *(const unsigned *)(s + 0x4660);
    if (g_wig_state_last == 0 && g_wig_state_first == 0)
        g_wig_state_first = code;
    if (code != g_wig_state_last) {
        ++g_wig_state_changes;
        g_wig_state_last = code;
        unsigned k = 0;
        for (; k < g_wig_state_nseen; ++k)
            if (g_wig_state_seen[k] == code)
                break;
        if (k == g_wig_state_nseen &&
            g_wig_state_nseen < sizeof g_wig_state_seen / sizeof g_wig_state_seen[0])
            g_wig_state_seen[g_wig_state_nseen++] = code;
    }
}

/* SM64DS_WIG_TRACE=1: per-frame trace of the scene state word, the wiggler
   count and the substate histogram of the fifteen sub-objects. OFF unless set,
   writes nothing, and it exists because this class's "state" is an address
   rather than an index and a dispatch count alone cannot say whether the
   machine moved. */
static void wig_trace(const char *s, int tick)
{
    static unsigned last;
    const unsigned code = *(const unsigned *)(s + 0x4660);
    int sub[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    const int n = data_ov006_0213c958 > 0 && data_ov006_0213c958 <= 0xf
                ? data_ov006_0213c958 : 0xf;
    for (int i = 0; i < n; ++i) {
        const unsigned char v = *(const unsigned char *)(s + 0x4678 + i * 0x98 + 0x95);
        if (v < 8) ++sub[v];
    }
    if (code != last || (tick % 120) == 0) {
        last = code;
        std::fprintf(stderr, "  [wig] t%-5d state=0x%08x  kind=%d  wigglers=%d"
                     "  substates 0:%d 1:%d 2:%d 3:%d 4:%d 5:%d 6:%d\n",
                     tick, code, (int)data_ov006_02141fd8, n,
                     sub[0], sub[1], sub[2], sub[3], sub[4], sub[5], sub[6]);
        std::fflush(stderr);
    }
}

static int __fastcall wig_init(void *s, void *)
{
    WIG(0);
    const int r = func_ov006_020edb04(s);
    hal_gapless_minigames_latch();
    return r;
}

static int __fastcall wig_clean(void *s, void *)
{ WIG(3); return func_ov006_020ecec8(s); }

/* SM64DS_WIG_KIND=<n>: pin data_ov006_02141fd8, the selector
   src/func_ov006_020ecdb8.c's four-way switch reads to pick which wiggler
   set-up runs. A DIAGNOSTIC, off unless the variable is set.

   IT EXISTS FOR ONE REASON. Variant 3 of the four is this class's second floor
   (func_ov006_020ec4dc has a config symbol, no delink block and no src file),
   and an unattended boot reaches a variant when the game decides to. Pinning is
   how each of the four gets exercised on purpose, and it is the only way to
   make the trap fire and be counted rather than to report a zero that could
   mean either "the variant is fine" or "the variant was never reached".

   IT PINS RATHER THAN POKES ONCE, because the ROM writes this word itself from
   func_ov006_020ed494 and func_ov006_020ed8a4, and it writes only this one
   halfword of mounted .bss, before the ROM body reads it. */
static int __fastcall wig_beh(void *s, void *)
{
    WIG(6);
    static int kind = -2;
    if (kind == -2) {
        const char *e = std::getenv("SM64DS_WIG_KIND");
        kind = (e && e[0]) ? std::atoi(e) : -1;
        if (kind >= 0)
            std::fprintf(stderr, "  [scene] SM64DS_WIG_KIND=%d: pinning "
                         "dScMgHanachan_c's set-up selector at "
                         "data_ov006_02141fd8. This is a diagnostic and not how "
                         "the game runs.\n", kind);
    }
    if (kind >= 0)
        data_ov006_02141fd8 = (short)kind;

    static int tick;
    static int trace = -2;
    if (trace == -2)
        trace = std::getenv("SM64DS_WIG_TRACE") ? 1 : 0;
    ++tick;
    wig_sample_state((const char *)s);
    if (trace > 0)
        wig_trace((const char *)s, tick);
    return func_ov006_020ed18c((char *)s);
}

static int  __fastcall wig_render(void *s, void *)
{ WIG(9);  return func_ov006_020ecee4(s); }
static void *__fastcall wig_d2(void *s, void *)
{ WIG(16); return func_ov006_020ea280((char *)s); }
static void *__fastcall wig_d0(void *s, void *)
{ WIG(17); return func_ov006_020ea2c8(s); }
/* THE RIDE-THROUGH IS LOAD-BEARING AND THIS BODY ACTUALLY USES IT. Lane
   BASESET's census says every slot-18 dispatch site in both overlay images
   passes one argument, and a thunk declared (void*, void*) compiles to a bare
   ret, leaks four bytes and hands the caller's epilogue a garbage return
   address. On most classes the ROM body then ignores its r1. NOT HERE:
   0x020eda48 opens `cmp r1,#1` and `cmp r1,#0x12` and branches three ways on
   it, so the parameter is forwarded rather than only cleaned. */
static int __fastcall wig_reset(void *s, void *, int st)
{ WIG(18); func_ov006_020eda48((char *)s, st); return 1; }

/* SM64DS_SCENE_SLOT0=0 and SM64DS_SCENE_SLOT9=0, the diagnostics the ov003,
   ov007, curling, flower and bomroom seats all carry, counted separately so a
   run can never read a no-op as the real body having run. */
static unsigned g_wig_init_skipped, g_wig_render_skipped;
static int __fastcall wig_init_noop(void *, void *)
{ ++g_wig_init_skipped; return 1; }
static int __fastcall wig_render_noop(void *, void *)
{ ++g_wig_render_skipped; return 1; }

struct WigFace { unsigned ds; void *host; };

/* dScMgHanachan_c's own seven, the per-class half. Keyed on the ROM WORD each
   slot holds, exactly like kCurlingFaces, kCoinFaces and kBomroomFaces, so the
   array is order-independent and cannot land on a slot the ROM did not park
   that body in. */
static const WigFace kWigglerFaces[] = {
    {0x020edb04u, (void *)wig_init},   {0x020ecec8u, (void *)wig_clean},
    {0x020ed18cu, (void *)wig_beh},    {0x020ecee4u, (void *)wig_render},
    {0x020ea280u, (void *)wig_d2},     {0x020ea2c8u, (void *)wig_d0},
    {0x020eda48u, (void *)wig_reset},
};

static unsigned wig_apply(void **vt, unsigned n, const WigFace *f, unsigned nf)
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
   waiting to happen and the seat says so out loud rather than booting. */
static unsigned wig_raw_left(void **vt, unsigned n)
{
    unsigned left = 0;
    for (unsigned i = 0; i < n; ++i) {
        const unsigned w = (unsigned)(size_t)vt[i];
        if (w >= 0x02000000u && w < 0x02400000u)
            ++left;
    }
    return left;
}

extern "C" void port_scene_wiggler_hits(void);

extern "C" void port_scene_fill_wiggler(void)
{
    void **base = (void **)data_ov004_020bc0c0;
    void **vt   = (void **)data_ov006_0213cab8;

    /* THE BASE TABLE IS FILLED HERE TOO AND IT IS NOT CEREMONY, for the reason
       hal/scene_mg_flower.cpp's fill states: on a tree that also carries the
       curling row this is a second pass over words that are already host
       pointers and finds nothing, because the fill keys on a DS word and there
       are none left. It is here so this class does not depend on another
       class's registry row existing -- the factory's first act is
       func_ov004_020b2adc, which writes data_ov004_020bc0c0 into the object's
       first word before the derived table lands. */
    port_scene_mg_fill_shared(base, 36);

    /* THE DERIVED TABLE: shared arm9 words and dScMgBase_c's own first, then
       this class's seven. The two key sets are disjoint by construction, since
       a word is one address. */
    port_scene_mg_fill_shared(vt, 36);
    wig_apply(vt, 36, kWigglerFaces,
              sizeof kWigglerFaces / sizeof kWigglerFaces[0]);

    /* the two diagnostics, applied after the fill so they override it */
    {
        const char *s0 = std::getenv("SM64DS_SCENE_SLOT0");
        const char *s9 = std::getenv("SM64DS_SCENE_SLOT9");
        if (s0 && s0[0] == '0') vt[0] = (void *)wig_init_noop;
        if (s9 && s9[0] == '0') vt[9] = (void *)wig_render_noop;
    }

    {
        const unsigned lb = wig_raw_left(base, 36);
        const unsigned lv = wig_raw_left(vt, 36);
        if (lb || lv) {
            std::fprintf(stderr, "  [scene] MINIGAME FILL INCOMPLETE: "
                         "dScMgBase_c leaves %u of 36 raw DS words, "
                         "dScMgHanachan_c %u. A dispatch of any of them jumps "
                         "to a DS address as a host one.\n", lb, lv);
            std::fflush(stderr);
        }
    }

    /* The constructors, gated on the requested id, exactly as the curling,
       flower and bomroom fills do it. Idempotent behind scene_mg.cpp's own
       once-per-process guard, so on a boot that carries several minigame rows
       this is a no-op and on a tree that ever drops the curling row it is still
       correct.

       THIS CLASS HAS NO CONSTRUCTOR OF ITS OWN AMONG THE THIRTY-FIVE, and that
       is measured rather than assumed: no relocation from any __sinit block in
       ov006 lands anywhere in [0x0213c98c, 0x0213cab8]. The call is still made,
       because the class's closure reaches ov006 and ov004 data that OTHER
       constructors build. */
    port_scene_mg_prepare(port_scene_env_want());

    if (port_scene_env_want() == 386) {
        static int armed;
        if (!armed) {
            armed = 1;
            std::atexit(port_scene_wiggler_hits);
        }
    }
}

/* The registry's factory column is void *(*)(void) and so is the matched
   factory, but the forwarder is kept anyway for the reason title_spawn,
   port_mg_curling_spawn and port_mg_bomroom_spawn are: it gives the seat one
   place to observe the object without the registry table growing a second
   column.

   THE FACTORY NEEDS NO DISPLACEMENT RULING, which is worth recording because
   0x169's did. src/MgWhichWiggler_Spawn.cpp calls func_ov004_020b2adc(o) WITH
   its argument, where src/func_ov006_020e0574.cpp calls the same base
   constructor with none and rides r0 through. That callee dereferences on its
   first statement and then writes three vtable words through the pointer, so
   the difference is a wild write versus a correct one. This factory is on the
   correct side of it and is a plain slice line. Verified against the ROM at
   0x020ede18 (0x68 bytes) during this lane's adjudication pass: allocation size
   0x4f68 = 20328 matching src, and the fifteen 0x98-byte sub-objects at
   this+0x4678 built through func_020733a8 with func_ov006_020ede80 as the
   element constructor and func_ov006_020ea324 as the destructor. */
static char *g_wig_self;

extern "C" void *port_mg_wiggler_spawn(void)
{
    void *p = MgWhichWiggler_Spawn();
    g_wig_self = (char *)p;
    return p;
}

/* This class's own witness. hal/scene_boot.cpp's end-of-run block still has no
   branch per minigame and is still not a lane's region to widen, so the same
   std::atexit shape port_scene_fill_curling and port_scene_fill_bomroom use is
   used here. Registered only when scene 386 is the requested one, so a curling
   or level run prints nothing extra. */
extern "C" void port_scene_wiggler_hits(void)
{
    std::printf("[scene] dScMgHanachan_c slot hits: init %u, cleanup %u, "
                "behavior %u, render %u, D2 %u, D0 %u, state-reset %u\n",
                g_wig_hits[0], g_wig_hits[3], g_wig_hits[6], g_wig_hits[9],
                g_wig_hits[16], g_wig_hits[17], g_wig_hits[18]);
    if (g_wig_init_skipped || g_wig_render_skipped)
        std::printf("[scene] dScMgHanachan_c diagnostics: slot 0 skipped %u "
                    "time(s), slot 9 skipped %u time(s) -- the counts above "
                    "are NOT the real bodies\n",
                    g_wig_init_skipped, g_wig_render_skipped);
    {
        unsigned calls = 0, unknown = 0;
        unsigned mine = 0, hits = 0, scene = 0, opencoded = 0;
        port_mg_dispatch_counts(&calls, &unknown);
        port_mg_wiggler_counts(&mine, &hits, &scene, &opencoded);
        std::printf("[scene] dScMgHanachan_c state dispatch: %u reached this "
                    "class's switch, %u routed (%u of them the scene object's "
                    "own +0x4660 states, the rest the fifteen wigglers'), %u "
                    "framework call(s), %u UNHANDLED address(es)\n",
                    mine, hits, scene, calls, unknown);
        /* THE THING ONLY A RUN CAN SAY. func_ov006_020eb610 open-codes the ARM
           Itanium sequence in plain ints: no unresolvable symbol, no
           member-pointer spelling, so neither the link nor a source sweep can
           see it and only this count convicts it. Printed whether or not it
           fired, because a zero here means no wiggler ever had its second pair
           installed and NOT that the shape is absent. */
        std::printf("[scene] dScMgHanachan_c open-coded dispatcher "
                    "(func_ov006_020eb610, the wiggler sub-object's +0x00 "
                    "pair): %u dispatch(es) through the host decode\n",
                    opencoded);
    }
    /* STATE MOVEMENT, AS AN ADDRESS. This class has no state index; what moves
       is the code word of the pair at scene+0x4660. */
    {
        std::printf("[scene] dScMgHanachan_c state word at +0x4660: first "
                    "0x%08x, last 0x%08x, %u change(s), %u distinct:",
                    g_wig_state_first, g_wig_state_last, g_wig_state_changes,
                    g_wig_state_nseen);
        for (unsigned i = 0; i < g_wig_state_nseen; ++i)
            std::printf(" 0x%08x", g_wig_state_seen[i]);
        std::printf("\n");
    }
    /* THE TWO FLOORS, REPORTED WHETHER OR NOT THEY FIRED, and the two zeros
       mean different things. */
    {
        unsigned render = 0, setup = 0;
        port_mg_wiggler_trap_counts(&render, &setup);
        std::printf("[scene] dScMgHanachan_c floors: func_ov006_020ea914 "
                    "(0x324, slot 9 Render's only call, and the only caller it "
                    "has in ov006) entered %u time(s) -- a zero here means "
                    "Render never ran; func_ov006_020ec4dc (0x20c, wiggler "
                    "set-up variant 3 of 4, selected by data_ov006_02141fd8 = "
                    "%d) entered %u time(s) -- a zero here means the selector "
                    "never read 3, and variants 0, 1 and 2 are matched\n",
                    render, (int)data_ov006_02141fd8, setup);
    }
    if (g_wig_self)
        std::printf("[scene] dScMgHanachan_c object at %p, state word 0x%08x, "
                    "adjustment %d, score %u, wiggler count %d\n",
                    (void *)g_wig_self,
                    *(unsigned *)(g_wig_self + 0x4660),
                    *(int *)(g_wig_self + 0x4664),
                    *(unsigned *)(g_wig_self + 0xbc),
                    data_ov006_0213c958);
    std::fflush(stdout);
}
