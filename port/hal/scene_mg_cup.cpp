// dScMgCup_c, actor id 0x169, scene 361 -- the minigame the ROM calls
// "TOX BOX SHUFFLE". Run mg9, lane CUP. The twelfth ov006 minigame class the
// port seats and the THIRD under dScMgSingle3DBase_c.
//
// ---- 0. THE PLAYER TITLE, DERIVED RATHER THAN GUESSED ---------------------
//
// The class name is the ROM's C++ one and says nothing a player would
// recognise. The title comes off a four-link chain, every link read out of the
// ROM, using the method run mg9 lane S371 published:
//
//   scene 361 = id 0x169
//     -> ov005 row 23, the ONLY row in the thirty-six that carries this id, so
//        this class has none of the menu-position ambiguity six other ids have
//     -> that row's own +0x04 word, param 0x000b1700
//     -> (param >> 16) & 0xff = 11, the name-text index
//     -> data_ov004_020bc070[11] = 559, the BMG message id. Cross-checked
//        against the LIVE object: this file's census prints +0x465e, which
//        src/func_ov004_020af094.cpp reads, and it also reads 559.
//     -> data/message/msg_data_eng.bin, LZ77 type 0x10, 91712 bytes,
//        MESGbmg1 / INF1 (711 messages; the header field at +0x0a reads
//        0x0040, which is 64 BITS -- an EIGHT-byte entry stride, not a
//        64-byte one: 711 entries at 64 bytes would need 45504 and the
//        section is 0x1660 = 5728. hal/message_boot.cpp indexes it as
//        `entries + id * 8` and include/MessageBank.h documents the same
//        8-byte stride. Corrected in the run mg9 amendment) / DAT1,
//        message 559:
//
//          "Tox Box Shuffle
//           Oh, no! Yoshi's trapped in a Tox Box!
//           Find which one he's in to..."
//
// THE CHAIN IS CONTROLLED, which is what makes it a derivation rather than a
// plausible reading. Run over every id this port has ALREADY seated it
// reproduces each one's known title exactly: 0x170 Bob-omb Squad, 0x16e
// Wanted!, 0x186 Loves me...?, 0x172 Sort or 'Splode, 0x17a Coincentration,
// 0x17c Puzzle Panel, 0x176 Shuffle Shell, 0x171 Lakitu Launch, 0x16b Memory
// Master, 0x177 Shell Smash -- and it reproduces BOTH rows of the duplicated
// ids, 0x178 as Bingo
// Ball (row 10) and Slots Shot (row 22), 0x17c as Puzzle Panel (row 15) and
// Puzzle Panic (row 35). The text is a GLYPH-INDEX stream and not ASCII, which
// is why earlier sweeps for these names missed them: 'A' is 0x0a, 'a' is 0x2d
// and space is 0x4d (hal/message_boot.cpp's decoder).
//
// THE REGISTRY ROW KEEPS ITS CLASS-DERIVED NAME, SCENE_MG_CUP, for the reason
// every other row does -- port/tools/battery.py reads the hosted-scene set out
// of that table and the family names its rows after the ROM's classes. The
// player title belongs in the prose, here.
//
// Read port/slice_cup.txt for the identity derivation, the three width checks
// and the closure. Read port/mg_fanout_costs.txt section 12 for the ten ROM
// adjudications and the two ride-throughs run link60 lane MGA paid for, and
// section 11 for the width correction that is load-bearing here. This file is
// the seat: the two new tables' faces, the fill, the factory forwarder and the
// run census.
//
// ---- 1. WHY THIS IS A SEPARATE FILE FROM hal/scene_mg.cpp -----------------
//
// The reason hal/scene_mg_flower.cpp gives and hal/scene_mg_memory2.cpp
// repeats, unchanged. scene_mg.cpp is the FAMILY half -- the mounts, the
// thirty-five overlay constructors, dScMgBase_c's twenty-eight address-keyed
// faces and the gate that keeps the constructors off a level boot -- and all
// of it is reached through the one seam that file exports,
// port_scene_mg_fill_shared(). Nothing of it is re-derived here and
// kMgBaseFaces is NOT copied: a second copy would install a different set of
// host thunks over the same DS addresses in the same mounted tables and split
// the per-slot witness in two.
//
// ---- 2. THE HIERARCHY IS THREE DEEP AND THE ROM SAYS SO THREE WAYS --------
//
//     Scene -> dScMgBase_c         data_ov004_020bc0c0  36 slots
//           -> dScMgSingle3DBase_c data_ov006_0213e448  36 slots
//           -> dScMgCup_c          data_ov006_0213c154  36 slots
//
// port/mg_fanout_costs.txt section 12 derives this from the two deleting
// destructors. THIS LANE ADDS THE STRONGER WITNESS, which is the one lane PPP
// prescribes and which no earlier reader of this class used: the RELOCATIONS
// out of the type_info records. The word immediately before a class's vtable
// points at its type_info, and that record's second word is the name and its
// third is the BASE record:
//
//   from:0x0213c150 -> 0x0213c048     the word before dScMgCup_c's vtable
//   0x0213c048 = { 0x0209a764 __si_class_type_info,
//                  name  -> 0x0213c054 "10dScMgCup_c",
//                  base  -> 0x0213bc64 }
//   0x0213bc64 = { 0x0209a764,
//                  name  -> 0x0213bd00 "19dScMgSingle3DBase_c",
//                  base  -> 0x020bbf6c }        (ov004)
//   0x020bbf6c = { 0x0209a764,
//                  name  -> 0x020bbf84 "11dScMgBase_c", ... }
//
// and 0x0213bc64 is ALSO the word before data_ov006_0213e448 (reloc row
// from:0x0213e444), so the middle table and the middle type_info are closed on
// each other. Every edge above is a relocation, and a relocation cannot be a
// per-TU placeholder spelling.
//
// The factory and the destructors agree: src/func_ov006_020e0574.cpp writes
// 0x0213e448 into the object's first word and then 0x0213c154 over it, and
// src/func_ov006_020dea1c.cpp (slot 17, D0) unwinds them in the opposite
// order. Both edges are code, not names.
//
// ---- 3. THE FILL IS ADDRESS-KEYED, SO THREE TABLES ARE CHEAP --------------
//
// port_scene_mg_fill_shared and cup_apply both key on the ROM WORD a slot
// holds, not on the slot number, so the same face array can be applied to
// every table in the chain and can only land where the ROM parked that exact
// body.
//
//   kSingle3DFaces  the eight bodies dScMgSingle3DBase_c overrides
//                   (slots 2, 5, 7, 10, 16, 17, 26, 33)
//   kCupFaces       the seven this class adds or replaces on top
//                   (slots 0, 6, 9, 16, 17, 18, 20)
//
// Slots 16 and 17 appear in both and hold DIFFERENT addresses -- 0x0210a4b0 /
// 0x0210a4e8 in the middle table, 0x020de988 / 0x020dea1c in this one -- so
// both arrays can be applied to both tables and each writes only its own. Six
// of kSingle3DFaces' eight fill slots 2, 5, 7, 10, 26 and 33 of the DERIVED
// table, because this class does not override them.
//
// THE ARITHMETIC, so a reader can check the shape rather than believe it:
// 13 overrides against dScMgBase_c's table (slots 0, 2, 5, 6, 7, 9, 10, 16,
// 17, 18, 20, 26, 33), of which 6 hold the MIDDLE base's body and 7 hold this
// class's own. 6 + 7 = 13, and port/tools/vtablerows.py --vtable 0x0213c154
// --width 36 reads ovr 13, mark 9, nosrc 0 out of the ROM independently.
//
// THE MIDDLE TABLE IS SHARED WITH TWO SEATS THAT ALREADY SHIP, AND THIS ROW
// GOES AFTER BOTH. hal/scene_mg_flower.cpp (390) and hal/scene_mg_memory2.cpp
// (363) both define their own eight-row array over the same eight DS words of
// data_ov006_0213e448, and all three fills run on every boot. mg_apply keys on
// a DS address, so the fill that runs FIRST claims the table and the ones
// after it find no DS word left there and write nothing. Row order decides
// which, and this class's registry row is APPENDED AFTER BOTH, so on any tree
// carrying all three:
//
//   - data_ov006_0213e448 keeps hal/scene_mg_flower.cpp's thunks, unchanged
//     from the baseline, and BOTH that file's witness and scene_mg_memory2's
//     keep counting exactly what they counted before this seat existed. The
//     lane verifies that by booting 390 and 363 after seating and comparing
//     their middle-table censuses against the same runs on the base commit.
//   - data_ov006_0213c154 gets THIS file's thunks in its six inherited slots,
//     which is the only table this seat needs to own. THOSE SIX ARE WHY THIS
//     FILE'S OWN dScMgSingle3DBase_c COUNTER IS NONZERO on a scene-361 run:
//     the array is applied to both tables, it claims nothing on the middle
//     one, and every hit it reports is a dispatch through the DERIVED table's
//     inherited slots. Measured on a 300-frame boot: 2(x1) 7(x300) 26(x2)
//     33(x1) = 304, and the flower's own line on scene 390 does not move.
//
// THIS IS THE THIRD CLASS UNDER THIS BASE, AND scene_mg_memory2.cpp's SECTION 3
// CALLED IT: "If a THIRD class under this base is ever seated, promoting
// kSingle3DFaces to an exported seam the way scene_mg.cpp exports
// port_scene_mg_fill_shared is the right move." IT IS STILL RIGHT AND IT IS
// STILL NOT TAKEN HERE, for the reason that file gives: the seam would have to
// be created in one of the two other lanes' files and exported from there, and
// a fan-out lane editing another lane's file is the five-way merge the
// per-class file split exists to avoid. What this lane can do without reaching
// across is state the cost honestly: THREE copies of an eight-row array now
// exist, all forwarding to the identical ROM bodies, and the only observable
// difference between them is which file's counter a middle-table dispatch
// lands in. A fourth class under this base should do the promotion rather than
// add a fourth copy, and this paragraph is the second argument for it.
//
// AND THE FOURTH CLASS IS NAMED NOW. Run mg9 lane LKY found that
// MgSnowballSlalom (0x179) -- the one id of the thirty whose vtable
// port/mg_fanout_costs.txt section 3 could not resolve at all -- sits under
// this same middle base: its table is 0x0214000c and its factory tail-calls
// func_ov006_021295ac, which writes 0x0213e448 and then 0x0214000c. So the
// promotion is no longer hypothetical, and whoever seats 0x179 is the lane
// these two paragraphs are addressed to.
//
// ---- 4. SLOT 2 IS NOT src's BODY, AND IT IS NOT THIS LANE'S HOST COPY -----
//
// func_ov006_0210a6e4 (AfterInitResources) drops the framework's second
// argument: the ROM never writes r1 before its `bl 0x20b08f0`, so the flags
// ride through in r1, and src spells the call with one argument because that
// is the only way to spell an unnamed value in C. On the host the callee reads
// stack litter, and func_ov004_020b08f0's tail is Scene::AfterInitResources
// (this, flags) where flags == 1 marks the actor for destruction -- a coin
// flip on whether the scene survives frame 0.
//
// port/unmatched/MgFlower_Slot2.cpp already carries the repair as
// port_mg_flower_after_init, and its header ends "THE NEXT LANE TO SEAT 361 OR
// 363 SHOULD CALL THIS RATHER THAN WRITE A SECOND". 363 called it in run mg6.
// THIS IS 361, the other id that header names, and it calls it too.
// src/func_ov006_0210a6e4.cpp stays out of port/slice_cup.txt for the same
// reason it is out of port/slice_flw.txt and port/slice_mem.txt: listing it
// would be an LNK2005 against that host copy.
//
// ---- 5. THE WIDTH IS 36 ON BOTH TABLES, CHECKED THREE WAYS ----------------
//
// port/mg_fanout_costs.txt section 11 measured that twelve of twenty-nine
// widths in its own section 3 table were phantoms, and 0x169 IS THE CLASS
// THAT CORRECTION WAS FOUND ON. Both tables went through all three of its
// checks again on this tree before a word was written:
//
//   table       span to next symbol    slot 35     word past the end
//   ---------   --------------------   ---------   -------------------------
//   0213c154    0213c1e4, 0x90 = 36    020ad660    0x020e0a24 followed by
//                                                  0x00000000 -- an mwcc
//                                                  {code, adj} PAIR, and it is
//                                                  CURLING'S
//   0213e448    0213e4d8, 0x90 = 36    020ad660    0x050a0a0f, the first word
//                                                  of data_ov006_0213e4d8
//
// THE THIRD CHECK IS NOT A FORMALITY ON THIS CLASS. 0x0213c1e4 is copied by
// src/__sinit_ov006_021304ac.c line 75:
//
//     data_ov006_02141930.p2 = data_ov006_0213c1e4;
//
// -- CURLING's own overlay constructor, filling curling's state table
// 0x02141930 slot 2, whose address port/unmatched/MgCurling_StateDispatch.cpp
// independently lists as 020e0a24. A 37-slot fill here writes a host thunk
// over the code word of a state belonging to the one minigame that has shipped
// longest. Re-read out of extracted/overlays/overlay_0006.bin and re-verified
// against that sinit line by this lane rather than cited.
//
// A PROPOSED FIFTH CHECK FAILS ON THIS CLASS, AND THAT IS WORTH MORE THAN A
// FIFTH CHECK WOULD HAVE BEEN. Run mg9 lane LKY offered one that "needs no
// symbols and no judgment": read the RELOCATION STRIDE out of relocs.txt,
// which runs at 4 bytes for exactly the table width and then jumps to 8.
// Measured on three tables of this family:
//
//   dScMgFlower_c       0x02140140   36 consecutive 4-byte relocs   -> 36  ok
//   dScMgSingle3DBase_c 0x0213e448   36, then a gap of 12           -> 36  ok
//   dScMgCup_c          0x0213c154   37, then a gap of 8            -> 37  WRONG
//
// The run does not stop at the end of the table here, because the word past
// the end IS A RELOCATED CODE WORD sitting at the next four-byte boundary --
// 0x0213c1e4 is the {code, adjustment} pair's code half, and its relocation
// continues the stride. The 8-byte gap the check looks for arrives one slot
// LATE, after the pair's zero adjustment word. So the check reads 37 on
// exactly the class section 11's whole hazard was found on, and reads it in
// the GRANTING direction. It is a fine confirmation when it agrees with the
// span; it cannot be a substitute for it, and no lane should adopt it as one.
//
// ---- 6. THE ROW GOES LAST, AND THE ORDER IS LOAD-BEARING TWICE ------------
//
// port_scene_registry_install walks port_scene_classes[] in table order and
// calls every row's fill on every boot; port_scene_mg_overlay_load runs the
// thirty-five constructors ONCE PER PROCESS from the tail of the first
// minigame row's fill. Appending after every existing row therefore means the
// constructors have already run with clean ROM words when this fill starts,
// which is the latent-safe direction section 11 names -- and section 3 above
// depends on the same ordering for the middle table's ownership.
//
// ---- 7. THE WALL IS THIS CLASS'S AND IT IS TWO TUs, NOT ONE ---------------
//
// port/unmatched/MgCup_StateDispatch.cpp carries the Behavior and the eight
// state addresses of data_ov006_02141870.
// port/unmatched/MgShared4f38_SubDispatch.cpp carries func_ov006_020c2b8c,
// which dispatches a member pointer held in a
// FIELD of the sub-object at +0x4f38 and which section 12 and
// port/slice_mga361.txt both missed -- section 12's adjudication row for
// 0x020e0204 calls it "the class ONLY pointer-to-member dispatch site" and
// slice_mga361 lists src/func_ov006_020c2b8c.cpp as an ordinary slice line.
// Both headers are the derivation. Nothing in THIS file dispatches a member
// pointer.
//
// ---- 8. THE THREE SWEEPS THIS LANE OWED, AND WHAT THEY FOUND -------------
//
// ELEMENT VTABLES: NONE, and it was checked rather than assumed. Run mg9 lane
// S381 found a class whose factory builds eighty card records each carrying
// its OWN two-slot vtable in word 0, which nothing in the ovr/mark/nosrc
// columns can see. This factory builds two arrays through func_020733a8 --
// 0x20 elements of 0x18 at +0x50e8 and 3 of 8 at +0x53e8 -- and BOTH element
// constructors are four-byte `bx lr` bodies in the ROM (func_ov006_020e0634 at
// 0x020e0634 = e12fff1e, and the arm9 func_0203d738, both size 0x4 in
// config), so no element gets a vtable word. The +0x4f38 sub-object's word 0
// is not a vtable either: it is the mwcc {code, adjustment} pair
// unmatched/MgShared4f38_SubDispatch.cpp routes.
//
// LOCAL SHADOW CLASSES: FOUR IN THIS CLOSURE, ONE WRONG AND FIXED. Section
// 13's test is which kind of table the shadowed object carries.
//   func_ov006_020c29dc  WRONG -- Model and BlendModelAnim carry HOST tables
//       in MSVC order. Host-copied,
//       unmatched/MgShared4f38_ModelRender_020c29dc.cpp.
//   func_ov006_020e0308  CORRECT -- it shadows `this`, whose vptr is the
//       MOUNTED ROM table this file fills in ROM word order, and its
//       `((VtObj *)c)->m18(3)` is ROM slot 18. Exercised (init 1) and clean,
//       and it is the WITNESS for the slot-18 argument decision below.
//   func_ov006_020df1c0  CORRECT, same shape, `((Obj *)c)->v18(-1)`. NOT
//       exercised: it is state slot 6 and no unattended boot reaches it.
//       Reported rather than claimed.
//   func_ov006_020c2848  CORRECT -- it shadows the arm9 object at
//       data_0209f5bc, whose table is hal/fdr_arm9_fader_seat.cpp's ROM-shaped
//       fill. Exercised through func_ov006_020c2924 on the slot-18 path and
//       clean.
//
// SLOT 34 IS NOT DISPATCHED BY THIS CLASS. Run mg9 lane S371 reported that
// hal/scene_mg.cpp's shared mb_v34 thunk is declared (void *, void *) while
// func_ov004_020ae3b4 takes five parameters at every ROM dispatch site, and
// asked whichever lane witnesses a slot-34 dispatch to take the repair. This
// class does not: every run's framework census reads `framework slots entered:
// 1(x1) 31(x1) 32(x1)`, this class's own slots are 0, 6, 9 and 18, and the
// middle base's are 2, 7, 10, 26 and 33. Nothing here would have been evidence
// for a repair, so none was made.
//
// ---- 9. WHAT THIS SEAT DOES NOT CLAIM -------------------------------------
//
// The state machine is proven to run by the census this file prints, not by
// this comment. A run that reports slot hits and zero routed dispatches has
// ticked an object without entering its state machine, and that reads as a
// success unless it is printed -- which is why every number below is printed
// whether it is zero or not.
//
// NO FRAME HAS BEEN LOOKED AT. The render path runs -- slot 9 enters on every
// frame of every run, SM64DS_MAT_LOG reports 2575 NORMALs under two live
// POLYGON_ATTR words in sixty frames, and SM64DS_TRI_LOG shows fifteen
// textured batches -- but nobody has held this next to a DS. Section 9's note
// stands for this class too, and the BMP captures exist so it can be judged by
// someone who can.
//
// TOUCH IS UNMEASURED, and on this class it is the live question rather than a
// formality: it is a shell game, the state index reaches 4 of 8 on a
// 1200-frame unattended boot and stops there, and states 5, 6 and 7 are never
// entered. Whether they need a stylus is not something a headless run can say.

