// NONMATCHING: register allocation and instruction scheduling only (42 of 197
// words differ). Size is exact (0x314), control flow is exact, and every one of
// the 16 relocations resolves to the destination the ROM's own word carries:
// four calls to RandomIntInternal (0x0203b990), one each to func_ov006_021238d0,
// _Z14ApproachLinearRiii (0x0203ae58), func_ov006_021237c8, func_ov006_0212373c,
// func_0203cc28 and func_ov006_020cefa4, four to func_ov006_0212382c, and the
// two pool words data_0209e650 and data_ov006_0212f328. THE ROM HAS FOURTEEN
// CALL SITES IN THIS BODY, NOT FIFTEEN -- counted by disassembling all 197 words
// and keeping the `bl` instructions, which are all fourteen external (this body
// makes no internal calls). THIRTEEN of the fourteen also land at the ROM's own
// byte offset; the fourteenth is at +0x198 against the ROM's +0x19c, displaced
// one word by divergence 3 below and nothing else. An earlier version of this
// paragraph said "fourteen of the fifteen", which counted a call site the ROM
// does not have. Logic verified instruction by instruction against the ROM at 0x02123428.
// Run mg12, lane TRM.
//
// THE THREE DIVERGENCES, ALL MEASURED, NONE OF THEM SEMANTIC:
//
//  1. The prologue's zero. The ROM materialises 0 in the loop counter (sb) and
//     derives both `mask = 0` (+0x040, `str sb, [sp]`) and the hoisted zero
//     constant (+0x04c, `mov r4, sb`) from it after the loop-invariant constant
//     spills. mwccarm 2004/b56 materialises the same zero in r0, stores mask
//     FIRST (+0x00c) and copies to sb and r4 from there. Same instruction count,
//     same 0x64 frame, same spill slots; the order and the register names differ.
//     Reordering the source (`for (i = 0, mask = 0; ...)`, `i = 0; mask = 0;`,
//     declaration order, mask type) was swept and every arm compiled SHORTER,
//     not closer.
//
//  2. `1 << (slot * 2)` is eager in the ROM (+0x10c, beside the `2 << (slot * 2)`
//     at +0x108) and lazy at 2004/b56, which defers it into the branch that first
//     needs it (+0x118 and +0x1ac). Binding both masks to locals, to a shift
//     local, and every operand order for the four tests were swept: all compile
//     shorter or equal, none eager.
//
//  3. The ROM re-loads `mask` and re-ANDs for each of the four tests (+0x114,
//     +0x170, +0x1b0); 2004/b56 keeps the first AND's result and re-tests it
//     (+0x170 `cmp r0, #0`). Moving the three arms inside the retry loop, and 27
//     spellings of the tests, leave this unchanged.
//
// The pool at +0x30c..+0x310 is two relocated words (data_0209e650,
// data_ov006_0212f328) and is wildcarded by the gate, not a divergence.
//
// Counts as decompiled, not matched. No delinks block, by the NONMATCHING seat
// convention (src/MgTrampolineTerror_Spawn.c precedent).
//
// WHAT IT DOES: builds one five-slot Trampoline Terror round. For each of five
// slots it rolls a kind 0..10, re-rolls 7 and 10 on a 1-in-4 gate, re-rolls any
// kind already used by an earlier slot (restarting the scan), maps the kind to
// one of five columns via func_ov006_021238d0, and consults a per-column
// two-bit occupancy mask: both bits set means the column is full, so it walks
// the kind forward with ApproachLinear (wrapping 11 -> 0) and starts over; one
// bit set picks the matching face variant; neither picks the free variant. The
// chosen face is fed to func_ov006_020cefa4 with an up vector rotated by 0x4000
// for the odd kinds and the two flagged faces.
extern int RandomIntInternal(int *seed);
extern int data_0209e650;
extern int data_ov006_0212f328;

extern int func_ov006_021238d0(int x);
extern unsigned short func_ov006_0212382c(int sel, int b);
extern int func_ov006_021237c8(int x);
extern int func_ov006_0212373c(int a);
extern int _Z14ApproachLinearRiii(int *v, int a, int b);
extern void func_0203cc28(int *p, int angle);
extern char *func_ov006_020cefa4(int a0, int *a1, int a2, int a3);

typedef struct {
    int x, y, z;
} Vec3i;

void func_ov006_02123428(char *c)
{
    int mask;
    int slot;
    int cur;
    int kinds[7];
    short faces[7];
    Vec3i v;
    int i, j;
    int t;

    mask = 0;
    for (i = 0; i < 5; i++) {
        do {
            cur = (int)((((unsigned int)RandomIntInternal(&data_0209e650)
                          & 0x7fffffff) >> 0x13) * 11) >> 12;
        } while ((cur == 7 || cur == 10)
                 && (int)(((unsigned int)RandomIntInternal(&data_0209e650)
                           & 0x7fffffff) >> 0x13) > 0x400);

        for (;;) {
            for (j = 0; j < i; j++) {
                if (cur == kinds[j]) {
                    cur = (int)((((unsigned int)RandomIntInternal(&data_0209e650)
                                  & 0x7fffffff) >> 0x13) * 11) >> 12;
                    j = -1;
                }
            }
            slot = func_ov006_021238d0(cur);
            if ((mask & (2 << (slot * 2))) != 0 && (mask & (1 << (slot * 2))) != 0) {
                do {
                    if (_Z14ApproachLinearRiii(&cur, 11, 1) != 0)
                        cur = 0;
                } while ((cur == 7 || cur == 10)
                         && (int)(((unsigned int)RandomIntInternal(&data_0209e650)
                                   & 0x7fffffff) >> 0x13) > 0x800);
                continue;
            }
            if ((mask & (2 << (slot * 2))) != 0) {
                kinds[i] = cur;
                faces[i] = 8;
                mask |= func_ov006_0212382c(slot, 1);
            } else if ((mask & (1 << (slot * 2))) != 0) {
                kinds[i] = cur;
                faces[i] = (short)func_ov006_021237c8(cur);
                mask |= func_ov006_0212382c(slot, 0);
            } else {
                kinds[i] = cur;
                faces[i] = (short)func_ov006_0212373c(cur);
                switch (faces[i]) {
                case 4:
                case 5:
                case 6:
                case 7:
                    mask |= func_ov006_0212382c(slot, 0);
                    break;
                default:
                    mask |= func_ov006_0212382c(slot, 1);
                    break;
                }
            }
            break;
        }

        v.x = 0;
        v.y = 0x1000;
        v.z = 0;
        if (cur >= 5 || ((cur & 1) != 0 && (faces[i] == 8 || faces[i] == 5)))
            func_0203cc28((int *)&v, 0x4000);
        t = 1;
        if (faces[i] == 6)
            t = 2;
        func_ov006_020cefa4((int)&data_ov006_0212f328 + kinds[i] * 0xc, (int *)&v,
                            t, faces[i]);
    }
}
