//cpp
/* Production translation unit for ov022/daObjFl_UkiKi_c, hand-curated.
 * 7 function(s), .text 0x02112130..0x02112380.
 *
 * Lethal Lava Land bobbing plank of the FL_UKI_KI profile (actor 82): a
 * dBgActor_c that remembers its spawn height, adds a sine term to it every
 * frame, and re-syncs the mesh collider when it is in range. ov022 is Lethal
 * Lava Land.
 *
 * NAME: daObjFl_UkiKi_c is the cartridge's RTTI spelling -- _ZTS at ov022
 * 0x021141b8 is the byte string "15daObjFl_UkiKi_c", and _ZTI at 0x021141ac
 * reads [__si_class_type_info+8, that string, _ZTI10dBgActor_c].
 *
 * THE DESTRUCTOR IS THIS CLASS'S KEY FUNCTION, so this TU also emits
 * _ZTV15daObjFl_UkiKi_c, _ZTI15daObjFl_UkiKi_c and _ZTS15daObjFl_UkiKi_c as
 * vague linkage, alongside the inherited bases' RTTI records. Every one of
 * them has a configured ROM home, so all of them license as deadstrip-data and
 * the seven-function object isolates cleanly. The destructor is declared and
 * defined inline and empty in the class header -- see that file for why the
 * body must not move here, and note that there is deliberately no destructor
 * text in this .cpp for an @symbol marker to sit above.
 *
 * THE FACTORY IS PART OF THIS RUN. daObjFl_UkiKi_c_classInit at 0x02112350
 * abuts the member run with a zero gap on both sides -- ov022's previous
 * function is daObjFl_Ukiyuka_c's classInit ending exactly at 0x02112130, and
 * the next is daObjFl_Fall_Block_cD1Ev starting exactly at 0x02112380, so the
 * whole 0x02112130..0x02112380 window is this class's and nothing unlabelled
 * sits at either edge. The ROM bytes allocate 0x328, call dBgActor_c's ctor
 * and store &_ZTV15daObjFl_UkiKi_c[2] -- exactly what `new daObjFl_UkiKi_c()`
 * compiles to, so the factory is written that way rather than by hand.
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
 * Leftover: func_020393d4 is a 4-byte store into dBgW+0x18
 *   (beforeClsnCallback). This TU calls it; naming belongs with dBgW in arm9.
 * Leftover: data_ov064_0211ba6c is the CLPS block, in overlay .data this TU
 *   does not own.
 * Leftover: g_profile_FL_UKI_KI, the FL_UKI_KI registry row that names this
 *   factory, is ov022 .data at 0x021141cc and lives outside this TU.
 */

#include "daObjFl_UkiKi_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

struct KCL_File;
struct CLPS_Block;

extern "C" {
/* This overlay's two file handles, constructed by the ov022 sinit as file IDs
 * 1554 (model) and 1555 (collision). Named rows in ov022/symbols.txt at
 * 0x02114620 and 0x02114618. */
extern SharedFilePtr FloatingFloorLllBig_ModelFile;   /* the BMD */
extern SharedFilePtr FloatingFloorLllBig_ClsnFile;    /* the KCL */
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
// @symbol daObjFl_UkiKi_c_classInit
/* The registry's allocator for FL_UKI_KI. The implicit default constructor is
   inlined into it at -O4: operator new, dBgActor_c's constructor, the vptr
   store. Historical alias in the rename ledger: LavaPlank_Spawn. */
extern "C" daObjFl_UkiKi_c *daObjFl_UkiKi_c_classInit()
{
    return new daObjFl_UkiKi_c();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjFl_UkiKi_c13InitResourcesEv
/* dBgW_KcMbg::SetFile takes Fix12<int> by value. An ordinary member call
 * triggers mwccarm's by-value-class parameter homing and changes the ROM ABI,
 * so that one call deliberately keeps the measured register-level spelling. */
s32 daObjFl_UkiKi_c::InitResources()
{
    mModel.SetFile((BMD_File *)Model::LoadFile(FloatingFloorLllBig_ModelFile), 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    KCL_File *kcl = (KCL_File *)dBgW_Kc::LoadFile(FloatingFloorLllBig_ClsnFile);
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
// @symbol _ZN15daObjFl_UkiKi_c8BehaviorEv
/* IsClsnInRange takes two Fix12<int> by value; see InitResources for why that
 * one call keeps the measured register-level spelling. */
s32 daObjFl_UkiKi_c::Behavior()
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
// @symbol _ZN15daObjFl_UkiKi_c6RenderEv
s32 daObjFl_UkiKi_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjFl_UkiKi_c16CleanupResourcesEv
s32 daObjFl_UkiKi_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();

    FloatingFloorLllBig_ModelFile.Release();
    FloatingFloorLllBig_ClsnFile.Release();
    return 1;
}
