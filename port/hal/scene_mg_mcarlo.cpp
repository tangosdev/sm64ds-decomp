// dScMgMCarlo_c, the "Pair-a-Gone" card minigame -- actor id 0x17d, scene 381.
// Run mg9, lane S381. The FOURTH class the port seats under
// dScMgSingle3DBase_c, and the first whose spawn record has no named symbol in
// any config at all.
//
// Read port/slice_s381.txt for the identity derivation (six ROM witnesses, of
// which the player title is one), the four width checks and the closure. Read
// port/mg_fanout_costs.txt for what the family costs. This file is the seat:
// the two tables' faces, the fill, the factory forwarder and the tick witness.
//
// ---- 1. WHY THIS IS A SEPARATE FILE FROM hal/scene_mg.cpp -----------------
//
// scene_mg.cpp is the FAMILY half: the mounts, the thirty-five overlay
// constructors, dScMgBase_c's address-keyed faces and the gate that keeps the
// constructors off a level boot. All of that is paid once and none of it is
// re-derived here. This file reaches it through the one seam that file exports,
// port_scene_mg_fill_shared(), and adds only what is this class's. The
// alternative -- another face array appended inside scene_mg.cpp -- is what a
// ten-lane fan-out would collide on.
//
// ---- 2. THE HIERARCHY IS THREE DEEP, AND THE ROM SAYS SO THREE TIMES ------
//
//     Scene -> dScMgBase_c         data_ov004_020bc0c0  36 slots
//           -> dScMgSingle3DBase_c data_ov006_0213e448  36 slots
//           -> dScMgMCarlo_c       data_ov006_0213d664  36 slots
//
// The middle edge is stated by the ROM three separate ways, which is one more
// than the flower and memory2 lanes had for the same table:
//
//   TYPEINFO. The word before this class's vtable, 0x0213d660, points at the
//     typeinfo record at 0x0213d588, whose third word is 0x0213bc64 --
//     dScMgSingle3DBase_c's OWN typeinfo. That is the compiler writing the
//     inheritance down, not an inference from a name.
//   FACTORY. func_ov006_020f8e44 calls func_ov004_020b2adc (dScMgBase_c's
//     constructor), writes 0x0213e448 into the object's first word, then
//     writes 0x0213d664 over it.
//   DESTRUCTORS. Slot 17 (func_ov006_020f76a8, the D0) and its unmarked D2
//     sibling slot 16 (func_ov006_020f7634) both unwind them in the opposite
//     order, 0x0213d664 first and 0x0213e448 second.
//
// A hierarchy the typeinfo states, the constructor writes and the destructor
// unwrites is not a guess.
//
// ---- 3. THE FILL IS ADDRESS-KEYED, AND THIS IS THE FOURTH CLAIM ON THE
//         MIDDLE TABLE -----------------------------------------------------
//
// port_scene_fill_rom and mca_apply both key on the ROM WORD a slot holds, not
// on the slot number, so the same face array can be applied to every table in
// the chain and can only land where the ROM parked that exact body. The two
// arrays below are disjoint by construction:
//
//   kSingle3DFaces  the eight bodies dScMgSingle3DBase_c overrides
//                   (slots 2, 5, 7, 10, 16, 17, 26, 33)
//   kMCarloFaces    the seven dScMgMCarlo_c adds or replaces on top
//                   (slots 0, 6, 9, 16, 17, 18, 19)
//
// Slots 16 and 17 appear in both and hold DIFFERENT addresses -- 0x0210a4b0 /
// 0x0210a4e8 in the middle table, 0x020f7634 / 0x020f76a8 in the derived one --
// so both arrays can be applied to both tables and each writes only its own.
//
// THREE OTHER SEATED FILES DEFINE THEIR OWN COPY OF kSingle3DFaces:
// hal/scene_mg_flower.cpp, hal/scene_mg_memory2.cpp and this one. That is safe
// for the reason hal/scene_mg_memory2.cpp section 3 measures: mg_apply keys on
// a DS address, every row's fill runs on every boot, and the fill that runs
// FIRST claims the table -- the later ones find nothing left to write. This
// class's registry row is appended after the flower's and memory2's, so the
// flower keeps the middle table and both earlier witnesses keep counting
// exactly what they counted before this seat existed. This seat's own middle
// copy therefore reports zero on any tree that carries the flower row, and
// that zero is the CHECK rather than a disappointment.
//
// A FOURTH CLASS IS THE POINT AT WHICH PROMOTING IT SHOULD HAPPEN. Both
// earlier files say a third would be the moment to export kSingle3DFaces as a
// seam the way scene_mg.cpp exports port_scene_mg_fill_shared. This lane did
// NOT take that refactor, deliberately: it touches two files another mg9 lane
// may be editing in parallel, and the run's merge plan is a serial merge of
// ten branches. The argument is recorded here so the lane that does it has the
// count -- FOUR copies now -- rather than the intention.
//
// ---- 4. SLOT 2 IS NOT src's BODY, AND SLOT 33 IS NOT src's EITHER ---------
//
// func_ov006_0210a6e4 (AfterInitResources) drops the framework's second
// argument: the ROM never writes r1 before its `bl 0x20b08f0`, so the flags
// ride through in r1 and src spells the call with one argument because that is
// the only way to spell an unnamed value in C. On the host the callee would
// read stack litter, and func_ov004_020b08f0's tail is Scene::AfterInitResources
// (this, flags) where flags == 1 marks the actor for destruction. The repair is
// port_mg_flower_after_init in port/unmatched/MgFlower_Slot2.cpp, whose header
// asks the next lane to call it rather than write a second one; memory2 was
// the second and this is the third. src/func_ov006_0210a6e4.cpp stays out of
// port/slice_s381.txt for the same reason it is out of the other two slices.
//
// func_ov006_0210a708 (slot 33, the once-per-boot 3D setup) ends with two
// stores to LIGHT_COLOR at 0x040004cc, which a plain src build lands in the
// memory ntr maps across the I/O window without ever telling the geometry
// engine -- the "Yoshi is a black silhouette" defect run mg5 lane YTEX
// measured. It builds from the hostgen'd copy port/CMakeLists.txt's
// FLW_HOSTGEN_SYMS already emits, and src/func_ov006_0210a708.c is out of this
// slice for the same reason.
//
// ---- 4b. THERE IS A THIRD TABLE, AND IT IS THE CARD DRAW ------------------
//
// THE SINGLE MOST IMPORTANT THING THIS SEAT DOES THAT THE VTABLE AXIS CANNOT
// SEE. The factory builds an array of 0x50 records of 0x30 bytes at
// this+0x51a8 through func_020733a8 with the element constructor
// func_ov006_020f8ed8, and that constructor's whole body is
//
//     p[0] = (int)&data_ov006_0213d5ac;  p[1] = 0;  p[2] = 0;
//
// so every one of the eighty records carries a MOUNTED ROM TABLE as its vptr.
// data_ov006_0213d5ac is dMgMCarloCardObj_c's vtable -- the typeinfo pointer
// at 0x0213d5a8 reads 0x0213d578, whose name string is "18dMgMCarloCardObj_c"
// -- and it is TWO slots wide:
//
//     [0]  0x020f7e2c   the card DRAW
//     [1]  0x020f7ee4   the card's per-frame update, which takes an event
//
// AND SLOT 9 DISPATCHES SLOT 0 THROUGH IT. src/func_ov006_020f85b0.cpp walks
// the list at data_ov006_02142504 twice and calls `n->f0()` on each node --
// `ldr r1,[r0] / ldr r1,[r1] / blx r1` at 0x020f8614 and 0x020f8654 in the
// ROM -- and those nodes are the card records: src/func_ov006_020f7c10.c
// links `c` (which slot 18 passes as this+0x51a8) into a doubly-linked list
// through +4 and +8 and parks the head in data_ov006_02142500. Nothing else in
// ov006 ever constructs an object with this vptr: the only two relocations
// naming data_ov006_0213d5ac in the whole overlay are this class's element
// constructor and element destructor.
//
// SO A SEAT THAT FILLED ONLY THE TWO SCENE TABLES WOULD LINK, BOOT AND JUMP TO
// A RAW DS ADDRESS ON THE FIRST RENDERED FRAME. It is the same class of hole
// port/mg_fanout_costs.txt section 15 measured for Memory Master from the other
// end -- there the body existed and was never called, and the card grid
// rendered onto an empty table with every dispatch counter green -- and it is
// found here BEFORE the run rather than after, by asking what the ROM parks in
// an object's first word.
//
// THE WIDTH IS 2, CHECKED THE SAME WAY THE 36s ARE: the span from
// data_ov006_0213d5ac to the next config symbol data_ov006_0213d5b4 is 8 bytes
// = 2 words; exactly 2 load relocations fall inside that span; and the word
// past the end is 0x000000ba, the first entry of the language file-id table
// slot 0 reads, not a code address.
//
// THE CARD DRAW ITSELF is func_ov006_020f7e2c, which gates on the record's
// +0x2e, indexes data_ov006_0213d600 by (identity + 1) * 5 + flip-frame and
// hands the sprite from data_ov006_0214250c to Hud_RenderSprite at
// 0x020af68c -- the same HUD sprite seam eight other ov006 minigames draw
// through, and the same shape section 15 disassembles for Memory Master's
// twenty cards. The census at the foot of this file prints the live record
// count for the reason that section gives: a census that measures only
// DISPATCH cannot see a board that was never dealt.
//
// ---- 5. THE WIDTH IS 36 ON BOTH SCENE TABLES, CHECKED FOUR WAYS -----------
//
// port/mg_fanout_costs.txt section 11 measured that twelve of twenty-nine
// widths in its own table were phantoms, and that a 37-slot fill writes a host
// thunk over live ROM data belonging to another class. Both tables went
// through all three of its checks plus a fourth the relocation set gives free:
//
//   table       span to next symbol    slot 35     word past the end
//   ---------   --------------------   ---------   -------------------------
//   0213e448    0213e4d8, 0x90 = 36    020ad660    0x050a0a0f, the first word
//                                                  of data_ov006_0213e4d8
//   0213d664    0213d6f4, 0x90 = 36    020ad660    0x00000028, a small integer
//                                                  with its own config symbol
//
// FOURTH CHECK: exactly 36 load relocations fall inside [0x0213d664,
// 0x0213d6f4) and none lands on index 36. A vtable slot is a relocated word by
// construction, so a 37th slot with no relocation is not a slot.
//
// ---- 6. THE ROW GOES LAST, AND THE ORDER IS LOAD-BEARING ------------------
//
// port_scene_registry_install walks port_scene_classes[] in table order and
// calls every row's fill on every boot; port_scene_mg_overlay_load runs the
// thirty-five constructors ONCE PER PROCESS from the tail of the first
// minigame row's fill. Appending therefore means the constructors have already
// run with clean ROM words when this fill starts, which is the latent-safe
// direction section 11 names. Here it is a rule obeyed rather than a hazard
// relied on: zero relocations leave ov006's .init code (0x0212f4c4..
// 0x02133600) for this class's code block or its data span, so no constructor
// reads anything this fill writes.
//
// ---- 7. THIS CLASS HAS NO POINTER-TO-MEMBER WALL, MEASURED THREE WAYS -----
//
// port/mg_fanout_costs.txt section 4 opens "THE WALL, AND IT IS THE SAME ONE
// FOR ALL THIRTY". It is not this one's, and all three of that section's
// prescribed detectors were run rather than one:
//
//   - THE SOURCE, READ BEFORE THE FACE ARRAY WAS WIRED. That ordering is
//     section 14's practical rule, added after dScMgPanel_c's Behavior turned
//     out to open-code the ARM Itanium sequence in plain ints and read clean on
//     every static check. src/func_ov006_020f869c.c is a plain
//     `switch (*(s16*)(c + 0x60a8))` with cases 1, 2 and 3; slot 19
//     (src/func_ov006_020f8a3c.c) continues the same index with cases 4 to 8.
//   - THE ROM. 0x020f86a8 is ldrsh / cmp #1 beq / cmp #2 beq / cmp #3 beq / b,
//     and 0x020f8a50 is cmp r0,#8 / addls pc,pc,r0,lsl #2 over a nine-entry
//     branch table. Neither is the five-instruction Itanium sequence, and no
//     body in this class's block carries the `>> 1` / `& 1` pair decode that
//     is the third shape.
//   - ZERO relocations leave ov006's .init CODE for this class's cluster, so
//     no overlay constructor builds a pair table for it, there is no .bss
//     dispatch table to copy into and stategen has no constructor to read.
//   - A `::*` SWEEP over every TU in this slice is clean.
//   - AND THE RUN IS THE ONE THAT CONVICTS. Section 4 is explicit that a link
//     and a source sweep between them find twelve of thirteen and cannot
//     license the word "none". Scene 381 boots under SM64DS_FAULTS_FATAL=1
//     with zero UNHANDLED addresses and no eip on a DS address.
//
// The FRAMEWORK's wall is still the framework's and is still paid: this class
// reaches func_ov004_020b87e0 through dScMgBase_c exactly as curling does, and
// unmatched/MgBase_StateSetter.cpp's eighty routed addresses are inherited.
//
// ---- 8. WHAT THIS SEAT DOES NOT CLAIM -------------------------------------
//
//   - NOBODY HAS LOOKED AT A FRAME NEXT TO A DS. The census below reports what
//     the class does; it cannot say the picture is right. Section 16's "the
//     zeros that are honest and the zeros that are not" applies unchanged.
//   - THE SAVE RECORD IS THE PORT'S USUAL GAP. Slot 0 seeds +0xb4 from
//     func_ov004_020ad878(), which is data_0209caf4[record][1], and slot 6
//     writes it back through func_ov004_020ad79c with the same 0x270f clamp
//     section 17 disassembles for Memory Master. The port has no backup cart,
//     so SaveData::SetDefaultValuesMg zeroes the record on every boot and this
//     class restarts at its first board every launch. That is a save-medium
//     gap, not a minigame gap, and it is the same one section 17 records.
//   - TOUCH IS UNMEASURED HERE. The class is a stylus card game; nothing in a
//     headless boot drives a pick.

