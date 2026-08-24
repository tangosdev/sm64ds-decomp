// dScMgJump2_c, the "Bounce and Trounce" minigame -- actor id 0x175, scene 373.
// Run mg11, lane BNT.  The twenty-second ov006 minigame class the port seats,
// and the FIRST under dScMgD3DBase_c.
//
// Read port/slice_bnt.txt for the identity derivation, the width checks, the
// closure and the sibling-attribution ruling.  Read
// port/unmatched/MgJump2_FieldPmf.cpp for the class's own pointer-to-member
// wall and port/unmatched/MgD3DBase_ObjStateDispatch.cpp for the two
// sub-object ones (this lane wrote those two as MgJump2_SubDispatch.cpp; lane
// BNP wrote the same two symbols under the base-named file, and the mg11 merge
// unified them there -- see its section 0).
// This file is the seat: the two new vtables' faces, the fill, the factory
// forwarder and the run census.
//
// ---- 1. WHY THIS IS A SEPARATE FILE FROM hal/scene_mg.cpp -----------------
//
// The reason hal/scene_mg_flower.cpp, hal/scene_mg_memory2.cpp and
// hal/scene_mg_luckystars.cpp give, unchanged.  scene_mg.cpp is the FAMILY
// half -- the mounts, the thirty-five overlay constructors, dScMgBase_c's
// address-keyed faces and the gate that keeps the constructors off a level
// boot -- and all of it is reached through the one seam that file exports,
// port_scene_mg_fill_shared().  Nothing of it is re-derived here and
// kMgBaseFaces is NOT copied: a second copy would install a different set of
// host thunks over the same DS addresses in the same mounted tables and split
// the per-slot witness in two.
//
// ---- 2. THE HIERARCHY IS THREE DEEP, AND THE RTTI SAYS SO ----------------
//
//     Scene -> dScMgBase_c        data_ov004_020bc0c0  36 slots
//           -> dScMgD3DBase_c     data_ov006_0213c62c  36 slots
//           -> dScMgJump2_c       data_ov006_0213ccfc  36 slots
//
// The ROM's own type_info, read the way hal/scene_mg_panel's lane established:
// the word BEFORE a vtable is its type_info, that record's second word is the
// name and its third is the BASE's type_info.
//
//     0x0213ccf8 -> 0x0213cca4 -> +4 -> 0x0213ccbc = "12dScMgJump2_c"
//                                -> +8 -> 0x0213c5c8 -> +4 -> 0x0213c5d4
//                                                       = "14dScMgD3DBase_c"
//
// THE CODE AGREES TWICE OVER.  The factory at 0x020efaf0 writes 0x0213c62c
// into the object's first word and then 0x0213ccfc over it, and slots 16 (D2,
// 0x020eebe8) and 17 (D0, 0x020eec9c) unwind them in the opposite order.  A
// hierarchy written by the constructor and unwritten by the destructor in
// reverse, with an RTTI base link agreeing, is not a guess.  The three load
// relocations that reach 0x0213ccfc in the whole overlay are exactly those
// three pools (from:0x020eec78, from:0x020eed3c, from:0x020efbdc) and there
// are no others.
//
// THE MIDDLE TABLE'S CONFIG NAME IS WRONG AND THE ROM SAYS SO.
// config/arm9/overlays/ov006/symbols.txt calls 0x0213c62c
// `_ZTV17MgBounceAndPounce`, i.e. the vtable of the class 0x174 is named for.
// Its own type_info reads "14dScMgD3DBase_c", and the table is the base of
// FOUR ids -- 0x174, 0x175, 0x180 and 0x181, whose type_info records all link
// to 0x0213c5c8 (relocs at 0x0213cba0, 0x0213ccac, 0x0213fad8, 0x0213fc0c).
// A vtable that four distinct classes inherit is not one of their vtables.
// Nothing here renames anything -- the seat uses the config's spelling, which
// is what the mounts define -- but the name is a config defect and the routing
// note in port/slice_bnt.txt section 9 says so for the decomp side.
//
// ---- 3. THE FILL IS ADDRESS-KEYED, SO TWO TABLES ARE CHEAP ----------------
//
// port_scene_mg_fill_shared and jump2_apply both key on the ROM WORD a slot
// holds, not on the slot number, so the same face array can be applied to
// every table in the chain and can only land where the ROM parked that exact
// body.
//
//   kD3DFaces     the seventeen bodies dScMgD3DBase_c overrides
//                 (slots 1, 2, 5, 7, 10, 11, 16, 17, 24..31, 33)
//   kJump2Faces   the eight this class adds or replaces on top
//                 (slots 0, 3, 6, 9, 16, 17, 18, 19)
//
// Slots 16 and 17 appear in both and hold DIFFERENT addresses -- 0x020e6c28 /
// 0x020e6c60 in the middle table, 0x020eebe8 / 0x020eec9c in this one -- so
// both arrays can be applied to both tables and each writes only its own.
// Fifteen of kD3DFaces' seventeen are what fills the DERIVED table's inherited
// slots, because this class does not override them.
//
// THE MIDDLE TABLE IS SHARED FOUR WAYS AND THREE OF THE FOUR ARE BEING SEATED
// IN THIS SAME WAVE.  This is the dScMgSingle3DBase_c situation with three
// siblings instead of two, and the same rule handles it: all the fills run on
// every boot, and the one that runs FIRST claims the middle table -- the ones
// after it find no DS word left there and write nothing, because jump2_apply
// keys on a DS address and a slot already holding a host thunk matches none.
// Row order decides which, and this class's row is APPENDED AFTER every
// existing row, so on a tree that also carries 0x174's row that row claims the
// middle table and THIS seat's middle-table witness reads ZERO.  It is printed
// rather than assumed, exactly as hal/scene_mg_luckystars.cpp prints its own.
//
// ---- 4. THE WIDTH IS 36 ON BOTH TABLES, AND ONE SPAN CHECK REFUSES -------
//
// port/mg_fanout_costs.txt section 11 measured that twelve of twenty-nine
// widths in its own table were phantoms and that a 37-slot fill writes a host
// thunk over live ROM data belonging to another class.  0x175 IS ONE OF THE
// TWELVE it corrected.  The checks, per table:
//
//   table       span to next symbol      slot 35     word past the end
//   ---------   ----------------------   ---------   ----------------------
//   0213ccfc    0213cd8c, 0x90 = 36      020ad660    0x020f15ac, and the word
//                                                    after it is 0 -- an mwcc
//                                                    {code, adjustment} PAIR
//   0213c62c    0213c67c, 0x50 = 20 (!)  020ad660    0x6f6d6168 = "hamo"...,
//                                                    an ASCII STRING
//
// THE MIDDLE TABLE'S SPAN CHECK REFUSES 36 AND IT IS THE CONFIG THAT IS WRONG,
// which is the hole section 11's own wording names ("or the config is missing
// a symbol") arriving from the other side: the config has an EXTRA symbol,
// data_ov006_0213c67c, INSIDE the table.  The word at that address is
// 0x020b2990, which is dScMgBase_c's own slot-20 body and is therefore a
// vtable slot, not the first word of another object.  port/tools/vtablerows.py
// refuses the width for that reason and this lane did NOT override it blind:
// slot 35 holds 0x020ad660 (the terminal-slot check every dScMgBase_c-derived
// table passes) and the word past slot 35 is a STRING, which is curling's own
// control shape ("/MG/").  Two independent ROM checks agree on 36 and the
// refusal is a config defect, recorded in port/slice_bnt.txt section 4.
//
// THE WORD PAST THE DERIVED TABLE IS A LIVE PAIR.  0x020f15ac followed by a
// zero adjustment is an mwcc member pointer, so a 37-slot fill of THIS table
// would write a host thunk over the code half of somebody else's state --
// section 11's exact hazard.  Both fills below are called with 36.
//
// ---- 5. THE STATE MACHINE IS A FIELD, NOT A TABLE ------------------------
//
// This class holds its member pointer at this+0x5004 and has no state table
// at all, so there is no state INDEX to print.  The analogue the census prints
// instead is the set of code words the field has held and how often each was
// entered -- the progression, which is the thing a state index is wanted for.
// A run with slot hits and one distinct state has ticked the object without
// ever leaving its first state, and that reads as a success unless it is said
// out loud.  unmatched/MgJump2_FieldPmf.cpp section 3 is the derivation.
//
// ---- 6. THE ROW GOES LAST, AND THE ORDER IS LOAD-BEARING ------------------
//
// port_scene_registry_install walks port_scene_classes[] in table order and
// calls every row's fill on every boot; port_scene_mg_overlay_load runs the
// thirty-five constructors ONCE PER PROCESS from the tail of the first
// minigame row's fill.  Appending after every existing row therefore means the
// constructors have already run with clean ROM words when this fill starts,
// which is the latent-safe direction section 11 names -- and section 3 above
// depends on it for a second reason.
//
// ---- 7. WHAT THIS SEAT DOES NOT CLAIM -------------------------------------
//
// That the game is playable.  The census below prints the numbers whether they
// are zero or not, and a reader should believe the numbers and nothing else.