#include "hal/screen_gap.h"

#include <cstdio>
#include <cstdlib>

extern "C" {

/* the seam into hal/scene_mg.cpp -- mounts, arm9 fill, dScMgBase_c's faces */
unsigned port_scene_mg_fill_shared(void **vt, unsigned n);
void     port_scene_mg_prepare(int id);
int      port_scene_env_want(void);
int      IsMinigameActorID(unsigned int id);
unsigned port_mg_scene_spawn_param(int scene_id);

/* the mount storage the fill writes into. All three are the mounts' own, so
   the fill replaces the mounted words rather than building a fresh host array
   -- the ov007, curling and memory2 treatment, for the same reason: a second
   host array of the same name is a duplicate symbol, and leaving the mounted
   table alone leaves live wild DS pointers in a table the factory installs. */
extern unsigned char data_ov004_020bc0c0[];   /* dScMgBase_c,         36 */
extern unsigned char data_ov006_0213e448[];   /* dScMgSingle3DBase_c, 36 */
extern unsigned char data_ov006_0213c154[];   /* dScMgCup_c,          36 */
extern unsigned char data_ov006_0213c020[];   /* the SpawnInfo record    */

/* dScMgSingle3DBase_c's eight overrides. Slot 2 is NOT src's body: see
   section 4 and port/unmatched/MgFlower_Slot2.cpp. */
int   port_mg_flower_after_init(void *c, unsigned f);   /* slot  2 */
void  func_ov006_0210a608(void *c, unsigned f);         /* slot  5 */
int   func_ov006_0210a698(void *c);                     /* slot  7 */
int   func_ov006_0210a664(void *c);                     /* slot 10 */
int   func_ov006_0210a4b0(char *c);                     /* slot 16 D2 */
int   func_ov006_0210a4e8(char *c);                     /* slot 17 D0 */
int   func_ov006_0210a600(void);                        /* slot 26 */
void  func_ov006_0210a708(char *c);                     /* slot 33 */

/* dScMgCup_c's own seven. func_ov006_020e0204 is the HOST COPY in
   unmatched/MgCup_StateDispatch.cpp, not the src TU: it is the
   pointer-to-member dispatcher and the port cannot compile the src. */
int   func_ov006_020e0308(char *c);       /* slot  0 InitResources */
int   func_ov006_020e0204(char *o);       /* slot  6 Behavior, host copy */
int   func_ov006_020e0068(char *c);       /* slot  9 Render */
void *func_ov006_020de988(char *c);       /* slot 16 D2 */
void *func_ov006_020dea1c(char *c);       /* slot 17 D0 */
void  func_ov006_020dfeec(char *c, int msg); /* slot 18 state reset */
void  func_ov006_020dfed4(char *p);       /* slot 20 */

/* the factory, host-copied for the dropped base-constructor argument; the
   ruling is in port/unmatched/MgCup_Factory.cpp's header */
void *func_ov006_020e0574(void);

/* the two dispatch files' witnesses */
unsigned port_mg_cup_state_hits(void);
unsigned port_mg_cup_floor_hits(void);
void     port_mg_cup_state_slots(unsigned *out8);
unsigned port_mg_sub4f38_calls(void);
unsigned port_mg_sub4f38_routed(void);
unsigned port_mg_sub4f38_idle(void);
unsigned port_mg_sub4f38_unknown(void);
/* the framework's, from unmatched/MgBase_StateDispatch.cpp */
void     port_mg_dispatch_counts(unsigned *calls, unsigned *unknown);

/* the persistent minigame record, and the framework body that indexes it.
   Nothing new is defined here: the storage is hal/level_boot.cpp's
   .dsstate$savblk0004 and the index function is src/func_ov004_020adc3c.c.
   Named so the census can print the record this class is keyed to rather than
   leave it to be inferred -- run mg8 lane MMD's finding is that the record
   index is a real, per-minigame thing and that the port used to give every
   minigame record 0. */
extern void         *data_ov004_020beb68;     /* the live dScMgBase_c `this` */
extern unsigned char data_0209caf4[];         /* 36 records x 20 bytes       */
int  func_ov004_020adc3c(void *c);            /* (self->field_8 >> 8) & 0xff */

}  /* extern "C" */

