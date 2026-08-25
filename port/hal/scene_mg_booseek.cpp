// dScMgTeresa_c, the "Hide and Boo Seek" minigame -- actor id 0x183, scene
// 387. Run mg9, lane BOO.
//
// Read port/slice_boo.txt for the identity derivation, the four width checks,
// the six ROM adjudications and the closure. Read port/mg_fanout_costs.txt for
// what this family costs. This file is the seat: the class's eight faces, the
// fill, the factory forwarder, the two alias rows the link asked for, one
// named trap and the run witness.
//
// ---- 1. WHY THIS IS A SEPARATE FILE FROM hal/scene_mg.cpp -----------------
//
// scene_mg.cpp is the FAMILY half: the mounts, the thirty-five overlay
// constructors, dScMgBase_c's twenty-eight address-keyed faces and the gate
// that keeps the constructors off a level boot. None of it is re-derived here.
// This file reaches it through the one seam that file exports,
// port_scene_mg_fill_shared(), and adds only what is this class's. That is the
// flower precedent and the reason for it is the fan-out: five lanes appending
// five face arrays to one file is five merges of the same region.
//
// ---- 2. THE HIERARCHY IS TWO DEEP, AND THAT IS A MEASUREMENT --------------
//
//     Scene -> dScMgBase_c    data_ov004_020bc0c0  36 slots (ov004)
//           -> dScMgTeresa_c  data_ov006_0213fa0c  36 slots
//
// port/mg_fanout_costs.txt section 13's CORRECTION 1 is that the vtable axis
// CANNOT see an intermediate base -- one shows up only as "more overrides" --
// and prescribes two reads. Both were taken and both say two:
//
//   THE FACTORY, 0x021207a8, has ONE `str r0,[r4]` and its only vtable pool
//   word is 0x021207d8 = 0x0213fa0c. Contrast the flower's 0x0212b7f8, which
//   writes 0x0213e448 and then 0x02140140 over it.
//   SLOT 16 AND SLOT 17, the destructor pair, install ONE table on the way
//   down (src/func_ov006_0211cbd0.c and _0211cbf4.c both write
//   data_ov006_0213fa0c and then call the ov004 base's func_ov004_020b29c0).
//
// So this seat has ONE face array and fills TWO tables, not three. It is the
// cheapest hierarchy shape in the family, and it is why an eight-override
// class costs less here than the flower's twelve-override one did.
//
// ---- 3. THE FILL IS ADDRESS-KEYED, AND THE WIDTH IS 36 --------------------
//
// port_scene_mg_fill_shared and mg_apply both key on the ROM WORD a slot
// holds, never on the slot number, so a face can only land where the ROM
// parked that exact body. The count is still a count, and section 11 measured
// that twelve of twenty-nine widths in section 3's table were phantoms and
// that a 37-slot fill writes a host thunk over live ROM data belonging to
// another class. This table was put through all four checks before a word was
// written:
//
//   table       span to next symbol   slot 35     word past the end
//   ---------   -------------------   ---------   -------------------------
//   0213fa0c    0213fa9c, 36 words    020ad660    0x00000001, the first word
//                                                 of data_ov006_0213fa9c
//
// and the fourth, RELOC COUNT: no relocation in ov006 lands anywhere inside
// 0x0213fa0c..0x0213fa9c except the table's own type_info word at 0x0213fa08,
// so nothing else in the overlay reaches into this span and section 11's
// hazard -- a fill writing a word an overlay CONSTRUCTOR later copies into a
// .bss dispatch table -- cannot arise here. The registry row still goes last,
// because obeying the rule costs nothing and relying on its absence does.
//
// ---- 4. THE STATE MACHINE IS TWO LEVELS AND FOUR TABLES -------------------
//
// port/unmatched/MgTeresa_StateDispatch.cpp is the whole of it and carries the
// derivation. What belongs in THIS file is the one rule it exists to obey:
// vtable slot 6 is NOT wired to src/func_ov006_021203ac.cpp. That TU declares
// its table through `struct Entry { PMF pmf; }`, which mangles
// ?data_ov006_02142eb0@@3PAUEntry@@A -- the @@3PAU spelling section 10's tool
// finding 1 records that port/tools/facegen.py does NOT refuse. It links, it
// compiles, a `::*` sweep sees it but a face generator does not, and it would
// stride an eight-byte table by four. The face below points at the host copy.
//
// ---- 5. THE RENDER TRAP IS RETIRED (run mg10, lane F387) ------------------
//
// func_ov006_0211e72c, ov006, size 0xac, the SIXTH call vtable slot 9 (Render)
// makes: src/func_ov006_02120348.c runs func_ov004_020b1e34, then 0211ddcc,
// 0211e29c, 0211e460, 0211e118, and then this one.
//
// Run mg9 gave it a counted trap because no src file defined it and
// port/tools/inferred_stub_guard.py exists to refuse the invention that would
// otherwise go in its place. mg9's own measurement of what that cost was
// exactly right and is worth keeping in view now that the bill has been paid:
// the trap ran 300 times in a 300-frame run and 1200 times in a 1200-frame run
// and did nothing each time, so THE CLASS HAS BEEN DRAWING EVERYTHING EXCEPT
// THE BOOS.
//
// It is decompiled: src/func_ov006_0211e72c.c byte-matches the ROM under
// mwccarm 2004/b56 and passes tools/linkcheck.py VERIFIED with zero blind
// relocations. It is a plain OAM sprite loop over the sixteen 0x24-stride
// records at this + 0x4660 -- gate byte at +0x1a, x and y at +0x00 and +0x04,
// one OAM::Render per live record out of the fourteen-pointer sprite table
// data_ov006_0213a964, indexed facing * 7 + frame. So there is no trap in this
// file any more and the slice carries the body instead.
//
// SECTION 13's CORRECTION 3 IS WHY IT WAS LOOKED FOR AT ALL, and that stands
// whether or not the floor is retired. Both of dScMgMemory2_c's "is it a
// floor" questions were answered wrong by looking at the STATE tables, where
// section 4 points a reader, and its real floor was in a vtable body's own
// callees. The check that finds one is a delinks join over every callee of
// every override, and that is the check that found this.
//
// THE OTHER TWO FLOORS. func_ov006_0211ebdc is a state, was reported by the
// address switch in unmatched/MgTeresa_StateDispatch.cpp, and is retired by
// this same lane -- src/func_ov006_0211ebdc.c, same two gates. It is the body
// that drives the byte at +0x1e the loop above picks the sprite ROW with, so
// the two were always one piece of work. func_ov004_020ae5c4 is ov004's, is
// STILL A FLOOR, and its trap stays in hal/scene_mg_faces.cpp beside
// func_ov004_020ae858, which is where this family's ov004 traps live. Seven
// ov006 TUs across the family call it, so a per-lane copy of that trap would
// be a duplicate symbol the day a second lane reaches it.
//
// AMENDED, run mg12 lane PANEL: neither body is a floor any more and neither is
// trapped. func_ov004_020ae5c4 was seated by run mg10 lane F371 and
// func_ov004_020ae858 by this run; hal/scene_mg_faces.cpp holds no ov004 trap now.
//
// ---- 6. THE TWO ALIAS ROWS ------------------------------------------------
//
// ROW 1, THE VTABLE UNDER ITS C++ SPELLING. src/MgHideAndBooSeek_Spawn.c
// writes `p[0] = (int)_ZTV13dScMgTeresa_c` and the ov006 mount defines that
// storage as the plain C symbol data_ov006_0213fa0c. The ADDRESS is what the
// code uses and it is the same address either spelling names, so an alias is
// exactly right and no storage is involved. hal/scene_mg_faces.cpp:245 and
// hal/scene_mg_curling2.cpp:446 are the same row for their classes.
//
// ROW 2, A STATIC MEMBER FUNCTION'S MSVC NAME. src/func_ov006_02120248.cpp
// declares `struct G2S { static char* GetBG0CharPtr(); };` and calls it, which
// mangles ?GetBG0CharPtr@G2S@@SAPADXZ. The mount defines the Itanium spelling
// __ZN3G2S13GetBG0CharPtrEv. A STATIC member function is __cdecl with no
// implicit receiver, exactly like the free function on the other side, so this
// is an ordinary alias and NOT the argument-landing case section 4 reserves
// for Scene::AfterInitResources -- "an alias cannot change a calling
// convention" does not bite when neither side changes one.
// hal/sub_actors.cpp:636 carries the same row for GetBG3CharPtr.

