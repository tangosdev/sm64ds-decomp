// dScMgAmida_c, actor id 0x173, scene 371 -- THE ONE TRUE WIDTH-37 CLASS of
// the twenty-nine, and the twelfth ov006 minigame the port seats.
// Run mg9, lane S371.
//
// Read port/slice_s371.txt for the identity derivation, the three width checks
// written out one at a time, the two player titles and their control set, the
// proof that this class has no pointer-to-member wall, and the three floors.
// Read port/mg_fanout_costs.txt sections 3, 4 and 11 for what the family costs
// and for why the width column was wrong twelve times. This file is the seat:
// eleven faces, the fill, the factory forwarder and the witness.
//
// ---- 1. WHY THIS IS A SEPARATE FILE FROM hal/scene_mg.cpp -----------------
//
// hal/scene_mg_flower.cpp section 1's ruling, unchanged and not re-argued:
// scene_mg.cpp is the FAMILY half and exports exactly one seam,
// port_scene_mg_fill_shared(). Copying kMgBaseFaces into a second translation
// unit would install a second set of host thunks over the same DS words and
// split the per-slot witness in two, so this file reaches the family through
// the seam and adds only what is this class's.
//
// ---- 2. THE HIERARCHY IS TWO DEEP AND THREE BODIES SAY SO ----------------
//
//     Scene -> dScMgBase_c (data_ov004_020bc0c0) -> dScMgAmida_c (0213b918)
//
// The word before slot 0 is an mwcc typeinfo record {0x0209a764 the
// __si_class_type_info vptr, 0x0213b828 the name, 0x020bbf6c the base}, and
// 0x0213b828 reads "12dScMgAmida_c". The base word is ov004's, the same one
// dScMgCurling2_c's and dScMgPanel_c's records carry, so there is no
// intermediate table of the dScMgSingle3DBase_c kind and ONE face array
// appears below. Three bodies confirm it independently: the FACTORY
// (func_ov006_020d5974) stores 0x0213b918 into the object and nothing else,
// and both destructors -- slot 17 at 0x020d10b8 and slot 16 at 0x020d1018 --
// store the same single word.
//
// ---- 3. THE WIDTH IS 37 AND THIS IS WHERE THE 37 LIVES -------------------
//
// port_scene_mg_fill_shared IS CALLED WITH A COUNT. The fill itself is
// address-keyed and would handle any width; the count is the parameter
// port/mg_fanout_costs.txt section 11 identifies as the thing to get right,
// and for this class alone in the family it is THIRTY-SEVEN. Two lines below
// carry it -- the fill call and am_raw_left -- and kAmidaFaces has eleven rows
// because this class overrides eleven slots. The dScMgBase_c table is still
// filled with 36, because that table really is 36 words.
//
// EACH OF SECTION 11'S THREE CHECKS ANSWERS DIFFERENTLY HERE THAN FOR EVERY
// SIBLING, and that is the whole reason this class is the exception:
//
//   1. SPAN. data_ov006_0213b918 -> data_ov006_0213b9ac is 0x94 = 37 words.
//      A span is an UPPER bound and can only ever refuse a width; for the
//      twelve classes section 11 corrected it read 36 and refused 37. Here it
//      reads 37 and DECLINES TO REFUSE. That is all it says.
//   2. TERMINAL SLOT. Every sibling holds ov004's 0x020ad660 at slot 35
//      because every sibling inherits that body. This one holds 0x020d1170,
//      an ov006 address in its own code block, so slot 35 is an OVERRIDE and
//      the terminator argument does not reach it.
//   3. WHAT THE WORD IS. Slot 36 is a RELOCATED CODE POINTER --
//      `from:0x0213b9a8 kind:load to:0x020d1188 module:overlay(6)` -- which no
//      phantom slot has ever been, because live data belonging to the next
//      object is not a pointer and carries no load relocation. The word at
//      index 37 (0x0213b9ac, the next config symbol) reads 0x00000100 and has
//      no relocation at all.
//
// AND THE CHECK THAT WOULD HAVE SURVIVED ALONE: slot 36 is DISPATCHED, 25
// times inside this class's own code, including four times in InitResources.
//
// WHY THE CLASS NEEDED A 37th VIRTUAL, in six instructions each. The family's
// slot 35 is a boolean over the spawn param's low byte; this class had to tell
// THREE values apart, so it split the boolean in two and put the second half
// in a new slot:
//
//     ov004 0x020ad660  return (field_8 & 0xff) != 0     (every sibling)
//     ov006 0x020d1170  return (field_8 & 0xff) == 1     (slot 35, here)
//     ov006 0x020d1188  return (field_8 & 0xff) == 2     (slot 36, here)
//
// ---- 4. ONE ID, TWO PLAYER TITLES, AND THE PORT BOOTS THE FIRST ----------
//
// This is dScMgPanel_c's shape (section 14). The ov005 launch table has two
// rows for 0x173: row 0 with param 0x00000000 and row 24 with param
// 0x00181802. Their name-text ids resolve through data_ov004_020bc070 to BMG
// messages 548 "Mario's Slides" and 588 "Connect the Characters" -- both
// amidakuji, which is what the RTTI name is short for. The chain is controlled
// 25 times over against the config's own spawn symbols in port/slice_s371.txt.
//
// port_mg_scene_spawn_param takes the FIRST matching row, so on the port this
// scene is "Mario's Slides", record 0, text 0, low byte 0. The low byte is
// what slots 35 and 36 read, so the port's boot is MODE 0 and both predicates
// answer false. Row 24's low byte is 0x02, the only 0x02 in the entire
// 36-row table.
//
// WHICH MODE IS WHICH TITLE IS DERIVED HERE, and section 14 explicitly could
// not do that for dScMgPanel_c. InitResources branches on slot 36 and loads a
// different file set on each arm: the TRUE arm takes the two face_up and
// face_down boards and FIVE localised sheets of CHARACTER FACES (F/G/I/S plus
// the plain English one, off a GetOwnerLanguage ladder), and the FALSE arm
// takes the amida1 and amida2 boards with one plain sprite sheet, no faces and
// no ladder. A board of character heads needs localising; a board of slides
// does not. So slot 36 true is "Connect the Characters" and slot 36 false is
// "Mario's Slides". port/slice_s371.txt section 2 has all twenty-one file ids
// and the control on dScMgPanel_c that fixes the MG-relative base.
//
// THE WITNESS COUNTS THE TWO PREDICATES SEPARATELY for exactly that reason. A
// run of this scene should show slot 36 asked many times and answering 0 every
// time, and slot 35 asked never. Anything else is news.
//
// ---- 5. NO POINTER-TO-MEMBER WALL, AND WHY THAT IS A MEASUREMENT ---------
//
// port/mg_fanout_costs.txt section 4 says no static evidence licenses the word
// "none", so this is established three ways in port/slice_s371.txt: the
// overlay constructor builds no pair table, no TU in the class declares a
// member-pointer type, and -- the one that makes it a measurement -- of the
// 93 relocations landing in this class's code block, the only TWELVE that come
// from a data word are the SpawnInfo's factory word and the eleven vtable
// slots. Nothing in the overlay stores a pointer to this class's code
// anywhere else, so there is no eight-byte table for MSVC to stride by four.
// Slot 6 was also READ, for section 14's practical rule: it is a four-case
// switch on the int at +0x46d0, not the ARM Itanium sequence in plain ints.
//
// THE RUN IS STILL THE DETECTOR and the Behavior counter below is it.
//
// ---- 6. THREE FLOORS, AND THEY ARE ALL ON THE PATH -----------------------
//
// func_ov006_020d27dc (0xe48) and func_ov006_020d36a4 (0x4fc) have config
// symbols, no delink block and no src file in any overlay. Slot 6 calls both;
// func_ov006_020d3ba0 -- which slot 0 and slot 18 both end on -- calls the
// second. Six of this class's 25 slot-36 dispatch sites are inside them, so
// the mode branch in those two bodies is code this seat cannot run.
//
// THE THIRD IS ov004's AND THE LINK FOUND IT, not the delinks walk: this
// class's own block walk cannot see a hole one overlay over.
// func_ov004_020ae5c4 (0x294) is named in ov004's symbols.txt, covered by no
// delink block and defined by no src file, and three of this class's TUs call
// it at ten sites between them. It is also the FAMILY'S SLOT-34 DISPATCHER,
// which is why section 5's am_v34 note can say slot 34 is unreachable on every
// scene today.
//
// port/unmatched/MgAmida_Faces.cpp holds all three counting traps and the
// derivation, and the witness prints their counters beside the green ones
// rather than only when they are nonzero.
//
// ---- 7. THE ROW GOES LAST -------------------------------------------------
//
// port_scene_registry_install walks port_scene_classes[] in table order and
// calls every row's fill on every boot; port_scene_mg_overlay_load runs the
// thirty-five constructors once per process from the tail of the FIRST
// minigame row's fill. Appending after every existing row means the
// constructors have already run against clean ROM words when this fill starts,
// which is the latent-safe direction section 11 names.
//
// FOR THIS CLASS THE RULE IS LOAD-BEARING IN A WAY IT IS NOT FOR THE OTHERS,
// and it is worth saying plainly because this is the one seat in the family
// that writes a 37th word. Section 11's hazard is a fill writing over a word
// something else owns. The 37th word this fill writes is 0x0213b9a8, and the
// three checks above are what say that word belongs to this table: it is a
// load relocation to this class's own code, inside the span, past a slot 35
// that is this class's own override. The word AFTER it, 0x0213b9ac, is the
// next config symbol and this fill never touches it. The scene-374 curling
// canary is the check on all of that rather than this paragraph.

