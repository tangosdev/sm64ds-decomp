// dScMgTrampoline2_c, the "Trampoline Terror" minigame -- actor id 0x181,
// scene 385.  Run mg11, lane TTE.  The twenty-second ov006 minigame class the
// port seats and the FIRST under dScMgD3DBase_c.
//
// Read port/slice_tte.txt for the identity derivation, the five width checks
// and the closure.  Read port/unmatched/MgTrampolineTerror_StateDispatch.cpp
// for the wall.  This file is the seat: the two vtables' faces, the fill, the
// factory forwarder and the run census.
//
// ---- 1. WHY THIS IS A SEPARATE FILE FROM hal/scene_mg.cpp -----------------
//
// The reason hal/scene_mg_flower.cpp and hal/scene_mg_luckystars.cpp give,
// unchanged.  scene_mg.cpp is the FAMILY half -- the mounts, the thirty-five
// overlay constructors, dScMgBase_c's address-keyed faces and the gate that
// keeps the constructors off a level boot -- and all of it is reached through
// the one seam that file exports, port_scene_mg_fill_shared().  Nothing of it
// is re-derived here and kMgBaseFaces is NOT copied: a second copy would
// install a different set of host thunks over the same DS addresses in the same
// mounted tables and split the per-slot witness in two.
//
// ---- 2. THE HIERARCHY IS THREE DEEP, AND THE ROM'S OWN RTTI SAYS SO -------
//
//     Scene -> dScMgBase_c      data_ov004_020bc0c0  36 slots
//           -> dScMgD3DBase_c   0x0213c62c           36 slots  (17 overrides)
//           -> dScMgTrampoline2_c data_ov006_0213fc7c 36 slots (12 more)
//
// The word immediately before a vtable is its type_info, that record's second
// word is the name pointer and its third is the BASE's type_info.  Read out of
// extracted/overlays/overlay_0006.bin at base 0x020bfec0:
//
//     0x0213fc78 -> 0x0213fc04 -> +4 -> 0x0213fc5c = "18dScMgTrampoline2_c"
//                                -> +8 -> 0x0213c5c8 -> "14dScMgD3DBase_c"
//
// and the code agrees twice over: src/MgTrampolineTerror_Spawn.c writes
// _ZTV17MgBounceAndPounce into the object and then data_ov006_0213fc7c over it,
// while slots 16 and 17 (src/func_ov006_021225ac.c, src/func_ov006_021226b0.cpp)
// unwind them in the opposite order.
//
// THE MIDDLE TABLE'S SYMBOL IS A SPAWN NAME AND NOT THE CLASS NAME.
// config/arm9/overlays/ov006/symbols.txt calls 0x0213c62c
// _ZTV17MgBounceAndPounce, which is actor 0x174's spawn symbol; the ROM's RTTI
// at 0x0213c5c8 calls the class dScMgD3DBase_c.  Four classes hold this table:
//
//     0x174  dScMgJump_c        vtable 0x0213cbe4     "Bounce and Pounce"
//     0x175  dScMgJump2_c       vtable 0x0213ccfc     "Bounce and Trounce"
//     0x180  dScMgTrampoline_c  vtable 0x0213fb34     "Trampoline Time"
//     0x181  dScMgTrampoline2_c vtable 0x0213fc7c     "Trampoline Terror"
//
// so the naming pass's per-body "recovered name" comments -- which call every
// one of the thirteen shared bodies dScMgTrampoline2_c_Something -- are naming
// artifacts, not attribution.  This file names the array kD3DBaseFaces after
// the class the ROM names, and the symbol after the symbol the tree has.
//
// ---- 3. THE FILL IS ADDRESS-KEYED, SO TWO TABLES ARE CHEAP ----------------
//
// port_scene_fill_rom and tte_apply both key on the ROM WORD a slot holds, not
// on the slot number, so the same face array can be applied to every table in
// the chain and can only land where the ROM parked that exact body.
//
//   kD3DBaseFaces  the seventeen bodies dScMgD3DBase_c overrides
//                  (slots 1, 2, 5, 7, 10, 11, 16, 17, 24, 25, 26..31, 33)
//   kTteFaces      the twelve this class adds or replaces on top
//                  (slots 0, 3, 6, 9, 16, 17, 18, 19, 23, 24, 25, 34)
//
// Slots 16, 17, 24 and 25 appear in both and hold DIFFERENT addresses -- the
// middle table has 0x020e6c28 / 0x020e6c60 / 0x020e6e78 / 0x020e6e54 where this
// one has 0x021225ac / 0x021226b0 / 0x021230e8 / 0x021230c4 -- so both arrays
// can be applied to both tables and each writes only its own.  Thirteen of
// kD3DBaseFaces' seventeen are what fills slots 1, 2, 5, 7, 10, 11, 26..31 and
// 33 of the DERIVED table, because this class does not override them.
//
// THE MIDDLE TABLE IS SHARED FOUR WAYS AND THE ROW ORDER DECIDES WHO CLAIMS IT.
// This is the dScMgSingle3DBase_c situation one family over
// (hal/scene_mg_luckystars.cpp section 3), with the difference that on THIS
// tree no other class under dScMgD3DBase_c is seated yet, so this seat's own
// middle-table claim reads SEVENTEEN rather than zero.  Run mg11 seats 0x174,
// 0x175 and 0x180 concurrently in their own worktrees and the merge lands them
// in id order, so on the merged tree the 0x174 row runs first, claims
// 0x0213c62c, and this seat's middle claim drops to 0 while its derived table
// is unchanged.  BOTH numbers are printed by the census below rather than
// assumed, and a reviewer on a single-lane tree should expect 17.
//
// ---- 4. SLOT 2 IS NOT src's BODY, AND THE DEFECT IS THE FLOWER'S ----------
//
// dScMgD3DBase_c's AfterInitResources is 0x020e70c0, and the ROM is nine
// instructions with r1 NEVER WRITTEN before its `bl 0x20b08f0`:
//
//     020e70c0  push {r4,lr}
//     020e70c4  mov  r4,r0
//     020e70c8  bl   0x20b08f0        <- r1 rides through untouched
//     020e70cc  ldr  r0,[pc,#0xc]     ; = 0x000047e4
//     020e70d0  add  r0,r4,r0
//     020e70d4  bl   0x2022f40        <- Particle::SysTracker::Initialise
//     020e70d8  pop  {r4,lr}
//     020e70dc  bx   lr
//
// which is EXACTLY func_ov006_0210a6e4's shape one family over, at +0x47e4
// instead of +0x471c.  src/_ZN17MgBounceAndPounce18AfterInitResourcesEj.cpp
// spells the call with ONE argument, which is the only way to spell an unnamed
// value in C, and on the host that means the framework's flags word is not
// pushed and func_ov004_020b08f0 reads whatever the stack holds.
// port/unmatched/MgFlower_Slot2.cpp's header records what flags decides:
// vfSuccess == 1 marks the actor for destruction, so a stack-garbage flags is a
// coin flip on whether the scene survives its first frame.
//
// The repair is port_mg_d3dbase_after_init, in the SHARED file
// port/unmatched/MgD3DBase_Slots.cpp -- not a lane file, because the four ids
// under this base inherit the same body and run mg11 seats all four at once.
// src/_ZN17MgBounceAndPounce18AfterInitResourcesEj.cpp stays OUT of
// port/slice_tte.txt for the flower's reason: the vtable face routes to the
// repaired body, nothing dispatches the src one, and listing a body no dispatch
// reaches would put a line in the headline that the linker drops anyway.
//
// SLOTS 10, 27 AND 28 ARE THE SAME DEFECT AND THEY COST THE PICTURE.
// port/unmatched/MgD3DBase_Slots.cpp carries all three, in a file whose name is
// deliberately NOT lane-scoped: the four ids under this base inherit the same
// three bodies and lane BNT reached them from scene 373 while this lane reached
// them from scene 385.  Slot 10 is BeforeRender and its src spells
// `func_ov004_020b04f4()` with no argument, so Actor::BeforeRender was handed
// stack litter and answered NO -- the framework then skipped SLOT 9 and this
// scene ticked its whole state machine while drawing nothing.  Measured with
// RENDERING ON at both ends, which is the only config in which the question
// means anything:
//
//   before, 1500 frames   render 0     slot 10 1500  slot 11 1500
//   after,  3000 frames   render 3000  slot 10 3000  slot 11 3000
//   control 388, 600      render 600   slot 10 600
//
// SLOT 10 WAS DISPATCHED THROUGHOUT AND ANSWERED NO; slot 9 is the one that
// never ran.  An earlier version of this comment said slots 9/10/11 all read
// ZERO, which was a SM64DS_SCENE_NO_RENDER=1 census compared against a rendered
// control -- the wrong config on one side of an A/B.  Slots 27 and 28 are
// tail-call veneers whose srcs drop the same receiver.
//
// SLOT 11 LOOKS LIKE THE SAME DEFECT AND IS NOT, checked rather than assumed.
// 0x020e700c also never reads r1 -- but it ends in `bx ip` to 0x0202e398, a
// TAIL JUMP, so both r0 and r1 ride into Scene::AfterRender; and
// src/_ZN17MgBounceAndPounce11AfterRenderEj.cpp spells that call with BOTH
// arguments.  The src is already correct and the face below forwards the value.
// Slots 1, 7, 10 and 33 read no second argument in the ROM and their srcs take
// none, so their thunks pop nothing.
//
// ---- 5. THE WIDTH IS 36 ON BOTH TABLES, CHECKED FIVE WAYS ----------------
//
// port/mg_fanout_costs.txt section 11 measured that twelve of twenty-nine
// widths in its own table were phantoms, and 0x181 IS ONE OF THE TWELVE it
// corrected.  Both tables went through all five checks before a word was
// written -- the full derivation is port/slice_tte.txt section 3:
//
//   table       span to next symbol     slot 35     word past the end
//   ---------   ---------------------   ---------   ------------------------
//   0213fc7c    0213fd0c, 0x90 = 36     020ad660    0x02124ae4  (a code word)
//   0213c62c    see below               020ad660    0x6f6d6168  ("hamo", ASCII)
//
// AND THE WORD PAST THE END OF THIS TABLE BELONGS TO A CLASS THAT ALREADY
// SHIPS.  data_ov006_0213fd0c is the mwcc pair {0x02124ae4, 0} -- the FIRST of
// dScMgBSC_c's fourteen state pairs, the class run mg9 lane LKY seated as
// SCENE_MG_LUCKYSTARS and the tree boots today.  A 37-slot fill here would
// write a host thunk over the code word of a live state of scene 388.  That is
// section 11's hazard with a named, shipping victim, and the fill below is
// called with 36.
//
// THE MIDDLE TABLE'S SPAN CHECK IS THE ONE THAT NEEDS A WORD, because reading
// it carelessly gives twenty.  config/arm9/overlays/ov006/symbols.txt has a
// symbol at 0x0213c67c -- INSIDE the table, at slot 20, where the ov004 word
// 0x020b2990 sits, and it is marked ambiguous.  The span to the next symbol
// past the table is 0x0213c6bc, exactly 0x90 = 36 words on.  The other four
// checks do not depend on the symbol table at all and all four say 36: slot 35
// holds the family terminal 0x020ad660; the word at index 36 is 0x6f6d6168,
// the first four bytes of the ASCII string "hamon" and not an address; ov006's
// relocs.txt carries exactly 36 load relocations with a source inside the
// table, indices 0..35 complete; and their stride is four for those 36 words
// and then jumps to twelve.  Section 11's rule is that the checks REFUSE a
// width rather than grant one, and a span reading that would have granted 20
// is refused by four independent measurements.
//
// ---- 6. THE ROW GOES LAST, AND THE ORDER IS LOAD-BEARING ------------------
//
// port_scene_registry_install walks port_scene_classes[] in table order and
// calls every row's fill on every boot; port_scene_mg_overlay_load runs the
// thirty-five constructors ONCE PER PROCESS from the tail of the first minigame
// row's fill.  Appending after every existing row therefore means the
// constructors have already run with clean ROM words when this fill starts,
// which is the latent-safe direction section 11 names -- and section 3 above
// depends on it for a second reason.
//
// FOR THIS CLASS THE CONSTRUCTOR HAZARD IS MEASURED ABSENT, not inherited.
// ZERO relocations whose source lies in ov006's .init block
// (0x0212f4c4..0x02133600) land anywhere in this class's whole .data span
// 0x0213fbc8..0x0213fd0c.  dScMgTrampoline2_c HAS NO OVERLAY CONSTRUCTOR: its
// five state pairs are reached from literal pools inside its own bodies, not
// copied into a .bss table by a __sinit.  Same answer for 0x180's span
// 0x0213fa9c..0x0213fbc8, which is why this lane and lane TTI cannot collide
// over a shared sinit the way Memory Match and Memory Master could.
//
// ---- 7. WHAT THIS SEAT DOES NOT CLAIM -------------------------------------
//
// Two bodies on the live path have no source at all and are trapped in
// unmatched/MgTrampolineTerror_StateDispatch.cpp: func_ov006_02123938 (0x1e8,
// every tick, and its own callee list makes it a DRAW) and func_ov006_02123428
// (0x314, the play state's worker).  Their want counts are printed below,
// whether zero or not.  A run that reports slot hits and zero routed dispatches
// has ticked an object without entering its state machine, and that reads as a
// success unless it is said out loud, so the routed count is printed too.

