//cpp
// @symbol _ZN9TowerStep8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "TowerStep.h"

extern "C" {
int DecIfAbove0_Byte(u8 *p);
void _ZN5Actor9UpdatePosEP12CylinderClsn(void *self, void *clsn);
int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
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
        if (DecIfAbove0_Byte(&unk_390) == 0) {
            s16 *p = &mPrevAngleY;
            s16 v = *p;
            u8 b = 0x87;
            v = (s16)(v + 0x8000);
            *p = v;
            unk_390 = b;
        }
        _ZN5Actor9UpdatePosEP12CylinderClsn(self, 0);
        UpdateModelPosAndRotY();
        if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(self, 0x150000, 0x1000) != 0)
            UpdateClsnPosAndRot();
    } else if (kind == 2) {
        if (DecIfAbove0_Byte(&unk_390) == 0) {
            if (unk_392 != 0) {
                int flag;
                int bound;
                int y;

                _ZN5Actor9UpdatePosEP12CylinderClsn(self, 0);
                bound = unk_37c;
                y = mPosY;
                flag = (y >= bound);
                if (flag != 0)
                    goto c2_hi;
                mPosY = (y < bound) ? bound : y;
                mVertSpeed = 0xa000;
                if (unk_391 == 0) {
                    flag = 0;
                    unk_392 = (u8)flag;
                }
                goto c2_after;
            c2_hi:
                bound = unk_380;
                flag = (y <= bound);
                if (flag != 0)
                    goto c2_after;
                mPosY = (y > bound) ? bound : y;
                mVertSpeed = -0xa000;
                unk_390 = 0x5a;
            }
        }
    c2_after:
        UpdateModelPosAndRotY();
        if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(self, 0x150000, 0x1000) != 0)
            UpdateClsnPosAndRot();
        if (Vec3_Dist(&mPosX, &unk_068) != 0) {
            unsigned z = 0;
            unsigned r = _ZN5Sound8PlayLongEjjjRK7Vector3s(
                (unsigned)unk_38c, 3, 0x82, &mCamSpacePosX, (s16)z);
            unk_38c = (s32)r;
        }
        unk_391 = 0;
    } else {
        _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(self, 0x150000, 0x1000);
    }

    func_020393a4(&mMeshCollider, 0x150000);
    func_02039394(&mMeshCollider, 0x1000);
    func_ov015_021123c8(self);
    return 1;
}
