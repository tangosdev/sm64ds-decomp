//cpp
/* daObjCtMecha04_c -- the conveyor belt of Tick Tock Clock (CT_MECHA04L/S,
 * ov065). Both spawn entries construct this class, and InitResources picks its
 * model, collision and CLPS row from actorID (0x6f -> row 0, 0x70 -> row 1).
 * Behavior scrolls the belt texture and, through AfterClsnCallback, moves
 * whatever stands on it.
 *
 * UpdateShadow, MoveActorOnBelt, and AfterClsnCallback are descriptive
 * reconstructions. The member/static forms and parameter spellings of the
 * latter two are also inferred; the manifest records the evidence boundary.
 */

// What stays, and why (each measured; simplifying changes the emitted bytes):
// dBgW_KcMbg::SetFile / DropShadowScaleXYZ / dBgActor_c::IsClsnInRange are
// header member calls whose headers spell Fix12 as Fix12i, a plain s32
// (6az). func_020393c4 / func_020393bc
// store/load dBgW+0x1c -- the ROM's own call, not an inlineable assignment
// (no setter).
// common.h first: UpdateShadow writes mShadowMat.m[9..11] on the flat 12-word
// spelling. InitResources keeps its two-store animationFiles[2] stack array,
// the variant reloads, MoveActorOnBelt's &mPosX-derived pointer chain and flat
// sin/cos index, and the int-returning IsOffScreen / IsLargeBelt tests --
// measured bool-widening / pointer-reuse shapes. `return new`
// emits a homeless _ZN10dBgActor_cD2Ev (compiler-only deadstrip); the header's
// inline dtor gives the D1/D0 pair, no D2.

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daObjCtMecha04_c_AssetId {
    ASSET_CT_MECHA_OBJ04L_BMD = 0x05be,
    ASSET_CT_MECHA_OBJ04L_KCL = 0x05bf,
    ASSET_CT_MECHA_OBJ04S_BMD = 0x05c0,
    ASSET_CT_MECHA_OBJ04S_KCL = 0x05c1
};

#include "common.h"
#include "daObjCtMecha04_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "dBgCh_Gnd.h"
#include "nitro/fx/fx.h"
#include "math/Approach.h"
#include "math/CalcLib.h"
#include "math/MtxStack.h"

enum {
    ACTOR_CT_MECHA04L = 0x6f,
    ACTOR_CT_MECHA04S = 0x70,
    CLOCK_SETTING_RANDOM = 2, /* Behavior picks a random belt direction */
    CLOCK_SETTING_STOPPED = 3 /* Behavior skips the belt texture, sound and callback */
};

/* Resource handles constructed by this TU's static initializer. The
 * constructor and destructor bodies stay out of line; mwcc registers them. */
struct Mecha04ModelFilePtr : SharedFilePtr {
    Mecha04ModelFilePtr(u32 fileID);
    ~Mecha04ModelFilePtr();
};

struct Mecha04ClsnFilePtr : SharedFilePtr {
    Mecha04ClsnFilePtr(u32 fileID);
    ~Mecha04ClsnFilePtr();
};

namespace Sound {
/* local extern: including Sound.h renumbers @NNN. */
int PlayLong(u32, u32, u32, const Vector3 &, s16);
}

/* One 2-row {model, collision, clps} table indexed by mVariant; the three
 * data_ov065_* symbols name row 0's fields, so each column's decl strides by
 * the row size. */
extern BTA_File *data_ov065_0211d16c[2];
extern SharedFilePtr *data_ov065_0211d194[][3];
extern SharedFilePtr *data_ov065_0211d198[][3];
extern CLPS_Block *data_ov065_0211d19c[][3];
extern u8 data_0209f2c0;
extern int data_0209e650;
extern int data_ov065_0211c0b8[]; /* per-clock-setting belt speeds */
extern s16 data_02082214[];       /* sin/cos pair per (angle >> 4) index */

/* local extern: the dBgW+0x1c callback store/load; no shared
 * header. */
extern "C" {
extern void func_020393c4(dBgW *collider, void *callback);
extern int func_020393bc(dBgW *collider);
}

/* mFlags 0x08 is the framework's off-screen bit (dActor_c.h bit table). */
static inline int IsOffScreen(dActor_c *actor) {
    return (actor->mFlags & 8) != 0;
}

static inline int IsLargeBelt(dActor_c *actor) {
    return actor->actorID == ACTOR_CT_MECHA04L;
}