#include "hal/screen_gap.h"
#include "MgBounceAndPounce.h"

#include <cstdio>
#include <cstdlib>

extern "C" {

/* the seam into hal/scene_mg.cpp -- mounts, arm9 fill, dScMgBase_c's faces */
unsigned port_scene_mg_fill_shared(void **vt, unsigned n);
void     port_scene_mg_prepare(int id);
int      port_scene_env_want(void);

/* the mount storage the fill writes into.  All three are the mounts' own, so
   the fill replaces the mounted words rather than building a fresh host array
   -- the ov007, curling and luckystars treatment, for the same reason: a second
   host array of the same name is a duplicate symbol, and leaving the mounted
   table alone leaves live wild DS pointers in a table the factory installs. */
extern unsigned char data_ov004_020bc0c0[];      /* dScMgBase_c,          36 */
extern unsigned char _ZTV17MgBounceAndPounce[];  /* dScMgD3DBase_c,       36 */
extern unsigned char data_ov006_0213fc7c[];      /* dScMgTrampoline2_c,   36 */
extern unsigned char MgTrampolineTerror_SpawnInfo[];

/* THE TRAMPOLINE-MARIO ELEMENT VTABLES.  The factory constructs five 0xdc-byte
   records at this+0x500c and each carries its OWN vtable in word 0 -- the
   element-vtable law run mg9 lane S381 banked, and this class is a live
   instance of it.  The ROM's own RTTI names the element classes: the type_info
   at 0x0213b238 points at 0x0213b25c, "17dScMgTrmpln2Mario_c", and its sibling
   at 0x0213b270 reads "18dScMgTrmpln3DMario_c".

   Three tables, written in constructor order by the three-deep chain
   func_ov006_020cd72c -> func_ov006_020cd6f4 -> the element ctor
   (src/_ZN6Player29TryExitCharacterDoorWithIntroEv.cpp, misnamed):

     0x0213b3c4  slot 4 = 0x020cd720, slot 11 = 0x020cd6d8   base-base
     0x0213b3e0  slot 4 = 0x020cd6d8                         base
     0x0213b2e0  slots 0..4 = 020c8e88 020c8e80 020c8dd4
                              020c8ddc 020cd6d8              most derived

   All THREE are filled, not just the live one: the intermediate tables are in
   the object's word 0 between the constructor's writes and again on the way
   down, so a virtual dispatched in that window reads them.  The words past each
   table's own slots are NOT touched -- 0x0213b2e0's index 5 is 0x118 and
   0x0213b3e0's index 5 is 0x3f001f, plain data -- because the fill is
   ADDRESS-KEYED and can only land where the ROM parked one of these six
   bodies. */
extern unsigned char data_ov006_0213b2e0[];   /* dScMgTrmpln2Mario_c   5 */
extern unsigned char data_ov006_0213b3e0[];   /* its base              */
extern unsigned char data_ov006_0213b3c4[];   /* its base's base       */
int  func_ov006_020c8e88(void *p);
int  func_ov006_020c8e80(void *p);
int  func_ov006_020c8dd4(void *p);
int  func_ov006_020c8ddc(char *c);
void func_ov006_020cd6d8(short *c);
void func_ov006_020cd720(short *p);

/* dScMgD3DBase_c's seventeen.  Slot 2 is NOT src's body: see section 4. */
int   port_mg_d3dbase_after_init(void *c, unsigned f);     /* slot  2 */
int   port_mg_d3dbase_before_render(void *c);              /* slot 10 */
int   _ZN17MgBounceAndPounceD1Ev(void *self);             /* slot 16 D2 */
int   _ZN17MgBounceAndPounceD0Ev(void *self);             /* slot 17 D0 */
int   func_ov006_020e6e78(char *self);                    /* slot 24 */
int   func_ov006_020e6e54(void *t);                       /* slot 25 */
int   func_ov006_020e6e4c(void);                          /* slot 26 */
void  port_mg_d3dbase_slot27(void *c);                     /* slot 27 */
void  port_mg_d3dbase_slot28(void *c);                    /* slot 28 */
void  func_ov006_020e6d24(char *c);                       /* slot 29 */
void  func_ov006_020e6cac(char *c);                       /* slot 30 */
void  func_ov006_020e72c0(char *c);                       /* slot 31 */
void  func_ov006_020e7124(char *obj);                     /* slot 33 */

/* dScMgTrampoline2_c's own twelve.  func_ov006_02123340 is the HOST COPY in
   unmatched/MgTrampolineTerror_StateDispatch.cpp, not the src TU: it is the
   field-held pointer-to-member dispatcher and the port cannot compile the src.

   SLOT 18 TAKES A SECOND ARGUMENT AND THE ROM BODY IGNORES IT, which is the
   ordinary case rather than dScMgBSC_c's.  0x021242cc never reads r1 in its
   0x120 bytes -- disassembled, not assumed -- so the value is only forwarded so
   __fastcall cleans the four bytes the caller pushed.  It is recorded anyway,
   because slot 0 dispatches slot 18 with -1 through the object's own vtable
   (`ldr r2,[r0]; mvn r1,#0; ldr r2,[r2,#0x48]; blx r2`) and a census that does
   not print the mode cannot show the thunk delivered it. */
int   func_ov006_021243ec(char *base);        /* slot  0 InitResources */
int   func_ov006_0212318c(void);              /* slot  3 CleanupResources */
int   func_ov006_02123340(void *self);        /* slot  6 Behavior, host copy */
int   func_ov006_021231ac(char *self);        /* slot  9 Render */
void *func_ov006_021225ac(void *thiz);        /* slot 16 D2 */
void *func_ov006_021226b0(char *thiz);        /* slot 17 D0 */
void  func_ov006_021242cc(void *self);        /* slot 18 state reset */
int   func_ov006_02124298(void);              /* slot 19 language load */
int   func_ov006_02122f24(char *self);        /* slot 23 */
int   func_ov006_021230e8(void *thiz);        /* slot 24 */
int   func_ov006_021230c4(void *t);           /* slot 25 */
void  func_ov006_02122cb0(void *obj, int x, int y, int val, int n); /* slot 34 */

/* the factory */
void *MgTrampolineTerror_Spawn(void);

/* the wall file's witnesses */
unsigned port_mg_tte_state_hits(void);
unsigned port_mg_tte_state_calls(void);
void     port_mg_tte_index_range(int *lo, int *hi);
void     port_mg_tte_state_report(void);
void     port_mg_tte_floor_counts(unsigned *a, unsigned *b);
void     port_mg_tte_sub_counts(unsigned *calls, unsigned *routed);
void     port_mg_tte_mario_counts(unsigned *calls, unsigned *routed);
void     port_mg_tte_trap_counts(unsigned *a, unsigned *b, unsigned *c,
                                 unsigned *d);
/* the framework's, from unmatched/MgBase_StateDispatch.cpp */
void     port_mg_dispatch_counts(unsigned *calls, unsigned *unknown);

/* the persistent minigame record this class's row is keyed to.  Nothing new is
   defined here: the storage is hal/level_boot.cpp's .dsstate$savblk0004 and the
   index function is src/func_ov004_020adc3c.c. */
extern void         *data_ov004_020beb68;     /* the live dScMgBase_c `this` */
extern unsigned char data_0209caf4[];         /* 36 records x 20 bytes       */
int  func_ov004_020adc3c(void *c);            /* (self->field_8 >> 8) & 0xff */

/* THE WORD PAST 0x180's TABLE, and it is this class's live flag.  Slot 23
   (src/func_ov006_02122f24.c) reads it and writes it on every OnAttacked2, so
   it is printed here as the witness for the sibling relay in
   port/slice_tte.txt section 3. */
extern int data_ov006_0213fbc4;

}  /* extern "C" */

