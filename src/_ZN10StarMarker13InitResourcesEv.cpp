//cpp
#include "types.h"
// @symbol _ZN10StarMarker13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "StarMarker.h"
struct Vec3 { s32 x, y, z; };
struct dBgCh_Gnd { char pad[0x50]; };

extern "C" {
extern void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(void *self, void *actor, const void *v, int d, int e, u32 f, u32 g);
extern void _ZN9dBgCh_GndC1Ev(void *self);
extern void _ZN5dBgCh19StartDetectingWaterEv(void *self);
extern void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(void *self, const void *v, void *actor);
extern int _ZN9dBgCh_Gnd10DetectClsnEv(void *self);
extern void *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(u32 a, u32 b, const void *v, const void *v16, int e, int f);
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *f, int a, int b);
extern void _ZN9dBgCh_GndD1Ev(void *self);
extern int _ZN11ShadowModel12InitCylinderEv(void *self);
extern int IsStarCollectedInCurLevel(u8 x);
extern void _ZN7fBase_c18MarkForDestructionEv(void *self);
extern int _ZN8dActor_c18GetBitInDeathTableEv(void *self);
}

extern char data_ov002_0210d9a8;
extern char data_ov002_0211092c;
extern u8 data_0209f2d8;
extern s8 data_0209f2f8;

int StarMarker::InitResources()
{
    struct Vec3 pos;
    struct Vec3 v0;
    struct Vec3 v4;
    struct Vec3 v5;
    struct dBgCh_Gnd rg;
    u32 raw;
    u8 kind;
    int r3;

    raw = ((u32)mParam >> 4) & 0xf;
    kind = (u8)raw;
    mFlags = 0;
    v0.x = 0;
    v0.y = -0x50000;
    v0.z = 0;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(((char *)this) + 0xd4, ((char *)this), &v0, 0x50000, 0xa0000, 0x100002, 0x8000);
    _ZN9dBgCh_GndC1Ev(&rg);
    _ZN5dBgCh19StartDetectingWaterEv(&rg);

    {
        s32 pyb = mPosY;
        s32 pz = mPosZ;
        s32 px = mPosX;
        s32 pyy = pyb + 0x1e000;
        pos.x = px;
        pos.y = pyy;
        pos.z = pz;
    }
    _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(&rg, &pos, ((char *)this));
    if (_ZN9dBgCh_Gnd10DetectClsnEv(&rg) != 0)
        mGroundY = *(s32 *)((char *)&rg + (0x80 - 0x3c));

    r3 = 0;
    mStarID = (u8)(mParam & 0xf);
    mState = 0;

    if (kind == 6) {
        void *sp;
        sp = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0xb2, mStarID | 0x60, ((char *)this) + 0x5c, (void *)0, (s8)mAreaId, -1);
        if (sp != 0) {
            u16 *p = (u16 *)(((int)sp + 0x4a2));
            *p = (u16)(*p | 0x80);
        }
        _ZN9ModelBase7SetFileEP8BMD_Fileii(((char *)this) + 0x114, _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0211093c), 1, 0x18);
        _ZN9dBgCh_GndD1Ev(&rg);
        return 0;
    }

    if (kind == 4) {
        u8 *p;
        mState = 2;
        p = (u8 *)(((int)((char *)this) + 0x1db));
        *p = (u8)(*p | 2);
        v4.x = 0;
        v4.y = -0x50000;
        v4.z = 0;
        _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(((char *)this) + 0xd4, ((char *)this), &v4, 0x50000, 0xa0000, 0x100004, 0);
        mAppearTimer = 0;
    } else if (kind == 5) {
        mState = 3;
        v5.x = 0;
        v5.y = -0x50000;
        v5.z = 0;
        _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(((char *)this) + 0xd4, ((char *)this), &v5, 0x50000, 0xa0000, 1, 0);
    } else if (kind & 1) {
        mState = 1;
        if (kind & 2) {
            u8 *p = (u8 *)(((int)((char *)this) + 0x1db));
            *p = (u8)((*p & ~1) | 1);
        }
        {
            u8 *p = (u8 *)(((int)((char *)this) + 0x1db));
            *p = (u8)(*p | 8);
        }
    } else {
        u8 *p = (u8 *)(((int)((char *)this) + 0x1db));
        u8 nv = (u8)((((int)kind >> 1) & 1) ^ 1);
        *p = (u8)((*p & ~2) | ((nv & 1) << 1));
    }

    if (mState != 0) {
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9a8);
        if (_ZN9ModelBase7SetFileEP8BMD_Fileii(((char *)this) + 0x114, _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0211092c), 1, 0x19) == 0) {
            _ZN9dBgCh_GndD1Ev(&rg);
            return 0;
        }
    } else {
        if (_ZN9ModelBase7SetFileEP8BMD_Fileii(((char *)this) + 0x114, _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0211093c), 1, 0x18) == 0) {
            _ZN9dBgCh_GndD1Ev(&rg);
            return 0;
        }
    }

    if (_ZN11ShadowModel12InitCylinderEv((char *)&mShadowModel) == 0) {
        _ZN9dBgCh_GndD1Ev(&rg);
        return 0;
    }

    if (((u32)(mFlags << 0x1e) >> 0x1f) == 0) {
        s32 *p = (s32 *)(((int)((char *)this) + 0xec));
        *p = *p | 1;
    }
    r3 = 0;
    mSpawnPosX = mPosX;
    mSpawnPosY = mPosY;
    mSpawnPosZ = mPosZ;
    mSpawnedActorID = r3;
    mSpawnedDeathTableID = -1;
    mHitActor = r3;

    if (data_0209f2d8 == 1)
        r3 = 1;
    if (r3 == 0 && SublevelToLevel((s8)data_0209f2f8) == 0x1d && IsStarCollectedInCurLevel(mStarID) != 0) {
        _ZN7fBase_c18MarkForDestructionEv(((char *)this));
        _ZN9dBgCh_GndD1Ev(&rg);
        return 0;
    }
    if (_ZN8dActor_c18GetBitInDeathTableEv(((char *)this)) != 0) {
        _ZN7fBase_c18MarkForDestructionEv(((char *)this));
        _ZN9dBgCh_GndD1Ev(&rg);
        return 0;
    }
    _ZN9dBgCh_GndD1Ev(&rg);
    return 1;
}
