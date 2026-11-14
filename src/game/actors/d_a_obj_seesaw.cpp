//cpp
/**
 * d_a_obj_seesaw.cpp
 * Object - Seesaw platforms
 *
 * SEESAW, BOMB_SEESAW, KM1_SEESAW, KM2_YOKOSEESAW, KM3_SEESAW,
 * KM3_YOKOSEESAW and RC_SEESAW share this body. A ground pound
 * pitches mAngleX toward the side that was hit; Behavior springs it
 * back to level. mVariant, from actorID, selects the SeesawAssets row
 * (model, collision and CLPS).
 *
 * decl_common.h stays first. It pulls common.h's flat Matrix4x3
 * (s32 m[12]). daObjSeesaw_c.h includes dBgW_KcMbg.h, which includes
 * math/Matrix.h, and that nested spelling changes UpdateClsnPosAndRot.
 *
 * #pragma defer_codegen off emits the out-of-line destructor as D1
 * then D0. The cartridge has no D2. The g_profile_* rows stay in
 * their own files.
 *
 * What stays:
 * - OnGroundPounded keeps separate locals for the hitter and its
 *   position: mwcc holds them in r8 and r4. Spelling both as
 *   &other.mPos recomputes the add, drops r8 and adds a stack adjust.
 * - Behavior keeps the 0/1 temporary for the off-screen bit; a direct
 *   bit test is three instructions shorter than the ROM.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daObjSeesaw_c_AssetId {
    ASSET_OBJ_SEESAW_BMD = 0x048a,
    ASSET_OBJ_SEESAW_KCL = 0x048b,
    ASSET_B_SI_SO_BMD = 0x05a5,
    ASSET_B_SI_SO_KCL = 0x05a6,
    ASSET_KM1_SHISO_BMD = 0x0657,
    ASSET_KM1_SHISO_KCL = 0x0658,
    ASSET_KM2_YOKOSHISO_BMD = 0x066d,
    ASSET_KM2_YOKOSHISO_KCL = 0x066e,
    ASSET_KM3_SHISO_BMD = 0x0685,
    ASSET_KM3_SHISO_KCL = 0x0686,
    ASSET_KM3_YOKOSHISO_BMD = 0x0687,
    ASSET_KM3_YOKOSHISO_KCL = 0x0688,
    ASSET_RC_SHISO_BMD = 0x06ad,
    ASSET_RC_SHISO_KCL = 0x06ae
};

#pragma defer_codegen off

#include "decl_common.h"
#include "daObjSeesaw_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "Sound.h"
#include "daPly_c.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "math/CalcLib.h"

/* Profile actor ids (symbols/profile_reconstruction_registry.json).
 * param1 on a PLAYER (0xbf) is the character; the low two bits index it. */
enum {
    ACTOR_SEESAW = 0x1c,
    ACTOR_BOMB_SEESAW = 0x27,
    ACTOR_RC_SEESAW = 0x80,
    ACTOR_KM1_SEESAW = 0x85,
    ACTOR_KM2_YOKOSEESAW = 0x8f,
    ACTOR_KM3_SEESAW = 0x95,
    ACTOR_KM3_YOKOSEESAW = 0x96,
    ACTOR_PLAYER = 0xbf
};

/* dActor_c::mFlags bit written by the framework when the actor is off screen. */
enum { kFlagOffScreen = 8 };

/* ±45 degrees in the angle units (full circle 0x10000). */
enum { kMaxPitch = 0x2000 };

/* fx32 2.0. Mega ground pounds use this instead of the character scale. */
enum { kMegaPoundScale = 0x2000 };

/* PlayLong bank-3 id while |mAngleXSpeed| > 0xa. */
enum { kTiltSound = 0x8b };


/* File-scope resource handles: the ctor/dtor are the ROM's SharedFilePtr
 * veneer pairs (func_02017acc / func_02017ab4 for the model, func_02017b4c /
 * SharedFilePtr_Destruct_Clsn for the collision), spelled through
 * declared-only subclasses so the static initializer names the real entry
 * points. */
