//cpp
#include "Sound.h"
// @symbol _ZN13BasementWater8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "BasementWater.h"
extern "C" {
extern void _ZN7Minimap19UpdateLevelSpecificEv(void);
extern void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void* thiz);
extern void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void* thiz);
extern void _ZN9Animation7AdvanceEv(void* thiz);
}
extern int data_0209caa0[];
extern int data_0209f32c;

int BasementWater::Behavior()
{
    if (mWasJustDrained != 0)
        Sound::PlaySecretSound((dActor_c *)(((char*)this)), (u16 *)(((char*)&mSoundTimer)));
    if (mPosY <= mLoweredY) {
        mPosY = mLoweredY;
    } else if (*(int*)((char*)data_0209caa0 + 8) & 0x80000) {
        mSoundID = Sound::PlayLong(mSoundID, 3, 0x96, *(const Vector3 *)(((char*)&mCamSpacePosX)), 0);
        *(int*)((char*)&mPosY) -= 0x5000;
        mWasJustDrained = 1;
        if (mPosY <= mLoweredY) {
            mPosY = mLoweredY;
            _ZN7Minimap19UpdateLevelSpecificEv();
        }
    }
    *(int*)&data_0209f32c = mPosY;
    _ZN10dBgActor_c21UpdateModelPosAndRotYEv(((char*)this));
    _ZN10dBgActor_c19UpdateClsnPosAndRotEv(((char*)this));
    (*(s32 *)((char *)&mTextureTransformer + 0xc)) = 0x1000;
    _ZN9Animation7AdvanceEv((char*)&(*(u8 *)&mTextureTransformer));
    return 1;
}
