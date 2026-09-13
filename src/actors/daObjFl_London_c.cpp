//cpp
/**
 * Lethal Lava Land lava bridge -- ov022/daObjFl_London_c (8 functions).
 *
 * ROM run 0x02111980..0x02111cac, plus the class's .data run at
 * 0x02113f2c..0x02113ff0 (_ZTI, _ZTS, g_profile_FL_LONDON, _ZTV). The run
 * ends where _ZTI16daObjFl_Seesaw_c begins.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function in the reverse of source order.
 *
 * daObjFl_London_c_classInit is reconstructed (RTTI daObjFl_London_c,
 * LAVA_BRIDGE / FL_LONDON registry). Retail does not store that spelling.
 *
 * deslop
 * Leftover: dBgW_KcMbg::SetFile stays mangled (Fix12<int> by value, wall 6az).
 * Leftover: dBgActor_c::IsClsnInRange stays mangled (same 6az claim; header
 *   method form Undefined).
 * Leftover: func_020393d4 stores &dBgW::UpdatePosWithTransform (no setter).
 * Leftover: data_ov022_02114580 / data_ov022_02114578 SharedFilePtr handles
 *   and data_ov064_0211bb2c CLPS; symbols.txt has no recovered names.
 * Leftover: common.h first (dBgActor_c.h already pulls it; Model.h's nested
 *   Matrix4x3 would scalarize a 12-word copy).
 * Leftover: Behavior writes mPrevAngleZ through a short* (member
 *   assignment size-DIFFs).
 * Leftover: inline destructor (out-of-line emits D0 before D1 plus a
 *   homeless D2). Leaf operator new(unsigned long) until #2570.
 */

#include "common.h"
#include "daObjFl_London_c.h"
#include "SharedFilePtr.h"
#include "Sound.h"
#include "dBgW.h"
#include "dBgW_Kc.h"

struct CLPS_Block;

extern "C" {
unsigned char DecIfAbove0_Byte(unsigned char *p);
void Matrix4x3_FromRotationZXYExt(void *m, int x, int y, int z);
void func_020393d4(void *p, void *v);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *t, int a, int b);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, void *kcl, Matrix4x3 *mtx, int fix, short s, void *clps);
void func_ov022_02111a1c(daObjFl_London_c *t);

extern SharedFilePtr data_ov022_02114580;
extern SharedFilePtr data_ov022_02114578;
extern CLPS_Block data_ov064_0211bb2c;
}

// @symbol daObjFl_London_c_classInit
extern "C" daObjFl_London_c *daObjFl_London_c_classInit()
{
    return new daObjFl_London_c();
}

/* The 0x1c actor descriptor the profile table points at. Word 0 relocates to
 * the factory above; the rest is the ROM's, read back at 0x02113f4c. */
struct LondonSpawnInfo {
    daObjFl_London_c *(*classInit)();
    s16 profileIDAndExecuteOrder;
    s16 drawOrder;
    u32 actorFlags;
    s32 clipOffsetY;
    s32 clipRadius;
    s32 clipDistance;
    s32 farDistance;
};
typedef char LondonSpawnInfo_size_must_be_0x1c[sizeof(LondonSpawnInfo) == 0x1c ? 1 : -1];

// @symbol g_profile_FL_LONDON
extern "C" LondonSpawnInfo g_profile_FL_LONDON = {
    daObjFl_London_c_classInit, 0x0049, 0x0119, 0x00000002,
    0x00100000, 0x00300000, 0x02000000, 0x00000000
};

// @symbol _ZN16daObjFl_London_c13InitResourcesEv
s32 daObjFl_London_c::InitResources()
{
    void *f = Model::LoadFile(data_ov022_02114580);
    mModel.SetFile((BMD_File *)f, 1, -1);
    func_ov022_02111a1c(this);
    UpdateClsnPosAndRot();
    void *k = dBgW_Kc::LoadFile(data_ov022_02114578);
    /* MATCH: real mMeshCollider.SetFile DIFFs (Fix12<int> by value, wall 6az). */
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, k, &mClsnMat, 0x1000, mAngleY, &data_ov064_0211bb2c);
    func_020393d4(&mMeshCollider, (void *)&dBgW::UpdatePosWithTransform);
    mFlag = 0;
    mCooldown = 0xf;
    return 1;
}

// @symbol _ZN16daObjFl_London_c8BehaviorEv
s32 daObjFl_London_c::Behavior()
{
    if (DecIfAbove0_Byte(&mCooldown) == 0) {
        if (mFlag == 0) {
            /* MATCH: mPrevAngleZ = mPrevAngleZ - 0x100 size-DIFFs (member
             * addressing CSEs the field). */
            short *p = &mPrevAngleZ;
            *p = *p - 0x100;
            if (mPrevAngleZ <= -0x2000) { mPrevAngleZ = -0x2000; mCooldown = 0xf; mFlag = 1; }
        } else {
            short *p = &mPrevAngleZ;
            *p = *p + 0x100;
            if (mPrevAngleZ >= 0) { mPrevAngleZ = 0; mCooldown = 0xf; mFlag = 0; }
        }
    } else {
        if (mCooldown == 1) {
            if (mFlag == 0) Sound::PlayBank3(0x34, *(Vector3 *)&mCamSpacePosX);
            else            Sound::PlayBank3(0x35, *(Vector3 *)&mCamSpacePosX);
        }
    }
    mAngleZ = mPrevAngleZ;
    func_ov022_02111a1c(this);
    /* MATCH: header method form of IsClsnInRange is Undefined (wall 6az). */
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0))
        UpdateClsnPosAndRot();
    return 1;
}

// @symbol _ZN16daObjFl_London_c6RenderEv
s32 daObjFl_London_c::Render()
{
    mModel.Render(0);
    return 1;
}

// @symbol _ZN16daObjFl_London_c16CleanupResourcesEv
s32 daObjFl_London_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    data_ov022_02114580.Release();
    data_ov022_02114578.Release();
    return 1;
}

// @symbol func_ov022_02111a1c
/* Rebuild the model matrix from the actor's three angles, then publish the
 * position at 1/8 scale. Both callers (InitResources and Behavior) are in
 * this TU. */
extern "C" void func_ov022_02111a1c(daObjFl_London_c *t)
{
    Matrix4x3_FromRotationZXYExt(&t->mModel.mat4x3, t->mAngleX, t->mAngleY, t->mAngleZ);
    t->mModel.mat4x3.m[9]  = t->mPosX >> 3;
    t->mModel.mat4x3.m[10] = t->mPosY >> 3;
    t->mModel.mat4x3.m[11] = t->mPosZ >> 3;
}

// @symbol _ZN16daObjFl_London_cD1Ev
// @symbol _ZN16daObjFl_London_cD0Ev
/* Both destructors are emitted from the INLINE `~daObjFl_London_c() {}` in
 * include/daObjFl_London_c.h -- there is deliberately no body here.
 *
 * Defining ~daObjFl_London_c() out of line HERE instead would break the TU two
 * ways: mwccarm would emit D0 at 0x021119c4 before D1 at 0x02111980, reversing
 * ROM order so objisolate refuses the entire TU, and it would emit a third
 * symbol, D2, which has no address anywhere in the cartridge.
 */
