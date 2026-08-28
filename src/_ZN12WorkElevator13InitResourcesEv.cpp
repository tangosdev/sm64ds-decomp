//cpp
// @symbol _ZN12WorkElevator13InitResourcesEv
#include "WorkElevator.h"
#include "SharedFilePtr.h"
#include "dBgCh_Gnd.h"

struct WorkElevatorFile {
    u32 unk_00;
    void *file;
};

extern WorkElevatorFile data_ov021_021149a0;
extern WorkElevatorFile data_ov021_021149a8;
extern WorkElevatorFile data_ov021_021149b0;
extern WorkElevatorFile data_ov021_021149b8;
extern CLPS_Block data_ov021_02113a60;
extern CLPS_Block data_ov021_02113a80;

extern "C" {
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *file, const Matrix4x3 *mat,
    Fix12i scale, s16 angle, CLPS_Block *clps);
void func_020393d4(dBgW *collider, void *callback);
void func_020393c4(dBgW *collider, void *callback);
void func_ov021_02111434(WorkElevator *elevator);
void func_ov021_0211129c(WorkElevator *elevator);
void _ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_();
void func_ov021_02112128();
void func_ov021_02111fe4();
void func_ov021_02111f8c();
void func_ov021_02111f34();
void func_ov021_02111edc();
}

s32 WorkElevator::InitResources()
{
    Model::LoadFile(*(SharedFilePtr *)&data_ov021_021149a0);
    dBgW_Kc::LoadFile(*(SharedFilePtr *)&data_ov021_021149a8);
    Model::LoadFile(*(SharedFilePtr *)&data_ov021_021149b0);
    dBgW_Kc::LoadFile(*(SharedFilePtr *)&data_ov021_021149b8);

    mModel.SetFile((BMD_File *)data_ov021_021149a0.file, 1, -1);
    {
        int i = 0;
        Model *platformModel = mPlatformModels;
        for (; i < 4; i++, platformModel++)
            platformModel->SetFile((BMD_File *)data_ov021_021149b0.file, 1, -1);
    }

    mLoweredPlatform = -1;
    func_ov021_02111434(this);
    func_ov021_0211129c(this);

    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, (KCL_File *)data_ov021_021149a8.file, &mClsnMat,
        0x199, mAngleY, &data_ov021_02113a60);
    func_020393d4(&mMeshCollider,
        (void *)&_ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
    func_020393c4(&mMeshCollider, (void *)&func_ov021_02112128);
    mMeshCollider.Enable(this);

    {
        int i = 0;
        Matrix4x3 *platformMat = mPlatformMats;
        dBgW_KcMbg *platformCollider = mPlatformColliders;
        for (; i < 4; i++, platformMat++, platformCollider++) {
            _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
                platformCollider, (KCL_File *)data_ov021_021149b8.file, platformMat,
                0x199, mAngleY, &data_ov021_02113a80);
            func_020393d4(platformCollider,
                (void *)&_ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
        }
    }

    func_020393c4(&mPlatformColliders[0], (void *)&func_ov021_02111fe4);
    func_020393c4(&mPlatformColliders[1], (void *)&func_ov021_02111f8c);
    func_020393c4(&mPlatformColliders[2], (void *)&func_ov021_02111f34);
    func_020393c4(&mPlatformColliders[3], (void *)&func_ov021_02111edc);

    for (int i = 0; i < 4; i++)
        mPlatformColliders[i].Enable(this);

    mTargetPos.x = mPosX;
    mTargetPos.y = mPosY;
    mTargetPos.z = mPosZ;

    Vector3 probePos;
    probePos.x = mPosX;
    probePos.y = mPosY;
    probePos.z = mPosZ;
    probePos.y -= 0x14000;

    dBgCh_Gnd ground;
    ground.SetObjAndPos(probePos, 0);
    unk_c6c = probePos.y;
    if (ground.DetectClsn())
        unk_c6c = ground.clsnY;
    return 1;
}
