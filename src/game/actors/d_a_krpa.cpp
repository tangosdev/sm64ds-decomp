//cpp
/* daKrpa_c -- the flame spitter (KERONPA), ov070; it spits KERONPA_FIRE.
 *
 * It turns toward the closest visible player within 700.0 (0x800 a frame)
 * and pulses in size from a frame table. With a player tracked, and while
 * it stands above data_0209f32c, it counts 115 frames and then spits: on
 * frame 30 of that animation it spawns KERONPA_FIRE (actor 0x10f, see
 * d_a_kp_fr.cpp) 80.0 in front of itself and plays sound 0x105. A collider
 * hit with flag 0x10 knocks it away tumbling (a mega kill, state 3). A
 * touch while it is being eaten (flag 0x20000) sends it to state 2, which
 * poofs it once neither 0x20000 nor 0x40000 is set.
 *
 * One TU, 25 functions. It began as the old one-function files
 * concatenated in reverse ROM order.
 *
 * _ZTS8daKrpa_c 0x02123334. _ZTI8daKrpa_c 0x02123340.
 * _ZTV8daKrpa_c 0x02123370. The inline destructor in the header is the key
 * function: D1 at 0x02121118, D0 at 0x02121160, no D2.
 *
 * DO NOT "TIDY" THESE -- each one is load-bearing:
 *   mwcc emits one .text section per ordinary definition in reverse source
 *   order; the inline destructor group comes out retail D1 then D0, no D2.
 *   M48, the array wrapper for InitResources' IDENTITY_MATRIX4X3 copy. This
 *   TU uses the nested Matrix4x3 (.t) that math/Matrix.h brings in via
 *   dExtAnmModel_c.h; putting common.h first would drop .t.
 *   ExecuteKnockedAway calls dBgCh_Actr_UpdateDiscreteNoLava_veneer: the
 *   veneer is the retail call destination.
 */

// What stays, and why:
// - The data_ov070_* SharedFilePtr / BCA / frame tables / state records are
//   overlay .data, not this TU's data claim (consumers: the spit sequencer
//   and the size-pulse reader in this TU).
// - The two Matrix4x3_FromRotation* helpers keep a local extern "C": this
//   TU's Matrix4x3 is the nested one, and math/MtxStack.h does not declare
//   FromRotationXYZExt.
/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_krpa_AssetId { ASSET_KERONPA_BMD = 0x0351 };

#include "daKrpa_c.h"
#include "SpawnInfo.h"
#include "dBgCh_Gnd.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "nitro/fx/fx.h"
#include "Sound.h"
#include "math/Approach.h"
#include "math/CalcLib.h"
#include "Particle__System.h"

extern int IDENTITY_MATRIX4X3[];
extern int data_0209f32c;
extern short data_02082214[];

/* local extern: arm9 matrix helpers this TU calls with its nested
   Matrix4x3; no shared header declares them in that spelling. */
extern "C" {
void Matrix4x3_FromRotationXYZExt(void *m, int x, int y, int z);
void Matrix4x3_FromRotationY(void *m, int angle);
}

// @symbol daKrpa_c_classInit
/* `return new daKrpa_c()` emits 0x50, calling the inherited
 * fBase_c::operator new (`_ZN7fBase_cnwEm`). Historical aliases:
 * daKrpa_c_Spawn and FlameChomp_Spawn. */
extern "C" daKrpa_c *daKrpa_c_classInit() {
    return new daKrpa_c();
}

/* Reconstructed source-style names: SM64DS proves the daKrpa_c RTTI identity,
 * KERONPA registry ID, descriptor/factory relationship, and object shape;
 * later EAD lineage supplies the spelling prior. Exact original SM64DS
 * spellings are not preserved. Historical aliases: daKrpa_c_Spawn and
 * daKrpa_c_SpawnInfo. The descriptor is at ov070:0x0212334c. */
extern "C" SpawnInfo<daKrpa_c> g_profile_KERONPA = {
    daKrpa_c_classInit, 0x010e, 0x0081, 0x00000003, 0x00000000, 0x0002d000, 0x01000000, 0x00ed8000};
