//cpp
/* Production translation unit for ov065/daObjCtMecha05_c.
 *
 * What stays, and why:
 * - dBgW_KcMbg::SetFile / DropShadowScaleXYZ / dBgActor_c::IsClsnInRange
 *   are header member calls whose headers spell Fix12 as Fix12i, a plain
 *   s32 (6az)
 * - func_020393d4 stores dBgW+0x18 (beforeClsnCallback); dBgW::SetClipRadius
 *   stores +0x0c; dBgW::SetClipOffsetY stores +0x10
 * - data_ov065_0211c0c8 / data_ov065_0211d26c; TTC_MovingBar_ModelFile /
 *   TTC_MovingBar_ClsnFile are symbols.txt BSS labels shared with the
 *   neighbouring class
 * - common.h first (UpdateShadow writes mShadowMatrix.m[9..11])
 * - return new emits homeless _ZN10dBgActor_cD2Ev; compiler-only policy
 *   deadstrips it
 *
 * mwccarm emits ordinary functions in reverse source order, so the nine
 * definitions below intentionally run from the highest retail address back
 * toward the compiler-owned destructor group. Keep the factory first.
 *
 * The factory is `return new` in this file.
 *
 * S41: D1 0x0211ab60 size 0x4c, D0 0x0211abac size 0x60; no separate body,
 * inline virtual destructor + vtable emits D1 then D0, no D2.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daObjCtMecha05_c_AssetId {
    ASSET_CT_MECHA_OBJ05_BMD = 0x05c2,
    ASSET_CT_MECHA_OBJ05_KCL = 0x05c3
};

#include "common.h"
#include "daObjCtMecha05_c.h"
#include "SpawnInfo.h"
#include "dBgCh_Gnd.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "nitro/fx/fx.h"
#include "math/CalcLib.h"
#include "math/MtxStack.h"

/* File-scope objects at the end of this file construct the two resource
 * handles (model file 0x5c2, collision file 0x5c3). mwcc emits
 * __sinit_d_a_obj_ct_mecha05.cpp from those definitions. The wrapper names
 * are local; the handle constructors and destructors are the ROM
 * resource-family functions, aliased in the manifest. The definitions
 * carry the TU's TTC_MovingBar_* names the code already uses; the retired
 * shard's data_ov065_* labels for the same addresses are dropped from
 * symbols.txt, and their stale decl_common.h lines with them (S42). */
struct Mecha05ModelFile : SharedFilePtr {
    Mecha05ModelFile(u32 fileID);
    ~Mecha05ModelFile();
};

struct Mecha05CollisionFilePtr : SharedFilePtr {
    Mecha05CollisionFilePtr(u32 fileID);
    ~Mecha05CollisionFilePtr();
};

extern Mecha05ModelFile TTC_MovingBar_ModelFile;
extern Mecha05CollisionFilePtr TTC_MovingBar_ClsnFile;

extern u8 data_0209f2c0;
extern int data_0209e650;
extern s16 data_02082214[];
extern s16 data_ov065_0211c0c8[];
extern s8 data_ov065_0211d26c[];
extern CLPS_Block data_ov035_02112258;

/* local extern: arm9 vector helpers and the dBgW stores; no shared header. */
extern "C" {
extern void MulVec3Mat4x3(const Vector3 *src, const Matrix4x3 *matrix, Vector3 *dst);
extern void AddVec3(Vector3 *left, Vector3 *right, Vector3 *dst);
extern void func_020393d4(dBgW *collider, void *callback);
}

/* CT_MECHA05 is the literal ROM registry ID. The g_profile spelling is a
 * lineage-supported reconstruction. The descriptor is at ov065:0x0211d290. */
