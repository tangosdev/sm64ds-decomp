/* hal/scene_mg_snowball.cpp -- run mg11 lane SNW.
 *
 * dScMgSnowball_c, actor id 0x179 = scene 377, the "Snowball Slalom" minigame.
 * The seat: this class's own vtable faces, its middle-base copy, the fill, the
 * spawn forwarder and the census.  The pointer-to-member wall is next door in
 * port/unmatched/MgSnowball_StateDispatch.cpp; port/slice_snw.txt is the
 * derivation and the slice.
 *
 * ---- 1. THIS IS THE ONE VTABLE NO TOOL RESOLVED --------------------------
 *
 * port/mg_fanout_costs.txt section 3 ranks all thirty minigame classes off a
 * load relocation inside each id's own factory.  Twenty-nine resolve.  This
 * one did not, and the row was left BLANK rather than guessed: "MgSnowball-
 * Slalom (0x179) has a SpawnInfo at 0x0213ffb8 and a factory, and no load
 * relocation inside that factory points at any of the thirty-two signature
 * tables."
 *
 * THAT SENTENCE IS TRUE AND THE REASON IS ONE DELEGATED CONSTRUCTOR.
 * src/MgSnowballSlalom_Spawn.cpp (0x0212a520, 12 instructions plus a one-word
 * pool = 13 = 0x34/4) writes no vtable at all.  It allocates 0xc59c through
 * func_02043444, returns early on a null, and then calls func_ov006_021295ac
 * with the object in r0 and returns that callee's value:
 *
 *     0x0212a528  ldr  r0,[pc,#0x20]     ; the pool word 0x0000c59c
 *     0x0212a52c  bl   0x02043444        ; allocate
 *     0x0212a530  cmp  r0,#0             ; null -> return it
 *     0x0212a540  bl   0x021295ac        ; the constructor
 *
 * and func_ov006_021295ac is the writer.  Its pool holds BOTH vtables and it
 * stores them into the object's first word in inheritance order:
 *
 *     0x021295c8  str  r1,[r4]           ; r1 = 0x0213e448  dScMgSingle3DBase_c
 *     0x021295dc  str  r1,[r4]           ; r1 = 0x0214000c  THIS CLASS
 *
 * so the object this factory produces carries data_ov006_0214000c, and this
 * class is a FOURTH class under the same middle base.
 *
 * IT IS NOT ADOPTED FROM THE HANDOVER, IT IS RE-DERIVED, and four independent
 * readings of the relocation set close the ring rather than one:
 *
 *   - MgSnowballSlalom_Spawn is the ONLY caller of func_ov006_021295ac
 *     anywhere in ov006 (one arm_call, from:0x0212a540).
 *   - func_ov006_021295ac is the ONLY body that writes 0x0214000c into an
 *     object it constructs.
 *   - The other two references to 0x0214000c in the entire overlay are
 *     func_ov006_0212568c and func_ov006_0212573c, which are SLOTS 16 AND 17
 *     OF THE TABLE ITSELF -- the D2 and the D0 -- and both write 0x0214000c on
 *     entry and 0x0213e448 on the way out.  The vtable names its own
 *     destructors and its own destructors name it.
 *   - The two destructors' literal pools hold the SAME five member offsets the
 *     constructor's does (0x471c, 0xaba4, 0xacd8, 0xb5d8, 0xba14), so they are
 *     reading and unwinding one object layout.
 *
 * THE ALTERNATIVE THE COSTS FILE OFFERED IS RULED OUT, not left open.  Its
 * other hypothesis was that "its class is one of the three signature tables no
 * id claims".  Those three are 0x0213e448, 0x0213c62c and 0x0214000c, and the
 * first two are not candidates for ANY single id:
 *
 *   0x0213e448  41 load relocations, from the constructors of at least
 *               fourteen different classes (MgPicturePoker, MgPsycheOut,
 *               MgMemoryMatch, MgMemoryMaster, MgPairAGoneAndOn,
 *               MgMushroomRoulette, MgBoomBox, MgLuckyStars and this one among
 *               them).  It is dScMgSingle3DBase_c, the shared middle base.
 *   0x0213c62c  14 load relocations, from MgBounceAndPounce_Spawn,
 *               MgTrampolineTime_Spawn, MgTrampolineTerror_Spawn,
 *               PathLift::BaseInitResources and the MgBounceAndPounce
 *               destructors.  A second shared base, one family further along.
 *
 * A table five classes write is not one class's vtable.  0x0214000c has three
 * references and all three are this class's own bodies.
 *
 * ---- 2. FIVE WIDTH CHECKS, AND THE FIFTH IS THE STRIDE -------------------
 *
 * The table is THIRTY-SIX words.  port/slice_snw.txt section 3 carries the
 * arithmetic; the short form:
 *
 *   span       0x0214000c..0x0214009c is 0x90 = 36 words, and the next object
 *              is not a table at all.
 *   slot 35    holds 0x020ad660, the family's terminal word, at 0x02140098.
 *   the word   at 0x0214009c is 0x2f474d2f -- ASCII "/MG/", the head of
 *              "/MG/d_2d_mg_bg_yukidama_ncl.bin".  Not a code address.
 *   relocs     36 load relocations inside 0x0214000c..0x0214009c, exactly one
 *              per slot, and ZERO in the 0x40 bytes after it.
 *   stride     those 36 are at a uniform stride of 4 with no gap, so no slot
 *              inside the span is a plain constant and the count cannot be
 *              short.
 *
 * THE WIDTH WORD IS ALSO AN IDENTITY WITNESS, which is a coincidence worth
 * writing down because it makes the check unusually cheap to re-run.  The
 * string a thirty-seventh slot would have eaten is a FILE THIS CLASS LOADS:
 * src/func_ov006_02129268 (slot 0, InitResources) calls func_020adc74 on
 * &data_ov006_0214009c, and yukidama is Japanese for snowball.
 *
 * ---- 3. FIVE INDEPENDENT WITNESSES SAY THIS IS SNOWBALL SLALOM ----------
 *
 *   1. The ROM's own RTTI.  The word BEFORE the vtable, 0x02140008, points at
 *      0x0213ffd0, whose name pointer 0x0213ffdc reads "15dScMgSnowball_c".
 *   2. The SpawnInfo.  0x0213ffb8 holds {0x0212a520, 0x01790179} -- the
 *      factory and the actor id twice, which is what
 *      port_scene_registry_install checks the row against.
 *   3. The config's recovered names: dScMgSnowball_c_InitResources,
 *      _CleanupResources, _Behavior, _OnKicked and so on, and a generated
 *      header src/../dScMgSnowball_c.h.
 *   4. Four ROM strings this class's InitResources loads:
 *      /MG/d_2d_mg_bg_yukidama_ncl.bin, /MG/d_2d_mg_yukidama_ncg.bin,
 *      /MG/d_2d_mg_yukidama_ncl.bin and /MG/snowman_head.bmd.
 *   5. The ov005 launch table and the BMG chain (section 4).
 *
 * ---- 4. TWO PLAYER TITLES ON ONE ID -------------------------------------
 *
 * This id has TWO rows in the ov005 launch table, the dScMgPanel_c and
 * dScMgAmida_c shape:
 *
 *   row  6   param 0x00060a00   record 10   text 6   low byte 0x00
 *   row 34   param 0x00082201   record 34   text 8   low byte 0x01
 *
 *     text 6 -> data_ov004_020bc070[6] = message 554  "Snowball Slalom"
 *     text 8 -> data_ov004_020bc070[8] = message 556  "Giant Snowball Slalom"
 *
 * and the bodies say they are the same game twice:
 *     554  "Rub the Touch Screen to roll the snowman's head to the goal
 *           before time runs out! Rocks will slow you down, as will touching
 *           his head!"
 *     556  "Roll the snowball over a longer course! As before, avoid rocks
 *           because they'll slow you down."
 *
 * THE ROWS THEMSELVES CORROBORATE "A LONGER COURSE" without going near the
 * text: each row carries a five-entry array at +0x14, and row 6's reads 1200
 * five times while row 34's reads 2400.  The second row is the same course at
 * double the distance.
 *
 * port/slice_s371.txt's 25-row control decoded this chain first and named this
 * class "Snowball Slalom"; it lists only ONE of this id's two titles, and the
 * second is added here rather than treated as a disagreement -- the chain is
 * the same and reproduces digit for digit, its list simply stopped at one row
 * per config name.  port_mg_scene_spawn_param takes the FIRST matching row, so
 * scene 377 boots as row 6: save record 10, name text 6, "Snowball Slalom".
 *
 * ---- 5. SIXTEEN OVERRIDES, FOURTEEN MARKERS, ZERO WITHOUT A SOURCE ------
 *
 * port/tools/vtablerows.py --vtable 0x0214000c --width 36 prints ovr 16, mark
 * 14, nosrc 0.  That is the row port/mg_fanout_costs.txt section 3 left blank,
 * now filled from the ROM.
 *
 * Of the sixteen, SIX are the middle base's own overrides rather than this
 * class's: slots 2, 5, 7, 10, 26 and 33 hold the same words
 * data_ov006_0213e448 holds.  TEN are this class's own, which is what the
 * kSnowFaces array below covers.  Five of the fourteen markers are the middle
 * base's shared glue and already carried rulings from run link60 lane MGB and
 * run mg9 lanes LKY, MMT and PSY; they are RE-READ here rather than cited.
 * The other nine are this class's and were disassembled and ruled before a
 * word was seated, with the evidence appended to
 * port/tools/inferred_stub_adjudicated.txt.  RESULT: 14 REAL_DECOMP, 0 guesses.
 *
 * SLOT 18 READS ITS SECOND ARGUMENT AND THE THUNK MUST FORWARD IT.  This is
 * the trap run mg9 lane LKY found in dScMgBSC_c and relayed, and this class is
 * the second instance: func_ov006_0212921c saves r1 into r4 at 0x0212922c and
 * compares it against 0x13 at 0x0212923c, and only on a match does it run
 * func_ov006_02126ee4 and func_ov006_02126a98.  A thunk that merely POPPED the
 * argument -- which is all most seated classes needed -- would hand the body
 * stack litter and let that arm fire at random.  It is forwarded and printed.
 *
 * ---- 6. THE MIDDLE TABLE IS NOT THIS SEAT'S TO CLAIM --------------------
 *
 * This is the FOURTH class the port seats under dScMgSingle3DBase_c, after the
 * flower row, the memory2 row and the luckystars row.  All four define their
 * own face array over the same eight DS words and the fill keys on a DS
 * address, so whichever row runs FIRST claims the middle table and the later
 * ones find nothing left to write.  This row is APPENDED after all three, so
 * the flower keeps it and all three earlier witnesses keep counting exactly
 * what they counted before this seat existed.  The count this seat's own
 * middle-table pass claims is printed rather than assumed, and 0 is the
 * expected reading on a tree that carries the flower row.
 *
 * THE SEAM PROMOTION IS STILL DUE AND STILL NOT TAKEN HERE.
 * hal/scene_mg_memory2.cpp called for kSingle3DFaces to become an exported
 * seam at the third class; hal/scene_mg_luckystars.cpp recorded the debt at
 * the third and declined mid-fan-out.  This is the fourth and the reason not
 * to take it is unchanged and now stronger: eight lanes of run mg11 are
 * seating classes concurrently out of their own worktrees, and promoting a
 * shared array mid-run turns a one-file seat into a five-way merge.  The debt
 * is recorded, not paid.
 */

