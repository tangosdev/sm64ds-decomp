//cpp
/* daBtfly_c -- butterfly (actor 0x150) on ov100. Kind 0 is the spawner:
 * State4 releases three more of actor 0x150, and kind 1 turns into a 1-Up
 * (actor 0x114) in State6. Behavior dispatches State0..State7 through
 * data_ov100_02148628, a pointer-to-member table.
 *
 * `#pragma defer_codegen off` is load-bearing. It makes the
 * `#pragma opt_common_subs` bracket around State2 bind (without the pair
 * State2 is 8 bytes short of 0x244) and it emits .text in source order, so
 * this file is ROM-ascending: D1, D0, State7..State0, CleanupResources,
 * OnPendingDestroy, Render, Behavior, InitResources, classInit
 * (0x02140d80..0x02141f04). Do not reorder the functions without the pragma.
 *
 * One ~daBtfly_c() emits D1 and D0 together. A second hand-mangled D0 beside
 * it is the mwccarm ICE (ELFgen.c:483).
 *
 * daBtfly_c_classInit (0x02141ea4..0x02141f04, historical alias
 * Butterfly_Spawn) appends after InitResources, at the end of source order:
 * the pragma above never toggles back on, so it stays in effect for this
 * new function too. fBase_c::operator new(size_t) forwards
 * `return new daBtfly_c();` to the same _ZN7fBase_cnwEm(1012) allocator the
 * loose factory called by hand, and daBtfly_c has no user-declared
 * constructor, so the inherited dActor_c ctor plus the vtable store plus the
 * six member subobjects in field order (dExtAnmModel_c, dExtSimpleModel_c, dExtShadowModel_c x2,
 * dBgCh_Actr, dCcAcPos_c) come from the implicit default constructor with
 * zero mangled calls.
 *
 * deslop leftovers:
 * - State7 mFlutterPhase: named += is 0x234 vs 0x258. The phase load is
 *   ldrsh plus a zero-extend shift (ldrh of the field is shorter), and the
 *   adds take the int-cast address, not add #0x300.
 * - State7 Player::Hurt: method form is 0x264 vs 0x258 (+0xc). Fix12<int>
 *   by value. Player.h does not declare Hurt.
 * - State7 UpdateContinuous(): WRONG-DEST. ROM calls
 *   dBgCh_Actr_UpdateContinuous_Veneer (0x020383fc).
 * - State7 func_02035638 is mClsnFlags & 0x10. No method; IsOnGround is
 *   mFlags & 0x10.
 * - Behavior DropShadowRadHeight: method form is 0x24c vs 0x22c (+0x20,
 *   both calls). Fix12<int> by value.
 * - Behavior scratch matrix: Matrix4x3 assignment is 0x264 vs 0x22c.
 *   Flat Mtx copy is the ROM ldm/stm; the real type's translation is a
 *   Vector3 and runs ~Vector3.
 * - State0: building the angle triple before reading param1 differs by
 *   10 words.
 * - daBtfly_c_classInit and g_profile_BUTTERFLY sit past this text run.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daBtfly_c_AssetId {
    ASSET_BUTTERFLY_BMD = 0x043a,
    ASSET_BUTTERFLY_FLY_BCA = 0x043b
};

#include "daBtfly_c.h"
#include "types.h"
#include "common.h"
#include "decl_common.h"
#include "decl_Actor.h"
#include "SharedFilePtr.h"
#include "Player.h"
#include "nitro/fx/fx.h"
#include "math/Approach.h"

/* Makes the per-member `#pragma opt_common_subs` bracket around State2 bind.
 * Without it mwccarm defers codegen and the last file-global state wins. */
#pragma defer_codegen off

/* Flat 12-word copy. Assigning Matrix4x3 would run ~Vector3 on its translation. */
struct Mtx { int w[12]; };

/* No destructor. dCcAcPos_c::Init's offset argument is passed as a pointer;
 * a stack Vector3 here would emit ~Vector3. */