#include "hal/screen_gap.h"

#include "MgBounceAndPounce.h"

#include <cstdio>
#include <cstdlib>

extern "C" {

/* the seam into hal/scene_mg.cpp -- mounts, arm9 fill, dScMgBase_c's faces */
unsigned port_scene_mg_fill_shared(void **vt, unsigned n);
void     port_scene_mg_prepare(int id);
int      port_scene_env_want(void);
unsigned port_mg_scene_spawn_param(int scene_id);

/* the mount storage the fill writes into.  All three are the mounts' own, so
   the fill replaces the mounted words rather than building a fresh host array
   -- the ov007, curling and memory2 treatment, for the same reason: a second
   host array of the same name is a duplicate symbol, and leaving the mounted
   table alone leaves live wild DS pointers in a table the factory installs. */
extern unsigned char data_ov004_020bc0c0[];        /* dScMgBase_c,     36 */
extern unsigned char _ZTV17MgBounceAndPounce[];    /* dScMgD3DBase_c,  36 */
extern unsigned char data_ov006_0213ccfc[];        /* dScMgJump2_c,    36 */
extern unsigned char MgBounceAndTrounce_SpawnInfo[];

/* dScMgD3DBase_c's seventeen overrides.  Four of them are REAL C++ METHODS in
   src (MgBounceAndPounce::BeforeInitResources, ::AfterCleanupResources,
   ::BeforeBehavior and ::AfterRender), so they are called through the class
   below rather than declared here -- a C-linkage face for them would be a
   symbol three other lanes in this wave also define. */
int   _ZN17MgBounceAndPounceD1Ev(void *self);                     /* slot 16 */
int   _ZN17MgBounceAndPounceD0Ev(void *self);                     /* slot 17 */
int   func_ov006_020e6e78(void *self);                            /* slot 24 */
int   func_ov006_020e6e54(void *t);                               /* slot 25 */
int   func_ov006_020e6e4c(void);                                  /* slot 26 */

/* FOUR OF THE SEVENTEEN ARE HOST COPIES, not the src TUs: slots 2, 10, 27 and
   28 each drop an argument the ROM rides through in a register, and
   unmatched/MgD3DBase_Slots.cpp is the repair. Its header is the reading; the
   short version is that slot 2's dropped RETURN is what put this class's actor
   on the cleanup list instead of the behaviour list on this lane's first run.
   The four src TUs stay out of port/slice_bnt.txt for the reason
   src/func_ov006_0210a6e4.cpp stays out of slice_flw.txt: nothing should claim
   a decompilation the build does not use. */
int   port_mg_d3dbase_after_init(void *c, unsigned int flags);    /* slot  2 */
int   port_mg_d3dbase_before_render(void *c);                     /* slot 10 */
void  port_mg_d3dbase_slot27(void *c);                            /* slot 27 */
void  port_mg_d3dbase_slot28(void *c);                            /* slot 28 */
void  func_ov006_020e6d24(char *c);                               /* slot 29 */
void  func_ov006_020e6cac(char *c);                               /* slot 30 */
void  func_ov006_020e72c0(char *c);                               /* slot 31 */
void  func_ov006_020e7124(char *obj);                             /* slot 33 */

/* dScMgJump2_c's own eight.  func_ov006_020ef3e0 is the HOST COPY in
   unmatched/MgJump2_FieldPmf.cpp, not the src TU: it is the pointer-to-member
   dispatcher and the port cannot compile the src.

   SLOTS 18 AND 19 TAKE A SECOND ARGUMENT AND THIS CLASS IGNORES IT, which was
   checked against the ROM rather than assumed after run mg9 lane LKY found a
   class that reads it.  0x020efaa8 does `push {r4,lr} / mov r4,r0` and then
   `mov r0,#0 / mov r1,r0` at 0x020efab4 -- r1 is WRITTEN before it is ever
   read -- and 0x020efa84 does the same at 0x020efa8c.  So the parameter exists
   here only so __fastcall cleans four bytes, and the thunks do not forward it.
   The value is recorded for the census so the claim is measured. */
int   func_ov006_020ef834(char *self);      /* slot  0 InitResources */
int   func_ov006_020ef110(void);            /* slot  3 CleanupResources */
int   func_ov006_020ef3e0(char *self);      /* slot  6 Behavior, host copy */
int   func_ov006_020ef148(char *self);      /* slot  9 Render */
void *func_ov006_020eebe8(char *c);         /* slot 16 D2 */
void *func_ov006_020eec9c(char *c);         /* slot 17 D0 */
void  func_ov006_020efaa8(char *c);         /* slot 18 state reset */
int   func_ov006_020efa84(void);            /* slot 19 */

/* THE ELEMENT VTABLE, run mg9 lane S381's headline arriving in this family.
   func_ov006_020c8a04 -- the constructor the factory runs over the THREE
   0xb8-byte records at this+0x500c -- writes data_ov006_0213b0cc into each
   record's word 0.  That is a THREE-SLOT vtable of its own, mounted ROM data,
   and nothing in the port filled it: a dispatch through slot 2 jumped to the
   raw DS address 0x020c762c and the fault probe caught it
   (FAULT c0000005 at +0x01cc762c accessing 020c762c, walker node 307FA5B0
   actor 307FA588 id 0x175).  All three bodies have matched src TUs. */
int  func_ov006_020c76d8(int p);      /* element slot 0, returns p + 20 */
int  func_ov006_020c76d0(int p);      /* element slot 1, returns p + 32 */
int  func_ov006_020c762c(char *c);    /* element slot 2, the state selector */
extern unsigned char data_ov006_0213b0cc[];   /* the element table, 3 slots */

/* the factory.  The config names 0x020efaf0 _ZN8PathLift17BaseInitResourcesEv,
   which is ov002's name for the same address in a DIFFERENT overlay; the body
   in ov006 is this class's factory, and section 8 of port/slice_bnt.txt is the
   disassembly that says so. */
void *_ZN8PathLift17BaseInitResourcesEv(void);

/* the dispatch files' witnesses */
void     port_mg_jump2_field_counts(unsigned *calls, unsigned *routed,
                                    unsigned *distinct, unsigned *last);
unsigned port_mg_jump2_field_row(unsigned i, unsigned *code);
void     port_mg_jump2_sub_counts(int which, unsigned *calls, unsigned *routed,
                                  unsigned *unknown, unsigned *distinct);
unsigned port_mg_jump2_sub_row(int which, unsigned i, unsigned *code);
/* THE CENSUS PRINTS ITS OWN CAPACITY, because a distinct-state count that has
   saturated reads exactly like one that has not.  Both tables used to hold
   EIGHT, which is the +0x30 machine's own observed count, so every "8 distinct"
   printed before the lane review was AT the cap and nothing said so. */
unsigned port_mg_jump2_sub_dropped(int which);
unsigned port_mg_jump2_sub_capacity(void);
unsigned port_mg_jump2_field_dropped(void);
unsigned port_mg_jump2_field_capacity(void);
/* the framework's, from unmatched/MgBase_StateDispatch.cpp */
void     port_mg_dispatch_counts(unsigned *calls, unsigned *unknown);

/* the persistent minigame record this class's level is keyed to.  Nothing new
   is defined here: the storage is hal/level_boot.cpp's .dsstate$savblk0004 and
   the index function is src/func_ov004_020adc3c.c. */
extern void         *data_ov004_020beb68;     /* the live dScMgBase_c `this` */
extern unsigned char data_0209caf4[];         /* 36 records x 20 bytes       */
int  func_ov004_020adc3c(void *c);            /* (self->field_8 >> 8) & 0xff */

}  /* extern "C" */

