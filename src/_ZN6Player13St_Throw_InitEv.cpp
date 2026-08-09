//cpp
// @symbol _ZN6Player13St_Throw_InitEv
/* recovered: named members + shared header, real C++ method
 *
 * mHeldObj is declared s32 in Player.h but is an actor pointer: this reads
 * ActorBase's mFlags (+0xb0) and actorID (+0xc) through it. Actor IDs 0xbd
 * and 0xbe get the alternate throw animation.
 */
#include "Player.h"
extern "C" {
extern int _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned a, unsigned b, const Vector3& v);
/* SetAnim takes a Fix12<int> by value -- the mwccarm 6az wall, runbook
   section 7 -- so it stays extern "C" with a scalar in that slot. */
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned,int,int,unsigned);
}

int Player::St_Throw_Init()
{
    Actor* held = (Actor*)mHeldObj;
    int b = (held->mFlags & 0x200) ? 1 : 0;
    if (!b) {
        unsigned short a = held->actorID;
        int t1 = (a == 0xbd);
        if (!t1) {
            int t2 = (a == 0xbe);
            if (!t2) goto anim30;
        }
        _ZN6Player7SetAnimEji5Fix12IiEj(this, 0x91, 0x40000000, 0x1000, 0);
        goto voice;
anim30:
        _ZN6Player7SetAnimEji5Fix12IiEj(this, 0x30, 0x40000000, 0x1000, 0);
voice:
        _ZN5Sound13PlayCharVoiceEjjRK7Vector3(mCharacter, 0x13, *(const Vector3*)&mCamSpacePosX);
    } else {
        _ZN6Player7SetAnimEji5Fix12IiEj(this, 0x8a, 0x40000000, 0x1000, 0);
    }
    return 1;
}
