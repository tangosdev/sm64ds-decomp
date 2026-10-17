//cpp
/* leftovers:
 * - SetFile, dCcAc_c::Init and IsClsnInRangeOnScreen stay scalar
 *   adapters. The Fix12<int> method calls grew InitResources from
 *   0xd0 to 0xf8.
 * - data_ov045_02112510 is the CLPS block, outside this TU. The KCL
 *   and BMD handles are the file-scope objects at the bottom.
 * - data_02082214 is the sine table.
 * - The phase step is *(s16 *)&mHeightAng += kPhaseStep. Adding
 *   0x100 to the u16 is a different halfword load.
 * - The destructor stays inline so D1 stays above D0.
 *
 * daObjKm2_Ami_Bou_c, the Fire Sea net pole. It bobs on the sine
 * table. param1 other than 0xffff drops the spawn Y by 300 and bobs
 * up by 7; otherwise it bobs down by 3. classInit is the KM2_AMI_BOU
 * factory. This TU owns g_profile_KM2_AMI_BOU.
 */
/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_km2_ami_bou_AssetId {
    ASSET_KM2_AMI_BOU_BMD = 0x065d,
    ASSET_KM2_AMI_BOU_KCL = 0x065e
};

#include "daObjKm2_Ami_Bou_c.h"
#include "SpawnInfo.h"
#include "SharedFilePtr.h"
#include "nitro/fx/fx.h"

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

/* Retail constructs these 8-byte handles in this order and registers
 * their destructors. The wrapper names are local; the ctor/dtor
 * addresses are the ROM resource-family functions. */
struct AmiBouModelFilePtr : SharedFilePtr {
    u32 words[2];

    AmiBouModelFilePtr(u32 fileID);
    ~AmiBouModelFilePtr();
};

struct AmiBouCollisionFilePtr : SharedFilePtr {
    u32 words[2];

    AmiBouCollisionFilePtr(u32 fileID);
    ~AmiBouCollisionFilePtr();
};

extern "C" {
extern AmiBouCollisionFilePtr data_ov045_021131a8; /* KCL */
extern AmiBouModelFilePtr data_ov045_021131b0;     /* BMD */
extern CLPS_Block data_ov045_02112510;
extern s16 data_02082214[];
}

// @symbol daObjKm2_Ami_Bou_c_classInit
extern "C" daObjKm2_Ami_Bou_c *daObjKm2_Ami_Bou_c_classInit()
{
    return new daObjKm2_Ami_Bou_c();
}

// @symbol g_profile_KM2_AMI_BOU
extern "C" SpawnInfo<daObjKm2_Ami_Bou_c> g_profile_KM2_AMI_BOU = {
    daObjKm2_Ami_Bou_c_classInit,
    0x008e, /* KM2_AMI_BOU registry id 142 */
    0x00d5,
    2,
    0x00100000,
    0x00300000,
    0x01000000,
    0
};

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
        mPosY += FX_MUL(s, kRiseAmp);
    } else {
        int s = data_02082214[(mHeightAng >> 4) << 1];
        mPosY -= FX_MUL(s, kFallAmp);
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

AmiBouModelFilePtr data_ov045_021131b0(ASSET_KM2_AMI_BOU_BMD);
AmiBouCollisionFilePtr data_ov045_021131a8(ASSET_KM2_AMI_BOU_KCL);
