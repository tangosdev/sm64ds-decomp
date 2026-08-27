//cpp
// @symbol _ZN15dScMgSnowball_c6RenderEv
#include "types.h"
#include "decl_common.h"
#include "dScMgSnowball_c.h"
/* dScMgSnowball_c::Render -- vtable slot 9, ov006 0x02127d10.
 *
 * Named from the table: 0x02127d10 is the word slot 9 of
 * _ZTV15dScMgSnowball_c holds where its base's table holds something else, so
 * it is this class's own override of the virtual fBase_c declares. The
 * pre-migration file had already retyped the receiver as dScMgSnowball_c and
 * named its fields; only the symbol was still a func_ov006_ one.
 *
 * `c` is kept alongside `self` for the same reason it was there before: the
 * two 0x80-entry parallel arrays at 0xacd8 and 0xb5d8 are indexed with a
 * per-element stride the header holds as raw bytes (mArray1/mArray2, element
 * type unevidenced -- their ROM element destructor is NullDestructor_0203d47c),
 * so those reads stay byte offsets rather than becoming a layout claim.
 *
 * BOTH #pragmas ARE LOAD-BEARING and are inherited verbatim from the
 * pre-migration file; they are the reason this body reproduces at all. */

#pragma opt_strength_reduction off
#pragma opt_common_subs off

extern "C" {
void func_ov004_020afdd0(void* a0, int a1, int a2, int a3, int a4);
void func_0203cd80(int *m, short angle);
void func_ov006_02126a98(char *c);
void func_ov004_020b2220(int a1, int a2, int a3, int a4, int a5, int a6, int a7);
s32  GetGameLanguage(void);
}

