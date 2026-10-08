//cpp
/* Lethal Lava Land's floating block (registry profile FL_BLOCK). It sinks
 * 2 units a frame while the player stands on it, down to 200 below its
 * spawn height, and rises back to that height once it is left alone.
 *
 * daObjFl_Block_c is the cartridge's RTTI spelling: _ZTS at ov022
 * 0x02113e74 is "15daObjFl_Block_c", and _ZTI at 0x02113e68 names
 * dBgActor_c. The empty destructor is inline in the class header. It is
 * the key function, so this TU emits the vtable and the typeinfo records
 * and has no destructor body of its own. mwccarm emits one .text section
 * per function in reverse source order. Do not reorder these four.
 *
 * Leftover: mMeshCollider.SetFile with a Fix12<int> scale local
 *   size-DIFF InitResources 0xac->0xb8. The scalar extern stays.
 * Leftover: IsClsnInRange(Fix12<int>, Fix12<int>) size-DIFF Behavior
 *   0xa4->0xc0. The scalar extern called as (this, 0, 0) matches.
 * Leftover: mMeshCollider.unk_0c = 0x150000 size-DIFF Behavior 0xa4->0xa0.
 *   func_020393a4 is the call the ROM makes.
 * Leftover: assigning beforeClsnCallback size-DIFF InitResources
 *   0xac->0xa8. Assigning unk_1c from func_ov022_0211193c size-DIFF
 *   0xac->0xa8. func_020393d4 and func_020393c4 stay.
 * Leftover: putting the limit on the left of the compare
 *   (`mMaxPosY - 0xc8000 > mPosY`, `mMaxPosY < mPosY`) keeps Behavior
 *   at 0xa4 and changes its words. mPosY on the left matches.
 * Leftover: func_ov022_0211193c is not defined in this TU
 *   (src/unnamed/ov022/func_ov022_0211193c.c). InitResources only stores it.
 *   func_ov022_0211191c, which that callback calls, writes mHadClsn
 *   for actor 0xbf. The factory and g_profile_FL_BLOCK are outside too.
 * Leftover: data_ov022_02114558, data_ov022_02114550 and
 *   data_ov064_0211bb0c are unnamed rows in their own modules.
 */

#include "daObjFl_Block_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

extern "C" {
extern SharedFilePtr data_ov022_02114558;
extern SharedFilePtr data_ov022_02114550;
extern char data_ov064_0211bb0c[];

/* Scalar ABI. The real members take Fix12<int> by value. */
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, char *kcl, const Matrix4x3 *mtx, int scale, s16 angleY,
    char *clps);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(dBgActor_c *self, int radius, int yOffset);

/* Stores at dBgW+0x0c, +0x18 and +0x1c. Spelling matches the C definitions. */
void func_020393a4(int *mesh, int range);
void func_020393d4(int *mesh, int callback);
void func_020393c4(int *mesh, int callback);
void func_ov022_0211193c(void *self, void *other, void *info);
}

// @symbol _ZN15daObjFl_Block_c13InitResourcesEv
s32 daObjFl_Block_c::InitResources()
{
    mModel.SetFile((BMD_File *)Model::LoadFile(data_ov022_02114558), 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    char *kcl = dBgW_Kc::LoadFile(data_ov022_02114550);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, kcl, &mClsnMat, 0x199, mAngleY, data_ov064_0211bb0c);

    func_020393d4((int *)&mMeshCollider, (int)&dBgW::UpdatePosWithVelocity);
    func_020393c4((int *)&mMeshCollider, (int)&func_ov022_0211193c);

    mMaxPosY = mPosY;
    return 1;
}

// @symbol _ZN15daObjFl_Block_c8BehaviorEv
s32 daObjFl_Block_c::Behavior()
{
    func_020393a4((int *)&mMeshCollider, 0x150000);

    if (mHadClsn) {
        /* Sink, but no lower than 200 below the spawn height. */
        mPosY -= 0x2000;
        if (mPosY < mMaxPosY - 0xc8000)
            mPosY = mMaxPosY - 0xc8000;
        mHadClsn = 0;
    } else {
        /* Rise back to the spawn height. */
        mPosY += 0x2000;
        if (mPosY > mMaxPosY)
            mPosY = mMaxPosY;
    }

    UpdateModelPosAndRotY();
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0) != 0)
        UpdateClsnPosAndRot();
    return 1;
}

// @symbol _ZN15daObjFl_Block_c6RenderEv
s32 daObjFl_Block_c::Render()
{
    mModel.Render(0);
    return 1;
}

// @symbol _ZN15daObjFl_Block_c16CleanupResourcesEv
s32 daObjFl_Block_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();

    data_ov022_02114558.Release();
    data_ov022_02114550.Release();
    return 1;
}
