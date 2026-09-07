//cpp
#include "StarSwitch.h"

extern "C" {
void LoadSilverStarAndNumber(void);
int _ZN5Model8LoadFileER13SharedFilePtr(int p);
void _ZN5Event8ClearBitEj(unsigned int b);
void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, int f, int a, int b);
void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void *self);
void func_ov002_020b9f80(char *self);
int _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(int p);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, int f, void *mtx, int fix, s16 s, int clps);
void func_020393c4(void *p, void *v);

extern int data_ov002_0211092c;
extern char data_ov002_021098e8[];
extern char data_ov002_021098ec[];
extern char data_ov002_021098f0[];
extern int func_ov002_020baa98;
}

int StarSwitch::InitResources()
{
    u8 idx;
    int f;

    mDrawScaleX = 0x1000;
    mDrawScaleY = 0x1000;
    mDrawScaleZ = 0x1000;
    mTargetActorID = 0;
    mTargetActor = 0;
    mHomeAreaId = mAreaId;

    {
        int b = 0;
        if (actorID == 0xc) b = 1;
        if (b) {
            mFlags |= 0x4000000;
            mSwitchType = 2;
            unk_351 = param1;
            if (unk_351 == 0xff)
                unk_351 = 0;
            mTimeLimit = (param1 >> 8) & 0xff;
            LoadSilverStarAndNumber();
            _ZN5Model8LoadFileER13SharedFilePtr((int)&data_ov002_0211092c);
            mResourceIdx = 1;
        } else {
            mSwitchType = param1 & 3;
            mEventBit = (param1 >> 3) & 0xf;
            mTimeLimit = (param1 >> 8) & 0xff;
            _ZN5Event8ClearBitEj(mEventBit);
            mResourceIdx = 0;
        }
    }

    idx = mResourceIdx;
    f = _ZN5Model8LoadFileER13SharedFilePtr(*(int*)(data_ov002_021098e8 + idx * 0xc));
    _ZN9ModelBase7SetFileEP8BMD_Fileii(&mModel, f, 1, -1);
    _ZN10dBgActor_c21UpdateModelPosAndRotYEv(this);
    func_ov002_020b9f80((char *)this);

    idx = mResourceIdx;
    f = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(*(int*)(data_ov002_021098ec + idx * 0xc));
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, f, &mClsnMat, 0x199, mAngleY,
        *(int*)(data_ov002_021098f0 + idx * 0xc));

    func_020393c4(&mMeshCollider, &func_ov002_020baa98);

    {
        u16 h = mTimeLimit;
        if (h == 0xff || h == 0)
            mTimeLimit = 0x190;
        else
            mTimeLimit *= 0xa;
    }
    unk_34f = 5;
    mPosY += 0x5000;
    mMusicFadeDone = 1;
    return 1;
}