#include "hal/screen_gap.h"

#include <cstdio>
#include <cstdlib>

extern "C" {

/* the seam into hal/scene_mg.cpp -- mounts, arm9 fill, dScMgBase_c's faces */
unsigned port_scene_mg_fill_shared(void **vt, unsigned n);
void     port_scene_mg_prepare(int id);
int      port_scene_env_want(void);
int      IsMinigameActorID(unsigned int id);

/* The mount storage the fill writes into. Both tables are the ov006 mount's
   own, so the fill replaces the mounted words rather than building a fresh
   host array -- the ov007 and curling treatment, for the same reason: a second
   host array of the same name is a duplicate symbol, and leaving the mounted
   table alone leaves live wild DS pointers in a table the factory installs. */
extern unsigned char data_ov004_020bc0c0[];   /* dScMgBase_c,         36 */
extern unsigned char data_ov006_0213e448[];   /* dScMgSingle3DBase_c, 36 */
extern unsigned char data_ov006_0213d664[];   /* dScMgMCarlo_c,       36 */
extern unsigned char data_ov006_0213d5ac[];   /* dMgMCarloCardObj_c,   2 */
extern unsigned char data_ov006_0213d580[];   /* the SpawnInfo record     */

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

/* dScMgMCarlo_c's own seven */
int   func_ov006_020f8d08(char *c);           /* slot  0 InitResources */
int   func_ov006_020f869c(void *c);           /* slot  6 Behavior      */
int   func_ov006_020f85b0(char *c);           /* slot  9 Render        */
void *func_ov006_020f7634(char *c);           /* slot 16 D2            */
void *func_ov006_020f76a8(char *c);           /* slot 17 D0            */
void  func_ov006_020f8c68(char *c);           /* slot 18 state reset   */
int   func_ov006_020f8a3c(char *c);           /* slot 19               */

/* dMgMCarloCardObj_c's two, section 4b */
void  func_ov006_020f7e2c(char *thiz);        /* slot  0 the card draw */
void  func_ov006_020f7ee4(char *thiz, int event);  /* slot  1 the update */

/* the factory */
void *func_ov006_020f8e44(void);

/* ---- THE LOOP TRACE, SM64DS_MC_TRACE=1 (run mg12 lane CRD) --------------
   The sibling of hal/scene_mg_mcarlo2.cpp's SM64DS_MC2_TRACE, same globals one
   class over: 0x0213d564/6c/70/74 here for 0x0213d6fc/f4/f8/700 there. */
extern int           data_ov006_0213d574;     /* the dealt-card sweep, Fix12 */
extern int           data_ov006_0213d570;     /* board height (cards)        */
extern int           data_ov006_0213d56c;     /* cards left                  */
extern int           data_ov006_0213d564;     /* the "input armed" flag      */
extern short         data_ov006_021424ec;     /* the pair timer              */
extern int           data_ov006_021424f4;     /* the clear animation counter */
extern char         *data_ov006_021424fc;     /* first selected card         */
extern char         *data_ov006_02142508;     /* second selected card        */
extern char         *data_ov006_02142500;     /* the live card list head     */
extern unsigned char data_020a0de8[];
extern unsigned char data_020a0de9[];
extern unsigned char data_020a0dea[];
extern unsigned char data_020a0deb[];
extern int           data_020a0e40[];
/* THE BLINK CLOCK. src/func_ov006_020f7e2c.c -- the card draw -- skips a
   SELECTED card (state 3) on every frame this counter has bit 3 set, and that
   blink is the ONLY visual difference between a selected card and an idle one.
   Printed here because a frozen clock makes the selection invisible while every
   other number in this trace stays correct. */
extern int           data_020a0db0;
int  func_ov006_020f7a90(void);               /* "the board has settled"     */
int  func_ov006_020f7b90(void);               /* "a matching pair exists"    */

/* the framework's dispatch witness, from unmatched/MgBase_StateDispatch.cpp */
void  port_mg_dispatch_counts(unsigned *calls, unsigned *unknown);

/* The persistent minigame record this class's board progress is READ FROM and
   WRITTEN BACK TO. Nothing new is defined here: the storage is
   hal/level_boot.cpp's .dsstate$savblk0004 and the index function is
   src/func_ov004_020adc3c.c. Named so the census can print the record rather
   than leave it to be inferred. */
extern void         *data_ov004_020beb68;     /* the live dScMgBase_c `this` */
extern unsigned char data_0209caf4[];         /* 36 records x 20 bytes       */
int   func_ov004_020adc3c(void *c);           /* (self->field_8 >> 8) & 0xff */

}  /* extern "C" */

