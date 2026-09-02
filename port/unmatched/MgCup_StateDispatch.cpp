// PORT_HOST_ABI. The mwcc POINTER-TO-MEMBER WALL, per-class half:
// dScMgCup_c's Behavior and its eight state addresses. Actor id 0x169,
// scene 361. Run mg9, lane CUP.
//
// Read unmatched/MgBase_StateDispatch.cpp's header first -- it carries the ROM
// disassembly of the mwcc dispatch sequence, the finding that the overlay
// constructors already copy the pairs at the right eight-byte stride, and the
// rulebook corollary about spellings. Then unmatched/MgCoin_StateDispatch.cpp,
// which is the template this file follows. Neither is repeated here.
//
// ---- THE EIGHT ADDRESSES, AND WHERE THEY COME FROM -------------------------
//
//     python port/tools/stategen.py --sinit src/__sinit_ov006_021303d0.c \
//            --overlay 6
//
//     ov006: 1 table(s), 8 slot(s), 8 distinct state address(es)
//     data_ov006_02141870  n 8  arity 0
//       slot 0  020dfd48   slot 1  020dfcd8   slot 2  020df5b8   slot 3  020df540
//       slot 4  020df3bc   slot 5  020df28c   slot 6  020df1c0   slot 7  020df1bc
//     REFUSALS: none
//
// Every one of the eight has a matched src TU and all eight are in
// port/slice_cup.txt, so this class has NO state floor -- the thing
// port/mg_fanout_costs.txt section 12 calls "the cheap half for this class".
// Curling has seven tables, twenty-five slots and one state with no decompiled
// body at all; this has one table, eight slots and none.
//
// THE MACHINE IS ONE LEVEL DEEP, WHICH IS THE CHECK SECTION 14 PRESCRIBES.
// dScMgPanel_c's cost was ten host copies rather than five because five of its
// eight top-level states were THEMSELVES dispatchers of a second table. The
// test is "is any state address also in the dispatcher list", and here it is
// not: none of the eight state TUs contains a member-pointer dispatch, a `::*`
// token or a second stategen table. One table, one dispatcher.
//
// ---- ARITY 0, READ OFF THE ROM RATHER THAN OFF THE TABLE -------------------
//
// func_ov006_020e0204's own bytes, disassembled out of
// extracted/overlays/overlay_0006.bin at base 0x020bfec0:
//
//     020e0204  push {r4,r5,r6,r7,r8,sb,sl,lr}
//     020e0208  mov  r6, r0             r6 = this, and it never moves
//     020e020c  add  r0, r6, #0x5000
//     020e0210  ldr  r0, [r0, #0x418]   the state index, this+0x5418
//     020e0214  ldr  r1, [pc, #0xd0]    data_ov006_02141870
//     020e0218  add  r3, r1, r0, lsl #3 stride EIGHT, the mwcc pair
//     020e021c  ldr  r1, [r3, #4]       the adjustment
//     020e0220  add  r0, r6, r1, asr #1 this, advanced
//     020e0224  ands r1, r1, #1         the virtual bit
//     020e0228  ldrne r2,[r0] / ldrne r1,[r3] / ldrne r1,[r2,r1]
//     020e0234  ldreq r1, [r3]          the code word
//     020e0238  blx  r1
//
// NOTHING WRITES r1 AS AN ARGUMENT, so the callee is passed the advanced
// `this` and nothing else. That is what "arity 0" means here, and it agrees
// with all eight state TUs' own signatures: seven take one `char *` receiver
// and func_ov006_020df1bc takes (void) because its ROM body is four bytes,
// a bare `bx lr`.
//
// THE STATE INDEX IS AT +0x5418 and it is cross-confirmed: vtable slot 18,
// src/func_ov006_020dfeec.c, is the state reset and zeroes the same offset.
// hal/scene_mg_cup.cpp's census prints it at exit.
//
// EVERY ADJUSTMENT WORD IS ZERO. All eight pairs were read out of the overlay
// image, not out of a running table: __sinit_ov006_021303d0 names them, and
// each pair's second word reads 0. So `v >> 1` is 0, the advanced `this` is
// the object itself, and the virtual arm is never taken. The virtual arm is
// therefore REPORTED rather than implemented below, for MgBase_StateDispatch's
// reason -- a dispatch shape nobody has measured is exactly the plausible body
// port/tools/inferred_stub_guard.py exists to refuse.
//
// ---- WHY THIS FILE HAS ITS OWN ENTRY POINT ---------------------------------
//
// unmatched/MgBase_StateDispatch.cpp owns port_mg_call0 / port_mg_call1 and
// chains them to exactly ONE per-class pair, port_mg_try_ov006_0 and _1, which
// unmatched/MgCurling_StateDispatch.cpp defines. A second class cannot define
// those names. port/slice_mga361.txt flagged that singleton in run link60 and
// lane FLW hit it again in run mg5; this is at least the fourth lane to meet
// it and it is still not this lane's to refactor across two other lanes'
// files.
//
// So port_mg_cup_call0 tries THIS class's switch and hands everything else to
// the framework's entry point unchanged. The framework stays the single place
// that decides what a null code word means, what a nonzero adjustment means
// and how an UNHANDLED address is reported. Only one link is added in front.
//
// IT DEFINES func_ov006_020e0204, so src/func_ov006_020e0204.cpp is OUT of
// port/slice_cup.txt. That is one linked function traded for a working
// dispatch, the trade MG2 made five times for curling.
//
// THIS FILE IS NOT THE WHOLE OF THIS CLASS'S WALL. There is a SECOND
// dispatcher, func_ov006_020c2b8c, reached from the tail of the Behavior
// below; unmatched/MgShared4f38_SubDispatch.cpp carries it and its derivation.
// port/mg_fanout_costs.txt section 12 says 0x020e0204 is "the class ONLY
// pointer-to-member dispatch site" and that is wrong -- see that file.

