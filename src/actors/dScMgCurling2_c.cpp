//cpp
/* Two-player curling. mPiece falls from the top of the screen and wraps
 * once y passes 0xc8. mValue is the number SpawnValue drops between two
 * stones when they collide.
 *
 * Leftover: SpawnValue still reads the other stone as
 *   raw + 0x4000 + other * 0x30 + 0x660. mStone[other] folds that into one
 *   constant and loses the raw + 0x4000 base the ROM shares.
 * Leftover: DragUpdate and func_ov006_020e513c call
 *   _ZN4cstd5atan2E5Fix12IiES1_. cstd::atan2 takes Fix12 by value.
 * Leftover: func_ov006_020e4a84 and func_ov006_020e4b00 still walk the
 *   five records at 0x4870 by offset. This file does not name them.
 * Leftover: func_ov006_020e513c stays a free function. SpawnValue is the
 *   call it makes here, and func_ov006_020e5e3c.c calls the C spelling.
 */

/* Required: ROM order, and the brackets on func_ov006_020e4b00 and DragUpdate. */
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
/* Draw every live mValue. mode 1 and mode 2 pick the last argument. */
extern "C" void func_ov006_020e38b0(dScMgCurling2_c *self)
{
    int i;
    for (i = 0; i < 0x3c; i++) {
        unsigned char mode = self->mValue[i].mode;
        if (mode != 0) {
            int x = self->mValue[i].x >> 0xc;
            int y = self->mValue[i].y >> 0xc;
            int value = self->mValue[i].value;
            if (mode == 1) {
                func_ov004_020b1ea4(x, y, value, -1, -1, 0, 0);
            } else {
                func_ov004_020b1ea4(x, y, value, -1, -1, 0, 0x32);
            }
        }
    }
}


// @symbol func_ov006_020e3948
/* Age mValue one frame: count lifetime down, apply yInc and decay it
 * by 0x40, and clear the entry when the lifetime expires. */
extern "C" void func_ov006_020e3948(dScMgCurling2_c *self)
{
    int i;
    for (i = 0; i < 0x3c; i++)
    {
        if (self->mValue[i].live != 0)
        {
            if (self->mValue[i].lifetime != 0)
            {
                self->mValue[i].lifetime -= 1;
                self->mValue[i].y += self->mValue[i].yInc;
                self->mValue[i].yInc -= 0x40;
            }
            else
            {
                self->mValue[i].live = 0;
                self->mValue[i].mode = 0;
            }
        }
    }
}


// @symbol _ZN15dScMgCurling2_c10SpawnValueEii
/* Take the first free mValue and place it midway between the two
 * stones. The number is (n+1)*10 when either stone carries the 0x468d
 * flag and (n+1)*100 when neither does, where n is the 0x55bf counter,
 * saturating at 0x17.
 *
 * `raw + 0x4000 + other * 0x30 + 0x660` is the same kind of record as
 * `raw + stone * 0x30 + 0x4660` on the other operand. Folding the split
 * base into one constant changes the code: the ROM shares raw + 0x4000
 * across the `other` reads. Do not tidy. */