/* The record row this class's object is keyed to, or -1 with no live base. */
static int port_mg_jump2_record_index(void)
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
static unsigned g_j2_hits[36];
static unsigned g_j2_base_hits[36];   /* the same slots on the MIDDLE table */

#define J2(n)    (++g_j2_hits[(n)])
#define D3B(n)   (++g_j2_base_hits[(n)])

/* The two values slots 18 and 19 were last handed, kept because the block
   above says the argument is IGNORED here and a census that does not print it
   cannot show that the claim was measured rather than assumed. */
static int g_j2_mode18 = -1, g_j2_mode19 = -1;

/* ---- dScMgD3DBase_c's seventeen ----------------------------------------- */
static int  __fastcall d3_binit(void *s, void *)
{ D3B(1);  return ((MgBounceAndPounce *)s)->MgBounceAndPounce::BeforeInitResources(); }
/* SLOT 2 RETURNS A VALUE THE FRAMEWORK READS, and the flag it is handed is
   read too. Both were dropped in this lane's first build and the actor went
   straight to the cleanup list; see unmatched/MgD3DBase_Slots.cpp. */
static int __fastcall d3_ainit(void *s, void *, unsigned f)
{ D3B(2);  return port_mg_d3dbase_after_init(s, f); }
static void __fastcall d3_aclean(void *s, void *, unsigned f)
{ D3B(5);  ((MgBounceAndPounce *)s)->MgBounceAndPounce::AfterCleanupResources(f); }
static int  __fastcall d3_bbeh(void *s, void *)
{ D3B(7);  return ((MgBounceAndPounce *)s)->MgBounceAndPounce::BeforeBehavior(); }
static int  __fastcall d3_bren(void *s, void *)
{ D3B(10); return port_mg_d3dbase_before_render(s); }
static void __fastcall d3_aren(void *s, void *, unsigned f)
{ D3B(11); ((MgBounceAndPounce *)s)->MgBounceAndPounce::AfterRender(f); }
static void *__fastcall d3_d2(void *s, void *)
{ D3B(16); return (void *)(size_t)_ZN17MgBounceAndPounceD1Ev(s); }
static void *__fastcall d3_d0(void *s, void *)
{ D3B(17); return (void *)(size_t)_ZN17MgBounceAndPounceD0Ev(s); }
static int  __fastcall d3_v24(void *s, void *)
{ D3B(24); return func_ov006_020e6e78(s); }
static int  __fastcall d3_v25(void *s, void *)
{ D3B(25); return func_ov006_020e6e54(s); }
static int  __fastcall d3_v26(void *, void *)
{ D3B(26); return func_ov006_020e6e4c(); }
static int  __fastcall d3_v27(void *s, void *)
{ D3B(27); port_mg_d3dbase_slot27(s); return 0; }
static int  __fastcall d3_v28(void *s, void *)
{ D3B(28); port_mg_d3dbase_slot28(s); return 0; }
static int  __fastcall d3_v29(void *s, void *)
{ D3B(29); func_ov006_020e6d24((char *)s); return 0; }
static int  __fastcall d3_v30(void *s, void *)
{ D3B(30); func_ov006_020e6cac((char *)s); return 0; }
static int  __fastcall d3_v31(void *s, void *)
{ D3B(31); func_ov006_020e72c0((char *)s); return 0; }
static int  __fastcall d3_v33(void *s, void *)
{ D3B(33); func_ov006_020e7124((char *)s); return 0; }

