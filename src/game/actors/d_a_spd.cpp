//cpp
/* daSpd_c, the spider enemy. ROM RTTI is daSpd_c; vtable 0x02122c2c.
 * daSpd_c_classInit is a reconstructed name (alias Scuttlebug_Spawn).
 * ~daSpd_c is inline in the header, so this TU emits D1 then D0
 * (0x0211f000, 0x0211f048) and no D2. OnYoshiTryEat is the key function,
 * so this TU emits _ZTV/_ZTI/_ZTS7daSpd_c; the manifest compares them
 * against the cartridge's copies and discards them.
 *
 * Source is REVERSE of ROM order (highest address first). Do not reorder.
 *
 * deslop leftovers:
 *  - The 25 state handlers/helpers keep func_ov071_* address names as member
 *    names; no original names are recovered. SetState was Scuttlebug_SetState.
 *  - The position temporaries are Vector3 (measured: same object as the old
 *    POD twin).
 *  - The extern "C" preamble is hand-spelt: Fix12<int>/Vector3 parameters
 *    are written as plain words where the true types break the register
 *    convention, and the spelled mangled calls stay where the member
 *    spelling would change codegen.
 *  - volatile int force_stack pads OnTurnIntoEgg's frame to the ROM shape.
 */

/* common.h first: its flat Matrix4x3 must win the guard; InitResources'
 * whole-matrix store at +0x350 only reproduces with that spelling. */
/* Runtime asset handles resolved through the overlay 0 file table. */
enum Scuttlebug_AssetId {
    ASSET_SPIDER_BMD = 0x0425,
    ASSET_SPIDER_WALK_BCA = 0x0426
};

#include "common.h"
#include "ParticleIds.h"
#include "daSpd_c.h"
#include "dBgCh_Gnd.h"
#include "daPly_c.h"
#include "SharedFilePtr.h"
#include "dExtFrameCtrl_c.h"
#include "SurfaceInfo.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "Sound.h"
#include "Particle__System.h"
#include "math/Approach.h"
#include "nitro/fx/fx_cp.h"

/* ------------------------------------------------------------------------
 * Local value shapes carried from the legacy one-function files.
 * ------------------------------------------------------------------------ */

/* Flat 0x30-byte matrix block, as func_ov071_0211f524 and the scratch matrix
 * at arm9:0x020a0e68 are used: whole-block assignment, never by field. */

typedef void (daSpd_c::*PMF)();

/* One row of the state table at ov071:0x02122fa8: enter handler, then the
 * per-frame run handler. The static initializer copies it from .data words. */
typedef struct { PMF enter, run; } ScuttlebugStateRow;

/* ------------------------------------------------------------------------
 * ABI imports.  One declaration per symbol, hand-reconciled: the generated
 * preamble carried these names with contradictory spellings from thirty-odd
 * source files.  The by-value Fix12<int> and Vector3 parameters are spelt as
 * plain words / pointers on purpose -- the true types are passed differently
 * and break the register convention.
 * ------------------------------------------------------------------------ */

extern "C" int AngleDiff(int a, int b);
extern "C" unsigned short DecIfAbove0_Short(void *p);
int   Vec3_Dist(const Vector3 *a, const Vector3 *b);
short Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);

extern "C" void Matrix4x3_FromRotationY(void *m, int angle);
extern "C" void Matrix4x3_ApplyInPlaceToTranslation(void *m, int x, int y, int z);
extern "C" void Matrix4x3_ApplyInPlaceToRotationX(void *m, s16 angX);

void func_02012694(unsigned int id, const Vector3 *pos);

/* dBgCh_Actr::Init's last two parameters are Vector3_16* (the S5_
 * back-references the pointer type). */
/* Parameter names keep this TU's @NNN. */
typedef void BgchInitAt_(void *self, dActor_c *actor, int radius, int height, Vector3_16 *a,
                         Vector3_16 *b);