void dScMgCurling2_c::SpawnValue(int stone, int other)
{
    char *raw = (char *)this;
    int i;
    int sx;
    int sy;
    unsigned char *count;

    for (i = 0; i < 0x3c; i++) {
        if (mValue[i].live != 0) continue;

        sx = *(int *)(raw + stone * 0x30 + 0x4660) + *(int *)(raw + 0x4000 + other * 0x30 + 0x660);
        sy = *(int *)(raw + stone * 0x30 + 0x4664) + *(int *)(raw + 0x4000 + other * 0x30 + 0x664);

        mValue[i].live = 1;
        mValue[i].mode = 1;
        mValue[i].x = sx >> 1;
        mValue[i].y = sy >> 1;
        mValue[i].lifetime = 0x40;
        mValue[i].xInc = 0;
        mValue[i].yInc = 0;

        /* The && and || arms really do compute the same value; collapsing them
           into one `||` changes the code, so the ROM branched twice too. */
        if (*(unsigned char *)(raw + stone * 0x30 + 0x468d) != 0 && *(unsigned char *)(raw + 0x4000 + other * 0x30 + 0x68d) != 0) {
            mValue[i].value = (*(unsigned char *)(raw + 0x55bf) + 1) * 10;
        } else if (*(unsigned char *)(raw + stone * 0x30 + 0x468d) != 0 || *(unsigned char *)(raw + 0x4000 + other * 0x30 + 0x68d) != 0) {
            mValue[i].value = (*(unsigned char *)(raw + 0x55bf) + 1) * 10;
        } else {
            mValue[i].value = (*(unsigned char *)(raw + 0x55bf) + 1) * 100;
            mValue[i].mode = 2;
        }

        count = (unsigned char *)(raw + 0x55bf);
        *count = *count + 1;
        if (*(unsigned char *)(raw + 0x55bf) >= 0x17) *(unsigned char *)(raw + 0x55bf) = 0x17;
        func_ov004_020adb1c(mValue[i].value + func_ov004_020adbc0());
        return;
    }
}


// @symbol func_ov006_020e3b9c
/* Clear every collision value. */
extern "C" void func_ov006_020e3b9c(dScMgCurling2_c *self)
{
    int i;
    for (i = 0; i < 0x3c; i++) {
        self->mValue[i].live = 0;
        self->mValue[i].mode = 0;
    }
}


// @symbol func_ov006_020e3bc4
/* Draw every piece whose drawEnable is set, as two stacked sprites
 * from data_ov006_0213a5e0. */
extern "C" void func_ov006_020e3bc4(dScMgCurling2_c *self)
{
    int i;
    for (i = 0; i < 0x32; i++) {
        if (self->mPiece[i].drawEnable) {
            int x = self->mPiece[i].x >> 0xc;
            int y = self->mPiece[i].y >> 0xc;
            func_ov004_020af948(data_ov006_0213a5e0[self->mPiece[i].sprite0], x, y, 0);
            DrawOamSprite(data_ov006_0213a5e0[self->mPiece[i].sprite1], x, y, 0);
        }
    }
}


// @symbol _ZN15dScMgCurling2_c13StepYRampDownEi
/* countdown3, else ease yInc back toward 0x100, else clear yIndex. */
void dScMgCurling2_c::StepYRampDown(int entry)
{
    if (mPiece[entry].countdown3 != 0) {
        short *p = (short *)&mPiece[entry].countdown3;
        *p = (short)(*(unsigned short *)p - 1);
        if (*p < 0)
            *p = 0;
    } else if (mPiece[entry].yInc > 0x100) {
        int *q = &mPiece[entry].yInc;
        *q = *q - 0x10;
        if ((short)*q < 0x100)
            *q = 0x100;
    } else {
        mPiece[entry].yIndex = 0;
    }
}


// @symbol _ZN15dScMgCurling2_c11StepYRampUpEi
/* Ramp yInc up toward yTarget, then run countdown3 down. When it
 * expires, set yIndex to 2 and reseed the countdown. */
void dScMgCurling2_c::StepYRampUp(int entry)
{
    if (mPiece[entry].yTarget > mPiece[entry].yInc) {
        mPiece[entry].yInc += 0x10;
        if (mPiece[entry].yTarget > mPiece[entry].yInc)
            mPiece[entry].yInc = mPiece[entry].yTarget;
    }
    if (mPiece[entry].countdown3 != 0) {
        mPiece[entry].countdown3 = mPiece[entry].countdown3 - 1;
        if (*(short *)&mPiece[entry].countdown3 < 0) *(short *)&mPiece[entry].countdown3 = 0;
    } else {
        mPiece[entry].yIndex = 2;
        *(short *)&mPiece[entry].countdown3 = (short)(unsigned char)((((0x20 * (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff)) >> 0xf)) + 0x20);
    }
}