/* ---- dScMgJump2_c's own eight ------------------------------------------- */
static int  __fastcall j2_init(void *s, void *)
{ J2(0);  const int r = func_ov006_020ef834((char *)s);
  /* the GaplessMinigames latch, for hal/scene_mg.cpp's reason: every seated
     minigame calls it so the ones the gapless table does not name can say
     "unsupported" instead of doing nothing quietly.  hal_gapless_splice() is
     deliberately NOT called -- scene 373 has no row in screen_gap.cpp's table
     and gapless behaviour is decided per minigame BY PLAYING IT.  This lane
     has not played it and does not get to vote. */
  hal_gapless_minigames_latch(); return r; }
static int  __fastcall j2_clean(void *, void *)
{ J2(3);  return func_ov006_020ef110(); }
static int  __fastcall j2_beh(void *s, void *)
{ J2(6);  return func_ov006_020ef3e0((char *)s); }
static int  __fastcall j2_render(void *s, void *)
{ J2(9);  return func_ov006_020ef148((char *)s); }
static void *__fastcall j2_d2(void *s, void *)
{ J2(16); return func_ov006_020eebe8((char *)s); }
static void *__fastcall j2_d0(void *s, void *)
{ J2(17); return func_ov006_020eec9c((char *)s); }
static int  __fastcall j2_reset(void *s, void *, int mode)
{ J2(18); g_j2_mode18 = mode; func_ov006_020efaa8((char *)s); return 1; }
static int  __fastcall j2_v19(void *, void *, int mode)
{ J2(19); g_j2_mode19 = mode; return func_ov006_020efa84(); }

