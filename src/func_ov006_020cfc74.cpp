//cpp
// NONMATCHING: eight words, every one of them provably semantics-preserving --
// six of pre-loop schedule and one addressing split (8 divergences against 339
// ROM code words, 97.6% aligned). Run mg13, lane BOUNCE. This is the TRAMPOLINE
// BOUNCE: with it trapped the trampolines drew and every character fell straight
// through them.
//
// WHAT IT DOES, read out of the ROM. `c` is a trampoline RECORD
// (data_ov006_02140990, stride 0x32c -- the same records
// port/hal/trampoline_records.cpp reports). Each of the up-to-five character
// records the scene registered in data_ov006_0214097c is asked, through its own
// vtable, for its current position (slot 0) and its target one (slot 4),
// skipping any that is absent or reports inactive (slot 0xc). Both points are
// moved into the record's frame (origin at +0x08) and projected onto the
// record's two axes (+0x14 and +0x20), and the 2D segment they span is tested
// against the trampoline's own segment, which runs from -(+0x58) to +(+0x58)
// along the first axis. The test is the standard four-sided orientation test:
// three Vec2_Sub differences and two func_0203d524 cross products for the
// first side, the same again for the second, each result clamped to -1/0/+1, and
// a crossing declared when both sign products are non-positive.
//
// ON A CROSSING it stores the contact point at +0x38 and the character's
// direction at +0x44, plays the bounce through func_ov006_020e6db4 -- sound
// 0x1b1 with the 0213b30c/0213b310 blend when the scaled projection is under
// 0x400000, sound 0x1ae with the 0213b2f4/0213b308 blend otherwise -- writes the
// character's bounce state (2 or 1) to its +0x18, copies the record's axis into
// the character's +0x04..+0x0c with the projection and the half-length at
// +0x10/+0x14, clears the record's animation clock at +0x320, hands the contact
// to func_ov006_020cf040, and then DECREMENTS THE RECORD'S REMAINING COUNT AT
// +0x324. If that count reaches zero it sets the render gate at +0x328 to 3,
// calls func_ov006_020cfa28 and returns 1; otherwise it bumps the caught count
// at +0x326 and carries on. WITH NO CROSSING it re-tests the character against
// the segment extended a half-length each way and sets state 3 on the near miss.
//
// THE RETURN TYPE IS int AND THE ROM'S CALLER IGNORES IT. The ROM ends both
// exits with mov r0,#1 / mov r0,#0 before the pop, so the body cannot be spelled
// void without losing two words. src/func_ov006_020cfa44.c calls it at
// 0x020cfa50 and reloads its own argument from the stack at 0x020cfa54 without
// reading r0, and that file (already matched) still declares it void. The
// disagreement is between two translation units in C and is harmless -- the
// value comes back in a register nobody reads -- but it is stated here rather
// than left for a reader to trip over.
//
// THIS IS A C++ TU AND THAT IS LOAD-BEARING ON THE HOST, NOT ON THE ROM. The
// element is declared as a class with four virtuals and the three dispatches are
// spelled arr[i]->IsActive(), ->GetPos() and ->GetTargetPos(), which is what
// src/func_ov006_020ce8a0.cpp and src/func_ov006_020ce108.cpp -- the two other
// ROM bodies that dispatch this same vtable -- already do.
//
// It began as a .c file with a struct of function pointers. That spelling
// compiles to the SAME BYTES at 2004/b56 -- verified: same 0x56c, same 339 code
// words, same 8-word pool, the same eight divergence offsets, byte-identical
// object and identical relocations -- and it is WRONG ON THE HOST. The port
// binds this vtable's slots to __fastcall adapters, because MSVC dispatches a
// virtual through ECX. A C function-pointer call is __cdecl, so MSVC pushed the
// receiver on the stack and left ECX holding whatever register allocation had
// last put there. dumpbin of the .c build showed it landing both ways inside
// this one function: at the IsActive site ECX happened to hold the object, and
// at BOTH position sites it held the VPTR, so GetPos returned vptr + 0x24 and
// GetTargetPos returned vptr + 0x30. The body read its two positions out of the
// hosted vtable, its crossing test could never fire, and the trampolines drew
// while every character fell straight through them. The C++ spelling makes the
// receiver arrive by contract instead of by luck, and it changes not one byte of
// what mwccarm emits.
//
// WHAT IS EXACT. The size is the ROM's 0x56c to the byte: 339 code words and 8
// pool words in both. All TWENTY-EIGHT calls sit at the ROM's own byte offsets,
// in the ROM's order -- +0x0b8 +0x0c8 +0x0d8 +0x0e4 +0x0f4 +0x104 +0x114 +0x124
// +0x130 +0x17c +0x18c +0x19c +0x1b0 +0x1c0 +0x214 +0x234 +0x254 +0x270 +0x280
// +0x30c +0x378 +0x3b0 +0x400 +0x430 +0x468 +0x478 +0x488 +0x498 -- and every one
// of the 33 relocation slots resolves to the symbol config records for that
// address. The literal pool is the ROM's, slot for slot and in the ROM's order:
// data_ov006_0214097c, data_ov006_0213b30c, data_ov006_0213b310, 0x1b1,
// data_ov006_0213b2f4, data_ov006_0213b308, 0x1ae, 0x326 -- five relocated
// symbols and three constants, not one slot displaced. The push list, the 0xc4
// frame, both epilogues, the whole segment test, the whole hit tail and the
// whole near-miss tail are instruction for instruction the ROM's.
//
// THE DIVERGENCES, ACCOUNTED FOR IN FULL, AND WHY NEITHER CLASS CAN CHANGE
// BEHAVIOUR. 331 of the ROM's 339 code words align exactly. The eight that do
// not fall into two classes, and both are closed sets -- there is no third
// class and nothing is left out of this list:
//
//  1. THE PRE-LOOP SCHEDULE (6 words: +0x008, +0x00c, +0x010, +0x014, +0x018,
//     +0x030). The ROM emits
//         mov r4,r0 | mov sb,#0 | ldr r7,[pc] | add fp,sp,#0x70 | mov r5,#1
//         | (four constant hoists) | mvn r6,#0 | (one more hoist)
//     and this body emits
//         ldr r7,[pc] | mov r4,r0 | mov sb,#0 | mvn r6,#0 | add fp,sp,#0x70
//         | (four constant hoists) | mov r5,#1 | (one more hoist)
//     THE MULTISET IS IDENTICAL -- the same six instructions, the same six
//     destination registers, the same six values -- and the five constant-hoist
//     stores among them (0x14=2, 0x18=0, 0xc=0, 0x10=0, 0x1c=3) are at the ROM's
//     own offsets in the ROM's order. Each of the six is an independent
//     initialisation of a distinct register from a constant or from r0, with no
//     operand in common with any other, so any permutation of them reaches the
//     same machine state. This is mwcc's emission order for the pre-loop hoist
//     list and nothing else.
//
//  2. THE +0x324 ADDRESSING SPLIT (2 words: +0x40c and +0x418). The ROM reads
//     the remaining count with
//         mov r0, sl                 ldrh r0, [r0, #0x24]
//     and this body with
//         add r0, sl, #0x24          ldrh r0, [r0]
//     with sl = c + 0x300 in both. Both compute the effective address c + 0x324
//     and land the halfword in r0; ldrh's 8-bit displacement is why the ROM
//     splits a 0x324 offset at all. Same address, same destination, same
//     instruction count, at the same two offsets.
//
// Every divergence in this body is therefore a permutation or a re-association
// of the same computation. No arithmetic, no constant, no memory access and no
// control-flow edge differs from the ROM's.
//
// WHAT MOVED THE FIGURE, for the next lane. The banked near-miss row for this
// address measures 91 divergences against this instrument and is one code word
// SHORT of the ROM. Two levers took it to 8:
//
//   * THE s64 ROUND-TRIP ON THE +0x324 TEST, worth 78 and the missing word.
//     Writing the remaining-count test as
//     *(u16 *)(int)(long long)(int)(base300 + 0x24) forces mwcc to materialise
//     the whole address into a register instead of folding +0x24 into the load,
//     which is the word the draft was missing and which re-aligned the entire
//     tail. Laundering the BASE alone does not work: mwcc folds
//     (int)(long long)(int)ptr back to ptr whenever the result is immediately a
//     pointer base, and nine spellings of that shape measured identically to
//     doing nothing. Spelling the read as *(u16 *)(c + 0x324) instead gives the
//     ROM's exact two instructions and flips the whole callee-saved colouring
//     (c moves r4 -> sl and every other local shifts one place, 91 divergences);
//     all 120 declaration orders were compiled against that variant and its best
//     is 91, so that family is worse everywhere, not merely at its default.
//
//   * one = 1 ASSIGNED INSIDE THE LOOP, worth 5. A constant assigned before the
//     loop is emitted from mwcc's pre-loop assignment slot; the same constant
//     assigned inside the loop is picked up by the loop-invariant hoist pass and
//     emitted from the hoist list instead, which is a different position. The ROM
//     wants `one` in the late slot and `negone` in the early one, and this is the
//     only pairing of the two that puts `negone` where the ROM has it. Which
//     statement inside the loop it sits beside does not matter -- eight anchors
//     from the first call down to the clamp itself all measure the same -- but
//     negone will not survive the same move: at any anchor from the projection
//     stores onward it stops being hoisted and the body grows four words (305).
//
// WHAT DID NOT MOVE IT, so nobody pays for it twice. All 120 declaration orders
// of the five pre-loop locals -- the file's order is the best of them, and it is
// the order the ROM's own colouring implies, since i, a, arr, negone, one map to
// sb, r8, r7, r6, r5, descending in declaration order. All 24 assignment orders.
// Thirteen mwcc pragmas: opt_lifetimes, scheduling, peephole, global_optimizer,
// opt_dead_code and the rest are inert here, and only opt_propagation off is
// load-bearing (removing it costs 298). Sixty-four combinations of hoist anchors
// for the two constants. Six structural rewrites: the array indexed directly
// instead of through arr, explicit locals for the hoisted 2/0/3, a for loop in
// place of the do-while, the vector locals lifted to function scope, and both
// initialiser-at-declaration forms. Literals in place of one/negone reproduce the
// ROM's pre-loop SCHEDULE exactly and lose the colouring instead (43).
//
// THE PERMUTER WAS RUN AND ITS OUTPUT WAS REJECTED. decomp-permuter reached
// score 490 against its base 530, and the winning source is worse by every
// measure that matters: its pycparser round-trip split
// (int)(long long)(int)(base300 + 0x24) into a new_var assignment plus
// (int)((long long) new_var), which mwcc folds, so the lever is gone -- the
// output measures 91 divergences and is one code word short of the ROM. The
// permuter's own metric preferred it anyway. Score is not truth.
//
// ONE CALLEE NAME IN THE DRAFT WAS A PLACEHOLDER AND THE LINK CAUGHT IT. The
// banked row called 0x0203d6d0 func_0203d6d0; config/arm9/symbols.txt calls it
// Vec2_Sub and src/Vec2_Sub.c is what defines it, so the first seated link
// failed with one LNK2019 in all three targets. The byte gate could not have
// caught this on its own: match.py wildcards a relocated word and resolves a
// func_<addr> spelling straight back to the address, so the wrong NAME reads as
// a correct destination. Renaming all eight call sites left the figure at 8 --
// a symbol name is not codegen -- and the other fifteen externs were checked
// against config the same way before the rebuild rather than one link at a time.
//
// The other laundering the draft carries is load-bearing and was measured, not
// assumed: the s64 mask on the parameter copy is worth 68, and the s64
// round-trips on the two +0x324/+0x326 read-modify-writes are worth 296 and 292.
// The `#pragma opt_common_subs off` the draft also carried measured inert at
// 2004/b56 and has been removed rather than left to read as a lever.

