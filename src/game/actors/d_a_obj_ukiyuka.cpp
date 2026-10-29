//cpp
/* The floating floors' shared base -- ov002/daObjUkiyuka_c.
 * Abstract: slots 0 (InitResources) and 3 (CleanupResources) are pure.
 * daObjFl_Ukiyuka_c (ov022) and daObjKm2_Ukishima_c (ov045) implement them
 * by handing their own Resources record to InitResourcesFrom /
 * CleanupResourcesFrom. Setup seeds mRestY from mPosY and mBobAmplitude
 * from its argument; Behavior bobs mPosY on a sine of mBobPhase.
 *
 * Notes:
 * - mBobPhase is stepped through a u16 pun, *(u16 *)&mBobPhase; the plain
 *   s16 member step differs by one word.
 * - The (s64) cast on mBobPhase before the u16 table index is load-bearing;
 *   the plain (u16) read is two instructions shorter.
 * - The sine table keeps its arm9 linker name, data_02082214.
 * - Abstract class: no factory, no g_profile row.
 */

#pragma defer_codegen off

#include "daObjUkiyuka_c.h"
#include "nitro/fx/fx.h"
#include "SharedFilePtr.h"
#include "math/CalcLib.h"

extern s16 data_02082214[];

// @symbol _ZN14daObjUkiyuka_cD0Ev
// @symbol _ZN14daObjUkiyuka_cD1Ev
/* ROM ordinals 0 and 1 -- ov002 0x020b6388 (D0, 0x58) and 0x020b63e0 (D1,
 * 0x44). No source here: both destructor variants come from the ONE
 * inline body in include/daObjUkiyuka_c.h, which the class's descendants need
 * visible to inline its vptr store.
 *
 * The two calls below are never executed. Under `#pragma defer_codegen off`
 * the compiler emits ordinary functions at parse time in source order, so the
 * delete-expression pulls the deleting variant out of line first and the
 * explicit destructor call pulls the complete-object variant out second --
 * the cartridge's D0-then-D1 order, which no deferred form reaches without a
 * D2 the image does not contain. A delete-expression for D0 because
 * dBgActor_c declares Kill, a key function reachable from this class. */

/* Not called. Forces the out-of-line copy of the deleting destructor. */
void daObjUkiyuka_c_EmitDeletingDestructor(daObjUkiyuka_c *p) {
    delete p;
}

/* Not called. Forces the out-of-line copy of the inline destructor. */
void daObjUkiyuka_c_EmitDestructor(daObjUkiyuka_c *p) {
    p->~daObjUkiyuka_c();
}

/* ROM ordinal 2 -- ov002 0x020b6424, size 0x48.
 * Teardown half both leaves call. */
// @symbol _ZN14daObjUkiyuka_c20CleanupResourcesFromEPNS_9ResourcesE
int daObjUkiyuka_c::CleanupResourcesFrom(Resources *resources) {
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    resources->model->Release();
    resources->collision->Release();
    return 1;
}

/* ROM ordinal 3 -- vtable slot 9, ov002 0x020b646c. */
// @symbol _ZN14daObjUkiyuka_c6RenderEv
s32 daObjUkiyuka_c::Render() {
    mModel.Render(0);
    return 1;
}

/* ROM ordinal 4 -- vtable slot 6, ov002 0x020b6494.
 * mBobPhase is declared s16 but stepped as u16; mRestTimer is passed to
 * DecIfAbove0_Short by address. */
// @symbol _ZN14daObjUkiyuka_c8BehaviorEv
s32 daObjUkiyuka_c::Behavior() {
    if (DecIfAbove0_Short(&mRestTimer) != 0) {
        IsClsnInRange(0, 0);
        return 1;
    }
    *(u16 *)&mBobPhase += 0x100;
    {
        u16 h = (u16)((s64)mBobPhase);
        mPosY -= FX_MUL(mBobAmplitude, data_02082214[(h >> 4) * 2]);
    }
    {
        int d = mPosY - mRestY;
        if (d < 0)
            d = -d;
        if (d == 0)
            mRestTimer = 0x3c;
    }
    UpdateModelPosAndRotY();
    if (IsClsnInRange(0, 0) != 0)
        UpdateClsnPosAndRot();
    return 1;
}

/* ROM ordinal 5 -- ov002 0x020b6584, size 0x88.
 * Shared resource setup both leaves call. Slot 0 is dExtSimpleModel_c::LoadFile, slot 1
 * is dBgW_Kc::LoadFile, slot 2 is CLPS into SetFile; then mRestY = mPosY
 * and mBobAmplitude = bobAmplitude. */
// @symbol _ZN14daObjUkiyuka_c17InitResourcesFromEPNS_9ResourcesEi
int daObjUkiyuka_c::InitResourcesFrom(Resources *resources, int bobAmplitude) {
    mModel.SetFile((BMD_File *)dExtSimpleModel_c::LoadFile(*resources->model), 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    mMeshCollider.SetFile((KCL_File *)dBgW_Kc::LoadFile(*resources->collision), mClsnMat, 0x1000,
                          mAngleY, *resources->clps);
    mRestY = mPosY;
    mBobAmplitude = bobAmplitude;
    return 1;
}