#include "hal/screen_gap.h"

#include <cstdio>
#include <cstdlib>

extern "C" {

/* The fan-out seam.  hal/scene_mg.cpp owns kMgBaseFaces and the arm9 fill; a
   per-class seat never copies that array (a second copy would install a
   different host thunk over the same DS word and the per-slot census would
   count only whichever ran last). */
unsigned port_scene_mg_fill_shared(void **vt, unsigned n);
void     port_scene_mg_prepare(int id);
int      port_scene_env_want(void);

/* The three tables this seat fills, all MOUNTED ROM words.  The fill replaces
   the mounted words rather than building a fresh host array -- the ov007,
   curling, memory2 and luckystars treatment, for the same reason: a second
   host array of the same name is a duplicate symbol, and leaving the mounted
   table alone leaves live wild DS pointers in a table the factory installs. */
extern unsigned char data_ov004_020bc0c0[];   /* dScMgBase_c,         36 */
extern unsigned char data_ov006_0213e448[];   /* dScMgSingle3DBase_c, 36 */
extern unsigned char data_ov006_0214000c[];   /* dScMgSnowball_c,     36 */
extern unsigned char MgSnowballSlalom_SpawnInfo[];

/* The middle base's eight, spelled with the parameter list each src TU
   defines.  port_mg_flower_after_init is unmatched/MgFlower_Slot2.cpp's
   repair of slot 2's ride-through, not the src body. */
int   port_mg_flower_after_init(void *c, unsigned f);   /* slot  2 */
void  func_ov006_0210a608(void *c, unsigned f);         /* slot  5 */
int   func_ov006_0210a698(void *c);                     /* slot  7 */
int   func_ov006_0210a664(void *c);                     /* slot 10 */
int   func_ov006_0210a4b0(char *c);                     /* slot 16 D2 */
int   func_ov006_0210a4e8(char *c);                     /* slot 17 D0 */
int   func_ov006_0210a600(void);                        /* slot 26 */
void  func_ov006_0210a708(char *c);                     /* slot 33 */

/* This class's own ten.  Slot 6 is the src TU and NOT a host copy: it holds
   no pointer-to-member dispatch of its own -- a body-by-body scan of the ROM
   for the five-instruction Itanium sequence finds zero sites in it -- and
   reaches the state machine by calling func_ov006_0212a2e0, which IS host-
   copied.  That is the whole shape port/mg_fanout_costs.txt section 14 warns
   about, read the right way round. */
int   func_ov006_02129268(void *self);        /* slot  0 InitResources     */
int   func_ov006_021291f8(void *self);        /* slot  3 CleanupResources  */
int   func_ov006_021283a4(char *self);        /* slot  6 Behavior          */
int   func_ov006_02127d10(char *self);        /* slot  9 Render            */
void *func_ov006_0212568c(char *c);           /* slot 16 D2                */
void *func_ov006_0212573c(char *c);           /* slot 17 D0                */
void  func_ov006_0212921c(void *c, int mode); /* slot 18 state reset       */
int   func_ov006_021291d4(void *self);        /* slot 23                   */
int   func_ov006_02128fb8(char *self);        /* slot 24                   */
int   func_ov006_021291b0(void *self);        /* slot 25                   */

/* The factory, linked from the slice.  It needs no displacement ruling: it
   calls func_02043444 with its own size argument and hands the RESULT to
   func_ov006_021295ac, which is the constructor -- there is no dropped
   argument riding through in r0.  (The section-12 grant exists for 0x169,
   whose factory calls the base constructor with no argument at all.) */
void *MgSnowballSlalom_Spawn(void);

/* unmatched/MgSnowball_StateDispatch.cpp */
unsigned port_mg_snowball_state_hits(void);
unsigned port_mg_snowball_floor_hits(void);
unsigned port_mg_snowball_live(void);
void     port_mg_snowball_sel_range(int *lo, int *hi);
void     port_mg_snowball_slot_hits(const unsigned **v, unsigned *n);

/* unmatched/MgSnowball_Faces.cpp used to declare a counter accessor for three
   trapped closure floors here.  Run mg12 lane SNO decompiled and seated all
   three (slice_snw.txt section 17), so the traps and the accessor are gone and
   the census below measures what the real bodies WROTE instead. */

/* unmatched/MgBase_StateDispatch.cpp's framework census. */
void     port_mg_dispatch_counts(unsigned *calls, unsigned *unknown);

/* The persistent per-minigame save record this class's param points at.
   Nothing new is defined here: the storage is hal/level_boot.cpp's
   .dsstate$savblk0004 and the index function is src/func_ov004_020adc3c.c. */
extern void         *data_ov004_020beb68;     /* the live dScMgBase_c `this` */
int  func_ov004_020adc3c(void *c);            /* (self->field_8 >> 8) & 0xff */

}  /* extern "C" */

