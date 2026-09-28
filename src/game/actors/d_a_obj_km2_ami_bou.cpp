//cpp
/**
 * Bowser in the Fire Sea net pole.
 *
 * Oscillates vertically from the sine table. param1 != 0xffff
 * drops spawn Y by 300 and bobs up by 7; otherwise bobs down by 3.
 *
 * daObjKm2_Ami_Bou_c_classInit is the KM2_AMI_BOU factory. Retail
 * does not store that spelling.
 *
 * Leftover: SetMeshFile / InitClsn / ClsnInRangeOnScreen are scalar
 *   adapters. dBgW_KcMbg::SetFile, dCcAc_c::Init, and
 *   dBgActor_c::IsClsnInRangeOnScreen take Fix12<int> by value.
 *   Calling those methods with Fix12 locals grew InitResources from
 *   0xd0 to 0xf8 in this TU.
 * Leftover: data_ov045_021131a8 / 021131b0 are this overlay's KCL
 *   and BMD (Init LoadFile, Cleanup Release). data_ov045_02112510
 *   is the CLPS block handed to SetFile.
 * Leftover: data_02082214 is the sine table. Its name belongs with
 *   that table, not this actor.
 * Leftover: the phase step is *(s16 *)&mHeightAng += kPhaseStep.
 *   mHeightAng += 0x100 on the u16 is ldrh; Behavior's add is ldrsh
 *   (1 word, this TU). mHeightAng = mHeightAng + 0x100 still differs
 *   by 4 words.
 * Leftover: g_profile_KM2_AMI_BOU is defined outside this file.
 * Leftover: the destructor stays inline so D1 stays above D0.
 */

#include "daObjKm2_Ami_Bou_c.h"
#include "SharedFilePtr.h"

enum {
    kUnsetParam = 0xffff,
    kLoweredY = 0x12c000,       /* 300.0 */
    kRiseAmp = 0x7000,          /* 7.0 */
    kFallAmp = 0x3000,          /* 3.0 */
    kPhaseStep = 0x100,
    kMeshScale = 0x199,
    kCylinderRadius = 0x35555,
    kCylinderHeight = 0x258000, /* 600.0 */
    kClsnFlags = 0x0280000c,
    kOnScreenRange = 0x400000   /* 1024.0 */
};

extern "C" {
extern SharedFilePtr data_ov045_021131a8; /* KCL */
extern SharedFilePtr data_ov045_021131b0; /* BMD */
extern CLPS_Block data_ov045_02112510;
extern s16 data_02082214[];
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
    if (param1 != kUnsetParam)
        mPosY -= kLoweredY;
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    KCL_File *kcl = (KCL_File *)dBgW_Kc::LoadFile(data_ov045_021131a8);
    SetMeshFile(kcl, kMeshScale, &data_ov045_02112510);
    InitClsn(kCylinderRadius, kCylinderHeight, kClsnFlags, 0);
    return 1;
}

// @symbol _ZN18daObjKm2_Ami_Bou_c8BehaviorEv
s32 daObjKm2_Ami_Bou_c::Behavior()
{
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    if (param1 != kUnsetParam) {
        int s = data_02082214[(mHeightAng >> 4) << 1];
        mPosY += (int)(((long long)s * kRiseAmp + 0x800) >> 12);
    } else {
        int s = data_02082214[(mHeightAng >> 4) << 1];
        mPosY -= (int)(((long long)s * kFallAmp + 0x800) >> 12);
    }
    *(s16 *)&mHeightAng += kPhaseStep;
    UpdateModelPosAndRotY();
    if (ClsnInRangeOnScreen(kOnScreenRange, 0))
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
/* The inline destructor in the header emits D1 then D0, and no D2. */