struct Vec3 { s32 x, y, z; };

typedef void (daBtfly_c::*ButterflyState)();

/* File-scope objects at the end of this file construct the two resource
 * handles (model file 0x43a, animation file 0x43b) and fill the eight-entry
 * state table. mwcc emits __sinit_d_a_btfly.cpp from those definitions. */
struct BtflyModelFilePtr : SharedFilePtr {
    BtflyModelFilePtr(u32 fileID);
    ~BtflyModelFilePtr();
};

struct BtflyAnimationFilePtr : SharedFilePtr {
    BtflyAnimationFilePtr(u32 fileID);
    ~BtflyAnimationFilePtr();
};

/* State7's flutter-phase add. A plain mFlutterPhase += folds to add #0x300;
 * the int cast keeps the pool-offset add the ROM has. */
#define L(p) ((int)(p))

extern "C" {
s16 Vec3_VertAngle(const Vector3 *v1, const Vector3 *v0);
s16 Vec3_HorzAngle(const void *a, const void *b);
void Vec3_Sub(Vector3 *out, void *a, void *b);
int Vec3_HorzLen(Vector3 *v);
void Vec3_Asr(void *d, void *s, int sh);
void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationY(void *m, s16 ang);
int RandomIntInternal(int *seed);
extern int data_0209e650;
extern s16 data_02082214[]; /* sin/cos; index (u16 angle >> 4) * 2 */
extern Matrix4x3 data_020a0e68; /* scratch matrix */
/* Init LoadFile: ov002 BMD shared with the other small fauna, this overlay's
 * BCA, the still-model BMD, and the animated-model BMD. Cleanup Release. */
extern SharedFilePtr data_ov002_0210d9d8;
extern BtflyAnimationFilePtr gButterflyFlyAnimationFile;
extern SharedFilePtr data_ov100_02148668;
extern BtflyModelFilePtr gButterflyModelFile;
extern ButterflyState data_ov100_02148628[];

/* Fix12<int> by value. The header method form homes those arguments on the
 * stack; these scalar entries are the ROM calls. */
/* Parameter names keep this TU's @NNN. */
typedef void BgchInitAt_(void *w, void *actor, int radius, int height, void *a, void *b);
}

// @symbol _ZN9daBtfly_cD1Ev
// @symbol _ZN9daBtfly_cD0Ev
/* One ~daBtfly_c() emits both D1 and D0. */
daBtfly_c::~daBtfly_c()
{
}

// @symbol _ZN9daBtfly_c6State7Ev
void daBtfly_c::State7()
{
    char *c = (char *)this; /* unused, but removing it renumbers the @NNN labels */
    int hasContact;
    int v;

    /* Read mFlutterPhase signed, then zero-extend: a single (u16) read
     * shortens State7 by 8 bytes. */
    if (mStateTimer > 0x78) {
        int ang = mFlutterPhase;
        ang = (unsigned short)(short)ang;
        int idx = ang >> 4;
        v = FX_MUL((int)data_02082214[(idx << 1) + 1], 0xa3);
        if (v > 0) {
            v = FX_MUL(v, 0x4800);
            *(u16 *)L(&mFlutterPhase) += 0x2710;
        } else {
            *(u16 *)L(&mFlutterPhase) += 0xfa0;
        }
        mScale += v;
    }

    ApproachLinear(mHorzSpeed, 0x14000, 0x1000);

    Player *player = ClosestPlayer();
    if (player != 0) {
        ApproachLinear(mPrevAngleY, HorzAngleToCPlayer(), 0x320);
        ApproachLinear(mPrevAngleX, Vec3_VertAngle(&mPos, &player->mPos), 0x320);
    }

    dBgCh_Actr_UpdateContinuous_Veneer(&mWithMeshClsn);

    {
        int noId = (mdCcAcPos_c.otherOwner == 0);
        hasContact = (noId == 0);
    }
    if (hasContact == 0) {
        if (mWithMeshClsn.IsOnWall() == 0) {
            if (mWithMeshClsn.IsOnGround() == 0) {
                /* mClsnFlags & 0x10. No accessor; IsOnGround is mFlags & 0x10. */
                if (func_02035638((u8 *)&mWithMeshClsn) == 0) {
                    if (mStateTimer <= 0x9d)
                        goto cylinder_only;
                }
            }
        }
    }

    if (hasContact != 0) {
        dActor_c *hit = dActor_c::FindWithID(mdCcAcPos_c.otherOwner);
        if (hit != 0) {
            int isPlayer = (hit->actorID == 0xbf);
            if (isPlayer != 0) {
                Vector3 pos;
                pos.x = mPosX;
                pos.y = mPosY;
                pos.z = mPosZ;
                ((Player *)hit)->Hurt(pos, 2, 0xc000, 1, 0, 1);
            }
        }
    }

    TriplePoofDust();
    MarkForDestruction();

cylinder_only:
    mdCcAcPos_c.Clear();
    {
        Vector3 off;
        off.x = 0;
        off.y = -0x32000;
        off.z = 0;
        mdCcAcPos_c.SetPosRelativeToActor(off);
    }
    mdCcAcPos_c.Update();
}