#include "hal/screen_gap.h"

#include <cstdio>
#include <cstdlib>

extern "C" {

/* the seam into hal/scene_mg.cpp -- mounts, arm9 fill, dScMgBase_c's faces */
unsigned port_scene_mg_fill_shared(void **vt, unsigned n);
void     port_scene_mg_prepare(int id);
int      port_scene_env_want(void);
int      IsMinigameActorID(unsigned int id);

/* the framework's own dispatch census, from unmatched/MgBase_StateDispatch.cpp */
void port_mg_dispatch_counts(unsigned *calls, unsigned *unknown);

/* the mount storage the fill writes into. Both tables are inside their
   overlay's mount, so the fill replaces the mount's own words rather than
   building a fresh host array: a second host array of the same name is a
   duplicate symbol, and leaving the mounted table alone would leave live wild
   DS pointers in a table the factory installs. */
extern unsigned char data_ov004_020bc0c0[];   /* dScMgBase_c,    36 slots */
extern unsigned char data_ov006_0213fa0c[];   /* dScMgTeresa_c,  36 slots */

/* the class's own eight vtable bodies. TWO of the eight are host copies and
   not the src TU:
     021203fc  InitResources -- MSVC will not compile the src (C2036 on a
               `void*` compound assignment); unmatched/MgTeresa_InitResources
     021203ac  Behavior -- the pointer-to-member dispatcher;
               unmatched/MgTeresa_StateDispatch.cpp */
int   func_ov006_021203fc(char *self);        /* slot  0  InitResources */
int   func_ov006_021203ac(char *self);        /* slot  6  Behavior, host copy */
int   func_ov006_02120348(void *self);        /* slot  9  Render */
int   func_ov006_0211cbd0(int *self);         /* slot 16  D2 */
int  *func_ov006_0211cbf4(int *self);         /* slot 17  D0 */
void  func_ov006_02120248(char *self, int r); /* slot 18  state reset */
int   func_ov006_02120238(void);              /* slot 20  FreeGfxSlotsById(8) */
/* SLOT 34 TAKES FIVE ARGUMENTS, and the declaration says so rather than
   dropping four. The ROM's prologue at 0x021200dc is
       push {r4,r5,r6,r7,r8,sb,sl,fp,lr} / sub sp,sp,#0x1c / ldr sl,[sp,#0x40]
   where 0x40 = 0x24 + 0x1c is the first word above the frame, so it reads
   r0..r3 plus one stack word; src/func_ov006_021200dc.c defines it the same
   way. The dScMgBase_c body it overrides, func_ov004_020ae3b4, has the same
   five-argument shape (ldr fp,[sp,#0x38] after push{9}+sub#0x14).

   A ONE-ARGUMENT DECLARATION HERE WOULD BE THE bin-softlock DISEASE, which is
   why port/tools/aritycheck.py's plain-name ratchet refused it: on ARM the
   dropped words ride registers that are still live and the byte gate stays
   green, while on the host the callee reads stack slots nobody wrote. */
void  func_ov006_021200dc(int self, int cx, int cy, int val, int n);

/* the factory */
void *MgHideAndBooSeek_Spawn(void);

/* the state machine's witnesses, from unmatched/MgTeresa_StateDispatch.cpp */
void port_mg_teresa_counts(unsigned *hits, unsigned *unknown);
void port_mg_teresa_state_index(int *l1);

}  /* extern "C" */