/* The record row this class's object is keyed to, or -1 with no live base.
   func_ov004_020ad878 -- which slot 0 seeds +0xb4 from -- goes through the
   same index function on the same pointer. */
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
static unsigned g_mca_hits[36];
static unsigned g_mca_base_hits[36];   /* the same slots on the MIDDLE table */

#define MCA(n)   (++g_mca_hits[(n)])
#define M3D(n)   (++g_mca_base_hits[(n)])

/* ---- dScMgSingle3DBase_c's eight ---------------------------------------- */
static void *__fastcall s3_ainit(void *s, void *, unsigned f)
{ M3D(2);  return (void *)(size_t)port_mg_flower_after_init(s, f); }
static void __fastcall s3_aclean(void *s, void *, unsigned f)
{ M3D(5);  func_ov006_0210a608(s, f); }
static int  __fastcall s3_bbeh(void *s, void *)
{ M3D(7);  return func_ov006_0210a698(s); }
static int  __fastcall s3_bren(void *s, void *)
{ M3D(10); return func_ov006_0210a664(s); }
static void *__fastcall s3_d2(void *s, void *)
{ M3D(16); return (void *)(size_t)func_ov006_0210a4b0((char *)s); }
static void *__fastcall s3_d0(void *s, void *)
{ M3D(17); return (void *)(size_t)func_ov006_0210a4e8((char *)s); }
static int  __fastcall s3_v26(void *, void *)
{ M3D(26); return func_ov006_0210a600(); }
static int  __fastcall s3_v33(void *s, void *)
{ M3D(33); func_ov006_0210a708((char *)s); return 0; }

