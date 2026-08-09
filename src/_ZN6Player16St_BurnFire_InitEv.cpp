//cpp
// @symbol _ZN6Player16St_BurnFire_InitEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
/* SetAnim takes a Fix12<int> BY VALUE, which is the mwccarm 6az wall (runbook
   section 7): declaring it truthfully makes the compiler home the incoming
   registers to the stack. It stays an extern "C" declaration with a scalar in
   that slot. */
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void* c, unsigned int a, int b, int d, unsigned int e);
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int charID, unsigned int soundID, const Vector3* pos);
}

int Player::St_BurnFire_Init()
{
    mIsTakingDamage = 1;
    mIsAirborne = 1;
    mLandSoundPlayed = 0;
    _ZN6Player7SetAnimEji5Fix12IiEj(this, 0x16, 0, 0x1000, 0);
    mStateStep = 0;
    mStateTimer = 0x51;
    /* mFloorClass == 1 is the class SetFloorSurfaceInfo latches on (see the
       CLPS block in Player.h): on it the player is planted on the ground
       instead of being popped upward by the burn. */
    if (mFloorClass != 1) {
        mVertSpeed = 0x1e000;
    } else {
        mPosY = mGroundY;
    }
    mHorzSpeed = 0;
    mStateWork = 0;
    /* mCamSpacePosX/Y/Z are three consecutive words, which is the Vector3 the
       sound wants; Actor.h declares them individually rather than as one. */
    _ZN5Sound13PlayCharVoiceEjjRK7Vector3(mCharacter, 0x23, (const Vector3*)&mCamSpacePosX);
    return 1;
}
