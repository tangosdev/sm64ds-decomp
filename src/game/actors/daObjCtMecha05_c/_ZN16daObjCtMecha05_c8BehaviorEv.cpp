//cpp
// @symbol _ZN16daObjCtMecha05_c8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "daObjCtMecha05_c.h"

/* daObjCtMecha05_c::Behavior - recovered from vtable slot identity.
 *
 * Reads/writes mPosX/mPosZ (0x05c/0x064), mAngleY (0x08e) and mPrevAngleY
 * (0x094) through their inherited dActor_c names -- see the header comment
 * in daObjCtMecha05_c.h for why the flat placeholder's unk_05c/08e/094/098
 * were never this class's own storage. mHorzSpeed (0x098) is genuinely
 * reused here as a plain fix12 accumulator, not a physical speed. */
extern "C" {
extern int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
extern u16 DecIfAbove0_Short(u16 *p);
extern int RandomIntInternal(int *seed);

extern s16 data_02082214[];
extern u8 data_0209f2c0;
extern int data_0209e650;
}

#define I32(off) (*(int *)(((int)c + (off))))
#define I8(off)  (*(u8 *)(((int)c + (off))))

int daObjCtMecha05_c::Behavior()
{
    char *c = (char *)this;

    if (data_0209f2c0 == 3) {
        int ang;
        /* Read UNSIGNED here -- ldrh, not the ldrsh a plain `s16 mPrevAngleY`
           read would emit -- matching the ROM exactly even though mPrevAngleY
           itself is signed (dActor_c.h, evidenced elsewhere). */
        ang = *(u16 *)(c + 0x94);
        mPosX = mHomePosX +
            (int)(((long long)data_02082214[(ang >> 4) << 1] * 0xfa000 + 0x800) >> 12);
        ang = *(u16 *)(c + 0x94);
        mPosZ = mHomePosZ +
            (int)(((long long)data_02082214[((ang >> 4) << 1) + 1] * 0xfa000 + 0x800) >> 12);
        UpdateModelPosAndRotY();
        func_ov065_0211ac0c(c);
        if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(c, 0, 0) != 0)
            UpdateClsnPosAndRot();
        return 1;
    }

    mPrevTravel = mTravel;
    I32(0x32c) += mHorzSpeed;

    switch (mState) {
    case 0:
        if (DecIfAbove0_Short((u16 *)(c + 0x334)) != 0) goto Lend;
        if (data_0209f2c0 == 2) {
            int v = (u16)((unsigned)RandomIntInternal(&data_0209e650) >> 16);
            if (v % 2 == 0) {
                mStateTimer = v % 100 + 20;
            }
        }
        I8(0x336)++;
        mHorzSpeed = -0x8000;
        goto Lend;

    case 1:
        I32(0x98) += 0xbae;
        if (mHorzSpeed > 0) {
            if (DecIfAbove0_Short((u16 *)(c + 0x334)) != 0) {
                mHorzSpeed = 0;
                goto Lend;
            }
            I8(0x336)++;
            mHorzSpeed = 0x1d000;
        }
        goto Lend;

    case 2: {
        int d = mTravel;
        if (d != 0xfa000) {
            int val = (int)(((long long)(0xfa000 - d) * (0xfa000 - mPrevTravel) + 0x800) >> 12);
            if (val >= 0) goto L280;
        }
        {
            int sp = mHorzSpeed;
            if (sp <= -0x8000) goto L280;
            if (sp >= 0x8000) goto L280;
            I8(0x336)++;
            mHorzSpeed = 0;
            mStateTimer = 0x1e;
            goto Lend;
        }
      L280:
        {
            int m = (mTravel < 0xfa000) ? 0x6666 : -0x6666;
            int sp = mHorzSpeed;
            int delta;
            if ((int)(((long long)sp * m + 0x800) >> 12) >= 0)
                delta = m;
            else
                delta = (int)(((long long)m * 0x2599 + 0x800) >> 12);
            I32(0x98) += delta;
            if (data_0209f2c0 != 2) goto Lend;
        }
        if ((int)(((long long)mTravel * mPrevTravel + 0x800) >> 12) >= 0) goto Lend;
        if ((RandomIntInternal(&data_0209e650) & 3) != 0) goto Lend;
        func_ov065_0211ad70(c);
        goto Lend;
    }

    case 3:
        if (DecIfAbove0_Short((u16 *)(c + 0x334)) != 0) goto Lend;
        mHorzSpeed = -0x5000;
        if (mTravel < 0)
            func_ov065_0211ad70(c);
        goto Lend;
    }

Lend:
    UpdatePos(0);
    UpdateModelPosAndRotY();
    func_ov065_0211ac0c(c);
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(c, 0x1c0000, 0x1000) != 0)
        UpdateClsnPosAndRot();
    return 1;
}