/* ---- dScMgMCarlo_c's own seven ------------------------------------------ */
static int  __fastcall mca_init(void *s, void *)
{ MCA(0);  const int r = func_ov006_020f8d08((char *)s);
  /* the GaplessMinigames latch, for hal/scene_mg.cpp's reason: every seated
     minigame calls it so the ones the gapless table does not name can say
     "unsupported" instead of doing nothing quietly. */
  hal_gapless_minigames_latch(); return r; }
/* SM64DS_MC_TRACE=1: the state-3 loop, once a frame. Section CRD. */
static int  g_mca_trace = -1;
static void mca_trace(const char *when, const char *self)
{
    if (g_mca_trace < 0) {
        const char *e = std::getenv("SM64DS_MC_TRACE");
        g_mca_trace = (e && *e && *e != '0') ? 1 : 0;
    }
    if (!g_mca_trace) return;
    const int idx = data_020a0e40 ? (int)((unsigned char *)data_020a0e40)[0] : 0;
    std::printf("[mc1] %-3s tick %u state %d step %d sweep %d (%d) "
                "h570 %d left56c %d armed564 %d clr24f4 %d timer24ec %d "
                "sel %p/%p head %p settled %d pair %d "
                "touch %u/%u @%u,%u blink %d/%d\n",
                when, g_mca_hits[6],
                self ? *(const short *)(self + 0x60a8) : -1,
                self ? *(const short *)(self + 0x60ae) : -1,
                data_ov006_0213d574, data_ov006_0213d574 >> 12,
                data_ov006_0213d570, data_ov006_0213d56c,
                data_ov006_0213d564, data_ov006_021424f4,
                (int)data_ov006_021424ec,
                (void *)data_ov006_021424fc, (void *)data_ov006_02142508,
                (void *)data_ov006_02142500,
                func_ov006_020f7a90(), func_ov006_020f7b90(),
                (unsigned)data_020a0de8[idx * 4], (unsigned)data_020a0de9[idx * 4],
                (unsigned)data_020a0dea[idx * 4], (unsigned)data_020a0deb[idx * 4],
                data_020a0db0, data_020a0db0 & 8);
    if (self && when[0] == 'i' && (g_mca_hits[6] % 5) == 0) {
        for (int i = 0; i < 0x50; ++i) {
            const char *r = self + 0x51a8 + i * 0x30;
            std::printf("[mc1]     card %d state %u key %d face %u pos %d,%d "
                        "vis %u\n", i,
                        (unsigned)(unsigned char)r[0x2d],
                        (int)*(const short *)(r + 0x2a),
                        (unsigned)(unsigned char)r[0x2c],
                        *(const int *)(r + 0x0c) >> 12,
                        *(const int *)(r + 0x10) >> 12,
                        (unsigned)(unsigned char)r[0x2e]);
        }
    }
    std::fflush(stdout);
}

