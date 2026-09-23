//cpp
/* dScMgCurling2_c: the two-player curling minigame scene, ov006.
 *
 * This file holds the lower 31 functions of the class (.text
 * 0x020e3854..0x020e5450). func_ov006_020e5450, next in ROM, is not matched
 * yet and the ROM's own bytes cover it, so the 21 functions above it
 * (InitResources, Behavior, Render, OnYoshiTryEat and the rest) still live
 * in their own files.
 *
 * Functions run in ROM order, lowest address first, because of
 * `#pragma defer_codegen off` below; do not reorder, and do not drop that
 * pragma. It also makes the out-of-line destructor come out D1, D0 as in
 * the ROM (the unused D2 trails it and is deadstripped), and it is what
 * lets the push and pop brackets on func_ov006_020e4b00 and DragUpdate
 * bind. Both brackets are needed; each was measured.
 *
 * The destructor is declared first and out of line, so this file owns the
 * key function and emits the vtable and RTTI for the whole base chain.
 *
 * Two arrays inside the object drive most of this file. The ROM names
 * neither, so the comments call them by offset:
 *
 *   0x48c0, 0x32 entries of 0x24: +0x00 and +0x04 an x,y pair in 20.12;
 *     +0x08 and +0x0c the per-axis increments; +0x10 a value the +0x0c
 *     increment ramps toward; +0x14, +0x16 and +0x18 three 16-bit
 *     countdowns; +0x1c update enable and +0x20 draw enable; +0x1d, +0x1e
 *     and +0x1f indices into the pointer-to-member tables
 *     data_ov006_02141988, _021419f8 or _021419b8, and _021419a0; +0x21 and
 *     +0x22 two sprite indices into data_ov006_0213a5e0. Entries are seeded
 *     across the top of the screen and recycled once y passes 0xc8, so they
 *     fall down the screen and wrap (falling snow is the likely reading, but
 *     it is not proven).
 *
 *   0x4fe0, 0x3c entries of 0x18: +0x00 and +0x04 an x,y pair in 20.12;
 *     +0x0c a y increment that decays by 0x40 a frame; +0x10 a 16-bit
 *     lifetime; +0x12 a 16-bit value drawn as the sprite's number; +0x14 a
 *     live flag and +0x15 a 1-or-2 mode. One is spawned between two stones
 *     whenever they collide (SpawnValue).
 *
 * Still raw:
 * - Both arrays and the 0x4870 records are untyped in dScMgCurling2_c.h, so
 *   they are reached by offset. Giving them structs is what unblocks
 *   typed access here.
 * - The twelve func_ov006_ functions keep their C names because the
 *   unpromoted files above the gap (and decl_common.h) call them by those
 *   names. They become methods when that half is promoted.
 * - The seventeen methods have coined names; the header gives the
 *   derivation from the pointer-to-member tables.
 * - cstd::atan2 takes Fix12<int> by value, so it stays mangled.
 * - decl_common.h is left out: eight of its declarations of these
 *   functions disagree with the matched definitions.
 */

#pragma defer_codegen off

#include "types.h"
#include "dScMgCurling2_c.h"

/* The receiver for the pointer-to-member tables. It must stay incomplete:
 * mwccarm picks the pointer-to-member layout from whether the class is
 * complete. */
struct C;
typedef void (C::*PMF)(int);

/* Everything this file calls or reads that no included header declares.
 * Keep it above the first function. */
extern "C" {

extern int  func_ov004_020adbc0(void);
extern void func_ov004_020adb1c(int self);
extern void func_ov004_020af948(int a, int b, int c, int d);
extern void func_ov004_020b1a5c(int a, int b);
extern void func_ov004_020b1ea4(int a, int b, int c, int d, int e, int f, int g);
extern void func_ov004_020b2444(int a, int b, int c, int d, int e, int f, int g);
extern int  func_ov004_020afdd0(int a, int b, int c, int d, int e);
extern void func_02012718(int id, int v);
extern void DrawOamSprite(int a, int b, int c, int d);
extern void RenderOamBothScreens(void *a0, int a1, int a2, int a3, int a4, void *a5);
extern int  RandomIntInternal(int *seed);
extern int  GetGameLanguage(void);
extern int  _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);

extern int            data_0209d4b8;
extern u8             data_020a0e40;
extern u8             data_020a0de8[];
extern u8             data_020a0de9[];
extern u8             data_020a0dea[];
extern u8             data_020a0deb[];
extern s16            data_02082214[];
extern unsigned char  data_ov006_0212e4f4[];
extern unsigned char  data_ov006_0212e4f8[];
extern int            data_ov006_0212e4fc[];
extern int            data_ov006_0213a5e0[];
extern int            data_ov006_0213c3fc;
extern int            data_ov006_0213c44c;
extern int            data_ov006_0213c454;
extern int           *data_ov006_0213c4dc[];
extern int            data_ov006_0213c4f0[];

/* The four pointer-to-member tables this side of the hole dispatches through.
   __sinit_ov006_02130758 fills them from the 8-byte {code pointer, zero
   adjustment} records in ov006 .data. */
extern PMF data_ov006_02141988[];
extern PMF data_ov006_021419a0[];
extern PMF data_ov006_021419b8[];
extern PMF data_ov006_021419f8[];

}  /* extern "C" */

namespace cstd  { s32 sqrt(u64 value); }
namespace Sound { void PlayBank2_2D(unsigned int id); }


// @symbol _ZN15dScMgCurling2_cD1Ev
// @symbol _ZN15dScMgCurling2_cD0Ev
/* One out-of-line definition; with codegen not deferred it emits D1, then
 * D0, as in the ROM. The body is only the compiler-generated vtable store
 * and base destructor call. */
dScMgCurling2_c::~dScMgCurling2_c()
{
}


// @symbol func_ov006_020e38b0
/* Draw every live entry of the 0x4fe0 array, mode picking
 * the last argument. */