#include "hal/screen_gap.h"

#include <cstdio>
#include <cstdlib>

extern "C" {

/* the seam into hal/scene_mg.cpp -- mounts, arm9 fill, dScMgBase_c's faces */
unsigned port_scene_mg_fill_shared(void **vt, unsigned n);
void     port_scene_mg_prepare(int id);
int      port_scene_env_want(void);
int      IsMinigameActorID(unsigned int id);

/* the mount storage the fill writes into. Inside the ov006 mount, so the fill
   replaces the mount's own words rather than building a fresh host array --
   the ov007, curling and flower treatment, for the same reason: a second host
   array of the same name is a duplicate symbol, and leaving the mounted table
   alone would leave live wild DS pointers in the table the factory installs. */
extern unsigned char data_ov006_0213b918[];   /* dScMgAmida_c, 37 slots */
extern unsigned char data_ov004_020bc0c0[];   /* dScMgBase_c,   36 slots */
extern unsigned char data_ov006_0213b814[];   /* the SpawnInfo record    */

/* the class's eleven overrides, signatures as their src TUs declare them */
int   func_ov006_020d5384(void *c);           /* slot  0 InitResources     */
void *func_ov006_020d5924(char *c, int f);    /* slot  5 AfterCleanup      */
int   func_ov006_020d4b7c(char *c);           /* slot  6 Behavior          */
int   func_ov006_020d48dc(char *c);           /* slot  9 Render            */
void *func_ov006_020d1018(char *c);           /* slot 16 D2                */
void *func_ov006_020d10b8(void *c);           /* slot 17 D0                */
void  func_ov006_020d52f0(char *c, int arg);  /* slot 18 state reset       */
void  func_ov006_020d11a0(void);              /* slot 31 sub BG1 load      */
void  func_ov006_020d14c0(void *c, int a1, int a2, int a3);  /* slot 34    */
int   func_ov006_020d1170(void *c);           /* slot 35 mode == 1         */
int   func_ov006_020d1188(void *c);           /* slot 36 mode == 2         */

/* the factory */
void *func_ov006_020d5974(void);

/* the three floors, from unmatched/MgAmida_Faces.cpp */
unsigned port_mg_amida_floor_27dc(void);
unsigned port_mg_amida_floor_36a4(void);
unsigned port_mg_amida_floor_ae5c4(void);

void port_scene_amida_hits(void);

}  /* extern "C" */

