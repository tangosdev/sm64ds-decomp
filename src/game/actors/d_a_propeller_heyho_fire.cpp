//cpp
/* daPropeller_Heyho_Fire_c -- the propeller Shy Guy's fireball (registry
 * profile PROPELLER_HEYHO_FIRE).
 *
 * A short-lived projectile with one state. InitResources binds the shared
 * model, sizes the hit cylinder and the wall-and-floor probe, and installs
 * the state through SetState, which stores it in mCurrentState and runs its
 * init handler, InitState0 (mStateTimer = 200). Behavior counts mStateTimer
 * down, runs the main handler, falls under gravity and rebuilds the model
 * matrix (UpdateModelMatrix).
 *
 * The main handler, State0, bursts the fireball in a puff when
 * mStateTimer reaches 0 or it touches ground or a wall. Otherwise, until a
 * metal player has deflected it, it calls CheckPlayerHit: when the
 * actor that touched the hit cylinder is the player (actor ID 0xbf) and not
 * vanished, the fireball bursts and hurts it -- unless the player is metal.
 * Then the fireball flips its facing half a turn, stops its horizontal
 * motion and launches upward (mSpeed.y 0x1e000, mVertAccel -0x4000). Each burst
 * plays sound 0x166 through func_02012694, which tail-calls
 * Sound::Play(3, id, pos).
 *
 * defer_codegen off keeps this file in ROM order. .text
 * 0x020feabc..0x020ff014, 13 functions. The out-of-line destructor is
 * the key function.
 */
#pragma defer_codegen off

#include "daPropeller_Heyho_Fire_c.h"
#include "types.h"
#include "common.h"
#include "SharedFilePtr.h"
#include "daPly_c.h"
#include "Sound.h"
#include "math/CalcLib.h"
#include "math/MtxStack.h"

struct BMD_File;

/* Was spelled `func_0211d610`, a name no symbols.txt defines. ov002's relocs
   record both loads (0x020fee10, 0x020fefc0) as `overlays(65,75)`; ov065's
   row there is the one unambiguous `kind:bss`, and CleanupResources already
   named it. It is the fireball's shared model file. */
extern SharedFilePtr data_ov065_0211d610;
/* The one state. InitState0 then State0. The initializer copies the two
   member pointers into data_ov002_02111190, which InitResources installs. */
extern daPropeller_Heyho_Fire_c::State data_ov002_02111190;

/* local extern: no shared header declares these. */
void func_02012694(unsigned int a, const Vector3 *v);

/* Empty. D1 then D0 follow from the members. */
daPropeller_Heyho_Fire_c::~daPropeller_Heyho_Fire_c() {}

/* The player-hit test. otherOwner is the ID of whatever touched the hit
   cylinder. The hurt is skipped when the cylinder's hitFlags has bit 0x10
   (dCc_c.h's unverified best-effort bit table reads it as "mega
   character"). */
void daPropeller_Heyho_Fire_c::CheckPlayerHit() {
    dActor_c *otherActor;
    daPly_c *player;
    int isPlayer; /* Preserve the observed widened comparison before the daPly_c cast. */
    int flags;
    Vector3 v;

    if (mdCcAc_c.otherOwner == 0) {
        return;
    }
    otherActor = dActor_c::FindWithID(mdCcAc_c.otherOwner);
    if (otherActor == 0) {
        return;
    }
    isPlayer = (otherActor->actorID == 0xbf);
    if (isPlayer == 0) {
        return;
    }
    player = static_cast<daPly_c *>(otherActor);
    if (player->mIsVanish != 0) {
        return;
    }
    if (player->mIsMetal != 0) {
        /* Deflected: mPrevAngle.y (which Behavior copies into mAngle.y)
           swings half a turn, so only the facing flips. mSpeed.x and mSpeed.z
           are the x and z of the velocity UpdatePosWithOnlySpeed adds, so
           zeroing them stops the horizontal motion; the vertical speed
           becomes 0x1e000 up with -0x4000 acceleration. */
        mDeflected = 1;
        Sound::Play(0, 0xa5, mCamSpacePos);
        mPrevAngle.y -= 0x8000;
        mSpeed.x = 0;
        mSpeed.y = 0x1e000;
        mSpeed.z = 0;
        mVertAccel = -0x4000;
        return;
    }
    flags = mdCcAc_c.hitFlags;
    SmallPoofDust();
    func_02012694(0x166, &mCamSpacePos);
    MarkForDestruction();
    if ((flags & 0x10) != 0) {
        return;
    }
    v.x = mPos.x;
    v.y = mPos.y;
    v.z = mPos.z;
    player->Hurt(v, 1, 0xc000, 1, 0, 1);
}

