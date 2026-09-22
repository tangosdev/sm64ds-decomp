//cpp
/* Production translation unit for ov065/daObjCtMecha05_c.
 *
 * deslop
 *
 * Leftover:
 * - dBgW_KcMbg::SetFile / DropShadowScaleXYZ / dBgActor_c::IsClsnInRange
 *   stay mangled (Fix12-by-value, 6az)
 * - func_020393d4 stores dBgW+0x18 (beforeClsnCallback); func_020393a4
 *   stores +0x0c; func_02039394 stores +0x10 (no setter)
 * - func_ov065_0211ac0c / func_ov065_0211ad70 stay address-named
 *   (symbols.txt; naming them as methods would miss those labels)
 * - data_ov065_0211c0c8 / data_ov065_0211d26c; TTC_MovingBar_ModelFile /
 *   TTC_MovingBar_ClsnFile are symbols.txt BSS labels shared with the
 *   neighbouring class
 * - SharedFilePtr +4 BMD (LoadFile/Release; header has no fields)
 * - common.h first (func_ov065_0211ac0c mShadowMatrix.m[9..11])
 * - return new emits homeless _ZN10dBgActor_cD2Ev; compiler-only policy
 *   deadstrips it
 *
 * mwccarm emits ordinary functions in reverse source order, so the nine
 * definitions below intentionally run from the highest retail address back
 * toward the compiler-owned destructor group. Keep the factory first.
 *
 * The factory is `return new` in this file.
 */

#include "common.h"
#include "daObjCtMecha05_c.h"
#include "dBgCh_Gnd.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

/* Actor/process profile descriptor at ov065:0x0211d290. Field roles are
 * recovered from fBase_c/dActor_c consumers; exact original member spellings
 * are not preserved. */
struct CtMecha05SpawnInfo {
    daObjCtMecha05_c *(*classInit)();
    s16 profileIDAndExecuteOrder;
    s16 drawOrder;
    u32 actorFlags;
    Fix12i clipOffsetY;
    Fix12i clipRadius;
    Fix12i clipDistance;
    Fix12i farDistance;
};

typedef char CtMecha05SpawnInfo_size_must_be_0x1c[
    sizeof(CtMecha05SpawnInfo) == 0x1c ? 1 : -1];

/* Fix12-by-value calls retain their measured raw ABI declarations. Natural
 * class-typed declarations make mwccarm home arguments absent from retail. */
extern "C" {
extern void Matrix4x3_FromRotationY(Matrix4x3 *matrix, int angle);
extern void MulVec3Mat4x3(const Vector3 *src, const Matrix4x3 *matrix,
                          Vector3 *dst);
extern void AddVec3(const Vector3 *left, const Vector3 *right, Vector3 *dst);
extern void _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
    dActor_c *actor, ShadowModel *shadow, Matrix4x3 *matrix,
    int scaleX, int scaleY, int scaleZ, u32 opacity);
extern int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(
    dBgActor_c *actor, int radius, int verticalOffset);
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *collider, KCL_File *file, Matrix4x3 *matrix,
    int scale, s16 angle, CLPS_Block *clps);
extern void func_020393d4(dBgW *collider, void *callback);
extern void func_020393a4(dBgW *collider, int value);
extern void func_02039394(dBgW *collider, int value);

extern void func_ov065_0211ac0c(daObjCtMecha05_c *actor);
extern void func_ov065_0211ad70(daObjCtMecha05_c *actor);
extern int RandomIntInternal(int *seed);
extern u16 DecIfAbove0_Short(u16 *p);

extern SharedFilePtr TTC_MovingBar_ModelFile;
extern SharedFilePtr TTC_MovingBar_ClsnFile;
extern u8 data_0209f2c0;
extern int data_0209e650;
extern s16 data_02082214[];
extern s16 data_ov065_0211c0c8[];
extern s8 data_ov065_0211d26c[];
extern CLPS_Block data_ov035_02112258;
}

/* CT_MECHA05 is the literal ROM registry ID. The g_profile spelling is a
 * lineage-supported reconstruction. */