// ---- the tick witness ------------------------------------------------------
//
// One counter per dispatched slot, the instrument every scene seat in this port
// carries, for hal/scene_mg.cpp's reason: an object that EXISTS and an object
// that RUNS look identical from outside. These count only the slots THIS file's
// faces own; the framework slots dScMgBase_c keeps are counted by scene_mg.cpp's
// own array and printed by its atexit report, which a scene-371 boot also
// registers because 371 is an id IsMinigameActorID accepts.
//
// THE ARRAY IS 37 LONG. Every other seat in the family sizes this 36. Slot 36
// is a real slot here and a counter that stopped at 35 would drop the one
// number this lane most needs to print.
static unsigned g_am_hits[37];

/* The two mode predicates get their own counters as well as their slot hits,
   because what matters about them is not only how often they are asked but
   WHAT THEY ANSWER. On the port's boot the spawn param is row 0's zero, so
   slot 35 should answer 0 every time and slot 36 should answer 0 every time;
   a nonzero here means the seated param changed and the scene became a
   different minigame. */
static unsigned g_am_v35_true, g_am_v36_true;

#define AM(n)  (++g_am_hits[(n)])

static int __fastcall am_init(void *s, void *)
{ AM(0);  const int r = func_ov006_020d5384(s);
  /* the GaplessMinigames latch, for hal/scene_mg.cpp's reason: every seated
     minigame calls it so the ones the gapless table does not name can say
     "unsupported" instead of doing nothing quietly. */
  hal_gapless_minigames_latch(); return r; }
