//cpp
#include "FortressWall.h"

namespace Sound {
int PlaySecretSound(dActor_c *actor, u16 *state);
}

extern "C" void func_020393a4(int *collider, int range);

/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *, int a, int b);

int FortressWall::Behavior()
{
    int ok = (actorID == 0x30);
    if (ok != 0 && mBroken != 0) {
        if (mMeshCollider.IsEnabled() != 0) {
            mMeshCollider.Disable();
        }
        if (Sound::PlaySecretSound(this, &mBreakSoundState) != 0) {
            Vector3 pos;
            pos.x = mPosX;
            pos.y = mPosY;
            pos.z = mPosZ;
            pos.y += 0xc8000;
            dActor_c::Spawn(0xb2, mStarId | 0x40, pos, (Vector3_16 *)0, mAreaId, -1);
            MarkForDestruction();
        }
        return 1;
    }
    func_020393a4((int *)&mMeshCollider, 0x240000);
    _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0x240000, 0);
    return 1;
}
