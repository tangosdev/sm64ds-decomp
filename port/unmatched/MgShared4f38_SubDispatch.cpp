// PORT_HOST_ABI. THE SHARED +0x4f38 SUB-OBJECT'S FIELD-HELD POINTER-TO-MEMBER
// DISPATCH, host-copied ONCE for both classes that mount it.
//
// Run mg9, adjudicated at the merge. This file is the single canonical copy of
// func_ov006_020c2b8c. Lane CUP (dScMgCup_c, actor 0x169, scene 361) and lane
// BOX (dScMgSound_c, actor 0x16f, scene 367) each host-copied this body into a
// lane-named file without being able to see the other; the merged tree would
// have defined the symbol twice. Both lanes' repairs are carried below and the
// attribution is in runs/mg9/out/ADJUDICATE/canonical.md.
//
// THE BODY IS SHARED, AND THAT IS A ROM FACT, not a merge convenience.
// config/arm9/overlays/ov006/relocs.txt carries EXACTLY TWO arm_calls to
// 0x020c2b8c and they are one per class:
//
//     from:0x020e02dc  inside func_ov006_020e0204   dScMgCup_c   slot 6
//     from:0x0211c960  inside func_ov006_0211c720   dScMgSound_c slot 6
//
// The whole +0x4f38 family is dual-hosted the same way -- 020c2924, 020c29dc
// and this body all have exactly one caller in 0x020e0xxx and one in
// 0x0211cxxx -- which is why three files, not one, came out of the merge as
// duplicate definitions.
//
// ---- WHAT SECTION 12 AND slice_mga361 GOT WRONG ----------------------------
//
// port/tools/inferred_stub_adjudicated.txt's row for func_ov006_020e0204 calls
// it "the class ONLY pointer-to-member dispatch site", and
// port/slice_mga361.txt section 4 says "exactly ONE TU dispatches it" and then
// lists src/func_ov006_020c2b8c.cpp as an ORDINARY SLICE LINE.
//
// Both sentences are true of a class's own STATE TABLE and neither is true of
// the class. Each Behavior's last act but one is
//
//     func_ov006_020c2b8c(o + 0x4f38);
//
// and that function dispatches a member pointer held in a FIELD of the
// sub-object at +0x4f38. It is section 4's second silent shape -- "FOUR HAVE
// NO PMF GLOBAL AT ALL ... nothing external is emitted for it in any linkage"
// -- so no link would ever have named it. It IS findable, because its src does
// spell `(c->*(c->pmf))()`, which is why both lanes ran the `::*` sweep that
// section 4 prescribes over their own closures instead of trusting a clean
// closure probe. Lane MGA published a closure and never wired it, so nothing
// ever ran to convict it.
//
// THE ROM, disassembled out of extracted/overlays/overlay_0006.bin at base
// 0x020bfec0 (size 0x5c from config/arm9/overlays/ov006/symbols.txt). Read
// first-hand at adjudication after checking the reader on a body neither lane
// wrote (func_ov006_0210a600 = `mov r0,#1 / bx lr`, which
// port/tools/inferred_stub_adjudicated.txt already records):
//
//     020c2b8c  e92d4010  push  {r4, lr}
//     020c2b90  e1a04000  mov   r4, r0
//     020c2b94  eb000013  bl    #0x20c2be8
//     020c2b98  e5942000  ldr   r2, [r4]          the CODE word
//     020c2b9c  e3520000  cmp   r2, #0
//     020c2ba0  0a000006  beq   #0x20c2bc0        the ROM's own null-pmf guard
//     020c2ba4  e5941004  ldr   r1, [r4, #4]      the ADJUSTMENT
//     020c2ba8  e08400c1  add   r0, r4, r1, asr #1   this, advanced
//     020c2bac  e2111001  ands  r1, r1, #1        the virtual bit, in the LSB
//     020c2bb0  15901000  ldrne r1, [r0]
//     020c2bb4  17911002  ldrne r1, [r1, r2]
//     020c2bb8  05941000  ldreq r1, [r4]
//     020c2bbc  e12fff31  blx   r1
//     020c2bc0  e28400c8  add   r0, r4, #0xc8
//     020c2bc4  ebfd4c1c  bl    #0x2015c3c        Animation::Advance
//     020c2bc8  e28400dc  add   r0, r4, #0xdc
//     020c2bcc  ebfd4c1a  bl    #0x2015c3c        Animation::Advance
//     020c2bd0  e2840008  add   r0, r4, #8
//     020c2bd4  ebfd4e28  bl    #0x201647c        BlendModelAnim::Advance
//     020c2bd8  e1a00004  mov   r0, r4
//     020c2bdc  ebfffdab  bl    #0x20c2290
//     020c2be0  e8bd4010  pop   {r4, lr}
//     020c2be4  e12fff1e  bx    lr
//
// which is the five-instruction sequence MgBase_StateDispatch.cpp's header
// disassembles, verbatim, with the null guard in front of it. NO ARGUMENT
// REGISTER IS SET before the `blx` -- r0 carries the advanced receiver and
// nothing else -- so the dispatch is ARITY 0.
//
// WHAT MSVC WOULD HAVE DONE. src spells `struct C { PMF pmf; };` with
// `typedef void (C::*PMF)()`. MSVC's single-inheritance member pointer is FOUR
// bytes and is just the code address, so `c->pmf` reads word 0 of the mounted
// sub-object -- a raw DS code address -- and `(c->*(c->pmf))()` calls it. It
// compiles, it links, and the first frame either Behavior runs it jumps to a DS
// address with eip equal to that address, which is lane FLW's symptom exactly.
//
// ---- THE SIX ADDRESSES, READ OUT OF THE ROM --------------------------------
//
// Not guessed and not read from a running object. SEVEN eight-byte
// {code, adjustment} pairs sit in the block at 0x0213adb8..0x0213adf8. Six are
// installed into word 0 of the sub-object by a state-setter; the seventh is
// the sentinel a predicate compares against. The block is NOT a contiguous
// table -- two bone-name strings ("m_eye_r", "m_eye_l") are interleaved at
// 0x0213add8 and 0x0213ade8 -- so a naive 8-byte sweep manufactures phantoms
// and every row below is derived from RELOCATIONS in both directions instead.
//
// Words dumped from the overlay image, and every one cross-checked against
// config/arm9/overlays/ov006/relocs.txt, which carries a load relocation from
// each pair's first word TO the code it names AND a load relocation from the
// installing function's literal pool TO the pair:
//
//   pair address          words                installed from   inside
//   -------------------   ------------------   --------------   -------------------
//   data_ov006_0213add0   020c22d8  00000000   0x020c23a4       func_ov006_020c2300
//   data_ov006_0213adf8   020c23a8  00000000   0x020c24e0       func_ov006_020c2440
//   data_ov006_0213adc0   020c24e4  00000000   0x020c2638       func_ov006_020c2594
//   data_ov006_0213adf0   020c263c  00000000   0x020c26f0       func_ov006_020c2664
//   data_ov006_0213ade0   020c26f4  00000000   0x020c27c0       func_ov006_020c271c
//   data_ov006_0213adc8   020c27c4  00000000   0x020c2920       func_ov006_020c2848
//   data_ov006_0213adb8   020c27c4  00000000   NOT INSTALLED ANYWHERE -- see below
//
// SEVEN PAIRS, SIX DISTINCT CODE WORDS: 0x020c27c4 is both 0213adc8's and the
// sentinel's.
//
// 0x0213adb8 IS A READ-ONLY SENTINEL AND NOT A STATE INSTALL. Its literal-pool
// citation 0x020c29d8 is the last word of func_ov006_020c2994 (0x020c2994,
// ends 0x020c29dc), NOT of func_ov006_020c2924 (0x020c2924, size 0x60, ends
// 0x020c2984 with its own pool word 0209F5BC at 0x020c2980). func_ov006_020c2994
// disassembles to `ldr r3,[pc,#0x3c] / ldr r2,[r0] / ldr r1,[r3] / cmp / ldr
// r1,[r0,#4] / ldr r0,[r3,#4] / cmp / ...` with ZERO stores in the body: it
// COMPARES and never writes. Lane BOX's derivation table lists this row as an
// install; that is the one factual error in either lane's table and it is
// corrected here. It changes no routing, because 0x020c27c4 is installed for
// real from 0x0213adc8.
//
// EVERY ADJUSTMENT WORD IS ZERO, the same measurement MgBase_StateDispatch
// records for its own closure, so `v >> 1` is 0, the advanced `this` is the
// object itself, and the virtual branch is never taken. The virtual arm is
// REPORTED rather than implemented below, for that file's reason: a dispatch
// shape nobody has measured is exactly the plausible body
// port/tools/inferred_stub_guard.py exists to refuse.
//
// ---- WHY SIX AND NOT FOUR ---------------------------------------------------
//
// Lane CUP routed FOUR and deliberately left 020c22d8 and 020c26f4 unrouted,
// on the correct finding that neither installer is in dScMgCup_c's closure.
// That is right for CUP ALONE and wrong for the merged tree, and the ROM says
// why. Each installer has exactly one arm_call in all of ov006:
//
//   func_ov006_020c2300  <- from:0x0211bf10, inside func_ov006_0211bc8c
//                           (0x0211bc8c, size 0x2b8) -- dScMgSound_c's ONE
//                           bodiless floor. Unreachable in either scene today;
//                           reachable the moment that floor is decompiled.
//   func_ov006_020c271c  <- from:0x0211b8b8, inside func_ov006_0211b80c
//                           (0x0211b80c, size 0x148), which IS on
//                           port/slice_box.txt and IS linked. So state
//                           0x020c26f4 is LIVE in scene 367.
//
// Both callers are in the 0x0211xxxx block, which is why neither can reach
// dScMgCup_c and why lane CUP's own reading was sound. In the merged tree both
// src TUs (src/func_ov006_020c22d8.c and src/func_ov006_020c26f4.c) are linked
// through port/slice_box.txt, so routing them is a link this tree already has
// and NOT the link error CUP's header warned an unbacked case would be. The
// union is therefore both safe and required: with four cases, scene 367's
// live state 0x020c26f4 would fall out of the switch.
//
// The four state-setters the CUP closure does reach are themselves shared --
// func_ov006_020c2594, _020c2664 and _020c2440 each have TWO arm_calls, one at
// 0x020dfxxx and one at 0x0211bxxx/0x0211cxxx -- which is the same dual-host
// shape as the three host-copied bodies.
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
// call. Same choice as MgCurling_StateDispatch, MgFlower_SubDispatch and
// MgSound_StateDispatch, and made for the sharper of the two reasons.
//
// ---- WHY IT DOES NOT ROUTE THROUGH A PER-CLASS call0 -----------------------
//
// MgFlower_SubDispatch.cpp records: port_mg_try_ov006_0 is DEFINED in
// unmatched/MgCurling_StateDispatch.cpp and DECLARED in
// MgBase_StateDispatch.cpp, so a second class cannot add cases to it. Each
// class's TABLE dispatch chains to its own (port_mg_cup_call0 in
// unmatched/MgCup_StateDispatch.cpp, port_mg_sound_call1 in
// unmatched/MgSound_StateDispatch.cpp); this SUB-object one belongs to
// NEITHER class, so it owns its own switch and its own counters and hands
// anything it does not know to the framework's port_mg_call0 -- the same
// fallthrough MgCup_StateDispatch.cpp:191 and MgSound_StateDispatch.cpp
// already use.
//
// IT DEFINES func_ov006_020c2b8c, so src/func_ov006_020c2b8c.cpp is out of
// port/slice_cup.txt AND out of port/slice_box.txt. Listing it in either would
// be an LNK2005.