// @symbol _ZN21daKrpaFrameController9SetFramesEPjjj
void daKrpaFrameController::SetFrames(u32 *frames, u32 count, u32 mode) {
    this->frames = frames;
    this->count = count;
    this->mode = mode;
    this->cursor = 0;
}
// @symbol _ZN21daKrpaFrameController9NextFrameEv
u32 daKrpaFrameController::NextFrame() {
    switch (this->mode) {
    case 0:
        if (this->cursor < this->count)
            ++this->cursor;
        break;
    case 1:
        ++this->cursor;
        this->cursor %= this->count;
        break;
    }
    return this->frames[this->cursor];
}

// @symbol _ZN8daKrpa_c13InitResourcesEv
struct M48 {
    int w[12];
};
struct KrpaModelFilePtr : SharedFilePtr {
    KrpaModelFilePtr(u32 fileID);
    ~KrpaModelFilePtr();
};
extern KrpaModelFilePtr gKeronpaModelFile;

int daKrpa_c::InitResources() {
    Vector3 offset;
    void *bmd = dExtSimpleModel_c::LoadFile(gKeronpaModelFile);
    int groundDistance;
    mAnmModel.SetFile((BMD_File *)bmd, 1, 1);
    if (!mShadowModel.InitCylinder())
        return 0;

    offset.x = 0;
    offset.y = -0x32000;
    offset.z = 0;
    mdCcAcPos_c.Init(this, offset, 0x32000, 0x64000, 0x200002, 0x8000);
    mWithMeshClsn.Init(this, 0x32000, 0x32000, 0, 0);

    mVertAccel = 0;
    mTerminalVelocity = 0;
    ChangeState(0);
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    *(M48 *)&mMatrix = *(M48 *)IDENTITY_MATRIX4X3;

    dBgCh_Gnd ground;
    ground.SetObjAndPos(mPos, this);
    if (ground.DetectClsn())
        groundDistance = (mPosY - ground.clsnY) + 0x1e000;
    else
        groundDistance = 0x1f4000;
    mGroundDistance = groundDistance;
    UpdateModelAndShadow();
    return 1;
}

// @symbol _ZN8daKrpa_c8BehaviorEv
int daKrpa_c::Behavior() {
    ExecuteState();
    UpdateModelAndShadow();
    return 1;
}

// @symbol _ZN8daKrpa_c6RenderEv
int daKrpa_c::Render() {
    mAnmModel.Render(&mScale);
    return 1;
}

// @symbol _ZN8daKrpa_c16OnPendingDestroyEv

void daKrpa_c::OnPendingDestroy() {}

// @symbol _ZN8daKrpa_c16CleanupResourcesEv

int daKrpa_c::CleanupResources() {
    gKeronpaModelFile.Release();
    return 1;
}
extern daKrpaState data_ov070_021236ac[];
// @symbol _ZN8daKrpa_c11ChangeStateEi
void daKrpa_c::ChangeState(int state) {
    this->mStateMethods = &data_ov070_021236ac[state];
    this->EnterState();
}
// @symbol _ZN8daKrpa_c10EnterStateEv
void daKrpa_c::EnterState() {
    daKrpaStateMethod *method = &this->mStateMethods->init;
    (this->**method)();
}
// @symbol _ZN8daKrpa_c12ExecuteStateEv
void daKrpa_c::ExecuteState() {
    daKrpaStateMethod *method = &this->mStateMethods->behavior;
    (this->**method)();
}
// @symbol _ZN8daKrpa_c9EnterIdleEv
extern char data_ov070_021234c4[];
extern char data_ov070_021234dc[];
extern u32 data_ov070_02122404[];
extern u32 data_ov070_021222e8[];