#pragma opt_propagation off
typedef unsigned char u8;
typedef unsigned short u16;

struct Vec3 { int x, y, z; };
struct V2 { int x, y; };

struct Cannon;
struct SObj {
    char pad[0x324];
    u16 cnt324;
    u16 cnt326;
    u8 state328;
};
struct Cannon {
    virtual struct Vec3 *GetPos();
    virtual struct Vec3 *GetTargetPos();
    virtual void Pad08();
    virtual int IsActive();
    struct Vec3 v4;
    int f10;
    int f14;
    u16 f18;
};

extern "C" void Vec3_Sub(struct Vec3 *out, struct Vec3 *a, struct Vec3 *b);
extern "C" void SubVec3(struct Vec3 *a, struct Vec3 *b, struct Vec3 *c);
extern "C" int DotVec3(struct Vec3 *a, struct Vec3 *b);
extern "C" void Vec2_Sub(int *o, int *a, int *b);
extern "C" int func_0203d524(int *a, int *b);
extern "C" int _ZN4cstd4fdivEii(int a, int b);
extern "C" void func_ov006_020e6db4(int a0, int a1, int a2);
extern "C" void func_ov006_020cf040(char *sl, void *arg1, struct Vec3 *r2);
extern "C" void func_ov006_020cfa28(char *p);
extern "C" void Vec3_MulScalar(struct Vec3 *out, struct Vec3 *in, int scale);
extern "C" void Vec3_Add(struct Vec3 *out, struct Vec3 *a, struct Vec3 *b);