struct SeesawModelFilePtr : SharedFilePtr {
    SeesawModelFilePtr(u32 fileID);
    ~SeesawModelFilePtr();
};
struct SeesawCollisionFilePtr : SharedFilePtr {
    SeesawCollisionFilePtr(u32 fileID);
    ~SeesawCollisionFilePtr();
};

extern int data_ov095_02136f58[];

/* Per-variant asset row: the model file, the collision file and the CLPS block,
 * indexed by mVariant. */
struct SeesawAssets {
    SharedFilePtr *modelFile;
    SharedFilePtr *clsnFile;
    CLPS_Block *clps;
};

extern SeesawAssets data_ov095_021374a0[7];

/* D1 stores this vtable, then dBgActor_c's (inlined), then destroys
 * mMeshCollider and mModel. D0 is that plus the inline operator delete.
 * This class adds no member with a destructor. */
daObjSeesaw_c::~daObjSeesaw_c() {}

/* Weight of the actor that hit or stands on the seesaw. data_ov095_02136f58 is
 * four fix12 weights indexed by param1 & 3 (the character on a player:
 * 0x1000, 0x800, 0x1800, 0x1000). A non-player keeps that weight. Mega
 * replaces it with kMegaPoundScale. */
int daObjSeesaw_c::GetActorWeight(dActor_c *other) {
    int scale = data_ov095_02136f58[(int)other->param1 & 3];
    unsigned isPlayer = (other->actorID == ACTOR_PLAYER);
    if (isPlayer == 0)
        return scale;
    if (((daPly_c *)other)->mIsMega != 0)
        scale = kMegaPoundScale;
    return scale;
}

void daObjSeesaw_c::OnGroundPounded(dActor_c &other) {
    /* b and bPos stay separate locals: mwcc keeps the hitter in r8 and
     * its position in r4. */
    dActor_c *b = &other;
    Vector3 *bPos = &b->mPos;
    int dist = Vec3_Dist(&mPos, bPos);
    s16 angle = Vec3_HorzAngle(&mPos, bPos);
    int weight;
    s16 diff;

    mPoundedThisFrame = 1;
    weight = GetActorWeight(b);
    diff = AngleDiff(angle, mAngleY);
    /* The kick is weighted by the cosine of the hitter's bearing off the
     * seesaw's heading. */
    mAngleX += (s16)(FX_MUL(FX_MUL(dist, weight), FX_CosIdx((u16)diff >> 4)) / 4096);
    if (mAngleX > kMaxPitch)
        mAngleX = kMaxPitch;
    if (mAngleX < -kMaxPitch)
        mAngleX = -kMaxPitch;
}

/* Spring *angle toward target by *speed. Behavior calls this with
 * &mAngleX, &mAngleXSpeed, target 0, thresh 6, accel 3, mult 3. It reads
 * nothing from the seesaw itself. Returns 1 once the angle has settled.
 * accel is a short: the ROM sign-extends it after the negation and
 * multiplies it with smulbb. */
int daObjSeesaw_c::SpringAngle(short *angle, short *speed, int target, short thresh, short accel,
                               short mult) {
    short old = *angle;
    *angle += *speed;
    if (*angle == target ||
        ((*angle - target) * (old - target) < 0 && *speed > -thresh && *speed < thresh)) {
        *angle = target;
        *speed = 0;
        return 1;
    }
    if (*angle >= target)
        accel = -accel;
    if (*speed * accel < 0)
        accel *= mult;
    *speed += accel;
    return 0;
}

/* Model matrix from the actor angles, translation = position >> 3
 * (world fix12 to the model's units). */
void daObjSeesaw_c::UpdateModelMatrix() {
    Matrix4x3_FromRotationXYZExt(&mModel.mat4x3, mAngleX, mAngleY, mAngleZ);
    mModel.mat4x3.m[9] = mPosX >> 3;
    mModel.mat4x3.m[10] = mPosY >> 3;
    mModel.mat4x3.m[11] = mPosZ >> 3;
}

