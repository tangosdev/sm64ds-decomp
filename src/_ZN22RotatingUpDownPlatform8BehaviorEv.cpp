//cpp
// @symbol _ZN22RotatingUpDownPlatform8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "RotatingUpDownPlatform.h"
extern "C" void _Z14ApproachLinearRiii(int*, int, int);
extern "C" void _ZN4dBgW21UpdatePosWithVelocityERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_();
/* Not a member above, though it is one on the real class. Declared `int
   IsClsnInRange(int, int)` it mangles _ZN10dBgActor_c13IsClsnInRangeEii, which nothing
   defines; the ROM's symbol takes two Fix12<int>, and that type is an aggregate with
   no converting constructor from int, so materialising a zero one costs stack traffic
   the ROM does not have. The pair goes in registers exactly as two ints either way. */
extern "C" int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
struct PmfEntry;
extern "C" void func_020393d4(void *p, void *v);

typedef void (dBgActor_c::*PMF)();
struct PmfRow { PMF pmf; };
extern "C" PmfRow data_ov091_021354e0[];

/* How far the platform gives under a load, and how fast it gets there. */
static const int cSinkDepth = 0x1e000;
static const int cSinkRate  = 0x5000;

int RotatingUpDownPlatform::Behavior()
{
    char *s = (char*)((dBgActor_c *)this);
    int old = mState;
    (((dBgActor_c *)this)->*data_ov091_021354e0[old].pmf)();
    mStateTimer += 1;
    /* Restart the clock on a state change, and stop feeding the collider a
       velocity while the state is switching over. */
    if (old != mState) {
        mStateTimer = 0;
        func_020393d4(s + 0x124, 0);
    } else {
        func_020393d4(s + 0x124, (void*)&_ZN4dBgW21UpdatePosWithVelocityERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
    }
    if (mVariant == 0) {
        int rate = cSinkRate;
        int saved = mPosY;
        _Z14ApproachLinearRiii(&mSinkOffsetY, mIsPressed ? cSinkDepth : 0, rate);
        *(int*)(((int)s + 0x60)) -= mSinkOffsetY;
        mPosY = saved;
    }
    ((dBgActor_c *)this)->UpdateModelPosAndRotY();
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0) != 0)
        ((dBgActor_c *)this)->UpdateClsnPosAndRot();
    mIsPressed = 0;
    return 1;
}
