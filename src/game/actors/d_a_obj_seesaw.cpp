//cpp
/**
 * d_a_obj_seesaw.cpp
 * Object - Seesaw platforms
 *
 * SEESAW, BOMB_SEESAW, KM1_SEESAW, KM2_YOKOSEESAW, KM3_SEESAW,
 * KM3_YOKOSEESAW and RC_SEESAW share this body. A ground pound
 * pitches mAngleX toward the side that was hit; Behavior springs it
 * back to level. mVariant, from actorID, selects the model, the
 * collision and the CLPS. Those three symbols are one record, stride
 * 0xc, so each is indexed from its own base.
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
#include "Player.h"
#include "nitro/fx/fx.h"
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

/* Fix12 2.0. Mega ground pounds use this instead of the character scale. */
enum { kMegaPoundScale = 0x2000 };

/* PlayLong bank-3 id while |mAngleXSpeed| > 0xa. */
enum { kTiltSound = 0x8b };

/* The three symbols are the first record's model, collision and CLPS.
 * Variant i lives at symbol + i * 0xc (one 3-word record). */
enum { kFileStride = 0xc };

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
extern s16 data_02082214[];

/* local extern: no shared header declares these. */
extern "C" {
void func_020393d4(void *clsn, void *callback);
void func_020393c4(void *clsn, void *callback);
}

/* D1 stores this vtable, then dBgActor_c's (inlined), then destroys
 * mMeshCollider and mModel. D0 is that plus the inline operator delete.
 * This class adds no member with a destructor. */
// @symbol _ZN13daObjSeesaw_cD1Ev
// @symbol _ZN13daObjSeesaw_cD0Ev
daObjSeesaw_c::~daObjSeesaw_c() {}

/* Weight of the actor that hit or stands on the seesaw. data_ov095_02136f58 is
 * four fix12 weights indexed by param1 & 3 (the character on a player:
 * 0x1000, 0x800, 0x1800, 0x1000). A non-player keeps that weight. Mega
 * replaces it with kMegaPoundScale. */
// @symbol _ZN13daObjSeesaw_c14GetActorWeightEP8dActor_c
int daObjSeesaw_c::GetActorWeight(dActor_c *other) {
    int scale = data_ov095_02136f58[(int)other->param1 & 3];
    unsigned isPlayer = (unsigned)(other->actorID == ACTOR_PLAYER);
    if (isPlayer == 0)
        return scale;
    if (((Player *)other)->mIsMega != 0)
        scale = kMegaPoundScale;
    return scale;
}

#pragma push
#pragma opt_propagation off
// @symbol _ZN13daObjSeesaw_c15OnGroundPoundedER8dActor_c
void daObjSeesaw_c::OnGroundPounded(dActor_c &other) {
    /* b and bPos stay separate locals: mwcc keeps the hitter in r8 and
     * its position in r4. */
    dActor_c *b = &other;
    Vector3 *bPos = &b->mPos;
    int dist = Vec3_Dist(&mPos, bPos);
    s16 angle = Vec3_HorzAngle(&mPos, bPos);
    int n, d, idx, prod, cur, v;
    s16 *pitch;

    mPoundedThisFrame = 1;
    n = GetActorWeight(b);
    d = AngleDiff(angle, mAngleY);
    /* data_02082214 is interleaved cos, sin. (angle >> 4) * 2 + 1 is sine. */
    idx = ((u16)(s16)d >> 4) << 1;

    prod = FX_MUL(dist, n);
    idx = idx + 1;

    pitch = &mAngleX;
    cur = *pitch;
    v = FX_MUL(prod, data_02082214[idx]);
    /* Signed divide by 4096, then narrow onto the s16 angle. */
    v = (v + ((unsigned)(v >> 11) >> 20)) << 4;
    *pitch = (s16)(cur + (v >> 16));
    if (mAngleX > kMaxPitch)
        mAngleX = kMaxPitch;
    if (mAngleX < -kMaxPitch)
        mAngleX = -kMaxPitch;
}
#pragma pop

/* Spring *angle toward target by *speed. Behavior calls this with
 * &mAngleX, &mAngleXSpeed, target 0, thresh 6, accel 3, mult 3. It reads
 * nothing from the seesaw itself. Returns 1 once the angle has settled. */
#pragma push
#pragma opt_common_subs off
// @symbol _ZN13daObjSeesaw_c11SpringAngleEPsS0_isis
int daObjSeesaw_c::SpringAngle(short *angle, short *speed, int target, short thresh, int accel,
                               short mult) {
    short old = angle[0];
    angle[0] = old + speed[0];
    short now = angle[0];
    if (now == target ||
        ((now - target) * (old - target) < 0 && speed[0] > -thresh && speed[0] < thresh)) {
        angle[0] = target;
        speed[0] = 0;
        return 1;
    }
    if (now >= target)
        accel = (short)-accel;
    if ((short)speed[0] * (short)accel < 0)
        accel = (short)accel * (short)mult;
    speed[0] = speed[0] + accel;
    return 0;
}
#pragma pop

/* Model matrix from the actor angles, translation = position >> 3
 * (world fix12 to the model's units). */
// @symbol _ZN13daObjSeesaw_c17UpdateModelMatrixEv
void daObjSeesaw_c::UpdateModelMatrix() {
    Matrix4x3_FromRotationXYZExt(&mModel.mat4x3, mAngleX, mAngleY, mAngleZ);
    mModel.mat4x3.m[9] = mPosX >> 3;
    mModel.mat4x3.m[10] = mPosY >> 3;
    mModel.mat4x3.m[11] = mPosZ >> 3;
}