/* SM64DS_SCENE_SLOT0=0 and SM64DS_SCENE_SLOT9=0, the diagnostics every scene
   seat in this port carries, counted separately so a run can never read a
   no-op as the real body having run. */
static unsigned g_j2_init_skipped, g_j2_render_skipped;
static int __fastcall j2_init_noop(void *, void *)
{ ++g_j2_init_skipped; return 1; }
static int __fastcall j2_render_noop(void *, void *)
{ ++g_j2_render_skipped; return 1; }

/* ---- the element vtable's three ----------------------------------------
   The records are plain C++ objects the ROM dispatches through word 0, so the
   thunks are __fastcall for the same reason every vtable thunk in this port is:
   `this` arrives in ECX where the ROM put it in r0. */
static unsigned g_j2_elem_hits[3];

static int __fastcall el_v0(void *s, void *)
{ ++g_j2_elem_hits[0]; return func_ov006_020c76d8((int)(size_t)s); }
static int __fastcall el_v1(void *s, void *)
{ ++g_j2_elem_hits[1]; return func_ov006_020c76d0((int)(size_t)s); }
static int __fastcall el_v2(void *s, void *)
{ ++g_j2_elem_hits[2]; return func_ov006_020c762c((char *)s); }

struct J2Face { unsigned ds; void *host; };

/* Keyed on the ROM WORD each slot holds, so the array is order-independent and
   cannot land on a slot the ROM did not park that body in. */
static const J2Face kD3DFaces[] = {
    {0x020e70e4u, (void *)d3_binit},  {0x020e70c0u, (void *)d3_ainit},
    {0x020e6f60u, (void *)d3_aclean}, {0x020e7074u, (void *)d3_bbeh},
    {0x020e7040u, (void *)d3_bren},   {0x020e700cu, (void *)d3_aren},
    {0x020e6c28u, (void *)d3_d2},     {0x020e6c60u, (void *)d3_d0},
    {0x020e6e78u, (void *)d3_v24},    {0x020e6e54u, (void *)d3_v25},
    {0x020e6e4cu, (void *)d3_v26},    {0x020e6d98u, (void *)d3_v27},
    {0x020e6d8cu, (void *)d3_v28},    {0x020e6d24u, (void *)d3_v29},
    {0x020e6cacu, (void *)d3_v30},    {0x020e72c0u, (void *)d3_v31},
    {0x020e7124u, (void *)d3_v33},
};

static const J2Face kElemFaces[] = {
    {0x020c76d8u, (void *)el_v0}, {0x020c76d0u, (void *)el_v1},
    {0x020c762cu, (void *)el_v2},
};

static const J2Face kJump2Faces[] = {
    {0x020ef834u, (void *)j2_init},   {0x020ef110u, (void *)j2_clean},
    {0x020ef3e0u, (void *)j2_beh},    {0x020ef148u, (void *)j2_render},
    {0x020eebe8u, (void *)j2_d2},     {0x020eec9cu, (void *)j2_d0},
    {0x020efaa8u, (void *)j2_reset},  {0x020efa84u, (void *)j2_v19},
};