/* SLOT 5 TAKES A REAL SECOND ARGUMENT and the ROM body branches on it:
   `cmp r4,#2` at 0x020d5930 with a predicated early-exit epilogue, so a thunk
   that dropped the flag would free the two 0x15800 buffers on every teardown
   flag rather than only on 2. This is dScMgBase_c's own slot-5 shape
   (mb_aclean declares the same third parameter) and not a ride-through. */
static void *__fastcall am_aclean(void *s, void *, int flag)
{ AM(5);  return func_ov006_020d5924((char *)s, flag); }
static int __fastcall am_beh(void *s, void *)
{ AM(6);  return func_ov006_020d4b7c((char *)s); }
static int __fastcall am_render(void *s, void *)
{ AM(9);  return func_ov006_020d48dc((char *)s); }
static void *__fastcall am_d2(void *s, void *)
{ AM(16); return func_ov006_020d1018((char *)s); }
static void *__fastcall am_d0(void *s, void *)
{ AM(17); return func_ov006_020d10b8(s); }
/* SLOT 18 TAKES A REAL SECOND ARGUMENT TOO, and this one is measured rather
   than inherited: `cmp r1,#0` at 0x020d52f8 branches to two different arms --
   the flag==0 arm increments the saved clear count at +0xbc, the flag!=0 arm
   zeroes it and redraws the star badge. dScMgPanel_c's slot 18 is the same
   shape (lane PPP), curling's is the ride-through. A thunk that only CLEANED
   four bytes without forwarding them would take the increment arm on every
   reset. */
static int __fastcall am_reset(void *s, void *, int flag)
{ AM(18); func_ov006_020d52f0((char *)s, flag); return 1; }
/* SLOT 31'S ROM BODY GENUINELY TAKES NO ARGUMENT, which is not an assumption:
   0x020d11a0 opens `push {r4,lr} / ldr r3,[pc,#0x74] / mov r0,#0` and
   overwrites r0 before reading it. src declares it (void) and agrees. */
static int __fastcall am_v31(void *, void *)
{ AM(31); func_ov006_020d11a0(); return 0; }
/* SLOT 34 IS DECLARED WITH ITS FULL ROM SHAPE AND IS NOT DISPATCHED TODAY.
   Both halves are worth stating because the second explains why the first is
   free. Every slot-34 dispatch site in either overlay image passes FIVE
   arguments -- r0..r3 plus one pushed word (the eight sites in
   func_ov004_020ae5c4 all do `str r7,[sp]` before `ldr r4,[r4,#0x88]; blx r4`,
   and the two in func_ov006_020ce108 match) -- and src/func_ov004_020ae3b4.c,
   the BASE's own slot-34 body, declares five parameters to match. This class's
   override reads only four of them (mov r6,r0 / mov r5,r1 / mov r4,r2 /
   mov r8,r3, and src declares four), so the fifth is dropped here on purpose
   and the parameter exists so __fastcall cleans the right number of bytes.
   NEITHER DISPATCHER IS IN A WIRED SLICE: src/func_ov004_020ae5c4.* does not
   exist at all and src/func_ov006_020ce108.cpp belongs to another class and is
   in no slice_*.txt, so nothing in this tree can reach slot 34 on any scene
   today. hal/scene_mg.cpp's shared mb_v34 declares (void *, void *) and calls
   the five-parameter base body with one argument, which is the lane-BASESET
   slot-18/19 defect unexercised rather than absent; it is recorded in this
   lane's report and NOT changed here, because a repair with no dispatch to
   witness it is wiring without evidence. */
static int __fastcall am_v34(void *s, void *, int a1, int a2, int a3, int /*a4*/)
{ AM(34); func_ov006_020d14c0(s, a1, a2, a3); return 0; }
/* THE TWO MODE PREDICATES. Both return their answer, which is the repair lane
   PPP made to mb_v35 after finding it returned a constant 0 and read the stack
   in place of `this`. These take the receiver and return what the ROM returns. */
static int __fastcall am_v35(void *s, void *)
{ AM(35); const int r = func_ov006_020d1170(s); if (r) ++g_am_v35_true; return r; }
static int __fastcall am_v36(void *s, void *)
{ AM(36); const int r = func_ov006_020d1188(s); if (r) ++g_am_v36_true; return r; }