// @symbol _ZN13daObjSeesaw_c16CleanupResourcesEv
int daObjSeesaw_c::CleanupResources() {
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    (*(SharedFilePtr **)(data_ov095_021374a0 + mVariant * kFileStride))->Release();
    (*(SharedFilePtr **)(data_ov095_021374a4 + mVariant * kFileStride))->Release();
    return 1;
}

// @symbol _ZN13daObjSeesaw_c6RenderEv
int daObjSeesaw_c::Render() {
    mModel.Render(0);
    return 1;
}

// @symbol _ZN13daObjSeesaw_c8BehaviorEv
int daObjSeesaw_c::Behavior() {
    /* The 0/1 temporary is the ROM's movne/moveq/cmp. A direct test
     * of the bit is three instructions shorter. */
    int offScreen = (int)((mFlags & kFlagOffScreen) != 0);
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

// @symbol _ZN13daObjSeesaw_c13InitResourcesEv
int daObjSeesaw_c::InitResources() {
    unsigned char idx;
    int f;
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
    idx = mVariant;
    f = (int)dExtSimpleModel_c::LoadFile(
        **(SharedFilePtr **)(data_ov095_021374a0 + idx * kFileStride));
    mModel.SetFile((BMD_File *)f, 1, -1);
    UpdateModelMatrix();
    UpdateClsnPosAndRot();
    {
        unsigned char i = mVariant;
        f = (int)dBgW_Kc::LoadFile(**(SharedFilePtr **)(data_ov095_021374a4 + i * kFileStride));
        mMeshCollider.SetFile((KCL_File *)f, mClsnMat, 0x1000, mAngleY,
                              *(CLPS_Block *)(*(void **)(data_ov095_021374a8 + i * kFileStride)));
    }
    /* func_020393d4 stores dBgW+0x18 (beforeClsnCallback). func_020393c4
     * stores dBgW+0x1c, the stood-on callback: StoodOnCallback, the
     * veneer into OnStoodOn below. Both are calls in the ROM; the
     * bodies are a single word store. */
    func_020393d4(&mMeshCollider, (int *)&dBgW::UpdatePosWithTransform);
    func_020393c4(&mMeshCollider, (void *)&StoodOnCallback);
    return 1;
}

/* Stood-on tilt. The mesh collider calls this while another actor rests
 * on the seesaw: mAngleX keeps integrating mAngleXSpeed, and the stander's
 * weight (GetActorWeight) and off-centre distance drive mAngleXSpeed
 * toward a clamp. Unlike OnGroundPounded it does not kick the pitch
 * directly; it accelerates the tilt speed, hard when pushing into the
 * current lean, soft when pushing out of it. */
// @symbol _ZN13daObjSeesaw_c9OnStoodOnEP8dActor_c
void daObjSeesaw_c::OnStoodOn(dActor_c *other) {
    Vector3 *op = &other->mPos;
    int dist = Vec3_Dist(&mPos, op);
    int ang = Vec3_HorzAngle(&mPos, op);
    mPoundedThisFrame = 1;
    int r = GetActorWeight(other);

    mAngleX += mAngleXSpeed;
    int x = FX_MUL(dist, r);
    s16 d = (s16)AngleDiff(ang, mAngleY);
    int y = FX_MUL(x, data_02082214[((u16)d >> 4) * 2 + 1]);

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
// @symbol _ZN13daObjSeesaw_c15StoodOnCallbackEPvPS_P8dActor_c
void daObjSeesaw_c::StoodOnCallback(void *collider, daObjSeesaw_c *self, dActor_c *other) {
    self->OnStoodOn(other);
}
#pragma pop

// @symbol daObjSeesaw_c_classInit_RC_SEESAW
extern "C" daObjSeesaw_c *daObjSeesaw_c_classInit_RC_SEESAW() {
    return new daObjSeesaw_c();
}

// @symbol daObjSeesaw_c_classInit_KM3_YOKOSEESAW
extern "C" daObjSeesaw_c *daObjSeesaw_c_classInit_KM3_YOKOSEESAW() {
    return new daObjSeesaw_c();
}

// @symbol daObjSeesaw_c_classInit_KM3_SEESAW
extern "C" daObjSeesaw_c *daObjSeesaw_c_classInit_KM3_SEESAW() {
    return new daObjSeesaw_c();
}

// @symbol daObjSeesaw_c_classInit_KM2_YOKOSEESAW
extern "C" daObjSeesaw_c *daObjSeesaw_c_classInit_KM2_YOKOSEESAW() {
    return new daObjSeesaw_c();
}

// @symbol daObjSeesaw_c_classInit_KM1_SEESAW
extern "C" daObjSeesaw_c *daObjSeesaw_c_classInit_KM1_SEESAW() {
    return new daObjSeesaw_c();
}

// @symbol daObjSeesaw_c_classInit_BOMB_SEESAW
extern "C" daObjSeesaw_c *daObjSeesaw_c_classInit_BOMB_SEESAW() {
    return new daObjSeesaw_c();
}

// @symbol daObjSeesaw_c_classInit_SEESAW
extern "C" daObjSeesaw_c *daObjSeesaw_c_classInit_SEESAW() {
    return new daObjSeesaw_c();
}

/* Seven variants, each a model/collision handle pair; the pointer table at
 * data_ov095_021374a0 indexes them through kFileStride. */
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