static int port_mg_snowball_record_index(void)
{
    return data_ov004_020beb68 ? func_ov004_020adc3c(data_ov004_020beb68) : -1;
}

/* Per-slot hit counters.  Two arrays because this class's table and the
   middle base's table are different tables with the same slot numbering, and
   a single array would silently sum them. */
static unsigned g_snw_hits[36];
static unsigned g_snw_base_hits[36];

#define SNW(n)   (++g_snw_hits[(n)])
#define B3D(n)   (++g_snw_base_hits[(n)])

/* Section 5 says slot 18's argument is READ here, and a census that does not
   print it cannot show the thunk delivered it. */
static int g_snw_mode18 = -1;

/* ---- the middle base's eight ------------------------------------------- */
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

/* ---- this class's own ten ---------------------------------------------- */
static int  __fastcall snw_init(void *s, void *)
{ SNW(0);  const int r = func_ov006_02129268(s);
  /* Latch the gapless machine so a minigame the gapless table does not name
     can say "unsupported" rather than doing nothing quietly.
     hal_gapless_splice() is deliberately NOT called: scene 377 has no row in
     screen_gap.cpp's table, and gapless behaviour is decided per minigame BY
     PLAYING IT.  This lane has not played it and does not get to vote.  Same
     ruling as hal/scene_mg_luckystars.cpp and hal/scene_mg_flower.cpp. */
  hal_gapless_minigames_latch(); return r; }