/* ---- 6. THE TWO ALIAS ROWS, and the derivation is in the header ---------- */
#pragma comment(linker, "/alternatename:__ZTV13dScMgTeresa_c=_data_ov006_0213fa0c")
#pragma comment(linker, "/alternatename:?GetBG0CharPtr@G2S@@SAPADXZ=__ZN3G2S13GetBG0CharPtrEv")

// ---- the tick witness ------------------------------------------------------
//
// One counter per dispatched slot, the instrument every scene seat in this
// port carries, for the reason hal/scene_mg.cpp gives: an object that EXISTS
// and an object that RUNS look identical from outside. These count only the
// eight slots THIS file's faces own; the framework slots dScMgBase_c keeps are
// counted by scene_mg.cpp's own array and printed by its atexit report, which
// a scene-387 boot also registers because 387 is an id IsMinigameActorID
// accepts.
static unsigned g_boo_hits[36];

#define BOO(n)  (++g_boo_hits[(n)])

static int __fastcall boo_init(void *s, void *)
{ BOO(0);  const int r = func_ov006_021203fc((char *)s);
  /* the GaplessMinigames latch, for hal/scene_mg.cpp's reason: every seated
     minigame calls it so the ones the gapless table does not name can say
     "unsupported" instead of doing nothing quietly. hal_gapless_splice() is
     deliberately NOT called -- scene 387 has no row in screen_gap.cpp's table,
     so a splice here would be a provable no-op today and an unverified
     per-game gapless change the day that table grows a row. */
  hal_gapless_minigames_latch(); return r; }