int daKrpa_c::EnterIdle() {
    this->mAnmModel.SetAnim((BCA_File *)data_ov070_021234c4, 0, 0x1000, 0);
    this->mFrameController.SetFrames(data_ov070_02122404, 0x64, 1);
    this->mStateTimer = 0x73;
    this->mStateIndex = 0;
    return 1;
}
// @symbol _ZN8daKrpa_c11ExecuteIdleEv
int daKrpa_c::ExecuteIdle() {
    if (this->mPlayer) {
        if (this->mPosY > data_0209f32c) {
            if (DecIfAbove0_Byte(&this->mStateTimer) == 0)
                this->ChangeState(1);
        }
    } else {
        this->mStateTimer = 0x73;
    }
    this->mAnmModel.Advance();
    u32 frame = this->mFrameController.NextFrame();
    this->mScaleX = frame;
    this->mScaleY = frame;
    this->mScaleZ = frame;
    this->TrackClosestPlayer();
    this->HandlePlayerHit();
    this->mdCcAcPos_c.Clear();
    this->mdCcAcPos_c.Update();
    return 1;
}
// @symbol _ZN8daKrpa_c9EnterSpitEv
int daKrpa_c::EnterSpit() {
    this->mAnmModel.SetAnim((BCA_File *)data_ov070_021234dc, 0x40000000, 0x1000, 0);
    this->mFrameController.SetFrames(data_ov070_021222e8, 0x47, 0);
    this->mStateIndex = 1;
    return 1;
}
// @symbol _ZN8daKrpa_c11ExecuteSpitEv
int daKrpa_c::ExecuteSpit() {
    if (this->mFrameController.cursor == 0x1e) {
        Vector3 firePos;
        int idx = (int)(u16)this->mAngleY >> 4;
        int cosv = data_02082214[idx * 2 + 1];
        int sinv = data_02082214[idx * 2];
        int offZ = FX_MUL(cosv, 0x50000);
        int offX = FX_MUL(sinv, 0x50000);
        int x = this->mPosX + offX;
        int z = this->mPosZ + offZ;
        int y = this->mPosY - 0x29000;
        firePos.x = x;
        firePos.z = z;
        firePos.y = y;
        dActor_c::Spawn(0x10f, 0, firePos, &this->mAngle, this->mAreaId, -1);
        func_0201267c(0x105, &this->mCamSpacePos);
    }
    if (this->mFrameController.cursor == this->mFrameController.count)
        this->ChangeState(0);
    this->mAnmModel.Advance();
    u32 frame = this->mFrameController.NextFrame();
    this->mScaleX = frame;
    this->mScaleY = frame;
    this->mScaleZ = frame;
    this->TrackClosestPlayer();
    this->HandlePlayerHit();
    this->mdCcAcPos_c.Clear();
    this->mdCcAcPos_c.Update();
    return 1;
}
// @symbol _ZN8daKrpa_c10EnterEatenEv
int daKrpa_c::EnterEaten() {
    this->mdCcAcPos_c.Clear();
    this->mStateIndex = 2;
    return 1;
}
// @symbol _ZN8daKrpa_c12ExecuteEatenEv
int daKrpa_c::ExecuteEaten() {
    int flags = this->mFlags;
    int blocked = (flags & 0x20000) != 0;
    if (!blocked) {
        blocked = (flags & 0x40000) != 0;
        if (!blocked) {
            this->PoofDust();
            this->MarkForDestruction();
        }
    }
    return 1;
}
// @symbol _ZN8daKrpa_c16EnterKnockedAwayEv
int daKrpa_c::EnterKnockedAway() {
    Sound::PlayBank0(9, this->mCamSpacePos);
    this->mFlags &= ~1;
    this->mVertAccel = -0x2000;
    this->mTerminalVelocity = -0x3c000;
    this->mHorzSpeed = 0xa000;
    this->mVertSpeed = 0x28000;
    this->mScaleX = 0x1000;
    this->mScaleY = 0x1000;
    this->mScaleZ = 0x1000;
    this->mStateTimer = 0x2d;
    this->mAnmModel.SetAnim((BCA_File *)data_ov070_021234c4, 0, 0x1000, 0);
    Particle::System::NewSimple(0x43, this->mPosX, this->mPosY, this->mPosZ);
    Particle::System::NewSimple(0x44, this->mPosX, this->mPosY, this->mPosZ);
    this->mStateIndex = 3;
    return 1;
}
// @symbol _ZN8daKrpa_c18ExecuteKnockedAwayEv
/* The collision update veneer is retained because it is the retail call
 * destination; the rest are ordinary real class calls. */
