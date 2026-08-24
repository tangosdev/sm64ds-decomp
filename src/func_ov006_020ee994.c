// NONMATCHING: ONE codegen divergence and its fallout, decomposed below.
// Logic verified instruction-for-instruction against
// extracted/overlays/overlay_0006.bin at base 0x020bfec0 (file offset
// 0x2ead4, 0x168 bytes = 85 instructions plus 5 pool words). Counts as
// decompiled, not matched. Run mg12, lane IGN.
//
// dScMgJump_c (actor id 0x174, scene 372, "Bounce and Pounce"), VTABLE SLOT 18
// -- THE IGNITION. Slot 0 (InitResources, src/func_ov006_020ee690.cpp:112)
// ends with `((Base *)base)->m48(-1)`, vtable offset 0x48 = slot 18, and this
// body is the only thing in ov006 that reaches the state-0 installer
// func_ov006_020ee658, whose only job is to copy the {code,adj} pair at
// data_ov006_0213cb54 into self+0x5004. Nothing else in ov006 writes that
// field before vtable slot 6 first reads it, so without this body the class
// boots, ticks and renders with a NULL state pair and nothing in the minigame
// moves. port/slice_bnp.txt section 8 and port/hal/scene_mg_jump.cpp section 4
// carry the floor's derivation; this file retires it.
//
// ---- THE ARGUMENT SELECTS ONE OF THREE ARMS -------------------------------
//
// Slot 0 passes -1, which takes the third arm: the round counter at self+0xbc
// is not touched, so a fresh scene ignites at round 0. The other two arms are
// the ROM's round advance (0) and its reset (0x12); both clamp the counter at
// 0x270e, and the ROM emits the clamp after the reset too even though a store
// of 0 can never trip it. That dead clamp is transcribed rather than dropped,
// because it is what the bytes say.
//
// ---- WHAT IT DOES, IN ROM ORDER -------------------------------------------
//
//   arg 0     ++self->+0xbc, clamped at 0x270e
//   arg 0x12  self->+0xbc = 0, same clamp, then the global pointer at
//             data_ov004_020beb68 gets its +0xb4 zeroed and
//             func_ov004_020adb1c(self->+0xb4) runs -- a DIFFERENT object's
//             +0xb4 is zeroed than the one whose +0xb4 is read
//   otherwise that global's +0xb4 = self->+0xbc
//   then      func_ov006_020c72b4() if +0xbc != 0 else func_ov006_020c72c8()
//             (they write 0 and 1 to data_ov006_02140400[0])
//   then      the draw: only from round 0xf on, a 0..9 value rerolled while it
//             equals the cached last draw in data_ov006_0213cb48 (the ROM
//             image initialises that word to 0xffffffff, which is why it is a
//             scratch int and not the adjustment half of a state pair -- see
//             unmatched/MgBounceAndPounce_StateDispatch.cpp section 1). Below
//             round 0xf the draw stays equal to the round number.
//   then      func_ov006_020c719c(+0xbc, draw)
//   then      data_ov006_02140328 = 3 / 4 / 5 / 6 for +0xbc < 0xf / < 0x14 /
//             < 0x19 / else -- the difficulty ladder, i.e. how many Fly Guys
//             this round asks for
//   then      func_ov006_020c44b4(+0xbc, draw), which copies that ladder value
//             into data_ov006_02140304 -- the live remaining-target counter
//             the play state func_ov006_020ee508 tests
//   tail      func_ov006_020ee658(self), the state-0 installer
//
// The ROM does not set r0 before its epilogue, so the word a caller would see
// is whatever func_ov006_020ee658 leaves behind (self+0x5000, from its own
// struct-copy address). No caller reads it: slot 0 declares m48 void.
//
// ---- THE DIVERGENCE, AND IT IS ONE THING ----------------------------------
//
// Candidate 0x164 against the ROM's 0x168 -- one word short. The reroll loop's
// three loop-invariant constants (the address of data_0209e650, the literal
// 0xa the multiply needs in a register, and the address of
// data_ov006_0213cb48) are REMATERIALISED per iteration by the ROM and HOISTED
// into a preheader by mwccarm. Everything else follows from that:
//
//   ROM  020EEA44 ldr r0,[pc,#0xa4]   \  three in-loop materialisations,
//        020EEA54 mov r0,#0xa          }  no preheader
//        020EEA5C ldr r0,[pc,#0x90]   /
//   ours          ldr r4,[pc,#0xa4]   \  three preheader materialisations
//                 ldr r8,[pc,#0xa4]    }  plus one in-loop `mov r0,r4`,
//                 mov r7,#0xa         /   net +1 word in that region
//
// Holding three constants across the call costs three more callee-saved
// registers, so the frame is push{r4,r5,r6,r7,r8,lr} where the ROM has
// push{r4,r5,lr} plus the `sub sp,sp,#4` that three pushed registers need for
// 8-byte alignment (and the matching `add sp,sp,#4`), and the receiver colours
// r6 where the ROM colours r5, the draw r5 where the ROM colours r4. Net
// 0x164 vs 0x168. Every other instruction is the same opcode with the same
// operands in the same order, and the five pool words are in the ROM's order.
//
// LEVERS TRIED, ALL FAILED. Loop form is not the axis: do-while, backward
// goto, for(;;)+break, while(1)+break and store-inside-the-loop all compile to
// a BYTE-IDENTICAL object (same md5), so mwccarm normalises the shape before
// it decides. Also tried and byte-identical: the seed as a local pointer (the
// spelling src/func_ov006_020c44b4.c uses), the seed as an array, a signed
// compare, `10 * x` operand order, a temporary for the shifted value, and
// dropping the inner parentheses; a `long long` launder on the multiply made
// it worse (0x180). Not a compiler-version question either -- match.py --all
// over ALL TWENTY-FIVE installed mwccarm builds returns 0x164 from every one.
// Not an optimisation-level question -- -O4,p, -O4,s, -O4, -O3,p and -O2,p all
// give 0x164 (-O1 gives 0x184). And the pragma family the near-miss database
// names for this shape (nearmiss row 44, the Mix-a-Mug floor's "rematerialised
// per outer iter in ROM vs hoisted" divergence) DOES NOT EXIST in this build:
// opt_loop_invariants, opt_propagation, opt_common_subs,
// opt_strength_reduction, opt_lifetimes, register_coloring, global_optimizer,
// peephole, optimize_for_size, opt_unroll_loops, opt_pointer_analysis and
// opt_vectorize_loops were each set `off` and each produced an object with the
// SAME md5 as no pragma at all -- they are accepted (no illpragmas warning)
// and inert. Register pressure is not the axis either: adding live locals
// makes mwccarm reach for r9/r10/r11, it never stops hoisting.

