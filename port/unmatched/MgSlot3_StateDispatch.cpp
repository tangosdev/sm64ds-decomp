// PORT_HOST_ABI. The mwcc POINTER-TO-MEMBER WALL, per-class half:
// dScMgSlot3_c's ONE table dispatcher and its eight state addresses.
// Run mg11, lane MUG. Actor id 0x16d, scene 365, "Mix-a-Mug".
//
// Read unmatched/MgBase_StateDispatch.cpp's header first: it carries the ROM
// disassembly of the mwcc dispatch sequence, the finding that the overlay
// constructors already copy the pairs at the right stride, and the framework's
// null-code guard, nonzero-adjustment refusal and UNHANDLED report. None of
// that is repeated here. unmatched/MgCup_StateDispatch.cpp is the template
// this file follows: one table, arity 0, a per-slot counter array.
//
// ---- 1. THE TABLE IS THIS CLASS'S, AND THE PROOF IS THE DISPATCH SITE ------
//
// port/mg_fanout_costs.txt section 9 and run mg9 lane LKY both record the same
// trap: the pair symbols of several ov006 classes are INTERLEAVED in address
// order, so attributing a table to the class whose code block it sits nearest
// hands a lane another class's dispatchers. This table's eight pair symbols
// (0x0213e4e0, 0x0213e520, 0x0213e530, 0x0213e538, 0x0213e548, 0x0213e550,
// 0x0213e578, 0x0213e580) are scattered through a run that ALSO contains
// data_ov006_0213e508 -- this class's own SpawnInfo, {0x0210c120, 0x016d016d}
// -- and data_ov006_0213e560, which is id 0x16c's SpawnInfo and belongs to
// dScMgSlot1_c, the SEATED scene-364 class. A sweep of the address range would
// have taken both and a factory word.
//
// TWO INDEPENDENT WITNESSES SAY THE TABLE IS dScMgSlot3_c's, and neither is
// adjacency:
//
//   THE CONSTRUCTOR. src/__sinit_ov006_02132894.c assigns exactly these eight
//     pairs into data_ov006_02142bdc[0..7], in this order:
//
//       [0] 0x0213e520 -> 0x0210b1fc      [4] 0x0213e550 -> 0x0210ac3c
//       [1] 0x0213e530 -> 0x0210af64      [5] 0x0213e548 -> 0x0210ac38
//       [2] 0x0213e4e0 -> 0x0210adac      [6] 0x0213e580 -> 0x0210ab94
//       [3] 0x0213e538 -> 0x0210ac3c      [7] 0x0213e578 -> 0x0210ab90
//
//     The constructor's own assignment list is the slot order. The pair symbols
//     are NOT in address order and slots 3 and 4 hold the SAME body, so a lane
//     that sorted the symbols would have produced a different, wrong table.
//
//   THE DISPATCH SITE. config/arm9/overlays/ov006/relocs.txt carries exactly
//     TWO relocations naming 0x02142bdc in the whole overlay: the constructor's
//     own, at 0x0213296c, and one literal-pool load at 0x0210bd94 -- which is
//     inside func_ov006_0210bcb0, this class's vtable SLOT 6. No other code in
//     ov006 names the table. "An offset match is a candidate; a blx on the
//     loaded word is the finding" (mg9 section 19), and the blx is at
//     0x0210bce4.
//
// ---- 2. THE ROM'S OWN DISPATCH SHAPE, DISASSEMBLED ------------------------
//
// Read out of extracted/overlays/overlay_0006.bin at base 0x020bfec0, not from
// src, because the index offset and the `this` a callee is handed are the two
// things a wrong host copy gets silently wrong. func_ov006_0210bcb0 is 0x100 =
// 64 words, 57 instructions plus a seven-word literal pool.
//
//     0210bcb8  add   r0, r4, #0x5000
//     0210bcbc  ldr   r0, [r0]              the state index at +0x5000
//     0210bcc0  ldr   r1, [pc, #0xcc]       = 0x02142bdc
//     0210bcc4  add   r3, r1, r0, lsl #3    STRIDE EIGHT
//     0210bcc8  ldr   r1, [r3, #4]          the ADJUSTMENT
//     0210bccc  add   r0, r4, r1, asr #1    this, advanced
//     0210bcd0  ands  r1, r1, #1            the virtual bit, in the LSB
//     0210bcd4  ldrne r2, [r0]
//     0210bcd8  ldrne r1, [r3]
//     0210bcdc  ldrne r1, [r2, r1]
//     0210bce0  ldreq r1, [r3]              the CODE word
//     0210bce4  blx   r1
//
// NOTHING SETS r1 BEFORE THE blx, so the arity is 0 and `this` is the CLASS
// BASE (r4 is the incoming r0, untouched). port/tools/stategen.py run on this
// constructor derives the same arity and the same eight rows.
//
// THE WHOLE CLASS HAS EXACTLY ONE PMF SITE. Every word of this class's code
// block 0x0210a954..0x0210c180 was decoded and matched against the ARM Itanium
// signature -- `add Rd,Rn,Rm,asr #1`, `ands Rx,Rm,#1` and the eight-byte
// `add Rd,Rn,Rm,lsl #3` stride -- which is the detector run mg9 lane LKY
// invented for the third shape, the open-coded-in-plain-ints one that neither
// a link nor a `::*` source sweep can see. THREE hits, all three inside
// func_ov006_0210bcb0 and all three part of the sequence above. So there is no
// second dispatcher, no field-held member pointer and no open-coded site in
// this class, and that is a measurement rather than an absence of evidence.
//
// ---- 3. SLOT 2 WAS THE CLASS'S ONE FLOOR. IT IS RETIRED --------------------
//
// RUN mg12, LANE MAM MATCHED IT. src/func_ov006_0210adac.c byte-reproduces
// 0x0210adac under mwccarm 2004/b56 (and 1.2/base, 1.2/sp2, 1.2/sp2p3), it has
// a delink block in config/arm9/overlays/ov006/delinks.txt covering
// 0x0210adac..0x0210af64, linkcheck reports VERIFIED with zero blind slots, and
// port/slice_mug.txt lists it, so the switch below CALLS it. Everything from
// here to the end of this section is the mg11 seat lane's reading of the state,
// kept because it is what the match was checked against; the paragraphs that
// said "no body" are rewritten, not deleted, so the two runs can be compared.
//
// WHAT IT WAS. 0x0210adac had a config symbol (`kind:function(arm,size=0x1b8)`)
// and neither a delink block nor a src file in any extension. It was the ONLY
// body in this class's code block without one, and port/mg_fanout_costs.txt
// section 3's nosrc column reads 0 for this row because that column is computed
// over the VTABLE and this address is in a STATE table. A seat lane that
// trusted the column would not have looked.
//
// WHAT IT COSTS THE PLAYER, derived rather than guessed. The state index at
// +0x5000 moves like this:
//
//   0 -> 1   src/func_ov006_0210b1fc.c's tail (`*q += 1`), after the 0x50-frame
//            open and the reel-lights setup
//   1 -> 2   src/func_ov006_0210af64.c's tail: once the third reel has been
//            stopped (`*(u8 *)(c + 0x503d) >= 3`) it sets +0x503e to 0x1e and
//            writes 2
//   3 / 4    src/func_ov006_0210ac3c.c, the payout and the round reset, which
//            dispatches vtable slot 18 with the argument 4
//   6 -> 7   src/func_ov006_0210ab94.c's countdown
//
// So state 2 is the FACE-EVALUATION step between "all three reels stopped" and
// "the payout is tallied": the ROM body reads the three visible symbols out of
// +0x5031, turns each reel's angle into a row, and decides what the player won.
// (mg11 read the divisor as 5 off the literal pool at 0x0210af60. The pool word
// 0x66666667 IS the signed divide-by-five magic, but the shift that follows the
// smull is `asr #5`, not `asr #1`, so the divisor is 5 << 4 = 80. The matched
// source divides by 80 and the byte gate agrees.) While it had no
// body a Mix-a-Mug round on this port played up to the third reel stopping and
// then stopped advancing. NOTHING IN AN UNATTENDED BOOT REACHED IT -- stopping
// a reel takes a stylus tap -- so the mg11 boot proofs in port/slice_mug.txt
// were clean for the honest reason and not because the hole was small.
//
// The matched body confirms that reading line for line: it copies the three
// visible faces out of +0x5031 into a three-byte local, and for each row i it
// walks reels 1 and 2 (j = 1, 2) asking whether the same face id sits at
// ((i + (reel j's angle at +0x4fe4 + 4*j >> 12) / 80) % the face count at
// +0x503a) of reel j's five-byte strip at +0x501c + 5*j. A row that disagrees
// on either reel has its face replaced with the sentinel 5, so it can only pay
// if the paying face at +0x503b is itself 5. The first row that still matches
// +0x503b wins and the scan stops there: row 1 pays 6, rows 0 and 2 pay 3, the
// winning row index goes to +0x5010 and the payout to +0x5014.
//
// WHILE IT WAS A FLOOR IT WAS HANDLED HERE RATHER THAN LEFT TO THE FRAMEWORK,
// and the difference mattered to a reader of a run log. Falling through would
// have reached MgBase_StateDispatch.cpp's mg_unhandled and printed "UNHANDLED",
// which is the report for an address nobody has accounted for. The UNHANDLED
// counter keeps meaning "a dispatch this port cannot explain".
//
// HOW THE MATCH WAS GOT, because the source carries one token that looks like
// noise and is not. nearmiss/db.jsonl line 45 (run `fable-hardmatch-20260801`)
// held a 101-of-110 candidate whose one divergence was that the ROM initialises
// the strength-reduced row base INSIDE the outer loop (`add r6,sl,#5`) where
// mwcc hoists it into the prologue and spills it. All 25 installed mwccarm
// builds hoist it; 60 opt pragmas, every declaration order, the whole flag
// range and 400-odd source shapes do not move it. A redundant `(int)` cast on
// the left operand of the sum inside the modulo does: it changes the expression
// tree mwcc builds and the initialiser stays put. That cast is load-bearing and
// src/func_ov006_0210adac.c says so at the top. The near-miss row was retired
// with the match.
//
// IT HAS TWO EXITS AND THIS PARAGRAPH USED TO NAME ONLY ONE. On the PAY path
// (total > 0) it writes the payout to +0x5014, sets the state index to 3,
// plays sound 0x26 and holds +0x503e for 0x28 frames. On the NO-WIN path it
// sets the state index to 4, plays 0xe and holds for 0x50. The ROM says it
// first-hand as well -- the payout store to +0x5014 at 0x0210aef8, then
// `mov r2,#3` at 0x0210aefc / `mov r0,#0x26` / `str r2,[r1]` with the 0x28
// strb at 0x0210af14, against `mov r2,#4` at 0x0210af24 / `mov r0,#0xe` /
// `str r2,[r1]` with the 0x50 strb at 0x0210af3c -- so the two exits are the
// disassembly's finding and the candidate's agreement with it, not the
// candidate taken on trust.
//
// BOTH EXITS LAND ON THE SAME BODY, which is the detail that makes the table
// make sense: slots 3 and 4 of data_ov006_02142bdc are BOTH
// func_ov006_0210ac3c, so one payout state serves a win and a loss and tells
// them apart by the index it was entered on. It is also why the per-slot
// counter in this file charges slot 3 for both, and why the switch below has
// one arm for the two slots.
//
// NO BODY WAS EVER INVENTED FOR IT, and none is now: the file the switch calls
// is a byte match, not a transcription. The mg11 adjudication arithmetic
// (108 instructions plus a two-word pool = 110 = 0x1b8/4) and the five callees
// (DecIfAbove0_Byte 0x0203add4, the ITCM divide helper 0x01ffabe4,
// func_02012790 twice, func_ov004_020adb1c) held exactly: the matched object is
// 0x1b8 bytes with the same two pool words and the same five relocations.
//
// ---- 4. ALL EIGHT NOW HAVE MATCHED src TUs --------------------------------
//
// All eight resolve to a src file with a delink block, and none carries a
// NONMATCHING banner. Two of them -- 0x0210ab90 and 0x0210ab94's neighbour
// 0x0210ac38 -- are four-byte `bx lr` bodies whose src spells `(void)`, which
// is checked against the ROM here rather than inferred from the src: both are
// literally the single instruction 0xe12fff1e. Calling them with no argument
// is the faithful host form for unmatched/MgCup_StateDispatch.cpp's reason --
// there is no body for a dropped receiver to be dropped from.