static int  __fastcall mca_beh(void *s, void *)
{ MCA(6);  mca_trace("in", (const char *)s);
  const int r = func_ov006_020f869c(s);
  mca_trace("out", (const char *)s); return r; }
static int  __fastcall mca_render(void *s, void *)
{ MCA(9);  return func_ov006_020f85b0((char *)s); }
static void *__fastcall mca_d2(void *s, void *)
{ MCA(16); return func_ov006_020f7634((char *)s); }
static void *__fastcall mca_d0(void *s, void *)
{ MCA(17); return func_ov006_020f76a8((char *)s); }
/* THE RIDE-THROUGH IS LOAD-BEARING ON BOTH OF THESE, and this class is where a
   lane can watch it happen rather than take it on the family's word: slot 0's
   own tail dispatches slot 18 through the object's vtable at offset 0x48 with
   `mvn r1,#0` -- one argument, value -1 -- at 0x020f8e08. Both ROM bodies then
   ignore their r1 (func_ov006_020f8c68 overwrites it with `mov r1,#4` on its
   fifth instruction; func_ov006_020f8a3c never reads it), so the parameter
   exists so __fastcall cleans four bytes and the callee is called without it.
   A thunk declared (void*, void*) compiles to a bare ret, leaks those four
   bytes, and the caller's own `ret` then takes a garbage return address --
   which is the fault lane BASESET repaired across the family and the flower
   seat's flw_reset carries. runs/mg5/out/baseset/slot18_19_scan.txt is the
   census: 22 slot-18 sites and 14 slot-19 sites, argument count ONE at every
   one of them. */
static int  __fastcall mca_reset(void *s, void *, int /*ridethrough*/)
{ MCA(18); func_ov006_020f8c68((char *)s); return 1; }
static int  __fastcall mca_v19(void *s, void *, int /*ridethrough*/)
{ MCA(19); return func_ov006_020f8a3c((char *)s); }

/* ---- dMgMCarloCardObj_c's two, section 4b ------------------------------- */
/* These are NOT __fastcall thunks over a Scene slot: the ROM dispatches them
   through the card record's own two-slot vtable with `this` in r0 and nothing
   else, so the host shape is __thiscall -- one hidden receiver, and for slot 1
   one stack argument the ROM passes in r1. The counters are here for section
   15's reason and not for symmetry: the draw is the only code in this class
   that puts a card pixel anywhere, so a run that ticks the board and never
   enters it looks identical from every other census line in this file. */
static unsigned g_mca_card_draws, g_mca_card_updates;
static void __fastcall card_draw(void *s, void *)
{ ++g_mca_card_draws; func_ov006_020f7e2c((char *)s); }
static void __fastcall card_update(void *s, void *, int event)
{ ++g_mca_card_updates; func_ov006_020f7ee4((char *)s, event); }

/* SM64DS_SCENE_SLOT0=0 and SM64DS_SCENE_SLOT9=0, the diagnostics every scene
   seat in this port carries, counted separately so a run can never read a
   no-op as the real body having run. */
static unsigned g_mca_init_skipped, g_mca_render_skipped;
static int __fastcall mca_init_noop(void *, void *)
{ ++g_mca_init_skipped; return 1; }
static int __fastcall mca_render_noop(void *, void *)
{ ++g_mca_render_skipped; return 1; }