#include <cstdio>

/* The eight-byte mwcc member pointer, in the only spelling that is true on
   both machines: two words, no member-pointer type anywhere. */
struct MgPmf { unsigned code; int adj; };

extern "C" {

/* the framework's entry point; see MgBase_StateDispatch.cpp */
void port_mg_call0(void *self, unsigned code, int adj);

/* ---- the eight matched state bodies, in table-slot order ------------------
   Each is declared with the parameter list ITS OWN src TU defines, so a
   receiver is passed the way the ROM passes it rather than the way the slot's
   arity would suggest. func_ov006_020df1bc is the (void) one: its ROM body is
   0x4 bytes, `bx lr`, and its src is an empty function. */
void func_ov006_020dfd48(char *c);      /* slot 0 */
void func_ov006_020dfcd8(char *o);      /* slot 1 */
void func_ov006_020df5b8(char *c);      /* slot 2 */
void func_ov006_020df540(char *c);      /* slot 3 */
void func_ov006_020df3bc(char *c);      /* slot 4 */
void func_ov006_020df28c(char *self);   /* slot 5 */
void func_ov006_020df1c0(char *c);      /* slot 6 */
void func_ov006_020df1bc(void);         /* slot 7, the empty state */

/* the mount table, re-typed to the ROM's eight-byte pair. The mount defines
   the storage; __sinit_ov006_021303d0 fills it at overlay-constructor time. */
extern MgPmf data_ov006_02141870[];

/* the ordinary callees the host copy below keeps, each spelled as its own src
   TU spells it */
void func_ov006_020dedfc(char *r7, int r6, int r5, int r4);
void func_ov006_020debfc(char *c);
void func_ov006_020c2b8c(char *c);      /* host copy, MgShared4f38_SubDispatch.cpp */

/* src/func_ov006_020e0204.cpp's own animation-frame table: an array of
   POINTERS to {advance, hold} pairs. All seven words carry a relocation in
   config/arm9/overlays/ov006/relocs.txt (0x0213c0d8..0x0213c0f0 -> 0x02139db4
   .. 0x0213a274), so the mount rebases every one of them and the double
   subscript below dereferences host storage, not a DS address. */
struct MgCupFrame { int a, b; };
extern MgCupFrame *data_ov006_0213c0d8[];

/* this file's own definition, and the two witnesses hal/scene_mg_cup.cpp
   prints. func_ov006_020e0204 is DEFINED at the foot of this file with the
   signature src/func_ov006_020e0204.cpp defines, `int (char *)`. */
int      func_ov006_020e0204(char *o);
void     port_mg_cup_call0(void *self, unsigned code, int adj);
unsigned port_mg_cup_state_hits(void);
unsigned port_mg_cup_floor_hits(void);
void     port_mg_cup_state_slots(unsigned *out8);

}  /* extern "C" */

// ---- the class's address switch --------------------------------------------