// @symbol _ZN9daBtfly_c6State6Ev
void daBtfly_c::State6()
{
    if (mStateTimer == 0x14) {
        if (mKind != 1) {
            mScale = 0;
            mUseAnimModel = 0;
            return;
        }
        dActor_c::Spawn(0x114, 0, mPos, 0, mAreaId, -1);
        MarkForDestruction();
        return;
    }
    if (mStateTimer <= 0x14)
        return;
    mScale += 0x40;
    if (mScale < 0x800)
        return;
    SetRanges(0x32000, 0x32000, 0x1000000, 0x320000);
    mFlutterPhase = 0;
    mStateTimer = 0;
    mState = 7;
}

// @symbol _ZN9daBtfly_c6State5Ev
void daBtfly_c::State5()
{
    s16 hAngle;

    if (!IsPlayerInRange(0x5dc)) {
        MarkForDestruction();
        return;
    }

    if (mStateTimer > 0x6e && DistToCPlayer() < 0xc8000 &&
        (unsigned char)(mKind + 0xff) <= 1) {
        mHorzSpeed = 0;
        mStateTimer = 0;
        mState = 6;
        mFlags &= ~0x10000;
    } else {
        ApproachLinear(mHorzSpeed, 0x8000, 0x800);
    }

    if (mStateTimer >= 0x3c)
        hAngle = mWanderAngle;
    else
        hAngle = Vec3_HorzAngle(&mPosX, &mHomePosX);

    {
        s16 target;
        if (mPosY < mHomePosY +
                (int)(((unsigned)RandomIntInternal(&data_0209e650) >> 16 & 0xfff) * 0x32 + 0x32000))
            target = -0x2000;
        else
            target = 0x2000;
        ApproachLinear(mPrevAngleX, target, 0x190);
    }

    ApproachLinear(mPrevAngleY, hAngle,
        (s16)((((unsigned)RandomIntInternal(&data_0209e650) >> 16) % 800) + 0x190));
}

