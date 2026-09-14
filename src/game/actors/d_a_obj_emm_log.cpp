//cpp
/**
 * Tiny-Huge Island rolling log -- ov052/daObjEmmLog_c (7 functions).
 *
 * ROM run 0x021111a0..0x02111440. Bobs mPosY on a sine of mBobPhase;
 * nothing writes mAngle*.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function in the reverse of source order.
 *
 * daObjEmmLog_c_classInit is reconstructed (RTTI daObjEmmLog_c, EMM_LOG
 * registry). Retail does not store that spelling.
 *
 * deslop
 * Leftover: dBgW_KcMbg::SetFile stays mangled (Fix12<int> by value, wall 6az).
 * Leftover: dBgActor_c::IsClsnInRange stays mangled (same 6az claim; header
 *   method form Undefined).
 * Leftover: func_020393a4 is an 8-byte store into dBgW+0x0c. This TU calls
 *   it; naming belongs with dBgW in arm9.
 * Leftover: data_ov052_021124d4 is this overlay's BMD/KCL/CLPS bundle;
 *   symbols.txt has no recovered name.
 * Leftover: sine table data_02082214 (arm9).
 * Leftover: inline destructor (out-of-line emits D0 before D1 plus a
 *   homeless D2). Leaf operator new(unsigned long) until #2570.
 * Leftover: g_profile_EMM_LOG lives outside this TU (S14).
 */

#include "daObjEmmLog_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

struct daObjEmmLog_c_Resources {
    SharedFilePtr *model;
    SharedFilePtr *collision;
    CLPS_Block *clps;
};

extern "C" {
void func_020393a4(dBgW *bgw, int range);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(dBgActor_c *self, int range, int b);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *kcl, const Matrix4x3 *mtx, int scale,
    s16 angleY, CLPS_Block *clps);
extern short data_02082214[];
extern daObjEmmLog_c_Resources data_ov052_021124d4;
}

// @symbol daObjEmmLog_c_classInit
extern "C" daObjEmmLog_c *daObjEmmLog_c_classInit()
{
    return new daObjEmmLog_c();
}

// @symbol _ZN13daObjEmmLog_c13InitResourcesEv
/* dBgW_KcMbg::SetFile takes Fix12<int> by value. An ordinary member call
 * triggers mwccarm's by-value-class parameter homing and changes the ROM ABI. */
s32 daObjEmmLog_c::InitResources()
{
    mModel.SetFile((BMD_File *)Model::LoadFile(*data_ov052_021124d4.model), 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider,
        (KCL_File *)dBgW_Kc::LoadFile(*data_ov052_021124d4.collision),
        &mClsnMat, 0x1000, mAngleY, data_ov052_021124d4.clps);

    mBasePosY = mPosY;
    u8 amplitude = param1 & 0xff;
    if (amplitude == 0xff || param1 == 0)
        mBobAmplitude = 0x64000;
    else
        mBobAmplitude = amplitude * 0xa000;

    mBobPhase = mAngleX;
    return 1;
}

// @symbol _ZN13daObjEmmLog_c8BehaviorEv
s32 daObjEmmLog_c::Behavior()
{
    func_020393a4(&mMeshCollider, 0x600000);

    int idx = (u16)mBobPhase >> 4;
    int s = *(short *)((char *)data_02082214 + (idx << 2));
    int m = (int)(((long long)mBobAmplitude * s + 0x800) >> 12);
    mPosY = mBasePosY + m;

    mBobPhase += 0x200;

    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0x600000, 0);
    return 1;
}

// @symbol _ZN13daObjEmmLog_c6RenderEv
s32 daObjEmmLog_c::Render()
{
    mModel.Render(0);
    return 1;
}

// @symbol _ZN13daObjEmmLog_c16CleanupResourcesEv
s32 daObjEmmLog_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    data_ov052_021124d4.model->Release();
    data_ov052_021124d4.collision->Release();
    return 1;
}

// @symbol _ZN13daObjEmmLog_cD1Ev
// @symbol _ZN13daObjEmmLog_cD0Ev
/* Both destructors are emitted from the INLINE `~daObjEmmLog_c() {}` in
 * include/daObjEmmLog_c.h -- there is deliberately no body here. */