extern Matrix4x3 IDENTITY_MATRIX4X3;
extern Matrix4x3 data_020a0e68;           /* scratch matrix        */

/* Intra-TU forward declarations.  mwccarm lays .text down in reverse source
 * order, so this file is written ROM-descending and nearly every intra-TU call
 * is a forward reference. */

/* The bug's file-scope SharedFilePtr objects, spelled through the same
   wrapper idiom the landed folds use: the handles' code is the veneer pair
   the cartridge already carries, so the wrappers get no bodies of their own.
   The constructors alias func_02017acc (model) and SharedFilePtr::Construct
   (animation) in this unit's manifest, and the destructors func_02017ab4
   and SharedFilePtr_Destruct_Anim. */
struct ScuttlebugModelFilePtr : SharedFilePtr {
    ScuttlebugModelFilePtr(unsigned int id);
    ~ScuttlebugModelFilePtr();
};

struct ScuttlebugAnimFileHandle : SharedFilePtr {
    ScuttlebugAnimFileHandle(unsigned int id);
    ~ScuttlebugAnimFileHandle();
};

/* ------------------------------------------------------------------------
 * The model handle, the animation handle and the state table's eighteen
 * member pointers (nine {enter, run} rows), in static-initializer order:
 * the initializer constructs each handle, registers its destructor, then
 * copies the pointers from .data. InitResources feeds the model to
 * dExtSimpleModel_c::LoadFile and the animation to dExtFrameCtrl_c::LoadFile.
 * ------------------------------------------------------------------------ */
ScuttlebugModelFilePtr   data_ov071_02122f80(ASSET_SPIDER_BMD);   /* model */
ScuttlebugAnimFileHandle gSpiderWalkAnimationFile(ASSET_SPIDER_WALK_BCA);   /* anim  */

/* The member pointers the initializer copies into the table
   (0x02122b60..0x02122bec in .data, an unclaimed run). Each record is one
   eight-byte {fn, adj} pair. */
extern PMF data_ov071_02122b88;
extern PMF data_ov071_02122b80;
extern PMF data_ov071_02122b68;
extern PMF data_ov071_02122b70;
extern PMF data_ov071_02122b78;
extern PMF data_ov071_02122b60;
extern PMF data_ov071_02122be8;
extern PMF data_ov071_02122be0;
extern PMF data_ov071_02122bd8;
extern PMF data_ov071_02122bd0;
extern PMF data_ov071_02122bc8;
extern PMF data_ov071_02122bc0;
extern PMF data_ov071_02122bb8;
extern PMF data_ov071_02122b90;
extern PMF data_ov071_02122bb0;
extern PMF data_ov071_02122ba8;
extern PMF data_ov071_02122ba0;
extern PMF data_ov071_02122b98;

PMF data_ov071_02122fa8[18] = {
    data_ov071_02122b88, data_ov071_02122b80, data_ov071_02122b68,
    data_ov071_02122b70, data_ov071_02122b78, data_ov071_02122b60,
    data_ov071_02122be8, data_ov071_02122be0, data_ov071_02122bd8,
    data_ov071_02122bd0, data_ov071_02122bc8, data_ov071_02122bc0,
    data_ov071_02122bb8, data_ov071_02122b90, data_ov071_02122bb0,
    data_ov071_02122ba8, data_ov071_02122ba0, data_ov071_02122b98,
};

daSpd_c *daSpd_c_classInit(void) {
    return new daSpd_c();
}

/* Pays the player mCoinCount coins (a cap coin if Yoshi is wearing the cap,
 * an egg coin otherwise). A child (param1 != 0) resets; the original dies. */

void daSpd_c::OnTurnIntoEgg(daPly_c &player) {
    volatile int force_stack;
    if (player.IsCollectingCap())
        GivePlayerCoins(player, mCoinCount, 0);
    else
        player.RegisterEggCoinCount(mCoinCount, 0, 0);
    if (param1 != 0) {
        mCoinCount = 0;
        mFlags &= ~0x40000;
        SetState(0);
    } else {
        MarkForDestruction();
    }
}

