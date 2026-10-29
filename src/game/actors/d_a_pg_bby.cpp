//cpp
/* leftovers:
 * - The local file-handle wrappers' constructor/destructor veneers are
 *   mapped to the retail destinations in this TU's manifest.
 * - The state steps stay func_ov072_*. The cartridge has no names
 *   for them.
 *
 * daPgBby_c, the lost baby penguin (PENGUIN_BABY 256). ov072 .text
 * 0x02120c58..0x02121ffc. RTTI names daPgBby_c. classInit is
 * reconstructed. Six states, declared on daPgBby_c.h.
 */
#include "daPgBby_c.h"
#include "SpawnInfo.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "dBgCh_Gnd.h"
#include "dBgCh_Actr.h"
#include "dClipper.h"
#include "nitro/fx/fx.h"
#include "Sound.h"
#include "cstd.h"
#include "math/Approach.h"

/* A plain twelve-word copy of the carry matrix. */
struct PgBbyMtx {
    s32 m[12];
};

/* Descriptive local types and object names, not recovered source spellings.
 * dExtSimpleModel_c and animation handles use different veneers. */
struct PgBbyModelFile : SharedFilePtr {
    PgBbyModelFile(u32 fileID);
    ~PgBbyModelFile();
};

struct PgBbyAnimationFile : SharedFilePtr {
    PgBbyAnimationFile(u32 fileID);
    ~PgBbyAnimationFile();
};

typedef char PgBbyModelFile_size_must_be_0x08[sizeof(PgBbyModelFile) == 0x08 ? 1 : -1];
typedef char PgBbyAnimationFile_size_must_be_0x08[sizeof(PgBbyAnimationFile) == 0x08 ? 1 : -1];

extern "C" {
/* Parameter names keep this TU's @NNN. */
typedef void BgchInitAt_(void *thiz, void *actor, int r, int h, void *v, int b);

int Vec3_Dist(const Vector3 *a, const Vector3 *b);
short Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
unsigned short DecIfAbove0_Short(unsigned short *p);
void Vec3_Asr(Vector3 *d, Vector3 *s, int sh);
void Matrix4x3_FromRotationY(void *m, int angle);

extern short data_02082214[];   /* sine/cosine table */
extern dClipper data_0209f43c;
extern Matrix4x3 data_0209b3ec;

extern PgBbyModelFile gPenguinBabyModelFile;
extern PgBbyAnimationFile gPenguinBabyDiveAnimationFile;
extern PgBbyAnimationFile gPenguinBabyWaitAnimationFile;
extern PgBbyAnimationFile gPenguinBabyStandUpAnimationFile;
extern PgBbyAnimationFile gPenguinBabySlideAnimationFile;
extern PgBbyAnimationFile gPenguinBabyWalkAnimationFile;
extern SharedFilePtr *const gPenguinBabyAnimationFiles[5];
extern Vector3 gPenguinBabyCarryOffsets[4];
extern daPgBby_c::State gPenguinBabyStates[6];
}

// @symbol daPgBby_c_classInit
extern "C" daPgBby_c *daPgBby_c_classInit()
{
    return new daPgBby_c();
}

// @symbol _ZN9daPgBby_c13OnTurnIntoEggER6Player
void daPgBby_c::OnTurnIntoEgg(Player &player)
{
    MarkForDestruction();
}

// @symbol _ZN9daPgBby_c13InitResourcesEv
s32 daPgBby_c::InitResources()
{
    BMD_File *f = (BMD_File *)dExtSimpleModel_c::LoadFile(gPenguinBabyModelFile);
    mAnmModel.SetFile(f, 1, -1);
    int i;
    for (i = 0; i < 5; i++) {
        dExtFrameCtrl_c::LoadFile(*gPenguinBabyAnimationFiles[i]);
    }
    if (mShadowModel.InitCylinder() == 0) return 0;
    mCylClsn.Init(this, 0x28000, 0x50000, 0x800004, 0x9000);
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    mSpawnPosX = mPosX;
    mSpawnPosY = mPosY;
    mSpawnPosZ = mPosZ;
    mScaleX = 0x400;
    mScaleY = 0x400;
    mScaleZ = 0x400;
    mWithMeshClsn.Init(this, 0x32000, 0x32000, 0, 0);
    mEatingPlayer = 0;
    mCarrier = 0;
    func_ov072_02121d50(0);
    func_ov072_021210c4();
    return 1;
}