// @symbol _ZN9daBtfly_c6State4Ev
void daBtfly_c::State4()
{
    int typ = param1 & 0xff;
    if ((typ & 0xc0) != 0) {
        mKind = (u8)(typ >> 6);
    } else {
        if (DistToCPlayer() >= 0xc8000)
            return;
        {
            int sb = (int)((unsigned int)RandomIntInternal(&data_0209e650) % 3);
            int sel;
            int i;
            int mask = typ & 0x30;
            struct AngleTriple { u16 w[3]; };
            AngleTriple rot;
            int two = 2;
            int three = 3;
            i = 1;
            rot = *(AngleTriple *)&mPrevAngleX;
            for (; i < 3; i++) {
                if (i == sb)
                    sel = 1;
                else if (mask == 0x20)
                    sel = two;
                else
                    sel = three;
                rot.w[1] = (s16)((s16)rot.w[1] + (s16)((unsigned int)RandomIntInternal(&data_0209e650) >> 0x10));
                dActor_c::Spawn(0x150, mask | (sel << 6), mPos, (Vector3_16 *)&rot, mAreaId, -1);
            }
            if (sb == 0)
                mKind = 1;
            else if (mask == 0x20)
                mKind = 2;
            else
                mKind = 3;
        }
    }

    mWanderAngle = (s16)((unsigned int)RandomIntInternal(&data_0209e650) >> 0x10);
    mPrevAngleY = (s16)(mWanderAngle + (int)(((unsigned int)RandomIntInternal(&data_0209e650) >> 0x10) & 0x3fff));
    mHorzSpeed = (int)(((unsigned int)RandomIntInternal(&data_0209e650) >> 0x10) & 0xfff) * 0xf + 0xf000;
    mStateTimer = 0;
    mState = 5;
}

// @symbol _ZN9daBtfly_c6State3Ev
void daBtfly_c::State3()
{
    ApproachLinear(mPrevAngleY, Vec3_HorzAngle(&mPosX, &mHomePosX), 0x800);
    ApproachLinear(mPrevAngleX, Vec3_VertAngle(&mPos, (Vector3 *)&mHomePosX), 0x50);
    UpdatePos(0);

    {
        int *p = &mPosY;
        *p = *p -
             (FX_MUL(mHorzSpeed, data_02082214[((unsigned short)mPrevAngleX >> 4) * 2]) +
              (short)data_02082214[((unsigned short)(short)((mStateTimer << 16) / 100) >> 4) * 2 +
                                   1] *
                  (short)20 / 4);
    }

    mStateTimer = mStateTimer + 1;
    if (mStateTimer > 100)
        mStateTimer = 0;

    if (IsPlayerInRange(0xbb8))
        return;

    mPosX = mHomePosX;
    mPosY = mHomePosY;
    mPosZ = mHomePosZ;
    mState = 1;
}

#pragma opt_common_subs off
// @symbol _ZN9daBtfly_c6State2Ev
void daBtfly_c::State2()
{
    Player *player;
    Vector3 v;
    Vector3 d;

    mPosX = mHomePosX;
    mPosY = mHomePosY;
    mPosZ = mHomePosZ;

    player = ClosestPlayer();
    if (player != 0) {
        int *pp;
        int k = 0x5000;
        int py;
        int t;
        int five = 5;
        Vec3_Sub(&d, &mPosX, &player->mPosX);
        v.x = d.x;
        v.y = d.y;
        v.z = d.z;
        if (Vec3_HorzLen(&v) > 0x4b0000)
            mState = 3;

        pp = &player->mPosX;
        mPosX = mPrevPosX;
        mPosY = mPrevPosY;
        mPosZ = mPrevPosZ;

        v.x = pp[0];
        v.y = pp[1];
        v.z = pp[2];
        t = mStateTimer * k;
        v.x = v.x + t / 4;
        t = mStateTimer * k;
        v.z = v.z + t / 4;
        ApproachLinear(mPrevAngleY, Vec3_HorzAngle(&mPosX, &v), 0x300);

        pp = &player->mPosX;
        v.x = pp[0];
        py = pp[1];
        v.y = py;
        v.z = pp[2];
        v.y = py + ((mStateTimer * five + 0x100) << 12) / 4;
        ApproachLinear(mPrevAngleX, Vec3_VertAngle(&mPos, &v), 0x500);

        UpdatePos(0);

        {
            int *p = &mPosY;
            *p = *p - (FX_MUL(mHorzSpeed, data_02082214[((unsigned short)mPrevAngleX >> 4) * 2]) +
                       (short)data_02082214
                               [((unsigned short)(short)((mStateTimer << 16) / 100) >> 4) * 2 + 1] *
                           (short)20 / 4);
            int *cnt = &mStateTimer;
            *cnt = *cnt + 1;
            if (mStateTimer > 100)
                mStateTimer = 0;
        }
        return;
    }
    mPosX = mPrevPosX;
    mPosY = mPrevPosY;
    mPosZ = mPrevPosZ;
}