static int __fastcall boo_beh(void *s, void *)
{ BOO(6);  return func_ov006_021203ac((char *)s); }
static int __fastcall boo_render(void *s, void *)
{ BOO(9);  return func_ov006_02120348(s); }
static void *__fastcall boo_d2(void *s, void *)
{ BOO(16); return (void *)(size_t)func_ov006_0211cbd0((int *)s); }
static void *__fastcall boo_d0(void *s, void *)
{ BOO(17); return (void *)func_ov006_0211cbf4((int *)s); }
/* THE RIDE-THROUGH IS LOAD-BEARING, and this class is the one that proves it
   rather than inheriting it. Every slot-18 dispatch in both overlay images
   passes one argument -- the 22-site census in hal/scene_mg.cpp's mg_reset
   block and runs/mg5/out/baseset/slot18_19_scan.txt -- and this class's own
   slot-18 body READS IT: src/func_ov006_02120248.cpp's first statement is
   `if (reset == 0)` and the ROM's is `cmp r1,#0 / bne`. So the parameter is
   not only there to let __fastcall clean four bytes off the stack, it carries
   a value that picks between two different bodies of work, and a thunk
   declared (void*, void*) would have leaked the stack AND read a garbage
   reset flag. Section 4's BASESET amendment is the general case; this is a
   measured instance of it.

   IT IS FORWARDED AND IT IS UNEXERCISED, and run mg9 lane LKY's note asks for
   both halves said out loud. FORWARDED: the thunk names the parameter and
   hands it to the body, rather than declaring it only so __fastcall pops four
   bytes. UNEXERCISED: this class's slot-18 counter reads ZERO on every run in
   the proof set -- 300 and 1200 frames, with render and without -- so nothing
   here has watched the body take either arm. What is proven is that the
   ARGUMENT EXISTS and that the port hands it over; which arm a real reset
   takes is a touch question this lane cannot answer.

   SLOT 19 IS A DIFFERENT ANSWER TO THE SAME QUESTION and belongs beside it.
   This class does NOT override slot 19: it inherits dScMgBase_c's
   func_ov004_020b2994, whose entire body is `mov r0,#1 / bx lr`, so the second
   argument the ROM's dispatch sites pass is READ BY NOTHING. That one IS
   witnessed -- the 1200-frame census reports framework slot 19 entered once --
   and it is witnessed only because this lane repaired the shadow-array call
   that reaches it (port/unmatched/MgBase_ShadowSlot19.cpp). */
static int __fastcall boo_reset(void *s, void *, int reset)
{ BOO(18); func_ov006_02120248((char *)s, reset); return 1; }
static int __fastcall boo_v20(void *, void *)
{ BOO(20); return func_ov006_02120238(); }
/* THE FOUR RIDE-THROUGH PARAMETERS ARE THE ROM'S, and the shape is
   mb_reset_base's in hal/scene_mg.cpp: `this` in ecx, the unused edx word,
   then the stack arguments. NOTHING IN THE PORT DISPATCHES THIS SLOT TODAY and
   the run reports v34 zero, which is a measurement rather than a hope: a scan
   of both overlay images for `ldr rX,[rY,#0x88]` with Rn neither pc nor sp --
   section 14's filter, 0x88 being slot 34 -- finds EIGHT sites in ov004 and
   NONE in ov006, and all eight are inside func_ov004_020ae5c4, which is the
   ov004 floor this lane traps in hal/scene_mg_faces.cpp. So the only caller of
   slot 34 in the whole family is a body the port does not have.

   THE DAY THAT FLOOR IS DECOMPILED, READ THIS PARAGRAPH BEFORE WIRING IT. If
   the new TU spells the dispatch as a plain C five-argument pointer it will be
   __cdecl and clean twenty bytes while this thunk cleans sixteen, which is the
   slot-19 defect port/unmatched/MgBase_ShadowSlot19.cpp exists for, arriving
   at a different slot. hal/scene_mg.cpp's mb_v34 has the one-argument
   declaration this thunk used to have and is on aritycheck's baseline as
   pre-existing debt. RUN mg9 LANE S371 REPORTED THAT DEFECT and the run's
   standing rule is that the repair belongs to whichever lane can WITNESS a
   slot-34 dispatch. THIS LANE CANNOT: its census reads v34 zero on every run,
   at 300 and at 1200 frames, with and without render. So mb_v34 is left
   exactly as S371 reported it and only this class's own declaration is made
   honest, which is what the plain-name ratchet asked for. */