// @symbol _ZN9daPgBby_c8BehaviorEv
s32 daPgBby_c::Behavior()
{
    if (IsTooFarAwayFromPlayer(0x7d0000) && mWithMeshClsn.IsOnGround()) {
        func_ov072_02120d04();
    } else {
        if (mMother == 0)
            mMother = FindWithActorID(0x101, 0);
        mRespawnTimer = 0x384;
        func_ov072_02121cdc();
        func_ov072_021210c4();
    }
    return 1;
}

// @symbol _ZN9daPgBby_c6RenderEv
s32 daPgBby_c::Render()
{
    unsigned int f = mFlags;
    int b = ((f & 0x40000) != 0);
    if (b) return 1;
    mAnmModel.Render(&mScale);
    return 1;
}

// @symbol _ZN9daPgBby_c16OnPendingDestroyEv
void daPgBby_c::OnPendingDestroy()
{
}

// @symbol _ZN9daPgBby_c16CleanupResourcesEv
s32 daPgBby_c::CleanupResources()
{
    gPenguinBabyModelFile.Release();

    int i = 0;
    do {
        gPenguinBabyAnimationFiles[i]->Release();
        i++;
    } while (i < 5);

    return 1;
}

// @symbol _ZN9daPgBby_c19func_ov072_02121d50Ei
/* Switch to state `state` and run its enter step. */
void daPgBby_c::func_ov072_02121d50(int state)
{
    mState = &gPenguinBabyStates[state];
    func_ov072_02121d18();
}

// @symbol _ZN9daPgBby_c19func_ov072_02121d18Ev
/* Run the current state's enter step. */
void daPgBby_c::func_ov072_02121d18()
{
    StateFunc *step = &mState->enter;
    (this->**step)();
}

// @symbol _ZN9daPgBby_c19func_ov072_02121cdcEv
/* Run the current state's per-frame update step. */
void daPgBby_c::func_ov072_02121cdc()
{
    StateFunc *step = &mState->update;
    (this->**step)();
}

// @symbol _ZN9daPgBby_c19func_ov072_02121c94Ev
/* State 0, enter. */
int daPgBby_c::func_ov072_02121c94()
{
    mSubState = 0;
    mAnmModel.SetAnim((BCA_File *)gPenguinBabyWaitAnimationFile.filePtr, 0, 0x1000, 0);
    mStateId = 0;
    return 1;
}

// @symbol _ZN9daPgBby_c19func_ov072_02121a84Ev
/* State 0, update: keep a comfortable distance from the closest player,
   walking away when it comes near and back when it wanders off. */
int daPgBby_c::func_ov072_02121a84()
{
    Player *player;
    int dist;
    short angle;
    int frame;

    player = ClosestPlayer();
    dist = Vec3_Dist(&mPos, &player->mPos);
    switch (mSubState) {
    case 0:
        angle = (short)(Vec3_HorzAngle(&mPos, &player->mPos) + 0x8000);
        ApproachLinear(mAngleY, angle, 0x514);
        mPrevAngleY = mAngleY;
        mHorzSpeed = 0x5000;
        if (dist >= 0x1c2000) {
            mSubState = 2;
        }
        break;
    case 1:
        angle = Vec3_HorzAngle(&mPos, &player->mPos);
        ApproachLinear(mAngleY, angle, 0x514);
        mPrevAngleY = mAngleY;
        mHorzSpeed = 0x5000;
        if (dist > 0x384000) {
            mSubState = 2;
        } else if (dist < 0x1c2000) {
            mSubState = 2;
        }
        break;
    case 2:
        mHorzSpeed = 0;
        if (dist < 0x15e000) {
            mSubState = 0;
        } else if (dist >= 0x226000) {
            if (dist < 0x384000) {
                mSubState = 1;
            }
        }
        break;
    }
    if (mHorzSpeed != 0) {
        mAnmModel.SetAnim((BCA_File *)gPenguinBabyWalkAnimationFile.filePtr, 0, 0x1000, 0);
    } else {
        mAnmModel.SetAnim((BCA_File *)gPenguinBabyWaitAnimationFile.filePtr, 0, 0x1000, 0);
    }
    mAnmModel.speed = 0x1000;
    mAnmModel.Advance();
    func_ov072_02120e20();
    func_ov072_02120ddc();
    UpdatePos(&mCylClsn);
    func_ov072_02120fd4(&mWithMeshClsn);
    func_ov072_02120f14();
    func_ov072_02120e50();
    mCylClsn.Clear();
    mCylClsn.Update();
    if (mAnmModel.file == (BCA_File *)gPenguinBabyWalkAnimationFile.filePtr) {
        frame = (int)(((u32)mAnmModel.currFrame << 4) >> 16);
        if (frame == 9 || frame == 0x15) {
            func_0201267c(0xf3, &mCamSpacePos);
        }
    }
    return 1;
}