static int port_mg_cup_record_index(void)
{
    return data_ov004_020beb68 ? func_ov004_020adc3c(data_ov004_020beb68) : -1;
}

// ---- the tick witness ------------------------------------------------------
//
// One counter per dispatched slot on each of the two tables this file owns
// faces for. The instrument every scene seat in this port carries, for the
// reason hal/scene_mg.cpp gives: an object that EXISTS and an object that RUNS
// look identical from outside. The framework slots dScMgBase_c keeps are
// counted by scene_mg.cpp's own array.
static unsigned g_cup_hits[36];
static unsigned g_cup_base_hits[36];   /* the same slots on the MIDDLE table */

#define CUP(n)   (++g_cup_hits[(n)])
#define C3D(n)   (++g_cup_base_hits[(n)])

/* ---- dScMgSingle3DBase_c's eight ---------------------------------------- */
static void *__fastcall s3_ainit(void *s, void *, unsigned f)
{ C3D(2);  return (void *)(size_t)port_mg_flower_after_init(s, f); }
static void __fastcall s3_aclean(void *s, void *, unsigned f)
{ C3D(5);  func_ov006_0210a608(s, f); }
static int  __fastcall s3_bbeh(void *s, void *)
{ C3D(7);  return func_ov006_0210a698(s); }
static int  __fastcall s3_bren(void *s, void *)
{ C3D(10); return func_ov006_0210a664(s); }
static void *__fastcall s3_d2(void *s, void *)
{ C3D(16); return (void *)(size_t)func_ov006_0210a4b0((char *)s); }
static void *__fastcall s3_d0(void *s, void *)
{ C3D(17); return (void *)(size_t)func_ov006_0210a4e8((char *)s); }
static int  __fastcall s3_v26(void *, void *)
{ C3D(26); return func_ov006_0210a600(); }
static int  __fastcall s3_v33(void *s, void *)
{ C3D(33); func_ov006_0210a708((char *)s); return 0; }