/* SM64DS_SCENE_SLOT0=0 and SM64DS_SCENE_SLOT9=0, the diagnostics the ov003,
   ov007, curling, flower and curling2 seats all carry, counted separately so a
   run can never read a no-op as the real body having run. */
static unsigned g_am_init_skipped, g_am_render_skipped;
static int __fastcall am_init_noop(void *, void *)
{ ++g_am_init_skipped; return 1; }
static int __fastcall am_render_noop(void *, void *)
{ ++g_am_render_skipped; return 1; }

struct AmFace { unsigned ds; void *host; };

/* ELEVEN ROWS, one per override, keyed on the ROM word each slot holds. The
   last two are the pair no other class in the family has. */
static const AmFace kAmidaFaces[] = {
    {0x020d5384u, (void *)am_init},   {0x020d5924u, (void *)am_aclean},
    {0x020d4b7cu, (void *)am_beh},    {0x020d48dcu, (void *)am_render},
    {0x020d1018u, (void *)am_d2},     {0x020d10b8u, (void *)am_d0},
    {0x020d52f0u, (void *)am_reset},  {0x020d11a0u, (void *)am_v31},
    {0x020d14c0u, (void *)am_v34},
    {0x020d1170u, (void *)am_v35},    {0x020d1188u, (void *)am_v36},
};

static unsigned am_apply(void **vt, unsigned n, const AmFace *f, unsigned nf)
{
    unsigned hit = 0;
    for (unsigned i = 0; i < n; ++i) {
        const unsigned ds = (unsigned)(size_t)vt[i];
        for (unsigned k = 0; k < nf; ++k)
            if (f[k].ds == ds) { vt[i] = f[k].host; ++hit; break; }
    }
    return hit;
}

/* Count the words still holding a DS address, which is the only honest check
   that the fill is complete. Called with 37 for this class's table and 36 for
   the base's, and a nonzero answer is a wild call waiting to happen. */
static unsigned am_raw_left(void **vt, unsigned n)
{
    unsigned left = 0;
    for (unsigned i = 0; i < n; ++i) {
        const unsigned w = (unsigned)(size_t)vt[i];
        if (w >= 0x02000000u && w < 0x02400000u)
            ++left;
    }
    return left;
}

extern "C" void port_scene_fill_amida(void)
{
    void **base = (void **)data_ov004_020bc0c0;
    void **vt   = (void **)data_ov006_0213b918;

    /* THE BASE TABLE IS FILLED HERE TOO AND IT IS NOT CEREMONY, for
       hal/scene_mg_flower.cpp's reason: on a tree that carries the curling row
       this is a second pass over words that are already host pointers and finds
       nothing, and it is here so this class does not depend on another class's
       row existing. The factory's first act is func_ov004_020b2adc, which
       writes data_ov004_020bc0c0 into the object's first word before the
       derived table lands. THIRTY-SIX, because the base table is 36 words --
       the 37 below is this class's and nobody else's. */
    port_scene_mg_fill_shared(base, 36);

    /* THE DERIVED TABLE, AND THE ONLY 37 IN THE FAMILY. Shared arm9 and
       dScMgBase_c words first, then this class's eleven. The two key sets are
       disjoint because a word is one address, and the arithmetic closes: 7
       arm9 words, 19 ov004 words, 11 here, 7 + 19 + 11 = 37. */
    const unsigned before = am_raw_left(vt, 37);
    port_scene_mg_fill_shared(vt, 37);
    const unsigned after_shared = am_raw_left(vt, 37);
    const unsigned own = am_apply(vt, 37, kAmidaFaces,
                                  sizeof kAmidaFaces / sizeof kAmidaFaces[0]);
    const unsigned after_own = am_raw_left(vt, 37);

    /* THE FILL CENSUS, PRINTED ON EVERY BOOT AND NOT ONLY WHEN IT FAILS.
       Section 16's rule: a zero that is only printed when it is false is not a
       measurement. This is the one number this lane exists to produce -- the
       family's fill is called with a COUNT, and for this class alone that
       count is 37 -- so the three stages are printed separately and they have
       to add up to 37 in front of the reader:

         before        every slot still holds its mounted ROM word
         after shared  what port_scene_fill_rom (arm9) + kMgBaseFaces (ov004)
                       between them could claim, address-keyed
         own           the eleven this file's own array claims, INCLUDING
                       slots 35 and 36, which no sibling class has
         after own     must be ZERO, or a dispatch of the remainder jumps to a
                       DS address as a host one */
    std::printf("[scene] AMIDA FILL CENSUS, 37 slots: %u raw DS words before, "
                "%u after the shared arm9+dScMgBase_c pass (claimed %u), "
                "%u own faces applied, %u raw left. "
                "dScMgBase_c's own 36-slot table: %u raw left.
",
                before, after_shared, before - after_shared, own, after_own,
                am_raw_left(base, 36));

    /* the two diagnostics, applied after the fill so they override it */
    {
        const char *s0 = std::getenv("SM64DS_SCENE_SLOT0");
        const char *s9 = std::getenv("SM64DS_SCENE_SLOT9");
        if (s0 && s0[0] == '0') vt[0] = (void *)am_init_noop;
        if (s9 && s9[0] == '0') vt[9] = (void *)am_render_noop;
    }

    {
        const unsigned lb = am_raw_left(base, 36);
        const unsigned lv = after_own;
        if (lb || lv) {
            std::fprintf(stderr, "  [scene] AMIDA FILL INCOMPLETE: "
                         "dScMgBase_c leaves %u of 36 raw DS words, "
                         "dScMgAmida_c %u of 37. A dispatch of any of them "
                         "jumps to a DS address as a host one.\n", lb, lv);
            std::fflush(stderr);
        }
    }

    /* The constructors, gated on the requested id, exactly as every other
       minigame fill does it. Idempotent behind scene_mg.cpp's own
       once-per-process guard. THIS CLASS NEEDS IT FOR ITS FILE HANDLES rather
       than for a state table: src/__sinit_ov006_0212fc7c.c registers the three
       SharedFilePtr ids 0x21a, 0x21b and 0x21c that slot 0's asset loads
       resolve through. */
    port_scene_mg_prepare(port_scene_env_want());

    if (IsMinigameActorID((unsigned)port_scene_env_want())) {
        static int armed;
        if (!armed) {
            armed = 1;
            std::atexit(port_scene_amida_hits);
        }
    }
}