// @symbol _ZN9daPgBby_c19func_ov072_02121a28Ev
/* State 1, enter: a hop. */
int daPgBby_c::func_ov072_02121a28()
{
    mHorzSpeed = 0x19000;
    mVertSpeed = 0xc000;
    mAnmModel.SetAnim((BCA_File *)gPenguinBabyDiveAnimationFile.filePtr, 0x40000000, 0x1000, 0);
    mSubState = 0;
    mStateId = 1;
    return 1;
}

// @symbol _ZN9daPgBby_c19func_ov072_021218dcEv
/* State 1, update. */
int daPgBby_c::func_ov072_021218dc()
{
    switch (mSubState) {
    case 0:
        if (mAnmModel.Finished()) {
            mAnmModel.SetAnim((BCA_File *)gPenguinBabySlideAnimationFile.filePtr, 0, 0x1000, 0);
            mAnmModel.speed = 0x1000;
            mSubState++;
        }
        break;
    case 1:
        mHorzSpeed -= 0x3000;
        if (mHorzSpeed < 0) {
            mAnmModel.SetAnim((BCA_File *)gPenguinBabyStandUpAnimationFile.filePtr, 0x40000000,
                              0x1000, 0);
            mHorzSpeed = 0;
            mSubState++;
        }
        break;
    case 2:
        if (mAnmModel.Finished()) {
            func_ov072_02121d50(0);
        }
        break;
    }
    mAnmModel.Advance();
    UpdatePos(&mCylClsn);
    func_ov072_02120fd4(&mWithMeshClsn);
    func_ov072_02120f14();
    func_ov072_02120e50();
    mCylClsn.Clear();
    mCylClsn.Update();
    return 1;
}

// @symbol _ZN9daPgBby_c19func_ov072_02121890Ev
/* State 2, enter: picked up. */
int daPgBby_c::func_ov072_02121890()
{
    mAnmModel.SetAnim((BCA_File *)gPenguinBabyWaitAnimationFile.filePtr, 0, 0x1000, 0);
    mAnmModel.speed = 0x1000;
    mStateId = 2;
    return 1;
}

// @symbol _ZN9daPgBby_c19func_ov072_021217acEv
/* State 2, update: face where the carrier faces; once no longer held, drop
   to the ground under the carrier and return to state 0. */
int daPgBby_c::func_ov072_021217ac()
{
    mAngleY = mCarrier->mAngleY;
    mPrevAngleY = mAngleY;
    u32 flags = mFlags;
    int held = (flags & 0x100) != 0;
    if (held) {
        int thrown = (flags & 0x2000) != 0;
        if (!thrown)
            goto animate;
    }
    {
        Player *carrier = mCarrier;
        Vector3 from;
        int y = carrier->mPosY;
        int z = carrier->mPosZ;
        int x = carrier->mPosX;
        int aboveY = y + 0x32000;
        from.x = x;
        from.y = aboveY;
        from.z = z;
        DetectRaycastClsn(from, mPos, true);
        mCarrier = 0;
        func_ov072_02121d50(0);
    }
animate:
    mAnmModel.Advance();
    mCylClsn.Clear();
    if (param1 == 0) {
        u32 frame = (u32)(mAnmModel.currFrame << 4) >> 16;
        if (frame == 0x10 || frame == 0x25) {
            func_0201267c(0xf2, &mCamSpacePos);
        }
    }
    return 1;
}

