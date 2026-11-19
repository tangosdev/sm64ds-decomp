//cpp
/* daObjMaruta_c -- the abstract base of the two rolling logs, and the shared
 * implementation both of them forward to.
 *
 * Licensed .text run: ov080 0x02126fbc .. 0x0212766c, the two destructor
 * variants (D0 0x02126fbc, D1 0x02127014) and then eight methods.
 *
 * The file is in ROM order under `#pragma defer_codegen off`, with the two
 * destructor forcers first. include/daObjMaruta_c.h keeps the destructor in
 * the class body so that both descendants inline it; deferred codegen emits
 * that inline body as D1 then D0, and the cartridge has D0 below D1. Do not
 * reorder.
 *
 * ABSTRACT: no classInit in this TU. The leaves own the factories
 * (ov022 daObjFlMaruta_c, ov030 daObjHmMaruta_c).
 *
 * Notes:
 * - func_ov080_021274ac / 0212714c / 021270dc keep C linkage for now.
 *   daObjFlMaruta_c (ov022) and daObjHmMaruta_c (ov030) forward
 *   InitResources, Behavior and CleanupResources into them, so naming them
 *   means converting both leaves in the same change.
 * - func_02012694 (sound 0x1e at mCamSpacePos), Matrix4x3_FromRotationXYZExt,
 *   Vec3_Sub and AngleDiff are arm9 C helpers no header declares yet.
 * - Abstract class: the leaves own classInit.
 */

#pragma defer_codegen off

#include "common.h"
#include "daObjMaruta_c.h"
#include "daPly_c.h"
#include "SharedFilePtr.h"
#include "Sound.h"
#include "dBgW.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "math/CalcLib.h"

#include "ResourceDescriptor.h"
#include "math/MtxStack.h"

void func_02012694(unsigned int a, const Vector3 *b);
extern "C" int AngleDiff(int a, int b);


/* -------------------------------------------------------------------------- */
/* ROM 0x02126fbc (D0, size 0x58) and 0x02127014 (D1, size 0x44)              */
/* -------------------------------------------------------------------------- */
/* No source here: both destructor variants come from the ONE inline body in
 * include/daObjMaruta_c.h, which daObjFlMaruta_c and daObjHmMaruta_c need
 * visible to inline it.
 *
 * The two calls below are never executed. Under `#pragma defer_codegen off`
 * the compiler emits ordinary functions at parse time in source order, so the
 * delete-expression pulls the deleting variant out of line first and the
 * explicit destructor call pulls the complete-object variant out second --
 * the cartridge's D0-then-D1 order, with no D2. */

/* Not called. Forces the out-of-line copy of the deleting destructor. */
void daObjMaruta_c_EmitDeletingDestructor(daObjMaruta_c *p) {
    delete p;
}

/* Not called. Forces the out-of-line copy of the inline destructor. */
void daObjMaruta_c_EmitDestructor(daObjMaruta_c *p) {
    p->~daObjMaruta_c();
}

/* Vtable slot 27, and this class's KEY FUNCTION: the destructor is inline in
   the class body, so the first out-of-line virtual declared anchors
   _ZTV13daObjMaruta_c and the RTTI chain, and this TU is where it is defined.
   dBgActor_c::KillByMegaChar is non-virtual, so the unqualified call is the
   direct `bl` the ROM has. */