/* ---- dScMgCup_c's own seven --------------------------------------------- */
static int  __fastcall cup_init(void *s, void *)
{ CUP(0);  const int r = func_ov006_020e0308((char *)s);
  /* the GaplessMinigames latch, for hal/scene_mg.cpp's reason: every seated
     minigame calls it so the ones the gapless table does not name can say
     "unsupported" instead of doing nothing quietly. Scene 361 has no row in
     kGaplessScenes, so hal_gapless_splice is deliberately NOT called -- the
     hal/scene_mg_bomroom.cpp precedent for an unlisted scene. */
  hal_gapless_minigames_latch(); return r; }
static int  __fastcall cup_beh(void *s, void *)
{ CUP(6);  return func_ov006_020e0204((char *)s); }
static int  __fastcall cup_render(void *s, void *)
{ CUP(9);  return func_ov006_020e0068((char *)s); }
static void *__fastcall cup_d2(void *s, void *)
{ CUP(16); return func_ov006_020de988((char *)s); }
static void *__fastcall cup_d0(void *s, void *)
{ CUP(17); return func_ov006_020dea1c((char *)s); }
/* SLOT 18 TAKES ONE STACK ARGUMENT AND THIS CLASS'S BODY READS IT, which is
   the difference from every earlier seat and is worth stating rather than
   inheriting. Run mg5 lane BASESET scanned slot 18's offset (0x48) out of the
   two overlay images word by word: 22 sites, argument count ONE at every one.
   Six seated classes declared the parameter only so __fastcall would clean the
   caller's four bytes, because their bodies ignore r1. THIS ONE DOES NOT:
   src/func_ov006_020dfeec.c is `void (char *c, int msg)` and its whole top
   half is `if (msg == 3 || msg == 0x12) ... else if (msg == 0) ...`, three
   different resets. Declaring the parameter and dropping it on the floor would
   have compiled, linked, cleaned the stack correctly and silently taken the
   msg == 0 arm every time. The face passes it.

   THE ROM SAYS IT IN FOUR INSTRUCTIONS, so this is a reading and not a guess:

       020dfeec  push {r4, lr} / sub sp, sp, #8 / mov r4, r0
       020dfef8  cmp  r1, #3
       020dfefc  beq  #0x20dff08
       020dff00  cmp  r1, #0x12
       020dff04  bne  #0x20dff6c

   r1 is compared twice before anything else happens.

   AND IT IS WITNESSED RATHER THAN ASSERTED. src/func_ov006_020e0308.cpp
   (InitResources, slot 0) ends with `((VtObj *)c)->m18(3)` -- ROM slot 18 with
   mode 3 -- and every run of this scene reports state-reset 1, one dispatch,
   on the init path. The observable consequence is in the census: with mode 3
   the reset writes set = 3 into +0x5434+i*4 for each cup whose +0x5462+i flag
   is nonzero and 0 for the others, and the run reports flags f1/f0/f0 and
   cups 4/1/1 after func_ov006_020def80's own branches ran on top -- which is
   the pairing those two bodies produce only for that flag pattern.

   SLOT 19 IS NOT THIS CLASS'S. dScMgCup_c holds dScMgBase_c's own 0x020b2994
   at slot 19, so hal/scene_mg.cpp's mb_v19 serves it and already declares the
   ride-through. Run mg9 lane LKY's slot-18/19 warning applies to slot 18 here
   and to nothing else. */
