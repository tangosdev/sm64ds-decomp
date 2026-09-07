//cpp
/* Slot 0. Opens with the base's own SaveSnapshot body written out inline
 * rather than called -- same idiom as every sibling. While mWasHit is set,
 * runs a one-shot transition (an effect/sound trigger, gated on mHitTimer
 * still being 0) and reloads mHitTimer from mVariant; otherwise, while mHitTimer
 * is still counting down, decrements it and, once it drops under 4, scans a
 * fixed actor list for anything within a fixed-point distance of 0x11000,
 * arming mHitTimer back to 4 on a hit. Always ends by easing the base's
 * mRadius toward 0x7000 or 0, depending on mVariant and whether mHitTimer has
 * drained. */
#include "cMgSmartball_kinoko_c.h"

typedef struct { int x, y; } V2;

extern "C" void func_ov006_02115598(void *c, int *src, int v2, int v3, int v5);
extern "C" void func_ov006_02115008(void *p);
extern "C" void func_02012718(void *a, int b);
extern "C" void func_ov006_0211470c(int *a, int *b);
extern "C" void Vec2_Sub(int *o, int *a, int *b);
extern "C" int Vec2_Len(const void *v);

#pragma opt_strength_reduction off
void cMgSmartball_kinoko_c::SaveSnapshot()
{
    V2 pair;
    V2 bufA;
    V2 bufB;
    char *obj;
    int i;

    mSnapshot0 = mCurrent0;
    mSnapshot1 = mCurrent1;

    if (mWasHit == 1) {
        if (mHitTimer == 0) {
            pair.x = mCurrent0;
            pair.y = mCurrent1;
            func_ov006_02115598((void *)mpManager, (int *)&pair, 0x12c, 0, 1);
            func_ov006_02115008((void *)mpManager);
            func_02012718((void *)0x19f, mCurrent0);
        }
        mWasHit = 0;
        if (mVariant == 1)
            mHitTimer = 0x100;
        else
            mHitTimer = 4;
    } else if (mHitTimer > 0) {
        mHitTimer -= 1;
        if (mHitTimer < 4) {
            obj = (char *)mpManager;
            i = 0;
            if (*(int *)(obj + 0x4668) > 0) {
                do {
                    void *n = (i >= 0xd) ? (void *)0 : *(void **)(obj + i * 4 + 0x4688);
                    if (*(unsigned char *)((int)n + 0x30) != 0) {
                        void *m = (i >= 0xd) ? (void *)0 : *(void **)(obj + i * 4 + 0x4688);
                        int hit;
                        func_ov006_0211470c((int *)&bufA, (int *)m);
                        Vec2_Sub((int *)&bufB, &mCurrent0, (int *)&bufA);
                        hit = Vec2_Len((void *)&bufB) < 0x11000;
                        if (hit != 0) {
                            mHitTimer = 4;
                            goto done;
                        }
                    }
                    obj = (char *)mpManager;
                    i++;
                } while (i < *(int *)(obj + 0x4668));
            }
        }
    }

done:
    if (mVariant == 1) {
        if (mHitTimer <= 0) {
            mRadius += 0x1000;
            if (mRadius > 0x7000)
                mRadius = 0x7000;
        } else {
            mRadius -= 0x1000;
            if (mRadius < 0)
                mRadius = 0;
        }
    }
}
