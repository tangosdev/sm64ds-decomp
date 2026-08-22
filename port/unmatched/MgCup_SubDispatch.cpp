// PORT_HOST_ABI. The mwcc POINTER-TO-MEMBER WALL, dScMgCup_c's SECOND half --
// the one that is NOT the class's own state machine and that
// port/mg_fanout_costs.txt section 12 says does not exist. Run mg9, lane CUP.
//
// ---- WHAT SECTION 12 AND slice_mga361 GOT WRONG ----------------------------
//
// port/tools/inferred_stub_adjudicated.txt's row for func_ov006_020e0204 calls
// it "the class ONLY pointer-to-member dispatch site", and
// port/slice_mga361.txt section 4 says "exactly ONE TU dispatches it ...
// Curling needs five host copies; this needs one" and then lists
// src/func_ov006_020c2b8c.cpp as an ORDINARY SLICE LINE.
//
// Both sentences are true of the class's own STATE TABLE and neither is true
// of the class. The Behavior's last act but one is
//
//     func_ov006_020c2b8c(o + 0x4f38);
//
// and that function dispatches a member pointer held in a FIELD of the
// sub-object at +0x4f38. It is section 4's second silent shape -- "FOUR HAVE
// NO PMF GLOBAL AT ALL ... nothing external is emitted for it in any linkage"
// -- so no link would ever have named it. It IS findable, because its src does
// spell `(c->*(c->pmf))()`, which is why this lane ran the `::*` sweep that
// section 4 prescribes over its own closure instead of trusting a clean
// closure probe. Lane MGA published a closure and never wired it, so nothing
// ever ran to convict it.
//
// THE ROM, disassembled out of extracted/overlays/overlay_0006.bin at base
// 0x020bfec0 (size 0x5c from config/arm9/overlays/ov006/symbols.txt):
//
//     020c2b8c  push  {r4, lr}
//     020c2b90  mov   r4, r0
//     020c2b94  bl    #0x20c2be8
//     020c2b98  ldr   r2, [r4]          the CODE word
//     020c2b9c  cmp   r2, #0
//     020c2ba0  beq   #0x20c2bc0        the ROM's own null-pmf guard
//     020c2ba4  ldr   r1, [r4, #4]      the ADJUSTMENT
//     020c2ba8  add   r0, r4, r1, asr #1   this, advanced
//     020c2bac  ands  r1, r1, #1        the virtual bit, in the LSB
//     020c2bb0  ldrne r1, [r0] / ldrne r1, [r1, r2]
//     020c2bb8  ldreq r1, [r4]
//     020c2bbc  blx   r1
//     020c2bc0  add   r0, r4, #0xc8 / bl Animation::Advance
//     020c2bc8  add   r0, r4, #0xdc / bl Animation::Advance
//     020c2bd0  add   r0, r4, #8    / bl BlendModelAnim::Advance
//     020c2bd8  mov   r0, r4        / bl #0x20c2290
//     020c2be0  pop   {r4, lr} / bx lr
//
// which is the five-instruction sequence MgBase_StateDispatch.cpp's header
// disassembles, verbatim, with the null guard in front of it.
//
// WHAT MSVC WOULD HAVE DONE. src spells `struct C { PMF pmf; };` with
// `typedef void (C::*PMF)()`. MSVC's single-inheritance member pointer is FOUR
// bytes and is just the code address, so `c->pmf` reads word 0 of the mounted
// sub-object -- a raw DS code address -- and `(c->*(c->pmf))()` calls it. It
// compiles, it links, and the first frame the Behavior runs it jumps to a DS
// address with eip equal to that address, which is lane FLW's symptom exactly.
//
// ---- THE FOUR ADDRESSES, READ OUT OF THE ROM -------------------------------
//
// Not guessed and not read from a running object. Four matched TUs copy an
// eight-byte {code, adjustment} pair into word 0 of the sub-object, and a
// fifth global is the sentinel a predicate compares against. Every pair below
// is read out of the overlay image AND cross-checked against
// config/arm9/overlays/ov006/relocs.txt, which carries a load relocation from
// each pair's first word to the code it names:
//
//   pair address          words                 written by            reloc row
//   -------------------   -------------------   -------------------   ---------
//   data_ov006_0213adc0   020c24e4  00000000    func_ov006_020c2594   11165
//   data_ov006_0213adc8   020c27c4  00000000    func_ov006_020c2848   11166
//   data_ov006_0213adf0   020c263c  00000000    func_ov006_020c2664   11169
//   data_ov006_0213adf8   020c23a8  00000000    func_ov006_020c2440   11170
//   data_ov006_0213adb8   020c27c4  00000000    NOT a state: the sentinel
//                                               func_ov006_020c2994 tests the
//                                               live pair against
//
// EVERY ADJUSTMENT IS ZERO, the same measurement MgBase_StateDispatch records
// for its own closure, so `v >> 1` is 0, the advanced `this` is the object
// itself, and the virtual branch is never taken. The virtual arm is REPORTED
// rather than implemented below, for that file's reason: a dispatch shape
// nobody has measured is exactly the plausible body
// port/tools/inferred_stub_guard.py exists to refuse.
//
// FOUR DISTINCT CODE WORDS, not five: 0x020c27c4 is both 0213adc8's and the
// sentinel's. All four have matched src TUs and all four are in
// port/slice_cup.txt, so this costs no decomp and creates no floor.
//
// TWO MORE PAIRS SIT IN THE SAME BLOCK AND ARE DELIBERATELY NOT ROUTED.
// data_ov006_0213add0 = {020c22d8, 0} and data_ov006_0213ade0 = {020c26f4, 0}
// are the same shape, and their writers -- func_ov006_020c2300 and
// func_ov006_020c271c -- are in NO part of this class's closure
// (func_ov006_020c271c's only caller in src/ is func_ov006_0211b80c, another
// class's; func_ov006_020c2300 has no caller in src/ at all). So neither word
// can reach this class's sub-object, and adding a case for a body this slice
// does not carry would be a link error, not a safety net. If one of them ever
// shows up here the switch REPORTS it by address, which is the honest answer
// and the one a later lane can act on. Recorded rather than silently omitted.
//
// ---- THE STORED WORD IS DELIBERATELY NOT REWRITTEN -------------------------
//
// The alternative to routing at the dispatch site is the ov085 / ov100
// port_*_states_seat shape: write HOST addresses over the pairs before
// anything copies them. IT WOULD BREAK A PREDICATE HERE, exactly as it would
// for dScMgFlower_c. src/func_ov006_020c2994.c is
//
//     struct P { int x, y; };
//     extern struct P data_ov006_0213adb8;
//     int func_ov006_020c2994(struct P *c) {
//         int ip = 1;  struct P *g = &data_ov006_0213adb8;
//         if (c->x == g->x) { if (c->y != g->y && c->x != 0) ; else ip = 0; }
//         return ip == 0;
//     }
//
// -- an "is this sub-object in the idle state" test, BY VALUE, and
// func_ov006_020c2be8 calls it on the very line before this dispatcher runs.
// Rewriting the stored words would make that comparison ask whether a host
// address equals a DS address, which is false forever, and the animation
// machine would silently never see its own idle state. Routing at the dispatch
// site leaves every reader of the word correct and touches nothing but the one
// call. Same choice as MgCurling_StateDispatch and MgFlower_SubDispatch, and
// made for the sharper of the two reasons.
//
// ---- WHY IT DOES NOT ROUTE THROUGH port_mg_call0 ---------------------------
//
// The same singleton MgFlower_SubDispatch.cpp records: port_mg_try_ov006_0 is
// DEFINED in unmatched/MgCurling_StateDispatch.cpp and DECLARED in
// MgBase_StateDispatch.cpp, so a second class cannot add cases to it. This
// class's TABLE dispatch does chain to the framework, through
// port_mg_cup_call0 in unmatched/MgCup_StateDispatch.cpp; this SUB-object one
// does not, because its universe is a different closure with a different
// sentinel and mixing them would put the flower's and the cup's animation
// states in one switch for no gain. It carries its own adjustment ruling,
// spelled identically, and reports its own counts.
//
// IT DEFINES func_ov006_020c2b8c, so src/func_ov006_020c2b8c.cpp is OUT of
// port/slice_cup.txt. That is the second linked function this seat trades for
// a working dispatch; the first is the Behavior and the third is the factory.