static int  __fastcall cup_reset(void *s, void *, int msg)
{ CUP(18); func_ov006_020dfeec((char *)s, msg); return 1; }
/* Slot 20 takes the receiver and nothing else, the same shape as the base's
   own mb_v20 in hal/scene_mg.cpp. */
static int  __fastcall cup_v20(void *s, void *)
{ CUP(20); func_ov006_020dfed4((char *)s); return 0; }

/* SM64DS_SCENE_SLOT0=0 and SM64DS_SCENE_SLOT9=0, the diagnostics every scene
   seat in this port carries, counted separately so a run can never read a
   no-op as the real body having run. */
static unsigned g_cup_init_skipped, g_cup_render_skipped;
static int __fastcall cup_init_noop(void *, void *)
{ ++g_cup_init_skipped; return 1; }
static int __fastcall cup_render_noop(void *, void *)
{ ++g_cup_render_skipped; return 1; }

struct CupFace { unsigned ds; void *host; };

/* Keyed on the ROM WORD each slot holds, so the arrays are order-independent
   and cannot land on a slot the ROM did not park that body in. */
static const CupFace kSingle3DFaces[] = {
    {0x0210a6e4u, (void *)s3_ainit},  {0x0210a608u, (void *)s3_aclean},
    {0x0210a698u, (void *)s3_bbeh},   {0x0210a664u, (void *)s3_bren},
    {0x0210a4b0u, (void *)s3_d2},     {0x0210a4e8u, (void *)s3_d0},
    {0x0210a600u, (void *)s3_v26},    {0x0210a708u, (void *)s3_v33},
};

