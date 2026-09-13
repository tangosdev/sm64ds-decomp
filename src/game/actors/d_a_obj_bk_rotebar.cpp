//cpp
/**
 * Whomp's Fortress rotating bar.
 *
 * Pauses 0x3c frames, then turns in yaw until a half turn
 * (mPrevAngleY & 0x7fff == 0) and pauses again.
 *
 * daObjBk_Rotebar_c_classInit is reconstructed (RTTI
 * daObjBk_Rotebar_c, BK_ROTEBAR registry). Retail does not store
 * that spelling.
 *
 * deslop
 * Leftover: dBgW_KcMbg::SetFile and dBgActor_c::IsClsnInRange stay
 *   mangled in this TU -- both take Fix12<int> by value (wall 6az);
 *   a method call homes the argument and size-DIFFs InitResources /
 *   Behavior.
 * Leftover: func_020393a4 / func_020393d4 are 4-byte stores into
 *   dBgW+0x0c and dBgW+0x18 (beforeClsnCallback). This TU calls
 *   them; naming belongs with dBgW in arm9.
 * Leftover: Sound::PlayLong is declared in this TU; Sound.h still
 *   only has PlayBank3.
 * Leftover: data_ov015_02114a84 / 02114a8c are this overlay's KCL/BMD
 *   handles. symbols.txt also coins TowerStep_* on the same
 *   addresses (wrong class). This TU keeps the address-true names.
 * Leftover: data_ov015_02113654 is the CLPS block in overlay .data
 *   this TU does not own.
 * Leftover: g_profile_BK_ROTEBAR lives outside this TU (S14).
 */

#include "daObjBk_Rotebar_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

struct KCL_File;
struct CLPS_Block;

namespace Sound {
unsigned PlayLong(unsigned handle, unsigned a, unsigned id,
                  const Vector3 &pos, short f);
}

extern "C" {
extern SharedFilePtr data_ov015_02114a84;   /* collision KCL */
extern SharedFilePtr data_ov015_02114a8c;   /* bar BMD */
extern CLPS_Block    data_ov015_02113654;

unsigned char DecIfAbove0_Byte(unsigned char *p);
void func_020393a4(dBgW *bgw, int range);
void func_020393d4(dBgW *bgw, void *fn);

int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(dBgActor_c *self, int a, int b);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *kcl, const Matrix4x3 *mtx, int scale,
    s16 angleY, CLPS_Block *clps);
}

// @symbol daObjBk_Rotebar_c_classInit
extern "C" daObjBk_Rotebar_c *daObjBk_Rotebar_c_classInit()
{
    return new daObjBk_Rotebar_c();
}

// @symbol _ZN17daObjBk_Rotebar_c13InitResourcesEv
s32 daObjBk_Rotebar_c::InitResources()
{
    mModel.SetFile((BMD_File *)Model::LoadFile(data_ov015_02114a8c), 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    KCL_File *kcl = (KCL_File *)dBgW_Kc::LoadFile(data_ov015_02114a84);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, kcl, &mClsnMat, 0x199, mAngleY, &data_ov015_02113654);

    func_020393d4(&mMeshCollider, (void *)&dBgW::UpdatePosAndAngs);

    mPauseTimer = 0x3c;
    return 1;
}

// @symbol _ZN17daObjBk_Rotebar_c8BehaviorEv
s32 daObjBk_Rotebar_c::Behavior()
{
    if (DecIfAbove0_Byte((unsigned char *)&mPauseTimer) == 0) {
        mPrevAngleY += 0x100;
        mAngleY = mPrevAngleY;
        mTurnSound = Sound::PlayLong(mTurnSound, 3, 0x88,
                                     *(Vector3 *)&mCamSpacePosX, 0);
        if ((mPrevAngleY & 0x7fff) == 0)
            mPauseTimer = 0x3c;
    }

    UpdateModelPosAndRotY();
    func_020393a4(&mMeshCollider, 0x320000);
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0x320000, 0))
        UpdateClsnPosAndRot();

    return 1;
}

// @symbol _ZN17daObjBk_Rotebar_c6RenderEv
s32 daObjBk_Rotebar_c::Render()
{
    mModel.Render(0);
    return 1;
}

// @symbol _ZN17daObjBk_Rotebar_c16CleanupResourcesEv
s32 daObjBk_Rotebar_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();

    data_ov015_02114a8c.Release();
    data_ov015_02114a84.Release();
    return 1;
}
