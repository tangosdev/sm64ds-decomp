//cpp
// @symbol _ZN6Player17PlayMammaMiaSoundEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int a, unsigned int b, const Vector3 *v);
}

void Player::PlayMammaMiaSound()
{
    /* mCamSpacePosX/Y/Z are three consecutive words; Actor.h declares them
       individually rather than as one Vector3. */
    _ZN5Sound13PlayCharVoiceEjjRK7Vector3(mCharacter, 0x27, (const Vector3 *)&mCamSpacePosX);
}
