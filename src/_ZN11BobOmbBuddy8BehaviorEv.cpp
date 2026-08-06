//cpp
// @symbol _ZN11BobOmbBuddy8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "BobOmbBuddy.h"
extern "C" void *_ZN5Actor13ClosestPlayerEv(void *thiz);
extern "C" int Vec3_HorzDist(const Vector3* a, const Vector3* b);
extern "C" short Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
extern "C" int _Z14ApproachLinearRsss(short *r, short b, short c);
extern "C" void _ZN9Animation7AdvanceEv(void *a);
extern "C" void func_02012694(int a0, void *a1);
extern "C" void _ZN12CylinderClsn5ClearEv(void *c);
extern "C" void _ZN12CylinderClsn6UpdateEv(void *c);

int BobOmbBuddy::Behavior()
{
    Vector3 v;
    char *p;
    func_ov084_0212c9a8(((char *)this));
    p = (char*)_ZN5Actor13ClosestPlayerEv(((char *)this));
    if (p != 0) {
        Vector3 *ps = (Vector3 *)(((unsigned long long)(unsigned)(p + 0x5c)) & 0xFFFFFFFFFFFFFFFFULL);
        v = *ps;
        if (Vec3_HorzDist((Vector3*)((char *)&mPosX), &v) < 0x12c000) {
            short ang = Vec3_HorzAngle((Vector3*)((char *)&mPosX), &v);
            _Z14ApproachLinearRsss((short*)((char *)&unk_08e), ang, 0x100);
        }
    }
    _ZN9Animation7AdvanceEv((char *)(Animation *)&mModelAnim);
    if ((unsigned short)(mModelAnim.currFrame >> 12) == 0)
        func_02012694(0xd7, ((char *)this) + 0x74);
    func_ov084_0212ce50(((char *)this));
    _ZN12CylinderClsn5ClearEv((char *)&mMovingCylinderClsn);
    _ZN12CylinderClsn6UpdateEv((char *)&mMovingCylinderClsn);
    return 1;
}
