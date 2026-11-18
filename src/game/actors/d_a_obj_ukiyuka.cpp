//cpp
/* The floating floors' shared base -- ov002/daObjUkiyuka_c.
 * Abstract: slots 0 (InitResources) and 3 (CleanupResources) are pure.
 * daObjFl_Ukiyuka_c (ov022) and daObjKm2_Ukishima_c (ov045) implement them
 * by handing their own Resources record to InitResourcesFrom /
 * CleanupResourcesFrom. Setup seeds mRestY from mPos.y and mBobAmplitude
 * from its argument; Behavior bobs mPos.y on a sine of mBobPhase.
 *
 * Notes:
 * - mBobPhase is stepped through a u16 pun, *(u16 *)&mBobPhase; the plain
 *   s16 member step differs by one word. Measured (#821 r68): a u16 member
 *   steps right but turns the later (s64) read's ldrsh into ldrh.
 * - The (s64) cast on mBobPhase before the u16 table index is load-bearing;
 *   the plain (u16) read is two instructions shorter.
 * - FX_SinIdx reads the sine table, which keeps its arm9 linker name,
 *   FX_SinCosTable_.
 * - Abstract class: no factory, no g_profile row.
 */

#pragma defer_codegen off

#include "daObjUkiyuka_c.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "SharedFilePtr.h"
#include "math/CalcLib.h"

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
int daObjUkiyuka_c::CleanupResourcesFrom(Resources *resources) {
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    resources->model->Release();
    resources->collision->Release();
    return 1;
}

/* ROM ordinal 3 -- vtable slot 9, ov002 0x020b646c. */
s32 daObjUkiyuka_c::Render() {
    mModel.Render(0);
    return 1;
}

/* ROM ordinal 4 -- vtable slot 6, ov002 0x020b6494.
 * mBobPhase is declared s16 but stepped as u16; mRestTimer is passed to
 * DecIfAbove0_Short by address. */
s32 daObjUkiyuka_c::Behavior() {
    if (DecIfAbove0_Short(&mRestTimer) != 0) {
        IsClsnInRange(0, 0);
        return 1;
    }
    /* Kept (#821 r76): a sign pun on a scalar; no block-copy or type lever (r72-r75) applies to
       this scalar pin (r68: s16 plain step is -1 word).
       Kept (#821 r80): getter tried -> an s16 or u16 phase accessor in the step is 0xec of 0xf0.
       Kept (#821 r81): the (s64) read -> an inline s16 GetBobPhase(), a plain u16 or int read,
       or (u16)GetBobPhase() >> 4 are all 0xe8 of 0xf0 (ldrh replaces ldrsh + zero-extend). */
    *(u16 *)&mBobPhase += 0x100;
    {
        u16 h = (u16)((s64)mBobPhase);
        mPos.y -= FX_MUL(mBobAmplitude, FX_SinIdx(h >> 4));
    }
    {
        int d = mPos.y - mRestY;
        if (d < 0)
            d = -d;
        if (d == 0)
            mRestTimer = 60;
    }
    UpdateModelPosAndRotY();
    if (IsClsnInRange(0, 0) != 0)
        UpdateClsnPosAndRot();
    return 1;
}

/* ROM ordinal 5 -- ov002 0x020b6584, size 0x88.
 * Shared resource setup both leaves call. Slot 0 is dExtSimpleModel_c::LoadFile, slot 1
 * is dBgW_Kc::LoadFile, slot 2 is CLPS into SetFile; then mRestY = mPos.y
 * and mBobAmplitude = bobAmplitude. */
int daObjUkiyuka_c::InitResourcesFrom(Resources *resources, int bobAmplitude) {
    mModel.SetFile(dExtSimpleModel_c::LoadFile(*resources->model), 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    mMeshCollider.SetFile(dBgW_Kc::LoadFile(*resources->collision), mClsnMat, 0x1000, mAngle.y,
                          *resources->clps);
    mRestY = mPos.y;
    mBobAmplitude = bobAmplitude;
    return 1;
}