/* The registry's factory column is void *(*)(void) and so is the matched
   factory, but the forwarder is kept for the reason title_spawn,
   port_mg_curling_spawn and port_mg_curling2_spawn are: it gives the seat one
   place to observe the object without the registry table growing a second
   column.

   NO HOST COPY OF THE FACTORY IS OWED HERE, and that is measured rather than
   assumed. port/mg_fanout_costs.txt section 12 records dScMgCup_c's factory
   calling func_ov004_020b2adc with NO argument, which on the host reads
   whatever is at [esp+4] and then writes three vtable words through it. This
   factory does `movs r4,r0` and then `bl 0x020b2adc` with the object still in
   r0 at 0x020d598c, and src/func_ov006_020d5974.cpp spells the argument. */
static void *g_am_self;

extern "C" void *port_mg_amida_spawn(void)
{
    void *p = func_ov006_020d5974();
    g_am_self = p;
    return p;
}

extern "C" void port_scene_amida_hits(void)
{
    unsigned total = 0;
    for (int i = 0; i < 37; ++i) total += g_am_hits[i];

    std::printf("[scene] dScMgAmida_c slot hits: init %u, behavior %u, "
                "render %u, state-reset %u%s\n",
                g_am_hits[0], g_am_hits[6], g_am_hits[9], g_am_hits[18],
                g_am_render_skipped
                    ? "  [RENDER SLOT NO-OP'd: SM64DS_SCENE_SLOT9=0]" : "");
    if (g_am_init_skipped)
        std::printf("[scene] INIT SLOT NO-OP'd: SM64DS_SCENE_SLOT0=0, %u "
                    "time(s)\n", g_am_init_skipped);

    std::printf("[scene] dScMgAmida_c own slots entered:");
    for (int i = 0; i < 37; ++i)
        if (g_am_hits[i]) std::printf(" %d(x%u)", i, g_am_hits[i]);
    std::printf("   (%u total)\n", total);

    /* THE TWO MODE PREDICATES, PRINTED UNCONDITIONALLY. This is the number
       this lane exists to produce: slot 36 is the 37th virtual whose existence
       the width column was wrong about twelve times, and a run that dispatches
       it is the strongest statement available that the slot is real. The
       "answered true" columns say WHICH minigame the boot is: the port takes
       ov005 row 0, whose param is zero, so both should read 0 and slot 35
       should not be asked at all. */
    std::printf("[scene] amida mode predicates: slot 35 (mode==1) asked %u, "
                "answered true %u; slot 36 (mode==2) asked %u, answered true "
                "%u  [ov005 row 0, param 0 -> Mario's Slides, both expected 0]"
                "\n", g_am_hits[35], g_am_v35_true,
                g_am_hits[36], g_am_v36_true);

    /* THE TWO FLOORS, PRINTED WHETHER OR NOT THEY FIRED. A zero row is a claim
       and a claim only printed when it is false is not a measurement --
       section 16's rule. Both bodies branch on slot 36 internally, so these
       counters are also the count of mode decisions this seat did not get to
       make. */
    std::printf("[scene] dScMgAmida_c floor asks: func_ov006_020d27dc %u "
                "(0xe48, one caller, vtable slot 6), func_ov006_020d36a4 %u "
                "(0x4fc, two callers, slot 6 and the init tail), "
                "func_ov004_020ae5c4 %u (0x294, ov004, ten call sites in this "
                "class and the family's slot-34 dispatcher)\n",
                port_mg_amida_floor_27dc(), port_mg_amida_floor_36a4(),
                port_mg_amida_floor_ae5c4());

    /* THE STATE INDEX IS PRINTED BECAUSE IT IS THE ONLY THING THAT SEPARATES
       "the behavior slot ran" from "the state machine ran". Slot 6 is a
       four-case switch on the int at +0x46d0 and sets it to 1 out of case 0;
       slot 0 zeroes +0x53d4 and writes +0x53e8 = +0xbc * 5; slot 18 zeroes
       +0x53e8 and +0xbc. +0xbc is this class's saved clear count, capped at
       0x270e the way dScMgMemory2_c's +0xb4 is. */
    if (g_am_self) {
        const char *c = (const char *)g_am_self;
        std::printf("[scene] amida state +0x46d0 = %d, clear count +0xbc = %d, "
                    "score +0x53e8 = %d, +0x53d4 = %d, layout +0x4700 = %d\n",
                    *(const int *)(c + 0x46d0), *(const int *)(c + 0x00bc),
                    *(const int *)(c + 0x53e8), *(const int *)(c + 0x53d4),
                    *(const int *)(c + 0x4700));
        /* The mode byte the two predicates read, straight out of the object,
           so the census above can be checked against its own input rather than
           only against the ov005 table this file quotes. */
        std::printf("[scene] amida mode byte (field_8 & 0xff) = %u, field_8 = "
                    "0x%08x\n",
                    (unsigned)(*(const unsigned *)(c + 8) & 0xffu),
                    *(const unsigned *)(c + 8));
    } else {
        std::printf("[scene] amida state: the class never spawned\n");
    }

    std::fflush(stdout);
}