struct McaFace { unsigned ds; void *host; };

static const McaFace kSingle3DFaces[] = {
    {0x0210a6e4u, (void *)s3_ainit},  {0x0210a608u, (void *)s3_aclean},
    {0x0210a698u, (void *)s3_bbeh},   {0x0210a664u, (void *)s3_bren},
    {0x0210a4b0u, (void *)s3_d2},     {0x0210a4e8u, (void *)s3_d0},
    {0x0210a600u, (void *)s3_v26},    {0x0210a708u, (void *)s3_v33},
};

/* dMgMCarloCardObj_c's two, section 4b. A SEPARATE ARRAY rather than two more
   rows in kMCarloFaces, because it is applied to a DIFFERENT table and the
   address-keyed fill would otherwise be free to write a card body into a scene
   table if the ROM ever parked one there. Keeping the arrays per-table is what
   makes "this array wrote n slots" a checkable number. */
static const McaFace kCardFaces[] = {
    {0x020f7e2cu, (void *)card_draw},  {0x020f7ee4u, (void *)card_update},
};

static const McaFace kMCarloFaces[] = {
    {0x020f8d08u, (void *)mca_init},   {0x020f869cu, (void *)mca_beh},
    {0x020f85b0u, (void *)mca_render}, {0x020f7634u, (void *)mca_d2},
    {0x020f76a8u, (void *)mca_d0},     {0x020f8c68u, (void *)mca_reset},
    {0x020f8a3cu, (void *)mca_v19},
};

static unsigned mca_apply(void **vt, unsigned n, const McaFace *f, unsigned nf)
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
static unsigned mca_raw_left(void **vt, unsigned n)
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
static unsigned g_mca_mid_claimed, g_mca_vt_claimed, g_mca_card_claimed;

extern "C" void port_scene_mcarlo_hits(void);

extern "C" void port_scene_fill_mcarlo(void)
{
    void **base = (void **)data_ov004_020bc0c0;
    void **mid  = (void **)data_ov006_0213e448;
    void **vt   = (void **)data_ov006_0213d664;

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
       arrays here is safe beside the flower's and memory2's, and why the
       claimed count below is expected to be ZERO on a tree carrying them. */
    port_scene_mg_fill_shared(mid, 36);
    g_mca_mid_claimed =
        mca_apply(mid, 36, kSingle3DFaces,
                  sizeof kSingle3DFaces / sizeof kSingle3DFaces[0])
      + mca_apply(mid, 36, kMCarloFaces,
                  sizeof kMCarloFaces / sizeof kMCarloFaces[0]);

    /* THE DERIVED TABLE. Shared arm9 and dScMgBase_c words first, then the
       intermediate base's eight, then this class's own seven. Order does not
       matter -- the three key sets are disjoint, because a word is one address
       -- but it reads in inheritance order. */
    port_scene_mg_fill_shared(vt, 36);
    g_mca_vt_claimed =
        mca_apply(vt, 36, kSingle3DFaces,
                  sizeof kSingle3DFaces / sizeof kSingle3DFaces[0])
      + mca_apply(vt, 36, kMCarloFaces,
                  sizeof kMCarloFaces / sizeof kMCarloFaces[0]);

    /* THE CARD TABLE, section 4b. Two slots, and it is not a Scene table: no
       arm9 or dScMgBase_c word appears in it, so port_scene_mg_fill_shared is
       NOT called over it -- only this class's own two rows can land, and if
       either fails to the mounted table keeps a raw DS word that the check
       below reports by name. Filling it here rather than at spawn time is
       correct because the element constructor writes the table's ADDRESS into
       each of the eighty records, not its contents, so records built later
       pick the host pointers up on their own. */
    {
        void **card = (void **)data_ov006_0213d5ac;
        g_mca_card_claimed =
            mca_apply(card, 2, kCardFaces,
                      sizeof kCardFaces / sizeof kCardFaces[0]);
    }

    /* the two diagnostics, applied after the fill so they override it */
    {
        const char *s0 = std::getenv("SM64DS_SCENE_SLOT0");
        const char *s9 = std::getenv("SM64DS_SCENE_SLOT9");
        if (s0 && s0[0] == '0') vt[0] = (void *)mca_init_noop;
        if (s9 && s9[0] == '0') vt[9] = (void *)mca_render_noop;
    }

    {
        const unsigned lb = mca_raw_left(base, 36);
        const unsigned lm = mca_raw_left(mid, 36);
        const unsigned lv = mca_raw_left(vt, 36);
        const unsigned lc = mca_raw_left((void **)data_ov006_0213d5ac, 2);
        if (lb || lm || lv || lc) {
            std::fprintf(stderr, "  [scene] MCARLO FILL INCOMPLETE: "
                         "dScMgBase_c leaves %u of 36 raw DS words, "
                         "dScMgSingle3DBase_c %u, dScMgMCarlo_c %u, "
                         "dMgMCarloCardObj_c %u of 2. A dispatch of any of "
                         "them jumps to a DS address as a host one.\n",
                         lb, lm, lv, lc);
            std::fflush(stderr);
        }
    }

    /* The constructors, gated on the requested id, exactly as every other
       minigame fill does it. Idempotent behind scene_mg.cpp's own
       once-per-process guard. */
    port_scene_mg_prepare(port_scene_env_want());

    if (port_scene_env_want() == 381) {
        static int armed;
        if (!armed) {
            armed = 1;
            std::atexit(port_scene_mcarlo_hits);
        }
    }
}

