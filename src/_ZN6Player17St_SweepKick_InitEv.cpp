//cpp
// @symbol _ZN6Player17St_SweepKick_InitEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
/* SetAnim takes a Fix12<int> by value -- the mwccarm 6az wall, runbook
   section 7 -- so it stays extern "C" with a scalar in that slot. */
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
extern int _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int,unsigned int,const Vector3*);
}

int Player::St_SweepKick_Init()
{
    _ZN6Player7SetAnimEji5Fix12IiEj(this, 0x68, 0x40000000, 0x1000, 0);
    mHorzSpeed = 0;
    unk_726 = 0;
    /* mCamSpacePosX/Y/Z are three consecutive words; Actor.h declares them
       individually rather than as one Vector3. */
    _ZN5Sound13PlayCharVoiceEjjRK7Vector3(mCharacter, 0xa, (const Vector3*)&mCamSpacePosX);
    return 1;
}