/* The record row this class's object is keyed to, or -1 with no live base. */
static int port_mg_tte_record_index(void)
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
static unsigned g_tte_hits[36];
static unsigned g_tte_base_hits[36];   /* the same slots on the MIDDLE table */

#define TTE(n)   (++g_tte_hits[(n)])
#define D3D(n)   (++g_tte_base_hits[(n)])

/* the values slots 18, 5 and 11 were last handed */
static int g_tte_mode18 = -0x7fffffff, g_tte_mode19 = -0x7fffffff;
static unsigned g_tte_flags5 = 0xffffffffu, g_tte_flags11 = 0xffffffffu;
static unsigned g_tte_flags2 = 0xffffffffu;

/* ---- dScMgD3DBase_c's seventeen ----------------------------------------- */
static int  __fastcall d3_v1(void *s, void *)
{ D3D(1);  return ((MgBounceAndPounce *)s)->BeforeInitResources(); }
static int  __fastcall d3_v2(void *s, void *, unsigned f)
{ D3D(2);  g_tte_flags2 = f; return port_mg_d3dbase_after_init(s, f); }
static int  __fastcall d3_v5(void *s, void *, unsigned f)
{ D3D(5);  g_tte_flags5 = f;
  ((MgBounceAndPounce *)s)->AfterCleanupResources(f); return 0; }