extern "C" void func_ov006_020e38b0(char *raw)
{
    int i;
    char *entry = raw;
    for (i = 0; i < 0x3c; i++) {
        unsigned char mode = *(unsigned char *)(entry + 0x4ff5);
        if (mode != 0) {
            int x = *(int *)(entry + 0x4fe0) >> 0xc;
            int y = *(int *)(entry + 0x4fe4) >> 0xc;
            int value = *(unsigned short *)(entry + 0x4ff2);
            if (mode == 1) {
                func_ov004_020b1ea4(x, y, value, -1, -1, 0, 0);
            } else {
                func_ov004_020b1ea4(x, y, value, -1, -1, 0, 0x32);
            }
        }
        entry += 0x18;
    }
}


// @symbol func_ov006_020e3948
/* Age the 0x4fe0 array one frame: run the lifetime down,
 * apply and decay the y increment, and clear the entry when it expires. */
extern "C" void func_ov006_020e3948(char *p)
{
    int i;
    for (i = 0; i < 0x3c; i++, p += 0x18)
    {
        if (*(unsigned char *)(p + 0x4ff4) != 0)
        {
            if (*(unsigned short *)(p + 0x4ff0) != 0)
            {
                *(unsigned short *)(p + 0x4ff0) -= 1;
                *(int *)(p + 0x4fe4) += *(int *)(p + 0x4fec);
                *(int *)(p + 0x4fec) -= 0x40;
            }
            else
            {
                *(unsigned char *)(p + 0x4ff4) = 0;
                *(unsigned char *)(p + 0x4ff5) = 0;
            }
        }
    }
}


// @symbol _ZN15dScMgCurling2_c10SpawnValueEii
/* Take the first free 0x4fe0 entry and place it midway between the two
 * stones. The payload is (n+1)*10 when either stone
 * carries the 0x468d flag and (n+1)*100 when neither does, where n is the
 * 0x55bf counter, saturating at 0x17.
 *
 * `raw + 0x4000 + other * 0x30 + 0x660` is the same kind of record as
 * `raw + stone * 0x30 + 0x4660` on the other operand. Folding the split
 * base into one constant changes the code: the ROM shares raw + 0x4000
 * across the `other` reads. Do not tidy. */
void dScMgCurling2_c::SpawnValue(int stone, int other)
{
    char *raw = (char *)this;
    int i;
    char *entry;
    int sx;
    int sy;
    unsigned char *count;

    entry = raw;
    for (i = 0; i < 0x3c; i++, entry += 0x18) {
        if (*(unsigned char *)(entry + 0x4ff4) != 0) continue;

        sx = *(int *)(raw + stone * 0x30 + 0x4660) + *(int *)(raw + 0x4000 + other * 0x30 + 0x660);
        sy = *(int *)(raw + stone * 0x30 + 0x4664) + *(int *)(raw + 0x4000 + other * 0x30 + 0x664);

        *(unsigned char *)(raw + i * 0x18 + 0x4ff4) = 1;
        *(unsigned char *)(raw + 0x4ff5 + i * 0x18) = 1;
        *(int *)(raw + i * 0x18 + 0x4fe0) = sx >> 1;
        *(int *)(raw + i * 0x18 + 0x4fe4) = sy >> 1;
        *(unsigned short *)(raw + i * 0x18 + 0x4ff0) = 0x40;
        *(int *)(raw + i * 0x18 + 0x4fe8) = 0;
        *(int *)(raw + i * 0x18 + 0x4fec) = 0;

        /* The && and || arms really do compute the same value; collapsing them
           into one `||` changes the code, so the ROM branched twice too. */
        if (*(unsigned char *)(raw + stone * 0x30 + 0x468d) != 0 && *(unsigned char *)(raw + 0x4000 + other * 0x30 + 0x68d) != 0) {
            *(unsigned short *)(raw + 0x4ff2 + i * 0x18) = (*(unsigned char *)(raw + 0x55bf) + 1) * 10;
        } else if (*(unsigned char *)(raw + stone * 0x30 + 0x468d) != 0 || *(unsigned char *)(raw + 0x4000 + other * 0x30 + 0x68d) != 0) {
            *(unsigned short *)(raw + 0x4ff2 + i * 0x18) = (*(unsigned char *)(raw + 0x55bf) + 1) * 10;
        } else {
            *(unsigned short *)(raw + 0x4ff2 + i * 0x18) = (*(unsigned char *)(raw + 0x55bf) + 1) * 100;
            *(unsigned char *)(raw + 0x4ff5 + i * 0x18) = 2;
        }

        count = (unsigned char *)(raw + 0x55bf);
        *count = *count + 1;
        if (*(unsigned char *)(raw + 0x55bf) >= 0x17) *(unsigned char *)(raw + 0x55bf) = 0x17;
        func_ov004_020adb1c(*(unsigned short *)(raw + 0x4ff2 + i * 0x18) + func_ov004_020adbc0());
        return;
    }
}


// @symbol func_ov006_020e3b9c
/* Clear the 0x4fe0 array. */
extern "C" void func_ov006_020e3b9c(char *p)
{
    int i;
    for (i = 0; i < 0x3c; i++) {
        *(unsigned char *)(p + 0x4ff4) = 0;
        *(unsigned char *)(p + 0x4ff5) = 0;
        p += 0x18;
    }
}


// @symbol func_ov006_020e3bc4
/* Draw every draw-enabled entry of the 0x48c0 array as two
 * stacked sprites from data_ov006_0213a5e0. */
extern "C" void func_ov006_020e3bc4(char *c)
{
    int i;
    for (i = 0; i < 0x32; i++) {
        if (*(unsigned char *)(c + 0x48e0)) {
            int x = *(int *)(c + 0x48c0) >> 0xc;
            int y = *(int *)(c + 0x48c4) >> 0xc;
            func_ov004_020af948(data_ov006_0213a5e0[*(unsigned char *)(c + 0x48e1)], x, y, 0);
            DrawOamSprite(data_ov006_0213a5e0[*(unsigned char *)(c + 0x48e2)], x, y, 0);
        }
        c += 0x24;
    }
}