#include <cstdio>

/* The eight-byte mwcc member pointer, in the only spelling that is true on both
   machines: two words, no member-pointer type anywhere. */
struct MgPmf { unsigned code; int adj; };

extern "C" {

/* the framework's entry point; see MgBase_StateDispatch.cpp */
void port_mg_call0(void *self, unsigned code, int adj);

/* the eight routed state bodies, each declared with the parameter list ITS OWN
   src TU defines */
void func_ov006_0210b1fc(char *p);            /* slot 0 */
void func_ov006_0210af64(char *c);            /* slot 1 */
void func_ov006_0210adac(char *c);            /* slot 2 */
void func_ov006_0210ac3c(char *c);            /* slots 3 and 4 */
void func_ov006_0210ac38(void);               /* slot 5, bx lr body */
void func_ov006_0210ab94(char *c);            /* slot 6 */
void func_ov006_0210ab90(void);               /* slot 7, bx lr body */

/* the mount table this file dispatches, re-typed to the ROM's eight-byte pair.
   The ov006 mount defines the storage; __sinit_ov006_02132894 fills it at
   minigame scene load. */
extern MgPmf data_ov006_02142bdc[];

/* the two ordinary callees the host copy below keeps, spelled as their own src
   TUs spell them */
void func_ov006_020c2144(void *a);
int  RandomIntInternal(int *seed);
extern int          data_0209e650;
extern unsigned char data_0209d45c;

}  /* extern "C" */

