//cpp
#include "types.h"
// @symbol _ZN13WaterfallMist13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "WaterfallMist.h"
extern void _ZN9Animation8LoadFileER13SharedFilePtr(void *sfp);
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *sfp);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *thiz, void *bmd, int a, int b);
extern int _ZN11ShadowModel12InitCylinderEv(void *thiz);
extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void *thiz, void *actor, s32 f1, s32 f2, u32 a, u32 b);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void *thiz, void *actor, s32 f1, s32 f2, void *v, void *w);
extern int _ZN5Actor13ClosestPlayerEv(void *thiz);
extern void func_ov002_020b7f2c(void *c, void *p);
extern void func_ov002_020b7f7c(void *thiz);
extern void func_ov001_020ab228(void *c, void *a1, int idx, int a3, int a5);

extern char data_ov002_0210de50;
extern char data_ov002_0210de60;
extern char data_ov002_0210de48;
extern char data_ov002_0210de28;
extern char data_ov002_0210de08;
extern char data_ov002_0210de20;
extern char data_ov002_0210de40;
extern char data_ov002_0210de10;
extern char data_ov002_0210de00;
extern char data_ov002_0210de58;
extern char data_ov002_0210de18;
extern char data_ov002_0210de30;
extern char data_ov002_0210de38;
extern char data_ov002_0210df54;

int WaterfallMist::InitResources()
{
    int flag;
    int v;

    mType = mParam & 0xff;
    mModelIndex = (mParam >> 8) & 0xf;
    unk_400 = (mParam >> 0xc) & 0xf;

    if (mType == 0xff)
        mType = 0;

    if (mModelIndex >= 3)
        return 0;

    if (mType == 0x11 || mType == 4) {
        if (unk_400 > 2)
            unk_400 = 0;
    } else {
        if (unk_400 > 1)
            unk_400 = 0;
    }

    switch (mType) {
    case 0xf:
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov002_0210de50);
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov002_0210de60);
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov002_0210de48);
        break;
    case 0x14:
    case 0x15:
    case 0x16:
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov002_0210de28);
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov002_0210de08);
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov002_0210de20);
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov002_0210de40);
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov002_0210de10);
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov002_0210de00);
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov002_0210de58);
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov002_0210de18);
        break;
    case 0x10:
    case 0x11:
    case 0x12:
    case 0x13:
    default:
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov002_0210de30);
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov002_0210de38);
        break;
    }

    if (_ZN9ModelBase7SetFileEP8BMD_Fileii(((char *)this) + 0x300,
            _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_020ff0ac[mModelIndex]),
            1, -1) == 0)
        return 0;

    _ZN11ShadowModel12InitCylinderEv((char *)&mShadowModel);

    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;

    flag = 0;

    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(((char *)this) + 0x110, ((char *)this), 0x1e000, 0x1e000, 0x800002, 0);
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(((char *)this) + 0x144, ((char *)this), 0x1e000, 0x16000, 0, 0);

    unk_3c4 = mPosX;
    unk_3c8 = mPosY;
    unk_3cc = mPosZ;

    unk_09c = -0x1000;
    unk_0a0 = -0x1e000;

    switch (mType) {
    default:
        break;
    case 0:
        *(s32 *)(((long long)((char *)&unk_12c))) |= 0x8000;
        unk_400 = 4;
        func_ov002_020b7f2c(((char *)this), &data_ov002_0210df64);
        break;
    case 1:
        unk_400 = 0xff;
        func_ov002_020b7f2c(((char *)this), &data_ov002_0210df84);
        break;
    case 2:
        *(s32 *)(((long long)((char *)&unk_12c))) |= 0x8000;
        unk_400 = 4;
        func_ov002_020b7f2c(((char *)this), &data_ov002_0210df04);
        break;
    case 3:
        unk_400 = 0xff;
        func_ov002_020b7f2c(((char *)this), &data_ov002_0210df24);
        break;
    case 19:
        unk_400 = 0xff;
        unk_3c0 = _ZN5Actor13ClosestPlayerEv(((char *)this));
        func_ov002_020b7f2c(((char *)this), &data_ov002_0210df54);
        unk_401 = 1;
        break;
    case 20:
    case 21:
    case 22:
        func_ov002_020b7f7c(((char *)this));
        /* fallthrough */
    case 10:
    case 15:
        unk_400 = 0xff;
        func_ov002_020b7f2c(((char *)this), &data_ov002_0210df74);
        break;
    case 12:
        unk_400 = 4;
        func_ov002_020b7f2c(((char *)this), &data_ov002_0210df14);
        break;
    case 13:
        unk_400 = 4;
        func_ov002_020b7f2c(((char *)this), &data_ov002_0210df44);
        break;
    case 14:
        unk_400 = 2;
        unk_114 = 0x32000;
        unk_118 = 0x32000;
        mParam -= 0xa;
        mType = 4;
        func_ov002_020b7f2c(((char *)this), &data_ov002_0210df34);
        break;
    case 17:
        unk_114 = 0x32000;
        unk_118 = 0x32000;
        flag = 1;
        func_ov002_020b7f2c(((char *)this), &data_ov002_0210df34);
        break;
    case 6:
    case 7:
    case 8:
    case 9:
        unk_400 = 0xff;
        unk_114 = 0x32000;
        unk_118 = 0x32000;
        func_ov002_020b7f2c(((char *)this), &data_ov002_0210df34);
        break;
    case 5:
    case 11:
    case 18:
        mAreaId = -1;
        /* fallthrough */
    case 16:
        unk_400 = 3;
        /* fallthrough */
    case 4:
        unk_114 = 0x32000;
        unk_118 = 0x32000;
        func_ov002_020b7f2c(((char *)this), &data_ov002_0210df34);
        break;
    }

    unk_35c = 0x1000;

    if (unk_400 != 0xff) {
        if (flag != 0)
            v = 1;
        else
            v = 0;
        func_ov001_020ab228(((char *)this) + 0x3d0, ((char *)this), mModelIndex & 0xff, unk_400, v);
    }

    mParam &= 0xfff;
    return 1;
}