static int __fastcall boo_v34(void *s, void *, int cx, int cy, int val, int n)
{ BOO(34); func_ov006_021200dc((int)(size_t)s, cx, cy, val, n); return 0; }

/* SM64DS_SCENE_SLOT0=0 and SM64DS_SCENE_SLOT9=0, the diagnostics the ov003,
   ov007 and every minigame seat carry, counted separately so a run can never
   read a no-op as the real body having run. */
static unsigned g_boo_init_skipped, g_boo_render_skipped;
static int __fastcall boo_init_noop(void *, void *)
{ ++g_boo_init_skipped; return 1; }
static int __fastcall boo_render_noop(void *, void *)
{ ++g_boo_render_skipped; return 1; }

struct BooFace { unsigned ds; void *host; };

static const BooFace kTeresaFaces[] = {
    {0x021203fcu, (void *)boo_init},   {0x021203acu, (void *)boo_beh},
    {0x02120348u, (void *)boo_render}, {0x0211cbd0u, (void *)boo_d2},
    {0x0211cbf4u, (void *)boo_d0},     {0x02120248u, (void *)boo_reset},
    {0x02120238u, (void *)boo_v20},    {0x021200dcu, (void *)boo_v34},
};

static unsigned boo_apply(void **vt, unsigned n, const BooFace *f, unsigned nf)
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
static unsigned boo_raw_left(void **vt, unsigned n)
{
    unsigned left = 0;
    for (unsigned i = 0; i < n; ++i) {
        const unsigned w = (unsigned)(size_t)vt[i];
        if (w >= 0x02000000u && w < 0x02400000u)
            ++left;
    }
    return left;
}

extern "C" void port_scene_booseek_hits(void);

extern "C" void port_scene_fill_booseek(void)
{
    void **base = (void **)data_ov004_020bc0c0;
    void **vt   = (void **)data_ov006_0213fa0c;

    /* THE BASE TABLE IS FILLED HERE TOO AND IT IS NOT CEREMONY, for the reason
       hal/scene_mg_flower.cpp's and hal/scene_mg_bomroom.cpp's fills state: on
       a tree that also carries the curling row this is a second pass over
       words that are already host pointers and finds nothing, because the fill
       keys on a DS word and there are none left. It is here so this class does
       not depend on another class's registry row existing -- the factory's
       first act is func_ov004_020b2adc, which writes data_ov004_020bc0c0 into
       the object's first word before the derived table lands. */
    port_scene_mg_fill_shared(base, 36);

    /* THE DERIVED TABLE: shared arm9 words and dScMgBase_c's twenty-eight
       first, then this class's eight. The two key sets are disjoint by
       construction, since a word is one address. */
    port_scene_mg_fill_shared(vt, 36);
    boo_apply(vt, 36, kTeresaFaces,
              sizeof kTeresaFaces / sizeof kTeresaFaces[0]);

    /* the two diagnostics, applied after the fill so they override it */
    {
        const char *s0 = std::getenv("SM64DS_SCENE_SLOT0");
        const char *s9 = std::getenv("SM64DS_SCENE_SLOT9");
        if (s0 && s0[0] == '0') vt[0] = (void *)boo_init_noop;
        if (s9 && s9[0] == '0') vt[9] = (void *)boo_render_noop;
    }

    {
        const unsigned lb = boo_raw_left(base, 36);
        const unsigned lv = boo_raw_left(vt, 36);
        if (lb || lv) {
            std::fprintf(stderr, "  [scene] MINIGAME FILL INCOMPLETE: "
                         "dScMgBase_c leaves %u of 36 raw DS words, "
                         "dScMgTeresa_c %u. A dispatch of any of them jumps "
                         "to a DS address as a host one.\n", lb, lv);
            std::fflush(stderr);
        }
    }

    /* The constructors, gated on the requested id, exactly as the curling,
       flower and bomroom fills do it. Idempotent behind scene_mg.cpp's own
       once-per-process guard, so on a boot that carries several minigame rows
       this is a no-op and on a tree that ever drops the curling row it is
       still correct. */
    port_scene_mg_prepare(port_scene_env_want());

    if (port_scene_env_want() == 387) {
        static int armed;
        if (!armed) {
            armed = 1;
            std::atexit(port_scene_booseek_hits);
        }
    }
}

