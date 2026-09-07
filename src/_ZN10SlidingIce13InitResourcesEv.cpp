//cpp
#include "SlidingIce.h"

extern "C" {
int _ZN5Model8LoadFileER13SharedFilePtr(void *);
int _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void *);
int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *, int, int, int);
void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void *);
void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *, int, void *, int, int, void *);
void func_020393d4(int *p, int v);
extern int data_ov027_02113be8[];
extern char data_ov027_02113be0[];
extern char data_ov027_02113108[];
extern int _ZN4dBgW21UpdatePosWithVelocityERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_[];
}

int SlidingIce::InitResources()
{
    _ZN5Model8LoadFileER13SharedFilePtr(data_ov027_02113be8);
    _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov027_02113be0);

    int on = (actorID == 0x5d);
    if (on) {
        if (_ZN9ModelBase7SetFileEP8BMD_Fileii(&mModel, data_ov027_02113be8[1], 1, -1) == 0)
            return 0;
        _ZN10dBgActor_c21UpdateModelPosAndRotYEv(this);
        _ZN10dBgActor_c19UpdateClsnPosAndRotEv(this);
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            &mMeshCollider, *(int *)(data_ov027_02113be0 + 4), &mClsnMat,
            0x1000, mAngleY, data_ov027_02113108);
        func_020393d4((int *)&mMeshCollider,
            (int)_ZN4dBgW21UpdatePosWithVelocityERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
        mMeshCollider.unk_4c = 0;
        mMeshCollider.Enable(this);
        mHorzSpeed = 0x2d000;
        mDelayTimer = 0x64;
        mPrevAngleY = -0x4000;
        mMinPosY = mPosY - 0xc8000;
    } else {
        mDelayTimer = (u8)mNumToBigIce * 0x14;
        mNumToBigIce = 5;
    }
    return 1;
}
