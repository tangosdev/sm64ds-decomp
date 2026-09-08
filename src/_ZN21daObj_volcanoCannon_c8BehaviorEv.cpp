//cpp
// @symbol _ZN21daObj_volcanoCannon_c8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "daObj_volcanoCannon_c.h"
struct Vector3_16f;

extern "C" unsigned short DecIfAbove0_Short(unsigned short* p);
extern "C" void _ZN8dActor_c9UpdatePosEP5dCc_c(void* a, dCc_c* c);
extern "C" void _ZN5dCc_c5ClearEv(void* c);
extern "C" void _ZN5dCc_c6UpdateEv(void* c);
extern "C" unsigned int _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
    unsigned int a, unsigned int b, Fix12i c, Fix12i d, Fix12i e, const Vector3_16f* f);

int daObj_volcanoCannon_c::Behavior()
{
    DecIfAbove0_Short(&mKillTimer);
    if (mState->behavior != 0)
        (this->*mState->behavior)();
    _ZN8dActor_c9UpdatePosEP5dCc_c(this, &mCylinderClsn);
    _ZN5dCc_c5ClearEv(&mCylinderClsn);
    _ZN5dCc_c6UpdateEv(&mCylinderClsn);
    {
        int b = (int)((mFlags & 8) != 0);
        if (b == 0) {
            mParticleID = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
                mParticleID, 0x129, mPosX, mPosY, mPosZ, 0);
        }
    }
    return 1;
}