/* The registry's factory column is void *(*)(void) and so is the matched
   factory, but the forwarder is kept anyway for the reason title_spawn,
   port_mg_curling_spawn and port_mg_bomroom_spawn are: it gives the seat one
   place to observe the object without the registry table growing a second
   column.

   THIS FACTORY NEEDS NO DISPLACEMENT RULING, which is worth recording because
   0x169's did. src/MgHideAndBooSeek_Spawn.c calls func_ov004_020b2adc(p) WITH
   its argument, where src/func_ov006_020e0574.cpp calls the same base
   constructor with none and rides r0 through. That callee dereferences on its
   first statement and then writes a vtable word through the pointer, so the
   difference is a wild write versus a correct one. This factory is on the
   correct side of it and rides port/slice_boo.txt rather than being
   host-copied. Verified against the ROM at 0x021207a8 (0x34 bytes) during this
   lane's adjudication pass, where its two pool words also gave the object size
   (0x4c28 = 19496, which the src spells as a decimal literal) and the vtable
   address. */
static char *g_boo_self;

extern "C" void *port_mg_booseek_spawn(void)
{
    void *p = MgHideAndBooSeek_Spawn();
    g_boo_self = (char *)p;
    return p;
}

/* This class's own witness. hal/scene_boot.cpp's end-of-run block still has no
   branch for a twelfth minigame and is still not this lane's region to widen,
   so the std::atexit shape every minigame seat since curling uses is used
   here. Registered only when scene 387 is the requested one, so a curling or
   level run prints nothing extra. */
