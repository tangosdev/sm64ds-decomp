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
#include <cstdlib>

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
/* run link100 lane PMFB6 gate 3: the seat over the seven pairs */
void     port_mg_sub4f38_seat(void);

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

// ---- run link100 LANE PMFB6, GATE 3: THE SEVEN PAIRS ARE SEATED ------------
//
// THE PARAGRAPH ABOVE THIS ONE IS SUPERSEDED, and the measurement that
// supersedes it is the point. "THE STORED WORD IS DELIBERATELY NOT REWRITTEN"
// refused a seat because src/func_ov006_020c2994.c compares an object's stored
// pair BY VALUE against data_ov006_0213adb8, so writing host addresses over the
// pairs "would make that comparison ask whether a host address equals a DS
// address, which is false forever".
//
// That is true of a face per RECORD and false of a face per CODE WORD, which is
// the rule lane FWD wrote for exactly this shape. 0213adb8 (the sentinel) and
// 0213adc8 (the installed idle state) hold the SAME code word 0x020c27c4. Seat
// both with the same face and the predicate compares the same host address
// against itself: it answers what it answered before, on every path. The null
// case is unchanged too -- a field nothing installed still reads {0,0} and
// still fails the first test, because the sentinel's word is non-zero either
// way.
//
// THE UNIVERSE IS CLOSED, re-measured for this gate rather than carried
// (runs/link100/out/PMFB6/sweep_shared4f38.txt): every `load` relocation in
// ov006 whose destination lands in this class's own code block
// 0x020c2290..0x020c2a00, kept when the word reads the target and the following
// word is zero, gives exactly SEVEN pairs and they are exactly the run
// 0x0213adb8..0x0213adf8 this file already names. Nothing else in ov006 can
// hand this field an address in this class. Every reference INTO that run comes
// from inside the same block except sixteen that point at 0x0213add8 and
// 0x0213ade8, the two bone-name strings interleaved in it.
//
// THE EMITTED SIDE, off src/func_ov006_020c2b8c.cpp's own /FAsc listing under
// the port's own flags (runs/link100/out/PMFB6/emit_gate3_out.txt):
//     mov eax,[esi] / test eax,eax / je / mov ecx,[esi+4] / add ecx,esi /
//     call eax
// -- word 0 the code, word 4 the adjustment, the ROM's own null guard, receiver
// in ecx and ARITY ZERO. /Zp4 changes 0 listing lines outside the TITLE. Block
// R8's /vmg /vmm is what makes the field eight bytes; the "MSVC's
// single-inheritance member pointer is FOUR bytes" paragraph above predates it.
//
// WHAT THE CENSUS CAN STILL SEE. The dispatch is inside the matched TU now, so
// the faces count what they can and this file says what they cannot rather than
// leaving a counter that reads zero forever (lane PMFB3's tapped-pad trap):
//   calls   / routed  bumped by every face, one per dispatch, still exact;
//   idle    NO LONGER OBSERVABLE. It counted entries the ROM's own null guard
//           skipped, and that guard is now inside src/func_ov006_020c2b8c.cpp
//           where nothing outside can see it. It reads 0 and the seat says so.
//   unknown STRUCTURALLY ZERO. An unrouted code word was a field holding an
//           address this file did not know; after the seat the field can only
//           hold one of the six faces, because the sweep above says the six are
//           the whole universe.

static unsigned g_sub_face_calls;

#define SUB_FACE(tag, sym, cast)                                          \
    static void __fastcall sub4f38_##tag(void *self, void *dead_edx)      \
    {                                                                     \
        (void)dead_edx;                                                   \
        ++g_sub_calls;                                                    \
        ++g_sub_routed;                                                   \
        ++g_sub_face_calls;                                               \
        sym((cast)self);                                                  \
    }

SUB_FACE(020c22d8, func_ov006_020c22d8, char *)
SUB_FACE(020c23a8, func_ov006_020c23a8, void *)
SUB_FACE(020c24e4, func_ov006_020c24e4, void *)
SUB_FACE(020c263c, func_ov006_020c263c, char *)
SUB_FACE(020c26f4, func_ov006_020c26f4, char *)
SUB_FACE(020c27c4, func_ov006_020c27c4, char *)

namespace {
struct SubSeat { unsigned *rec; unsigned rom; void *face; const char *what; };
const SubSeat g_sub4f38_seats[] = {
    {0, 0x020c27c4, (void *)sub4f38_020c27c4, "0213adb8 the idle SENTINEL"},
    {0, 0x020c24e4, (void *)sub4f38_020c24e4, "0213adc0"},
    {0, 0x020c27c4, (void *)sub4f38_020c27c4, "0213adc8 the idle state"},
    {0, 0x020c22d8, (void *)sub4f38_020c22d8, "0213add0"},
    {0, 0x020c26f4, (void *)sub4f38_020c26f4, "0213ade0"},
    {0, 0x020c263c, (void *)sub4f38_020c263c, "0213adf0"},
    {0, 0x020c23a8, (void *)sub4f38_020c23a8, "0213adf8"},
};
}  /* namespace */

extern "C" {
extern unsigned data_ov006_0213adb8[], data_ov006_0213adc0[],
    data_ov006_0213adc8[], data_ov006_0213add0[], data_ov006_0213ade0[],
    data_ov006_0213adf0[], data_ov006_0213adf8[];
}

extern "C" void port_mg_sub4f38_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    unsigned *recs[] = {
        data_ov006_0213adb8, data_ov006_0213adc0, data_ov006_0213adc8,
        data_ov006_0213add0, data_ov006_0213ade0, data_ov006_0213adf0,
        data_ov006_0213adf8,
    };
    for (unsigned i = 0; i < sizeof recs / sizeof recs[0]; ++i) {
        unsigned *p = recs[i];
        if (p[0] != g_sub4f38_seats[i].rom || p[1] != 0) {
            std::fprintf(stderr, "FATAL: +0x4f38 sub-object pair %s: the mount "
                         "holds %08x/%08x, the cartridge's own record says "
                         "%08x/0 -- WRONG BYTES\n", g_sub4f38_seats[i].what,
                         p[0], p[1], g_sub4f38_seats[i].rom);
            std::abort();
        }
        p[0] = (unsigned)(size_t)g_sub4f38_seats[i].face;
        p[1] = 0;
    }
}

/* HOST COPY RETIRED, run link100 lane PMFB6 gate 3.
   src/func_ov006_020c2b8c.cpp dispatches its own field now. */
