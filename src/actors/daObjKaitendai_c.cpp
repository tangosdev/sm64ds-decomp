//cpp
/* The turntable platforms' shared base -- ov002/daObjKaitendai_c.
 * Abstract: slots 0 (InitResources) and 3 (CleanupResources) are pure.
 * Five leaves call func_ov002_020b676c / func_ov002_020b66a8 with their
 * own file tables. Setup seeds mPrevAngleZ from arg 3 (or mAngleZ when
 * that is non-zero); Behavior advances mPrevAngleY by that increment.
 * No fields of its own; all five factories pass 0x320.
 *
 * deslop
 * Leftover: func_ov002_020b676c / 020b66a8 keep ROM address names (slots 0
 *   and 3 are pure virtual; the five leaves call these).
 * Leftover: dBgW_KcMbg::SetFile and dBgActor_c::IsClsnInRangeOnScreen stay
 *   mangled (Fix12<int> by value, wall 6az). SetFile's scale here is 0x199.
 * Leftover: func_020393d4 is a 4-byte store into dBgW+0x18. This TU stores
 *   dBgW::UpdatePosAndAngs there; naming belongs with dBgW in arm9.
 * Leftover: Behavior keeps `s16 *p = &mPrevAngleY` -- compound assignment
 *   recomputes the address and size-DIFFs (0x50 vs 0x54).
 * Leftover: abstract class -- no factory, no g_profile row (S14).
 */

#include "daObjKaitendai_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

struct CLPS_Block;

struct ResourceDescriptor {
    SharedFilePtr *model;
    SharedFilePtr *collision;
    CLPS_Block *clps;
};
typedef char ResourceDescriptor_size_must_be_0x0c[
    sizeof(ResourceDescriptor) == 0x0c ? 1 : -1];

extern "C" {
int _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(void *c, int a, int b);
void func_020393d4(dBgW_KcMbg *clsn, void *fn);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *kcl, const Matrix4x3 *mtx, int scale,
    s16 angleY, CLPS_Block *clps);

int func_ov002_020b676c(daObjKaitendai_c *self, ResourceDescriptor *descriptor, s16 angle);
int func_ov002_020b66a8(daObjKaitendai_c *self, ResourceDescriptor *descriptor);
}

/* ROM ordinal 3 -- func_ov002_020b676c, 0x020b676c, size 0xa8.
 * Shared resource setup the five leaves call. Slot 0 is Model::LoadFile,
 * slot 1 is dBgW_Kc::LoadFile, slot 2 is CLPS into SetFile; then bind
 * UpdatePosAndAngs and seed mPrevAngleZ. */
// @symbol func_ov002_020b676c
extern "C" {
int func_ov002_020b676c(daObjKaitendai_c *self, ResourceDescriptor *descriptor, s16 angle)
{
    s16 y;

    self->mModel.SetFile((BMD_File *)Model::LoadFile(*descriptor->model), 1, -1);
    self->UpdateModelPosAndRotY();
    self->UpdateClsnPosAndRot();
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &self->mMeshCollider,
        (KCL_File *)dBgW_Kc::LoadFile(*descriptor->collision),
        &self->mClsnMat, 0x199, self->mAngleY, descriptor->clps);
    func_020393d4(&self->mMeshCollider, (void *)&dBgW::UpdatePosAndAngs);
    self->mPrevAngleZ = angle;
    y = self->mAngleZ;
    if (y != 0) {
        self->mPrevAngleZ = y;
        self->mAngleZ = 0;
    }
    return 1;
}
}

/* ROM ordinal 2 -- vtable slot 6, ov002 0x020b6718.
 * s16 *p is load-bearing: mPrevAngleY += mPrevAngleZ recomputes the
 * address and comes out 0x50 against the ROM's 0x54. */
// @symbol _ZN16daObjKaitendai_c8BehaviorEv
s32 daObjKaitendai_c::Behavior()
{
    s16 *p = &mPrevAngleY;
    *p = *p + mPrevAngleZ;
    mAngleY = mPrevAngleY;
    UpdateModelPosAndRotY();
    if (_ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(this, 0, 0))
        UpdateClsnPosAndRot();
    return 1;
}

/* ROM ordinal 1 -- vtable slot 9, ov002 0x020b66f0. */
// @symbol _ZN16daObjKaitendai_c6RenderEv
s32 daObjKaitendai_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* ROM ordinal 0 -- func_ov002_020b66a8, 0x020b66a8, size 0x48.
 * Teardown half the five leaves call. */
// @symbol func_ov002_020b66a8
extern "C" {
int func_ov002_020b66a8(daObjKaitendai_c *self, ResourceDescriptor *descriptor)
{
    if (self->mMeshCollider.IsEnabled())
        self->mMeshCollider.Disable();
    descriptor->model->Release();
    descriptor->collision->Release();
    return 1;
}
}