// @symbol _ZN15dScMgCurling2_c13StepYRampDownEi
/* 0x48c0 callback: run the +0x18 countdown down, else ease
 * +0x0c back toward 0x100, else clear the +0x1f index. */
void dScMgCurling2_c::StepYRampDown(int entry)
{
    char *raw = (char *)this;
    int off = entry * 0x24;
    if (*(unsigned short *)(raw + 0x48d8 + off) != 0) {
        short *p = (short *)(raw + 0x48d8 + off);
        *p = (short)(*(unsigned short *)p - 1);
        if (*p < 0)
            *p = 0;
    } else if (*(int *)(raw + 0x48cc + off) > 0x100) {
        int *q = (int *)(raw + 0x48cc + off);
        *q = *q - 0x10;
        if ((short)*q < 0x100)
            *q = 0x100;
    } else {
        *(unsigned char *)(raw + off + 0x48df) = 0;
    }
}


// @symbol _ZN15dScMgCurling2_c11StepYRampUpEi
/* 0x48c0 callback: ramp +0x0c up toward +0x10, then run the
 * +0x18 countdown down, reseeding it and the +0x1f index when it expires. */
void dScMgCurling2_c::StepYRampUp(int entry)
{
    char *raw = (char *)this;
    int off = entry * 0x24;
    if (*(int *)(raw + 0x48d0 + off) > *(int *)(raw + 0x48cc + off)) {
        *(int *)(raw + 0x48cc + off) += 0x10;
        if (*(int *)(raw + 0x48d0 + off) > *(int *)(raw + 0x48cc + off))
            *(int *)(raw + 0x48cc + off) = *(int *)(raw + 0x48d0 + off);
    }
    if (*(unsigned short *)(raw + 0x48d8 + off) != 0) {
        *(unsigned short *)(raw + 0x48d8 + off) = *(unsigned short *)(raw + 0x48d8 + off) - 1;
        if (*(short *)(raw + 0x48d8 + off) < 0) *(short *)(raw + 0x48d8 + off) = 0;
    } else {
        *(unsigned char *)(raw + off + 0x48df) = 2;
        *(short *)(raw + 0x48d8 + off) = (short)(unsigned char)((((0x20 * (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff)) >> 0xf)) + 0x20);
    }
}


// @symbol _ZN15dScMgCurling2_c12StepYRestartEi
/* 0x48c0 callback: zero +0x0c, pick a fresh +0x10 target and
 * +0x18 countdown, and set the +0x1f index to 1. */
