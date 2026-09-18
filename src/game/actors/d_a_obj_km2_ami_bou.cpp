//cpp
/* Bowser in the Fire Sea net pole (KM2_AMI_BOU).
 * A configured pole starts 300 units lower and bobs upward; param1 == 0xffff
 * keeps the placed height and reverses the motion. Both advance one shared
 * sine phase per frame.
 */

#include "daObjKm2_Ami_Bou_c.h"
#include "SharedFilePtr.h"

struct KCL_File;
struct CLPS_Block;

extern int _ZTV18daObjKm2_Ami_Bou_c[];

extern "C" {
extern SharedFilePtr data_ov045_021131a8; /* the KCL */
extern SharedFilePtr data_ov045_021131b0; /* the BMD */
extern CLPS_Block data_ov045_02112510;
extern short data_02082214[];

/* Scalar interfaces agree with their definitions. Direct Fix12 member-call
 * forms still differ in the measured probes recorded in the repair handoff. */
int _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(
    dBgActor_c *self, int a, int b);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *kcl, const Matrix4x3 *mtx, int scale,
    s16 angleY, CLPS_Block *clps);
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
    dCcAc_c *self, dActor_c *act, int radius, int height,
    unsigned flags, unsigned vulnFlags);
}

// @symbol daObjKm2_Ami_Bou_c_classInit
extern "C" daObjKm2_Ami_Bou_c *daObjKm2_Ami_Bou_c_classInit()
{
    return new daObjKm2_Ami_Bou_c();
}

// @symbol _ZN18daObjKm2_Ami_Bou_c13InitResourcesEv
s32 daObjKm2_Ami_Bou_c::InitResources()
{
    mModel.SetFile((BMD_File *)Model::LoadFile(data_ov045_021131b0), 1, -1);
    if (param1 != 0xffff)
        mPosY -= 0x12c000;
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    KCL_File *kcl = (KCL_File *)dBgW_Kc::LoadFile(data_ov045_021131a8);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, kcl, &mClsnMat, 0x199, mAngleY,
        &data_ov045_02112510);
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
        &mCylinder, this, 0x35555, 0x258000, 0x280000c, 0);
    return 1;
}

// @symbol _ZN18daObjKm2_Ami_Bou_c8BehaviorEv
s32 daObjKm2_Ami_Bou_c::Behavior()
{
    mCylinder.Clear();
    mCylinder.Update();
    if (param1 != 0xffff) {
        int sinIndex = (u16)mHeightAng >> 4;
        int sine = data_02082214[sinIndex * 2];
        mPosY = mPosY + (int)(((long long)sine * 0x7000 + 0x800) >> 12);
    } else {
        int sinIndex = (u16)mHeightAng >> 4;
        int sine = data_02082214[sinIndex * 2];
        mPosY = mPosY - (int)(((long long)sine * 0x3000 + 0x800) >> 12);
    }
    mHeightAng += 0x100;
    UpdateModelPosAndRotY();
    if (_ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(this, 0x400000, 0))
        UpdateClsnPosAndRot();
    return 1;
}

// @symbol _ZN18daObjKm2_Ami_Bou_c6RenderEv
s32 daObjKm2_Ami_Bou_c::Render()
{
    mModel.Render(0);
    return 1;
}

// @symbol _ZN18daObjKm2_Ami_Bou_c16CleanupResourcesEv
s32 daObjKm2_Ami_Bou_c::CleanupResources()
{
    mMeshCollider.Disable();
    data_ov045_021131b0.Release();
    data_ov045_021131a8.Release();
    return 1;
}

// @symbol _ZN18daObjKm2_Ami_Bou_cD1Ev
// @symbol _ZN18daObjKm2_Ami_Bou_cD0Ev
/* The header's inline destructor emits both cartridge variants. */