int daKrpa_c::ExecuteKnockedAway() {
    this->mAngleX = this->mAngleX - 0x1000;
    this->mAnmModel.Advance();
    this->UpdatePos(&this->mdCcAcPos_c);
    dBgCh_Actr_UpdateDiscreteNoLava_veneer(&this->mWithMeshClsn);
    if (!this->mWithMeshClsn.JustHitGround()) {
        if (DecIfAbove0_Byte(&this->mStateTimer) != 0)
            goto end;
    }
    this->PoofDust();
    this->MarkForDestruction();
end:
    return 1;
}
// @symbol _ZN8daKrpa_c20UpdateModelAndShadowEv
void daKrpa_c::UpdateModelAndShadow() {
    if (this->mStateIndex == 3) {
        Matrix4x3_FromRotationXYZExt(&this->mAnmModel.mat4x3, this->mAngleX, this->mAngleY,
                                     this->mAngleZ);
    } else {
        Matrix4x3_FromRotationY(&this->mAnmModel.mat4x3, this->mAngleY);
    }
    this->mAnmModel.mat4x3.t.x = this->mPosX >> 3;
    this->mAnmModel.mat4x3.t.y = this->mPosY >> 3;
    this->mAnmModel.mat4x3.t.z = this->mPosZ >> 3;
    this->mMatrix.t.x = this->mPosX >> 3;
    this->mMatrix.t.y = this->mPosY >> 3;
    this->mMatrix.t.z = this->mPosZ >> 3;
    DropShadowRadHeight(this->mShadowModel, this->mMatrix, this->mScaleX * 0x46,
                        this->mGroundDistance, 0xf);
}

// @symbol _ZN8daKrpa_c18TrackClosestPlayerEv
void daKrpa_c::TrackClosestPlayer() {
    Player *player = this->ClosestNonVanishPlayer();
    if (!player) {
        this->mPlayer = 0;
        return;
    }
    if (Vec3_Dist(&this->mPos, &player->mPos) >= 0x2bc000) {
        this->mPlayer = 0;
        return;
    }
    this->mPlayer = player;
    ApproachLinear(this->mAngleY, Vec3_HorzAngle(&this->mPos, &player->mPos), 0x800);
}
// @symbol _ZN8daKrpa_c15HandlePlayerHitEv
void daKrpa_c::HandlePlayerHit() {
    u32 id = this->mdCcAcPos_c.otherOwner;
    if (id == 0)
        return;
    dActor_c *found = dActor_c::FindWithID(id);
    if (found == 0)
        return;
    int isPlayer = (found->actorID == 0xbf);
    if (isPlayer == 0)
        return;
    Player *player = (Player *)found;
    int beingEaten = ((this->mFlags & 0x20000) != 0);
    if (beingEaten != 0) {
        this->ChangeState(2);
        return;
    }
    if ((this->mdCcAcPos_c.hitFlags & 0x10) == 0)
        return;
    this->mPrevAngleY = Vec3_HorzAngle(&player->mPos, &this->mPos);
    this->mAngleY = (short)(this->mPrevAngleY + 0x8000);
    player->IncMegaKillCount();
    this->ChangeState(3);
}

// @symbol _ZN8daKrpa_c13OnYoshiTryEatEv

int daKrpa_c::OnYoshiTryEat() {
    return 5;
}

// @symbol _ZN8daKrpa_cD0Ev

/* No separate body: the inline class destructor plus vtable instantiation
 * makes mwcc emit the retail deleting variant after D1. */

// @symbol _ZN8daKrpa_cD1Ev

/* No separate body: the inline class destructor emits this complete variant
 * first, through the class vtable instantiated in this TU. */

/* Retail initializer order is the model handle (file 0x351) with its
 * destructor registration, then the eight {init,behavior} state-method
 * records copied in ascending table slot. */
KrpaModelFilePtr gKeronpaModelFile(ASSET_KERONPA_BMD);
daKrpaState data_ov070_021236ac[4] = {
    {(daKrpaStateMethod)&daKrpa_c::EnterIdle, (daKrpaStateMethod)&daKrpa_c::ExecuteIdle},
    {(daKrpaStateMethod)&daKrpa_c::EnterSpit, (daKrpaStateMethod)&daKrpa_c::ExecuteSpit},
    {(daKrpaStateMethod)&daKrpa_c::EnterEaten, (daKrpaStateMethod)&daKrpa_c::ExecuteEaten},
    {(daKrpaStateMethod)&daKrpa_c::EnterKnockedAway,
     (daKrpaStateMethod)&daKrpa_c::ExecuteKnockedAway},
};