extern "C" void port_scene_booseek_hits(void)
{
    unsigned total = 0;
    for (int i = 0; i < 36; ++i) total += g_boo_hits[i];

    std::printf("[scene] dScMgTeresa_c slot hits: init %u, behavior %u, "
                "render %u, D2 %u, D0 %u, state-reset %u, v20 %u, v34 %u\n",
                g_boo_hits[0], g_boo_hits[6], g_boo_hits[9], g_boo_hits[16],
                g_boo_hits[17], g_boo_hits[18], g_boo_hits[20],
                g_boo_hits[34]);
    if (g_boo_init_skipped || g_boo_render_skipped)
        std::printf("[scene] dScMgTeresa_c diagnostics: slot 0 skipped %u "
                    "time(s), slot 9 skipped %u time(s) -- the counts above "
                    "are NOT the real bodies\n",
                    g_boo_init_skipped, g_boo_render_skipped);

    std::printf("[scene] dScMgTeresa_c own slots entered:");
    for (int i = 0; i < 36; ++i)
        if (g_boo_hits[i]) std::printf(" %d(x%u)", i, g_boo_hits[i]);
    std::printf("   (%u total)\n", total);

    /* THE STATE MACHINE'S OWN WITNESS, and the three counts are disjoint:
       ROUTED is a state that reached a real body, FLOOR is 0x0211ebdc which
       reached nothing on purpose, UNKNOWN is a code word neither switch knows
       and is the number that convicts a missed dispatcher. A run with slot
       hits and zero routed calls has ticked the object without ever entering
       its state machine, and that reads as a success unless it is printed --
       which is exactly what section 10 left unproven for curling and section
       13 proved on a different class. */
    {
        unsigned hits = 0, unknown = 0;
        unsigned fcalls = 0, funknown = 0;
        int l1 = -1;
        port_mg_teresa_counts(&hits, &unknown);
        port_mg_dispatch_counts(&fcalls, &funknown);
        port_mg_teresa_state_index(&l1);
        /* THE FLOOR COLUMN IS GONE FROM THIS LINE, and that is the whole of
           what run mg10 changed about the witness. mg9 printed "%u ask(s) for
           the floor 0x0211ebdc" beside the routed count because that state had
           no body; it has one now, so those asks are inside the routed number
           and there is no third column to print. A reader comparing an mg9 log
           with this one should expect routed to jump by exactly what the floor
           column used to read. */
        std::printf("[scene] dScMgTeresa_c state dispatch: %u routed across "
                    "its FOUR tables, %u UNHANDLED address(es); framework %u "
                    "call(s), %u UNHANDLED\n", hits, unknown, fcalls, funknown);
        std::printf("[scene] dScMgTeresa_c level-1 state index +0x4be8 = %d "
                    "at exit (InitResources seats it at 1; slot 18 zeroes "
                    "it)\n", l1);
    }

    /* The object's own live fields, so "the class ran" is a measurement rather
       than an inference from a slot counter. +0x4be8 is the level-1 state
       index the Behavior indexes its table with; +0x4bbc / +0x4bba and
       +0x4be0 / +0x4be1 are the gate and index bytes the two constant-argument
       level-2 dispatchers read; +0xb4 is the ov004 save-record score field
       InitResources zeroes. */
    if (g_boo_self) {
        char *c = g_boo_self;
        std::printf("[scene] dScMgTeresa_c fields: +0x4be8 L1 state %d, "
                    "e88 gate +0x4bbc %u idx +0x4bba %u, f18 gate +0x4be0 %u "
                    "idx +0x4be1 %u, score +0xb4 %d\n",
                    *(int *)(c + 0x4be8),
                    *(unsigned char *)(c + 0x4bbc),
                    *(unsigned char *)(c + 0x4bba),
                    *(unsigned char *)(c + 0x4be0),
                    *(unsigned char *)(c + 0x4be1),
                    *(int *)(c + 0xb4));

        /* THE SIXTEEN-RECORD CENSUS. func_ov006_0211f6fc walks sixteen records
           at stride 0x24 and dispatches data_ov006_02142ed8 for every one
           whose gate byte at +0x4677 is nonzero, using the index byte at
           +0x4678. Printing how many are live and what indices they hold is
           what separates "the second level dispatched" from "the second level
           dispatched the same state sixteen times", and it is the only place
           a reader can see that the loop's receiver stayed the base object. */
        {
            int live = 0, i;
            std::printf("[scene] dScMgTeresa_c ed8 records:");
            for (i = 0; i < 0x10; ++i) {
                char *e = c + i * 0x24;
                const unsigned g = *(unsigned char *)(e + 0x4677);
                if (g) {
                    ++live;
                    std::printf(" %d:s%u", i,
                                *(unsigned char *)(e + 0x4678));
                }
            }
            std::printf("   (%d of 16 live)\n", live);
        }

        /* THE RENDER CENSUS, AND IT IS A SEPARATE GATE FROM THE ONE ABOVE.
           +0x4677 is the TICK gate func_ov006_0211f6fc reads; +0x467a is the
           RENDER gate src/func_ov006_0211e72c reads, and src/
           func_ov006_0211ebdc.c clears BOTH in the same statement when a Boo
           finishes fading. Printing the drawn set beside the ticked set is what
           makes "the sprite loop ran" a measurement rather than an inference
           from the picture: a run where these two columns disagree has a Boo
           being ticked and not drawn, or drawn and not ticked, and either one
           is a wiring fault this seat should say out loud. Each row also prints
           the sprite index the loop would have used, row*7 + column, which is
           the only place a reader can see that it stayed inside the
           fourteen-pointer table. */
        {
            int drawn = 0, i;
            std::printf("[scene] dScMgTeresa_c drawn records:");
            for (i = 0; i < 0x10; ++i) {
                char *e = c + i * 0x24;
                if (*(unsigned char *)(e + 0x467a)) {
                    const unsigned row = *(unsigned char *)(e + 0x467e);
                    const unsigned col = *(unsigned char *)(e + 0x467d);
                    ++drawn;
                    std::printf(" %d:r%uc%u=%u", i, row, col, row * 7 + col);
                }
            }
            std::printf("   (%d of 16 drawn, sprite table holds 14)\n", drawn);
        }
    } else {
        std::printf("[scene] dScMgTeresa_c: the class never spawned\n");
    }
}
