//cpp
/* The rickshaw platforms' shared base -- ov002/daObjKuruma_c.
 * Abstract: slots 0 (InitResources) and 3 (CleanupResources) are pure.
 * daObjKm1_Kuruma_c (ov043) and daObjKm3_Kuruma_c (ov047) call
 * func_ov002_020b6958 / func_ov002_020b68b0 with their own file tables.
 * No fields of its own; both leaf factories pass 0x320.
 *
 * deslop
 * Leftover: func_ov002_020b6958 / 020b68b0 keep ROM address names (slots 0
 *   and 3 are pure virtual; the two leaves call these).
 * Leftover: dBgW_KcMbg::SetFile and dBgActor_c::IsClsnInRange stay mangled
 *   (Fix12<int> by value, wall 6az). SetFile's scale here is 0x199.
 * Leftover: func_020393d4 is a 4-byte store into dBgW+0x18. This TU stores
 *   dBgW::UpdatePosWithVelocity there; naming belongs with dBgW in arm9.
 * Leftover: abstract class -- no factory, no g_profile row (S14).
 */

#include "daObjKuruma_c.h"
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
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *thiz, int x, int z);
void func_020393d4(dBgW_KcMbg *clsn, void *fn);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *kcl, const Matrix4x3 *mtx, int scale,
    s16 angleY, CLPS_Block *clps);

int func_ov002_020b6958(daObjKuruma_c *self, ResourceDescriptor *descriptor);
int func_ov002_020b68b0(daObjKuruma_c *self, ResourceDescriptor *descriptor);
}

/* ROM ordinal 3 -- func_ov002_020b6958, 0x020b6958, size 0x8c.
 * Shared resource setup both leaves call. Slot 0 is Model::LoadFile, slot 1
 * is dBgW_Kc::LoadFile, slot 2 is CLPS into SetFile; then bind the mesh
 * to dBgW::UpdatePosWithVelocity. No field of this class is seeded. */
// @symbol func_ov002_020b6958
extern "C" {
int func_ov002_020b6958(daObjKuruma_c *self, ResourceDescriptor *descriptor)
{
    self->mModel.SetFile((BMD_File *)Model::LoadFile(*descriptor->model), 1, -1);
    self->UpdateModelPosAndRotY();
    self->UpdateClsnPosAndRot();
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &self->mMeshCollider,
        (KCL_File *)dBgW_Kc::LoadFile(*descriptor->collision),
        &self->mClsnMat, 0x199, self->mAngleY, descriptor->clps);
    func_020393d4(&self->mMeshCollider, (void *)&dBgW::UpdatePosWithVelocity);
    return 1;
}
}

/* ROM ordinal 2 -- vtable slot 6, ov002 0x020b6920. */
// @symbol _ZN13daObjKuruma_c8BehaviorEv
s32 daObjKuruma_c::Behavior()
{
    UpdateModelPosAndRotY();
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0))
        UpdateClsnPosAndRot();
    return 1;
}

/* ROM ordinal 1 -- vtable slot 9, ov002 0x020b68f8. Key function. */
// @symbol _ZN13daObjKuruma_c6RenderEv
s32 daObjKuruma_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* ROM ordinal 0 -- func_ov002_020b68b0, 0x020b68b0, size 0x48.
 * Teardown half both leaves call. */
// @symbol func_ov002_020b68b0
extern "C" {
int func_ov002_020b68b0(daObjKuruma_c *self, ResourceDescriptor *descriptor)
{
    if (self->mMeshCollider.IsEnabled())
        self->mMeshCollider.Disable();
    descriptor->model->Release();
    descriptor->collision->Release();
    return 1;
}
}