// @symbol _ZN9daPgBby_c19func_ov072_02121758Ev
/* State 3, enter. */
int daPgBby_c::func_ov072_02121758()
{
    mAnmModel.SetAnim((BCA_File *)gPenguinBabyWalkAnimationFile.filePtr, 0, 0x1000, 0);
    mAnmModel.speed = 0x1000;
    mHorzSpeed = 0x5000;
    mStateId = 3;
    return 1;
}

// @symbol _ZN9daPgBby_c19func_ov072_02121670Ev
/* State 3, update: walk to the mother, keeping between 0xc8 and 0xdc
   units from her. */
int daPgBby_c::func_ov072_02121670()
{
    int dist = Vec3_Dist(&mPos, &mMother->mPos);
    short tgt = mAngleY;
    if (dist > 0xdc000) {
        tgt = Vec3_HorzAngle(&mPos, &mMother->mPos);
    } else if (dist < 0xc8000) {
        tgt = (short)(Vec3_HorzAngle(&mPos, &mMother->mPos) + 0x8000);
    }
    ApproachLinear(mAngleY, tgt, 0x2bc);
    mPrevAngleY = mAngleY;
    mAnmModel.Advance();
    UpdatePos(&mCylClsn);
    func_ov072_02120fd4(&mWithMeshClsn);
    func_ov072_02120f14();
    func_ov072_02120e50();
    mCylClsn.Clear();
    mCylClsn.Update();
    {
        int frame = (int)((unsigned)(mAnmModel.currFrame << 4) >> 16);
        if (frame == 9 || frame == 0x15) {
            func_0201267c(0xf3, &mCamSpacePos);
        }
    }
    return 1;
}

// @symbol _ZN9daPgBby_c19func_ov072_02121640Ev
/* State 4, enter. */
int daPgBby_c::func_ov072_02121640()
{
    mSubState = 0;
    mHorzSpeed = 0;
    mCylClsn.Clear();
    mStateId = 4;
    return 1;
}

// @symbol _ZN9daPgBby_c19func_ov072_021214dcEv
/* State 4, update: the carrier's message 0xb2, then wait for the talk to
   close and be put down. */
int daPgBby_c::func_ov072_021214dc()
{
    switch (mSubState) {
    case 0:
    {
        u32 flags = mFlags;
        int inMouth = (flags & 0x40000) != 0;
        if (inMouth) {
            mSubState++;
        } else {
            int talking = (flags & 0x20000) != 0;
            if (talking) break;
            if (inMouth) break;
            mEatingPlayer = 0;
            func_ov072_02121d50(0);
        }
        break;
    }
    case 1:
    {
        if (mCarrier->ShowMessage(*this, 0xb2, 0, 0, 0)) {
            func_0201267c(0xf2, &mCamSpacePos);
            mSubState++;
        }
        int spat = (mFlags & 0x80000) != 0;
        if (spat) {
            func_ov072_02121d50(5);
        }
        break;
    }
    case 2:
        if (mCarrier->GetTalkState() == -1) {
            mCarrier->DropActor();
            mSubState++;
        }
        break;
    case 3:
    {
        int spat = (mFlags & 0x80000) != 0;
        if (spat) {
            func_ov072_02121d50(5);
        }
        break;
    }
    }
    return 1;
}

// @symbol _ZN9daPgBby_c19func_ov072_02121368Ev
/* State 5, enter: spat out by Yoshi -- appear 0x50 units in front of the
   eating player and raycast down from above it. The statement order below
   is the retail schedule's. */
