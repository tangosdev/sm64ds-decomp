//cpp
#include "common.h"
#include "types.h"
// @symbol _ZN8BookShot13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "BookShot.h"
struct dActor_c; struct Vector3; struct Vector3_16; struct BMD_File;
/* SharedFilePtr stays incomplete: Model.h forward-declares it and its layout is
   deliberately not recovered (include/SharedFilePtr.h). Used only by address here. */

extern "C" {
extern void LoadBlueCoinModel(void* c);
/* The production mesh initializer still takes actor/rotation addresses as int.
   Its native header spells a different symbol; retain that scalar contract. */
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
    dBgCh_Actr* self, int actor, int radius, int height,
    int first, int second);
/* Native cylinder call experiments are recorded in
   notes/experiments/pr2869-source-repair-0920.json. */
extern void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
    dCcAcPos_c* self, dActor_c* actor, const Vector3* offset,
    int radius, int height, u32 flags, u32 vulnFlags);
}

extern SharedFilePtr data_ov020_02114aa0;
extern SharedFilePtr data_ov020_02114ab8;
extern SharedFilePtr data_ov020_02114aa8;
extern SharedFilePtr data_ov020_02114ab0;

#define LDR(p) (p)

extern struct Matrix4x3 IDENTITY_MATRIX4X3;

int BookShot::InitResources()
{
    Model::LoadFile(data_ov020_02114aa0);
    Model::LoadFile(data_ov020_02114ab8);
    Animation::LoadFile(data_ov020_02114aa8);
    Animation::LoadFile(data_ov020_02114ab0);
    LoadBlueCoinModel(this);

    if (mShadowModel.InitCylinder() == 0)
        return 0;

    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, (int)this, 0x32000, 0x32000, 0, 0);

    unk_438 = 0;
    unk_43c = 0;
    unk_440 = 0;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(&mdCcAcPos_c, this, (const Vector3*)&unk_438, 0x19000, 0x32000, 0x200001, 0);

    unk_418 = 0;
    unk_41c = 0;
    mScaleX = 0x800;
    mScaleY = 0x800;
    mScaleZ = 0x800;
    unk_42c = mPosX;
    unk_430 = mPosY;
    unk_434 = mPosZ;

    if (mModel.SetFile((BMD_File*)((int*)&data_ov020_02114ab8)[1], 1, -1) == 0)
        return 0;

    *(struct Matrix4x3*)((char*)&unk_1ec) = IDENTITY_MATRIX4X3;
    unk_450 = 0;
    *(short*)(int)LDR((char*)&mAngleY) = *(short*)(int)LDR((char*)&mAngleY) + 0x8000;
    unk_44c = 0x800;

    {
        unsigned short id = actorID;
        unsigned int match1 = (id == 0x145);

        if (match1)
        {
            unk_420 = 0;
            mState = 4;
            unk_108 = 0;
            *(int*)(int)LDR((char*)&mdCcAcPos_c.vulnFlags) |= 0x26fe0;
            goto success;
        }
        {
            unsigned int match2 = (id == 0x147);
            if (match2)
            {
                unk_420 = 0;
                mState = 0;
                unk_108 = 3;
                *(int*)(int)LDR((char*)&mdCcAcPos_c.vulnFlags) |= 0x2efe0;
                goto success;
            }
        }
        {
            unsigned int match3 = (id == 0xd5);
            if (match3)
            {
                unk_420 = 1;
                mState = 6;
                unk_108 = 0;
                *(int*)(int)LDR((char*)&mdCcAcPos_c.flags) |= 4;
                *(int*)(int)LDR((char*)&mdCcAcPos_c.vulnFlags) |= 0x3c0;
                mScaleX = 0x1000;
                mScaleY = 0x800;
                mScaleZ = 0x800;
                goto success;
            }
        }
    }
    return 0;

success:
    return 1;
}