/* The state's main handler. The (u16) read matches the ROM's ldrh; the plain
   s16 read is an ldrsh. */
int daPropeller_Heyho_Fire_c::State0() {
    if (mStateTimer == 0 || mWithMeshClsn.IsOnGround() || mWithMeshClsn.IsOnWall()) {
        SmallPoofDust();
        func_02012694(0x166, &mCamSpacePos);
        MarkForDestruction();
        return 1;
    }
    if (mDeflected == 0)
        CheckPlayerHit();
    return 1;
}

/* The state's init handler. */
int daPropeller_Heyho_Fire_c::InitState0() {
    mStateTimer = 200;
    return 1;
}

/* Installs a state and runs its init handler, if it has one. */
int daPropeller_Heyho_Fire_c::SetState(State *state) {
    mCurrentState = state;
    State *s = mCurrentState;
    if (s->mInit == 0)
        return 1;
    return (this->*(s->mInit))();
}

/* The model matrix: position (shifted right by 3, Vec3_Asr) and the
   X/Y/Z angles, built in the scratch matrix and copied out. */
void daPropeller_Heyho_Fire_c::UpdateModelMatrix() {
    Vector3 v;
    Vec3_Asr(&v, &mPos, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, mAngle.x, mAngle.y, mAngle.z);
    mModel.mat4x3 = data_020a0e68;
}

/* Releases the shared file InitResources claimed. */
int daPropeller_Heyho_Fire_c::CleanupResources() {
    data_ov065_0211d610.Release();
    return 1;
}

/* Empty. */
void daPropeller_Heyho_Fire_c::OnPendingDestroy() {}

int daPropeller_Heyho_Fire_c::Render() {
    mModel.Render(0);
    return 1;
}

int daPropeller_Heyho_Fire_c::Behavior() {
    DecIfAbove0_Short(&mStateTimer);
    State *h = mCurrentState;
    if (h->mMain != 0) {
        (this->*(h->mMain))();
    }
    {
        /* Gravity, clamped at terminal velocity. mSpeed.z is read and written
           back unchanged -- the ROM really does reload and restore it here. */
        int speed = mSpeed.y;
        int accel = mVertAccel;
        int limit = mTerminalVelocity;
        int keep = mSpeed.z;
        int next = speed + accel;
        if (next >= limit)
            limit = next;
        mSpeed.y = limit;
        mSpeed.z = keep;
        UpdatePosWithOnlySpeed(&mdCcAc_c);
    }
    UpdateWMClsn(mWithMeshClsn, 0);
    mAngle.y = mPrevAngle.y;
    UpdateModelMatrix();
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}

int daPropeller_Heyho_Fire_c::InitResources() {
    BMD_File *file = dExtSimpleModel_c::LoadFile(data_ov065_0211d610);
    if (mModel.SetFile(file, 1, -1) == 0)
        return 0;
    unk_358 = param1 & 0xff;
    if (unk_358 > 1)
        unk_358 = 0;
    mTerminalVelocity = -0x64000;
    mdCcAc_c.Init(this, 0xa000, 0xa000, 0x200004, 0);
    mAngle.y = mPrevAngle.y;
    mWithMeshClsn.Init(this, 0xa000, 0xa000, 0, 0);
    SetState(&data_ov002_02111190);
    return 1;
}

/* PROPELLER_HEYHO_FIRE factory. */
daPropeller_Heyho_Fire_c *daPropeller_Heyho_Fire_c_classInit(void) {
    return new daPropeller_Heyho_Fire_c;
}

/* Retail copy order: InitState0, then State0.
   mwcc emits __sinit_d_a_propeller_heyho_fire.cpp. */
daPropeller_Heyho_Fire_c::State data_ov002_02111190 = {
    &daPropeller_Heyho_Fire_c::InitState0,
    &daPropeller_Heyho_Fire_c::State0,
};
