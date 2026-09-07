//cpp
// @symbol _ZN9SeesawBob8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "SeesawBob.h"
#include "dBgW.h"
typedef short s16;
struct V3 { int x, y, z; };
extern "C" {
int _ZN5Sound8PlayLongEjjjRK7Vector3s(unsigned int a, unsigned int b, unsigned int c, void* v, unsigned int d);
void func_ov095_021358cc(void* c, void* a, void* b, int d, int e, int f, int g);
void func_ov095_0213597c(char *t);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void* self, int a, int b);
void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void* self);
}

int SeesawBob::Behavior()
{
    int b = (int)(((*(s32 *)&mFlags) & 8) != 0);
    if (b != 0) {
        if (((dBgW *)((char*)&(*(u8 *)&mMeshCollider)))->IsEnabled()) {
            ((dBgW *)((char*)&(*(u8 *)&mMeshCollider)))->Disable();
        }
        return 1;
    }
    if (mPoundedThisFrame == 0) {
        func_ov095_021358cc(((char*)this), ((char*)this) + 0x8c, ((char*)this) + 0x324, 0, 6, 3, 3);
    }
    {
        int s = mAngleXSpeed;
        if (s < 0) s = (short)-s;
        if (s > 0xa) {
            mTiltSound = _ZN5Sound8PlayLongEjjjRK7Vector3s(
                mTiltSound, 3, 0x8b, ((char*)this) + 0x74, 0);
        }
    }
    if (mAngleX > 0x2000) mAngleX = 0x2000;
    if (mAngleX < -0x2000) mAngleX = -0x2000;
    func_ov095_0213597c(((char*)this));
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(((char*)this), 0, 0)) {
        _ZN10dBgActor_c19UpdateClsnPosAndRotEv(((char*)this));
    }
    mPoundedThisFrame = 0;
    return 1;
}