static int  __fastcall d3_v7(void *s, void *)
{ D3D(7);  return ((MgBounceAndPounce *)s)->BeforeBehavior(); }
static int  __fastcall d3_v10(void *s, void *)
{ D3D(10); return port_mg_d3dbase_before_render(s); }
static int  __fastcall d3_v11(void *s, void *, unsigned f)
{ D3D(11); g_tte_flags11 = f;
  ((MgBounceAndPounce *)s)->AfterRender(f); return 0; }
static void *__fastcall d3_v16(void *s, void *)
{ D3D(16); return (void *)(size_t)_ZN17MgBounceAndPounceD1Ev(s); }
static void *__fastcall d3_v17(void *s, void *)
{ D3D(17); return (void *)(size_t)_ZN17MgBounceAndPounceD0Ev(s); }
static int  __fastcall d3_v24(void *s, void *)
{ D3D(24); return func_ov006_020e6e78((char *)s); }
static int  __fastcall d3_v25(void *s, void *)
{ D3D(25); return func_ov006_020e6e54(s); }
static int  __fastcall d3_v26(void *, void *)
{ D3D(26); return func_ov006_020e6e4c(); }
static int  __fastcall d3_v27(void *s, void *)
{ D3D(27); port_mg_d3dbase_slot27(s); return 0; }
static int  __fastcall d3_v28(void *s, void *)
{ D3D(28); port_mg_d3dbase_slot28(s); return 0; }
static int  __fastcall d3_v29(void *s, void *)
{ D3D(29); func_ov006_020e6d24((char *)s); return 0; }
static int  __fastcall d3_v30(void *s, void *)
{ D3D(30); func_ov006_020e6cac((char *)s); return 0; }
static int  __fastcall d3_v31(void *s, void *)
{ D3D(31); func_ov006_020e72c0((char *)s); return 0; }
static int  __fastcall d3_v33(void *s, void *)
{ D3D(33); func_ov006_020e7124((char *)s); return 0; }

