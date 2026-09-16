//cpp
#include "Sound.h"
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
void _ZN7Minimap19UpdateLevelSpecificEv(void);
}

int TinyWater::Behavior()
{
    if (_ZN5Event6GetBitEj(0xe)) {
        if (mPosY <= mMinPosY) {
            if (Sound::PlaySecretSound((dActor_c *)this, (u16 *)&mSoundTimer))
                MarkForDestruction();
        } else {
            mPosY -= 0x1000;
            mSoundID = Sound::PlayLong(mSoundID, 3, 0x96, *(const Vector3 *)&mCamSpacePosX, 0);
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