static unsigned jump2_apply(void **vt, unsigned n, const J2Face *f, unsigned nf)
{
    unsigned hit = 0;
    for (unsigned i = 0; i < n; ++i) {
        const unsigned ds = (unsigned)(size_t)vt[i];
        for (unsigned k = 0; k < nf; ++k)
            if (f[k].ds == ds) { vt[i] = f[k].host; ++hit; break; }
    }
    return hit;
}

/* Words still holding a DS address.  A minigame table is 36 slots and every
   one of them is dispatched by something, so a nonzero answer is a wild call
   waiting to happen and the seat says so out loud. */
static unsigned jump2_raw_left(void **vt, unsigned n)
{
    unsigned left = 0;
    for (unsigned i = 0; i < n; ++i) {
        const unsigned w = (unsigned)(size_t)vt[i];
        if (w >= 0x02000000u && w < 0x02400000u)
            ++left;
    }
    return left;
}

extern "C" void port_scene_jump2_hits(void);

/* How many words this seat's own copy of the middle-table array claimed.  Zero
   is the EXPECTED reading on a tree that also carries 0x174's row, and section
   3 is why; it is printed rather than assumed. */
static unsigned g_j2_mid_claimed;
static unsigned g_j2_elem_claimed;

extern "C" void port_scene_fill_jump2(void)
{
    void **base = (void **)data_ov004_020bc0c0;
    void **mid  = (void **)_ZTV17MgBounceAndPounce;
    void **vt   = (void **)data_ov006_0213ccfc;

    /* THE BASE TABLE IS FILLED HERE TOO AND IT IS NOT CEREMONY.  Earlier rows'
       fills already did it and run first, so on a tree carrying them this is a
       second pass over words that are already host pointers and finds nothing.
       It is here so this class does not depend on another class's row
       existing: the factory's second act is func_ov004_020b2adc, which writes
       data_ov004_020bc0c0 into the object's first word before either derived
       table lands. */
    port_scene_mg_fill_shared(base, 36);

    /* THE MIDDLE TABLE.  Live storage in its own right -- the destructor chain
       writes it into the object on the way down, so a slot dispatched between
       the two vptr writes reads it.  Section 3 is why applying this file's own
       arrays here is safe next to the three sibling classes'. */
    port_scene_mg_fill_shared(mid, 36);
    g_j2_mid_claimed =
        jump2_apply(mid, 36, kD3DFaces, sizeof kD3DFaces / sizeof kD3DFaces[0]);
    g_j2_mid_claimed +=
        jump2_apply(mid, 36, kJump2Faces,
                    sizeof kJump2Faces / sizeof kJump2Faces[0]);

    /* THE DERIVED TABLE.  Shared arm9 and dScMgBase_c words first, then the
       intermediate base's seventeen, then this class's own eight.  Order does
       not matter -- the three key sets are disjoint, because a word is one
       address -- but it reads in inheritance order.  THIRTY-SIX, not
       thirty-seven: section 4, and the word a thirty-seventh would take is a
       live mwcc member-pointer pair. */
    port_scene_mg_fill_shared(vt, 36);
    jump2_apply(vt, 36, kD3DFaces, sizeof kD3DFaces / sizeof kD3DFaces[0]);
    jump2_apply(vt, 36, kJump2Faces,
                sizeof kJump2Faces / sizeof kJump2Faces[0]);

    /* THE ELEMENT VTABLE.  Three slots, address-keyed like every other fill
       here, so it is idempotent and cannot land on a slot the ROM did not park
       that body in.  0x174's factory ALSO runs func_ov006_020c8a04 over its own
       +0x500c array (relocs from:0x020eebd8 and from:0x020efbe8 are the only
       two that reach the constructor), so this table is shared with lane BNP
       and the fill that runs first claims it -- the same rule as the middle
       vtable, and the count is printed for the same reason. */
    g_j2_elem_claimed = jump2_apply((void **)data_ov006_0213b0cc, 3, kElemFaces,
                                    sizeof kElemFaces / sizeof kElemFaces[0]);

    /* the two diagnostics, applied after the fill so they override it */
    {
        const char *s0 = std::getenv("SM64DS_SCENE_SLOT0");
        const char *s9 = std::getenv("SM64DS_SCENE_SLOT9");
        if (s0 && s0[0] == '0') vt[0] = (void *)j2_init_noop;
        if (s9 && s9[0] == '0') vt[9] = (void *)j2_render_noop;
    }

    {
        const unsigned lb = jump2_raw_left(base, 36);
        const unsigned lm = jump2_raw_left(mid, 36);
        const unsigned lv = jump2_raw_left(vt, 36);
        if (lb || lm || lv) {
            std::fprintf(stderr, "  [scene] JUMP2 FILL INCOMPLETE: "
                         "dScMgBase_c leaves %u of 36 raw DS words, "
                         "dScMgD3DBase_c %u, dScMgJump2_c %u. A dispatch of "
                         "any of them jumps to a DS address as a host one.\n",
                         lb, lm, lv);
            std::fflush(stderr);
        }
    }

    /* The constructors, gated on the requested id, exactly as every other
       minigame fill does it.  Idempotent behind scene_mg.cpp's own
       once-per-process guard. */
    port_scene_mg_prepare(port_scene_env_want());

    if (port_scene_env_want() == 373) {
        static int armed;
        if (!armed) {
            armed = 1;
            std::atexit(port_scene_jump2_hits);
        }
    }
}

