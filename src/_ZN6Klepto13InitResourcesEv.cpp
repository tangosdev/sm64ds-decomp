//cpp
// @symbol _ZN6Klepto13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_SaveData.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Klepto.h"
/* SharedFilePtr stays incomplete: Model.h forward-declares it and its layout is
   deliberately not recovered (include/SharedFilePtr.h). Used only by address here. */

extern "C" {
extern void *_ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr *f);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *f, int a, int b);
extern int _ZN11ShadowModel12InitCylinderEv(void *self);
extern void *_ZN9Animation8LoadFileER13SharedFilePtr(SharedFilePtr *f);
extern void _ZN7PathPtrC1Ev(void *self);
extern void _ZN7PathPtr6FromIDEj(void *self, unsigned int id);
extern void _ZNK7PathPtr7GetNodeER7Vector3j(void *self, void *v, unsigned int idx);
extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void *self, void *a, Fix12i r, Fix12i h, unsigned int d, unsigned int e);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void *self, void *a, Fix12i b, Fix12i c, void *d, void *e);
extern void *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int a, unsigned int b, void *pos, void *rot, int e, int f);
extern void _ZN5Actor9SetRangesE5Fix12IiES1_S1_S1_(void *self, Fix12i a, Fix12i b, Fix12i c, Fix12i d);
extern void func_ov062_0211c658(void *c, void *p);
extern short Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
}

extern SharedFilePtr data_ov062_0211e0fc;
extern SharedFilePtr data_ov062_0211e114;
extern SharedFilePtr data_ov062_0211e10c;
extern SharedFilePtr data_ov062_0211e104;
extern SharedFilePtr data_ov002_0210da40;
extern SharedFilePtr data_ov002_0210d9a0;
extern SharedFilePtr data_ov002_0210d9c0;
extern char data_ov062_0211e17c;
extern void *data_0209f394;

int Klepto::InitResources()
{
    void *bmd;
    void *spawned;
    void *pl;
    char path1[8];
    char path2[8];
    unsigned char hat;
    int area;
    int param;
    int zero;
    

    bmd = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov062_0211e0fc);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char *)this) + 0x334, bmd, 1, -1);
    _ZN11ShadowModel12InitCylinderEv((char *)&mShadowModel);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov062_0211e114);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov062_0211e10c);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov062_0211e104);
    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210da40);
    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9a0);
    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9c0);

    mPathId = mParam & 0xff;
    mCarriedItem = (mParam >> 8) & 0xf;
    unk_46c = (mParam >> 0xc) & 0xf;
    if (mPathId < 0)
        mPathId = 0;
    if (mCarriedItem == 0xff)
        mCarriedItem = 0;
    if (mCarriedItem == 2) {
        unk_448 = 2;
        mCarriedItem = 1;
    }

    _ZN7PathPtrC1Ev(path1);
    _ZN7PathPtr6FromIDEj(path1, mPathId);
    unk_470 = 4;
    unk_0a0 = -0x1e000;
    unk_484 = mPosX;
    unk_488 = mPosY;
    unk_48c = mPosZ;
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(((char *)this) + 0x110, ((char *)this), 0x64000, 0xa0000, 0x200002, 0x3eff0);
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(((char *)this) + 0x144, ((char *)this), 0x3c000, 0xa0000, 0x200000, 0);
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(((char *)this) + 0x178, ((char *)this), 0x1e000, 0x1e000, 0, 0);

    _ZN7PathPtrC1Ev(path2);
    _ZN7PathPtr6FromIDEj(path2, mPathId);
    _ZNK7PathPtr7GetNodeER7Vector3j(path2, ((char *)this) + 0x430, unk_474);
    mBlendModelAnim.speed = 0x1000;
    mHeldActorID = 0;

    if (mCarriedItem == 1) {
        if (unk_448 != 2) {
            spawned = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(
                0xb2, unk_46c | 0x50, ((char *)this) + 0x5c, 0, mAreaId, -1);
        } else {
            spawned = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(
                0xb3, 0x50, ((char *)this) + 0x5c, 0, mAreaId, -1);
        }
        if (spawned != 0) {
            mHeldActorID = *(int *)((char *)spawned + 4);
            _ZN5Actor9SetRangesE5Fix12IiES1_S1_S1_(spawned, 0x64000, 0x258000, 0x1f40000, 0x1f40000);
        }
        mPosX = unk_430;
        mPosY = unk_434;
        mPosZ = unk_438;
        func_ov062_0211c658(((char *)this), &data_ov062_0211e15c);
    } else {
        pl = data_0209f394;
        if (pl != 0 && *(int *)((char *)pl + 8) != 3 && _ZN8SaveData16HasPlayerLostCapEv() != 0) {
            {
                unsigned int hat = *(unsigned char *)((char *)pl + 0x6d9);
                int area = mAreaId;
                unsigned int param = 0;
                param = param | (hat << 8);
                spawned = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(
                    0x10d, param, ((char *)this) + 0x5c, 0, area, -1);
            }
            if (spawned != 0) {
                _ZN5Actor9SetRangesE5Fix12IiES1_S1_S1_(spawned, 0x64000, 0x258000, 0x1f40000, 0x1f40000);
                mHeldActorID = *(int *)((char *)spawned + 4);
            }
        }
        unk_44a = Vec3_HorzAngle((Vector3 *)((char *)&mPosX), (Vector3 *)((char *)&unk_484));
        func_ov062_0211c658(((char *)this), &data_ov062_0211e17c);
    }
    return 1;
}