static int  __fastcall snw_clean(void *s, void *)
{ SNW(3);  return func_ov006_021291f8(s); }
/* ---- SM64DS_SNW_TRACE: the ROLL CHAIN, link by link ---------------------
 *
 * A scroll-word census answers "did the number move" and NOTHING ELSE, and a
 * single output word cannot tell a game that will not respond apart from a
 * game that was driven badly.  Run mg12 lane CRD's finding is the reason this
 * exists: two card games were called deadlocked on a dispatch census when the
 * scripted taps had simply never made a legal move.
 *
 * So this samples every link of scene 377's chain once per Behavior tick, in
 * the order the ROM computes them, and the census prints the whole chain:
 *
 *   the stylus record  ->  +0xab70/+0xab74 the latched stroke origin
 *                      ->  +0xab60/+0xab64 velocity
 *                      ->  +0xab38/+0xab3c ball position
 *                      ->  +0xab6c         BG scroll
 *
 * plus +0xb9e4, the byte func_ov006_02125f68 sets when the obstacle pass finds
 * the ball touching something.  That one is the check on the SEAT rather than
 * on the input: with the floors trapped it could never be set, so a run where
 * it stays zero is a run where the rocks and walls were never reached, and
 * "the ball travelled the same distance as before the seat" means something
 * completely different depending on which way it reads.
 *
 * Off unless the variable is set, sampled read-only, and host state - nothing
 * here is in .dsstate and nothing writes the object. */
static int      g_snw_trace;
static unsigned g_snw_t_ticks, g_snw_t_moving, g_snw_t_contact, g_snw_t_hole;
static int      g_snw_t_vxmax, g_snw_t_vymax;
static int      g_snw_t_scroll0 = -1, g_snw_t_scrollmin, g_snw_t_ymin;
/* PUSH-BACK, which is the only way to see a WALL from outside the object.
   func_ov006_02125f68 sets +0xb9e4 only on an OBSTACLE contact; its wall arm
   is the `+0xb9e4 != 1` branch and sets nothing, so a wall collision is
   invisible to that byte.  What both arms DO is move the ball backwards --
   `pos -= push` -- so a tick whose Y rose while the ball was under forward
   velocity is a tick the seated obstacle pass pushed it out of geometry.
   Counting those is what turns "the ball stopped" from an inference into a
   measurement. */
static int      g_snw_t_ylast = -1;
static unsigned g_snw_t_back;