void daObjMaruta_c::OnHitByMegaChar(daPly_c &player) {
    player.IncMegaKillCount();
    func_02012694(0x1e, &mCamSpacePos);
    KillByMegaChar(player);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- 0x02127094, size 0x48 */
/* -------------------------------------------------------------------------- */
void daObjMaruta_c::UpdateModelTransform() {
    Matrix4x3_FromRotationXYZExt(&mModel.mat4x3, mAngle.x, mAngle.y, mAngle.z);
    mModel.mat4x3.t.x = mPos.x >> 3;
    mModel.mat4x3.t.y = mPos.y >> 3;
    mModel.mat4x3.t.z = mPos.z >> 3;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov080_021270dc, 0x021270dc, size 0x48 */
/* -------------------------------------------------------------------------- */
int daObjMaruta_c::func_ov080_021270dc(ResourceDescriptor *arg) {
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    arg->model->Release();
    arg->collision->Release();
    return 1;
}

/* Dispatches through the dExtSimpleModel_c at 0xd4 (dBgActor_c's mModel, inherited) --
   dExtSimpleModel_c::Render(const Vector3 *) with a null scale, matching the ROM's
   literal-0 argument. */
s32 daObjMaruta_c::Render() {
    mModel.Render(0);
    return 1;
}

static inline int IsActorID(dActor_c *actor, int id) {
    return actor->actorID == id;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov080_0212714c, 0x0212714c, size 0x360 */
/* -------------------------------------------------------------------------- */
int daObjMaruta_c::func_ov080_0212714c(int *maxDist) {
    Vector3 offset;
    int len;
    int ang;
    s16 v;
    int horz;

    if (UpdateKillByMegaChar(-0x1500, 0, 0, 0) != 0)
        return 1;
    if ((int)((mFlags & 8) != 0) != 0)
        return 1;

    if (mHitByPlayer == 0) {
        if (DecIfAbove0_Byte(&mHitTimer)) {
            if (mRollAngVel > 0) {
                mRollAngVel -= 0x10;
            } else {
                mRollAngVel += 0x10;
            }
            v = mRollAngVel;
            if (v <= 0x10 && v >= -0x10)
                mRollAngVel = 0;
        } else {
            v = mRollAngVel;
            if (v != 0x100) {
                if (v > 0x100) {
                    mRollAngVel -= 0x10;
                } else {
                    mRollAngVel += 0x10;
                }
                v = mRollAngVel;
                if (v < 0x110 && v > 0xf0)
                    mRollAngVel = 0x100;
            }
        }
        if (Vec3_Dist(&mPos, &mRestPos) < 0x2000)
            mRollAngVel = 0;
    } else {
        horz = Vec3_HorzAngle(&mPos, &mHitPos);
        AngleDiff(horz, mAngle.y);
        ang = AngleDiff(horz, mAngle.y);
        if (ang > 0x4000) {
            mRollAngVel -= 0x10;
        } else {
            mRollAngVel += 0x10;
        }
        v = mRollAngVel;
        if (v < -0x200)
            v = -0x200;
        else if (v > 0x200)
            v = 0x200;
        mRollAngVel = v;
    }

    {
        int d = mRollAngVel;
        mHorzSpeed = (d / 64) << 12;
    }
    UpdatePos(0);
    Vec3_Sub(&offset, &mPos, &mRestPos);
    len = VEC_Mag(&offset);
    ang = Vec3_HorzAngle(&mPos, &mRestPos);
    ang = AngleDiff(ang, mAngle.y);
    if (ang >= 0x7f00)
        ang = 0x8000;
    if (ang <= 0x100)
        ang = 0;
    if (len > *maxDist) {
        mPos.x = mPrevPos.x;
        mPos.y = mPrevPos.y;
        mPos.z = mPrevPos.z;
    }
    if (ang == mPathAngDiff || ang == 0x8000) {
        mPos.x = mPrevPos.x;
        mPos.y = mPrevPos.y;
        mPos.z = mPrevPos.z;
        mHitTimer = 10;
    }
    mAngle.x += mRollAngVel;
    if ((mAngle.x & 0x1fff) < 0x210 && mRollAngVel != 0) {
        if (mSoundTimer == 0) {
            Sound::PlayBank3(0x25, mCamSpacePos);
            mSoundTimer = 14;
        }
    } else {
        DecIfAbove0_Short(&mSoundTimer);
    }
    UpdateModelTransform();
    {
        if (IsActorID(this, 0x66)) {
            if (IsClsnInRangeOnScreen(0, 0) != 0)
                UpdateClsnPosAndRot();
        } else {
            if (IsClsnInRange(0, 0) != 0)
                UpdateClsnPosAndRot();
        }
    }
    mHitByPlayer = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov080_021274ac, 0x021274ac, size 0xe0 */
/* -------------------------------------------------------------------------- */
int daObjMaruta_c::func_ov080_021274ac(ResourceDescriptor *arg) {
    struct Vector3 d;
    mModel.SetFile(dExtSimpleModel_c::LoadFile(*arg->model), 1, -1);
    UpdateModelTransform();
    UpdateClsnPosAndRot();
    mMeshCollider.SetFile(dBgW_Kc::LoadFile(*arg->collision), mClsnMat, 0x1000, mAngle.y,
                          *(arg->clps));
    mMeshCollider.SetBeforeClsnCallback(&dBgW::UpdatePosWithVelocity);
    mMeshCollider.SetStoodOnCallback(&daObjMaruta_c::StoodOnCallback);
    mRestPos.x = mPos.x;
    mRestPos.y = mPos.y;
    mRestPos.z = mPos.z;
    Vec3_Sub(&d, &mPos, &mRestPos);
    VEC_Mag(&d);
    mPathAngDiff = AngleDiff(Vec3_HorzAngle(&mPos, &mRestPos), mAngle.y);
    return 1;
}

static inline int IsPlayerActor(dActor_c *actor) {
    return actor->actorID == 0xbf;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- 0x0212758c, size 0xcc */
/* -------------------------------------------------------------------------- */
/* If the other actor's actorID is PLAYER (0xbf), copy its mPos into mHitPos,
   arm mHitByPlayer / mHitTimer, and shove that actor along mAngle.y by half
   of mHorzSpeed. */
void daObjMaruta_c::HandlePlayerContact(dActor_c *other) {
    int half;

    if (IsPlayerActor(other)) {
        Vector3 *position = &other->mPos;
        mHitPos = *position;
        mHitByPlayer = 1;
        mHitTimer = 120;

        half = mHorzSpeed >> 1;
        position->x += FX_MUL(half, FX_SinIdx((u16)mAngle.y >> 4));
        position->z += FX_MUL(half, FX_CosIdx((u16)mAngle.y >> 4));
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- 0x02127658, size 0x14 */
/* -------------------------------------------------------------------------- */
/* Arg-shifting tail-call veneer, handed to the dBgW at mMeshCollider by
   func_ov080_021274ac above. Drops the first argument and forwards the next
   two; long_calls emits the pooled `ldr ip,[pc,#8]; bx ip` absolute
   tail-call. Bracketed closed immediately: long_calls is positional. */
#pragma long_calls on
void daObjMaruta_c::StoodOnCallback(dBgW *collider, dActor_c *log, dActor_c *other) {
    static_cast<daObjMaruta_c *>(log)->HandlePlayerContact(other);
}
#pragma long_calls off