/* The registry's factory column is void *(*)(void) and so is the matched
   factory, but the forwarder is kept for the reason title_spawn and
   port_mg_curling_spawn are: it gives the seat one place to observe the object
   without the registry table growing a second column. */
static char *g_mca_self;

extern "C" void *port_mg_mcarlo_spawn(void)
{
    void *p = func_ov006_020f8e44();
    g_mca_self = (char *)p;
    return p;
}

extern "C" void port_scene_mcarlo_hits(void)
{
    unsigned total = 0, mtotal = 0;
    for (int i = 0; i < 36; ++i) {
        total += g_mca_hits[i];
        mtotal += g_mca_base_hits[i];
    }

    std::printf("[scene] dScMgMCarlo_c slot hits: init %u, behavior %u, "
                "render %u, D2 %u, D0 %u, state-reset %u, slot19 %u%s\n",
                g_mca_hits[0], g_mca_hits[6], g_mca_hits[9], g_mca_hits[16],
                g_mca_hits[17], g_mca_hits[18], g_mca_hits[19],
                g_mca_render_skipped
                    ? "  [RENDER SLOT NO-OP'd: SM64DS_SCENE_SLOT9=0]" : "");
    if (g_mca_init_skipped)
        std::printf("[scene] INIT SLOT NO-OP'd: SM64DS_SCENE_SLOT0=0, %u "
                    "time(s)\n", g_mca_init_skipped);

    std::printf("[scene] dScMgMCarlo_c own slots entered:");
    for (int i = 0; i < 36; ++i)
        if (g_mca_hits[i]) std::printf(" %d(x%u)", i, g_mca_hits[i]);
    std::printf("   (%u total)\n", total);

    std::printf("[scene] dScMgSingle3DBase_c slots entered (this seat's copy):");
    for (int i = 0; i < 36; ++i)
        if (g_mca_base_hits[i]) std::printf(" %d(x%u)", i, g_mca_base_hits[i]);
    std::printf("   (%u total)\n", mtotal);

    /* Section 3's claim, measured rather than asserted: on a tree carrying the
       flower and memory2 rows this seat's middle copy should claim ZERO slots
       and its derived copy should claim THIRTEEN of the fifteen face rows --
       this class's own seven plus six of the middle base's eight. The two
       that do not land are the middle base's own D2 and D0 (0x0210a4b0 and
       0x0210a4e8): this class overrides slots 16 and 17 with bodies of its
       own, so those two words are not in the derived table to be claimed.
       13 + 8 + 2 = 23 host writes across the three tables, and the fill
       report below prints all three. Measured on a 300-frame scene-381 boot:
       middle 0, derived 13, card 2 of 2. */
    std::printf("[scene] dScMgMCarlo_c fill claims: middle table %u slot(s), "
                "derived table %u slot(s) (of 8 + 7 face rows), card table %u "
                "of 2\n",
                g_mca_mid_claimed, g_mca_vt_claimed, g_mca_card_claimed);

    /* THE CARD TABLE'S OWN WITNESS, section 4b. The draw is the only code in
       this class that puts a card pixel anywhere, so this pair of numbers is
       the difference between "the board ticked" and "the board was drawn". */
    std::printf("[scene] dMgMCarloCardObj_c dispatches: %u draw(s) through "
                "slot 0, %u update(s) through slot 1\n",
                g_mca_card_draws, g_mca_card_updates);

    /* THE STATE MACHINE'S OWN WITNESS. This class dispatches its states through
       a plain compiled switch on the s16 at +0x60a8 rather than through a
       member-pointer table, so there is no address switch to count and the
       honest instrument is the INDEX ITSELF: slot 18 sets it to 1, slot 6
       carries it through 1, 2 and 3, and slot 19 carries it through 4 to 8. A
       run whose index never moves has ticked the object without running its
       state machine, and that reads as a success unless it is said out loud.
       The framework's own dispatch counters are printed beside it because a
       zero there is a different fact from a zero here. */
    {
        unsigned calls = 0, unknown = 0;
        port_mg_dispatch_counts(&calls, &unknown);
        std::printf("[scene] dScMgMCarlo_c state index +0x60a8 = %d, sub-timer "
                    "+0x60aa = %d, step +0x60ae = %d; %u framework call(s), "
                    "%u UNHANDLED address(es)\n",
                    g_mca_self ? *(const short *)(g_mca_self + 0x60a8) : -1,
                    g_mca_self ? *(const short *)(g_mca_self + 0x60aa) : -1,
                    g_mca_self ? *(const short *)(g_mca_self + 0x60ae) : -1,
                    calls, unknown);
    }

    /* THE CARD ARRAY. The factory builds 0x50 records of 0x30 bytes at
       +0x51a8 with the element vtable 0x0213d5ac ("18dMgMCarloCardObj_c"), and
       slot 18 hands the whole array to func_ov006_020f7c10 on every round
       reset. Section 15's lesson is why this line exists at all: a census that
       measures only DISPATCH cannot see a board that was never dealt, and
       Memory Master's sixteen cards rendered onto an empty table with every
       dispatch counter reading green. */
    if (g_mca_self) {
        unsigned live = 0;
        for (int i = 0; i < 0x50; ++i)
            if (*(const void *const *)(g_mca_self + 0x51a8 + i * 0x30))
                ++live;
        std::printf("[scene] dScMgMCarlo_c card array: %u of 80 record(s) hold "
                    "a vptr\n", live);
    }

    /* THE SAVED PROGRESS, both ends. Slot 0 seeds +0xb4 from
       func_ov004_020ad878() = data_0209caf4[record][1]; slot 6 writes it back
       through func_ov004_020ad79c with the 0x270f clamp its literal pool
       carries. Printing both says which of them moved, which is the question
       section 17 had to disassemble a whole function to answer for Memory
       Master. */
    {
        const int idx = port_mg_record_index();
        if (g_mca_self)
            std::printf("[scene] dScMgMCarlo_c progress: +0xb4 level = %d, "
                        "+0xb8 best = %d, +0xa8 score = %d\n",
                        *(const int *)(g_mca_self + 0xb4),
                        *(const int *)(g_mca_self + 0xb8),
                        *(const int *)(g_mca_self + 0xa8));
        if (idx >= 0 && idx < 36) {
            const int *rec = (const int *)(data_0209caf4 + idx * 20);
            std::printf("[scene] dScMgMCarlo_c save record data_0209caf4[%d] "
                        "= { %d, %d, %d, %d, %d }\n", idx,
                        rec[0], rec[1], rec[2], rec[3], rec[4]);
        } else {
            std::printf("[scene] dScMgMCarlo_c save record: NO minigame index "
                        "(data_ov004_020beb68 = %p)\n", data_ov004_020beb68);
        }
    }
    std::fflush(stdout);
}

