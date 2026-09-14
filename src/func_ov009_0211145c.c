// @symbol func_ov009_0211145c
/* recovered: shared common types */
#include "common.h"
// NONMATCHING: hand-written asm, not a C decompilation. Byte-exact via an asm hatch on a
// proven mwccarm 1.2 register-allocation/scheduling wall; does NOT count as matched. Reverts
// to a draft until someone reproduces the bytes from real C.
/* func_ov009_0211145c — spawn-count loop + free-bird velocity/sound setup.
 * Store/call tail uses mwccarm asm: pure C emits rsb r0 + wrong str/mov
 * interleave for -0x14000 / 0xff06a000 / func_0201267c (same wall as sibling
 * 021116ec avoids by not calling after the stores).
 *
 * MEASURED 2026-09-09 (run link100 lane MATCH4). Replacing the asm block with the
 * plain C it spells -- *(int*)(c+0x160) = -0x14000; *(int*)(c+0x168) = 0xff06a000;
 * func_0201267c(0x6a, c+0x74); -- is size-exact at div=5 under 2004/b56, all of it in
 * one 5-instruction window at +0xe0..+0xf4:
 *   ROM:  rsb r1,r0,#0 / str r1,[sl,#0x160] / ldr r0,[pc,#0x78] / add r1,sl,#0x74 /
 *         str r0,[sl,#0x168] / mov r0,#0x6a
 *   here: rsb r0,r0,#0 / ldr r2,[pc,#0x7c] / str r0,[sl,#0x160] / add r1,sl,#0x74 /
 *         mov r0,#0x6a / str r2,[sl,#0x168]
 * mwccarm hoists the pool load above the first store and negates in place; the ROM
 * negates into a second register and keeps the pool load between the two stores.
 * Byte-identical to that baseline, so all inert: a named int for 0x14000; 0 - 0x14000;
 * unsigned store types; an explicit (int)0xff06a000; a named char *arg = c + 0x74
 * before the second store; a two-step w = -v; the arg named at the top of the block;
 * the arg named in a nested scope. decomp-permuter on 2004/b56, 3425 iterations at -j4,
 * base score 280, never improved.
 */
/* Re-measured 2026-09-12 under 2004/b56 (the stored near-miss draft is div 3, size-exact
 * 0x17c; residual is the pool constant 0xff06a000 coloured r3 not r0, so our str/mov pair
 * lands the wrong way round against the ROM's `ldr r0; add r1; str r0; mov r0,#0x6a; bl`).
 * Five axes the stored evidence had not covered, all byte-identical to that baseline:
 * declaring func_0201267c void instead of int (the missing-return-value lever), declaring
 * its first parameter int rather than unsigned, storing the constant through an unsigned
 * type, storing it as a pointer value, and naming `c + 0x74` in a local before the stores.
 * The wall is that the outgoing 0x6a pins r0 before the store is scheduled; the sibling
 * func_ov009_021116ec colours the same constant r0 only because it has no call after the
 * stores to compete for it. Ordinary ARM, so NOT an asm primitive: this stays NONMATCHING.
 *
 * Re-measured 2026-09-13, run link100 lane CRK-O, 1,938 further cells, nothing under 3:
 *   * 270 cells of notes 6cd (a dead assignment to t / nvel / s and to nineteen source
 *     expressions, at five positions from the function body down into the spawn loop's
 *     own preamble). Every cell byte-identical to the baseline.
 *   * 1,280 cells of the joint axis of notes 6cc and 6ce: {both store addresses laundered
 *     none/A/B/C} x {the pool constant named int / long / unsigned / unsigned long /
 *     char* / inline in three spellings} x {the negation named, named once, or inline in
 *     two spellings} x declaration order. Nothing below 3.
 *   * 388 cells of notes 6o's ordering families: the outer guard as if / inverted else /
 *     goto-skip / do-once, crossed with equal-arm ternaries on BOTH call arguments under
 *     seven conditions and with pointer and unsigned spellings of each.
 *   * 135 pragma cells, the full accepted 2004/b56 vocabulary in both directions, run as
 *     REPLACEMENTS the way notes 6cj says to run them. All 3.
 * MICRO-LAB (the mechanism, stated so the next lane does not re-derive it): in an isolated
 * six-line function, 2004/b56 NEVER lets a pool constant reach r0 when r0 is also an
 * outgoing argument holding a small immediate. Across {no preamble, one call, a loop, a
 * call plus a loop, a guarded return} x {both addresses laundered or not} x {constant
 * named or inline}, `mov r0,#0x6a` is emitted before the 0x168 store in all forty cells
 * and the constant takes r2 or r3 every time. It only sinks behind the store when arg0
 * stops being an immediate (a global read), which changes other bytes. That is the floor.
 */

extern char* _ZN8dActor_c13ClosestPlayerEv(void* self);
extern int Vec3_Sub(struct Vector3* dst, void* a, void* b);
extern int Vec3_HorzLen(struct Vector3* v);
extern char* _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int id, unsigned int p1, void* pos, void* rot, int a, int b);
extern void func_ov009_02111224(char* c, int r1);
extern int func_0201267c(unsigned int a, void* b);
extern int RandomIntInternal(void* rng);
extern int data_0209e650;

void func_ov009_0211145c(char* c) {
    if (*(unsigned char*)(c + 0x180) != 0) {
        char* p;
        struct Vector3 copy;
        struct Vector3 diff;
        int n;
        p = _ZN8dActor_c13ClosestPlayerEv(c);
        if (p == 0) {
            return;
        }
        Vec3_Sub(&diff, c + 0x5c, p + 0x5c);
        copy.x = diff.x;
        copy.y = diff.y;
        copy.z = diff.z;
        if (Vec3_HorzLen(&copy) > 0x7d0000) {
            return;
        }
        n = *(int*)(c + 8) & 0xf;
        if (n > 1) {
            int i = 0;
            int lim = n - 1;
            if (lim > 0) {
                do {
                    char* a = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0x157, 0x10, c + 0x5c, 0, *(signed char*)(c + 0xcc), -1);
                    if (a != 0) {
                        func_ov009_02111224(a, *(int*)(c + 4));
                    }
                    i++;
                } while (i < lim);
            }
        }
        asm {
            mov r0,#0x14000
            rsb r1,r0,#0
            str r1,[c,#0x160]
            ldr r0,=0xff06a000
            add r1,c,#0x74
            str r0,[c,#0x168]
            mov r0,#0x6a
            bl func_0201267c
        }
    }
    *(short*)(c + 0x92) = 5000 - (unsigned int)RandomIntInternal(&data_0209e650) % 4000;
    *(unsigned short*)(c + 0x94) = (unsigned int)RandomIntInternal(&data_0209e650) >> 16;
    *(int*)(c + 0x174) = 0x28000;
    *(int*)(c + 0x17c) = 3;
    *(int*)(((int)c + 0xb0) & 0xFFFFFFFFFFFFFFFF) &= ~0x10000;
}