/* ---- dScMgTrampoline2_c's own twelve ------------------------------------ */
static int  __fastcall tte_init(void *s, void *)
{ TTE(0);  const int r = func_ov006_021243ec((char *)s);
  /* the GaplessMinigames latch, for hal/scene_mg.cpp's reason: every seated
     minigame calls it so the ones the gapless table does not name can say
     "unsupported" instead of doing nothing quietly.  hal_gapless_splice() is
     deliberately NOT called -- scene 385 has no row in screen_gap.cpp's table
     and gapless behaviour is decided per minigame BY PLAYING IT.  This lane has
     not played it and does not get to vote.  Same ruling as
     hal/scene_mg_luckystars.cpp and hal/scene_mg_bomroom.cpp section 8. */
  hal_gapless_minigames_latch(); return r; }
static int  __fastcall tte_v3(void *, void *)
{ TTE(3);  return func_ov006_0212318c(); }
static int  __fastcall tte_beh(void *s, void *)
{ TTE(6);  return func_ov006_02123340(s); }
static int  __fastcall tte_render(void *s, void *)
{ TTE(9);  return func_ov006_021231ac((char *)s); }
static void *__fastcall tte_d2(void *s, void *)
{ TTE(16); return func_ov006_021225ac(s); }
static void *__fastcall tte_d0(void *s, void *)
{ TTE(17); return func_ov006_021226b0((char *)s); }
static int  __fastcall tte_v18(void *s, void *, int mode)
{ TTE(18); g_tte_mode18 = mode; func_ov006_021242cc(s); return 1; }
/* SLOT 19 TAKES A STACK PARAMETER EVEN THOUGH THIS CLASS'S BODY IGNORES IT,
   and getting that wrong cost this lane a 1600-frame crash.  0x02124298 never
   reads r1 in its 0x34 bytes -- disassembled, not assumed -- so the value is
   not forwarded.  But the SIGNATURE is decided by the callers, not the callee:
   hal/scene_mg.cpp's mb_v19 is `(void *, void *, int)` and
   port/unmatched/MgBase_ShadowSlot19.cpp's two host copies call slot 19
   through an `int (__fastcall *)(void *, void *, int)`, which is __fastcall
   with ONE stack parameter, so the callee must clean four bytes and end
   `ret 4`.  A two-parameter thunk ends `ret 0` and leaves esp four bytes low
   inside func_ov004_020b6c9c, whose epilogue then pops the return address into
   ebp and returns into the stack.

   THAT IS EXACTLY WHAT HAPPENED.  A 1800-frame boot of scene 385 faulted with
       FAULT c0000005 at +0xffdaf3a4 accessing 001af3a4
         regs ... ebp=020b6c9c
         port_mg_try_base_state / port_mg_call0 / func_ov004_020b8778
         / func_ov004_020b0620 / MgBounceAndPounce::BeforeBehavior
   -- eip inside the stack and ebp holding a DS code word, which is the
   signature MgBase_ShadowSlot19.cpp's header describes from the other side.
   The path is the one that file names: the framework state machine runs at all
   -> message index 18 -> per-frame tick 0x020b6c9c -> slot 19.  1500 frames
   pass and 1800 do not, deterministically.

   dScMgTrampoline2_c is the SECOND seated class to run the framework state
   machine (dScMgTeresa_c, run mg9 lane BOO, was the first), so this is the
   first time the family convention has been tested by a lane that did not
   already know it. */
static int  __fastcall tte_v19(void *, void *, int mode)
{ TTE(19); g_tte_mode19 = mode; return func_ov006_02124298(); }
static int  __fastcall tte_v23(void *s, void *)
{ TTE(23); return func_ov006_02122f24((char *)s); }
static int  __fastcall tte_v24(void *s, void *)
{ TTE(24); return func_ov006_021230e8(s); }
static int  __fastcall tte_v25(void *s, void *)
{ TTE(25); return func_ov006_021230c4(s); }
/* SLOT 34 TAKES FIVE AND THE TREE HAS A DISPATCHER FOR IT.  hal/scene_mg.cpp's
   mb_v34 header records that every slot-34 dispatch site passes r0..r3 plus one
   word at [sp], and that the tree's only dispatcher, func_ov004_020ae5c4, was
   seated by run mg10 lane F371.  THIS CLASS IS ON THAT DISPATCHER'S CALLER LIST
   -- src/func_ov006_02124088.c, state 1, calls it -- so the five-parameter
   spelling is not symmetry here, it is a live path. */
static int  __fastcall tte_v34(void *s, void *, int cx, int cy, int val, int n)
{ TTE(34); func_ov006_02122cb0(s, cx, cy, val, n); return 0; }

/* ---- the Trampoline-Mario element faces --------------------------------- */
static unsigned g_tte_elem_hits[5];
static int  __fastcall el_v0(void *s, void *)
{ ++g_tte_elem_hits[0]; return func_ov006_020c8e88(s); }
static int  __fastcall el_v1(void *s, void *)
{ ++g_tte_elem_hits[1]; return func_ov006_020c8e80(s); }
static int  __fastcall el_v2(void *s, void *)
{ ++g_tte_elem_hits[2]; return func_ov006_020c8dd4(s); }
static int  __fastcall el_v3(void *s, void *)
{ ++g_tte_elem_hits[3]; return func_ov006_020c8ddc((char *)s); }
static int  __fastcall el_v4(void *s, void *)
{ ++g_tte_elem_hits[4]; func_ov006_020cd6d8((short *)s); return 0; }
static int  __fastcall el_b4(void *s, void *)
{ func_ov006_020cd720((short *)s); return 0; }

