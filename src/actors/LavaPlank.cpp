//cpp
/* Production translation unit for ov022/LavaPlank, hand-curated.
 * 4 function(s), .text 0x021121cc..0x02112350.
 *
 * Lethal Lava Land bobbing plank of the FL_UKI_KI profile (actor 82): a
 * dBgActor_c that remembers its spawn height, adds a sine term to it every
 * frame, and re-syncs the mesh collider when it is in range. ov022 is Lethal
 * Lava Land.
 *
 * NAME: the cartridge's RTTI spells this class daObjFl_UkiKi_c -- _ZTS at ov022
 * 0x021141b8 is the byte string "15daObjFl_UkiKi_c", and _ZTI at 0x021141ac
 * reads [__si_class_type_info+8, that string, _ZTI10dBgActor_c]. `LavaPlank` is
 * this project's coined name, which every symbol in this range still carries.
 *
 * PARTIAL PROMOTION -- 4 of the class's 6 one-function sources. The ROM run
 * 0x02112130..0x02112350 is one linker run, but its first two members are
 * _ZN9LavaPlankD1Ev and _ZN9LavaPlankD0Ev, and the destructor is this class's
 * key function: any source that defines it also emits _ZTV9LavaPlank,
 * _ZTI9LavaPlank and _ZTS9LavaPlank as vague linkage. The cartridge's own RTTI
 * records are spelled daObjFl_UkiKi_c, so under the coined class name
 * _ZTI9LavaPlank / _ZTS9LavaPlank have no configured ROM home; they can be
 * licensed neither as `deadstrip-data` (which requires a home and compares the
 * bytes) nor as a plain `deadstrip` (which RTTI records may not take), and a
 * multi-function object with surviving non-.text content is refused at
 * isolation. MEASURED: with the destructor declared out of line and left
 * undefined, this source emits no unlicensed symbol or section at all, which is
 * what makes the four-member range promotable today. The two destructors stay
 * one-function sources -- objisolate reduces a single-function object without a
 * policy -- until the class is renamed to the cartridge's RTTI spelling, at
 * which point the whole six-member run can be one file.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S. mwccarm 2004/b56
 * emits one .text section per function in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST here. Do not reorder.
 *
 * deslop
 * Leftover: dBgW_KcMbg::SetFile and dBgActor_c::IsClsnInRange stay mangled in
 *   this TU -- both take Fix12<int> by value (wall 6az); a member call homes
 *   the argument and size-DIFFs InitResources / Behavior.
 * Leftover: func_020393d4 is a 4-byte store into dBgW+0x18
 *   (beforeClsnCallback). This TU calls it; naming belongs with dBgW in arm9.
 * Leftover: data_ov022_02114618 / 02114620 are this overlay's KCL/BMD handles;
 *   data_ov064_0211ba6c is the CLPS block, in overlay .data this TU does not
 *   own.
 * Leftover: g_profile_FL_UKI_KI and daObjFl_UkiKi_c_classInit live outside
 *   this TU (0x02112350, src/d_a_obj_fl_uki_ki.c).
 */

#include "LavaPlank.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

struct KCL_File;
struct CLPS_Block;

extern "C" {
extern SharedFilePtr data_ov022_02114618;   /* the KCL */
extern SharedFilePtr data_ov022_02114620;   /* the BMD */
extern CLPS_Block    data_ov064_0211ba6c;   /* the CLPS block SetFile is handed */
extern s16           data_02082214[];   /* shared sin/cos table, sin at [i*2] */

void func_020393d4(dBgW *bgw, void *fn);

int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(dBgActor_c *self, int a, int b);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *kcl, const Matrix4x3 *mtx, int scale,
    s16 angleY, CLPS_Block *clps);
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9LavaPlank13InitResourcesEv
/* dBgW_KcMbg::SetFile takes Fix12<int> by value. An ordinary member call
 * triggers mwccarm's by-value-class parameter homing and changes the ROM ABI,
 * so that one call deliberately keeps the measured register-level spelling. */
s32 LavaPlank::InitResources()
{
    mModel.SetFile((BMD_File *)Model::LoadFile(data_ov022_02114620), 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    KCL_File *kcl = (KCL_File *)dBgW_Kc::LoadFile(data_ov022_02114618);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, kcl, &mClsnMat, 0x1000, mAngleY,
        &data_ov064_0211ba6c);

    /* The plank carries the mesh along by velocity rather than by its
       transform -- it slides straight up and down. */
    func_020393d4(&mMeshCollider, (void *)&dBgW::UpdatePosWithVelocity);

    mOriginalPosY = mPosY;
    mPhaseAngle = mAngleX;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9LavaPlank8BehaviorEv
/* IsClsnInRange takes two Fix12<int> by value; see InitResources for why that
 * one call keeps the measured register-level spelling. */
s32 LavaPlank::Behavior()
{
    int val = (u16)mPhaseAngle >> 4;
    mPosY = data_02082214[val * 2] * (s16)0x1e + mOriginalPosY;
    mPhaseAngle += 0x400;

    UpdateModelPosAndRotY();
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0))
        UpdateClsnPosAndRot();

    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9LavaPlank6RenderEv
s32 LavaPlank::Render()
{
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9LavaPlank16CleanupResourcesEv
s32 LavaPlank::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();

    data_ov022_02114620.Release();
    data_ov022_02114618.Release();
    return 1;
}
