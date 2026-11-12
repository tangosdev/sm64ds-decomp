//cpp
/* The turntable platforms' shared base -- ov002/daObjKaitendai_c.
 * Abstract: slots 0 (InitResources) and 3 (CleanupResources) are pure.
 * Five leaves call InitResourcesFrom / CleanupResourcesFrom with their
 * own file tables. Setup seeds mPrevAngleZ from arg 3 (or mAngleZ when
 * that is non-zero); Behavior advances mPrevAngleY by that increment.
 * No fields of its own; all five factories pass 0x320.
 *
 * InitResourcesFrom / CleanupResourcesFrom are coined names (slots 0 and 3
 * are pure virtual; the five leaves call these). Nothing in the ROM names
 * them.
 *
 * What stays: abstract class -- no factory, no g_profile row (S14).
 */

#pragma defer_codegen off

#include "daObjKaitendai_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

#include "ResourceDescriptor.h"

/* ROM ordinals 0 and 1 -- ov002 0x020b660c (D0, 0x58) and 0x020b6664 (D1,
 * 0x44). No source here: both destructor variants come from the ONE
 * inline body in include/daObjKaitendai_c.h, which the class's descendants need
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
void daObjKaitendai_c_EmitDeletingDestructor(daObjKaitendai_c *p) {
    delete p;
}

/* Not called. Forces the out-of-line copy of the inline destructor. */
void daObjKaitendai_c_EmitDestructor(daObjKaitendai_c *p) {
    p->~daObjKaitendai_c();
}

/* ROM ordinal 2 -- ov002 0x020b66a8, size 0x48.
 * Teardown half the five leaves call. */
int daObjKaitendai_c::CleanupResourcesFrom(ResourceDescriptor *descriptor) {
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    descriptor->model->Release();
    descriptor->collision->Release();
    return 1;
}

/* ROM ordinal 3 -- vtable slot 9, ov002 0x020b66f0. */
s32 daObjKaitendai_c::Render() {
    mModel.Render(0);
    return 1;
}

/* ROM ordinal 4 -- vtable slot 6, ov002 0x020b6718. */
s32 daObjKaitendai_c::Behavior() {
    mPrevAngleY += mPrevAngleZ;
    mAngleY = mPrevAngleY;
    UpdateModelPosAndRotY();
    if (IsClsnInRangeOnScreen(0, 0))
        UpdateClsnPosAndRot();
    return 1;
}

/* ROM ordinal 5 -- ov002 0x020b676c, size 0xa8.
 * Shared resource setup the five leaves call. Slot 0 is dExtSimpleModel_c::LoadFile,
 * slot 1 is dBgW_Kc::LoadFile, slot 2 is CLPS into SetFile; then bind
 * UpdatePosAndAngs and seed mPrevAngleZ. */
int daObjKaitendai_c::InitResourcesFrom(ResourceDescriptor *descriptor, s16 angle) {
    s16 y;

    mModel.SetFile(dExtSimpleModel_c::LoadFile(*descriptor->model), 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    mMeshCollider.SetFile(dBgW_Kc::LoadFile(*descriptor->collision), mClsnMat, 0x199, mAngleY,
                          *descriptor->clps);
    mMeshCollider.SetBeforeClsnCallback(&dBgW::UpdatePosAndAngs);
    mPrevAngleZ = angle;
    y = mAngleZ;
    if (y != 0) {
        mPrevAngleZ = y;
        mAngleZ = 0;
    }
    return 1;
}
