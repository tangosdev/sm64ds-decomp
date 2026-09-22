//cpp
// @symbol _ZN6Player12St_Dive_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" void _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int, const Vector3&);
extern "C" int RandomIntInternal(int* seed);
extern "C" void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int, unsigned int, const Vector3&);
extern "C" void _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int, int, int, unsigned int);
extern int data_ov002_0210e160;

int Player::St_Dive_Init()
{
    unsigned char* p = (unsigned char*)((void*)this);
    _ZN5Sound9PlayBank0EjRK7Vector3(0x11, *(Vector3*)(p + 0x74));
    unsigned int r = (unsigned int)RandomIntInternal(&data_ov002_0210e160);
    _ZN5Sound13PlayCharVoiceEjjRK7Vector3(p[0x6d9], data_ov002_020ff100[(r >> 4) & 1], *(Vector3*)(p + 0x74));
    _ZN6Player7SetAnimEji5Fix12IiEj(((void*)this), 0x40, 0x40000000, 0x1000, 0);
    mStateStep = 0;
    int* yv = (int*)(p + 0x98);
    mVertSpeed = 0x1e000;
    yv[0] += 0xf000;
    if (mHorzSpeed > 0x28000) mHorzSpeed = 0x28000;
    mIsAirborne = 1;
    mLandSoundPlayed = 0;
    mStateWork = 0;
    mStatePhase = 0;
    mSlideStoppedTimer = 0;
    mIsSlidingOnGround = 0;
    mStateArg = mSlideType;
    return 1;
}
