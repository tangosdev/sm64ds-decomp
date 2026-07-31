//cpp
// @symbol _ZN10MrBlizzard13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_PathPtr.h"
#include "decl_SaveData.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "MrBlizzard.h"
typedef int s32;
typedef unsigned int u32;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned char u8;
typedef signed char s8;

struct Vec3 { int x, y, z; };

extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *f, int a, int b);
extern void _ZN11ShadowModel12InitCylinderEv(void *self);
extern void *_ZN9Animation8LoadFileER13SharedFilePtr(void *fp);
extern int _ZN5Actor18GetBitInDeathTableEv(void *self);
extern void _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(u32 a, u32 b, const void *v, const void *v16, int e, int f);
extern void _ZN7PathPtrC1Ev(void *self);
extern void _ZN7PathPtr6FromIDEj(void *self, u32 id);
extern void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(
    void *self, void *actor, const void *v, int d, int e, u32 f, u32 g);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(
    void *self, void *actor, int b, int c, void *v16, int e);
extern void _ZNK7PathPtr7GetNodeER7Vector3j(void *self, void *v, u32 idx);
extern void func_ov081_02125488(void *c, void *p);

extern void *data_ov081_02128d90;
extern void *data_ov081_02128d98;
extern void *data_ov081_02128d88;
extern void *data_ov081_02128da0;
extern struct Vec3 data_ov081_02128998;
extern void *data_ov081_02128e54;
extern void *data_ov081_02128e84;

int MrBlizzard::InitResources()
{
    struct Vec3 v;

    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char *)this) + 0x30c,
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov081_02128db0), 1, -1);
    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov081_02128d90);
    _ZN11ShadowModel12InitCylinderEv((char *)&mShadowModel);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov081_02128d98);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov081_02128db8);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov081_02128da8);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov081_02128d88);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov081_02128da0);

    mPathId = mParam & 0xff;
    mType = (mParam & 0xff00) >> 8;
    if (mPathId == 0xff)
        mPathId = 0;
    if (mType == 0xff)
        mType = 0;

    unk_44c = mPosX;
    unk_450 = mPosY;
    unk_454 = mPosZ;

    if (_ZN5Actor18GetBitInDeathTableEv(((char *)this)) != 0) {
        if (mType == 2) {
            if (_ZN8SaveData16HasPlayerLostCapEv() != 0) {
                _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0xdf, 0x200,
                    ((char *)this) + 0x44c, 0, mAreaId, -1);
            } else {
                _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0xdf, 0x300,
                    ((char *)this) + 0x44c, 0, mAreaId, -1);
            }
        }
        if (mType != 3)
            return 0;
    }

    unk_3fc = 0;
    unk_400 = 0;

    if (mType == 0) {
        char pp[8];
        _ZN7PathPtrC1Ev(pp);
        _ZN7PathPtr6FromIDEj(pp, mPathId);
        unk_420 = _ZNK7PathPtr8NumNodesEv(pp);
    }

    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    unk_0a0 = -0xc8000;

    v.x = data_ov081_02128998.x;
    v.y = data_ov081_02128998.y;
    v.z = data_ov081_02128998.z;
    _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(
        ((char *)this) + 0x110, ((char *)this), &v, 0x3c000, 0x96000, 0x200004, 0x42050);

    unk_08e = unk_094;
    unk_414 = unk_08e;
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(
        ((char *)this) + 0x150, ((char *)this), 0x14000, 0xf000, 0, 0);

    unk_108 = 1;
    unk_10a = 2;

    if (mType == 0) {
        char pp[8];
        _ZN7PathPtrC1Ev(pp);
        _ZN7PathPtr6FromIDEj(pp, mPathId);
        _ZNK7PathPtr7GetNodeER7Vector3j(pp, ((char *)this) + 0x5c, unk_424);
        unk_424 = 1;
        unk_09c = -0x2000;
        unk_0b0 = 0x10000000;
        func_ov081_02125488(((char *)this), &data_ov081_02128e54);
    } else {
        *(s32 *)(((int)((char *)this) + 0x60) & 0xFFFFFFFFFFFFFFFF) -= 0x118000;
        func_ov081_02125488(((char *)this), &data_ov081_02128e84);
        if (mType == 3) {
            unk_108 = 0;
            unk_10a = 0;
            unk_0b0 = 2;
        }
        if (mType == 2)
            unk_0b0 = 0x8000002;
        else
            unk_0b0 = 3;
    }

    return 1;
}