#include <cstdio>

extern "C" {

/* The framework's arity-0 fallthrough, from unmatched/MgBase_StateDispatch.cpp.
   It owns the guards and its own report. */
void port_mg_call0(void *self, unsigned code, int adj);

/* The SIX state bodies, each declared with the parameter list its own src TU
   defines -- checked one by one against src/ rather than copied from either
   lane. All six are matched; four are on both slices, and 020c22d8 / 020c26f4
   are on port/slice_box.txt. */
int  func_ov006_020c22d8(char *t);   /* src/func_ov006_020c22d8.c   int(char*)  */
void func_ov006_020c23a8(void *c);   /* src/func_ov006_020c23a8.cpp void(void*) */
void func_ov006_020c24e4(void *c);   /* src/func_ov006_020c24e4.cpp void(void*) */
int  func_ov006_020c263c(char *t);   /* src/func_ov006_020c263c.c   int(char*)  */
int  func_ov006_020c26f4(char *t);   /* src/func_ov006_020c26f4.c   int(char*)  */
void func_ov006_020c27c4(char *c);   /* src/func_ov006_020c27c4.cpp void(char*) */

/* the rest of the dispatching TU, verbatim from src */
void func_ov006_020c2be8(char *c);   /* src/func_ov006_020c2be8.c   void(char*) */
void func_ov006_020c2290(char *c);   /* src/func_ov006_020c2290.c   void(char*) */
void _ZN9Animation7AdvanceEv(void *anim);
void _ZN14BlendModelAnim7AdvanceEv(void *anim);

/* The parameter is `char *`, which is what include/decl_common.h:2401 already
   declares (`extern void func_ov006_020c2b8c(char*);`) and therefore what every
   matched caller in the tree sees -- src/func_ov006_0211c720.c includes that
   header. Both spellings link (the symbol has C linkage), but only this one
   agrees with the tree's own declaration. */
void     func_ov006_020c2b8c(char *c);

/* The witnesses. Named for the SUB-OBJECT rather than for either class,
   because the body belongs to neither. Both seats' censuses read these. */
unsigned port_mg_sub4f38_calls(void);
unsigned port_mg_sub4f38_routed(void);
unsigned port_mg_sub4f38_idle(void);
unsigned port_mg_sub4f38_unknown(void);

}  /* extern "C" */