/* ---- THE NAME-SPELLING FACES --------------------------------------------
 *
 * SEVEN ROWS, all /alternatename, none a stand-in for a body. Every one was
 * checked against port/mg_fanout_costs.txt section 10 finding 1 FIRST -- the
 * facegen failure that aliased curling's twenty-five-entry pointer-to-member
 * STATE TABLE because its wall test is `"P8" in sym` and a struct wrapper
 * hides the P8, and whose refusal path misses the @@3PAU array spelling too.
 * TWO OF THE SEVEN ARE EXACTLY THAT SHAPE ON THE SURFACE and both are ruled
 * safe on evidence rather than on the guard's silence.
 *
 * THE COMMON HALF. All five data rows are the same defect: a TU spells a
 * mounted DS symbol under a C++ type, so MSVC mangles the TYPE into the name
 * and the ov006 / arm9 mount's plain C symbol cannot satisfy it. The ADDRESS
 * is the same either way, no storage is involved, and an alias is exact.
 *
 * ROW 1 and 2 -- THE STYLUS RECORD, from src/func_ov006_020d1958.cpp, which
 * declares `extern unsigned char data_020a0dea[][4];` and reads
 * data_020a0dea[slot][0]. That is an ARRAY of 4-byte records, not a pointer,
 * and MSVC spells an unknown-bound array of unsigned char[4] as @@3PAY03EA.
 * hal/auto_bss.cpp lays the four stylus names out CONTIGUOUSLY in
 * .dsstate$touch0000..0003 precisely so a stride-4 read through any one of
 * them lands on the right byte of the right record, so the alias is not only
 * name-correct, it is the layout that file exists to provide.
 * hal/cxx_aliases.cpp:627 already carries the @@3PAEA spelling of the same
 * symbol, which is a DIFFERENT LHS and therefore not a duplicate directive:
 * port/tools/alternatename_guard.py counts pragmas per LHS, and two spellings
 * of one symbol from two different declarations are two rows by construction.
 *
 * ROW 3 -- G2S::GetBG0CharPtr. src/func_ov006_020d3624.cpp, _020d3668.cpp and
 * the host copy of _020d3ba0 all declare it inside `namespace G2S`, so MSVC
 * emits ?GetBG0CharPtr@G2S@@YAPADXZ. The Y and the A in that mangling say
 * __cdecl FREE FUNCTION IN A NAMESPACE, not a method -- which is section 10
 * finding 4's case, the one facegen classifies as a reverse face and refuses
 * for the wrong reason. Both sides are __cdecl with zero arguments and the
 * arm9 body is _ZN3G2S13GetBG0CharPtrEv at 0x02054edc, so a plain alias is
 * correct and no argument-landing face is owed.
 *
 * ROW 4 -- data_ov006_0213b8b8, AND THIS IS THE ONE THAT LOOKS LIKE THE WALL.
 * src/func_ov006_020d3ba0.c declares `typedef struct { int a; int b; } Pair;`
 * and `extern Pair data_ov006_0213b8b8[11]`, which mangles @@3PAUPair@@A --
 * the exact array-of-struct spelling that slipped both of facegen's guards on
 * curling's state table. IT IS NOT A MEMBER-POINTER TABLE and there are two
 * independent witnesses:
 *   1. THE RELOCATION SET. config/arm9/overlays/ov006/relocs.txt has NO row at
 *      all in 0x0213b8b8..0x0213b910. A {code, adjustment} table would carry
 *      eleven load relocations on its code words, one per pair; a table with
 *      zero relocations contains no pointers.
 *   2. THE BYTES. Read out of extracted/overlays/overlay_0006.bin at base
 *      0x020bfec0 the eleven pairs are {0x20,-0x87} {0xa0,-0x87} {0x60,-0x5a}
 *      {0x20,-0x2d} {0xa0,-0x2d} {0x60,-0x78} {0x20,-0x69} {0xa0,-0x69}
 *      {0x20,-0x4b} {0xa0,-0x4b} {0x60,-0x3c} -- small signed screen
 *      coordinates, not code addresses.
 * And section 4's rulebook corollary keys on the SPELLING as well as the use:
 * "A PAIR WHOSE CONSUMER SPELLS IT AS TWO INTS IS SAFE AS AN ALIAS." This
 * consumer spells two ints, `struct { int a; int b; }` is eight bytes on both
 * machines, and the only thing it does with the table is copy the block.
 *
 * ROW 5 -- data_ov006_0213b880, the BY-VALUE struct spelling @@3UBuf14@@A that
 * facegen does refuse. Same treatment and the same two witnesses:
 * src/func_ov006_020d48dc.cpp declares `typedef struct { int v[14]; } Buf14;`
 * and copies it whole into a local, the fourteen words read
 * {2,1,1,1,2,2,2,2,2,2,2,1,1,1}, and no relocation lands anywhere in
 * 0x0213b880..0x0213b8b4. Fourteen ints are 56 bytes on both machines.
 *
 * ROWS 6 and 7 -- data_ov006_0213a458 and _0213a4b0, declared `extern void*
 * data_ov006_0213a458[]` and indexed by a per-slot number before being handed
 * to func_ov004_020afdd0. These ARE pointer tables, and that is why the alias
 * is right rather than in spite of it: every word in them carries a load
 * relocation (from:0x0213a458 -> 0x0213a450, ..45c -> ..448, ..460 -> ..440
 * and so on), so tools/ovdata.py emits them with their interior pointers
 * REBASED into the mount. Aliasing the C++ spelling onto the mount's cell is
 * what hands the callee a host address; a private host array would hand it a
 * DS one.
 */
#pragma comment(linker, "/alternatename:?data_020a0dea@@3PAY03EA=_data_020a0dea")
#pragma comment(linker, "/alternatename:?data_020a0deb@@3PAY03EA=_data_020a0deb")
#pragma comment(linker, "/alternatename:?GetBG0CharPtr@G2S@@YAPADXZ=__ZN3G2S13GetBG0CharPtrEv")
#pragma comment(linker, "/alternatename:?data_ov006_0213b8b8@@3PAUPair@@A=_data_ov006_0213b8b8")
#pragma comment(linker, "/alternatename:?data_ov006_0213b880@@3UBuf14@@A=_data_ov006_0213b880")
#pragma comment(linker, "/alternatename:?data_ov006_0213a458@@3PAPAXA=_data_ov006_0213a458")
#pragma comment(linker, "/alternatename:?data_ov006_0213a4b0@@3PAPAXA=_data_ov006_0213a4b0")