int daSpd_c::InitResources() {
    BMD_File *mf = dExtSimpleModel_c::LoadFile(data_ov071_02122f80);
    mAnmModel.SetFile(mf, 1, -1);
    dExtFrameCtrl_c::LoadFile(gSpiderWalkAnimationFile);
    if (mShadowModel.InitCylinder() == 0)
        return 0;
    mdCcAc_c.Init(this, 0x46000, 0x64000, 0x200000, 0x6eff0);
    mWithMeshClsn.Init(this, 0x50000, 0x50000, (Vector3_16 *)0, (Vector3_16 *)0);
    mWithMeshClsn.StartDetectingWater();
    mHomeX = mPosX;
    mHomeY = mPosY;
    mHomeZ = mPosZ;
    mHomeAngleY = mAngleY;
    mAnchorX = mPosX;
    mAnchorY = mPosY;
    mAnchorZ = mPosZ;
    if (param1 != 0)
        SetState(0);
    else
        SetState(2);
    mCoinCount = 3;
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    mParent = 0;
    mTimer = 0x3c;
    mShadowMat = IDENTITY_MATRIX4X3;
    func_ov071_0211f524();
    return 1;
}

int daSpd_c::Behavior() {
    DecIfAbove0_Short(&mTimer);
    func_ov071_02120278();
    MakeVanishLuigiWork(mdCcAc_c);
    if (mWithMeshClsn.GetResultFlag1() &&
        mWithMeshClsn.TouchesWater()) {
        func_ov071_0211f498();
    }
    func_ov071_0211f524();
    return 1;
}

int daSpd_c::Render() {
    int flag = (mFlags & 0x40000) ? 1 : 0;
    if (flag || !mState)
        return 1;
    /* dExtAnmModel_c slot 5. The call stays virtual. */
    mAnmModel.Render(0);
    return 1;
}

/* Empty: the override exists only to occupy vtable slot 12. */

void daSpd_c::OnPendingDestroy() {}

/* Releases the two shared files. Never touches `this`. */

int daSpd_c::CleanupResources() {
    data_ov071_02122f80.Release();
    gSpiderWalkAnimationFile.Release();
    return 1;
}

/* Points mStateRow at one row of the table and tail-calls its entry handler. */

void daSpd_c::SetState(int idx) {
    mStateRow = (ScuttlebugStateRow *)data_ov071_02122fa8 + idx;
    func_ov071_021202b4();
}

void daSpd_c::func_ov071_021202b4() {
    PMF *p = (PMF *)mStateRow;
    (this->**p)();
}

/* Call the state's per-frame handler, one slot further into the row. */

void daSpd_c::func_ov071_02120278() {
    PMF *p = (PMF *)mStateRow + 1;
    (this->**p)();
}

int daSpd_c::func_ov071_02120200() {
    int z;
    short ang;

    mFlags = mFlags & ~0x10000001;
    mPosX = mHomeX;
    z = 0;
    mPosY = mHomeY;
    mPosZ = mHomeZ;
    mPrevAngleY = mHomeAngleY;
    ang = mHomeAngleY;
    mAngleX = z;
    mAngleY = ang;
    mAngleZ = z;
    mHorzSpeed = z;
    mTimer = 0x1e;
    mdCcAc_c.Clear();
    mState = 0;
    return 1;
}

int daSpd_c::func_ov071_021201b4() {
    if (mTimer) return 1;
    if (DistToCPlayer() < 0x5dc000) SetState(1);
    return 1;
}