extern "C" struct Cannon *data_ov006_0214097c[];
extern "C" int data_ov006_0213b30c;
extern "C" int data_ov006_0213b310;
extern "C" int data_ov006_0213b2f4;
extern "C" int data_ov006_0213b308;

extern "C" int func_ov006_020cfc74(char *c)
{
    int i;
    int a;
    struct Cannon **arr;
    int negone;
    int one;

    i = 0;
    arr = data_ov006_0214097c;
    negone = -1;
    do {
        int b;
        int cc;
        int dd;
        struct Vec3 pos0;
        struct Vec3 pos1;
        struct Vec3 diff;
        struct V2 p1proj;
        struct V2 p0proj;
        struct V2 negW;
        struct V2 posW;
        struct V2 gateDir;
        struct V2 aTest;
        struct V2 bTest;
        int flag;
        int s1;
        int s2;
        struct Vec3 sumPt;
        struct Vec3 diffPt;
        struct Vec3 farFwd;
        struct Vec3 farBack;
        struct Vec3 *p0;
        struct Vec3 *p1;
        char *s = (char *)((long long)(int)c & 0xFFFFFFFFFFFFFFFFLL);

        if (arr[i] == 0)
            continue;
        if (arr[i]->IsActive() == 0)
            continue;

        p0 = arr[i]->GetPos();
        pos0.x = p0->x;
        pos0.y = p0->y;
        pos0.z = p0->z;

        p1 = arr[i]->GetTargetPos();
        pos1.x = p1->x;
        pos1.y = p1->y;
        pos1.z = p1->z;

        Vec3_Sub(&diff, &pos0, &pos1);
        SubVec3(&pos0, (struct Vec3 *)(s + 8), &pos0);
        SubVec3(&pos1, (struct Vec3 *)(s + 8), &pos1);

        a = DotVec3((struct Vec3 *)(s + 0x20), &pos0);
        b = DotVec3((struct Vec3 *)(s + 0x14), &pos0);
        cc = DotVec3((struct Vec3 *)(s + 0x20), &pos1);
        dd = DotVec3((struct Vec3 *)(s + 0x14), &pos1);
        DotVec3((struct Vec3 *)(s + 0x14), &diff);
        DotVec3((struct Vec3 *)(s + 0x20), &diff);

        p0proj.y = b;
        p1proj.x = cc;
        p0proj.x = a;
        p1proj.y = dd;

        negW.x = -(*(int *)(s + 0x58));
        negW.y = 0;
        posW.x = *(int *)(s + 0x58);
        posW.y = 0;

        Vec2_Sub((int *)&gateDir, (int *)&negW, (int *)&posW);
        Vec2_Sub((int *)&aTest, (int *)&negW, (int *)&p1proj);
        Vec2_Sub((int *)&bTest, (int *)&negW, (int *)&p0proj);

        flag = 0;
        s1 = func_0203d524((int *)&gateDir, (int *)&aTest);
        s2 = func_0203d524((int *)&gateDir, (int *)&bTest);
        one = 1;
        if (s1 < -1) s1 = negone; else if (s1 > 1) s1 = one;
        if (s2 < -1) s2 = negone; else if (s2 > 1) s2 = one;

        if (s1 * s2 <= 0 && s1 > s2) {
            struct V2 edge;
            struct V2 e1;
            struct V2 e2;
            int t1;
            int t2;
            Vec2_Sub((int *)&edge, (int *)&p1proj, (int *)&p0proj);
            gateDir = edge;
            Vec2_Sub((int *)&e1, (int *)&p1proj, (int *)&negW);
            aTest = e1;
            Vec2_Sub((int *)&e2, (int *)&p1proj, (int *)&posW);
            bTest = e2;

            t1 = func_0203d524((int *)&gateDir, (int *)&aTest);
            t2 = func_0203d524((int *)&gateDir, (int *)&bTest);
            if (t1 < -1) t1 = negone; else if (t1 > 1) t1 = one;
            if (t2 < -1) t2 = negone; else if (t2 > 1) t2 = one;
            if (t1 * t2 <= 0) flag = one;
        }

        if (flag != 0) {
            int mag;
            int t;
            *(int *)(s + 0x38) = pos0.x;
            *(int *)(s + 0x3c) = pos0.y;
            *(int *)(s + 0x40) = pos0.z;
            *(int *)(s + 0x44) = diff.x;
            *(int *)(s + 0x48) = diff.y;
            *(int *)(s + 0x4c) = diff.z;

            t = _ZN4cstd4fdivEii((a < 0) ? -a : a, *(int *)(s + 0x58));
            {
                int w = *(int *)(s + 0x58);
                int av = (a < 0) ? -a : a;
                mag = (int)(((long long)av * w + 0x800) >> 12);
            }

            if (mag < 0x400000) {
                arr[i]->f18 = 2;
                func_ov006_020e6db4(0x1b1, *(int *)(s + 8),
                    (data_ov006_0213b310 * t + data_ov006_0213b30c * (0x1000 - t)) >> 12);
            } else {
                arr[i]->f18 = one;
                func_ov006_020e6db4(0x1ae, *(int *)(s + 8),
                    (data_ov006_0213b308 * t + data_ov006_0213b2f4 * (0x1000 - t)) >> 12);
            }

            {
                int *dst = (int *)((char *)arr[i] + 4);
                char *base300;
                dst[0] = *(int *)(s + 0x14);
                dst[1] = *(int *)(s + 0x18);
                base300 = s + 0x300;
                dst[2] = *(int *)(s + 0x1c);
                ((int *)arr[i])[4] = a;
                ((int *)arr[i])[5] = *(int *)(s + 0x58);
                *(u16 *)(base300 + 0x20) = 0;
                func_ov006_020cf040(s, (void *)(s + 0x38), (struct Vec3 *)(s + 0x44));

                {
                    char *pbase = s + (a - a);
                    u16 *p = (u16 *)(int)(((long long)(int)(pbase + 0x324)));
                    *p = (u16)(*p - 1);
                }
                if (*(u16 *)(int)(long long)(int)(base300 + 0x24) == 0) {
                    *(u8 *)(s + 0x328) = 3;
                    func_ov006_020cfa28(s);
                    return 1;
                }
                {
                    char *pbase = s + (a - a);
                    u16 *p = (u16 *)(int)(((long long)(unsigned)(pbase + 0x326)));
                    *p = (u16)(*p + 1);
                }
            }
        } else {
            Vec3_MulScalar(&farFwd, (struct Vec3 *)(s + 0x20), *(int *)(s + 0x58));
            Vec3_Add(&sumPt, &pos0, &farFwd);
            Vec3_MulScalar(&farBack, (struct Vec3 *)(s + 0x20), *(int *)(s + 0x58));
            Vec3_Sub(&diffPt, &pos0, &farBack);

            if (sumPt.y < 0 && sumPt.y > -0x30000) {
                if (sumPt.x > -0x8000 && sumPt.x < 0) {
                    if (diff.x > 0) {
                        arr[i]->f18 = 3;
                    }
                }
            } else {
                if (diffPt.y < 0 && diffPt.y > -0x30000 && diffPt.x > 0 && diffPt.x < 0x8000 && diff.x < 0) {
                    arr[i]->f18 = 3;
                }
            }
        }
    } while (++i < 5);
    return 0;
}
