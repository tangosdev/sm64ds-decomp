//cpp
/* The floating floors' shared base -- ov002/daObjUkiyuka_c.
 * Abstract: slots 0 (InitResources) and 3 (CleanupResources) are pure.
 * daObjFl_Ukiyuka_c (ov022) and daObjKm2_Ukishima_c (ov045) call
 * func_ov002_020b6584 / func_ov002_020b6424 with their own file tables.
 * Setup seeds mRestY from mPosY and mBobAmplitude from arg 3; Behavior
 * bobs mPosY on a sine of mBobPhase.
 *
 * deslop
 * Leftover: func_ov002_020b6584 / 020b6424 keep ROM address names (slots 0
 *   and 3 are pure virtual; the two leaves call these).
 * Leftover: dBgW_KcMbg::SetFile and dBgActor_c::IsClsnInRange stay mangled
 *   (Fix12<int> by value, wall 6az).
 * Leftover: mBobPhase is stepped as *(u16 *)((int)this + 0x328) and
 *   mPosY as *(int *)((int)this + 0x60) -- writing through the member
 *   changes what mwccarm CSEs.
 * Leftover: (s64) mBobPhase / mBobAmplitude launders; a plain int mul
 *   size-DIFFs.
 * Leftover: sine table data_02082214 (arm9).
 * Leftover: abstract class -- no factory, no g_profile row (S14).
 */

#pragma defer_codegen off

#include "daObjUkiyuka_c.h"
#include "SharedFilePtr.h"

struct CLPS_Block;

struct ResourceDescriptor {
    SharedFilePtr *model;
    SharedFilePtr *collision;
    CLPS_Block *clps;
};
typedef char ResourceDescriptor_size_must_be_0x0c[
    sizeof(ResourceDescriptor) == 0x0c ? 1 : -1];

extern "C" {
extern s16 data_02082214[];
u16 DecIfAbove0_Short(u16 *p);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *c, int a, int b);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *kcl, const Matrix4x3 *mtx, int scale,
    s16 angleY, CLPS_Block *clps);

int func_ov002_020b6584(daObjUkiyuka_c *self, ResourceDescriptor *descriptor, int bobAmplitude);
int func_ov002_020b6424(daObjUkiyuka_c *self, ResourceDescriptor *descriptor);
}

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
void daObjUkiyuka_c_EmitDeletingDestructor(daObjUkiyuka_c *p)
{
    delete p;
}

/* Not called. Forces the out-of-line copy of the inline destructor. */
void daObjUkiyuka_c_EmitDestructor(daObjUkiyuka_c *p)
{
    p->~daObjUkiyuka_c();
}

/* ROM ordinal 2 -- func_ov002_020b6424, 0x020b6424, size 0x48.
 * Teardown half both leaves call. */
// @symbol func_ov002_020b6424
extern "C" {
int func_ov002_020b6424(daObjUkiyuka_c *self, ResourceDescriptor *descriptor)
{
    if (self->mMeshCollider.IsEnabled())
        self->mMeshCollider.Disable();
    descriptor->model->Release();
    descriptor->collision->Release();
    return 1;
}
}

/* ROM ordinal 3 -- vtable slot 9, ov002 0x020b646c. */
// @symbol _ZN14daObjUkiyuka_c6RenderEv
s32 daObjUkiyuka_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* ROM ordinal 4 -- vtable slot 6, ov002 0x020b6494.
 * mBobPhase is declared s16 but stepped as u16; mRestTimer is passed to
 * DecIfAbove0_Short by address. The two (int)this + off launders stay. */
// @symbol _ZN14daObjUkiyuka_c8BehaviorEv
s32 daObjUkiyuka_c::Behavior()
{
    char *c = (char *)this;

    if (DecIfAbove0_Short(&mRestTimer) != 0) {
        _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(c, 0, 0);
        return 1;
    }
    *(u16 *)((int)c + 0x328) += 0x100;
    {
        u16 h = (u16)((s64)mBobPhase);
        *(int *)((int)c + 0x60) -=
            (int)((((s64)mBobAmplitude * data_02082214[(h >> 4) * 2]) + 0x800) >> 0xc);
    }
    {
        int d = mPosY - mRestY;
        if (d < 0)
            d = -d;
        if (d == 0)
            mRestTimer = 0x3c;
    }
    UpdateModelPosAndRotY();
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0) != 0)
        UpdateClsnPosAndRot();
    return 1;
}

/* ROM ordinal 5 -- func_ov002_020b6584, 0x020b6584, size 0x88.
 * Shared resource setup both leaves call. Slot 0 is Model::LoadFile, slot 1
 * is dBgW_Kc::LoadFile, slot 2 is CLPS into SetFile; then mRestY = mPosY
 * and mBobAmplitude = arg 3. */
// @symbol func_ov002_020b6584
extern "C" {
int func_ov002_020b6584(daObjUkiyuka_c *self, ResourceDescriptor *descriptor, int bobAmplitude)
{
    self->mModel.SetFile((BMD_File *)Model::LoadFile(*descriptor->model), 1, -1);
    self->UpdateModelPosAndRotY();
    self->UpdateClsnPosAndRot();
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &self->mMeshCollider,
        (KCL_File *)dBgW_Kc::LoadFile(*descriptor->collision),
        &self->mClsnMat, 0x1000, self->mAngleY, descriptor->clps);
    self->mRestY = self->mPosY;
    self->mBobAmplitude = bobAmplitude;
    return 1;
}
}