void dScMgCurling2_c::StepYRestart(int entry)
{
    char *raw = (char *)this;
    int off = entry * 0x24;
    unsigned int rnd;

    *(int *)(raw + 0x48cc + off) = 0;
    rnd = ((unsigned)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    *(int *)(raw + 0x48d0 + off) = (((rnd << 4) >> 15) << 4) + 0x300;
    *(unsigned char *)(raw + 0x48df + off) = 1;
    rnd = ((unsigned)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    rnd = ((rnd << 5) >> 15) + 0x20;
    *(short *)(raw + 0x48d8 + off) = (unsigned char)rnd;
}


// @symbol _ZN15dScMgCurling2_c11StepXSettleEi
/* 0x48c0 callback: step the position, hold while a
 * countdown runs, then bleed the x increment toward zero 8 a frame from either
 * side and clear the +0x1e index once it lands.  The hold tests +0x14 and
 * decrements +0x16 -- the ROM's own asymmetry, and StepXPushNeg and StepXPushPos do
 * not share it. */
void dScMgCurling2_c::StepXSettle(int entry)
{
    char *base = (char *)this;
    int off = entry * 0x24;
    /* Three cached bases, declared in the ROM's own load order.  Reordering
       them, or folding them back into base + constant, moves bytes. */
    char *vx = base + 0x48c8;
    char *x = base + 0x48c0;
    char *y = base + 0x48c4;
    *(int *)(x + off) = *(int *)(x + off) + *(int *)(vx + off);
    *(int *)(y + off) = *(int *)(y + off) + *(int *)(base + off + 0x48cc);
    if (*(u16 *)(base + off + 0x48d4) != 0) {
        char *timer = base + 0x48d6;
        *(u16 *)(timer + off) = *(u16 *)(timer + off) - 1;
        if (*(s16 *)(timer + off) < 0) *(s16 *)(timer + off) = 0;
        return;
    }
    if (*(int *)(vx + off) > 0) {
        *(int *)(vx + off) = *(int *)(vx + off) - 8;
        if ((s16)*(int *)(vx + off) < 0) *(int *)(vx + off) = 0;
        return;
    }
    if (*(int *)(vx + off) < 0) {
        *(int *)(vx + off) = *(int *)(vx + off) + 8;
        if (*(int *)(vx + off) > 0) *(int *)(vx + off) = 0;
        return;
    }
    *(u8 *)(base + off + 0x48de) = 0;
}


// @symbol _ZN15dScMgCurling2_c12StepXPushNegEi
/* 0x48c0 callback: step the position, hold for the +0x14
 * countdown, then push the x increment negative to a -0x300 floor, hold again
 * for +0x16, and hand over to index 3 with a fresh countdown. */
void dScMgCurling2_c::StepXPushNeg(int entry)
{
    char *base = (char *)this;
    int off = entry * 0x24;
    unsigned short count;

    *(int *)(base + 0x48c0 + off) = *(int *)(base + 0x48c0 + off) + *(int *)(base + 0x48c8 + off);
    *(int *)(base + 0x48c4 + off) = *(int *)(base + 0x48c4 + off) + *(int *)(base + 0x48cc + off);

    if (*(unsigned short *)(base + 0x48d4 + off) != 0) {
        *(unsigned short *)(base + 0x48d4 + off) = *(unsigned short *)(base + 0x48d4 + off) - 1;
        if (*(short *)(base + 0x48d4 + off) < 0)
            *(unsigned short *)(base + 0x48d4 + off) = 0;
        return;
    }

    if (*(int *)(base + 0x48c8 + off) > -0x300) {
        *(int *)(base + 0x48c8 + off) -= 8;
        if (*(int *)(base + 0x48c8 + off) <= -0x300)
            *(int *)(base + 0x48c8 + off) = 0x300;
    }

    count = *(unsigned short *)(base + 0x48d6 + off);
    if (count != 0) {
        *(unsigned short *)(base + 0x48d6 + off) = count - 1;
        if (*(short *)(base + 0x48d6 + off) < 0)
            *(unsigned short *)(base + 0x48d6 + off) = 0;
        return;
    }

    *(unsigned char *)(base + 0x48de + off) = 3;
    *(unsigned short *)(base + 0x48d6 + off) = (unsigned char)(((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 5 >> 0xf) + 0x20);
}


// @symbol _ZN15dScMgCurling2_c12StepXPushPosEi
/* The mirror of StepXPushNeg: same shape, x increment pushed the
 * other way to a +0x300 ceiling. */
void dScMgCurling2_c::StepXPushPos(int entry)
{
    char *base = (char *)this;
    int off = entry * 0x24;
    unsigned short count;

    *(int *)(base + 0x48c0 + off) += *(int *)(base + 0x48c8 + off);
    *(int *)(base + 0x48c4 + off) += *(int *)(base + 0x48cc + off);

    count = *(unsigned short *)(base + 0x48d4 + off);
    if (count != 0) {
        *(short *)(base + 0x48d4 + off) = count - 1;
        if (*(short *)(base + 0x48d4 + off) < 0)
            *(short *)(base + 0x48d4 + off) = 0;
        return;
    }

    if (*(int *)(base + 0x48c8 + off) < 0x300) {
        *(int *)(base + 0x48c8 + off) += 8;
        if (*(int *)(base + 0x48c8 + off) >= 0x300)
            *(int *)(base + 0x48c8 + off) = 0x300;
    }

    count = *(unsigned short *)(base + 0x48d6 + off);
    if (count != 0) {
        *(short *)(base + 0x48d6 + off) = count - 1;
        if (*(short *)(base + 0x48d6 + off) < 0)
            *(short *)(base + 0x48d6 + off) = 0;
        return;
    }

    *(char *)(base + 0x48de + off) = 3;
    *(short *)(base + 0x48d6 + off) = (((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 5) >> 0xf) + 0x20 & 0xff;
}


// @symbol _ZN15dScMgCurling2_c9StepXPickEi
/* 0x48c0 callback: hold for the +0x14 countdown, then zero
 * the x increment, pick the next +0x1e index out of data_ov006_0212e4f4 and
 * reseed both countdowns. */
void dScMgCurling2_c::StepXPick(int entry)
{
    char *raw = (char *)this;
    int off = entry * 0x24;
    if (*(unsigned short *)(raw + 0x48d4 + off) != 0) {
        *(unsigned short *)(raw + 0x48d4 + off) = *(unsigned short *)(raw + 0x48d4 + off) - 1;
        if (*(short *)(raw + 0x48d4 + off) < 0) *(short *)(raw + 0x48d4 + off) = 0;
        return;
    }
    *(int *)(raw + 0x48c8 + off) = 0;
    *(unsigned char *)(raw + 0x48de + off) = data_ov006_0212e4f4[(((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 1 >> 15];
    *(unsigned short *)(raw + 0x48d4 + off) = (short)(unsigned char)((0x10 * (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff)) >> 0xf);
    *(unsigned short *)(raw + 0x48d6 + off) = (short)(unsigned char)(((0x40 * (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff)) >> 0xf) + 0x60);
}


// @symbol _ZN15dScMgCurling2_c9StepXAndYEi
/* Run one 0x48c0 entry's two pointer-to-member callbacks,
 * indexed by the +0x1e and +0x1f state bytes.
 *
 * The second load really goes through `(char *)self` where the first goes
 * through `raw`; making the two agree changes the code. */
void dScMgCurling2_c::StepXAndY(int entry)
{
    char *raw = (char *)this;
    C *self = (C *)raw;
    int off = entry * 0x24;
    unsigned char k0 = *(unsigned char *)(raw + off + 0x48de);
    (self->*data_ov006_021419f8[k0])(entry);
    unsigned char k1 = *(unsigned char *)((char *)self + off + 0x48df);
    (self->*data_ov006_021419a0[k1])(entry);
}


// @symbol _ZN15dScMgCurling2_c15StepXSettleFastEi
/* 0x48c0 callback: step the position, then bleed the x
 * increment toward zero 0x20 a frame and clear the +0x1e index on arrival. */
void dScMgCurling2_c::StepXSettleFast(int entry)
{
    char *base = (char *)this;
    int off = entry * 0x24;
    /* x, vx, y -- the ROM's load order, not a tidy one.  Do not reorder. */
    int *x = (int *)(base + 0x48c0 + off);
    int *vx = (int *)(base + 0x48c8 + off);
    int *y = (int *)(base + 0x48c4 + off);
    *x += *vx;
    *y += *(int *)(base + off + 0x48cc);
    if (*vx > 0) {
        *vx -= 0x20;
        if ((int)(short)*vx < 0) *vx = 0;
    } else if (*vx < 0) {
        *vx += 0x20;
        if (*vx > 0) *vx = 0;
    } else {
        *(unsigned char *)(base + off + 0x48de) = 0;
    }
}


// @symbol _ZN15dScMgCurling2_c16StepXPushNegFastEi
/* 0x48c0 callback: as StepXPushNeg at 0x20 a frame and a -0x400
 * floor, handing over to index 3 without reseeding the countdown. */
void dScMgCurling2_c::StepXPushNegFast(int entry)
{
    char *raw = (char *)this;
    int off = entry * 0x24;

    *(int *)(raw + 0x48c0 + off) =
        *(int *)(raw + 0x48c0 + off) + *(int *)(raw + 0x48c8 + off);
    *(int *)(raw + 0x48c4 + off) =
        *(int *)(raw + 0x48c4 + off) + *(int *)(raw + 0x48cc + off);

    if (*(u16 *)(raw + 0x48d4 + off) != 0) {
        *(s16 *)(raw + 0x48d4 + off) =
            (s16)(*(u16 *)(raw + 0x48d4 + off) - 1);
        if (*(s16 *)(raw + 0x48d4 + off) < 0)
            *(s16 *)(raw + 0x48d4 + off) = 0;
        return;
    }

    if (*(int *)(raw + 0x48c8 + off) > -0x400) {
        *(int *)(raw + 0x48c8 + off) -= 0x20;
        if (*(int *)(raw + 0x48c8 + off) <= -0x400)
            *(int *)(raw + 0x48c8 + off) = 0x400;
    }

    if (*(u16 *)(raw + 0x48d6 + off) != 0) {
        *(s16 *)(raw + 0x48d6 + off) =
            (s16)(*(u16 *)(raw + 0x48d6 + off) - 1);
        if (*(s16 *)(raw + 0x48d6 + off) < 0)
            *(s16 *)(raw + 0x48d6 + off) = 0;
        return;
    }

    *(unsigned char *)(raw + 0x48de + off) = 3;
}


// @symbol _ZN15dScMgCurling2_c16StepXPushPosFastEi
/* The mirror of StepXPushNegFast, x increment pushed to a +0x400
 * ceiling. */
void dScMgCurling2_c::StepXPushPosFast(int entry)
{
    char *raw = (char *)this;
    int off = entry * 0x24;

    *(int *)(raw + 0x48c0 + off) += *(int *)(raw + 0x48c8 + off);
    *(int *)(raw + 0x48c4 + off) += *(int *)(raw + 0x48cc + off);

    {
        unsigned short count = *(unsigned short *)(raw + 0x48d4 + off);
        if (count != 0) {
            *(short *)(raw + 0x48d4 + off) = (short)(count - 1);
            if (*(short *)(raw + 0x48d4 + off) < 0) {
                *(short *)(raw + 0x48d4 + off) = 0;
            }
            return;
        }
    }

    if (*(int *)(raw + 0x48c8 + off) < 0x400) {
        *(int *)(raw + 0x48c8 + off) += 0x20;
        if (*(int *)(raw + 0x48c8 + off) >= 0x400) {
            *(int *)(raw + 0x48c8 + off) = 0x400;
        }
    }

    {
        unsigned short count2 = *(unsigned short *)(raw + 0x48d6 + off);
        if (count2 != 0) {
            *(short *)(raw + 0x48d6 + off) = (short)(count2 - 1);
            if (*(short *)(raw + 0x48d6 + off) < 0) {
                *(short *)(raw + 0x48d6 + off) = 0;
            }
            return;
        }
    }

    *(unsigned char *)(raw + 0x48de + off) = 3;
}


// @symbol _ZN15dScMgCurling2_c13StepXPickFastEi
/* 0x48c0 callback: hold for the +0x14 countdown, then zero
 * the x increment, pick a fresh y increment and a +0x1e index out of
 * data_ov006_0212e4f8, and reseed both countdowns. */
void dScMgCurling2_c::StepXPickFast(int entry)
{
    char *raw = (char *)this;
    int off = entry * 0x24;
    if (*(unsigned short *)(raw + 0x48d4 + off) != 0) {
        *(unsigned short *)(raw + 0x48d4 + off) = *(unsigned short *)(raw + 0x48d4 + off) - 1;
        if (*(short *)(raw + 0x48d4 + off) < 0) *(short *)(raw + 0x48d4 + off) = 0;
        return;
    }
    *(int *)(raw + 0x48c8 + off) = 0;
    *(int *)(raw + 0x48cc + off) = ((((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 5) >> 0xf) << 4) + 0x600;
    *(unsigned char *)(raw + 0x48de + off) = data_ov006_0212e4f8[(((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 1 >> 15];
    *(unsigned short *)(raw + 0x48d4 + off) = (unsigned short)((((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 4) >> 0xf) & 0xff);
    *(unsigned short *)(raw + 0x48d6 + off) = (unsigned short)(((((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 0x30) >> 0xf) + 0x30) & 0xff);
}


// @symbol _ZN15dScMgCurling2_c9StepXOnlyEi
/* Run one 0x48c0 entry's data_ov006_021419b8 callback,
 * indexed by the +0x1e state byte. */
void dScMgCurling2_c::StepXOnly(int entry)
{
    char *raw = (char *)this;
    unsigned char idx = *(unsigned char *)(raw + entry * 0x24 + 0x48de);
    (((C *)raw)->*data_ov006_021419b8[idx])(entry);
}


// @symbol _ZN15dScMgCurling2_c12PickStepModeEi
/* 0x48c0 callback: pick the +0x1d index out of
 * data_ov006_0212e4fc (entry 1 on one roll in eight) and clear +0x1e.
 */
void dScMgCurling2_c::PickStepMode(int entry)
{
    char *raw = (char *)this;
    unsigned rnd = (unsigned)RandomIntInternal(&data_0209d4b8);
    int pick = 0;
    unsigned roll = ((rnd >> 16) & 0x7fff) << 3 >> 0xf;
    if (roll == 5) pick = 1;
    char *rec = raw + entry * 0x24;
    *(unsigned char *)(rec + 0x48dd) = data_ov006_0212e4fc[pick];
    *(unsigned char *)(rec + 0x48de) = 0;
}


// @symbol func_ov006_020e4800
/* Step every update-enabled 0x48c0 entry through its
 * data_ov006_02141988 callback, and reseed the ones whose y has passed 0xc8
 * back to a random x at y = -0x8000. */
extern "C" void func_ov006_020e4800(char *raw)
{
    int i;
    char *entry = raw;
    for (i = 0; i < 0x32; i++) {
        if (*(unsigned char *)(entry + 0x48dc) != 0) {
            unsigned char idx = *(unsigned char *)(entry + 0x48dd);
            (((C *)raw)->*data_ov006_02141988[idx])(i);
            if ((*(int *)(entry + 0x48c4) >> 0xc) >= 0xc8) {
                *(int *)(entry + 0x48c0) = (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 5 >> 0xf << 0xf;
                *(int *)(entry + 0x48c4) = -0x8000;
                *(unsigned char *)(entry + 0x48de) = 0;
                *(unsigned char *)(entry + 0x48dd) = 0;
                *(unsigned char *)(entry + 0x48df) = 0;
            }
        }
        entry += 0x24;
    }
}


// @symbol func_ov006_020e48d4
/* Zero all 0x32 entries of the 0x48c0 array, then seed each
 * one: enabled, at a random x, with two random sprite indices whose second is
 * the first plus one to four modulo five, and a y spread over the screen.  The
 * first x, the y at -0x8000 and the first +0x14 store are all overwritten a few
 * lines later; the dead stores are the ROM's, not a merge artifact. */
extern "C" void func_ov006_020e48d4(char *raw)
{
    int i;
    char *entry;
    unsigned int rnd;
    unsigned int v;
    int q;
    unsigned int m;

    i = 0;
    entry = raw;
    for (; i < 0x32; i++)
    {
        *(int *)(entry + 0x48c0) = 0;
        *(int *)(entry + 0x48c4) = 0;
        *(int *)(entry + 0x48c8) = 0;
        *(int *)(entry + 0x48cc) = 0;
        *(short *)(entry + 0x48d4) = 0;
        *(short *)(entry + 0x48d6) = 0;
        *(short *)(entry + 0x48d8) = 0;
        *(char *)(entry + 0x48dc) = 0;
        *(char *)(entry + 0x48dd) = 0;
        *(char *)(entry + 0x48de) = 0;
        *(char *)(entry + 0x48df) = 0;
        *(char *)(entry + 0x48e0) = 0;
        *(char *)(entry + 0x48e1) = 0;
        *(char *)(entry + 0x48e2) = 1;
        entry += 0x24;
    }

    i = 0;
    entry = raw;
    for (; i < 0x32; i++)
    {
        rnd = (unsigned int)RandomIntInternal(&data_0209d4b8);
        m = ((rnd >> 16) & 0x7fff) << 5;
        *(int *)(entry + 0x48c0) = (int)((m >> 0xf)) << 0xf;
        *(int *)(entry + 0x48c4) = -0x8000;
        *(char *)(entry + 0x48dc) = 1;
        *(char *)(entry + 0x48e0) = 1;
        *(char *)(entry + 0x48dd) = 0;
        *(char *)(entry + 0x48de) = 0;

        rnd = (unsigned int)RandomIntInternal(&data_0209d4b8);
        *(char *)(entry + 0x48e1) = (char)(((rnd >> 16) & 0x7fff) * 5 >> 0xf);

        rnd = (unsigned int)RandomIntInternal(&data_0209d4b8);
        v = *(unsigned char *)(entry + 0x48e1) + ((((rnd >> 16) & 0x7fff) << 2) >> 0xf) + 1;
        v = v & 0xff;
        if (v >= 5)
            v = (v - 5) & 0xff;
        *(char *)(entry + 0x48e2) = (char)v;

        rnd = (unsigned int)RandomIntInternal(&data_0209d4b8);
        *(short *)(entry + 0x48d4) = (short)(((i & 7) << 6) + (((rnd >> 16) & 0x7fff) * 0x30 >> 0xf));

        rnd = (unsigned int)RandomIntInternal(&data_0209d4b8);
        m = ((rnd >> 16) & 0x7fff) << 5;
        *(int *)(entry + 0x48c0) = (int)((m >> 0xf)) << 0xf;

        rnd = (unsigned int)RandomIntInternal(&data_0209d4b8);
        q = (((rnd >> 16) & 0x7fff) * 0x1a) >> 0xf;
        *(int *)(entry + 0x48c4) = (((q << 3) - 8)) << 0xc;
        *(short *)(entry + 0x48d4) = 0;
        entry += 0x24;
    }
}


// @symbol func_ov006_020e4a84
/* Draw the five 0x10-byte records at 0x4870 whose +0x0d
 * flag is set, each with its +0x08 value as the caption.
 *
 * `(i & 0xFFFFFFFF)` stays: 0xFFFFFFFF is unsigned, so the mask makes this
 * subscript unsigned, and deleting it changes the code. */
extern "C" void func_ov006_020e4a84(unsigned char *raw)
{
    int x;
    int i;

    func_ov004_020b1a5c(func_ov004_020adbc0(), 6);
    for (i = 0; i < 5; i++) {
        unsigned char *rec = raw + (i & 0xFFFFFFFF) * 0x10;
        if (*(unsigned char *)(rec + 0x487d)) {
            x = *(int *)(rec + 0x4870) >> 12;
            func_ov004_020b2444(x, *(int *)(rec + 0x4874) >> 12,
                                *(unsigned short *)(rec + 0x4878), -1, -1, 0, 0);
        }
    }
}


/* Age the five 0x4870 records: run the +0x0a countdown down
 * and, on the frame it reaches zero, set the +0x0d flag that func_ov006_020e4a84 draws on and
 * play sound 0x1bc. The `opt_strength_reduction off` bracket is needed
 * (see the banner). */
#pragma push
#pragma opt_strength_reduction off
// @symbol func_ov006_020e4b00
extern "C" void func_ov006_020e4b00(char *raw)
{
    int i;

    for (i = 0; i < 5; i++) {
        char *rec = raw + (i << 4);
        unsigned short *timer;
        if (*(unsigned char *)(rec + 0x487c) == 0) continue;
        if (*(unsigned short *)(rec + 0x487a) == 0) continue;
        timer = (unsigned short *)(rec + 0x487a);
        *timer = *timer - 1;
        if (*timer != 0) continue;
        *(unsigned char *)(rec + 0x487d) = 1;
        Sound::PlayBank2_2D(0x1bc);
    }
}
#pragma pop


// @symbol func_ov006_020e4b78
/* Draw the drag cursor at the 0x5584/0x5588 drag point,
 * offset up and left, while the 0x55b9 flag is set. */
extern "C" void func_ov006_020e4b78(char *raw)
{
    dScMgCurling2_c *self = (dScMgCurling2_c *)raw;
    if (self->unk_55b9 == 0) return;

    int x = self->unk_5584;
    int y = self->unk_5588;
    func_ov004_020afdd0((int)data_ov006_0213c4f0, (x >> 12) - 0x20, (y >> 12) - 8, -1, 0);
}


/* Stylus drag update. Runs while the player drags a stone before the throw. With no touch in
 * the current input record it just drops the drag flags. Otherwise it moves
 * the drag point to the stylus plus the grab offset, clamps it to the rink,
 * and bails out (restoring the old point) when the stylus moved a unit or
 * less. The vertical direction-change tracker (unk_55be, unk_55a8,
 * unk_5590) plays the scrape sound on the first move and flips phases on
 * reversals; the throw angle (unk_55b2) is atan2 of the move with the
 * horizontal halved, snapped to straight up inside [0x4000,0x8000] and to
 * zero below, then averaged with the previous frame; the throw power
 * (unk_559c) follows the move length times 9 with a 0xc000 cap, rising
 * instantly and decaying by half. Finally the grab offset is recomputed
 * from the new point and a fresh read of the input index.
 *
 * Both pragmas are load-bearing. The ROM shares nothing across an extended
 * basic block that 2004/b56 would share by default: dx >> 1 is recomputed
 * after the atan2 call and the input index is re-read at the end, so
 * opt_common_subs is off, and every value the ROM does share is a named
 * local here (i4, bx/by, dy2, j-free ax/ay). opt_propagation off keeps the
 * stylus byte loads above the first store (the ROM loads both before it)
 * and puts i4 in r4. Loading by before bx, and ay before ax, colours the
 * two scratch pairs the ROM's way; the twin func_ov006_020e1854 in
 * dScMgCurling_c reads the same. */
#pragma push
#pragma opt_common_subs off
#pragma opt_propagation off
// @symbol _ZN15dScMgCurling2_c10DragUpdateEv
void dScMgCurling2_c::DragUpdate()
{
    dScMgCurling2_c *self = this;
    u8 idx;
    int dx;
    int oldx;
    int oldy;
    int diff;
    u16 oldAng;
    int mag;
    int i4;

    idx = data_020a0e40;
    i4 = idx * 4;
    if (data_020a0de8[idx * 4] != 0) {
        int dy2;
        int dy;
        u8 bx;
        u8 by;

        by = data_020a0deb[i4];
        bx = data_020a0dea[i4];
        oldx = self->unk_5584;
        oldy = self->unk_5588;
        self->unk_5584 = self->unk_5594 + (bx << 12);
        self->unk_5588 = self->unk_5598 + (by << 12);
        if (self->unk_5588 <= 0x94000) self->unk_5588 = 0x94000;
        if (self->unk_5584 <= 0x20000) self->unk_5584 = 0x20000;
        if (self->unk_5584 >= 0xe0000) self->unk_5584 = 0xe0000;
        if (self->unk_5588 >= 0xb8000) self->unk_5588 = 0xb8000;
        dx = (self->unk_5584 - oldx) >> 12;
        dy = (self->unk_5588 - oldy) >> 12;
        dy2 = dy * dy;
        if (cstd::sqrt((u64)(dx * dx + dy2)) <= 1) {
            self->unk_5584 = oldx;
            self->unk_5588 = oldy;
            return;
        }
        diff = (self->unk_5588 - self->unk_5590) >> 12;
        if (self->unk_55be == 0) {
            func_02012718(0x1d6, self->unk_5584);
            self->unk_55be = 2;
            self->unk_55a8 = (self->unk_5588 - self->unk_5590) >> 12;
            self->unk_5590 = self->unk_5588;
        } else if (self->unk_55be == 1) {
            if (self->unk_55a8 * diff > 0) {
                if (diff < 0) diff = -diff;
                if (diff >= 0xa) self->unk_55be = 0;
            } else {
                self->unk_55a8 = diff;
                self->unk_5590 = self->unk_5588;
            }
        } else {
            if (self->unk_55a8 * diff < 0) self->unk_55be = 1;
            self->unk_55a8 = (self->unk_5588 - self->unk_5590) >> 12;
            self->unk_5590 = self->unk_5588;
        }
        oldAng = self->unk_55b2;
        self->unk_55b2 = _ZN4cstd5atan2E5Fix12IiES1_(dy, dx >> 1);
        {
            u16 a = self->unk_55b2;

            if (a <= 0x8000 && a >= 0x4000) {
                self->unk_55b2 = 0x8000;
            } else if (a <= 0x4000) {
                self->unk_55b2 = 0;
            }
        }
        self->unk_55b2 = (self->unk_55b2 + oldAng) >> 1;
        mag = cstd::sqrt((u64)((dx >> 1) * (dx >> 1) + dy2)) * 9;
        mag = (mag << 12) >> 4;
        if (mag >= 0xc000) mag = 0xc000;
        if (mag > self->unk_559c) self->unk_559c = mag;
        if (self->unk_559c > mag) self->unk_559c -= (self->unk_559c - mag) >> 1;
        {
            int j = data_020a0e40;
            int ay = (self->unk_5588 >> 12) - data_020a0deb[j * 4];
            int ax = (self->unk_5584 >> 12) - data_020a0dea[j * 4];

            self->unk_5594 = ax << 12;
            self->unk_5598 = ay << 12;
        }
        return;
    }
    self->unk_55b8 = 0;
    self->unk_55b9 = 1;
}
#pragma pop


// @symbol _ZN15dScMgCurling2_c9DragBeginEv
/* Begin a drag: latch the grab offset and the first angle. Entry 0 of
 * data_ov006_02141978; DragUpdate is entry 1. */
void dScMgCurling2_c::DragBegin()
{
    char *raw = (char *)this;
    int idx;
    int flag = 0;
    int x;
    int y;

    idx = data_020a0e40;
    /* `4 & 0xFFFFFFFF` is not decoration: 0xFFFFFFFF is unsigned, so the mask
       makes this one subscript unsigned where the next is signed, and that is
       what the ROM's index arithmetic does.  Deleting it costs 20 words. */
    if (data_020a0de8[idx * (4 & 0xFFFFFFFF)] != 0)
    {
        if (data_020a0de9[idx * 4] != 0)
            flag = 1;
    }
    if (flag == 0)
        return;

    x = (unk_5584 >> 0xc) - data_020a0dea[idx * 4];
    y = (unk_5588 >> 0xc) - data_020a0deb[idx * 4];

    unk_5594 = x << 0xc;
    unk_5598 = y << 0xc;
    unk_55b8 = 1;
    unk_55b2 = 0xc000;

    if (*(u8 *)(raw + 0x55bd) == 0)
    {
        func_02012718(0x1d2, unk_5584);
        *(u8 *)(raw + 0x55bd) = 6;
    }

    *(int *)(raw + 0x55a0) = 0;
    *(int *)(raw + 0x55a4) = 0;
    *(int *)(raw + 0x558c) = unk_5584 + unk_5594;
    unk_5590 = unk_5588 + unk_5598;
    unk_55a8 = 0xff;
    unk_55be = 0;
}


// @symbol func_ov006_020e4fe8
/* Draw the counter at the bottom of the screen: a fixed
 * sprite, a language-dependent label out of data_ov006_0213c4dc, and 5 minus
 * the 0x55ba count, floored at zero. */
extern "C" void func_ov006_020e4fe8(char *raw)
{
    int left;
    if (((dScMgCurling2_c *)raw)->unk_55c3 == 0)
        return;
    left = 5 - *(unsigned char *)(raw + 0x55ba);
    if (left < 0)
        left = 0;
    func_ov004_020af948((int)&data_ov006_0213c3fc, 0xd0, 0xb4, 0);
    func_ov004_020af948(data_ov006_0213c4dc[GetGameLanguage()][1], 0xe8, 0xb4, 0);
    func_ov004_020b2444(0xf8, 0xb4, left, 0, 0, 0, 0);
}


// @symbol func_ov006_020e507c
/* Draw each live, visible stone on both screens: the body
 * sprite, picked by the stone's 0x468d flag, and a shadow eight pixels down. */
extern "C" void func_ov006_020e507c(char *p)
{
    int x, y;
    int i;

    for (i = 0; i < 0xb; i++, p += 0x30) {
        void *tex;
        if (*(unsigned char *)(p + 0x4689) == 0) continue;
        if (*(unsigned char *)(p + 0x468a) == 0) continue;
        x = *(int *)(p + 0x4660) >> 12;
        y = *(int *)(p + 0x4664) >> 12;
        if (*(unsigned char *)(p + 0x468d) != 0) tex = &data_ov006_0213c44c;
        else tex = &data_ov006_0213c3fc;
        RenderOamBothScreens(tex, x, y, -1, 1, 0);
        RenderOamBothScreens(&data_ov006_0213c454, x, y + 8, -1, 2, 0);
    }
}


// @symbol func_ov006_020e513c
/* Stone separation. The two-player rink's twin of func_ov006_020e1dc8 in
 * dScMgCurling_c: eleven stones at a 0x30 stride instead of five at 0x2c,
 * and after the bump sound the pushed pair gets a SpawnValue. Keep vx and
 * vy named and the inner scan's locals declared in the loop body; both
 * shapes are needed, as in the Curling version.
 *
 * This one stays a free function because src/func_ov006_020e5e3c.c still
 * calls it by its C name. */
extern "C" void func_ov006_020e513c(dScMgCurling2_c *self, int idx)
{
    int i;
    int j;
    int dx;
    int dy;
    int dist;
    u16 ang;
    int k;

    for (i = 0; i < 11; i++) {
        if (self->mStone[i].active == 0) continue;
        if (idx == i) continue;
        dx = (self->mStone[i].x - self->mStone[idx].x) >> 12;
        dy = (self->mStone[i].y - self->mStone[idx].y) >> 12;
        dist = cstd::sqrt((u64)(dx * dx + dy * dy));
        ang = _ZN4cstd5atan2E5Fix12IiES1_(dy, dx);
        if (dist > 0x18) continue;
        {
            int cs;
            int sn;
            int vx;
            int vy;

            k = (ang >> 4) * 2;
            cs = data_02082214[k + 1];
            vx = (int)(((long long)cs * 0x1a + 0x800) >> 12);
            self->mStone[i].x = self->mStone[idx].x + (vx << 12);
            sn = data_02082214[k];
            vy = (int)(((long long)sn * 0x1a + 0x800) >> 12);
            self->mStone[i].y = self->mStone[idx].y + (vy << 12);
            for (j = 0; j < 11; j++) {
                int dx2;
                int dy2;
                int dist2;
                u16 ang2;

                if (self->mStone[j].active == 0) continue;
                if (i == j) continue;
                dx2 = (self->mStone[j].x - self->mStone[i].x) >> 12;
                dy2 = (self->mStone[j].y - self->mStone[i].y) >> 12;
                dist2 = cstd::sqrt((u64)(dx2 * dx2 + dy2 * dy2));
                ang2 = _ZN4cstd5atan2E5Fix12IiES1_(dy2, dx2);
                if (dist2 > 0x18) continue;
                {
                    int cs2;
                    int sn2;
                    int vx2;
                    int vy2;

                    k = (ang2 >> 4) * 2;
                    cs2 = data_02082214[k + 1];
                    vx2 = (int)(((long long)cs2 * 0x1a + 0x800) >> 12);
                    self->mStone[j].x = self->mStone[i].x + (vx2 << 12);
                    sn2 = data_02082214[k];
                    vy2 = (int)(((long long)sn2 * 0x1a + 0x800) >> 12);
                    self->mStone[j].y = self->mStone[i].y + (vy2 << 12);
                    func_02012718(0xe8, self->mStone[idx].x);
                    self->SpawnValue(idx, i);
                    return;
                }
            }
            return;
        }
    }
}