// @symbol daObjCtMecha04_c_classInit_CT_MECHA04S
extern "C" daObjCtMecha04_c *daObjCtMecha04_c_classInit_CT_MECHA04S() {
    return new daObjCtMecha04_c();
}

// @symbol daObjCtMecha04_c_classInit_CT_MECHA04L
extern "C" daObjCtMecha04_c *daObjCtMecha04_c_classInit_CT_MECHA04L() {
    return new daObjCtMecha04_c();
}

// @symbol _ZN16daObjCtMecha04_c17AfterClsnCallbackEP4dBgWP8dActor_cS3_
/* Inferred descriptive name and observed three-register callback ABI. The
 * wrapper deliberately ignores the collider and tail-calls the owner method. */
void daObjCtMecha04_c::AfterClsnCallback(dBgW *collider, dActor_c *owner, dActor_c *other) {
    ((daObjCtMecha04_c *)owner)->MoveActorOnBelt(*other);
}

// @symbol _ZN16daObjCtMecha04_c15MoveActorOnBeltER8dActor_c
/* Inferred descriptive name. The collision callback supplies this conveyor as
 * owner and the actor whose X/Z position should advance with the belt. Each
 * axis moves by mBeltSpeed * 4 scaled by one sin/cos pair of data_02082214,
 * indexed by (u16)mAngleY >> 4: [2i] for X, [2i + 1] for Z. Direct
 * actor.mPosX/mPosZ stores differ; the PosX-derived pointer chain matches. */
void daObjCtMecha04_c::MoveActorOnBelt(dActor_c &actor) {
    u16 angleForX = (u16)mAngleY;
    int angleIndexForX = angleForX >> 4;
    int beltStepForX = mBeltSpeed << 2;
    int sinY = data_02082214[angleIndexForX * 2];
    int deltaX = FX_MUL(beltStepForX, sinY);
    s32 *actorPosX = &actor.mPosX;
    int oldX = *actorPosX;
    *actorPosX = oldX + deltaX;

    u16 angleForZ = (u16)mAngleY;
    int angleIndexForZ = angleForZ >> 4;
    int beltStepForZ = mBeltSpeed << 2;
    s32 *actorPosZ = actorPosX + 2;
    int cosY = data_02082214[(angleIndexForZ * 2) + 1];
    int oldZ = *actorPosZ;
    int deltaZ = FX_MUL(beltStepForZ, cosY);
    *actorPosZ = oldZ + deltaZ;
}

// @symbol _ZN16daObjCtMecha04_c13InitResourcesEv
int daObjCtMecha04_c::InitResources() {
    BTA_File *animationFiles[2];
    Vector3 position;
    unsigned char variant;
    void *modelFile;
    void *collisionFile;

    animationFiles[0] = data_ov065_0211d16c[0];
    animationFiles[1] = data_ov065_0211d16c[1];

    if (actorID != ACTOR_CT_MECHA04L) {
        if (actorID == ACTOR_CT_MECHA04S)
            mVariant = 1;
    } else {
        mVariant = 0;
    }

    variant = mVariant;
    modelFile = dExtSimpleModel_c::LoadFile(*data_ov065_0211d194[variant][0]);
    mModel.SetFile((BMD_File *)modelFile, 1, -1);

    mShadowModel.InitCuboid();

    variant = mVariant;
    dExtAnmTexSRT_c::Prepare(*(BMD_File *)((void **)data_ov065_0211d194[variant][0])[1],
                             *animationFiles[variant]);

    mTextureTransformer.SetFile(*animationFiles[mVariant], 0, 0x1000, 0);

    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    variant = mVariant;
    collisionFile = dBgW_Kc::LoadFile(*data_ov065_0211d198[variant][0]);
    mMeshCollider.SetFile((KCL_File *)collisionFile, mClsnMat, 0x199, mAngleY,
                          *data_ov065_0211d19c[variant][0]);

    func_020393c4(&mMeshCollider, (void *)&daObjCtMecha04_c::AfterClsnCallback);

    mTargetBeltSpeed = data_ov065_0211c0b8[data_0209f2c0];
    mBeltSpeed = mTargetBeltSpeed;
    mTextureTransformer.speed = mBeltSpeed;

    position.x = mPosX;
    position.y = mPosY;
    position.z = mPosZ;
    position.y -= 0xa000;

    {
        dBgCh_Gnd ground;

        ground.SetObjAndPos(position, 0);
        mGroundY = position.y;

        if (ground.DetectClsn())
            mGroundY = ground.clsnY;
    }

    return 1;
}

