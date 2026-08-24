//cpp
// @symbol _ZN9TowerStep8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "TowerStep.h"

extern "C" {
int DecIfAbove0_Byte(u8 *p);
void _ZN8dActor_c9UpdatePosEP5dCc_c(void *self, void *clsn);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
int Vec3_Dist(void *a, void *b);
unsigned _ZN5Sound8PlayLongEjjjRK7Vector3s(unsigned a, unsigned b, unsigned c, void *pos, s16 e);
void func_020393a4(void *p, int v);
void func_02039394(void *p, int v);
void func_ov015_021123c8(void *self);
}

int TowerStep::Behavior()
{
    char *self = (char *)this;
    int kind = param1 & 0xff;

    if (kind == 1) {
        if (DecIfAbove0_Byte(&mMoveTimer) == 0) {
            s16 *p = &mPrevAngleY;
            s16 v = *p;
            u8 b = 0x87;
            v = (s16)(v + 0x8000);
            *p = v;
            mMoveTimer = b;
        }
        _ZN8dActor_c9UpdatePosEP5dCc_c(self, 0);
        UpdateModelPosAndRotY();
        if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(self, 0x150000, 0x1000) != 0)
            UpdateClsnPosAndRot();
    } else if (kind == 2) {
        if (DecIfAbove0_Byte(&mMoveTimer) == 0) {
            if (mMove != 0) {
                int flag;
                int bound;
                int y;

                _ZN8dActor_c9UpdatePosEP5dCc_c(self, 0);
                bound = mMinPosY;
                y = mPosY;
                flag = (y >= bound);
                if (flag != 0)
                    goto c2_hi;
                mPosY = (y < bound) ? bound : y;
                mVertSpeed = 0xa000;
                if (mJustSteppedOn == 0) {
                    flag = 0;
                    mMove = (u8)flag;
                }
                goto c2_after;
            c2_hi:
                bound = mMaxPosY;
                flag = (y <= bound);
                if (flag != 0)
                    goto c2_after;
                mPosY = (y > bound) ? bound : y;
                mVertSpeed = -0xa000;
                mMoveTimer = 0x5a;
            }
        }
    c2_after:
        UpdateModelPosAndRotY();
        if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(self, 0x150000, 0x1000) != 0)
            UpdateClsnPosAndRot();
        if (Vec3_Dist(&mPosX, &mPrevPosX) != 0) {
            unsigned z = 0;
            unsigned r = _ZN5Sound8PlayLongEjjjRK7Vector3s(
                (unsigned)mSoundID, 3, 0x82, &mCamSpacePosX, (s16)z);
            mSoundID = (s32)r;
        }
        mJustSteppedOn = 0;
    } else {
        _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(self, 0x150000, 0x1000);
    }

    func_020393a4(&mMeshCollider, 0x150000);
    func_02039394(&mMeshCollider, 0x1000);
    func_ov015_021123c8(self);
    return 1;
}
