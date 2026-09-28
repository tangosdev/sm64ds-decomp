//cpp
/**
 * Tick Tock Clock rotating gear and triangle (CT_MECHA06 / CT_MECHA07,
 * actors 0x72 / 0x73).
 *
 * One class behind two profiles. InitResources loads the row of the
 * model / collision / clps table selected by the actor id, probes the
 * floor ten units under the spawn point, and hands the mesh
 * dBgW::UpdatePosAndAngs. Behavior is the cog. Clock setting 0 is the
 * slow hand (yaw speed 200), 1 the fast hand (400), 2 the random hand,
 * 3 the stopped hand. The speed lives in mPrevAngleZ, the random-hand
 * target in mPrevAngleX, and the accumulated yaw in mPrevAngleY, which
 * is copied to mAngleY for the model and the mesh.
 *
 * mwccarm lays the definitions below down back to front, so they run
 * from the highest retail address toward the destructor pair the
 * inline ~daObjCtKaitendai_c() emits. Keep that order.
 *
 * Leftover:
 * - dBgW_KcMbg::SetFile and dActor_c::DropShadowRadHeight stay mangled
 *   scalar calls. Passing a Fix12<int> local to the real method
 *   size-DIFFs this TU: InitResources 0x14c against 0x140, and
 *   func_ov065_0211b40c 0x80 against 0x70. A brace initializer and a
 *   C cast to Fix12<int> do not compile on this mwccarm. Same wall as
 *   notes/mwccarm-codegen.md 6az.
 * - dBgActor_c does not declare IsClsnInRange. Behavior's scalar extern
 *   is the call that matches; the Fix12-by-value spelling is the same
 *   wall.
 * - func_020393d4 is the store of dBgW::beforeClsnCallback (+0x18).
 *   This InitResources calls it with &dBgW::UpdatePosAndAngs. No setter.
 * - The sign flip and the yaw add go through a pointer. Writing
 *   mPrevAngleX = mPrevAngleX * -1 comes out 0x12c, and
 *   mPrevAngleY = mPrevAngleY + mPrevAngleZ comes out 0x134, against
 *   Behavior's 0x138. The pointer is what keeps the halfword multiply
 *   and the reload into mAngleY.
 * - ApproachLinear stays _Z14ApproachLinearRsss with an s16* first
 *   parameter, the declaration the rest of the tree agrees on. An s16&
 *   parameter matches this call's bytes and would be a new disagreement.
 * - func_ov065_0211b40c keeps the name in ov065's symbols.txt.
 * - data_ov065_0211d334 (the 200/400 speeds) and data_ov065_0211d35c
 *   (the two resource rows) are not this TU's data. Neither are
 *   g_profile_CT_MECHA06 / CT_MECHA07.
 * - common.h stays first. Matrix4x3's flat 12-word spelling is what
 *   puts the shadow's ground height in m[10].
 * - The actor-id nest stays `!= 0x72` then `== 0x73`, and the floor
 *   probe subtracts 0xa000 on its own statement. Either an else-if or
 *   folding the subtract into the y copy comes out 0x13c against
 *   InitResources' 0x140.
 * - return new emits a homeless _ZN10dBgActor_cD2Ev. The manifest
 *   deadstrips it.
 */

#include "common.h"
#include "daObjCtKaitendai_c.h"
#include "dBgCh_Gnd.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

struct CLPS_Block;

/* One row is {model, collision, clps}. InitResources indexes it by
 * mVariant; CleanupResources releases the same row. */
struct KaitendaiResources {
    SharedFilePtr *model;
    SharedFilePtr *collision;
    CLPS_Block *clps;
};

typedef char KaitendaiResources_size_must_be_0x0c[
    sizeof(KaitendaiResources) == 0x0c ? 1 : -1];

/* Fix12-by-value calls keep the scalar argument the callee actually
 * reads. The class spelling homes it (see the leftover above). */
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
extern u8 data_0209f2c0;              /* clock setting: 0 slow, 1 fast, 2 random, 3 stopped */
extern int data_0209e650;             /* shared RNG state */
extern s16 data_ov065_0211d334[2];    /* yaw speed for the slow and fast hands: 200, 400 */
extern KaitendaiResources data_ov065_0211d35c[];
}

// @symbol daObjCtKaitendai_c_classInit_CT_MECHA06
extern "C" daObjCtKaitendai_c *daObjCtKaitendai_c_classInit_CT_MECHA06()
{
    return new daObjCtKaitendai_c();
}

// @symbol daObjCtKaitendai_c_classInit_CT_MECHA07
extern "C" daObjCtKaitendai_c *daObjCtKaitendai_c_classInit_CT_MECHA07()
{
    return new daObjCtKaitendai_c();
}

// @symbol _ZN18daObjCtKaitendai_c13InitResourcesEv
int daObjCtKaitendai_c::InitResources()
{
    Vector3 pos;

    /* Gear is 0x72 and takes row 0; triangle is 0x73 and takes row 1. */
    if (actorID != 0x72) {
        if (actorID == 0x73)
            mVariant = 1;
    } else {
        mVariant = 0;
    }

    mModel.SetFile(
        (BMD_File *)Model::LoadFile(*data_ov065_0211d35c[mVariant].model),
        1, -1);

    mShadowModel.InitCylinder();
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider,
        (KCL_File *)dBgW_Kc::LoadFile(*data_ov065_0211d35c[mVariant].collision),
        mClsnMat, 0x199, mAngleY,
        *data_ov065_0211d35c[mVariant].clps);

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

// @symbol _ZN18daObjCtKaitendai_c8BehaviorEv
int daObjCtKaitendai_c::Behavior()
{
    switch (data_0209f2c0) {
    case 0:
    case 1:
        mPrevAngleZ = data_ov065_0211d334[data_0209f2c0];
        break;

    case 2:
        if (_Z14ApproachLinearRsss(&mPrevAngleZ, mPrevAngleX, 50) != 0) {
            unsigned short roll =
                (unsigned short)(RandomIntInternal(&data_0209e650) >> 16);

            mPrevAngleX = (s16)((roll % 7) * 200);
            if (roll >= 0x7fff) {
                s16 *target = &mPrevAngleX;
                *target *= (s16)-1;
            }
        }
        break;

    case 3:
        mPrevAngleZ = 0;
        break;
    }

    {
        s16 *yaw = &mPrevAngleY;
        *yaw = (s16)(*yaw + mPrevAngleZ);
    }
    mAngleY = mPrevAngleY;

    UpdateModelPosAndRotY();
    func_ov065_0211b40c(this);

    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0) != 0)
        UpdateClsnPosAndRot();

    return 1;
}

// @symbol _ZN18daObjCtKaitendai_c6RenderEv
int daObjCtKaitendai_c::Render()
{
    mModel.Render(0);
    return 1;
}

// @symbol _ZN18daObjCtKaitendai_c16CleanupResourcesEv
int daObjCtKaitendai_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    data_ov065_0211d35c[mVariant].model->Release();
    data_ov065_0211d35c[mVariant].collision->Release();
    return 1;
}

// @symbol func_ov065_0211b40c
/* Copy the model matrix, plant its translation Y on the floor probe,
 * and drop the shadow. m[10] is that Y in the flat Matrix4x3. */
extern "C" void func_ov065_0211b40c(daObjCtKaitendai_c *c)
{
    c->mShadowMat = c->mModel.mat4x3;
    c->mShadowMat.m[10] = (c->mGroundY + 0x32000) >> 3;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, &c->mShadowModel, &c->mShadowMat, 0x258000, 0xc8000, 0xf);
}