int daPgBby_c::func_ov072_02121368()
{
    Vector3 from;
    Vector3 *fromp;
    Player *eater;
    s32 *posX;
    s32 *posY;
    s32 *posZ;
    s16 angle;
    u16 index;
    int dist;
    int round;
    int store;
    int x;
    int y;
    int z;
    int aboveY;

    mFlags &= ~0x80000;
    mHorzSpeed = 0xa000;
    mVertSpeed = 0;

    eater = mEatingPlayer;
    posX = &mPosX;
    angle = eater->mAngleY;
    dist = 0x50000;
    round = 0x800;
    mAngleY = angle;
    angle = mAngleY;
    posY = &mPosY;
    posZ = &mPosZ;
    mPrevAngleY = angle;

    eater = mEatingPlayer;
    {
        s32 *src = &eater->mPosX;
        s32 srcX = src[0];
        fromp = &from;
        mPosX = srcX;
        s32 srcY = src[1];
        store = 1;
        mPosY = srcY;
        s32 srcZ = src[2];
        mPosZ = srcZ;
    }

    index = mAngleY;
    angle = data_02082214[(index >> 4) * 2];
    *posX = *posX + (int)(((s64)angle * dist + round) >> 12);
    *posY = *posY + dist;

    index = mAngleY;
    angle = data_02082214[(index >> 4) * 2 + 1];
    *posZ = *posZ + (int)(((s64)angle * dist + round) >> 12);

    eater = mEatingPlayer;
    y = eater->mPosY;
    z = eater->mPosZ;
    aboveY = y + dist;
    x = eater->mPosX;
    from.x = x;
    from.y = aboveY;
    from.z = z;
    DetectRaycastClsn(*fromp, *(Vector3 *)posX, store);

    mEatingPlayer = 0;
    mWithMeshClsn.SetLimMovFlag();
    mStateId = 5;
    return 1;
}

// @symbol _ZN9daPgBby_c19func_ov072_021212c0Ev
/* State 5, update: fall, bounce once at 40% and settle back into state 0. */
int daPgBby_c::func_ov072_021212c0()
{
    dBgCh_Actr_UpdateContinuous_Veneer(&mWithMeshClsn);
    if (mWithMeshClsn.JustHitGround()) {
        mVertSpeed = (mVertSpeed * -0x28) / 100;
    } else if (mWithMeshClsn.IsOnGround()) {
        mVertSpeed = 0;
        mWithMeshClsn.ClearLimMovFlag();
        func_ov072_02121d50(0);
    }
    UpdatePos(&mCylClsn);
    func_ov072_02120e50();
    mCylClsn.Clear();
    mCylClsn.Update();
    return 1;
}

// @symbol _ZN9daPgBby_c19func_ov072_021210c4Ev
/* Place the model: in the carrier's hands (one of four carry offsets) or at
   the actor, then size the drop shadow by the height above the ground. */
void daPgBby_c::func_ov072_021210c4()
{
    int idx;
    Matrix4x3 *carry;
    Vector3 v;
    int shadowHeight;
    int shadowRadius;

    if (mCarrier && ((mFlags & 0x4000) ? 1 : 0) && mCarrier->mCarryMatrix) {
        idx = 0;
        if (mCarrier->IsFrontSliding() || mCarrier->LostGrabbedObject()) {
            idx = 1;
        }
        if (mCarrier->param1 == 2) {
            idx = (idx + 2) & 0xff;
        }
        carry = UpdateCarry(*mCarrier, gPenguinBabyCarryOffsets[idx]);
        *(PgBbyMtx *)&mAnmModel.mat4x3 = *(PgBbyMtx *)carry;
    } else {
        Matrix4x3_FromRotationY(&mAnmModel.mat4x3, mAngleY);
        mAnmModel.mat4x3.t.x = mPosX >> 3;
        mAnmModel.mat4x3.t.y = mPosY >> 3;
        mAnmModel.mat4x3.t.z = mPosZ >> 3;
    }

    if ((mFlags & 0x40000) ? 1 : 0) {
        return;
    }

    shadowHeight = 0xb4000;
    shadowRadius = 0x5a000;
    if (mCarrier && ((mFlags & 0x4000) ? 1 : 0)) {
        v.x = mPosX;
        v.y = mPosY;
        v.z = mPosZ;
        v.y -= 0xa000;
        dBgCh_Gnd ground;
        ground.SetObjAndPos(v, 0);
        if (ground.DetectClsn()) {
            shadowHeight = mPosY - ground.clsnY;
            if (shadowHeight < 0x1000) shadowHeight = 0x1000;
            shadowRadius = shadowRadius - FX_MUL(shadowHeight, 0x180);
            if (shadowRadius < 0x1000) shadowRadius = 0x1000;
            if (shadowRadius > 0x5a000) shadowRadius = 0x5a000;
            shadowHeight = shadowHeight + 0x28000;
        }
    }

    DropShadowRadHeight(mShadowModel, mAnmModel.mat4x3, shadowRadius, shadowHeight, 0xf);
}