// @symbol _ZN16daObjCtMecha04_c8BehaviorEv
int daObjCtMecha04_c::Behavior() {
    if (data_0209f2c0 == CLOCK_SETTING_STOPPED) {
        func_020393c4(&mMeshCollider, 0);
        IsClsnInRange(0, 0);
    } else {
        if (!IsOffScreen(this)) {
            if (!func_020393bc(&mMeshCollider)) {
                func_020393c4(&mMeshCollider, (void *)&daObjCtMecha04_c::AfterClsnCallback);
            }

            if (data_0209f2c0 == CLOCK_SETTING_RANDOM) {
                if (ApproachLinear(mBeltSpeed, mTargetBeltSpeed, 0xcc) != 0 &&
                    DecIfAbove0_Short((u16 *)&mDirectionTimer) == 0) {
                    unsigned int randomValue =
                        (u16)((unsigned int)RandomIntInternal(&data_0209e650) >> 16);
                    /* New direction, held for 0xa + 0x14 * (0..6) ticks. */
                    mDirectionTimer = (s16)(((int)randomValue % 7) * 0x14 + 0xa);
                    if (randomValue >= 0x7fff) {
                        mTargetBeltSpeed = 0x1000;
                    } else {
                        mTargetBeltSpeed = -0x1000;
                    }
                }
            } else {
                mBeltSpeed = data_ov065_0211c0b8[data_0209f2c0];
            }

            mTextureTransformer.speed = mBeltSpeed;
            mTextureTransformer.Advance();
            if (mBeltSpeed != 0) {
                mSoundHandle =
                    Sound::PlayLong(mSoundHandle, 3, 0x88, *(Vector3 *)&mCamSpacePosX, 0);
            }
        }

        IsClsnInRange(0, 0);
    }

    UpdateShadow();
    return 1;
}

// @symbol _ZN16daObjCtMecha04_c6RenderEv
int daObjCtMecha04_c::Render() {
    mTextureTransformer.Update(mModel.data);
    mModel.Render(0);
    return 1;
}

// @symbol _ZN16daObjCtMecha04_c16CleanupResourcesEv
int daObjCtMecha04_c::CleanupResources() {
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    data_ov065_0211d194[mVariant][0]->Release();
    data_ov065_0211d198[mVariant][0]->Release();
    return 1;
}

// @symbol _ZN16daObjCtMecha04_c12UpdateShadowEv
/* Inferred descriptive name. DropShadowScaleXYZ's three sizes are Fix12i
 * (a plain s32); Fix12<int> by value homes arguments absent from the
 * cartridge (6az). */
void daObjCtMecha04_c::UpdateShadow() {
    int heightDiff = mPosY - mGroundY;
    int absHeightDiff = heightDiff < 0 ? -heightDiff : heightDiff;
    if (absHeightDiff > 0x7d0000)
        return;

    Matrix4x3_FromRotationY(&mShadowMat, mAngleY);
    mShadowMat.m[9] = mPosX >> 3;
    mShadowMat.m[10] = (mGroundY + 0x1000) >> 3;
    mShadowMat.m[11] = mPosZ >> 3;

    if (IsLargeBelt(this)) {
        DropShadowScaleXYZ(mShadowModel, mShadowMat, 0x1f4000, 0x32000, 0x3e8000, 0xf);
        return;
    }

    DropShadowScaleXYZ(mShadowModel, mShadowMat, 0x1f4000, 0x32000, 0x320000, 0xf);
}

// @symbol _ZN16daObjCtMecha04_cD1Ev
// @symbol _ZN16daObjCtMecha04_cD0Ev
/* No separate body lives here. The inline virtual destructor in the directly
 * included class header makes mwccarm emit retail's D1 then D0 order without
 * the otherwise homeless D2 variant. */

/* The ROM's initializer constructs both model handles (file IDs 1472 and
 * 1470) then both collision handles (1473 and 1471), registering a
 * destructor node for each. mwcc emits __sinit_d_a_obj_ct_mecha04.cpp from
 * these definitions. */
Mecha04ModelFilePtr gCtMechaObj04sModelFile(ASSET_CT_MECHA_OBJ04S_BMD);
Mecha04ModelFilePtr gCtMechaObj04lModelFile(ASSET_CT_MECHA_OBJ04L_BMD);
Mecha04ClsnFilePtr gCtMechaObj04sCollisionFile(ASSET_CT_MECHA_OBJ04S_KCL);
Mecha04ClsnFilePtr gCtMechaObj04lCollisionFile(ASSET_CT_MECHA_OBJ04L_KCL);