static int  __fastcall snw_beh(void *s, void *)
{
    SNW(6);
    const int r = func_ov006_021283a4((char *)s);
    if (g_snw_trace > 0 && s) {
        const char *c = (const char *)s;
        const int vx = *(const int *)(c + 0xab60);
        const int vy = *(const int *)(c + 0xab64);
        const int y  = *(const int *)(c + 0xab3c);
        const int sc = *(const int *)(c + 0xab6c);
        ++g_snw_t_ticks;
        if (vx || vy) ++g_snw_t_moving;
        if (*(const unsigned char *)(c + 0xb9e4)) ++g_snw_t_contact;
        if (*(const unsigned char *)(c + 0xb9e5)) ++g_snw_t_hole;
        if (vx > g_snw_t_vxmax) g_snw_t_vxmax = vx;
        if (vy < g_snw_t_vymax) g_snw_t_vymax = vy;   /* forward is NEGATIVE */
        if (g_snw_t_scroll0 < 0) {
            g_snw_t_scroll0 = sc; g_snw_t_scrollmin = sc; g_snw_t_ymin = y;
        }
        if (sc < g_snw_t_scrollmin) g_snw_t_scrollmin = sc;
        if (y  < g_snw_t_ymin)      g_snw_t_ymin = y;
        if (g_snw_t_ylast >= 0 && y > g_snw_t_ylast && vy < 0) ++g_snw_t_back;
        g_snw_t_ylast = y;
    }
    return r;
}
static int  __fastcall snw_render(void *s, void *)
{ SNW(9);  return func_ov006_02127d10((char *)s); }
static void *__fastcall snw_d2(void *s, void *)
{ SNW(16); return func_ov006_0212568c((char *)s); }
static void *__fastcall snw_d0(void *s, void *)
{ SNW(17); return func_ov006_0212573c((char *)s); }
/* The forwarding thunk section 5 argues for.  The value is kept for the
   census so the run can show it arrived rather than assert it. */
static int  __fastcall snw_reset(void *s, void *, int mode)
{ SNW(18); g_snw_mode18 = mode; func_ov006_0212921c(s, mode); return 1; }
static int  __fastcall snw_v23(void *s, void *)
{ SNW(23); return func_ov006_021291d4(s); }
static int  __fastcall snw_v24(void *s, void *)
{ SNW(24); return func_ov006_02128fb8((char *)s); }
static int  __fastcall snw_v25(void *s, void *)
{ SNW(25); return func_ov006_021291b0(s); }

/* The two env no-ops every seat carries, counted separately so a run can
   never read a no-op as the real body having run. */
static unsigned g_snw_init_skipped, g_snw_render_skipped;
static int __fastcall snw_init_noop(void *, void *)
{ ++g_snw_init_skipped; return 1; }
static int __fastcall snw_render_noop(void *, void *)
{ ++g_snw_render_skipped; return 1; }

struct SnwFace { unsigned ds; void *host; };

/* Keyed on the ROM WORD the slot holds, never on a slot index, so a fill
   cannot land on a slot the ROM did not park that body in. */
static const SnwFace kSingle3DFaces[] = {
    {0x0210a6e4u, (void *)s3_ainit},  {0x0210a608u, (void *)s3_aclean},
    {0x0210a698u, (void *)s3_bbeh},   {0x0210a664u, (void *)s3_bren},
    {0x0210a4b0u, (void *)s3_d2},     {0x0210a4e8u, (void *)s3_d0},
    {0x0210a600u, (void *)s3_v26},    {0x0210a708u, (void *)s3_v33},
};

static const SnwFace kSnowFaces[] = {
    {0x02129268u, (void *)snw_init},  {0x021291f8u, (void *)snw_clean},
    {0x021283a4u, (void *)snw_beh},   {0x02127d10u, (void *)snw_render},
    {0x0212568cu, (void *)snw_d2},    {0x0212573cu, (void *)snw_d0},
    {0x0212921cu, (void *)snw_reset}, {0x021291d4u, (void *)snw_v23},
    {0x02128fb8u, (void *)snw_v24},   {0x021291b0u, (void *)snw_v25},
};

static unsigned snw_apply(void **vt, unsigned n, const SnwFace *f, unsigned nf)
{
    unsigned hit = 0;
    for (unsigned i = 0; i < n; ++i) {
        const unsigned ds = (unsigned)(size_t)vt[i];
        for (unsigned k = 0; k < nf; ++k)
            if (f[k].ds == ds) { vt[i] = f[k].host; ++hit; break; }
    }
    return hit;
}

/* Words still holding a DS address after the fill.  Every one of a minigame
   table's 36 slots is dispatched by something, so a nonzero answer is a wild
   call waiting to happen and the seat says so out loud. */
static unsigned snw_raw_left(void **vt, unsigned n)
{
    unsigned left = 0;
    for (unsigned i = 0; i < n; ++i) {
        const unsigned w = (unsigned)(size_t)vt[i];
        if (w >= 0x02000000u && w < 0x02400000u)
            ++left;
    }
    return left;
}

extern "C" void port_scene_snowball_hits(void);

/* How many words THIS seat's middle-table pass claimed.  0 is the EXPECTED
   reading on a tree that carries the flower row (section 6); it is printed
   rather than assumed. */
static unsigned g_snw_mid_claimed;

