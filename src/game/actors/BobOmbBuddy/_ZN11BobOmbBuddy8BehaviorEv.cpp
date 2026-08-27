//cpp
// @symbol _ZN11BobOmbBuddy8BehaviorEv
#include "BobOmbBuddy.h"

extern "C" void *_ZN8dActor_c13ClosestPlayerEv(void *thiz);
extern "C" int Vec3_HorzDist(const Vector3* a, const Vector3* b);
extern "C" short Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
extern "C" int _Z14ApproachLinearRsss(short *r, short b, short c);
extern "C" void func_02012694(int a0, void *a1);
extern "C" void func_ov084_0212c9a8(void *c);
extern "C" void func_ov084_0212ce50(void *c);

int BobOmbBuddy::Behavior()
{
    Vector3 v;
    char *p;
    func_ov084_0212c9a8(this);
    p = (char*)_ZN8dActor_c13ClosestPlayerEv(this);
    if (p != 0) {
        Vector3 *ps = (Vector3 *)(p + 0x5c);
        v = *ps;
        if (Vec3_HorzDist((Vector3 *)&mPosX, &v) < 0x12c000) {
            short ang = Vec3_HorzAngle((Vector3 *)&mPosX, &v);
            _Z14ApproachLinearRsss(&mAngleY, ang, 0x100);
        }
    }
    mModelAnim.Advance();
    if ((unsigned short)(mModelAnim.currFrame >> 12) == 0)
        func_02012694(0xd7, &mCamSpacePosX);
    func_ov084_0212ce50(this);
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}