static unsigned g_sub_calls;     /* every entry, including the null-pmf one */
static unsigned g_sub_routed;    /* entries that ran a measured state        */
static unsigned g_sub_idle;      /* entries the ROM's own null guard skipped */
static unsigned g_sub_unknown;

extern "C" unsigned port_mg_sub4f38_calls(void)   { return g_sub_calls; }
extern "C" unsigned port_mg_sub4f38_routed(void)  { return g_sub_routed; }
extern "C" unsigned port_mg_sub4f38_idle(void)    { return g_sub_idle; }
extern "C" unsigned port_mg_sub4f38_unknown(void) { return g_sub_unknown; }

static void sub_unhandled(const char *why, unsigned code, int adj)
{
    static int said;
    ++g_sub_unknown;
    if (!said) {
        said = 1;
        std::fprintf(stderr, "  [scene] +0x4f38 SUB-OBJECT STATE %s: "
                     "code %08x adj %d. Handed to the framework. "
                     "port/unmatched/MgShared4f38_SubDispatch.cpp\n",
                     why, code, adj);
        std::fflush(stderr);
    }
}

/* The address switch. Six measured cases and no default body: an address this
   does not know is REPORTED and handed to the framework, never guessed at,
   which is the whole difference between this and the raw `blx r1` it replaces.
   Returns nonzero when it ran a state. */
