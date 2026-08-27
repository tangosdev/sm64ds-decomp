//cpp
// @symbol _ZN6Player24St_MetalWaterGround_InitEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned, unsigned, void*);
}

int Player::St_MetalWaterGround_Init()
{
    int* p = *(int**)((char*)&mHeldObj);
    int b0 = (p != 0);
    if (b0) {
        int h = *(unsigned short*)((char*)p+0xc);
        int b1 = (h == 0x10c);
        if (!b1) {
            int b2 = (h == 0x10b);
            if (!b2) goto tail;
        }
        *(int*)(int)((char*)p+0xb0) &= ~0x4000;
        *(int*)(int)((char*)*(int**)((char*)&mHeldObj)+0xb0) &= ~0x100;
        *(int**)((char*)&mHeldObj) = 0;
    }
tail:
    _ZN5Sound13PlayCharVoiceEjjRK7Vector3(mCharacter, 0x2e, ((char*)this)+0x74);
    mVertAccel = -0x1800;
    mTerminalVelocity = -0x14000;
    if (mHorzSpeed >= 0x14000) mHorzSpeed = 0x14000;
    mIsUnderwater = 1;
    return 1;
}