int daSpd_c::func_ov071_02120130() {
    mVertAccel = -0x4000;
    mTerminalVelocity = -0x3e000;
    mHorzSpeed = 0x16000;
    mVertSpeed = 0x4d000;
    mAnmModel.SetAnim((BCA_File *)gSpiderWalkAnimationFile.filePtr, 0, 0x1000, 0);
    mAnmModel.speed = 0x1000;
    mWithMeshClsn.SetLimMovFlag();
    func_0201267c(0xf1, &mCamSpacePos);
    mState = 1;
    return 1;
}

int daSpd_c::func_ov071_02120028() {
    mAnmModel.Advance();
    UpdatePos(&mdCcAc_c);
    dBgCh_Actr_UpdateDiscreteNoLava_veneer(&mWithMeshClsn);
    if (mWithMeshClsn.JustHitGround()) {
        Vector3 v = mPos;
        v.y += 0x28000;
        Particle::System::NewSimple(kPtclLandingDust, v.x, v.y, v.z);
        mVertSpeed = mVertSpeed * -0x28 / 100;
    } else if (mWithMeshClsn.IsOnGround()) {
        mVertSpeed = 0;
        mWithMeshClsn.ClearLimMovFlag();
        mAnchorX = mPosX;
        mAnchorY = mPosY;
        mAnchorZ = mPosZ;
        mFlags |= 0x10000001;
        SetState(2);
    }
    func_ov071_0211f29c();
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}

int daSpd_c::func_ov071_0211ff84() {
    if (Vec3_Dist(&mPos, (Vector3 *)&mAnchorX) > 0x5dc000) {
        SetState(5);
        return 1;
    }
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    mHorzSpeed = 0x4000;
    mAnmModel.SetAnim((BCA_File *)gSpiderWalkAnimationFile.filePtr, 0, 0x1000, 0);
    mAnmModel.speed = 0x1000;
    mState = 2;
    return 1;
}

int daSpd_c::func_ov071_0211fee4() {
    mAngleY = (short)(mAngleY + 0x2bc);
    mPrevAngleY = mAngleY;
    mAnmModel.Advance();
    unsigned short f = (unsigned short)(mAnmModel.currFrame >> 12);
    if (f == 0 || f == 8 || f == 0x17 || f == 0x1f) {
        func_0201267c(0xf0, &mCamSpacePos);
    }
    func_ov071_0211f0b4();
    UpdatePos(&mdCcAc_c);
    func_ov071_0211f148(&mWithMeshClsn);
    func_ov071_0211f29c();
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}

int daSpd_c::func_ov071_0211fe38() {
    int *p3a0 = &mLeapDist;
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    mHorzSpeed = 0xf000;
    mVertSpeed = 0x12000;
    mAngleY = mLeapAngle;
    mPrevAngleY = mAngleY;
    *p3a0 += 0x12c000;
    mAnmModel.SetAnim((BCA_File *)gSpiderWalkAnimationFile.filePtr, 0, 0x1000, 0);
    mAnmModel.speed = 0x2c00;
    mAnmModel.currFrame = 0;
    func_0201267c(0xf1, &mCamSpacePos);
    mState = 3;
    return 1;
}

int daSpd_c::func_ov071_0211fd58() {
    mAnmModel.Advance();
    int *p = &mLeapDist;
    *p -= 0xf000;
    if (mLeapDist <= 0) {
        mTimer = 0x3c;
        SetState(2);
    }
    UpdatePos(&mdCcAc_c);
    func_ov071_0211f148(&mWithMeshClsn);
    func_ov071_0211f29c();
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    if (mWithMeshClsn.IsOnGround() != 0) {
        unsigned int t = (unsigned int)(mAnmModel.currFrame << 4) >> 0x10;
        if ((t <= 2) || (t >= 8 && t <= 0xa) || (t >= 0x18 && t <= 0x1a) || (t >= 0x20 && t <= 0x22)) {
            func_0201267c(0xf0, &mCamSpacePos);
        }
    }
    return 1;
}