/* ---- THE NAME-SPELLING ALIASES -------------------------------------------
 *
 * The "C-named symbols declared at C++ linkage" case hal/scene_boot.cpp
 * section 1 carries twenty-three of for the star select and
 * port/mg_fanout_costs.txt section 4 counts twenty-two of in the family's own
 * wall. Three TUs in this slice reach a mounted DS data symbol through a
 * namespace-scope `extern` rather than through an extern "C" block, so MSVC
 * puts the TYPE into the symbol name and the mount's plain C definition can
 * never satisfy it. Each row below is the mangling of one such declaration.
 *
 * THEY ARE ALIASES AND NOT FACES, and the test is section 4's rulebook
 * corollary rather than habit: an alias is safe when the two spellings
 * describe the same eight-or-four bytes at the same address, and unsafe when
 * one of them is a member-pointer type (where mwcc's eight-byte pair and
 * MSVC's four-byte one disagree on stride). None of these three is a member
 * pointer: 0x0213d664 and 0x0213e448 are vtables the factory stores by
 * address, and 0x02142504 is a single mounted WORD holding a pointer to a card
 * record, which src/func_ov006_020f85b0.cpp reads exactly as the ROM's
 * `ldr r6,[r0]` does.
 *
 * ?data_ov006_02142504@@3PAUNode@@A IS THE SPELLING SECTION 10's TOOL FINDING
 * 1 WARNS ABOUT, and it is worth saying why it is still an alias here. That
 * finding is about @@3PAU hiding a member-pointer TABLE behind a struct
 * wrapper -- `struct Entry { PMF pmf[1]; } []` -- and the refusal exists
 * because such a table strides by 8 in the ROM and by 4 on the host. `Node` is
 * an ordinary object type with a vptr, a next pointer and an int at +0x20;
 * every one of those offsets is the same width on both machines, and the
 * disassembly at 0x020f85f0 reads the same three fields. The guard's shape
 * matches and its reason does not.
 *
 * TWO SPELLINGS THIS SLICE ALSO NEEDS ARE ALREADY IN THE IMAGE and are NOT
 * repeated here, because a second /alternatename for the same LHS is noise
 * a reader has to diff: ?data_ov006_0213e448@@3HA is in
 * hal/scene_mg_flower.cpp, ?data_ov006_0213e448@@3PAXA is in
 * port/unmatched/MgMemory2_Faces.cpp, and the bare-name rows _func_020beb68
 * and _func_020bc7d4 that this class's slot 6, slot 18 and slot 19 reach are
 * in port/unmatched/MgCoin_Faces.cpp and MgMemory2_Faces.cpp.
 */
#pragma comment(linker, "/alternatename:?data_ov006_0213d664@@3HA=_data_ov006_0213d664")
#pragma comment(linker, "/alternatename:?data_ov006_0213d664@@3PAXA=_data_ov006_0213d664")
#pragma comment(linker, "/alternatename:?data_ov006_02142504@@3PAUNode@@A=_data_ov006_02142504")
/* AND ONE FROM THE ov004 TALLY CHAIN. src/func_ov004_020b58c4.cpp declares
 * `extern int data_ov004_020bfa0c;` outside its extern "C" block; the mount
 * defines the plain C name for the ov004 bss word at 0x020bfa0c. It is the
 * ONLY one of that TU's nine namespace-scope externs the link asked for --
 * the other eight already have alias rows in the image, which is what a
 * ninth-minigame lane inheriting eight seats' worth of alias work looks
 * like. */
#pragma comment(linker, "/alternatename:?data_ov004_020bfa0c@@3HA=_data_ov004_020bfa0c")
