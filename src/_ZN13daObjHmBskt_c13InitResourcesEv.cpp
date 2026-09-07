//cpp
// @symbol _ZN13daObjHmBskt_c13InitResourcesEv
#include "daObjHmBskt_c.h"
#include "SharedFilePtr.h"

extern "C" {
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *file, const Matrix4x3 *mat,
    Fix12i scale, s16 angle, CLPS_Block *clps);
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
    dBgCh_Actr *self, dActor_c *actor, Fix12i radius, Fix12i height,
    Vector3_16 *a, Vector3_16 *b);

extern SharedFilePtr data_ov030_02115c88;
extern SharedFilePtr data_ov030_02115c80;
extern CLPS_Block data_ov030_02114ee4;
}

s32 daObjHmBskt_c::InitResources()
{
    BMD_File *modelFile = (BMD_File *)Model::LoadFile(data_ov030_02115c88);
    mModel.SetFile(modelFile, 1, -1);

    mVertAccel = 0;
    mTerminalVelocity = 0;
    mHorzSpeed = 0;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;

    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
        &mWithMeshClsn, this, 0x32000, 0x32000, 0, 0);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    KCL_File *clsnFile = (KCL_File *)dBgW_Kc::LoadFile(data_ov030_02115c80);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, clsnFile, &mClsnMat, 0x199, mAngleY,
        &data_ov030_02114ee4);

    Vector3 starPos;
    starPos.x = mPosX;
    starPos.y = mPosY;
    starPos.z = mPosZ;
    starPos.y += 0x3c000;
    mStarActor = dActor_c::Spawn(
        0xb2, (u8)(param1 & 0xf) | 0x50, starPos, 0, mAreaId, -1);

    return 1;
}