/* SM64DS_SCENE_SLOT0=0 and SM64DS_SCENE_SLOT9=0, the diagnostics every scene
   seat in this port carries, counted separately so a run can never read a no-op
   as the real body having run. */
static unsigned g_tte_init_skipped, g_tte_render_skipped;
static int __fastcall tte_init_noop(void *, void *)
{ ++g_tte_init_skipped; return 1; }
static int __fastcall tte_render_noop(void *, void *)
{ ++g_tte_render_skipped; return 1; }

struct TteFace { unsigned ds; void *host; };

/* Keyed on the ROM WORD each slot holds, so the array is order-independent and
   cannot land on a slot the ROM did not park that body in. */
static const TteFace kD3DBaseFaces[] = {
    {0x020e70e4u, (void *)d3_v1},   {0x020e70c0u, (void *)d3_v2},
    {0x020e6f60u, (void *)d3_v5},   {0x020e7074u, (void *)d3_v7},
    {0x020e7040u, (void *)d3_v10},  {0x020e700cu, (void *)d3_v11},
    {0x020e6c28u, (void *)d3_v16},  {0x020e6c60u, (void *)d3_v17},
    {0x020e6e78u, (void *)d3_v24},  {0x020e6e54u, (void *)d3_v25},
    {0x020e6e4cu, (void *)d3_v26},  {0x020e6d98u, (void *)d3_v27},
    {0x020e6d8cu, (void *)d3_v28},  {0x020e6d24u, (void *)d3_v29},
    {0x020e6cacu, (void *)d3_v30},  {0x020e72c0u, (void *)d3_v31},
    {0x020e7124u, (void *)d3_v33},
};

/* The element tables' six bodies, keyed on the ROM word the way every other
   array in this file is.  0x020cd6d8 appears in all three tables and lands in
   each of them. */
static const TteFace kTrmplnMarioFaces[] = {
    {0x020c8e88u, (void *)el_v0},  {0x020c8e80u, (void *)el_v1},
    {0x020c8dd4u, (void *)el_v2},  {0x020c8ddcu, (void *)el_v3},
    {0x020cd6d8u, (void *)el_v4},  {0x020cd720u, (void *)el_b4},
};

static const TteFace kTteFaces[] = {
    {0x021243ecu, (void *)tte_init},  {0x0212318cu, (void *)tte_v3},
    {0x02123340u, (void *)tte_beh},   {0x021231acu, (void *)tte_render},
    {0x021225acu, (void *)tte_d2},    {0x021226b0u, (void *)tte_d0},
    {0x021242ccu, (void *)tte_v18},   {0x02124298u, (void *)tte_v19},
    {0x02122f24u, (void *)tte_v23},   {0x021230e8u, (void *)tte_v24},
    {0x021230c4u, (void *)tte_v25},   {0x02122cb0u, (void *)tte_v34},
};

static unsigned tte_apply(void **vt, unsigned n, const TteFace *f, unsigned nf)
{
    unsigned hit = 0;
    for (unsigned i = 0; i < n; ++i) {
        const unsigned ds = (unsigned)(size_t)vt[i];
        for (unsigned k = 0; k < nf; ++k)
            if (f[k].ds == ds) { vt[i] = f[k].host; ++hit; break; }
    }
    return hit;
}

/* Words still holding a DS address.  A minigame table is 36 slots and every one
   of them is dispatched by something, so a nonzero answer is a wild call
   waiting to happen and the seat says so out loud. */
static unsigned tte_raw_left(void **vt, unsigned n)
{
    unsigned left = 0;
    for (unsigned i = 0; i < n; ++i) {
        const unsigned w = (unsigned)(size_t)vt[i];
        if (w >= 0x02000000u && w < 0x02400000u)
            ++left;
    }
    return left;
}

extern "C" void port_scene_trampoline2_hits(void);

/* How many words this seat's own copy of the middle-table array claimed.
   SEVENTEEN is the expected reading on a tree that carries no other
   dScMgD3DBase_c row; ZERO is the expected reading once 0x174 lands ahead of
   it.  Section 3 is why, and it is printed rather than assumed. */
static unsigned g_tte_mid_claimed;

/* How many words the element-table fill claimed across the three tables.
   SEVEN is the expected reading: five on 0x0213b2e0, one on 0x0213b3c4 and one
   on 0x0213b3e0, each table walked for its own span. */
static unsigned g_tte_elem_claimed;