extern "C" daObjCtMecha05_c *daObjCtMecha05_c_classInit();
extern "C" SpawnInfo<daObjCtMecha05_c> g_profile_CT_MECHA05 = {
    daObjCtMecha05_c_classInit, 0x0071, 0x00ec, 0x00000002, 0, 0x00600000, 0x01000000, 0x02000000};

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol daObjCtMecha05_c_classInit
extern "C" daObjCtMecha05_c *daObjCtMecha05_c_classInit() {
    return new daObjCtMecha05_c();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjCtMecha05_c13InitResourcesEv
int daObjCtMecha05_c::InitResources() {
    void *modelFile = dExtSimpleModel_c::LoadFile(TTC_MovingBar_ModelFile);
    mModel.SetFile((BMD_File *)modelFile, 1, -1);
    mShadowModel.InitCuboid();
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    void *collisionFile = dBgW_Kc::LoadFile(TTC_MovingBar_ClsnFile);
    mMeshCollider.SetFile((KCL_File *)collisionFile, mClsnMat, 0x199, mAngleY, data_ov035_02112258);
    func_020393d4(&mMeshCollider, (void *)&dBgW::UpdatePosWithTransform);
    mMeshCollider.SetClipRadius(0x1c0000);
    mMeshCollider.SetClipOffsetY(0x1000);
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
int daObjCtMecha05_c::Behavior() {
    if (data_0209f2c0 == 3) {
        int ang;
        /* ldrh, not the ldrsh a plain s16 mPrevAngleY read would emit. */
        ang = *(u16 *)&mPrevAngleY;
        mPosX = mHomePosX + FX_MUL(data_02082214[(ang >> 4) << 1], 0xfa000);
        ang = *(u16 *)&mPrevAngleY;
        mPosZ = mHomePosZ + FX_MUL(data_02082214[((ang >> 4) << 1) + 1], 0xfa000);
        UpdateModelPosAndRotY();
        UpdateShadow();
        if (IsClsnInRange(0, 0) != 0)
            UpdateClsnPosAndRot();
        return 1;
    }

    mPrevTravel = mTravel;
    mTravel += mHorzSpeed;

    switch (mState) {
    case 0:
        if (DecIfAbove0_Short((u16 *)&mStateTimer) != 0)
            goto Lend;
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
            int val = FX_MUL((0xfa000 - d), (0xfa000 - mPrevTravel));
            if (val >= 0)
                goto L280;
        }
        {
            int sp = mHorzSpeed;
            if (sp <= -0x8000)
                goto L280;
            if (sp >= 0x8000)
                goto L280;
            mState++;
            mHorzSpeed = 0;
            mStateTimer = 0x1e;
            goto Lend;
        }
    L280: {
        int m = (mTravel < 0xfa000) ? 0x6666 : -0x6666;
        int sp = mHorzSpeed;
        int delta;
        if (FX_MUL(sp, m) >= 0)
            delta = m;
        else
            delta = FX_MUL(m, 0x2599);
        mHorzSpeed += delta;
        if (data_0209f2c0 != 2)
            goto Lend;
    }
        if (FX_MUL(mTravel, mPrevTravel) >= 0)
            goto Lend;
        if ((RandomIntInternal(&data_0209e650) & 3) != 0)
            goto Lend;
        ResetToHome();
        goto Lend;
    }

    case 3:
        if (DecIfAbove0_Short((u16 *)&mStateTimer) != 0)
            goto Lend;
        mHorzSpeed = -0x5000;
        if (mTravel < 0)
            ResetToHome();
        goto Lend;
    }

Lend:
    UpdatePos(0);
    UpdateModelPosAndRotY();
    UpdateShadow();
    if (IsClsnInRange(0x1c0000, 0x1000) != 0)
        UpdateClsnPosAndRot();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjCtMecha05_c11ResetToHomeEv
void daObjCtMecha05_c::ResetToHome() {
    mState = 0;
    mTravel = 0;
    mHorzSpeed = 0;
    mPosX = mHomePosX;
    mPosY = mHomePosY;
    mPosZ = mHomePosZ;

    u8 setting = data_0209f2c0;
    mStateTimer = data_ov065_0211c0c8[setting];
    if (setting != 2)
        return;

    mStateTimer = data_ov065_0211d26c[(u16)((u32)RandomIntInternal(&data_0209e650) >> 16) & 3];
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjCtMecha05_c6RenderEv
int daObjCtMecha05_c::Render() {
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjCtMecha05_c16CleanupResourcesEv
int daObjCtMecha05_c::CleanupResources() {
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    TTC_MovingBar_ModelFile.Release();
    TTC_MovingBar_ClsnFile.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjCtMecha05_c12UpdateShadowEv
void daObjCtMecha05_c::UpdateShadow() {
    int distance = mPosY - mGroundY;
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

    Matrix4x3_FromRotationY(&mShadowMatrix, mAngleY);
    MulVec3Mat4x3(&offset, &mShadowMatrix, &shadowPos);
    AddVec3(&shadowPos, &mPos, &shadowPos);

    mShadowMatrix.m[9] = shadowPos.x >> 3;
    mShadowMatrix.m[10] = mGroundY >> 3;
    mShadowMatrix.m[11] = shadowPos.z >> 3;

    DropShadowScaleXYZ(mShadowModel, mShadowMatrix, 0xdc000, 0x32000, 0x28a000, 0xf);
}
// @symbol _ZN16daObjCtMecha05_cD1Ev
// @symbol _ZN16daObjCtMecha05_cD0Ev
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* No separate body lives here. The inline virtual destructor in the class
 * declaration and this TU's vtable instantiation make mwccarm emit retail's
 * D1 then D0 order, without the extra D2 produced by an out-of-line body. */

/* Static-init globals (was the handwritten __sinit_ov065_0211c890 shard).
 * Definition order is the retail initializer construction order. These
 * carry the TU's TTC_MovingBar_* names the code already uses; the retired
 * shard's data_ov065_* labels for the same addresses are dropped. */
Mecha05ModelFile TTC_MovingBar_ModelFile(ASSET_CT_MECHA_OBJ05_BMD);
Mecha05CollisionFilePtr TTC_MovingBar_ClsnFile(ASSET_CT_MECHA_OBJ05_KCL);
