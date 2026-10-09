//cpp
#include "Sound.h"
// @symbol _ZN6Player14St_Thrown_InitEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
/* SetAnim takes a Fix12<int> by value -- the mwccarm 6az wall, runbook
   section 7 -- so it stays extern "C" with a scalar in that slot. */
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned,int,int,unsigned);
}

int Player::St_Thrown_Init()
{
    int b = (mFlags & 0x400) ? 1 : 0;
    if (b)
        mStateTimer = 4;
    Sound::PlayCharVoice(mCharacter, 0x1d, *(const Vector3*)&mCamSpacePosX);
    _ZN6Player7SetAnimEji5Fix12IiEj(this, 0x15, 0x40000000, 0x1000, 0x12);
    mStateStep = 0;
    mStateWork = 0;
    mAngleY = mPrevAngleY;
    mStateArg = 0;
    return 1;
}