extern "C" void port_scene_fill_trampoline2(void)
{
    void **base = (void **)data_ov004_020bc0c0;
    void **mid  = (void **)_ZTV17MgBounceAndPounce;
    void **vt   = (void **)data_ov006_0213fc7c;

    /* THE BASE TABLE IS FILLED HERE TOO AND IT IS NOT CEREMONY.  Earlier rows'
       fills already did it and run first, so on a tree carrying them this is a
       second pass over words that are already host pointers and finds nothing.
       It is here so this class does not depend on another class's row existing:
       the factory's first act is func_ov004_020b2adc, which writes
       data_ov004_020bc0c0 into the object's first word before either derived
       table lands. */
    port_scene_mg_fill_shared(base, 36);

    /* THE MIDDLE TABLE.  Live storage in its own right -- the factory writes it
       into the object before the derived table and both destructors write it
       back on the way down, so a slot dispatched between the two vptr writes
       reads it.  Section 3 is why applying this file's own arrays here is safe
       next to the three sibling seats run mg11 lands beside it. */
    port_scene_mg_fill_shared(mid, 36);
    g_tte_mid_claimed =
        tte_apply(mid, 36, kD3DBaseFaces,
                  sizeof kD3DBaseFaces / sizeof kD3DBaseFaces[0]);
    g_tte_mid_claimed +=
        tte_apply(mid, 36, kTteFaces, sizeof kTteFaces / sizeof kTteFaces[0]);

    /* THE DERIVED TABLE.  Shared arm9 and dScMgBase_c words first, then the
       intermediate base's seventeen, then this class's own twelve.  Order does
       not matter -- the three key sets are disjoint, because a word is one
       address -- but it reads in inheritance order.  THIRTY-SIX, not
       thirty-seven: section 5, and the word a thirty-seventh would take is
       scene 388's first live state pair. */
    port_scene_mg_fill_shared(vt, 36);
    tte_apply(vt, 36, kD3DBaseFaces,
              sizeof kD3DBaseFaces / sizeof kD3DBaseFaces[0]);
    tte_apply(vt, 36, kTteFaces, sizeof kTteFaces / sizeof kTteFaces[0]);

    /* THE ELEMENT TABLES.  Five records at this+0x500c dispatch through them
       every tick of the play state, and before this fill existed the first
       dispatch called a raw DS word.

       EACH COUNT IS THAT TABLE'S OWN SPAN, and getting this wrong once is why
       it is spelled out.  config/arm9/overlays/ov006/symbols.txt puts the next
       symbol after 0x0213b2e0 at 0x0213b2f4 (5 words), after 0x0213b3c4 at
       0x0213b3e0 (7 words) and after 0x0213b3e0 at 0x0213b3f4 (5 words).  A
       first version of this fill walked 0x0213b3c4 for TWELVE words, which
       reaches 0x0213b3f0 -- that word is slot 4 of the table NEXT DOOR, and the
       only reason nothing was corrupted is that the fill is address-keyed and
       0x0213b3e0's own pass had already claimed it.  Section 5's width rule is
       not only for the 36-slot scene tables.

       SEVEN is the expected claim: five on 0x0213b2e0 (its whole table), one on
       0x0213b3c4 (slot 4, 0x020cd720) and one on 0x0213b3e0 (slot 4,
       0x020cd6d8).  The census prints it. */
    g_tte_elem_claimed  = tte_apply((void **)data_ov006_0213b2e0, 5,
                                    kTrmplnMarioFaces, 6);
    g_tte_elem_claimed += tte_apply((void **)data_ov006_0213b3c4, 7,
                                    kTrmplnMarioFaces, 6);
    g_tte_elem_claimed += tte_apply((void **)data_ov006_0213b3e0, 5,
                                    kTrmplnMarioFaces, 6);

    /* the two diagnostics, applied after the fill so they override it */
    {
        const char *s0 = std::getenv("SM64DS_SCENE_SLOT0");
        const char *s9 = std::getenv("SM64DS_SCENE_SLOT9");
        if (s0 && s0[0] == '0') vt[0] = (void *)tte_init_noop;
        if (s9 && s9[0] == '0') vt[9] = (void *)tte_render_noop;
    }

    {
        const unsigned lb = tte_raw_left(base, 36);
        const unsigned lm = tte_raw_left(mid, 36);
        const unsigned lv = tte_raw_left(vt, 36);
        if (lb || lm || lv) {
            std::fprintf(stderr, "  [scene] TRAMPOLINE2 FILL INCOMPLETE: "
                         "dScMgBase_c leaves %u of 36 raw DS words, "
                         "dScMgD3DBase_c %u, dScMgTrampoline2_c %u. A dispatch "
                         "of any of them jumps to a DS address as a host one.\n",
                         lb, lm, lv);
            std::fflush(stderr);
        }
    }

    /* The constructors, gated on the requested id, exactly as every other
       minigame fill does it.  Idempotent behind scene_mg.cpp's own
       once-per-process guard. */
    port_scene_mg_prepare(port_scene_env_want());

    if (port_scene_env_want() == 385) {
        static int armed;
        if (!armed) {
            armed = 1;
            std::atexit(port_scene_trampoline2_hits);
        }
    }
}

/* The registry's factory column is void *(*)(void) and so is the factory; the
   forwarder is kept for the reason title_spawn and port_mg_curling_spawn are,
   so the seat has one place to observe the object without the registry table
   growing a second column.

   THE FACTORY NEEDS NO DISPLACEMENT RULING, re-checked rather than assumed.
   port/mg_fanout_costs.txt section 12 grants one to 0x169 because
   src/func_ov006_020e0574.cpp calls the base constructor func_ov004_020b2adc
   with NO argument and rides r0 through.  src/MgTrampolineTerror_Spawn.c calls
   func_ov004_020b2adc(c) WITH its argument, so this class is on the correct
   side of it and the factory is linked from the slice rather than host-copied.
   That file carries a NONMATCHING banner (register allocation) and is a
   decompilation rather than a match; port/slice_tte.txt section 6 records it. */
static char *g_tte_self;

extern "C" void *port_mg_trampoline2_spawn(void)
{
    void *p = MgTrampolineTerror_Spawn();
    g_tte_self = (char *)p;
    return p;
}