// ---- the class's address switch --------------------------------------------

static unsigned g_slot3_state_hits;
/* THE FORMER BODILESS-STATE COUNTER. Run mg12 lane MAM matched slot 2's body,
   so this reads 0 for a structural reason now: there is no bodiless state left
   in this class. It is kept, and kept printed, because the census line and the
   slice's banked runs are written in terms of it -- a reader comparing an
   mg11 log with an mg12 log needs the same field to go 598 -> 0. */
static unsigned g_slot3_floor_hits;
/* per-slot, so a run can say WHICH states the machine visited rather than only
   how many times it dispatched. Indexed by the TABLE slot, which is the
   constructor's order and not the address order. */
static unsigned g_slot3_state_slot[8];

static int slot3_try_0(void *self, unsigned code)
{
    char *c = (char *)self;
    switch (code) {
    case 0x0210b1fcu: ++g_slot3_state_slot[0]; func_ov006_0210b1fc(c); return 1;
    case 0x0210af64u: ++g_slot3_state_slot[1]; func_ov006_0210af64(c); return 1;
    /* slot 2 -- the face evaluation, matched by run mg12 lane MAM; section 3 */
    case 0x0210adacu: ++g_slot3_state_slot[2]; func_ov006_0210adac(c); return 1;
    /* slots 3 AND 4 are the same body; the per-slot counter cannot tell them
       apart from the code word alone, so this arm charges slot 3 and the
       census says so rather than implying two distinct states ran. */
    case 0x0210ac3cu: ++g_slot3_state_slot[3]; func_ov006_0210ac3c(c); return 1;
    case 0x0210ac38u: ++g_slot3_state_slot[5]; func_ov006_0210ac38();  return 1;
    case 0x0210ab94u: ++g_slot3_state_slot[6]; func_ov006_0210ab94(c); return 1;
    case 0x0210ab90u: ++g_slot3_state_slot[7]; func_ov006_0210ab90();  return 1;
    default:                                                           return 0;
    }
}