int daObjSeesaw_c::CleanupResources() {
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    data_ov095_021374a0[mVariant].modelFile->Release();
    data_ov095_021374a0[mVariant].clsnFile->Release();
    return 1;
}

int daObjSeesaw_c::Render() {
    mModel.Render(0);
    return 1;
}

int daObjSeesaw_c::Behavior() {
    /* The 0/1 temporary is the ROM's movne/moveq/cmp. A direct test
     * of the bit is three instructions shorter. */
    int offScreen = ((mFlags & kFlagOffScreen) != 0);
    if (offScreen != 0) {
        if (mMeshCollider.IsEnabled())
            mMeshCollider.Disable();
        return 1;
    }
    if (mPoundedThisFrame == 0) {
        SpringAngle(&mAngleX, &mAngleXSpeed, 0, 6, 3, 3);
    }
    {
        int s = mAngleXSpeed;
        if (s < 0)
            s = (short)-s;
        if (s > 0xa) {
            mTiltSound = Sound::PlayLong(mTiltSound, 3, kTiltSound, mCamSpacePos, 0);
        }
    }
    if (mAngleX > kMaxPitch)
        mAngleX = kMaxPitch;
    if (mAngleX < -kMaxPitch)
        mAngleX = -kMaxPitch;
    UpdateModelMatrix();
    if (IsClsnInRange(0, 0))
        UpdateClsnPosAndRot();
    mPoundedThisFrame = 0;
    return 1;
}

int daObjSeesaw_c::InitResources() {
    void *f;
    switch (actorID) {
    case ACTOR_SEESAW:
        mVariant = 0;
        break;
    case ACTOR_BOMB_SEESAW:
        mVariant = 1;
        break;
    case ACTOR_KM1_SEESAW:
        mVariant = 2;
        break;
    case ACTOR_KM2_YOKOSEESAW:
        mVariant = 3;
        break;
    case ACTOR_KM3_SEESAW:
        mVariant = 4;
        break;
    case ACTOR_KM3_YOKOSEESAW:
        mVariant = 5;
        break;
    case ACTOR_RC_SEESAW:
        mVariant = 6;
        break;
    }
    f = dExtSimpleModel_c::LoadFile(*data_ov095_021374a0[mVariant].modelFile);
    mModel.SetFile((BMD_File *)f, 1, -1);
    UpdateModelMatrix();
    UpdateClsnPosAndRot();
    {
        int i = mVariant;
        f = dBgW_Kc::LoadFile(*data_ov095_021374a0[i].clsnFile);
        mMeshCollider.SetFile((KCL_File *)f, mClsnMat, 0x1000, mAngleY,
                              *data_ov095_021374a0[i].clps);
    }
    /* dBgW::SetBeforeClsnCallback stores dBgW+0x18 (beforeClsnCallback). dBgW::SetStoodOnCallback
     * stores dBgW+0x1c, the stood-on callback: StoodOnCallback, the
     * veneer into OnStoodOn below. Both are calls in the ROM; the
     * bodies are a single word store. */
    mMeshCollider.SetBeforeClsnCallback(&dBgW::UpdatePosWithTransform);
    mMeshCollider.SetStoodOnCallback((::StoodOnCallback)&StoodOnCallback);
    return 1;
}

/* Stood-on tilt. The mesh collider calls this while another actor rests
 * on the seesaw: mAngleX keeps integrating mAngleXSpeed, and the stander's
 * weight (GetActorWeight) and off-centre distance drive mAngleXSpeed
 * toward a clamp. Unlike OnGroundPounded it does not kick the pitch
 * directly; it accelerates the tilt speed, hard when pushing into the
 * current lean, soft when pushing out of it. */