extern "C" void port_scene_trampoline2_hits(void)
{
    unsigned total = 0, mtotal = 0;
    for (int i = 0; i < 36; ++i)
        { total += g_tte_hits[i]; mtotal += g_tte_base_hits[i]; }

    std::printf("[scene] dScMgTrampoline2_c slot hits: init %u, cleanup %u, "
                "behavior %u, render %u, D2 %u, D0 %u, state-reset %u, "
                "slot19 %u, attacked2 %u, kicked %u, pushed %u, brush %u%s\n",
                g_tte_hits[0], g_tte_hits[3], g_tte_hits[6], g_tte_hits[9],
                g_tte_hits[16], g_tte_hits[17], g_tte_hits[18], g_tte_hits[19],
                g_tte_hits[23], g_tte_hits[24], g_tte_hits[25], g_tte_hits[34],
                g_tte_render_skipped
                    ? "  [RENDER SLOT NO-OP'd: SM64DS_SCENE_SLOT9=0]" : "");
    if (g_tte_init_skipped)
        std::printf("[scene] INIT SLOT NO-OP'd: SM64DS_SCENE_SLOT0=0, %u "
                    "time(s)\n", g_tte_init_skipped);

    std::printf("[scene] dScMgTrampoline2_c own slots entered:");
    for (int i = 0; i < 36; ++i)
        if (g_tte_hits[i]) std::printf(" %d(x%u)", i, g_tte_hits[i]);
    std::printf("   (%u total)\n", total);

    std::printf("[scene] dScMgD3DBase_c slots entered (this seat's copy):");
    for (int i = 0; i < 36; ++i)
        if (g_tte_base_hits[i]) std::printf(" %d(x%u)", i, g_tte_base_hits[i]);
    std::printf("   (%u total); this seat's middle-table fill claimed %u word(s)"
                " (17 is EXPECTED on a tree with no other dScMgD3DBase_c row;"
                " 0 once 0x174 lands ahead of it -- section 3)\n",
                mtotal, g_tte_mid_claimed);

    std::printf("[scene] dScMgTrmpln2Mario_c element slots entered:");
    for (int i = 0; i < 5; ++i)
        if (g_tte_elem_hits[i]) std::printf(" %d(x%u)", i, g_tte_elem_hits[i]);
    std::printf("   (element-table fill claimed %u word(s), 7 expected)\n",
                g_tte_elem_claimed);

    {
        unsigned c = 0, r = 0;
        port_mg_tte_mario_counts(&c, &r);
        std::printf("[scene] dScMgTrmpln3DMario_c field member pointer at "
                    "offset 0: %u dispatch(es) attempted, %u routed to one of "
                    "the fourteen bodies in 0x0213b31c..0x0213b39c\n", c, r);
        c = r = 0;
        port_mg_tte_sub_counts(&c, &r);
        std::printf("[scene] dScMgTrmpln2Mario_c field member pointer at "
                    "+0x70: %u dispatch(es) attempted, %u routed to one of the "
                    "twenty bodies in 0x0213b0f4..0x0213b228\n", c, r);
    }

    /* THE STATE MACHINE'S OWN WITNESS.  Printed whether or not it fired. */
    {
        unsigned calls = 0, unknown = 0;
        int lo = -1, hi = -1;
        port_mg_dispatch_counts(&calls, &unknown);
        port_mg_tte_index_range(&lo, &hi);
        std::printf("[scene] dScMgTrampoline2_c state dispatch: %u routed to "
                    "one of the class's 5 field-held member pointers, %u "
                    "attempted at the site; state index range entered %d..%d; "
                    "%u framework call(s), %u UNHANDLED address(es)\n",
                    port_mg_tte_state_hits(), port_mg_tte_state_calls(),
                    lo, hi, calls, unknown);
        port_mg_tte_state_report();
    }

    {
        unsigned f938 = 0, f428 = 0;
        port_mg_tte_floor_counts(&f938, &f428);
        std::printf("[scene] dScMgTrampoline2_c FLOORS: func_ov006_02123938 "
                    "wanted %u time(s) (per-tick draw), func_ov006_02123428 "
                    "wanted %u time(s) (play-state worker). Both have no src "
                    "and no delinks block\n", f938, f428);
        unsigned t1 = 0, t2 = 0, t3 = 0, t4 = 0;
        port_mg_tte_trap_counts(&t1, &t2, &t3, &t4);
        std::printf("[scene] dScMgTrampoline2_c CLOSURE FLOORS: "
                    "func_ov006_020cf2fc %u, func_ov006_020d01e0 %u, "
                    "func_ov006_020d0c38 %u (the stroke-connected test "
                    "slot 23 asks; a trapped 0 takes the MISS arm), "
                    "func_ov006_020cfc74 %u\n", t1, t2, t3, t4);
    }

    /* The live object: the member pointer the ROM dispatches, the frame counter
       every state decrements, and the flag slot 23 shares with the word past
       0x180's vtable. */
    if (g_tte_self) {
        const unsigned code = *(const unsigned *)(g_tte_self + 0x5004);
        const int adj = *(const int *)(g_tte_self + 0x5008);
        std::printf("[scene] dScMgTrampoline2_c object at %p, member pointer "
                    "+0x5004 = {0x%08x, %d}, frame counter +0x7b84 = %d, "
                    "slot-18 mode last seen %d, slot-19 mode last seen %d, "
                    "slot-2 flags %u, slot-5 flags %u, slot-11 flags %u\n",
                    (void *)g_tte_self, code, adj,
                    *(const int *)(g_tte_self + 0x7b84), g_tte_mode18,
                    g_tte_mode19, g_tte_flags2, g_tte_flags5,
                    g_tte_flags11);
        std::printf("[scene] dScMgTrampoline2_c data_ov006_0213fbc4 = %d "
                    "(the word past 0x180's vtable; slot 23 reads and writes "
                    "it, so a 37-slot fill of SCENE_MG_TRAMPOLINE would "
                    "clobber it)\n", data_ov006_0213fbc4);
    }

    /* The save record.  ov005 row 32 is the only row for this id: its param is
       0x00202001, so the record index is 32 and the name-text index is 32,
       which is data_ov004_020bc070[32] = message 580 = "Trampoline Terror". */
    {
        const int idx = port_mg_tte_record_index();
        if (idx >= 0 && idx < 36) {
            const int *rec = (const int *)(data_0209caf4 + idx * 20);
            std::printf("[scene] dScMgTrampoline2_c save record "
                        "data_0209caf4[%d] = { %d, %d, %d, %d, %d }\n", idx,
                        rec[0], rec[1], rec[2], rec[3], rec[4]);
        } else {
            std::printf("[scene] dScMgTrampoline2_c save record: NO minigame "
                        "index (data_ov004_020beb68 = %p)\n",
                        data_ov004_020beb68);
        }
    }
    std::fflush(stdout);
}
