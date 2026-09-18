//cpp
/* Production translation unit for ov065/daObjCtKaitendai_c.
 *
 * deslop
 *
 * Leftover:
 * - dBgW_KcMbg::SetFile / DropShadowRadHeight / dBgActor_c::IsClsnInRange
 *   stay mangled (Fix12-by-value, 6az)
 * - func_020393d4 stores dBgW+0x18 (beforeClsnCallback); this TU's
 *   InitResources writes &dBgW::UpdatePosAndAngs (no setter)
 * - Behavior 0x96 / 0x92 / 0x94 / 0x8e stay offset (named mPrevAngleZ /
 *   mPrevAngleX / mPrevAngleY / mAngleY size-DIFF this TU's switch)
 * - ApproachLinear stays mangled `_Z14ApproachLinearRsss` (s16& form
 *   size-DIFFs the same body)
 * - func_ov065_0211b40c keeps its ROM address name
 * - data_ov065_0211d334 / 0211d35c handles; this TU is text-only
 *   (S14 no g_profile_CT_MECHA06 / CT_MECHA07)
 * - common.h first (func_ov065_0211b40c mShadowMat.m[10] needs the flat
 *   12-word spelling)
 * - return new emits homeless _ZN10dBgActor_cD2Ev; compiler-only policy
 *   deadstrips it
 *
 * mwccarm emits ordinary functions in reverse source order, so the nine
 * definitions below intentionally run from the highest retail address back
 * toward the compiler-owned destructor group. Keep the factories first.
 *
 * The two factories (CT_MECHA06 / CT_MECHA07) are `return new` in this file.
 * Those classInit spellings are reconstructed: ROM RTTI names the class and
 * the registry IDs name the profiles; exact original SM64DS identifiers are
 * not preserved.
 */

#include "common.h"
#include "daObjCtKaitendai_c.h"
#include "dBgCh_Gnd.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

struct CLPS_Block;

/* One 2-row {model, collision, clps} table indexed by mVariant.
 * 0211d35c / 0211d360 / 0211d364 are the three columns of row 0. Init
 * LoadFile/SetFile and Cleanup Release all consume those slots. */
struct KaitendaiResources {
    SharedFilePtr *model;
    SharedFilePtr *collision;
    CLPS_Block *clps;
};

typedef char KaitendaiResources_size_must_be_0x0c[
    sizeof(KaitendaiResources) == 0x0c ? 1 : -1];

/* Fix12-by-value calls retain their measured raw ABI declarations. Natural
 * class-typed declarations make mwccarm home arguments absent from retail. */
extern "C" {
extern int _Z14ApproachLinearRsss(s16 *cur, s16 target, s16 step);
extern void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    dActor_c *actor, ShadowModel *shadow, Matrix4x3 *matrix,
    int radius, int height, u32 opacity);
extern int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *thisp, KCL_File *kcl, const Matrix4x3 &mtx, int fix, short s,
    CLPS_Block &clps);
extern void func_020393d4(void *p, void *v);
extern void func_ov065_0211b40c(daObjCtKaitendai_c *c);
extern unsigned int RandomIntInternal(int *seed);
extern u8 data_0209f2c0;
extern int data_0209e650;
extern s16 data_ov065_0211d334;
extern KaitendaiResources data_ov065_0211d35c[];
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol daObjCtKaitendai_c_classInit_CT_MECHA06
/* Reconstructed source-style name. ROM RTTI names daObjCtKaitendai_c; the
 * CT_MECHA06 registry profile selects this factory. Exact original spelling
 * is not preserved. Historical alias: TtcRotatingGear_Spawn. */
