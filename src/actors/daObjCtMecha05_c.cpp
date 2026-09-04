//cpp
/* Production translation unit for ov065/daObjCtMecha05_c.
 *
 * mwccarm emits ordinary functions in reverse source order, so the eight
 * definitions below intentionally run from the highest retail address back
 * toward the compiler-owned destructor group. The adjacent C ABI factory is
 * independently enrolled in src/d_a_obj_ct_mecha05.cpp; adjacency alone is
 * not evidence that it belonged to this class TU.
 *
 * Superseded one-function sources (ROM address order):
 *   [0] 0x0211ab60  src/_ZN16daObjCtMecha05_cD1Ev.cpp
 *   [1] 0x0211abac  src/_ZN16daObjCtMecha05_cD0Ev.cpp
 *   [2] 0x0211ac0c  src/func_ov065_0211ac0c.c
 *   [3] 0x0211ad04  src/_ZN16daObjCtMecha05_c16CleanupResourcesEv.cpp
 *   [4] 0x0211ad48  src/_ZN16daObjCtMecha05_c6RenderEv.cpp
 *   [5] 0x0211ad70  src/func_ov065_0211ad70.c
 *   [6] 0x0211ae08  src/_ZN16daObjCtMecha05_c8BehaviorEv.cpp
 *   [7] 0x0211b1d4  src/_ZN16daObjCtMecha05_c13InitResourcesEv.cpp
 */

#include "daObjCtMecha05_c.h"
#include "dBgCh_Gnd.h"
#include "SharedFilePtr.h"

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

/* The remaining C ABI declarations are measured call seams or genuinely
 * address-named free helpers. In particular, the Fix12-by-value methods stay
 * ABI-shaped because their natural declarations trigger mwccarm stack homing
 * absent from retail. */
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
extern void _ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_();
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

extern "C" daObjCtMecha05_c *daObjCtMecha05_c_classInit();

/* This descriptor belongs to the class data group even though the adjacent
 * factory is kept as a separate, independently enrolled source. */
/* CT_MECHA05 is the literal ROM registry ID. The g_profile spelling is a
 * lineage-supported reconstruction; historical alias TTC_MovingBar_SpawnInfo. */
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
/* ROM ordinal 7 -- _ZN16daObjCtMecha05_c13InitResourcesEv, 0x0211b1d4, size 0x154 */
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
        (void*)&_ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
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
/* ROM ordinal 6 -- _ZN16daObjCtMecha05_c8BehaviorEv, 0x0211ae08, size 0x3cc */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjCtMecha05_c8BehaviorEv
/* mHorzSpeed is reused here as a fix12 travel accumulator rather than a
 * physical speed. */
int daObjCtMecha05_c::Behavior()
{
    if (data_0209f2c0 == 3) {
        int ang;
        /* Read UNSIGNED here -- ldrh, not the ldrsh a plain `s16 mPrevAngleY`
           read would emit -- matching the ROM exactly even though mPrevAngleY
           itself is signed (dActor_c.h, evidenced elsewhere). */
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
/* ROM ordinal 5 -- func_ov065_0211ad70, 0x0211ad70, size 0x98 */
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
/* ROM ordinal 4 -- _ZN16daObjCtMecha05_c6RenderEv, 0x0211ad48, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjCtMecha05_c6RenderEv
int daObjCtMecha05_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN16daObjCtMecha05_c16CleanupResourcesEv, 0x0211ad04, size 0x44 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjCtMecha05_c16CleanupResourcesEv
/* The two bss slots released here are ROM-proven by ADDRESS, not by name.
   0x0211d904 and 0x0211d90c appear as literal-pool words in the cartridge's
   overlay_0065.bin (file offsets 0x4e60/0x4e64, 0x542c/0x5430, 0x69fc/0x6a0c),
   so the overlay really does share these two slots with the neighbouring class
   -- that is fact, not a naming error carried over from it.
   The SPELLING TTC_MovingBar_* is project convention, not the cartridge's: the
   string "TTC_MovingBar" occurs nowhere in arm9, arm7 or any of the 103 overlay
   binaries. These names are assigned by us, at those two addresses, in
   config/arm9/overlays/ov065/symbols.txt. Read them as labels, not evidence. */
int daObjCtMecha05_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    TTC_MovingBar_ModelFile.Release();
    TTC_MovingBar_ClsnFile.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov065_0211ac0c, 0x0211ac0c, size 0xf8 */
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
/* ROM ordinals 1 and 0 -- compiler-owned destructor variants.                */
/*   _ZN16daObjCtMecha05_cD1Ev  0x0211ab60  size 0x4c  (complete-object)      */
/*   _ZN16daObjCtMecha05_cD0Ev  0x0211abac  size 0x60  (deleting)            */
/* -------------------------------------------------------------------------- */
/* No separate body lives here. The inline virtual destructor in the class
 * declaration and this TU's vtable instantiation make mwccarm emit retail's
 * D1 then D0 order, without the extra D2 produced by an out-of-line body. */