int daSpd_c::func_ov071_0211fcd4() {
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    mHorzSpeed = -0x4000;
    mVertSpeed = 0x12000;
    mAnmModel.SetAnim((BCA_File *)gSpiderWalkAnimationFile.filePtr, 0, 0x1000, 0);
    mAnmModel.speed = 0x2c00;
    func_0201267c(0xf1, &mCamSpacePos);
    mState = 4;
    return 1;
}

int daSpd_c::func_ov071_0211fc60() {
    mAnmModel.Advance();
    if (mWithMeshClsn.IsOnGround()) {
        mTimer = 0x3c;
        SetState(2);
    }
    UpdatePos(&mdCcAc_c);
    func_ov071_0211f148(&mWithMeshClsn);
    func_ov071_0211f29c();
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}

int daSpd_c::func_ov071_0211fbf4() {
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    mHorzSpeed = 0x4000;
    mAnmModel.SetAnim((BCA_File *)gSpiderWalkAnimationFile.filePtr, 0, 0x1000, 0);
    mAnmModel.speed = 0x1000;
    mState = 5;
    return 1;
}

int daSpd_c::func_ov071_0211fb24() {
    short ang = Vec3_HorzAngle(&mPos, (Vector3 *)&mAnchorX);
    ApproachLinear(mAngleY, ang, 0x2bc);
    mPrevAngleY = mAngleY;
    mAnmModel.Advance();
    if (Vec3_Dist(&mPos, (Vector3 *)&mAnchorX) < 0x12c000)
        SetState(2);
    func_ov071_0211f0b4();
    UpdatePos(&mdCcAc_c);
    func_ov071_0211f148(&mWithMeshClsn);
    func_ov071_0211f29c();
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    unsigned short v = (unsigned short)(mAnmModel.currFrame >> 0xc);
    if (v == 0 || v == 8 || v == 0x17 || v == 0x1f)
        func_0201267c(0xf0, &mCamSpacePos);
    return 1;
}

int daSpd_c::func_ov071_0211fb0c() {
    mHorzSpeed = 0;
    mState = 6;
    return 1;
}

int daSpd_c::func_ov071_0211fa54() {
    int b0 = (int)((mFlags & 0x40000) != 0);
    if (b0 != 0) {
        int *src = &mParent->mPosX;
        mPosX = src[0];
        mPosY = src[1];
        mPosZ = src[2];
    }
    {
        int v = mFlags;
        int b1 = (int)((v & 0x80000) != 0);
        if (b1 != 0) {
            SetState(7);
        } else {
            int b2 = (int)((v & 0x20000) != 0);
            if (b2 == 0) {
                int b3 = (int)((v & 0x40000) != 0);
                if (b3 == 0) {
                    mParent = 0;
                    SetState(2);
                }
            }
        }
    }
    mdCcAc_c.Clear();
    return 1;
}

int daSpd_c::func_ov071_0211f8d0() {
    mFlags &= 0xfff7fffe;
    mHorzSpeed = mParent->mHorzSpeed + 0x7000;
    mVertSpeed = 0;
    mAngleY = mParent->mAngleY;
    mPrevAngleY = mAngleY;
    mPos = mParent->mPos;
    mPosX += FX_MUL(FX_SinIdx((u16)mAngleY >> 4), 0x5a000);
    mPosY += 0x50000;
    mPosZ += FX_MUL(FX_CosIdx((u16)mAngleY >> 4), 0x5a000);
    Vector3 v(mParent->mPosX, mParent->mPosY + 0x50000, mParent->mPosZ);
    DetectRaycastClsn(v, mPos, 1);
    mParent = 0;
    mWithMeshClsn.SetLimMovFlag();
    mState = 7;
    return 1;
}

