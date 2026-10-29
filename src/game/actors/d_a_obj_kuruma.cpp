//cpp
/* The rickshaw platforms' shared base -- ov002/daObjKuruma_c.
 * Abstract: slots 0 (InitResources) and 3 (CleanupResources) are pure.
 * daObjKm1_Kuruma_c (ov043) and daObjKm3_Kuruma_c (ov047) implement them by
 * handing their own Resources record to InitResourcesFrom /
 * CleanupResourcesFrom. No fields of its own; both leaf factories pass 0x320.
 *
 * Notes:
 * - SetFile's scale here is 0x199.
 * - func_020393d4 is a 4-byte store into dBgW+0x18. This TU stores
 *   dBgW::UpdatePosWithVelocity there; naming belongs with dBgW in arm9.
 * - Abstract class: no factory, no g_profile row.
 */

#pragma defer_codegen off

#include "daObjKuruma_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

/* local extern: no shared header declares it. */
extern "C" {
void func_020393d4(dBgW_KcMbg *clsn, void *fn);
}

// @symbol _ZN13daObjKuruma_cD0Ev
// @symbol _ZN13daObjKuruma_cD1Ev
/* ROM ordinals 0 and 1 -- ov002 0x020b6814 (D0, 0x58) and 0x020b686c (D1,
 * 0x44). No source here: both destructor variants come from the ONE inline
 * body in include/daObjKuruma_c.h, which the two descendants need visible
 * to inline its vptr store.
 *
 * The two calls below are never executed. Under `#pragma defer_codegen off`
 * the compiler emits ordinary functions at parse time in source order, so the
 * delete-expression pulls the deleting variant out of line first and the
 * explicit destructor call pulls the complete-object variant out second --
 * the cartridge's D0-then-D1 order, which no deferred form reaches without a
 * D2 the image does not contain. A delete-expression for D0 because
 * dBgActor_c declares Kill, a key function reachable from this class. */

/* Not called. Forces the out-of-line copy of the deleting destructor. */
void daObjKuruma_c_EmitDeletingDestructor(daObjKuruma_c *p) {
    delete p;
}

/* Not called. Forces the out-of-line copy of the inline destructor. */
void daObjKuruma_c_EmitDestructor(daObjKuruma_c *p) {
    p->~daObjKuruma_c();
}

/* ROM ordinal 2 -- ov002 0x020b68b0, size 0x48.
 * Teardown half both leaves call. */
// @symbol _ZN13daObjKuruma_c20CleanupResourcesFromEPNS_9ResourcesE
int daObjKuruma_c::CleanupResourcesFrom(Resources *resources) {
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    resources->model->Release();
    resources->collision->Release();
    return 1;
}

/* ROM ordinal 3 -- vtable slot 9, ov002 0x020b68f8. Key function. */
// @symbol _ZN13daObjKuruma_c6RenderEv
s32 daObjKuruma_c::Render() {
    mModel.Render(0);
    return 1;
}

/* ROM ordinal 4 -- vtable slot 6, ov002 0x020b6920. */
// @symbol _ZN13daObjKuruma_c8BehaviorEv
s32 daObjKuruma_c::Behavior() {
    UpdateModelPosAndRotY();
    if (IsClsnInRange(0, 0))
        UpdateClsnPosAndRot();
    return 1;
}

/* ROM ordinal 5 -- ov002 0x020b6958, size 0x8c.
 * Shared resource setup both leaves call. Slot 0 is dExtSimpleModel_c::LoadFile, slot 1
 * is dBgW_Kc::LoadFile, slot 2 is CLPS into SetFile; then bind the mesh
 * to dBgW::UpdatePosWithVelocity. No field of this class is seeded. */
// @symbol _ZN13daObjKuruma_c17InitResourcesFromEPNS_9ResourcesE
int daObjKuruma_c::InitResourcesFrom(Resources *resources) {
    mModel.SetFile((BMD_File *)dExtSimpleModel_c::LoadFile(*resources->model), 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    mMeshCollider.SetFile((KCL_File *)dBgW_Kc::LoadFile(*resources->collision), mClsnMat, 0x199,
                          mAngleY, *resources->clps);
    func_020393d4(&mMeshCollider, (void *)&dBgW::UpdatePosWithVelocity);
    return 1;
}