#pragma opt_common_subs on

// @symbol _ZN9daBtfly_c6State1Ev
void daBtfly_c::State1()
{
    if (IsPlayerInRange(0x3e8) == 0)
        return;
    mState = 2;
    Player *p = ClosestPlayer();
    mPrevAngleY = Vec3_HorzAngle(&mPosX, &p->mPosX);
}

// @symbol _ZN9daBtfly_c6State0Ev
void daBtfly_c::State0()
{
    struct AngleTriple { u16 w[3]; };
    AngleTriple rot;
    int n;
    int i;
    n = (param1 & 0xf) - 1;
    rot = *(AngleTriple *)&mPrevAngleX;
    i = 0;
    if (n > 0) {
        do {
            Vector3 pos;
            int r;
            r = RandomIntInternal(&data_0209e650);
            pos.x = ((int)((unsigned int)r % 20) - 0xa) * 0xa000 + mPosX;
            r = RandomIntInternal(&data_0209e650);
            pos.y = mPosY + ((int)((unsigned int)r % 40) << 13);
            r = RandomIntInternal(&data_0209e650);
            pos.z = ((int)((unsigned int)r % 20) - 0xa) * 0xa000 + mPosZ;
            r = RandomIntInternal(&data_0209e650);
            rot.w[1] = (s16)((s16)rot.w[1] + ((r << 1) >> 16));
            dActor_c::Spawn(0x150, 0, pos, (Vector3_16 *)&rot, mAreaId, -1);
            i++;
        } while (i < n);
    }
    mState = 1;
}

// @symbol _ZN9daBtfly_c16CleanupResourcesEv
int daBtfly_c::CleanupResources()
{
    gButterflyModelFile.Release();
    gButterflyFlyAnimationFile.Release();
    data_ov002_0210d9d8.Release();
    data_ov100_02148668.Release();
    return 1;
}

// @symbol _ZN9daBtfly_c16OnPendingDestroyEv
void daBtfly_c::OnPendingDestroy()
{
}

// @symbol _ZN9daBtfly_c6RenderEv
int daBtfly_c::Render()
{
    if (mState == 4)
        return 1;
    if (mUseAnimModel != 0)
        mAnmModel.Render(0);
    else
        mModel.Render((Vector3 *)&mScaleX);
    return 1;
}

// @symbol _ZN9daBtfly_c8BehaviorEv
int daBtfly_c::Behavior()
{
    (this->*data_ov100_02148628[mState])();

    if (mKind != 0) {
        int spd = mScale;
        mScaleX = spd;
        mScaleY = spd;
        mScaleZ = spd;

        {
            int s = mHorzSpeed;
            int idx = ((unsigned short)mPrevAngleX >> 4) << 1;
            int velY = FX_MUL(-(int)data_02082214[idx], s);
            mVertSpeed = velY;
            int horiz = FX_MUL((int)data_02082214[idx + 1], s);
            int idx2 = ((unsigned short)mPrevAngleY >> 4) << 1;
            int velX = FX_MUL(horiz, (int)data_02082214[idx2]);
            mSpeedX = velX;
            int idx3 = (((unsigned short)mPrevAngleY >> 4) << 1) + 1;
            int velZ = FX_MUL(horiz, (int)data_02082214[idx3]);
            mSpeedZ = velZ;
        }
        UpdatePosWithOnlySpeed(0);
        mStateTimer++;
    }

    if (mState != 4) {
        int t[3];
        Vec3_Asr(t, &mPosX, 3);
        Matrix4x3_FromTranslation(&data_020a0e68, t[0], t[1], t[2]);
        mAngleY = mPrevAngleY;
        Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);
        if (mUseAnimModel != 0) {
            *(Mtx *)&mAnmModel.mat4x3 = *(Mtx *)&data_020a0e68;
            DropShadowRadHeight(mShadowModel1, mAnmModel.mat4x3, 0x14000, 0x12c000, 0xf);
            mAnmModel.Advance();
        } else {
            *(Mtx *)&mModel.mat4x3 = *(Mtx *)&data_020a0e68;
            DropShadowRadHeight(mShadowModel2, mModel.mat4x3, 0x64000, 0x12c000, 0xf);
        }
    }
    return 1;
}

