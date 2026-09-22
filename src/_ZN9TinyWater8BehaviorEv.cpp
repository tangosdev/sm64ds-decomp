//cpp
// @symbol _ZN9TinyWater8BehaviorEv
/* TinyWater::Behavior -- once event bit 0xe is set, drain the pool: sink one
 * unit a frame while looping the drain sound, and when the surface reaches
 * mMinPosY play the secret jingle and destroy the actor once it finishes.
 *
 * Sound, Event and Minimap have no headers in this tree, so those three stay
 * extern-C mangled free functions. Animation::Advance and
 * fBase_c::MarkForDestruction are declared, and are reached as members.
 */
#include "TinyWater.h"

extern "C" {
int  _ZN5Event6GetBitEj(u32 bit);
int  _ZN5Sound15PlaySecretSoundEP8dActor_cPt(dActor_c *actor, u16 *timer);
int  _ZN5Sound8PlayLongEjjjRK7Vector3s(u32 handle, u32 a, u32 b, const Vector3 *pos, s16 c);
void _ZN7Minimap19UpdateLevelSpecificEv(void);
}

int TinyWater::Behavior()
{
    if (_ZN5Event6GetBitEj(0xe)) {
        if (mPosY <= mMinPosY) {
            if (_ZN5Sound15PlaySecretSoundEP8dActor_cPt(this, &mSoundTimer))
                MarkForDestruction();
        } else {
            mPosY -= 0x1000;
            mSoundID = _ZN5Sound8PlayLongEjjjRK7Vector3s(mSoundID, 3, 0x96, (Vector3 *)&mCamSpacePosX, 0);
            if (mPosY <= mMinPosY) {
                mPosY = mMinPosY;
                _ZN7Minimap19UpdateLevelSpecificEv();
            }
        }
    }
    mTextureTransformer.speed = 0x1000;
    mTextureTransformer.Advance();
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    return 1;
}
