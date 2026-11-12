//cpp
/**
 * Tiny-Huge Island rolling log -- ov052/daObjEmmLog_c (7 functions).
 *
 * ROM run 0x021111a0..0x02111440. Bobs mPosY on a sine of mBobPhase;
 * nothing writes mAngle*.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function in the reverse of source order.
 *
 * daObjEmmLog_c_classInit is reconstructed (RTTI daObjEmmLog_c, EMM_LOG
 * registry). Retail does not store that spelling.
 *
 * What stays, and why:
 * - dBgW::SetClipRadius is an 8-byte store into dBgW+0x0c. This TU calls it;
 *   naming belongs with dBgW in arm9.
 * - data_ov052_021124d4 is this overlay's BMD/KCL/CLPS bundle;
 *   symbols.txt has no recovered name.
 * - The bob reads the arm9 sine table, FX_SinCosTable_, through FX_SinIdx.
 * - Inline destructor (out-of-line emits D0 before D1 plus a homeless D2).
 * - g_profile_EMM_LOG lives outside this TU (S14).
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_emm_log_AssetId { ASSET_EMM_LOG_BMD = 0x05e3, ASSET_EMM_LOG_KCL = 0x05e4 };

#include "daObjEmmLog_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"

struct daObjEmmLog_c_Resources {
    SharedFilePtr *model;
    SharedFilePtr *collision;
    CLPS_Block *clps;
};

/* File-scope resource handles: the ctor/dtor are the ROM's SharedFilePtr
 * veneer pairs (func_02017acc / func_02017ab4 for the model, func_02017b4c /
 * SharedFilePtr_Destruct_Clsn for the collision), spelled through
 * declared-only subclasses so the static initializer names the real entry
 * points. */
struct EmmLogModelFilePtr : SharedFilePtr {
    EmmLogModelFilePtr(u32 fileID);
    ~EmmLogModelFilePtr();
};
struct EmmLogCollisionFilePtr : SharedFilePtr {
    EmmLogCollisionFilePtr(u32 fileID);
    ~EmmLogCollisionFilePtr();
};

extern daObjEmmLog_c_Resources data_ov052_021124d4;

extern "C" daObjEmmLog_c *daObjEmmLog_c_classInit() {
    return new daObjEmmLog_c();
}

s32 daObjEmmLog_c::InitResources() {
    mModel.SetFile(dExtSimpleModel_c::LoadFile(*data_ov052_021124d4.model), 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    mMeshCollider.SetFile(dBgW_Kc::LoadFile(*data_ov052_021124d4.collision), mClsnMat, 0x1000,
                          mAngleY, *data_ov052_021124d4.clps);

    mBasePosY = mPosY;
    u8 amplitude = param1 & 0xff;
    if (amplitude == 0xff || param1 == 0)
        mBobAmplitude = 0x64000;
    else
        mBobAmplitude = amplitude * 0xa000;

    mBobPhase = mAngleX;
    return 1;
}

s32 daObjEmmLog_c::Behavior() {
    mMeshCollider.SetClipRadius(0x600000);

    int s = FX_SinIdx((u16)mBobPhase >> 4);
    int m = FX_MUL(mBobAmplitude, s);
    mPosY = mBasePosY + m;

    mBobPhase += 0x200;

    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    IsClsnInRange(0x600000, 0);
    return 1;
}

s32 daObjEmmLog_c::Render() {
    mModel.Render(0);
    return 1;
}

s32 daObjEmmLog_c::CleanupResources() {
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    data_ov052_021124d4.model->Release();
    data_ov052_021124d4.collision->Release();
    return 1;
}

/* Both destructors are emitted from the INLINE `~daObjEmmLog_c() {}` in
 * include/daObjEmmLog_c.h -- there is deliberately no body here. */

/* The log's model and collision file handles; data_ov052_021124d4.model
 * and .collision point at them. */
EmmLogModelFilePtr gEmmLogModelFile(ASSET_EMM_LOG_BMD);
EmmLogCollisionFilePtr gEmmLogCollisionFile(ASSET_EMM_LOG_KCL);