extern "C" void port_scene_fill_snowball(void)
{
    void **base = (void **)data_ov004_020bc0c0;
    void **mid  = (void **)data_ov006_0213e448;
    void **vt   = (void **)data_ov006_0214000c;

    /* dScMgBase_c's own 36 first.  On a tree carrying the earlier rows this is
       a second pass over words that are already host pointers and finds
       nothing; it is here so this class does not depend on another class's row
       existing.  The constructor's first act is func_ov004_020b2adc, which
       writes data_ov004_020bc0c0 into the object's first word before either
       derived table lands. */
    port_scene_mg_fill_shared(base, 36);

    /* The middle base next.  The constructor writes 0x0213e448 into the object
       on the way down and 0x0214000c over it, so a slot dispatched between the
       two vptr writes reads the middle table.  Section 6 is why applying this
       file's own arrays here is safe next to the flower's, memory2's and
       luckystars'. */
    port_scene_mg_fill_shared(mid, 36);
    g_snw_mid_claimed =
        snw_apply(mid, 36, kSingle3DFaces,
                  sizeof kSingle3DFaces / sizeof kSingle3DFaces[0]);
    g_snw_mid_claimed +=
        snw_apply(mid, 36, kSnowFaces,
                  sizeof kSnowFaces / sizeof kSnowFaces[0]);

    /* Then this class's own: the framework's shared words, the middle base's
       eight, then this class's ten.  Order does not matter -- the three key
       sets are disjoint, because a word is one address -- but it reads in
       inheritance order.  THIRTY-SIX, not thirty-seven: section 2, and the
       word a thirty-seventh would take is the filename string this class's
       own InitResources loads. */
    port_scene_mg_fill_shared(vt, 36);
    snw_apply(vt, 36, kSingle3DFaces,
              sizeof kSingle3DFaces / sizeof kSingle3DFaces[0]);
    snw_apply(vt, 36, kSnowFaces,
              sizeof kSnowFaces / sizeof kSnowFaces[0]);

    {
        const char *s0 = std::getenv("SM64DS_SCENE_SLOT0");
        const char *s9 = std::getenv("SM64DS_SCENE_SLOT9");
        if (s0 && s0[0] == '0') vt[0] = (void *)snw_init_noop;
        if (s9 && s9[0] == '0') vt[9] = (void *)snw_render_noop;
    }

    {
        const unsigned lb = snw_raw_left(base, 36);
        const unsigned lm = snw_raw_left(mid, 36);
        const unsigned lv = snw_raw_left(vt, 36);
        if (lb || lm || lv) {
            std::fprintf(stderr, "  [scene] SNOWBALL FILL INCOMPLETE: "
                         "dScMgBase_c leaves %u of 36 raw DS words, "
                         "dScMgSingle3DBase_c %u, dScMgSnowball_c %u. A "
                         "dispatch of any of them jumps to a DS address as a "
                         "host one.\n", lb, lm, lv);
            std::fflush(stderr);
        }
    }

    /* Run the thirty-five ov006 constructors once per process; the first
       minigame fill does it.  Idempotent behind scene_mg.cpp's own guard. */
    port_scene_mg_prepare(port_scene_env_want());

    if (port_scene_env_want() == 377) {
        static int armed;
        if (!armed) {
            armed = 1;
            std::atexit(port_scene_snowball_hits);
        }
    }
}

/* The registry calls the factory through this forwarder, the way title_spawn
   and port_mg_curling_spawn are called, so the seat has one place to observe
   the object without the registry table growing a second column. */
static char *g_snw_self;

/* THE CORRIDOR AT THE BALL'S OWN ROW, read out of the grid the seated layout
   generator wrote.  Tile id 1 is the solid fill outside the corridor (the
   layout paints 1 outside, a wall pair at each edge and 0 between), so the
   first and last non-1 column bound the drivable span.  This is what turns
   "the ball stopped" into "the ball stopped ON something" or "the ball
   stopped in clear space", which are different findings. */
static int snw_corridor_edge(int want_hi)
{
    if (!g_snw_self) return -1;
    const int row = (*(const int *)(g_snw_self + 0xab3c) >> 12) / 16;
    const int len = *(const int *)(g_snw_self + 0xba08);
    if (row < 0 || row >= (len > 0 && len <= 0x2e0 ? len : 0x2e0)) return -1;
    int found = -1;
    for (int i = 0; i < 16; ++i) {
        const int col = want_hi ? 15 - i : i;
        const unsigned short t =
            ((const unsigned short *)(g_snw_self + col * 0x5c0 + 0x4f38))[row];
        if (t != 1) { found = col; break; }
    }
    return found;
}
static int snw_corridor_lo(void) { return snw_corridor_edge(0); }
static int snw_corridor_hi(void) { return snw_corridor_edge(1); }


extern "C" void *port_mg_snowball_spawn(void)
{
    void *p = MgSnowballSlalom_Spawn();
    g_snw_self = (char *)p;
    /* Read once, at the spawn, so the per-tick sampler is a plain int test and
       an unset variable costs the Behavior face nothing. */
    g_snw_trace = std::getenv("SM64DS_SNW_TRACE") ? 1 : 0;
    return p;
}