int daSpd_c::func_ov071_0211f7d4() {
    dBgCh_Actr_UpdateDiscreteNoLava_veneer(&mWithMeshClsn);
    mAngleX = mAngleX + 0x1000;
    if (mWithMeshClsn.JustHitGround()) {
        if (((SurfaceInfo *)((char *)mWithMeshClsn.GetFloorResult() + 4))->clps.GetSurfaceType() ==
            4) {
            func_ov071_0211f498();
        } else {
            mVertSpeed = (mVertSpeed * -0x3c) / 0x64;
        }
    } else if (mWithMeshClsn.IsOnGround()) {
        mVertSpeed = 0;
        mWithMeshClsn.ClearLimMovFlag();
        mFlags |= 1;
        short z = 0;
        short ang = mPrevAngleY;
        mAngleX = z;
        mAngleY = ang;
        mAngleZ = z;
        SetState(2);
    }
    UpdatePos(&mdCcAc_c);
    func_ov071_0211f29c();
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}

int daSpd_c::func_ov071_0211f6f8() {
    Sound::PlayBank0(9, mCamSpacePos);
    mFlags &= ~1;
    mHorzSpeed = 0xa000;
    mVertSpeed = 0x28000;
    mTimer = 0x2d;
    mAnmModel.SetAnim((BCA_File *)gSpiderWalkAnimationFile.filePtr, 0, 0x1000, 0);
    mAnmModel.speed = 0x4000;
    int yOffset1 = OnAimedAtWithEgg();
    Particle::System::NewSimple(kPtclKnockAwayA, mPosX, mPosY + yOffset1, mPosZ);
    int yOffset2 = OnAimedAtWithEgg();
    Particle::System::NewSimple(kPtclKnockAwayB, mPosX, mPosY + yOffset2, mPosZ);
    mState = 8;
    return 1;
}

int daSpd_c::func_ov071_0211f694() {
    mAngleX = mAngleX - 0x1000;
    mAnmModel.Advance();
    UpdatePos(&mdCcAc_c);
    dBgCh_Actr_UpdateDiscreteNoLava_veneer(&mWithMeshClsn);
    if (mWithMeshClsn.JustHitGround() != 0 || mTimer == 0)
        func_ov071_0211f498();
    return 1;
}

void daSpd_c::func_ov071_0211f524() {
    int b = (int)((mFlags & 0x40000) != 0);
    if (b) {
        if (mState == 0)
            return;
    }

    Matrix4x3_FromRotationY(&mAnmModel.mat4x3, mAngleY);
    mAnmModel.mat4x3.m[9] = mPosX >> 3;
    mAnmModel.mat4x3.m[10] = mPosY >> 3;
    mAnmModel.mat4x3.m[11] = mPosZ >> 3;

    if (mAngleX != 0) {
        data_020a0e68 = mAnmModel.mat4x3;
        int y1 = OnAimedAtWithEgg() >> 3;
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, y1, 0);
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, mAngleX);
        int y2 = (-OnAimedAtWithEgg()) >> 3;
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, y2, 0);
        mAnmModel.mat4x3 = data_020a0e68;
    }

    mShadowMtx[9] = mPosX >> 3;
    mShadowMtx[10] = mPosY >> 3;
    mShadowMtx[11] = mPosZ >> 3;

    int dh = (mState == 8) ? 0x190000 : 0xc8000;
    DropShadowRadHeight(mShadowModel, mShadowMat, 0xa0000, dh, 0xf);
}

void daSpd_c::func_ov071_0211f498() {
    Vector3 v;
    v.x = mPosX;
    v.y = mPosY;
    v.z = mPosZ;
    SpawnCoins(v, mCoinCount, 0xf000, 0);
    PoofDust();
    func_02012694(0xc4, (const Vector3 *)(&mCamSpacePosX));
    if (param1) {
        mCoinCount = 0;
        SetState(0);
        return;
    }
    MarkForDestruction();
}