extern "C" daObjCtMecha05_c *daObjCtMecha05_c_classInit();
extern "C" CtMecha05SpawnInfo g_profile_CT_MECHA05 = {
    daObjCtMecha05_c_classInit,
    0x0071,
    0x00ec,
    0x00000002,
    0,
    0x00600000,
    0x01000000,
    0x02000000
};

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol daObjCtMecha05_c_classInit
extern "C" daObjCtMecha05_c *daObjCtMecha05_c_classInit()
{
    return new daObjCtMecha05_c();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjCtMecha05_c13InitResourcesEv
int daObjCtMecha05_c::InitResources()
{
    void *modelFile = Model::LoadFile(TTC_MovingBar_ModelFile);
    mModel.SetFile((BMD_File *)modelFile, 1, -1);
    mShadowModel.InitCuboid();
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    void *collisionFile = dBgW_Kc::LoadFile(TTC_MovingBar_ClsnFile);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, (KCL_File *)collisionFile,
        &mClsnMat, 0x199, mAngleY,
        &data_ov035_02112258);
    func_020393d4(&mMeshCollider,
        (void *)&dBgW::UpdatePosWithTransform);
    func_020393a4(&mMeshCollider, 0x1c0000);
    func_02039394(&mMeshCollider, 0x1000);
    mPrevAngleY = mAngleY;
    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    mStateTimer = data_ov065_0211c0c8[data_0209f2c0];

    Vector3 position;
    position.x = mPosX;
    position.y = mPosY;
    position.z = mPosZ;
    position.y = position.y - 0xa000;
    dBgCh_Gnd ground;
    ground.SetObjAndPos(position, 0);
    mGroundY = position.y;
    if (ground.DetectClsn())
        mGroundY = ground.clsnY;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjCtMecha05_c8BehaviorEv
/* mHorzSpeed is reused here as a fix12 travel accumulator rather than a
 * physical speed. */
int daObjCtMecha05_c::Behavior()
{
    if (data_0209f2c0 == 3) {
        int ang;
        /* ldrh, not the ldrsh a plain s16 mPrevAngleY read would emit. */
        ang = *(u16 *)&mPrevAngleY;
        mPosX = mHomePosX +
            (int)(((long long)data_02082214[(ang >> 4) << 1] * 0xfa000 + 0x800) >> 12);
        ang = *(u16 *)&mPrevAngleY;
        mPosZ = mHomePosZ +
            (int)(((long long)data_02082214[((ang >> 4) << 1) + 1] * 0xfa000 + 0x800) >> 12);
        UpdateModelPosAndRotY();
        func_ov065_0211ac0c(this);
        if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0) != 0)
            UpdateClsnPosAndRot();
        return 1;
    }

    mPrevTravel = mTravel;
    mTravel += mHorzSpeed;

    switch (mState) {
    case 0:
        if (DecIfAbove0_Short((u16 *)&mStateTimer) != 0) goto Lend;
        if (data_0209f2c0 == 2) {
            int v = (u16)((unsigned)RandomIntInternal(&data_0209e650) >> 16);
            if (v % 2 == 0) {
                mStateTimer = v % 100 + 20;
            }
        }
        mState++;
        mHorzSpeed = -0x8000;
        goto Lend;

    case 1:
        mHorzSpeed += 0xbae;
        if (mHorzSpeed > 0) {
            if (DecIfAbove0_Short((u16 *)&mStateTimer) != 0) {
                mHorzSpeed = 0;
                goto Lend;
            }
            mState++;
            mHorzSpeed = 0x1d000;
        }
        goto Lend;

    case 2: {
        int d = mTravel;
        if (d != 0xfa000) {
            int val = (int)(((long long)(0xfa000 - d) * (0xfa000 - mPrevTravel) + 0x800) >> 12);
            if (val >= 0) goto L280;
        }
        {
            int sp = mHorzSpeed;
            if (sp <= -0x8000) goto L280;
            if (sp >= 0x8000) goto L280;
            mState++;
            mHorzSpeed = 0;
            mStateTimer = 0x1e;
            goto Lend;
        }
      L280:
        {
            int m = (mTravel < 0xfa000) ? 0x6666 : -0x6666;
            int sp = mHorzSpeed;
            int delta;
            if ((int)(((long long)sp * m + 0x800) >> 12) >= 0)
                delta = m;
            else
                delta = (int)(((long long)m * 0x2599 + 0x800) >> 12);
            mHorzSpeed += delta;
            if (data_0209f2c0 != 2) goto Lend;
        }
        if ((int)(((long long)mTravel * mPrevTravel + 0x800) >> 12) >= 0) goto Lend;
        if ((RandomIntInternal(&data_0209e650) & 3) != 0) goto Lend;
        func_ov065_0211ad70(this);
        goto Lend;
    }

    case 3:
        if (DecIfAbove0_Short((u16 *)&mStateTimer) != 0) goto Lend;
        mHorzSpeed = -0x5000;
        if (mTravel < 0)
            func_ov065_0211ad70(this);
        goto Lend;
    }