extern "C" void port_scene_snowball_hits(void)
{
    unsigned total = 0, mtotal = 0;
    for (int i = 0; i < 36; ++i) {
        total  += g_snw_hits[i];
        mtotal += g_snw_base_hits[i];
    }

    std::printf("[scene] dScMgSnowball_c slot hits: init %u, cleanup %u, "
                "behavior %u, render %u, D2 %u, D0 %u, state-reset %u, "
                "v23 %u, v24 %u, v25 %u%s\n",
                g_snw_hits[0], g_snw_hits[3], g_snw_hits[6], g_snw_hits[9],
                g_snw_hits[16], g_snw_hits[17], g_snw_hits[18],
                g_snw_hits[23], g_snw_hits[24], g_snw_hits[25],
                g_snw_render_skipped
                    ? "  [RENDER SLOT NO-OP'd: SM64DS_SCENE_SLOT9=0]" : "");
    if (g_snw_init_skipped)
        std::printf("[scene] INIT SLOT NO-OP'd: SM64DS_SCENE_SLOT0=0, %u "
                    "time(s)\n", g_snw_init_skipped);

    std::printf("[scene] dScMgSnowball_c own slots entered:");
    for (int i = 0; i < 36; ++i)
        if (g_snw_hits[i]) std::printf(" %d(x%u)", i, g_snw_hits[i]);
    std::printf("   (%u total)\n", total);

    std::printf("[scene] dScMgSingle3DBase_c slots entered (this seat's copy):");
    for (int i = 0; i < 36; ++i)
        if (g_snw_base_hits[i]) std::printf(" %d(x%u)", i, g_snw_base_hits[i]);
    std::printf("   (%u total); this seat's middle-table fill claimed %u "
                "word(s) (0 is EXPECTED: the flower row runs first, "
                "section 6)\n", mtotal, g_snw_mid_claimed);

    /* THE DISPATCH CENSUS IS THE POINT OF THIS SEAT.  A run with slot hits and
       zero routed dispatches has ticked the object without entering its state
       machine, and that reads as a success unless it is said out loud. */
    {
        unsigned calls = 0, unknown = 0;
        int lo = -1, hi = -1;
        const unsigned *slots = 0;
        unsigned nslots = 0;

        port_mg_dispatch_counts(&calls, &unknown);
        port_mg_snowball_sel_range(&lo, &hi);
        port_mg_snowball_slot_hits(&slots, &nslots);

        std::printf("[scene] dScMgSnowball_c state dispatch: %u routed to one "
                    "of the class's 14 table slots (14 distinct bodies), %u "
                    "bodiless want(s); first-level selector range entered "
                    "%d..%d; %u live element(s) on the last Behavior pass; "
                    "%u framework call(s), %u UNHANDLED address(es)\n",
                    port_mg_snowball_state_hits(),
                    port_mg_snowball_floor_hits(), lo, hi,
                    port_mg_snowball_live(), calls, unknown);

        std::printf("[scene] dScMgSnowball_c per-slot:");
        {
            static const char *kName[14] = {
                "38.0", "38.1(DISP)", "38.2(DISP)",
                "50.0", "50.1", "50.2", "50.3",
                "70.0", "70.1", "70.2", "70.3",
                "20.0", "20.1", "20.2",
            };
            for (unsigned i = 0; i < nslots && i < 14; ++i)
                if (slots[i]) std::printf(" %s(x%u)", kName[i], slots[i]);
        }
        std::printf("\n");

        std::printf("[scene] dScMgSnowball_c state floor: NONE. All fourteen "
                    "addresses in the four tables reach a matched src TU, all "
                    "fourteen are distinct, and none carries a NONMATCHING "
                    "banner\n");

        /* THE CLOSURE FLOORS ARE RETIRED (run mg12, lane SNO), and what
           replaces the old call counters is a measurement of their EFFECT
           rather than of their invocation.

           A call counter on a trap answers "was the gap reached".  Once the
           bodies are real that question is worthless -- they are called from
           slots 0, 6 and 18 on every boot, so the counter can only say yes --
           while the question that is actually open is whether the seated
           bodies DO anything.  So this counts what they wrote:

             the course tile grid at +0x4f38, [16][0x2e0] u16 column-major
             with a 0x5c0 byte stride, which func_ov006_02126ee4 fills and
             func_ov006_02126b4c paints into both screens' BG2;
             the snowball table's in-use flags at +0xac58[0x80] and the
             scenery table's at +0xb358[0x80], both seeded by the layout.

           Bounded by the course length the scene actually booted with
           (+0xba08 rows), not by the array's full extent, so a short course
           does not read as a half-empty grid. */
        if (g_snw_self) {
            const int rows = *(const int *)(g_snw_self + 0xba08);
            const int n = rows > 0 && rows <= 0x2e0 ? rows : 0x2e0;
            unsigned tiles = 0, distinct[64] = {0}, kinds = 0;
            for (int col = 0; col < 16; ++col) {
                const unsigned short *g = (const unsigned short *)
                    (g_snw_self + 0x4f38 + col * 0x5c0);
                for (int r = 0; r < n; ++r)
                    if (g[r]) {
                        ++tiles;
                        if (g[r] < 64 && !distinct[g[r]]++) ++kinds;
                    }
            }
            unsigned balls = 0, scen = 0;
            for (int i = 0; i < 0x80; ++i) {
                balls += *(const unsigned char *)(g_snw_self + 0xac58 + i) != 0;
                scen  += *(const unsigned char *)(g_snw_self + 0xb358 + i) != 0;
            }
            std::printf("[scene] dScMgSnowball_c course: %u of %d grid cell(s) "
                        "nonzero over %d row(s) x 16 column(s), %u distinct "
                        "tile id(s); %u snowball slot(s) and %u scenery slot(s) "
                        "in use. The three CLOSURE floors are retired and "
                        "seated (src/func_ov006_02125f68.c, _02126ee4.cpp, "
                        "_02126b4c.c); a zero here would mean the layout ran "
                        "and wrote nothing\n",
                        tiles, n * 16, n, kinds, balls, scen);
        }
    }

    /* THE SCENE PHASE AND THE SCREEN-SWAP FLAG.  Slot 6 dispatches a six-arm
       jump table on the word at +0xb9f4 (0x02128400: ldr r0,[r2,#0x9f4] with
       r2 = this+0xb000, cmp #5, addls pc,pc,r0,lsl #2).  Slot 0 sets the byte
       at +0xb9f8 to 1 and slot 24 branches on it to decide WHICH WAY ROUND THE
       TWO SCREENS ARE: it writes POWCNT1 bit 15 at 0x04000304 in one arm and
       clears it in the other.  Three independent readers of two fields. */
    if (g_snw_self)
        std::printf("[scene] dScMgSnowball_c object at %p, scene phase "
                    "+0xb9f4 = %d, screen-swap flag +0xb9f8 = %u, scroll "
                    "+0xab6c = %d, slot-18 mode last seen %d, save record "
                    "index %d\n",
                    (void *)g_snw_self,
                    *(const int *)(g_snw_self + 0xb9f4),
                    (unsigned)*(const unsigned char *)(g_snw_self + 0xb9f8),
                    *(const int *)(g_snw_self + 0xab6c) >> 12,
                    g_snw_mode18, port_mg_snowball_record_index());

    /* THE ROLL CHAIN, link by link, when SM64DS_SNW_TRACE asked for it. */
    if (g_snw_trace > 0 && g_snw_self)
        std::printf("[scene] dScMgSnowball_c ROLL CHAIN over %u Behavior "
                    "tick(s): %u with a nonzero velocity, peak vx %d peak "
                    "forward vy %d (forward is negative); ball Y ends %d, "
                    "reached a minimum of %d; "
                    "scroll %d -> min %d (delta %d); obstacle-contact byte "
                    "+0xb9e4 set on %u tick(s), hole byte +0xb9e5 on %u; "
                    "PUSHED BACK (Y rose under forward velocity, so the "
                    "obstacle pass moved it out of geometry) on %u tick(s); "
                    "stroke origin +0xab70/+0xab74 = %d/%d; ball X %d, and on "
                    "the ball's own row the corridor runs columns %d..%d "
                    "(tile 1 is solid, so a ball outside that span is against "
                    "course geometry)\n",
                    g_snw_t_ticks, g_snw_t_moving, g_snw_t_vxmax,
                    g_snw_t_vymax,
                    *(const int *)(g_snw_self + 0xab3c) >> 12,
                    g_snw_t_ymin >> 12,
                    g_snw_t_scroll0 >> 12, g_snw_t_scrollmin >> 12,
                    (g_snw_t_scroll0 - g_snw_t_scrollmin) >> 12,
                    g_snw_t_contact, g_snw_t_hole, g_snw_t_back,
                    *(const int *)(g_snw_self + 0xab70),
                    *(const int *)(g_snw_self + 0xab74),
                    *(const int *)(g_snw_self + 0xab38) >> 12,
                    snw_corridor_lo(), snw_corridor_hi());

    /* THE FIFTY ELEMENTS, so "the machine ran" and "the machine did something"
       are different lines.  Each element is 0x24 bytes from +0xbe94: the live
       byte at +0xbeae, the first-level selector at +0xbeaf, and the two
       second-level selectors at +0xbeb0 and +0xbeb1, which is exactly what
       func_ov006_0212a2e0, func_ov006_0212a224 and func_ov006_02129d94 read. */
    if (g_snw_self) {
        unsigned live = 0;
        unsigned sel[4] = {0, 0, 0, 0};
        for (int i = 0; i < 0x32; ++i) {
            const unsigned char *e =
                (const unsigned char *)(g_snw_self + i * 0x24);
            if (e[0xbeae] == 0) continue;
            ++live;
            if (e[0xbeaf] < 4) ++sel[e[0xbeaf]];
        }
        std::printf("[scene] dScMgSnowball_c elements: %u of 50 live; "
                    "first-level selector histogram 0:%u 1:%u 2:%u (>2:%u)\n",
                    live, sel[0], sel[1], sel[2], sel[3]);
    }
}
