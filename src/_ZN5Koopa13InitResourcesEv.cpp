//cpp
// @symbol _ZN5Koopa13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
#include "SharedFilePtr.h"
/* recovered: named members + shared header, real C++ method */
#include "Koopa.h"

extern SharedFilePtr* data_ov062_0211cee8[9];
extern SharedFilePtr* data_ov062_0211ced8[2];
extern SharedFilePtr* data_ov062_0211cee0[2];

extern "C" {
extern void* _ZN9Animation8LoadFileER13SharedFilePtr(SharedFilePtr* f);
extern BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr* f);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, BMD_File* f, int a, int b);
extern int _ZN11ShadowModel12InitCylinderEv(void* self);
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void* self, dActor_c* a, int r, int h, unsigned int e, unsigned int g);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void* self, dActor_c* a, int r, int h, Vector3_16* p, int q);
extern void _ZN10dBgCh_Actr19StartDetectingWaterEv(void* self);
extern void LoadBlueCoinModel(void* c);
}

int Koopa::InitResources()
{
    int i;
    int r, h;
    int kind;
    BMD_File* f;
    unsigned int isSpecial;

    kind = param1 & 1;
    mModelIndex = kind;

    for (i = 0; i < 9; i++)
        _ZN9Animation8LoadFileER13SharedFilePtr(data_ov062_0211cee8[i]);

    f = _ZN5Model8LoadFileER13SharedFilePtr(data_ov062_0211ced8[mModelIndex]);
    if (_ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this) + 0x300, f, 1, -1) == 0)
        return 0;

    if (_ZN11ShadowModel12InitCylinderEv((char*)&mShadowModel) == 0)
        return 0;

    isSpecial = actorID == 0xcc;
    if (isSpecial)
    {
        mKoopaVariant = 2;
        mAnimSpeed = 0x2000;
        mScaleX = 0x599;
        mScaleY = 0x599;
        mScaleZ = 0x599;
        r = 0x1e000;
        h = 0x32000;
    }
    else
    {
        mKoopaVariant = 0;
        mAnimSpeed = 0x1000;
        mScaleX = 0xa66;
        mScaleY = 0xa66;
        mScaleZ = 0xa66;
        r = 0x3c000;
        h = 0x64000;
        _ZN5Model8LoadFileER13SharedFilePtr(data_ov062_0211cee0[mModelIndex]);
    }

    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(((char*)this) + 0x110, (dActor_c*)((char*)this), r, h, 0x200000, 0xb6efe0);

    mStateTimer = 0;
    *(short*)(((char*)this) + 0x300 + 0xc4) = 0;
    mState = 1;
    func_ov062_02117994(((char*)this), 0);

    unk_106 = 0;
    *(short*)(((char*)this) + 0x300 + 0xca) = 0;
    unk_39c = mPosX;
    unk_3a0 = mPosY;
    unk_3a4 = mPosZ;
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(((char*)this) + 0x144, (dActor_c*)((char*)this), 0x32000, 0x32000, 0, 0);

    _ZN10dBgCh_Actr19StartDetectingWaterEv((char*)&mWithMeshClsn);

    unk_108 = 3;
    LoadBlueCoinModel(((char*)this));

    return 1;
}
