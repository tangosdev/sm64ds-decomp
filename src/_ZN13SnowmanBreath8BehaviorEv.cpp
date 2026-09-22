//cpp
#include "types.h"
// @symbol _ZN13SnowmanBreath8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_Player.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "SnowmanBreath.h"
extern u8 data_0209f2d8[];

extern "C" {
extern int _ZN6Player9StartTalkER7fBase_cb(void *self, void *actor, int b);
/* Moved INSIDE the extern "C" block. A bare `extern` on a mangled name in a
   //cpp file mangles it a second time -- this one was reaching the linker as
   _Z48_ZN6Player11ShowMessage... and sat in the unresolved baseline. Byte
   gates cannot see it, because relocations compare as wildcards; only
   check_references does. */
extern int _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(void *self, void *actor,
                                                            unsigned int msg,
                                                            const Vector3 *pos,
                                                            unsigned int a,
                                                            unsigned int b);
}
extern "C" {
extern int _ZN6Player12GetTalkStateEv(void *self);
/* Same double-mangle defect as ShowMessage above: outside this block the
   bare `extern` reached the linker as _Z33_ZN5Sound8PlayLongEjjjRK7Vector3sijjPvj. */
extern int _ZN5Sound8PlayLongEjjjRK7Vector3s(int handle, unsigned int a,
                                             unsigned int b, void *pos,
                                             unsigned int c);
}

int SnowmanBreath::Behavior()
{
    int b;
    SnowmanBreathParticle *slotBase;
    u8 *idx;
    Vector3 pos;
    int i;
    SnowmanBreathParticle *slot;
    int zero;

    b = data_0209f2d8[0];
    b = (b == 2);
    if (b) {
        return 1;
    }

    if (mTalkDone == 0) {
        switch (mTalkState) {
        case 0:
            if (IsPlayerInRange() == 0) {
                break;
            }
            if (_ZN6Player9StartTalkER7fBase_cb(mTalkPlayer, this, 1) == 0) {
                break;
            }
            mTalkState++;
            break;
        case 1:
            pos.x = mPosX;
            zero = 0;
            pos.y = mPosY;
            pos.z = mPosZ;
            pos.y = pos.y + 0x12c000;
            if (_ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(
                    mTalkPlayer, this, 0xbb, &pos, zero, zero) == 0) {
                break;
            }
            mTalkState++;
            break;
        case 2:
            if (_ZN6Player12GetTalkStateEv(mTalkPlayer) == -1) {
                _ZN6Player18HasFinishedTalkingEv(mTalkPlayer);
                mTalkDone = 1;
            }
            break;
        }
    } else {
        if (IsPlayerInRange() != 0) {
            mTalkTimer++;
            if ((mTalkTimer & 7) != 0) {
                slotBase = mParticles;
                idx = &mNextParticle;
                do {
                    slot = slotBase + mNextParticle;
                    b = slot->TrySpawn(*mTalkPlayer);
                    *idx = *idx + 1;
                    *idx = *idx % 0x32;
                } while (b == 0);
            }
            mSoundHandle =
                _ZN5Sound8PlayLongEjjjRK7Vector3s(mSoundHandle, 3, 0x184,
                                                  &mCamSpacePosX, 0);
        }
    }

    slot = mParticles;
    i = 0;
    do {
        slot->Behavior();
        i++;
        slot++;
    } while (i < 0x32);

    return 1;
}