static unsigned g_cup_state_hits;
/* THE BODILESS-STATE COUNTER, KEPT AND STRUCTURALLY ZERO. Every one of the
   eight addresses below reaches a real matched symbol, so nothing increments
   this and hal/scene_mg_cup.cpp reports 0 for it. It is kept rather than
   deleted so the census field keeps its meaning: if a later lane adds a state
   this class cannot reach, this is where it is counted, and a nonzero reading
   is then a regression rather than a new field. */
static unsigned g_cup_floor_hits;
/* per-slot, so a run can say WHICH states the machine visited rather than only
   how many times it dispatched. Indexed by the switch, not by the table. */
static unsigned g_cup_state_slot[8];

static int cup_try_0(void *self, unsigned code)
{
    char *c = (char *)self;
    switch (code) {
    case 0x020dfd48u: ++g_cup_state_slot[0]; func_ov006_020dfd48(c); return 1;
    case 0x020dfcd8u: ++g_cup_state_slot[1]; func_ov006_020dfcd8(c); return 1;
    case 0x020df5b8u: ++g_cup_state_slot[2]; func_ov006_020df5b8(c); return 1;
    case 0x020df540u: ++g_cup_state_slot[3]; func_ov006_020df540(c); return 1;
    case 0x020df3bcu: ++g_cup_state_slot[4]; func_ov006_020df3bc(c); return 1;
    case 0x020df28cu: ++g_cup_state_slot[5]; func_ov006_020df28c(c); return 1;
    case 0x020df1c0u: ++g_cup_state_slot[6]; func_ov006_020df1c0(c); return 1;
    /* the empty state: `bx lr` in the ROM, `{}` in src, and its src signature
       is (void). Called with no argument for that reason and not because the
       receiver was dropped -- there is no body to drop it from. */
    case 0x020df1bcu: ++g_cup_state_slot[7]; func_ov006_020df1bc();  return 1;
    default:                                                         return 0;
    }
}

/* The one entry point the host copy below uses. Everything this switch does not
   own goes to the framework unchanged. */
extern "C" void port_mg_cup_call0(void *self, unsigned code, int adj)
{
    if (code != 0 && adj == 0 && cup_try_0(self, code)) {
        ++g_cup_state_hits;
        return;
    }
    port_mg_call0(self, code, adj);
}

extern "C" unsigned port_mg_cup_state_hits(void) { return g_cup_state_hits; }
extern "C" unsigned port_mg_cup_floor_hits(void) { return g_cup_floor_hits; }

extern "C" void port_mg_cup_state_slots(unsigned *out8)
{
    for (int i = 0; i < 8; ++i) out8[i] = g_cup_state_slot[i];
}

// ---- the host copy ---------------------------------------------------------
//
// src/func_ov006_020e0204.cpp verbatim except for the table declaration (MgPmf
// rather than a member-pointer type) and the dispatch site
// (port_mg_cup_call0 rather than `(c->*data_ov006_02141870[idx])()`). Nothing
// else moves: the +0x5418 index read, the three-iteration animation loop over
// data_ov006_0213c0d8 with its 0x5434 / 0x5440 / 0x544c offsets, the nonzero
// guard, the counter increment and wrap, func_ov006_020dedfc, and the two tail
// calls are all src's.

// PORT_HOST_ABI: dScMgCup_c vtable slot 6 Behavior table dispatcher for data_ov006_02141870; the mwcc eight-byte member-pointer table MSVC's four-byte pmf cannot stride, so the host strides the {code, adj} pairs directly.
extern "C" int func_ov006_020e0204(char *o)
{
    int i;

    {
        const MgPmf *e = &data_ov006_02141870[*(int *)(o + 0x5418)];
        port_mg_cup_call0(o, e->code, e->adj);
    }

    for (i = 0; i < 3; i++) {
        MgCupFrame *f = &data_ov006_0213c0d8[*(int *)(o + i * 4 + 0x5434)]
                                            [*(int *)(o + i * 4 + 0x5440)];
        int n = f->b;
        if (n != 0) {
            *(int *)(o + i * 4 + 0x544c) += 1;
            if (*(int *)(o + i * 4 + 0x544c) >= n) {
                *(int *)(o + i * 4 + 0x544c) = 0;
                *(int *)(o + i * 4 + 0x5440) += 1;
                func_ov006_020dedfc(o, *(int *)(o + i * 4 + 0x5434),
                                    *(int *)(o + i * 4 + 0x5440), i);
            }
        }
    }

    func_ov006_020debfc(o + 0x50e8);
    func_ov006_020c2b8c(o + 0x4f38);
    return 1;
}