#include <cstdio>

extern "C" {

/* the four state bodies, each declared with the parameter list its own src TU
   defines. All four are matched and all four are in port/slice_cup.txt. */
void func_ov006_020c23a8(void *c);
void func_ov006_020c24e4(void *c);
int  func_ov006_020c263c(char *t);
void func_ov006_020c27c4(char *c);

/* the rest of the dispatching TU, verbatim from src */
void func_ov006_020c2be8(char *c);
void func_ov006_020c2290(char *c);
void _ZN9Animation7AdvanceEv(void *anim);
void _ZN14BlendModelAnim7AdvanceEv(void *anim);

void     func_ov006_020c2b8c(char *c);
unsigned port_mg_cup_sub_calls(void);
unsigned port_mg_cup_sub_routed(void);
unsigned port_mg_cup_sub_idle(void);
unsigned port_mg_cup_sub_unknown(void);

}  /* extern "C" */

static unsigned g_sub_calls;     /* every entry, including the null-pmf one */
static unsigned g_sub_routed;    /* entries that ran a measured state        */
static unsigned g_sub_idle;      /* entries the ROM's own null guard skipped */
static unsigned g_sub_unknown;

extern "C" unsigned port_mg_cup_sub_calls(void)   { return g_sub_calls; }
extern "C" unsigned port_mg_cup_sub_routed(void)  { return g_sub_routed; }
extern "C" unsigned port_mg_cup_sub_idle(void)    { return g_sub_idle; }
extern "C" unsigned port_mg_cup_sub_unknown(void) { return g_sub_unknown; }