// @symbol _ZN9daPgBby_c19func_ov072_02120fd4EP10dBgCh_Actr
/* Wall/floor pass: on a slope, turn horizontal motion into vertical speed. */
void daPgBby_c::func_ov072_02120fd4(dBgCh_Actr *clsn)
{
    Vector3 floorNormal;
    Vector3 wallNormal;
    int b = (int)((mFlags & 0x4000) != 0);
    if (b != 0) return;
    dBgCh_Actr_UpdateContinuous_Veneer(clsn);
    if (clsn->IsOnGround()) {
        ((SurfaceInfo *)((char *)clsn->GetFloorResult() + 4))->CopyNormalTo(floorNormal);
        if (floorNormal.y != 0) {
            int x = FX_MUL(floorNormal.x, mSpeedX);
            int y = FX_MUL(floorNormal.z, mSpeedZ);
            mVertSpeed = -(cstd::fdiv(x + y, floorNormal.y) + 0x8000);
        }
    }
    if (clsn->IsOnWall()) {
        ((SurfaceInfo *)((char *)clsn->GetWallResult() + 4))->CopyNormalTo(wallNormal);
    }
}

// @symbol _ZN9daPgBby_c19func_ov072_02120f14Ev
/* Refuse a step onto ground more than 0x1.9 away from the current height:
   put the actor back where it was last frame. */
int daPgBby_c::func_ov072_02120f14()
{
    Vector3 v;

    if (mWithMeshClsn.IsOnGround()) {
        dBgCh_Gnd ground;
        {
            int z = mPosZ;
            int y = mPosY + 0x1e000;
            int x = mPosX;
            v.x = x;
            v.y = y;
            v.z = z;
        }
        ground.SetObjAndPos(v, this);
        if (ground.DetectClsn() != 0) {
            int d = ground.clsnY - mPosY;
            if (d < 0) d = -d;
            if (d <= 0x1900) goto bail;
        }
        mPosX = mPrevPosX;
        mPosY = mPrevPosY;
        mPosZ = mPrevPosZ;
        return 1;
    bail:
        ;
    }
    return 0;
}

// @symbol _ZN9daPgBby_c19func_ov072_02120e50Ev
/* Touched by the player (actor 0xbf): enter state 4 while talking, or
   state 2 when the player grabs it. */
int daPgBby_c::func_ov072_02120e50()
{
    Player *a;
    int isType;
    int flag;
    if (mCylClsn.otherOwner == 0) return 0;
    a = static_cast<Player *>(dActor_c::FindWithID(mCylClsn.otherOwner));
    if (a == 0 || (isType = (a->actorID == 0xbf)) == 0) return 0;
    flag = ((mFlags & 0x20000) != 0);
    if (flag) {
        mCarrier = a;
        func_ov072_02121d50(4);
    } else if ((mCylClsn.hitFlags & 0x1000) && a->TryGrab(*this)) {
        mCarrier = a;
        func_ov072_02121d50(2);
    }
    return 1;
}

// @symbol _ZN9daPgBby_c19func_ov072_02120e20Ev
/* A diving player nearby startles it into state 1. */
void daPgBby_c::func_ov072_02120e20()
{
    Player *player = ClosestPlayer();
    if (player->IsDiving()) func_ov072_02121d50(1);
}

// @symbol _ZN9daPgBby_c19func_ov072_02120ddcEv
/* Within 0x190 of the mother: state 3. */
void daPgBby_c::func_ov072_02120ddc()
{
    dActor_c *mother = mMother;
    if (mother == 0)
        return;
    if (Vec3_Dist(&mPos, &mother->mPos) >= 0x190000)
        return;
    func_ov072_02121d50(3);
}

// @symbol _ZN9daPgBby_c19func_ov072_02120d04Ev
/* Respawn: fallen far below the level, or far from home with the timer
   run out, and off screen -- go back to the spawn position. */