void daSpd_c::func_ov071_0211f29c() {
    daPly_c *hitPlayer;
    int b;

    if (FindEgg(mdCcAc_c) != 0) {
        Sound::PlayBank0(9, mCamSpacePos);
        func_ov071_0211f498();
        return;
    }

    {
        unsigned int id = mdCcAc_c.otherOwner;
        if (id == 0)
            return;
        hitPlayer = (daPly_c *)dActor_c::FindWithID(id);
    }
    if (hitPlayer == 0)
        return;

    b = (int)(hitPlayer->actorID == 0xbf);
    if (b == 0)
        return;

    b = (int)((mFlags & 0x20000) != 0);
    if (b != 0) {
        SetState(6);
        return;
    }

    if ((mdCcAc_c.hitFlags & 0x66fe0)
        || hitPlayer->IsOnShell() != 0
        || hitPlayer->mIsMetal != 0) {
        Sound::PlayBank0(9, mCamSpacePos);
        func_ov071_0211f498();
        return;
    }

    if (mdCcAc_c.hitFlags & 0x10) {
        mPrevAngleY = Vec3_HorzAngle(&hitPlayer->mPos, &mPos);
        mAngleY = (short)(mPrevAngleY + 0x8000);
        hitPlayer->IncMegaKillCount();
        SetState(8);
        return;
    }

    if (JumpedOnByPlayer(mdCcAc_c, *hitPlayer) != 0) {
        hitPlayer->Bounce(0x28000);
        func_ov071_0211f498();
        return;
    }

    if (mState == 7)
        return;

    {
        Vector3 v;
        v.x = mPosX;
        v.y = mPosY;
        v.z = mPosZ;
        if (hitPlayer->Hurt(v, 1, 0xc000, 1, 0, 1) != 0)
            SetState(4);
    }
}

void daSpd_c::func_ov071_0211f148(dBgCh_Actr *w) {
    Vector3 pos;
    Vector3 normal;
    Vector3 wallnormal;

    dBgCh_Actr_UpdateDiscreteNoLava_veneer(w);
    if (w->IsOnGround()) {
        dBgCh_Gnd rc;
        {
            int p60 = mPosY;
            int pz = mPosZ;
            int py = p60 + 0x1e000;
            pos.x = mPosX;
            pos.y = py;
            pos.z = pz;
        }
        rc.SetObjAndPos(pos, this);
        if (!rc.DetectClsn() || rc.clsnY < mPosY - 0x32000) {
            mHorzSpeed = 0;
            mPosX = mPrevPosX;
            mPosY = mPrevPosY;
            mPosZ = mPrevPosZ;
        } else {
            void *fr = w->GetFloorResult();
            ((SurfaceInfo *)((char *)fr + 4))->CopyNormalTo(normal);
            if (normal.y != 0) {
                mVertSpeed =
                    -(FX_Div(FX_MUL(normal.x, mSpeedX) + FX_MUL(normal.z, mSpeedZ), normal.y) +
                      0x8000);
            }
        }
    }
    if (w->IsOnWall()) {
        void *wr = w->GetWallResult();
        ((SurfaceInfo *)((char *)wr + 4))->CopyNormalTo(wallnormal);
    }
}

void daSpd_c::func_ov071_0211f0b4() {
    dActor_c *p;
    Fix12i d;
    short ang;
    if (mTimer != 0) return;
    p = (dActor_c *)ClosestNonVanishPlayer();
    if (p == 0) return;
    d = Vec3_Dist(&mPos, &p->mPos);
    if (d > 0x5dc000) return;
    ang = Vec3_HorzAngle(&mPos, &p->mPos);
    if (AngleDiff(ang, mAngleY) > 0x12c) return;
    mLeapDist = d;
    mLeapAngle = ang;
    SetState(3);
}

/* Vtable slot 29. Ignores `this` and returns a constant. */

int daSpd_c::OnAimedAtWithEgg() {
    return 204800;
}

/* Vtable slot 18. First non-inline virtual, so this is the key function:
 * the vtable is emitted with it and externalized (see the banner). */

int daSpd_c::OnYoshiTryEat() {
    return 6;
}