static void sub_unhandled(const char *why, unsigned code, int adj)
{
    static int said;
    ++g_sub_unknown;
    if (!said) {
        said = 1;
        std::fprintf(stderr, "  [scene] dScMgCup_c SUB-OBJECT STATE %s: "
                     "code %08x adj %d. No state was run. "
                     "port/unmatched/MgCup_SubDispatch.cpp\n", why, code, adj);
        std::fflush(stderr);
    }
}

/* The address switch. Four measured cases and no default body: an address this
   does not know is REPORTED, never guessed at, which is the whole difference
   between this and the raw `blx r1` it replaces. */
static void sub_call(void *p, unsigned code, int adj)
{
    if (adj != 0) {
        sub_unhandled("with a NONZERO ADJUSTMENT, which no measured pair in "
                      "this closure has", code, adj);
        return;
    }
    switch (code) {
    case 0x020c23a8u: func_ov006_020c23a8(p);            ++g_sub_routed; return;
    case 0x020c24e4u: func_ov006_020c24e4(p);            ++g_sub_routed; return;
    case 0x020c263cu: func_ov006_020c263c((char *)p);    ++g_sub_routed; return;
    case 0x020c27c4u: func_ov006_020c27c4((char *)p);    ++g_sub_routed; return;
    default: break;
    }
    sub_unhandled("UNHANDLED -- if it is 020c22d8 or 020c26f4 read this file's "
                  "header, those two pairs have no writer in this class's "
                  "closure and are deliberately unrouted", code, adj);
}

/* src/func_ov006_020c2b8c.cpp, verbatim except that the member-pointer read and
   the indirect call become the two-word read and sub_call. The null guard, the
   call order and the three Advance offsets are all src's and all agree with the
   ROM disassembly in the header. */
extern "C" void func_ov006_020c2b8c(char *c)
{
    ++g_sub_calls;

    func_ov006_020c2be8(c);

    {
        const unsigned code = *(const unsigned *)(c + 0);
        const int      adj  = *(const int *)(c + 4);
        if (code == 0) {
            /* the ROM's own `cmp r2,#0 / beq`, which src spells `if (c->pmf)` */
            ++g_sub_idle;
        } else if (adj & 1) {
            /* The virtual arm. The ROM reads the vtable at [r0] and indexes it
               by the code word as a BYTE OFFSET. No pair in this closure sets
               the bit, so there is nothing to verify an implementation against
               and it reports instead. */
            sub_unhandled("through the VIRTUAL arm of the member pointer, which "
                          "no measured pair in this closure uses", code, adj);
        } else {
            sub_call(c + (adj >> 1), code, adj);
        }
    }

    _ZN9Animation7AdvanceEv(c + 0xc8);
    _ZN9Animation7AdvanceEv(c + 0xdc);
    _ZN14BlendModelAnim7AdvanceEv(c + 8);
    func_ov006_020c2290(c);
}