void daObjSeesaw_c::OnStoodOn(dActor_c *other) {
    Vector3 *op = &other->mPos;
    int dist = Vec3_Dist(&mPos, op);
    int ang = Vec3_HorzAngle(&mPos, op);
    mPoundedThisFrame = 1;
    int r = GetActorWeight(other);

    mAngleX += mAngleXSpeed;
    int x = FX_MUL(dist, r);
    s16 d = (s16)AngleDiff(ang, mAngleY);
    int y = FX_MUL(x, FX_CosIdx((u16)d >> 4));

    int acc;
    if (mAngleXSpeed * x < 0)
        acc = FX_MUL(y, 0xa3LL);
    else
        acc = FX_MUL(y, 0x51LL);

    s16 dv = acc / 4096;
    mAngleXSpeed += dv;
    s16 lim = FX_MUL(r, 0x32000LL) / 4096;
    if (mAngleXSpeed > lim)
        mAngleXSpeed = lim;
    if (mAngleXSpeed < -lim)
        mAngleXSpeed = -lim;
}

/* The collider callback slot gets three arguments; OnStoodOn wants the
 * last two. long_calls keeps the pooled absolute tail call. */
#pragma push
#pragma long_calls on
void daObjSeesaw_c::StoodOnCallback(void *collider, daObjSeesaw_c *self, dActor_c *other) {
    self->OnStoodOn(other);
}
#pragma pop

daObjSeesaw_c *daObjSeesaw_c_classInit_RC_SEESAW() {
    return new daObjSeesaw_c();
}

daObjSeesaw_c *daObjSeesaw_c_classInit_KM3_YOKOSEESAW() {
    return new daObjSeesaw_c();
}

daObjSeesaw_c *daObjSeesaw_c_classInit_KM3_SEESAW() {
    return new daObjSeesaw_c();
}

daObjSeesaw_c *daObjSeesaw_c_classInit_KM2_YOKOSEESAW() {
    return new daObjSeesaw_c();
}

daObjSeesaw_c *daObjSeesaw_c_classInit_KM1_SEESAW() {
    return new daObjSeesaw_c();
}

daObjSeesaw_c *daObjSeesaw_c_classInit_BOMB_SEESAW() {
    return new daObjSeesaw_c();
}

daObjSeesaw_c *daObjSeesaw_c_classInit_SEESAW() {
    return new daObjSeesaw_c();
}

/* Seven variants, each a model/collision handle pair, reached through the
 * SeesawAssets rows at data_ov095_021374a0. */
SeesawModelFilePtr gObjSeesawModelFile(ASSET_OBJ_SEESAW_BMD);
SeesawModelFilePtr gBSiSoModelFile(ASSET_B_SI_SO_BMD);
SeesawModelFilePtr gKm1ShisoModelFile(ASSET_KM1_SHISO_BMD);
SeesawModelFilePtr gKm2YokoshisoModelFile(ASSET_KM2_YOKOSHISO_BMD);
SeesawModelFilePtr gKm3ShisoModelFile(ASSET_KM3_SHISO_BMD);
SeesawModelFilePtr gKm3YokoshisoModelFile(ASSET_KM3_YOKOSHISO_BMD);
SeesawModelFilePtr gRcShisoModelFile(ASSET_RC_SHISO_BMD);
SeesawCollisionFilePtr gObjSeesawCollisionFile(ASSET_OBJ_SEESAW_KCL);
SeesawCollisionFilePtr gBSiSoCollisionFile(ASSET_B_SI_SO_KCL);
SeesawCollisionFilePtr gKm1ShisoCollisionFile(ASSET_KM1_SHISO_KCL);
SeesawCollisionFilePtr gKm2YokoshisoCollisionFile(ASSET_KM2_YOKOSHISO_KCL);
SeesawCollisionFilePtr gKm3ShisoCollisionFile(ASSET_KM3_SHISO_KCL);
SeesawCollisionFilePtr gKm3YokoshisoCollisionFile(ASSET_KM3_YOKOSHISO_KCL);
SeesawCollisionFilePtr gRcShisoCollisionFile(ASSET_RC_SHISO_KCL);