Lend:
    UpdatePos(0);
    UpdateModelPosAndRotY();
    func_ov065_0211ac0c(this);
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(
            this, 0x1c0000, 0x1000) != 0)
        UpdateClsnPosAndRot();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
extern "C" {  /* Address-named TU-local helper; original spelling unknown. */
void func_ov065_0211ad70(daObjCtMecha05_c *actor)
{
    actor->mState = 0;
    actor->mTravel = 0;
    actor->mHorzSpeed = 0;
    actor->mPosX = actor->mHomePosX;
    actor->mPosY = actor->mHomePosY;
    actor->mPosZ = actor->mHomePosZ;

    u8 setting = data_0209f2c0;
    actor->mStateTimer = data_ov065_0211c0c8[setting];
    if (setting != 2)
        return;

    actor->mStateTimer = data_ov065_0211d26c[
        (u16)((u32)RandomIntInternal(&data_0209e650) >> 16) & 3];
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjCtMecha05_c6RenderEv
int daObjCtMecha05_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjCtMecha05_c16CleanupResourcesEv
int daObjCtMecha05_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    TTC_MovingBar_ModelFile.Release();
    TTC_MovingBar_ClsnFile.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
extern "C" {  /* Address-named TU-local helper; original spelling unknown. */
void func_ov065_0211ac0c(daObjCtMecha05_c *actor)
{
    int distance = actor->mPosY - actor->mGroundY;
    if (distance < 0)
        distance = -distance;
    if (distance > 0x9c4000 || distance < 0xa000)
        return;

    Vector3 offset;
    Vector3 shadowPos;
    offset.z = 0;
    offset.z = 0xffebb000;
    shadowPos.x = 0;
    shadowPos.y = 0;
    shadowPos.z = 0;
    offset.x = 0;
    offset.y = 0;

    Matrix4x3_FromRotationY(&actor->mShadowMatrix, actor->mAngleY);
    MulVec3Mat4x3(&offset, &actor->mShadowMatrix, &shadowPos);
    AddVec3(&shadowPos, (Vector3 *)&actor->mPosX, &shadowPos);

    actor->mShadowMatrix.m[9] = shadowPos.x >> 3;
    actor->mShadowMatrix.m[10] = actor->mGroundY >> 3;
    actor->mShadowMatrix.m[11] = shadowPos.z >> 3;

    _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
        actor, &actor->mShadowModel, &actor->mShadowMatrix,
        0xdc000, 0x32000, 0x28a000, 0xf);
}
}

/* -------------------------------------------------------------------------- */
/*   _ZN16daObjCtMecha05_cD1Ev  0x0211ab60  size 0x4c  (complete-object)      */
/*   _ZN16daObjCtMecha05_cD0Ev  0x0211abac  size 0x60  (deleting)            */
/* -------------------------------------------------------------------------- */
/* No separate body lives here. The inline virtual destructor in the class
 * declaration and this TU's vtable instantiation make mwccarm emit retail's
 * D1 then D0 order, without the extra D2 produced by an out-of-line body. */