#include "types.h"

extern void func_ov004_020adb1c(int self);
extern void func_ov006_020c719c(u32 a0, u32 a1);
extern void func_ov006_020c44b4(void *arg0, int arg1);
extern void func_ov006_020c72b4(void);
extern void func_ov006_020c72c8(void);
extern void func_ov006_020ee658(char *c);
extern int RandomIntInternal(int *seed);

extern int data_0209e650;
extern char *data_ov004_020beb68;
extern int data_ov006_0213cb48;
extern int data_ov006_02140328;

void func_ov006_020ee994(char *c, int a1)
{
    u32 v;

    if (a1 == 0) {
        u32 *p = (u32 *)(c + 0xbc);
        *p = *p + 1;
        if (*(u32 *)(c + 0xbc) > 0x270e)
            *(u32 *)(c + 0xbc) = 0x270e;
    } else if (a1 == 0x12) {
        *(u32 *)(c + 0xbc) = 0;
        if (*(u32 *)(c + 0xbc) > 0x270e)
            *(u32 *)(c + 0xbc) = 0x270e;
        if (data_ov004_020beb68 != 0)
            *(int *)(data_ov004_020beb68 + 0xb4) = 0;
        func_ov004_020adb1c(*(int *)(c + 0xb4));
    } else {
        u32 n = *(u32 *)(c + 0xbc);
        if (data_ov004_020beb68 != 0)
            *(u32 *)(data_ov004_020beb68 + 0xb4) = n;
    }

    if (*(u32 *)(c + 0xbc) != 0)
        func_ov006_020c72b4();
    else
        func_ov006_020c72c8();

    v = *(u32 *)(c + 0xbc);
    if (v >= 0xf) {
        do {
            v = (u32)(((int)(((u32)RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 0x13) * 10) >> 0xc);
        } while (v == (u32)data_ov006_0213cb48);
        data_ov006_0213cb48 = v;
    }
    func_ov006_020c719c(*(u32 *)(c + 0xbc), v);

    if (*(u32 *)(c + 0xbc) < 0xf)
        data_ov006_02140328 = 3;
    else if (*(u32 *)(c + 0xbc) < 0x14)
        data_ov006_02140328 = 4;
    else if (*(u32 *)(c + 0xbc) < 0x19)
        data_ov006_02140328 = 5;
    else
        data_ov006_02140328 = 6;

    func_ov006_020c44b4((void *)*(u32 *)(c + 0xbc), v);
    func_ov006_020ee658(c);
}
