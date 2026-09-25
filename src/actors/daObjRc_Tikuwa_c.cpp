//cpp
/* Production translation unit for ov036/daObjRc_Tikuwa_c.
 * 7 function(s), .text 0x0211193c..0x02111ca4. The RC_TIKUWA donut block:
 * once something has stood on it for fifteen frames it falls, poofs when it
 * lands or drops far enough below the player, and respawns at its home
 * position once the player has moved away.
 *
 * NAME: _ZTS16daObjRc_Tikuwa_c is "16daObjRc_Tikuwa_c" at ov036 0x02113cc0;
 * _ZTI at 0x02113cb4 reads [__si_class_type_info, that string,
 * _ZTI10dBgActor_c]. The tree previously called the class DonutBlock
 * (coined; vtable address only).
 *
 * The out-of-line destructor is the key function, so this TU emits _ZTV/_ZTI/
 * _ZTS. Under `#pragma defer_codegen off` it comes out D1 (0x0211193c), D0
 * (0x02111988), then a D2 the cartridge has no home for (manifest: deadstrip);
 * the same pragma lays .text down in source order, so this file is ROM-ascending.
 *
 * Leftover: dBgW_KcMbg::SetFile, dBgCh_Actr::Init and
 *   dBgActor_c::IsClsnInRange take Fix12<int> by value, so they stay mangled;
 *   a member call homes the argument and changes the ROM ABI.
 * Leftover: func_020393d4 / func_020393c4 are 4-byte stores into dBgW's
 *   callback slots; naming belongs with dBgW in arm9.
 * Leftover: func_ov036_02111ca4 (sets mHadClsn), func_ov036_02111cc4 (the
 *   callback InitResources installs) and the factory
 *   daObjRc_Tikuwa_c_classInit (0x02111cd8) sit past this run's right edge
 *   and stay one-function sources.
 */

#pragma defer_codegen off

#include "decl_common.h"
#include "daObjRc_Tikuwa_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

extern "C" {
void dBgCh_Actr_UpdateContinuous_Veneer(dBgCh_Actr *clsn);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(dBgActor_c *self, int a, int b);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, void *kcl, Matrix4x3 *mat, int scale, s16 angY, void *clps);
void func_020393d4(dBgW *bgw, void *fn);
void func_020393c4(dBgW *bgw, void *fn);
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
    dBgCh_Actr *clsn, dActor_c *actor, int radius, int height, void *a, void *b);
}

// @symbol _ZN16daObjRc_Tikuwa_cD1Ev
// @symbol _ZN16daObjRc_Tikuwa_cD0Ev
daObjRc_Tikuwa_c::~daObjRc_Tikuwa_c()
{
}

// @symbol _ZN16daObjRc_Tikuwa_c16CleanupResourcesEv
s32 daObjRc_Tikuwa_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    ((SharedFilePtr *)data_ov036_0211408c)->Release();
    ((SharedFilePtr *)data_ov036_02114084)->Release();
    return 1;
}

// @symbol _ZN16daObjRc_Tikuwa_c16OnPendingDestroyEv
void daObjRc_Tikuwa_c::OnPendingDestroy()
{
}

// @symbol _ZN16daObjRc_Tikuwa_c6RenderEv
s32 daObjRc_Tikuwa_c::Render()
{
    if (mState != 2)
        mModel.Render(0);
    return 1;
}

// @symbol _ZN16daObjRc_Tikuwa_c8BehaviorEv
s32 daObjRc_Tikuwa_c::Behavior()
{
    switch (mState) {
    case 0:
        if (mHadClsn == 0)
            mClsnTimer = 0;
        else {
            mClsnTimer += 1;
            mHadClsn = 0;
        }
        if (mClsnTimer >= 0xf)
            mState = 1;
        break;
    case 1:
        UpdatePos(0);
        dBgCh_Actr_UpdateContinuous_Veneer(&mWithMeshClsn);
        if (mWithMeshClsn.IsOnGround() == 0) {
            if (DistToCPlayer() <= 0x9c4000)
                break;
        }
        TriplePoofDust();
        if (mMeshCollider.IsEnabled() != 0)
            mMeshCollider.Disable();
        mPosX = mHomePosX;
        mPosY = mHomePosY;
        mPosZ = mHomePosZ;
        mState = 2;
        break;
    case 2: {
        s32 dist = DistToCPlayer();
        if (dist <= 0x3e8000)
            break;
        if (dist < 0x7d0000) {
            mVertSpeed = 0;
            mClsnTimer = 0;
            mHadClsn = 0;
            mState = 0;
        }
        break;
    }
    }
    UpdateModelPosAndRotY();
    if (mState != 2) {
        if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0x5dc000, 0) != 0)
            UpdateClsnPosAndRot();
    }
    return 1;
}

// @symbol _ZN16daObjRc_Tikuwa_c13InitResourcesEv
s32 daObjRc_Tikuwa_c::InitResources()
{
    mModel.SetFile((BMD_File *)Model::LoadFile(*(SharedFilePtr *)data_ov036_0211408c), 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    void *kcl = dBgW_Kc::LoadFile(*(SharedFilePtr *)data_ov036_02114084);
    /* Leftover: dBgW_KcMbg::SetFile and dBgCh_Actr::Init take Fix12<int> by
     * value, so they stay mangled (the header spellings take Fix12i and would
     * mangle symbols the ROM does not have). */
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, kcl, &mClsnMat, 0x1000, mAngleY, data_ov036_02112b48);
    func_020393d4(&mMeshCollider, (void *)&dBgW::UpdatePosWithVelocity);
    func_020393c4(&mMeshCollider, (void *)func_ov036_02111cc4);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
        &mWithMeshClsn, this, 0x32000, 0x64000, 0, 0);
    mTerminalVelocity = -0x1e000;
    mVertAccel = ~0x198;
    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    return 1;
}
