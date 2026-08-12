//cpp
// @symbol _ZN13BasementWater8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "BasementWater.h"
extern "C" {
extern void _ZN5Sound15PlaySecretSoundEP5ActorPt(void* a, unsigned short* p);
extern unsigned int _ZN5Sound8PlayLongEjjjRK7Vector3s(unsigned int a, unsigned int b, unsigned int d, void* v, unsigned int e);
extern void _ZN7Minimap19UpdateLevelSpecificEv(void);
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void* thiz);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void* thiz);
extern void _ZN9Animation7AdvanceEv(void* thiz);
}
extern int data_0209caa0[];
extern int data_0209f32c;

int BasementWater::Behavior()
{
    if (unk_33e != 0)
        _ZN5Sound15PlaySecretSoundEP5ActorPt(((char*)this), (unsigned short*)((char*)&unk_33c));
    if (mPosY <= mLoweredY) {
        mPosY = mLoweredY;
    } else if (*(int*)((char*)data_0209caa0 + 8) & 0x80000) {
        unk_338 = _ZN5Sound8PlayLongEjjjRK7Vector3s(
            unk_338, 3, 0x96, (void*)((char*)&mCamSpacePosX), 0);
        *(int*)((char*)&mPosY) -= 0x5000;
        unk_33e = 1;
        if (mPosY <= mLoweredY) {
            mPosY = mLoweredY;
            _ZN7Minimap19UpdateLevelSpecificEv();
        }
    }
    *(int*)&data_0209f32c = mPosY;
    _ZN8Platform21UpdateModelPosAndRotYEv(((char*)this));
    _ZN8Platform19UpdateClsnPosAndRotEv(((char*)this));
    (*(s32 *)((char *)&mTextureTransformer + 0xc)) = 0x1000;
    _ZN9Animation7AdvanceEv((char*)&(*(u8 *)&mTextureTransformer));
    return 1;
}