extern "C" daObjCtKaitendai_c *daObjCtKaitendai_c_classInit_CT_MECHA06()
{
    return new daObjCtKaitendai_c();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol daObjCtKaitendai_c_classInit_CT_MECHA07
/* Reconstructed source-style name. ROM RTTI names daObjCtKaitendai_c; the
 * CT_MECHA07 registry profile selects this factory. Exact original spelling
 * is not preserved. Historical alias: TtcRotatingTriangle_Spawn. */
extern "C" daObjCtKaitendai_c *daObjCtKaitendai_c_classInit_CT_MECHA07()
{
    return new daObjCtKaitendai_c();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN18daObjCtKaitendai_c13InitResourcesEv
int daObjCtKaitendai_c::InitResources()
{
    Vector3 pos;
    int i;
    void *bmd;
    void *kcl;

    if (actorID != 0x72) {
        if (actorID == 0x73)
            mVariant = 1;
    } else {
        mVariant = 0;
    }

    i = mVariant;
    bmd = Model::LoadFile(*data_ov065_0211d35c[i].model);
    mModel.SetFile((BMD_File *)bmd, 1, -1);

    mShadowModel.InitCylinder();
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    i = mVariant;
    kcl = dBgW_Kc::LoadFile(*data_ov065_0211d35c[i].collision);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, (KCL_File *)kcl, mClsnMat, 0x199, mAngleY,
        *data_ov065_0211d35c[i].clps);

    func_020393d4(&mMeshCollider, (void *)&dBgW::UpdatePosAndAngs);

    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    pos.y = pos.y - 0xa000;

    {
        dBgCh_Gnd ground;

        ground.SetObjAndPos(pos, (dActor_c *)0);
        mGroundY = pos.y;
        if (ground.DetectClsn() != 0)
            mGroundY = ground.clsnY;
    }

    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN18daObjCtKaitendai_c8BehaviorEv
int daObjCtKaitendai_c::Behavior()
{
    void *c = (void *)this;

    switch (data_0209f2c0) {
    case 0:
    case 1:
        *(s16 *)((char *)c + 0x96) =
            (&data_ov065_0211d334)[data_0209f2c0];
        break;

    case 2:
        if (_Z14ApproachLinearRsss(
                (s16 *)((char *)c + 0x96),
                *(s16 *)((char *)c + 0x92), 0x32) != 0) {
            unsigned short r =
                (unsigned short)(RandomIntInternal(&data_0209e650) >> 16);

            *(s16 *)((char *)c + 0x92) =
                (s16)((r % 7) * 0xc8);

            if (r < 0x7fff)
                goto no_reverse;

            {
                s16 *p = (s16 *)(
                    ((int)c + 0x92));
                *p *= (s16)-1;
            }
        }
    no_reverse:
        break;

    case 3:
        *(s16 *)((char *)c + 0x96) = 0;
        break;
    }

    {
        s16 *p = (s16 *)(
            ((int)c + 0x94));
        *p = (s16)(*p + *(s16 *)((char *)c + 0x96));
    }

    *(s16 *)((char *)c + 0x8e) =
        *(s16 *)((char *)c + 0x94);

    UpdateModelPosAndRotY();
    func_ov065_0211b40c((daObjCtKaitendai_c *)c);

    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(c, 0, 0) != 0)
        UpdateClsnPosAndRot();

    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN18daObjCtKaitendai_c6RenderEv
int daObjCtKaitendai_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN18daObjCtKaitendai_c16CleanupResourcesEv
int daObjCtKaitendai_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    data_ov065_0211d35c[mVariant].model->Release();
    data_ov065_0211d35c[mVariant].collision->Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov065_0211b40c
/* Drops the rotating bar's shadow: copies the model matrix, writes ground Y
 * into its translation, then hands it to dActor_c::DropShadowRadHeight. */
extern "C" void func_ov065_0211b40c(daObjCtKaitendai_c *c)
{
    c->mShadowMat = c->mModel.mat4x3;
    c->mShadowMat.m[10] = (c->mGroundY + 0x32000) >> 3;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, &c->mShadowModel, &c->mShadowMat, 0x258000, 0xc8000, 0xf);
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* No separate body lives here. The inline virtual destructor in the directly
 * included class header makes mwccarm emit retail's D1 then D0 order without
 * the otherwise homeless D2 variant. */