/* The one entry point the host copy below uses. Everything this switch does not
   own goes to the framework unchanged, so the null-code guard, the
   nonzero-adjustment refusal and the UNHANDLED report all still live in exactly
   one place. */
extern "C" void port_mg_slot3_call0(void *self, unsigned code, int adj)
{
    if (code != 0 && adj == 0 && slot3_try_0(self, code)) {
        ++g_slot3_state_hits;
        return;
    }
    port_mg_call0(self, code, adj);
}

extern "C" unsigned port_mg_slot3_state_hits(void) { return g_slot3_state_hits; }
extern "C" unsigned port_mg_slot3_floor_hits(void) { return g_slot3_floor_hits; }

extern "C" void port_mg_slot3_state_slots(unsigned *out8)
{
    for (int i = 0; i < 8; ++i) out8[i] = g_slot3_state_slot[i];
}

// ---- the host copy ---------------------------------------------------------
//
// src/func_ov006_0210bcb0.cpp verbatim except for the table declaration (MgPmf
// rather than `void (Obj::*)()`) and the dispatch site (port_mg_slot3_call0
// rather than `(self->*data_ov006_02142bdc[self->idx])()`). Nothing else moves:
// the +0x503f byte increment, the func_ov006_020c2144(self + 0x4f38) blend
// advance, the three-iteration loop whose only body is the `idx == 1` arm that
// walks +0x5018 down by 0x200 and +0x501a down by 0x400 and then breaks, the
// RandomIntInternal bit that sets or clears bit 1 of data_0209d45c, and the
// `return 1`.
//
// THE LOOP IS TRANSCRIBED AS SRC SPELLS IT and that is deliberate. src writes a
// three-iteration loop containing a single `if (idx == 1) { ...; break; }`,
// which the ROM compiles to the cmp/bne at 0x0210bd14..0x0210bd18 with the
// counter at 0x0210bd48. It runs the body at most once and is a no-op
// otherwise. Rewriting it as a plain `if` would be tidier and would stop this
// file being a transcription.

extern "C" int func_ov006_0210bcb0(char *self)
{
    int i;
    unsigned char *pc;
    unsigned short *ph1;
    unsigned short *ph2;
    unsigned char t;

    {
        const int idx = *(const int *)(self + 0x5000);
        const MgPmf p = data_ov006_02142bdc[idx];
        port_mg_slot3_call0(self, p.code, p.adj);
    }

    pc = (unsigned char *)(self + 0x503f);
    *pc = *pc + 1;
    func_ov006_020c2144(self + 0x4f38);

    for (i = 0; i < 3; i++) {
        if (*(const int *)(self + 0x5000) == 1) {
            ph1 = (unsigned short *)(self + 0x5018);
            ph2 = (unsigned short *)(self + 0x501a);
            *ph1 = (unsigned short)(*ph1 - 0x200);
            *ph2 = (unsigned short)(*ph2 - 0x400);
            break;
        }
    }

    t = (unsigned char)(((unsigned int)RandomIntInternal(&data_0209e650) >> 16) & 1);
    if (t)
        data_0209d45c |= 2;
    else
        data_0209d45c &= (unsigned char)~2;

    return 1;
}