// @symbol _ZN15dScMgCurling2_c12StepYRestartEi
/* Zero yInc, pick a fresh yTarget and countdown3, set yIndex to 1. */
void dScMgCurling2_c::StepYRestart(int entry)
{
    unsigned int rnd;

    mPiece[entry].yInc = 0;
    rnd = ((unsigned)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    mPiece[entry].yTarget = (((rnd << 4) >> 15) << 4) + 0x300;
    mPiece[entry].yIndex = 1;
    rnd = ((unsigned)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    rnd = ((rnd << 5) >> 15) + 0x20;
    *(short *)&mPiece[entry].countdown3 = (unsigned char)rnd;
}


// @symbol _ZN15dScMgCurling2_c11StepXSettleEi
/* Step the piece, hold while countdown is set (the hold decrements
 * countdown2, not countdown), then bleed xInc toward zero 8 a frame. */
void dScMgCurling2_c::StepXSettle(int entry)
{
    /* xInc, x, y: the ROM loads those bases in that order. */
    s32 *xInc = &mPiece[entry].xInc;
    s32 *x = &mPiece[entry].x;
    s32 *y = &mPiece[entry].y;
    *x = *x + *xInc;
    *y = *y + mPiece[entry].yInc;
    if (mPiece[entry].countdown != 0) {
        u16 *timer = &mPiece[entry].countdown2;
        *timer = *timer - 1;
        if (*(s16 *)timer < 0) *(s16 *)timer = 0;
        return;
    }
    if (*xInc > 0) {
        *xInc = *xInc - 8;
        if ((s16)*xInc < 0) *xInc = 0;
        return;
    }
    if (*xInc < 0) {
        *xInc = *xInc + 8;
        if (*xInc > 0) *xInc = 0;
        return;
    }
    mPiece[entry].xIndex = 0;
}


// @symbol _ZN15dScMgCurling2_c12StepXPushNegEi
/* Step the piece, hold for countdown, push xInc down to a -0x300
 * floor (the store at the floor is +0x300), hold for countdown2, then
 * hand xIndex to 3. */
void dScMgCurling2_c::StepXPushNeg(int entry)
{
    unsigned short count;

    mPiece[entry].x = mPiece[entry].x + mPiece[entry].xInc;
    mPiece[entry].y = mPiece[entry].y + mPiece[entry].yInc;

    if (mPiece[entry].countdown != 0) {
        mPiece[entry].countdown = mPiece[entry].countdown - 1;
        if (*(short *)&mPiece[entry].countdown < 0)
            mPiece[entry].countdown = 0;
        return;
    }

    if (mPiece[entry].xInc > -0x300) {
        mPiece[entry].xInc -= 8;
        if (mPiece[entry].xInc <= -0x300)
            mPiece[entry].xInc = 0x300;
    }

    count = mPiece[entry].countdown2;
    if (count != 0) {
        mPiece[entry].countdown2 = count - 1;
        if (*(short *)&mPiece[entry].countdown2 < 0)
            mPiece[entry].countdown2 = 0;
        return;
    }

    mPiece[entry].xIndex = 3;
    mPiece[entry].countdown2 = (unsigned char)(((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 5 >> 0xf) + 0x20);
}


// @symbol _ZN15dScMgCurling2_c12StepXPushPosEi
/* The mirror of StepXPushNeg: same shape, x increment pushed the
 * other way to a +0x300 ceiling. */
void dScMgCurling2_c::StepXPushPos(int entry)
{
    unsigned short count;

    mPiece[entry].x += mPiece[entry].xInc;
    mPiece[entry].y += mPiece[entry].yInc;

    count = mPiece[entry].countdown;
    if (count != 0) {
        *(short *)&mPiece[entry].countdown = count - 1;
        if (*(short *)&mPiece[entry].countdown < 0)
            *(short *)&mPiece[entry].countdown = 0;
        return;
    }

    if (mPiece[entry].xInc < 0x300) {
        mPiece[entry].xInc += 8;
        if (mPiece[entry].xInc >= 0x300)
            mPiece[entry].xInc = 0x300;
    }

    count = mPiece[entry].countdown2;
    if (count != 0) {
        *(short *)&mPiece[entry].countdown2 = count - 1;
        if (*(short *)&mPiece[entry].countdown2 < 0)
            *(short *)&mPiece[entry].countdown2 = 0;
        return;
    }

    *(char *)&mPiece[entry].xIndex = 3;
    *(short *)&mPiece[entry].countdown2 = (((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 5) >> 0xf) + 0x20 & 0xff;
}


// @symbol _ZN15dScMgCurling2_c9StepXPickEi
/* Hold for countdown, then zero xInc, pick xIndex from
 * data_ov006_0212e4f4 and reseed both countdowns. */
void dScMgCurling2_c::StepXPick(int entry)
{
    if (mPiece[entry].countdown != 0) {
        mPiece[entry].countdown = mPiece[entry].countdown - 1;
        if (*(short *)&mPiece[entry].countdown < 0) *(short *)&mPiece[entry].countdown = 0;
        return;
    }
    mPiece[entry].xInc = 0;
    mPiece[entry].xIndex = data_ov006_0212e4f4[(((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 1 >> 15];
    mPiece[entry].countdown = (short)(unsigned char)((0x10 * (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff)) >> 0xf);
    mPiece[entry].countdown2 = (short)(unsigned char)(((0x40 * (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff)) >> 0xf) + 0x60);
}


// @symbol _ZN15dScMgCurling2_c9StepXAndYEi
/* Run one piece's xIndex callback, then its yIndex callback.
 * yIndex is re-read through the pointer-to-member receiver. */
void dScMgCurling2_c::StepXAndY(int entry)
{
    C *self = (C *)this;
    unsigned char k0 = mPiece[entry].xIndex;
    (self->*data_ov006_021419f8[k0])(entry);
    unsigned char k1 = ((dScMgCurling2_c *)self)->mPiece[entry].yIndex;
    (self->*data_ov006_021419a0[k1])(entry);
}


// @symbol _ZN15dScMgCurling2_c15StepXSettleFastEi
/* Step the piece, then bleed xInc toward zero 0x20 a frame. */
void dScMgCurling2_c::StepXSettleFast(int entry)
{
    /* x, xInc, y -- the ROM's load order, not a tidy one. */
    int *x = &mPiece[entry].x;
    int *xInc = &mPiece[entry].xInc;
    int *y = &mPiece[entry].y;
    *x += *xInc;
    *y += mPiece[entry].yInc;
    if (*xInc > 0) {
        *xInc -= 0x20;
        if ((int)(short)*xInc < 0) *xInc = 0;
    } else if (*xInc < 0) {
        *xInc += 0x20;
        if (*xInc > 0) *xInc = 0;
    } else {
        mPiece[entry].xIndex = 0;
    }
}


// @symbol _ZN15dScMgCurling2_c16StepXPushNegFastEi
/* As StepXPushNeg, 0x20 a frame and a -0x400 floor. The store at the
 * floor is +0x400, and xIndex goes to 3 with no new countdown. */
void dScMgCurling2_c::StepXPushNegFast(int entry)
{
    mPiece[entry].x = mPiece[entry].x + mPiece[entry].xInc;
    mPiece[entry].y = mPiece[entry].y + mPiece[entry].yInc;

    if (mPiece[entry].countdown != 0) {
        *(s16 *)&mPiece[entry].countdown =
            (s16)(mPiece[entry].countdown - 1);
        if (*(s16 *)&mPiece[entry].countdown < 0)
            *(s16 *)&mPiece[entry].countdown = 0;
        return;
    }

    if (mPiece[entry].xInc > -0x400) {
        mPiece[entry].xInc -= 0x20;
        if (mPiece[entry].xInc <= -0x400)
            mPiece[entry].xInc = 0x400;
    }

    if (mPiece[entry].countdown2 != 0) {
        *(s16 *)&mPiece[entry].countdown2 =
            (s16)(mPiece[entry].countdown2 - 1);
        if (*(s16 *)&mPiece[entry].countdown2 < 0)
            *(s16 *)&mPiece[entry].countdown2 = 0;
        return;
    }

    mPiece[entry].xIndex = 3;
}


// @symbol _ZN15dScMgCurling2_c16StepXPushPosFastEi
/* The mirror of StepXPushNegFast, x increment pushed to a +0x400
 * ceiling. */
void dScMgCurling2_c::StepXPushPosFast(int entry)
{
    mPiece[entry].x += mPiece[entry].xInc;
    mPiece[entry].y += mPiece[entry].yInc;

    {
        unsigned short count = mPiece[entry].countdown;
        if (count != 0) {
            *(short *)&mPiece[entry].countdown = (short)(count - 1);
            if (*(short *)&mPiece[entry].countdown < 0) {
                *(short *)&mPiece[entry].countdown = 0;
            }
            return;
        }
    }

    if (mPiece[entry].xInc < 0x400) {
        mPiece[entry].xInc += 0x20;
        if (mPiece[entry].xInc >= 0x400) {
            mPiece[entry].xInc = 0x400;
        }
    }

    {
        unsigned short count2 = mPiece[entry].countdown2;
        if (count2 != 0) {
            *(short *)&mPiece[entry].countdown2 = (short)(count2 - 1);
            if (*(short *)&mPiece[entry].countdown2 < 0) {
                *(short *)&mPiece[entry].countdown2 = 0;
            }
            return;
        }
    }

    mPiece[entry].xIndex = 3;
}


// @symbol _ZN15dScMgCurling2_c13StepXPickFastEi
/* Hold for countdown, then zero xInc, pick a fresh yInc and an xIndex
 * from data_ov006_0212e4f8, and reseed both countdowns. */
void dScMgCurling2_c::StepXPickFast(int entry)
{
    if (mPiece[entry].countdown != 0) {
        mPiece[entry].countdown = mPiece[entry].countdown - 1;
        if (*(short *)&mPiece[entry].countdown < 0) *(short *)&mPiece[entry].countdown = 0;
        return;
    }
    mPiece[entry].xInc = 0;
    mPiece[entry].yInc = ((((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 5) >> 0xf) << 4) + 0x600;
    mPiece[entry].xIndex = data_ov006_0212e4f8[(((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 1 >> 15];
    mPiece[entry].countdown = (unsigned short)((((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 4) >> 0xf) & 0xff);
    mPiece[entry].countdown2 = (unsigned short)(((((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 0x30) >> 0xf) + 0x30) & 0xff);
}


// @symbol _ZN15dScMgCurling2_c9StepXOnlyEi
/* Run one piece's data_ov006_021419b8 callback, indexed by xIndex. */
void dScMgCurling2_c::StepXOnly(int entry)
{
    unsigned char idx = mPiece[entry].xIndex;
    (((C *)this)->*data_ov006_021419b8[idx])(entry);
}


// @symbol _ZN15dScMgCurling2_c12PickStepModeEi
/* Pick modeIndex from data_ov006_0212e4fc (1 on one roll in eight)
 * and clear xIndex. */
void dScMgCurling2_c::PickStepMode(int entry)
{
    unsigned rnd = (unsigned)RandomIntInternal(&data_0209d4b8);
    int pick = 0;
    unsigned roll = ((rnd >> 16) & 0x7fff) << 3 >> 0xf;
    if (roll == 5) pick = 1;
    mPiece[entry].modeIndex = data_ov006_0212e4fc[pick];
    mPiece[entry].xIndex = 0;
}


// @symbol func_ov006_020e4800
/* Step every piece whose updateEnable is set, and wrap the ones whose
 * y has passed 0xc8 back to a random x at y = -0x8000. */
extern "C" void func_ov006_020e4800(dScMgCurling2_c *self)
{
    int i;
    for (i = 0; i < 0x32; i++) {
        if (self->mPiece[i].updateEnable != 0) {
            unsigned char idx = self->mPiece[i].modeIndex;
            (((C *)self)->*data_ov006_02141988[idx])(i);
            if ((self->mPiece[i].y >> 0xc) >= 0xc8) {
                self->mPiece[i].x = (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 5 >> 0xf << 0xf;
                self->mPiece[i].y = -0x8000;
                self->mPiece[i].xIndex = 0;
                self->mPiece[i].modeIndex = 0;
                self->mPiece[i].yIndex = 0;
            }
        }
    }
}


// @symbol func_ov006_020e48d4
/* Clear every piece, then seed it: enabled, a random x, two sprite
 * indices (the second is the first plus one to four, modulo five) and a
 * y spread over the screen. The first x, the y at -0x8000 and the first
 * countdown store are overwritten a few lines later; those dead stores
 * are the ROM's. */
extern "C" void func_ov006_020e48d4(dScMgCurling2_c *self)
{
    int i;
    unsigned int rnd;
    unsigned int v;
    int q;
    unsigned int m;

    i = 0;
    for (; i < 0x32; i++)
    {
        self->mPiece[i].x = 0;
        self->mPiece[i].y = 0;
        self->mPiece[i].xInc = 0;
        self->mPiece[i].yInc = 0;
        self->mPiece[i].countdown = 0;
        self->mPiece[i].countdown2 = 0;
        self->mPiece[i].countdown3 = 0;
        self->mPiece[i].updateEnable = 0;
        self->mPiece[i].modeIndex = 0;
        self->mPiece[i].xIndex = 0;
        self->mPiece[i].yIndex = 0;
        self->mPiece[i].drawEnable = 0;
        self->mPiece[i].sprite0 = 0;
        self->mPiece[i].sprite1 = 1;
    }

    i = 0;
    for (; i < 0x32; i++)
    {
        rnd = (unsigned int)RandomIntInternal(&data_0209d4b8);
        m = ((rnd >> 16) & 0x7fff) << 5;
        self->mPiece[i].x = (int)((m >> 0xf)) << 0xf;
        self->mPiece[i].y = -0x8000;
        self->mPiece[i].updateEnable = 1;
        self->mPiece[i].drawEnable = 1;
        self->mPiece[i].modeIndex = 0;
        self->mPiece[i].xIndex = 0;

        rnd = (unsigned int)RandomIntInternal(&data_0209d4b8);
        self->mPiece[i].sprite0 = (char)(((rnd >> 16) & 0x7fff) * 5 >> 0xf);

        rnd = (unsigned int)RandomIntInternal(&data_0209d4b8);
        /* Own local: `mPiece[i].sprite0 + shift` puts the byte in r0. */
        unsigned char spr = self->mPiece[i].sprite0;
        v = spr + ((((rnd >> 16) & 0x7fff) << 2) >> 0xf) + 1;
        v = v & 0xff;
        if (v >= 5)
            v = (v - 5) & 0xff;
        self->mPiece[i].sprite1 = (char)v;

        rnd = (unsigned int)RandomIntInternal(&data_0209d4b8);
        self->mPiece[i].countdown = (short)(((i & 7) << 6) + (((rnd >> 16) & 0x7fff) * 0x30 >> 0xf));

        rnd = (unsigned int)RandomIntInternal(&data_0209d4b8);
        m = ((rnd >> 16) & 0x7fff) << 5;
        self->mPiece[i].x = (int)((m >> 0xf)) << 0xf;

        rnd = (unsigned int)RandomIntInternal(&data_0209d4b8);
        q = (((rnd >> 16) & 0x7fff) * 0x1a) >> 0xf;
        self->mPiece[i].y = (((q << 3) - 8)) << 0xc;
        self->mPiece[i].countdown = 0;
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