void daPgBby_c::func_ov072_02120d04()
{
    Vector3 out;
    Vector3 tmp;
    if (mStateId == 3) return;
    if ((unsigned)mPosY > (unsigned)(-0x1800000)) {
        if (Vec3_Dist((Vector3 *)&mSpawnPosX, &mPos) < 0x190000)
            return;
        if (DecIfAbove0_Short(&mRespawnTimer) != 0) return;
    }
    Vec3_Asr(&tmp, (Vector3 *)&mSpawnPosX, 3);
    if (data_0209f43c.TransformAndClip(data_0209b3ec, tmp, 0x1f400, out) <= 0xc350000)
        return;
    mPosX = mSpawnPosX;
    mPosY = mSpawnPosY;
    mPosZ = mSpawnPosZ;
}

// @symbol _ZN9daPgBby_c13OnYoshiTryEatEv
s32 daPgBby_c::OnYoshiTryEat()
{
    return 7;
}

/* NOT WRITTEN HERE ON PURPOSE. The inline destructor in the header
   emits D1 then D0 -- the cartridge's order -- and no D2. */
// @symbol _ZN9daPgBby_cD1Ev
// @symbol _ZN9daPgBby_cD0Ev

/* Runtime asset handles, distinct from NitroFS file IDs. Construction order
 * follows the retail initializer, including each destructor registration. */
enum PenguinBabyAssetId {
    ASSET_PENGUIN_CHILD_BMD = 0x03ff,
    ASSET_PENGUIN_DIVE_BCA = 0x0400,
    ASSET_PENGUIN_SLIDER_BCA = 0x0403,
    ASSET_PENGUIN_STAND_UP_BCA = 0x0405,
    ASSET_PENGUIN_WAIT1_BCA = 0x0406,
    ASSET_PENGUIN_WALK_BCA = 0x0408
};

PgBbyModelFile gPenguinBabyModelFile(ASSET_PENGUIN_CHILD_BMD);
PgBbyAnimationFile gPenguinBabyWaitAnimationFile(ASSET_PENGUIN_WAIT1_BCA);
PgBbyAnimationFile gPenguinBabyWalkAnimationFile(ASSET_PENGUIN_WALK_BCA);
PgBbyAnimationFile gPenguinBabyDiveAnimationFile(ASSET_PENGUIN_DIVE_BCA);
PgBbyAnimationFile gPenguinBabySlideAnimationFile(ASSET_PENGUIN_SLIDER_BCA);
PgBbyAnimationFile gPenguinBabyStandUpAnimationFile(ASSET_PENGUIN_STAND_UP_BCA);

SharedFilePtr *const gPenguinBabyAnimationFiles[5] = {
    &gPenguinBabyWaitAnimationFile,    &gPenguinBabyWalkAnimationFile,
    &gPenguinBabyDiveAnimationFile,    &gPenguinBabySlideAnimationFile,
    &gPenguinBabyStandUpAnimationFile,
};

daPgBby_c::State gPenguinBabyStates[6] = {
    {&daPgBby_c::func_ov072_02121c94, &daPgBby_c::func_ov072_02121a84},
    {&daPgBby_c::func_ov072_02121a28, &daPgBby_c::func_ov072_021218dc},
    {&daPgBby_c::func_ov072_02121890, &daPgBby_c::func_ov072_021217ac},
    {&daPgBby_c::func_ov072_02121758, &daPgBby_c::func_ov072_02121670},
    {&daPgBby_c::func_ov072_02121640, &daPgBby_c::func_ov072_021214dc},
    {&daPgBby_c::func_ov072_02121368, &daPgBby_c::func_ov072_021212c0},
};

Vector3 gPenguinBabyCarryOffsets[4] = {
    Vector3(0x1c000, 0, 0x1e000),
    Vector3(0x1c000, 0, 0x1e000),
    Vector3(0x2b000, 0, 0x1e000),
    Vector3(0x30000, 0, 0x32000),
};

/* The registry's typed factory and the actor profile consumed by dActor_c. */
extern "C" {
SpawnInfo<daPgBby_c> g_profile_PENGUIN_BABY = {
    daPgBby_c_classInit, 0x0100, 0x0072, 0x00808282, 0x00028000, 0x00028000, 0x01000000, 0x01000000,
};
}