static const CupFace kCupFaces[] = {
    {0x020e0308u, (void *)cup_init},  {0x020e0204u, (void *)cup_beh},
    {0x020e0068u, (void *)cup_render},{0x020de988u, (void *)cup_d2},
    {0x020dea1cu, (void *)cup_d0},    {0x020dfeecu, (void *)cup_reset},
    {0x020dfed4u, (void *)cup_v20},
};

static unsigned cup_apply(void **vt, unsigned n, const CupFace *f, unsigned nf)
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
static unsigned cup_raw_left(void **vt, unsigned n)
{
    unsigned left = 0;
    for (unsigned i = 0; i < n; ++i) {
        const unsigned w = (unsigned)(size_t)vt[i];
        if (w >= 0x02000000u && w < 0x02400000u)
            ++left;
    }
    return left;
}

extern "C" void port_scene_cup_hits(void);

extern "C" void port_scene_fill_cup(void)
{
    void **base = (void **)data_ov004_020bc0c0;
    void **mid  = (void **)data_ov006_0213e448;
    void **vt   = (void **)data_ov006_0213c154;

    /* THE BASE TABLE IS FILLED HERE TOO AND IT IS NOT CEREMONY. Earlier rows'
       fills already did it and run first, so on this tree this is a second
       pass over words that are already host pointers and finds nothing. It is
       here so this class does not depend on another class's row existing: the
       factory's first act after operator new is func_ov004_020b2adc, which
       writes data_ov004_020bc0c0 into the object's first word before either
       derived table lands. */
    port_scene_mg_fill_shared(base, 36);

    /* THE MIDDLE TABLE. Live storage in its own right -- the destructor chain
       writes it into the object on the way down, so a slot dispatched between
       the two vptr writes reads it. Section 3 is why applying this file's own
       arrays here is safe next to the flower's and memory2's, and why this
       fill is expected to claim NOTHING on a tree carrying either of them. */
    port_scene_mg_fill_shared(mid, 36);
    cup_apply(mid, 36, kSingle3DFaces,
              sizeof kSingle3DFaces / sizeof kSingle3DFaces[0]);
    cup_apply(mid, 36, kCupFaces,
              sizeof kCupFaces / sizeof kCupFaces[0]);

    /* THE DERIVED TABLE. Shared arm9 and dScMgBase_c words first, then the
       intermediate base's eight, then this class's own seven. Order does not
       matter -- the three key sets are disjoint, because a word is one address
       -- but it reads in inheritance order. */
    port_scene_mg_fill_shared(vt, 36);
    cup_apply(vt, 36, kSingle3DFaces,
              sizeof kSingle3DFaces / sizeof kSingle3DFaces[0]);
    cup_apply(vt, 36, kCupFaces,
              sizeof kCupFaces / sizeof kCupFaces[0]);

    /* the two diagnostics, applied after the fill so they override it */
    {
        const char *s0 = std::getenv("SM64DS_SCENE_SLOT0");
        const char *s9 = std::getenv("SM64DS_SCENE_SLOT9");
        if (s0 && s0[0] == '0') vt[0] = (void *)cup_init_noop;
        if (s9 && s9[0] == '0') vt[9] = (void *)cup_render_noop;
    }

    {
        const unsigned lb = cup_raw_left(base, 36);
        const unsigned lm = cup_raw_left(mid, 36);
        const unsigned lv = cup_raw_left(vt, 36);
        if (lb || lm || lv) {
            std::fprintf(stderr, "  [scene] CUP FILL INCOMPLETE: "
                         "dScMgBase_c leaves %u of 36 raw DS words, "
                         "dScMgSingle3DBase_c %u, dScMgCup_c %u. A dispatch "
                         "of any of them jumps to a DS address as a host one.\n",
                         lb, lm, lv);
            std::fflush(stderr);
        }
    }

    /* The constructors, gated on the requested id, exactly as every other
       minigame fill does it. Idempotent behind scene_mg.cpp's own
       once-per-process guard. */
    port_scene_mg_prepare(port_scene_env_want());

    if (port_scene_env_want() == 361) {
        static int armed;
        if (!armed) {
            armed = 1;
            std::atexit(port_scene_cup_hits);
        }
    }
}

