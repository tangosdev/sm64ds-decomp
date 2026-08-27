//cpp
// @symbol _ZN13QuestionBlock13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "QuestionBlock.h"

extern "C" {
void *_ZN5Model8LoadFileER13SharedFilePtr(void *sfp);
void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *thiz, void *bmd, int a, int b);
void *_ZN9Animation8LoadFileER13SharedFilePtr(void *sfp);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *thiz, void *bca, int a, int fx, unsigned int f);
void func_ov102_02149da8(void *c, int i);
void func_ov102_02149ff0(char *c);
void func_ov102_02149e38(char *self);
void *_ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void *sfp);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void *thiz, void *kcl, void *mtx, int fix, short s, void *clps);
unsigned char _ZN8dActor_c9TrackStarEjj(void *c, unsigned int a, unsigned int b);
}

extern char data_ov102_0214e7e8, data_ov102_0214e808, data_ov102_0214e7f8;
extern char data_ov002_0210d9e0, data_ov102_0214e800, data_ov102_0214e7f0;
extern char data_ov002_0210da40, data_ov102_0214e7d8, data_ov002_0210d9a0;
extern char data_ov102_0214e7e0, data_ov002_0210d9c0, data_ov102_0214e7d0;
extern char data_ov002_0210d954, data_ov002_0210da58, data_ov002_0210da18;
extern char data_ov002_0210d9d8, data_ov002_0210da30, data_ov002_0210d9b0;
extern char data_ov002_0210d9d0;

struct M12 { int w[12]; };

int QuestionBlock::InitResources()
{
    void *r5 = 0;
    switch (actorID - 0x14) {
    case 0:
        r5 = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov102_0214e7e8);
        _ZN9ModelBase7SetFileEP8BMD_Fileii((char *)&mModelAnim, _ZN5Model8LoadFileER13SharedFilePtr(&data_ov102_0214e808), 1, 0x19);
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char *)&mModelAnim, _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov102_0214e7f8), 0, 0x1000, 0);
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9e0);
        break;
    case 1:
    case 2:
        r5 = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov102_0214e800);
        break;
    case 3:
        r5 = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov102_0214e7f0);
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210da40);
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9e0);
        break;
    case 5:
        r5 = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov102_0214e7d8);
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9a0);
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9e0);
        break;
    case 4:
        r5 = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov102_0214e7e0);
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9c0);
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9e0);
        break;
    }

    _ZN9ModelBase7SetFileEP8BMD_Fileii((char *)&mModel, r5, 1, -1);
    mShadowModel.InitCuboid();
    func_ov102_02149da8((char *)this, 0);
    mTerminalVelocity = -0x3c000;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    func_ov102_02149ff0((char *)this);
    func_ov102_02149e38((char *)this);
    *(struct M12 *)&mShadowMat = *(struct M12 *)((char *)&mModel + 0x1c);
    {
        void *kcl = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(&data_ov102_0214e7d0);
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block((char *)&mMeshCollider, kcl, (char *)&mClsnMat, 0x199, mAngleY, &data_ov002_0210d954);
    }
    mHomePosY = mPosY;
    mContentType = (unsigned char)param1;
    if (mContentType == 0xff)
        mContentType = 0;
    if (mContentType == 1) {
        mStarId = (unsigned char)(param1 >> 8);
        if (mStarId == 0xff)
            mStarId = 0;
        mStarTracked = _ZN8dActor_c9TrackStarEjj((char *)this, mStarId, 2);
    }

    {
        int b16 = !(actorID != 0x16);
        if (b16 != 0) {
            mContentType = 0;
            _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210da58);
        }
    }

    {
        int b14 = !(actorID != 0x14);
        if (b14 == 0) {
            int b15 = !(actorID != 0x15);
            if (b15 == 0)
                goto end;
        }
    }

    switch (mContentType) {
    case 0:
    case 1:
        break;
    case 3:
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210da18);
        break;
    case 2:
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9d8);
        break;
    case 4:
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210da30);
        break;
    case 7:
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9b0);
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9d0);
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9e0);
        break;
    case 5:
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210da58);
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9b0);
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9d0);
        break;
    case 6:
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9b0);
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9d0);
        break;
    }
end:
    return 1;
}
