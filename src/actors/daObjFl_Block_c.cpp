//cpp
/* Production translation unit for ov022/daObjFl_Block_c, hand-curated.
 * 6 function(s), .text 0x021116c4..0x0211191c.
 *
 * Lethal Lava Land's floating block of the FL_BLOCK profile: a dBgActor_c
 * that sinks into the lava while the player stands on it -- 2 units a frame,
 * down to 200 below where it spawned -- and floats back up to its spawn
 * height once it is left alone. ov022 is Lethal Lava Land.
 *
 * NAME: daObjFl_Block_c is the cartridge's RTTI spelling -- _ZTS at ov022
 * 0x02113e74 is the byte string "15daObjFl_Block_c", and _ZTI at 0x02113e68
 * reads [__si_class_type_info+8, that string, _ZTI10dBgActor_c].
 *
 * THE DESTRUCTOR IS THIS CLASS'S KEY FUNCTION, so this TU also emits
 * _ZTV15daObjFl_Block_c, _ZTI15daObjFl_Block_c and _ZTS15daObjFl_Block_c as
 * vague linkage, alongside the inherited bases' RTTI records. Every one of
 * them has a configured ROM home, so all of them license as deadstrip-data
 * and the six-function object isolates cleanly. The destructor is declared
 * and defined inline and empty in the class header -- see that file for why
 * the body must not move here, and note that there is deliberately no
 * destructor text in this .cpp for an @symbol marker to sit above.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S. mwccarm 2004/b56
 * emits one .text section per function in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST here. Do not reorder. The
 * compiler chooses where the D1/D0 pair lands on its own.
 *
 * deslop
 * Leftover: dBgW_KcMbg::SetFile and dBgActor_c::IsClsnInRange stay mangled in
 *   this TU -- both take Fix12<int> by value (wall 6az); a member call homes
 *   the argument and size-DIFFs InitResources / Behavior.
 * Leftover: func_020393d4 / func_020393c4 are 4-byte stores into dBgW's
 *   callback slots, and func_020393a4 writes its collision extent. This TU
 *   calls them; naming belongs with dBgW in arm9.
 * Leftover: the model and collision SharedFilePtrs (ov022 .bss 0x02114558 /
 *   0x02114550) and the ov064 CLPS block at 0x0211bb0c are unnamed rows in
 *   their own modules' data, which this TU does not own.
 * Leftover: func_ov022_0211193c, the collision callback InitResources
 *   installs, and func_ov022_0211191c, which it forwards to, sit just past
 *   this run's right edge and are still one-function sources; so is the
 *   factory daObjFl_Block_c_classInit at 0x02111950. tu_map.py ends this
 *   candidate at 0x0211191c, so they are deliberately out of scope here.
 * Leftover: g_profile_FL_BLOCK, the registry row that names that factory, is
 *   ov022 .data at 0x02113e88 and lives outside this TU.
 */

#include "daObjFl_Block_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

extern "C" {
/* This class's model and collision files, ov022 .bss, and the CLPS block
 * dBgW_KcMbg::SetFile is handed, ov064 .data. */
extern SharedFilePtr data_ov022_02114558;
extern SharedFilePtr data_ov022_02114550;
extern char data_ov064_0211bb0c[];

void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, void *kcl, void *mtx, int scale, s16 angleY, void *clps);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);

void func_020393a4(void *p, int v);
void func_020393d4(void *p, void *fn);
void func_020393c4(void *p, void *fn);
void func_ov022_0211193c(void *self, void *a, void *b);
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjFl_Block_c13InitResourcesEv
/* dBgW_KcMbg::SetFile takes Fix12<int> by value. An ordinary member call
 * triggers mwccarm's by-value-class parameter homing and changes the ROM ABI,
 * so that one call deliberately keeps the measured register-level spelling. */
s32 daObjFl_Block_c::InitResources()
{
    void *f = Model::LoadFile(data_ov022_02114558);
    mModel.SetFile((BMD_File *)f, 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    char *k = dBgW_Kc::LoadFile(data_ov022_02114550);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, k, &mClsnMat, 0x199, mAngleY, data_ov064_0211bb0c);

    func_020393d4(&mMeshCollider, (void *)dBgW::UpdatePosWithVelocity);
    func_020393c4(&mMeshCollider, (void *)func_ov022_0211193c);

    mMaxPosY = mPosY;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjFl_Block_c8BehaviorEv
/* IsClsnInRange takes two Fix12<int> by value; see InitResources for why that
 * one call keeps the measured register-level spelling. */
s32 daObjFl_Block_c::Behavior()
{
    func_020393a4(&mMeshCollider, 0x150000);

    if (mHadClsn) {
        mPosY -= 0x2000;
        s32 floor = mMaxPosY - 0xc8000;
        if (mPosY < floor)
            mPosY = floor;
        mHadClsn = 0;
    } else {
        mPosY += 0x2000;
        s32 top = mMaxPosY;
        if (mPosY > top)
            mPosY = top;
    }

    UpdateModelPosAndRotY();
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0) != 0)
        UpdateClsnPosAndRot();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjFl_Block_c6RenderEv
s32 daObjFl_Block_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjFl_Block_c16CleanupResourcesEv
s32 daObjFl_Block_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();

    data_ov022_02114558.Release();
    data_ov022_02114550.Release();
    return 1;
}
