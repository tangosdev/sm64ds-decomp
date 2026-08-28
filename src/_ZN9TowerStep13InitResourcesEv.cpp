//cpp
// @symbol _ZN9TowerStep13InitResourcesEv
#include "TowerStep.h"
#include "SharedFilePtr.h"
#include "dBgCh_Gnd.h"

extern SharedFilePtr data_ov015_02114a64;
extern SharedFilePtr data_ov015_02114a5c;
extern CLPS_Block data_ov015_02113594;
extern s16 data_02082214[];
extern s8 data_0209f2f8;
extern u8 data_0209f220;

extern "C" {
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *file, const Matrix4x3 *mat,
    Fix12i scale, s16 angle, CLPS_Block *clps);
void func_020393d4(dBgW *collider, void *callback);
void func_020393c4(dBgW *collider, void *callback);
void _ZN4dBgW21UpdatePosWithVelocityERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_();
void func_ov015_021128f8();
void func_ov015_021123c8(TowerStep *step);
int IsStarCollectedInCurLevel(int starID);
}

int TowerStep::InitResources()
{
    mModel.SetFile((BMD_File *)Model::LoadFile(data_ov015_02114a64), 1, -1);
    mShadowModel.InitCuboid();
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider,
        (KCL_File *)dBgW_Kc::LoadFile(data_ov015_02114a5c),
        &mClsnMat, 0x199, mAngleY, &data_ov015_02113594);
    func_020393d4(&mMeshCollider,
        (void *)&_ZN4dBgW21UpdatePosWithVelocityERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
    func_020393c4(&mMeshCollider, (void *)&func_ov015_021128f8);

    Vector3 probePos;
    probePos.x = mPosX;
    probePos.y = mPosY;
    probePos.z = mPosZ;
    probePos.y -= 0x14000;

    dBgCh_Gnd ground;
    ground.SetObjAndPos(probePos, 0);
    mFloorPosY = probePos.y;
    if (ground.DetectClsn())
        mFloorPosY = ground.clsnY;

    u16 angle = mAngleY;
    int sine = data_02082214[(angle >> 4) * 2];
    mShadowOffsetX = (int)(((s64)sine * 0xc8000 + 0x800) >> 12);
    angle = mAngleY;
    int cosine = data_02082214[(angle >> 4) * 2 + 1];
    mShadowOffsetY = (int)(((s64)cosine * 0xc8000 + 0x800) >> 12);

    func_ov015_021123c8(this);

    mPrevAngleY = mAngleY + 0x8000;
    mMoveTimer = 0x87;

    int kind = param1 & 0xff;
    if (kind != 2) {
        mHorzSpeed = 0x3000;
    } else {
        mMinPosY = mPosY;
        mMaxPosY = mMinPosY + 0x1f4000;
        mTerminalVelocity = -0x3c000;
        mVertSpeed = 0xa000;
    }

    if (data_0209f2f8 == 7
        && (data_0209f220 == 1 || IsStarCollectedInCurLevel(1) == 0)
        && mPosY >= 0xdac000)
        return 0;

    return 1;
}
