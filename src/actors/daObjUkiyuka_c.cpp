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

/* ROM ordinal 3 -- func_ov002_020b6584, 0x020b6584, size 0x88.
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

/* ROM ordinal 2 -- vtable slot 6, ov002 0x020b6494.
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

/* ROM ordinal 1 -- vtable slot 9, ov002 0x020b646c. */
// @symbol _ZN14daObjUkiyuka_c6RenderEv
s32 daObjUkiyuka_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* ROM ordinal 0 -- func_ov002_020b6424, 0x020b6424, size 0x48.
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