/* The registry's factory column is void *(*)(void) and so is the host-copied
   factory; the forwarder is kept for the reason title_spawn,
   port_mg_curling_spawn and port_mg_memory2_spawn are, so the seat has one
   place to observe the object without the registry table growing a second
   column.

   THE FACTORY IS THE HOST COPY AND THAT IS THIS LANE'S DISPLACEMENT RULING.
   scene_mg_memory2.cpp's own forwarder header records the contrast from the
   other side: "THE FACTORY NEEDS NO DISPLACEMENT RULING, and that is worth
   recording because 0x169's did." This is 0x169. func_ov006_020e0574 is
   compiled from port/unmatched/MgCup_Factory.cpp, whose header is the ruling
   being spent; src/func_ov006_020e0574.cpp is out of port/slice_cup.txt. */
static char *g_cup_self;

extern "C" void *port_mg_cup_spawn(void)
{
    void *p = (void *)func_ov006_020e0574();
    g_cup_self = (char *)p;
    return p;
}

extern "C" void port_scene_cup_hits(void)
{
    unsigned total = 0, mtotal = 0;
    for (int i = 0; i < 36; ++i) {
        total  += g_cup_hits[i];
        mtotal += g_cup_base_hits[i];
    }

    std::printf("[scene] dScMgCup_c slot hits: init %u, behavior %u, render %u, "
                "D2 %u, D0 %u, state-reset %u, slot20 %u%s\n",
                g_cup_hits[0], g_cup_hits[6], g_cup_hits[9], g_cup_hits[16],
                g_cup_hits[17], g_cup_hits[18], g_cup_hits[20],
                g_cup_render_skipped
                    ? "  [RENDER SLOT NO-OP'd: SM64DS_SCENE_SLOT9=0]" : "");
    if (g_cup_init_skipped)
        std::printf("[scene] INIT SLOT NO-OP'd: SM64DS_SCENE_SLOT0=0, %u "
                    "time(s)\n", g_cup_init_skipped);

    std::printf("[scene] dScMgCup_c own slots entered:");
    for (int i = 0; i < 36; ++i)
        if (g_cup_hits[i]) std::printf(" %d(x%u)", i, g_cup_hits[i]);
    std::printf("   (%u total)\n", total);

    /* WHAT THIS LINE COUNTS, AND IT IS NOT THE MIDDLE TABLE. kSingle3DFaces is
       applied to TWO tables: data_ov006_0213e448 itself, and the six slots of
       the DERIVED table (2, 5, 7, 10, 26, 33) where this class inherits the
       middle base's body unchanged. On a tree carrying scene 390 the flower's
       row runs first and claims data_ov006_0213e448, so this seat's arrays
       write NOTHING there -- but the derived table is this seat's own, and
       every dispatch through one of its six inherited slots lands here. So a
       NONZERO reading is the expected and correct one, and it is the derived
       table talking. hal/scene_mg_memory2.cpp's copy reports the same shape
       for the same reason (8998 entries on a scene-363 run).

       WHAT WOULD BE WRONG is the flower's OWN census moving. That is checked
       by booting 390 and 363 and comparing their middle-table lines against
       the same runs before this seat existed, which is this lane's proof, not
       something this line can say on its own. */
    std::printf("[scene] dScMgSingle3DBase_c slots entered (this seat's copy):");
    for (int i = 0; i < 36; ++i)
        if (g_cup_base_hits[i]) std::printf(" %d(x%u)", i, g_cup_base_hits[i]);
    std::printf("   (%u total)\n", mtotal);

    /* THE STATE MACHINE'S OWN WITNESS. Printed whether or not it fired: a run
       with slot hits and zero routed dispatches has ticked the object without
       entering its state machine, and that reads as a success unless it is
       said out loud. */
    {
        unsigned calls = 0, unknown = 0, slots[8];
        port_mg_cup_state_slots(slots);
        port_mg_dispatch_counts(&calls, &unknown);
        std::printf("[scene] dScMgCup_c state dispatch: %u routed to one of the "
                    "class's 8 table states, %u bodiless want(s); %u framework "
                    "call(s), %u UNHANDLED address(es)\n",
                    port_mg_cup_state_hits(), port_mg_cup_floor_hits(),
                    calls, unknown);
        std::printf("[scene] dScMgCup_c state slots (data_ov006_02141870):");
        for (int i = 0; i < 8; ++i) std::printf(" %d:%u", i, slots[i]);
        std::printf("\n");
    }

    /* THE SUB-OBJECT DISPATCH, the half section 12 says this class does not
       have. calls counts every entry into func_ov006_020c2b8c, idle counts the
       entries the ROM's own null-pmf guard skipped, routed counts the ones
       that ran a measured animation state, and unknown must stay ZERO. */
    std::printf("[scene] dScMgCup_c sub-object dispatch (+0x4f38, "
                "func_ov006_020c2b8c): %u call(s), %u routed, %u idle "
                "(null pmf), %u UNHANDLED address(es)\n",
                port_mg_sub4f38_calls(), port_mg_sub4f38_routed(),
                port_mg_sub4f38_idle(), port_mg_sub4f38_unknown());

    /* The state index the ROM's own dispatcher reads, at the offset
       disassembled in unmatched/MgCup_StateDispatch.cpp: +0x5418, which
       vtable slot 18 (src/func_ov006_020dfeec.c) zeroes at the same offset,
       spelled there as c + 0x5000 + 0x418. Two independent readers of one
       field is what makes the offset a measurement. */
    if (g_cup_self)
        std::printf("[scene] dScMgCup_c object at %p, state index +0x5418 = %d, "
                    "frame timer +0x541c = %d\n",
                    (void *)g_cup_self,
                    *(int *)(g_cup_self + 0x5418),
                    *(int *)(g_cup_self + 0x541c));

    /* THE THREE ANIMATED CUPS. src/func_ov006_020e0204.cpp's own loop runs
       i = 0..2 over three parallel triples: +0x5434 is the animation-set index
       (the row of the seven-pointer table data_ov006_0213c0d8), +0x5440 is the
       frame within that set and +0x544c is the countdown to the next frame.
       Slot 18 writes all three: set = 3 where the byte at +0x5462+i is
       nonzero and 0 otherwise, frame 0, countdown 0. Printing them is the
       difference between "the shell game ticked" and "the shell game
       animated". */
    if (g_cup_self) {
        std::printf("[scene] dScMgCup_c cups (set/frame/countdown, and the "
                    "+0x5462 flag slot 18 seeds the set from):");
        for (int i = 0; i < 3; ++i)
            std::printf(" [%d] %d/%d/%d f%u", i,
                        *(const int *)(g_cup_self + 0x5434 + i * 4),
                        *(const int *)(g_cup_self + 0x5440 + i * 4),
                        *(const int *)(g_cup_self + 0x544c + i * 4),
                        *(const unsigned char *)(g_cup_self + 0x5462 + i));
        std::printf("\n");
    }

    /* THE SAVE RECORD THIS CLASS IS KEYED TO. Run mg8 lane MMD measured that
       the port used to hand every minigame record 0; the ov005 row for this id
       is row 23, param 0x000b1700, so record 23 and name text 11. Printed
       from the LIVE object rather than from that derivation, so the two can
       disagree out loud if the seam ever stops flowing. */
    if (g_cup_self) {
        const int idx = port_mg_cup_record_index();
        const unsigned param = port_mg_scene_spawn_param(361);
        std::printf("[scene] dScMgCup_c spawn param %08x -> save record %u, "
                    "name text %u, slot-35 byte %u; live record index %d, "
                    "+0x465e name text = %d\n",
                    param, (param >> 8) & 0xffu, (param >> 16) & 0xffu,
                    param & 0xffu, idx,
                    (int)*(const short *)(g_cup_self + 0x465e));
        if (idx >= 0 && idx < 36) {
            const int *rec = (const int *)(data_0209caf4 + idx * 20);
            std::printf("[scene] dScMgCup_c save record data_0209caf4[%d] "
                        "= { %d, %d, %d, %d, %d }\n", idx,
                        rec[0], rec[1], rec[2], rec[3], rec[4]);
        } else {
            std::printf("[scene] dScMgCup_c save record: NO minigame index "
                        "(data_ov004_020beb68 = %p)\n", data_ov004_020beb68);
        }
        std::printf("[scene] dScMgCup_c score fields: +0xa8 = %d, +0xac = %d, "
                    "+0xb4 = %d, +0xbc = %u\n",
                    *(const int *)(g_cup_self + 0xa8),
                    *(const int *)(g_cup_self + 0xac),
                    *(const int *)(g_cup_self + 0xb4),
                    *(const unsigned *)(g_cup_self + 0xbc));
    }

    /* The SpawnInfo cross-check every seat prints: word 0 is the factory and
       word 1 is the id twice. Read out of the mount at run time, so a mount
       that stopped rebasing says so here rather than in a fault. */
    std::printf("[scene] dScMgCup_c SpawnInfo data_ov006_0213c020 word1 = "
                "%08x (the ROM's 0x01690169)\n",
                *(const unsigned *)(data_ov006_0213c020 + 4));

    std::fflush(stdout);
}