s32 dScMgSnowball_c::Render()
{
    char *c = (char *)this;
    struct dScMgSnowball_c *self = (struct dScMgSnowball_c *)(void *)c;
    int m[3];
    int vecArr[3];

    *(int*)((long long)(int)(c + 0xb9d8)) = *(int*)((long long)(int)(c + 0xb9d8)) + 1;
    if (self->mAnimCounter >= 0x20) {
        self->mAnimCounter = 0;
    }

    {
        int base = self->mGoalY;
        int t = (((self->mPosY >> 0xc) - base) * 0x7c) / (self->mStartY - base);
        func_ov004_020afdd0(data_ov006_02139d24[0], 0xf0, t + 0x22, -1, 0);
    }
    func_ov004_020afdd0(data_ov006_02139d24[1], 0xf0, 0x60, -1, 0);

    m[2] = 0xfffff008;
    m[1] = 0;
    m[0] = 0;
    func_0203cd80(m, -0x2000);
    *(volatile int*)0x040004c8 =
        (((short)m[0] >> 3) & 0x3ff) |
        ((((short)m[1] >> 3) & 0x3ff) << 10) |
        ((((short)m[2] >> 3) & 0x3ff) << 20);
    *(int*)0x040004cc = 0x7fff;

    {
        int v = self->mBallSize;
        int t = v / 2 + v * 4;
        vecArr[0] = t;
        vecArr[1] = t;
        vecArr[2] = t;
        if (self->mBallSize > 0) {
            void *self = (void*)(c + 0xaba4);
            void (*fn)(void*, int*) = *(void(**)(void*, int*))((char*)(*(void**)self) + 0x14);
            fn(self, vecArr);
        }
    }

    func_ov006_02126a98(c);

    for (int i1 = 0; i1 < 0x80; i1++) {
        char *p = c + i1;
        if (*(u8*)(p + 0xac58) == 1) {
            int t6 = self->mScrollY;
            int t5 = *(int*)((long long)(int)(c + i1 * 8 + 0xacdc));
            if (t5 >= t6 - 0x20000 && t5 < t6 + 0x1a0000 + (func_ov004_020b04c0() << 0xc)) {
                if (*(int*)(c + i1 * 4 + 0xb0d8) == 1) {
                    int cnt = self->mAnimCounter;
                    int idx = (cnt / 4) & 7;
                    if (*(u8*)(p + 0xb2d8) == 1) idx += 8;
                    int a1 = (*(int*)(c + i1 * 8 + 0xacd8) - self->mScrollX) >> 0xc;
                    int a2 = ((t5 - t6) >> 0xc) - 0x110;
                    func_ov004_020afdd0(data_ov006_02138d64[idx], a1, a2, -1, -1);
                } else {
                    int a1 = (*(int*)(c + i1 * 8 + 0xacd8) - self->mScrollX) >> 0xc;
                    int a2 = ((t5 - t6) >> 0xc) - 0x110;
                    func_ov004_020afdd0(data_ov006_02139c24, a1, a2, -1, 2);
                }
            }
        }
    }

    for (int i2 = 0; i2 < 0x80; i2++) {
        if (*(u8*)(c + i2 + 0xb358) == 1) {
            int t7 = self->mScrollY;
            int t6 = *(int*)((long long)(int)(c + i2 * 8 + 0xb5dc));
            if (t6 >= t7 - 0x40000 && t6 < t7 + 0x1c0000 + (func_ov004_020b04c0() << 0xc)) {
                switch (*(int*)(c + i2 * 4 + 0xb3d8)) {
                case 0:
                case 1:
                case 2: {
                    int a1 = (*(int*)(c + i2 * 8 + 0xb5d8) - self->mScrollX) >> 0xc;
                    int a2 = ((t6 - t7) >> 0xc) - 0x110;
                    func_ov004_020afdd0(data_ov006_02139d18, a1, a2, -1, 1);
                    break;
                }
                case 3: {
                    int v = *(int*)(c + i2 * 8 + 0xb5d8);
                    int sel = (v < 0x80000) ? 1 : 0;
                    int a1 = (v - self->mScrollX) >> 0xc;
                    int a2 = ((t6 - t7) >> 0xc) - 0x110;
                    func_ov004_020afdd0(data_ov006_02139c80[sel], a1, a2, -1, 1);
                    break;
                }
                }
            }
        }
    }

    {
        int a1 = (self->mDrawPosX - self->mScrollX) >> 0xc;
        int a2 = ((self->mDrawPosY - self->mScrollY) >> 0xc) - 0x110;
        func_ov004_020afdd0(data_ov006_02139c38, a1, a2, -1, 2);
    }

    {
        int t4 = self->mCountdownTimer;
        if (t4 <= 0xf0 && t4 > 0x3c) {
            int q = t4 / 60;
            if (q >= 4) q = 3;
            func_ov004_020b2220(0x80, 0x60, q, -1, -1, 0x800, 0);
        } else {
            int mode = self->mState;
            if (mode == 1 || mode == 2
                || (mode == 3 && self->mEndDelayTimer > 0)
                || (mode == 4 && self->mCrashTimer <= 0x3c)) {
                int v = self->mTimeLeft;
                int q60 = v / 60;
                int rem60 = v % 60;
                int r7 = rem60 * 100;
                int centis = r7 / 60;
                int r4;
                int tens;
                int ones;
                if (v < 0x258) {
                    r4 = 0xc;
                } else if (v >= 0x276) {
                    r4 = -0x104;
                } else {
                    r4 = ((0x258 - v) * 0x110) / 30 + 0xc;
                }
                func_ov004_020b2444(0x84, r4, q60, -1, -1, 1, 0);
                tens = centis / 10;
                func_ov004_020b2444(0x9c, r4, tens, -1, -1, 1, 0);
                ones = centis % 10;
                if (ones == 5 || ones == 0) {
                    func_ov004_020b2444(0xac, r4, ones, -1, -1, 1, 0);
                } else {
                    func_ov004_020b2444(0xac, r4, ones + 1, -1, -1, 1, 0);
                }
                {
                    int idx = GetGameLanguage();
                    func_ov004_020b0380(*(void**)((char*)data_ov006_0213fff0[idx] + 0xc), 0x5c, r4, 0);
                }
                {
                    int idx = GetGameLanguage();
                    func_ov004_020b0380(*(void**)((char*)data_ov006_0213fff0[idx] + 0x1c), 0x90, r4, 0);
                }
            }
        }
    }

    func_ov006_02129690(c);
    func_ov006_02125804(c);
    return 1;
}