/* The registry's factory column is void *(*)(void) and so is the matched
   factory; the forwarder is kept for the reason title_spawn and
   port_mg_curling_spawn are, so the seat has one place to observe the object
   without the registry table growing a second column.

   THE FACTORY NEEDS NO DISPLACEMENT RULING, re-checked rather than assumed.
   port/mg_fanout_costs.txt section 12 grants one to 0x169 because
   src/func_ov006_020e0574.cpp calls the base constructor func_ov004_020b2adc
   with NO argument and rides r0 through.  The ROM body here does
   `movs r4,r0 / beq / bl 0x020b2adc` at 0x020efb00..0x020efb08 with r0 STILL
   holding the allocation, and src/_ZN8PathLift17BaseInitResourcesEv.c spells
   func_ov004_020b2adc(p) WITH its argument, so this class is on the correct
   side of the ruling and the factory is linked from the slice rather than
   host-copied. */
static char *g_j2_self;

extern "C" void *port_mg_jump2_spawn(void)
{
    void *p = _ZN8PathLift17BaseInitResourcesEv();
    g_j2_self = (char *)p;
    return p;
}

extern "C" void port_scene_jump2_hits(void)
{
    unsigned total = 0, mtotal = 0;
    for (int i = 0; i < 36; ++i) { total += g_j2_hits[i]; mtotal += g_j2_base_hits[i]; }

    std::printf("[scene] dScMgJump2_c slot hits: init %u, cleanup %u, "
                "behavior %u, render %u, D2 %u, D0 %u, state-reset %u, "
                "slot19 %u%s\n",
                g_j2_hits[0], g_j2_hits[3], g_j2_hits[6], g_j2_hits[9],
                g_j2_hits[16], g_j2_hits[17], g_j2_hits[18], g_j2_hits[19],
                g_j2_render_skipped
                    ? "  [RENDER SLOT NO-OP'd: SM64DS_SCENE_SLOT9=0]" : "");
    if (g_j2_init_skipped)
        std::printf("[scene] INIT SLOT NO-OP'd: SM64DS_SCENE_SLOT0=0, %u "
                    "time(s)\n", g_j2_init_skipped);

    std::printf("[scene] dScMgJump2_c own slots entered:");
    for (int i = 0; i < 36; ++i)
        if (g_j2_hits[i]) std::printf(" %d(x%u)", i, g_j2_hits[i]);
    std::printf("   (%u total)\n", total);

    std::printf("[scene] dScMgD3DBase_c slots entered (this seat's copy):");
    for (int i = 0; i < 36; ++i)
        if (g_j2_base_hits[i]) std::printf(" %d(x%u)", i, g_j2_base_hits[i]);
    std::printf("   (%u total); this seat's middle-table fill claimed %u "
                "word(s) (0 is EXPECTED once 0x174's row exists, section 3)\n",
                mtotal, g_j2_mid_claimed);

    std::printf("[scene] dScMgJump2_c element vtable data_ov006_0213b0cc "
                "(3 slots, on each of the three 0xb8 records at +0x500c): this "
                "seat's fill claimed %u word(s); slots entered 0(x%u) 1(x%u) "
                "2(x%u)\n", g_j2_elem_claimed, g_j2_elem_hits[0],
                g_j2_elem_hits[1], g_j2_elem_hits[2]);

    /* THE STATE MACHINE'S OWN WITNESS.  Printed whether or not it fired, and
       with the PROGRESSION rather than a bare count: this class has no state
       index, so the set of distinct code words the field has held is what a
       state-index range would have said elsewhere. */
    {
        unsigned calls = 0, routed = 0, distinct = 0, last = 0;
        unsigned fcalls = 0, funknown = 0;
        port_mg_jump2_field_counts(&calls, &routed, &distinct, &last);
        port_mg_dispatch_counts(&fcalls, &funknown);
        std::printf("[scene] dScMgJump2_c state dispatch (field +0x5004): %u "
                    "call(s), %u routed to one of the class's 5 states, %u "
                    "DISTINCT state(s) entered, last code %08x; %u framework "
                    "call(s), %u UNHANDLED address(es)\n",
                    calls, routed, distinct, last, fcalls, funknown);
        std::printf("[scene] dScMgJump2_c state progression:");
        for (unsigned i = 0; i < distinct; ++i) {
            unsigned code = 0;
            const unsigned n = port_mg_jump2_field_row(i, &code);
            std::printf(" %08x(x%u)", code, n);
        }
        if (!distinct) std::printf(" NONE");
        std::printf("   [%u of 5 possible; table holds %u, %u dropped]\n",
                    distinct, port_mg_jump2_field_capacity(),
                    port_mg_jump2_field_dropped());
        std::printf("[scene] dScMgJump2_c state floor: NONE. All five code "
                    "words reachable through data_ov006_0213cc74/84/8c/94/9c "
                    "have a matched src TU and none carries a NONMATCHING "
                    "banner\n");
    }

    /* The two SUB-OBJECT machines, printed the same way and for the same
       reason: they are the per-frame ticks of the objects the factory builds
       inside the scene, and a run that never enters them has drawn a scene
       whose contents never moved. */
    for (int w = 0; w < 2; ++w) {
        unsigned calls = 0, routed = 0, unknown = 0, distinct = 0;
        port_mg_jump2_sub_counts(w, &calls, &routed, &unknown, &distinct);
        std::printf("[scene] dScMgJump2_c sub-object %s (field +0x%x): %u "
                    "call(s), %u routed, %u to the framework, %u distinct:",
                    w ? "B, func_ov006_020c7860" : "A, func_ov006_020c4cd8",
                    w ? 0x3c : 0x30, calls, routed, unknown, distinct);
        for (unsigned i = 0; i < distinct; ++i) {
            unsigned code = 0;
            const unsigned n = port_mg_jump2_sub_row(w, i, &code);
            std::printf(" %08x(x%u)", code, n);
        }
        if (!distinct) std::printf(" NONE");
        std::printf("   [%u of %d possible; table holds %u, %u dropped]\n",
                    distinct, w ? 9 : 16, port_mg_jump2_sub_capacity(),
                    port_mg_jump2_sub_dropped(w));
    }

    /* THE SCREEN THE 3D IS ON.  The halfword at +0x4664 is this family's
       screen selector: dScMgD3DBase_c slot 24 (func_ov006_020e6e78) TOGGLES it
       and then re-points the camera at self+0x466c + it*0xbc, and this class's
       Render (src/func_ov006_020ef148.c) gates its whole OAM block on it being
       1.  Two independent readers of one field, so printing it says which
       screen the run was drawing to rather than leaving it inferred. */
    if (g_j2_self)
        std::printf("[scene] dScMgJump2_c object at %p (0x5a78 bytes), screen "
                    "selector +0x4664 = %u, camera block +0x466c stride 0xbc; "
                    "slot-18 arg last seen %d, slot-19 arg last seen %d (both "
                    "are IGNORED by the ROM bodies -- see the header)\n",
                    (void *)g_j2_self,
                    *(const unsigned short *)(g_j2_self + 0x4664),
                    g_j2_mode18, g_j2_mode19);

    /* ---- THE ov005 ROW, AND THE RECORD IT KEYS ---------------------------
       port/mg_fanout_costs.txt section 17 requires a lane to VERIFY that its
       scene's spawn-param row resolves rather than assume the plumbing feeds
       it.  Scene 373 sits on ov005 row 20 with param 0x001c1c01: save record
       28, name text 28.  0x175 appears EXACTLY ONCE in the 36-row table, so
       the first-match rule has nothing to choose between here. */
    {
        const unsigned param = port_mg_scene_spawn_param(373);
        const int idx = port_mg_jump2_record_index();
        std::printf("[scene] dScMgJump2_c spawn param %08x -> save record %u, "
                    "name text %u, slot-35 byte %u; live record index %d\n",
                    param, (param >> 8) & 0xffu, (param >> 16) & 0xffu,
                    param & 0xffu, idx);
        if (g_j2_self)
            std::printf("[scene] dScMgJump2_c +0x465e name text = %d "
                        "(data_ov004_020bc070[28] = 576, read from the table "
                        "and NOT computed: indices 24..35 are an explicit "
                        "list)\n",
                        (int)*(const short *)(g_j2_self + 0x465e));
        if (idx >= 0 && idx < 36) {
            const int *rec = (const int *)(data_0209caf4 + idx * 20);
            std::printf("[scene] dScMgJump2_c save record data_0209caf4[%d] "
                        "= { %d, %d, %d, %d, %d }\n", idx,
                        rec[0], rec[1], rec[2], rec[3], rec[4]);
        } else {
            std::printf("[scene] dScMgJump2_c save record: NO minigame index "
                        "(data_ov004_020beb68 = %p)\n", data_ov004_020beb68);
        }
    }

    /* The SpawnInfo cross-check every seat prints: word 0 is the factory and
       word 1 is the id twice.  Read out of the mount at run time, so a mount
       that stopped rebasing says so here rather than in a fault. */
    std::printf("[scene] dScMgJump2_c SpawnInfo MgBounceAndTrounce_SpawnInfo "
                "word1 = %08x (the ROM's 0x01750175), factory word = %p\n",
                *(const unsigned *)(MgBounceAndTrounce_SpawnInfo + 4),
                *(void *const *)MgBounceAndTrounce_SpawnInfo);
    std::fflush(stdout);
}
