//cpp
/* Tilting bridge in Lethal Lava Land. */

#include "daObjFl_London_c.h"
#include "decl_common.h"
#include "SharedFilePtr.h"
#include "Sound.h"

extern "C" {
unsigned char DecIfAbove0_Byte(unsigned char *value);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(dBgActor_c *self, int radius, int offsetY);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, void *file, const Matrix4x3 *matrix, int scale, short angleY, void *clps);
void func_020393d4(int *collider, int callback);
extern void *data_ov022_02114580;
extern void *data_ov022_02114578;
extern void *data_ov064_0211bb2c;
}

// @symbol daObjFl_London_c_classInit
extern "C" daObjFl_London_c *daObjFl_London_c_classInit()
{
    return new daObjFl_London_c();
}

struct LondonSpawnInfo {
    daObjFl_London_c *(*classInit)();
    u16 behaviorPriority;
    u16 renderPriority;
    u32 actorFlags;
    s32 clipOffsetY;
    s32 clipRadius;
    s32 clipDistance;
    s32 farDistance;
};
typedef char LondonSpawnInfo_size_must_be_0x1c[
    sizeof(LondonSpawnInfo) == 0x1c ? 1 : -1];

// @symbol g_profile_FL_LONDON
extern "C" LondonSpawnInfo g_profile_FL_LONDON = {
    daObjFl_London_c_classInit, 0x0049, 0x0119, 0x00000002,
    0x00100000, 0x00300000, 0x02000000, 0x00000000
};

// @symbol _ZN16daObjFl_London_c13InitResourcesEv
s32 daObjFl_London_c::InitResources()
{
    void *modelFile = Model::LoadFile(
        *reinterpret_cast<SharedFilePtr *>(&data_ov022_02114580));
    mModel.SetFile(static_cast<BMD_File *>(modelFile), 1, -1);
    func_ov022_02111a1c(reinterpret_cast<char *>(this));
    UpdateClsnPosAndRot();

    void *clsnFile = dBgW_Kc::LoadFile(
        *reinterpret_cast<SharedFilePtr *>(&data_ov022_02114578));
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, clsnFile, &mClsnMat, 0x1000, mAngleY, &data_ov064_0211bb2c);
    func_020393d4(reinterpret_cast<int *>(&mMeshCollider),
        reinterpret_cast<int>(&dBgW::UpdatePosWithTransform));
    mFlag = 0;
    mCooldown = 0xf;
    return 1;
}

// @symbol _ZN16daObjFl_London_c8BehaviorEv
s32 daObjFl_London_c::Behavior()
{
    if (DecIfAbove0_Byte(&mCooldown) == 0) {
        if (mFlag == 0) {
            mPrevAngleZ -= 0x100;
            if (mPrevAngleZ <= -0x2000) {
                mPrevAngleZ = -0x2000;
                mCooldown = 0xf;
                mFlag = 1;
            }
        } else {
            mPrevAngleZ += 0x100;
            if (mPrevAngleZ >= 0) {
                mPrevAngleZ = 0;
                mCooldown = 0xf;
                mFlag = 0;
            }
        }
    } else {
        if (mCooldown == 1) {
            if (mFlag == 0)
                Sound::PlayBank3(0x34, *reinterpret_cast<Vector3 *>(&mCamSpacePosX));
            else
                Sound::PlayBank3(0x35, *reinterpret_cast<Vector3 *>(&mCamSpacePosX));
        }
    }
    mAngleZ = mPrevAngleZ;
    func_ov022_02111a1c(reinterpret_cast<char *>(this));
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0))
        UpdateClsnPosAndRot();
    return 1;
}

// @symbol _ZN16daObjFl_London_c6RenderEv
s32 daObjFl_London_c::Render()
{
    mModel.Render(0);
    return 1;
}

// @symbol _ZN16daObjFl_London_c16CleanupResourcesEv
s32 daObjFl_London_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    reinterpret_cast<SharedFilePtr *>(&data_ov022_02114580)->Release();
    reinterpret_cast<SharedFilePtr *>(&data_ov022_02114578)->Release();
    return 1;
}

// @symbol func_ov022_02111a1c
extern "C" void func_ov022_02111a1c(char *actor)
{
    daObjFl_London_c *self = reinterpret_cast<daObjFl_London_c *>(actor);
    Matrix4x3_FromRotationZXYExt(&self->mModel.mat4x3,
        self->mAngleX, self->mAngleY, self->mAngleZ);
    self->mModel.mat4x3.m[9] = self->mPosX >> 3;
    self->mModel.mat4x3.m[10] = self->mPosY >> 3;
    self->mModel.mat4x3.m[11] = self->mPosZ >> 3;
}