static int sub_call(void *p, unsigned code)
{
    switch (code) {
    case 0x020c22d8u: func_ov006_020c22d8((char *)p);    return 1;
    case 0x020c23a8u: func_ov006_020c23a8(p);            return 1;
    case 0x020c24e4u: func_ov006_020c24e4(p);            return 1;
    case 0x020c263cu: func_ov006_020c263c((char *)p);    return 1;
    case 0x020c26f4u: func_ov006_020c26f4((char *)p);    return 1;
    case 0x020c27c4u: func_ov006_020c27c4((char *)p);    return 1;
    default:          return 0;
    }
}

/* src/func_ov006_020c2b8c.cpp, verbatim except that the member-pointer read and
   the indirect call become the two-word read and sub_call. The null guard, the
   call order and the three Advance offsets are all src's and all agree with the
   ROM disassembly in the header.

   BOTH LANES' REFUSALS ARE KEPT. Lane CUP refuses a nonzero adjustment outright
   and refuses the virtual arm; lane BOX gates routing on `adj == 0` and hands
   everything else to the framework. Every refusal arm below both REPORTS (CUP)
   and CHAINS (BOX). All three refusal arms are dead in both measured scenes --
   361 and 367 each report 0 UNHANDLED over 1153 calls -- so no census number
   depends on which half of the union fires. */
// PORT_HOST_ABI: the shared +0x4f38 sub-object's field-held pmf dispatcher; MSVC's four-byte single-inheritance member pointer reads word 0 of the mounted sub-object as a raw DS code address and jumps to it, so the host reads the {code, adj} pair and routes it.
extern "C" void func_ov006_020c2b8c(char *c)
{
    ++g_sub_calls;

    func_ov006_020c2be8(c);

    {
        const unsigned code = *(const unsigned *)(c + 0);

        if (code == 0) {
            /* the ROM's own `cmp r2,#0 / beq`, which src spells `if (c->pmf)`.
               The ROM does not read the adjustment on this path either -- the
               `ldr r1,[r4,#4]` at 0x020c2ba4 sits past the branch target, which
               is why the adjustment is read below and not above. */
            ++g_sub_idle;
        } else {
            const int   adj  = *(const int *)(c + 4);
            /* `add r0, r4, r1, asr #1` -- the ROM's advanced receiver. Every
               measured adjustment is zero, so this is the object itself; it is
               spelled the ROM's way rather than the measured way on purpose. */
            char *const self = c + (adj >> 1);

            if (adj & 1) {
                /* The virtual arm. The ROM reads the vtable at [r0] and indexes
                   it by the code word as a BYTE OFFSET. No pair in this closure
                   sets the bit, so there is nothing to verify an implementation
                   against and it reports instead. */
                sub_unhandled("through the VIRTUAL arm of the member pointer, "
                              "which no measured pair in this closure uses",
                              code, adj);
                port_mg_call0(self, code, adj);
            } else if (adj != 0) {
                sub_unhandled("with a NONZERO ADJUSTMENT, which no measured "
                              "pair in this closure has", code, adj);
                port_mg_call0(self, code, adj);
            } else if (sub_call(self, code)) {
                ++g_sub_routed;
            } else {
                sub_unhandled("UNHANDLED by this switch -- the sub-object's "
                              "state universe is larger than the six code words "
                              "the ROM's own literal pools install", code, adj);
                port_mg_call0(self, code, adj);
            }
        }
    }

    _ZN9Animation7AdvanceEv(c + 0xc8);
    _ZN9Animation7AdvanceEv(c + 0xdc);
    _ZN14BlendModelAnim7AdvanceEv(c + 8);
    func_ov006_020c2290(c);
}
