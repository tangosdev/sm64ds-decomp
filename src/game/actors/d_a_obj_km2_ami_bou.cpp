//cpp
/**
 * Bowser in the Fire Sea net pole.
 *
 * Oscillates vertically from the sine table. param1 != 0xffff
 * drops spawn Y by 0x12c000 and bobs up; otherwise bobs down.
 *
 * daObjKm2_Ami_Bou_c_classInit is reconstructed (RTTI
 * daObjKm2_Ami_Bou_c, KM2_AMI_BOU registry). Retail does not store
 * that spelling. Historical alias: PoleLift_Spawn. Coined C name;
 * no param-type claim.
 *
 * deslop
 * Leftover: dBgW_KcMbg::SetFile and dCcAc_c::Init stay mangled in
 *   this TU -- both take Fix12<int> by value (wall 6az); a method
 *   call homes the argument and size-DIFFs InitResources.
 * Leftover: dBgActor_c::IsClsnInRangeOnScreen stays mangled --
 *   Fix12<int> by value (wall 6az) and not declared on
 *   dBgActor_c.h. This TU's Behavior call.
 * Leftover: data_ov045_021131a8 / 021131b0 are this overlay's
 *   KCL/BMD handles (Init LoadFile / Cleanup Release).
 *   data_ov045_02112510 is the CLPS block SetFile is handed, in
 *   overlay .data this TU does not own.
 * Leftover: data_02082214 is the NitroSDK FX_SinCosTable_; Behavior
 *   indexes it by mHeightAng. Naming belongs with the SDK table.
 * Leftover: Behavior's mHeightAng increment stays
 *   `*(short *)((int)(char *)this + 0x354)` -- the signed short
 *   rematerialized store. `mHeightAng = mHeightAng + 0x100` on the
 *   u16 field DIFFs 4 words (this TU, measured).
 * Leftover: g_profile_KM2_AMI_BOU lives outside this TU (S14).
 * Leftover: inline destructor (out-of-line emits D0 before D1).
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
        &mdCcAc_c, this, 0x35555, 0x258000, 0x280000c, 0);
    return 1;
}

// @symbol _ZN18daObjKm2_Ami_Bou_c8BehaviorEv
s32 daObjKm2_Ami_Bou_c::Behavior()
{
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    if (param1 != 0xffff) {
        int idx = mHeightAng >> 4;
        int s = *(short *)((char *)data_02082214 + (idx << 2));
        mPosY = mPosY + (int)(((long long)s * 0x7000 + 0x800) >> 12);
    } else {
        int idx = mHeightAng >> 4;
        int s = *(short *)((char *)data_02082214 + (idx << 2));
        mPosY = mPosY - (int)(((long long)s * 0x3000 + 0x800) >> 12);
    }
    *(short *)(((int)((char *)this) + 0x354)) =
        *(short *)(((int)((char *)this) + 0x354)) + 0x100;
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
/* NOT WRITTEN HERE ON PURPOSE. The inline destructor in the header
   emits D1 then D0 -- the cartridge's order -- and no D2. */