// @symbol _ZN9daBtfly_c13InitResourcesEv
int daBtfly_c::InitResources()
{
    dExtSimpleModel_c::LoadFile(data_ov002_0210d9d8);
    dExtFrameCtrl_c::LoadFile(gButterflyFlyAnimationFile);
    dExtSimpleModel_c::LoadFile(data_ov100_02148668);
    void *bmd = dExtSimpleModel_c::LoadFile(gButterflyModelFile);

    if (mAnmModel.SetFile((BMD_File *)bmd, 1, 1) == 0)
        return 0;
    mAnmModel.SetAnim((BCA_File *)gButterflyFlyAnimationFile.filePtr, 0, 0x1000, 0);
    if (mShadowModel1.InitCylinder() == 0)
        return 0;
    if (mModel.SetFile((BMD_File *)data_ov100_02148668.filePtr, 1, 1) == 0)
        return 0;
    if (mShadowModel2.InitCylinder() == 0)
        return 0;

    mWithMeshClsn.Init(this, 0x32000, 0x32000, 0, 0);

    Vec3 v;
    v.x = 0;
    v.y = -0x32000;
    v.z = 0;
    mdCcAcPos_c.Init(this, *(Vector3 *)&v, 0x32000, 0x64000, 0x200000, 0);

    int sub = (int)(u8)(param1 & 0x30);
    if (sub != 0x10 && sub != 0x20) {
        mKind = 0;
        mHorzSpeed = 0x7800;
        mStateTimer = ((u32)RandomIntInternal(&data_0209e650) >> 16) % 100;
        if ((u32)(u8)(param1 & 0xf) > 1)
            mState = 0;
        else
            mState = 1;
    } else {
        mState = 4;
    }

    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;

    int r = RandomIntInternal(&data_0209e650);
    int fc = mAnmModel.GetFrameCount();
    u32 rem = (u32)r % (u32)fc;
    mAnmModel.currFrame = (rem << 16) >> 4;

    mScale = 0x1000;
    s32 spd = mScale;
    mScaleX = spd;
    mScaleY = spd;
    mScaleZ = spd;
    mUseAnimModel = 1;
    return 1;
}

// @symbol daBtfly_c_classInit
extern "C" daBtfly_c *daBtfly_c_classInit()
{
    return new daBtfly_c();
}

/* Static-init globals (was the handwritten __sinit_ov100_021473bc shard).
 * Definition order is the retail initializer's construction order: the model
 * handle, then the animation handle, then the state table. */
BtflyModelFilePtr gButterflyModelFile(ASSET_BUTTERFLY_BMD);
BtflyAnimationFilePtr gButterflyFlyAnimationFile(ASSET_BUTTERFLY_FLY_BCA);

ButterflyState data_ov100_02148628[8] = {
    &daBtfly_c::State0,
    &daBtfly_c::State1,
    &daBtfly_c::State2,
    &daBtfly_c::State3,
    &daBtfly_c::State4,
    &daBtfly_c::State5,
    &daBtfly_c::State6,
    &daBtfly_c::State7,
};
