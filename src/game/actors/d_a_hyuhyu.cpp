//cpp
/**
 * Fwoosh, the cloud that blows (profile HYUHYU).
 *
 * The cloud (mVariant 0) turns toward the nearest player. Once that player
 * is within 1000.0 and passes the signed bearing threshold, it plays its
 * blow animation; on
 * frames 31 and later it emits the two gust particle streams and, every
 * other frame, spawns a gust -- another HYUHYU actor with param1 = 1 --
 * aimed at the player with a little random spread in heading. When the
 * animation ends it waits 100 frames before it can blow again.
 *
 * A gust (mVariant 1) has no model. It flies 30.0 a frame along its pitch
 * and heading while its pitch climbs, and removes itself after 40 frames or
 * on touching the ground or a wall. A player it hits, unless metal, mega,
 * already collecting a cap, is blown away. Wings and balloon form prevent
 * the subsequent cap loss; otherwise an eligible cap is dropped as an
 * OBJ_MARIO_CAP actor that flies off.
 *
 * daHyuhyu_c_classInit is reconstructed (RTTI daHyuhyu_c, HYUHYU registry
 * profile). Retail does not store that spelling. Historical alias:
 * Fwoosh_Spawn.
 *
 * FUNCTION ORDER IS THE REVERSE OF THE ROM'S -- mwccarm emits one .text
 * section per function in the reverse of source order.
 *
 * The six state handlers (StateWait*, StateBlow*, StateGustFly*), SetState
 * and the two helpers are daHyuhyu_c members. ov091's static initializer
 * copies the handlers' pointer-to-members into the three state records
 * mState points at.
 *
 * Notes:
 * - data_ov091_02135674 / 0213567c / 02135684 are this overlay's BMD and
 *   two BCA SharedFilePtr handles; data_ov002_0210da40 / 0210d9a0 /
 *   0210d9c0 are shared ov002 files the cloud also claims. symbols.txt has
 *   no recovered names for any of them.
 * - func_ov002_020aea30 (fire hit), func_02012694 (sound at a
 *   camera-space position), data_02082214 (sin/cos pairs) and
 *   data_0209e650 (random seed) keep linker names.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_hyuhyu_AssetId {
    ASSET_HYUHYU_BMD = 0x032f,
    ASSET_HYUHYU_ATTACK_BCA = 0x0330,
    ASSET_HYUHYU_WAIT_BCA = 0x0331
};

#include "common.h"
#include "daHyuhyu_c.h"
#include "Player.h"
#include "SaveData.h"
#include "SharedFilePtr.h"
#include "Particle__System.h"
#include "math/MtxStack.h"

struct BCA_File;
struct BMD_File;

extern SharedFilePtr data_ov002_0210da40;
extern SharedFilePtr data_ov002_0210d9a0;
extern SharedFilePtr data_ov002_0210d9c0;
extern daHyuhyu_c_State data_ov091_021356b0; /* cloud: blowing */
extern daHyuhyu_c_State data_ov091_021356c0; /* gust: flying */
extern daHyuhyu_c_State data_ov091_021356d0; /* cloud: waiting */
extern s16 data_02082214[];                  /* sin/cos pairs */
extern int data_0209e650;                    /* random seed */

/* C helpers with no shared header. */
extern "C" {
void func_ov002_020aea30(void *self, void *actor, void *collision);
void func_02012694(u32 id, void *pos);
unsigned short DecIfAbove0_Short(unsigned short *p);
int RandomIntInternal(int *seed);
s16 Vec3_VertAngle(const Vector3 *v1, const Vector3 *v0);
s16 Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
int Vec3_Dist(const Vector3 *a, const Vector3 *b);
int ApproachAngle(s16 *cur, s16 target, int a, int b, int c);
void Vec3_Asr(Vector3 *dst, const Vector3 *src, int n);
void Matrix4x3_ApplyInPlaceToRotationX(Matrix4x3 *m, s16 angX);
void MulVec3Mat4x3(Vector3 *v, Matrix4x3 *m, Vector3 *res);
}

/* File-scope resource handles: the ctor/dtor are the ROM's SharedFilePtr
 * veneer pairs (func_02017acc / func_02017ab4 for the BMD, SharedFilePtr::
 * Construct / SharedFilePtr_Destruct_Anim for the two BCAs), spelled through
 * declared-only subclasses so the static initializer names the real entry
 * points. */
struct HyuhyuModelFilePtr : SharedFilePtr {
    HyuhyuModelFilePtr(u32 fileID);
    ~HyuhyuModelFilePtr();
};
struct HyuhyuAnimationFileHandle : SharedFilePtr {
    HyuhyuAnimationFileHandle(u32 fileID);
    ~HyuhyuAnimationFileHandle();
};
extern HyuhyuModelFilePtr data_ov091_02135674;        /* BMD */
extern HyuhyuAnimationFileHandle data_ov091_0213567c; /* BCA: blow */
extern HyuhyuAnimationFileHandle data_ov091_02135684; /* BCA: wait */

/* Actor ID 0xbf is PLAYER; dActor_c::mFlags bit 0x40000 is set while the
 * actor is in Yoshi's mouth. int-returning, so the ROM materializes the 0/1
 * before testing it; a direct test does not. */
static inline int IsPlayerActor(dActor_c *actor) {
    return actor->actorID == 0xbf;
}

static inline int IsInYoshiMouth(dActor_c *actor) {
    return (actor->mFlags & 0x40000) != 0;
}

// @symbol __sinit_d_a_hyuhyu.cpp
HyuhyuModelFilePtr data_ov091_02135674(ASSET_HYUHYU_BMD);               /* BMD */
HyuhyuAnimationFileHandle data_ov091_0213567c(ASSET_HYUHYU_ATTACK_BCA); /* BCA: blow */
HyuhyuAnimationFileHandle data_ov091_02135684(ASSET_HYUHYU_WAIT_BCA);   /* BCA: wait */

typedef int (daHyuhyu_c::*HyuhyuStateFn)();

/* The six state member-pointer records live in .data
 * (0x0213533c..0x0213536c); the static initializer copies them into the
 * three state records below, one {enter, execute} pair each. */
extern "C" HyuhyuStateFn data_ov091_02135364; /* StateWaitInit */
extern "C" HyuhyuStateFn data_ov091_0213535c; /* StateWaitMain */
extern "C" HyuhyuStateFn data_ov091_0213534c; /* StateBlowInit */
extern "C" HyuhyuStateFn data_ov091_02135354; /* StateBlowMain */
extern "C" HyuhyuStateFn data_ov091_02135344; /* StateGustFlyInit */
extern "C" HyuhyuStateFn data_ov091_0213533c; /* StateGustFlyMain */

daHyuhyu_c_State data_ov091_021356d0 = {data_ov091_02135364,
                                        data_ov091_0213535c}; /* cloud: waiting */
daHyuhyu_c_State data_ov091_021356b0 = {data_ov091_0213534c,
                                        data_ov091_02135354}; /* cloud: blowing */
daHyuhyu_c_State data_ov091_021356c0 = {data_ov091_02135344,
                                        data_ov091_0213533c}; /* gust: flying */

// @symbol daHyuhyu_c_classInit
/* The dEnemyBase_c C2, the vptr store and the dCcAc_c / dBgCh_Actr /
   dExtAnmModel_c C1s at 0x110 / 0x144 / 0x300 all fall out of the one `new`. */
extern "C" daHyuhyu_c *daHyuhyu_c_classInit() {
    return new daHyuhyu_c();
}

// @symbol _ZN10daHyuhyu_c13OnYoshiTryEatEv
int daHyuhyu_c::OnYoshiTryEat() {
    return 4;
}

// @symbol _ZN10daHyuhyu_c13OnTurnIntoEggER6Player
void daHyuhyu_c::OnTurnIntoEgg(Player &player) {
    MarkForDestruction();
}

// @symbol _ZN10daHyuhyu_c16OnAimedAtWithEggEv
int daHyuhyu_c::OnAimedAtWithEgg() {
    return 0x1f000;
}

// @symbol _ZN10daHyuhyu_c13InitResourcesEv
int daHyuhyu_c::InitResources() {
    mVariant = param1 & 0xff;
    mTerminalVelocity = -0x1e000;
    if (mVariant == 0xff)
        mVariant = 0;
    if (mVariant == 1) {
        mdCcAc_c.Init(this, 0x3c000, 0x14000, 0x200002, 0);
        SetState(&data_ov091_021356c0);
        mWithMeshClsn.Init(this, 0x14000, 0, 0, 0);
        mFlags = 0;
        return 1;
    }
    mdCcAc_c.Init(this, 0x64000, 0x64000, 0x200002, 0x4a010);
    mFlags = 0x10000000;
    dExtSimpleModel_c::LoadFile(data_ov002_0210da40);
    dExtSimpleModel_c::LoadFile(data_ov002_0210d9a0);
    dExtSimpleModel_c::LoadFile(data_ov002_0210d9c0);
    mAnmModel.SetFile((BMD_File *)dExtSimpleModel_c::LoadFile(data_ov091_02135674), 1, -1);
    dExtFrameCtrl_c::LoadFile(data_ov091_0213567c);
    dExtFrameCtrl_c::LoadFile(data_ov091_02135684);
    mAnmModel.speed = 0x1000;
    SetState(&data_ov091_021356d0);
    return 1;
}

// @symbol _ZN10daHyuhyu_c8BehaviorEv
int daHyuhyu_c::Behavior() {
    if (UpdateYoshiEat(mWithMeshClsn) != 0) {
        mdCcAc_c.Clear();
        if (mEatenByYoshi != 0 && mSpitOutTimer == 0)
            mdCcAc_c.Update();
        UpdateModelTransform();
        return 1;
    }

    DecIfAbove0_Short((unsigned short *)&mStateTimer);
    {
        daHyuhyu_c_State *state = mState;
        if (state->execute != 0)
            (this->*state->execute)();
    }
    mAngleX = mPrevAngleX;
    mAngleY = mPrevAngleY;
    mAngleZ = mPrevAngleZ;
    /* Fall, no faster than mTerminalVelocity. The cartridge reloads and
       rewrites mSpeedZ here too (a whole-vector store, most likely); a plain
       clamp misses. */
    {
        int speed = mVertSpeed + mVertAccel;
        int limit = mTerminalVelocity;
        if (speed >= limit)
            limit = speed;
        int tmp = mSpeedZ;
        mVertSpeed = limit;
        mSpeedZ = tmp;
    }
    UpdatePosWithOnlySpeed(&mdCcAc_c);
    HandleCylinderHit();
    mdCcAc_c.Clear();
    mdCcAc_c.Update();

    if (mVariant == 1) {
        UpdateWMClsn(mWithMeshClsn, 0);
        return 1;
    }

    mAnmModel.Advance();
    UpdateModelTransform();
    return 1;
}

// @symbol _ZN10daHyuhyu_c6RenderEv
int daHyuhyu_c::Render() {
    if (mVariant == 1)
        return 1;
    if (IsInYoshiMouth(this))
        return 1;
    mAnmModel.Render(0);
    return 1;
}

// @symbol _ZN10daHyuhyu_c16OnPendingDestroyEv
void daHyuhyu_c::OnPendingDestroy() {}

// @symbol _ZN10daHyuhyu_c16CleanupResourcesEv
int daHyuhyu_c::CleanupResources() {
    if (mVariant == 1)
        return 1;
    data_ov002_0210da40.Release();
    data_ov002_0210d9a0.Release();
    data_ov002_0210d9c0.Release();
    data_ov091_02135674.Release();
    data_ov091_0213567c.Release();
    data_ov091_02135684.Release();
    return 1;
}

// @symbol _ZN10daHyuhyu_c20UpdateModelTransformEv
/* The model matrix: translation at 1/8 scale, then the rotation. */
void daHyuhyu_c::UpdateModelTransform() {
    Vector3 pos;
    Vec3_Asr(&pos, &mPos, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, pos.x, pos.y, pos.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, mAngleX, mAngleY, mAngleZ);
    mAnmModel.mat4x3 = data_020a0e68;
}

// @symbol _ZN10daHyuhyu_c8SetStateEP16daHyuhyu_c_State
/* Enter `state`, running its enter handler if it has one. */
int daHyuhyu_c::SetState(daHyuhyu_c_State *state) {
    mState = state;
    daHyuhyu_c_State *cur = mState;
    if (cur->enter == 0)
        return 1;
    return (this->*cur->enter)();
}

// @symbol _ZN10daHyuhyu_c13StateWaitInitEv
/* Cloud, waiting: enter. Loop the wait animation. */
int daHyuhyu_c::StateWaitInit() {
    mAnmModel.SetAnim((BCA_File *)data_ov091_02135684.filePtr, 0, 0x1000, 0);
    return 1;
}

// @symbol _ZN10daHyuhyu_c13StateWaitMainEv
/* Cloud, waiting: execute. Turn toward the nearest player; blow once the
   cool-down is over, that player is within 1000.0, and the signed bearing
   returned by Vec3_HorzAngle is below 0x1000. */
int daHyuhyu_c::StateWaitMain() {
    Player *player = ClosestPlayer();

    if (player != 0) {
        Vector3 target;
        s16 angle;
        {
            Vector3 *pos = &player->mPos;
            target.x = pos->x;
            target.y = pos->y;
            target.z = pos->z;
        }
        angle = Vec3_HorzAngle(&mPos, &target);
        ApproachAngle(&mPrevAngleY, angle, 0xa, 0x200, 0x100);
        if ((u16)mStateTimer == 0 && Vec3_Dist(&mPos, &target) < 0x3e8000 && angle < 0x1000)
            SetState(&data_ov091_021356b0);
    }
    return 1;
}

// @symbol _ZN10daHyuhyu_c13StateBlowInitEv
/* Cloud, blowing: enter. Play the blow animation once. */
int daHyuhyu_c::StateBlowInit() {
    mAnmModel.SetAnim((BCA_File *)data_ov091_0213567c.filePtr, 0x40000000, 0x1000, 0);
    return 1;
}

// @symbol _ZN10daHyuhyu_c13StateBlowMainEv
/* Cloud, blowing: execute. From frame 31 on, stream the gust particles
   and spawn a gust at the nearest player every other frame; when the
   animation ends, cool down for 100 frames and wait again. */
int daHyuhyu_c::StateBlowMain() {
    Vector3_16 rot;
    Vector3 pos;
    Vector3 target;
    dActor_c *gust;
    Player *player;
    s16 heading;

    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;

    u16 frame = (u16)(mAnmModel.currFrame >> 12);
    if (frame == 0x1f)
        func_02012694(0x164, &mCamSpacePos);

    if ((u16)(mAnmModel.currFrame >> 12) > 0x1e) {
        rot.x = data_02082214[((u16)mAngleY >> 4) * 2];
        rot.y = 0;
        rot.z = data_02082214[((u16)mAngleY >> 4) * 2 + 1];

        pos.y = pos.y + 0x28000;

        mGustParticle = Particle::System::New(mGustParticle, 0x72, pos.x, pos.y, pos.z,
                                              (const Vector3_16f *)(&rot), 0);
        mGustParticle2 = Particle::System::New(mGustParticle2, 0x73, pos.x, pos.y, pos.z,
                                               (const Vector3_16f *)(&rot), 0);

        if (!((u16)(mAnmModel.currFrame >> 12) & 1)) {
            gust = dActor_c::Spawn(0xe7, 1, mPos, 0, mAreaId, -1);
            if (gust != 0) {
                player = ClosestPlayer();
                if (player != 0) {
                    Vector3 *ppos = &player->mPos;
                    target.x = ppos->x;
                    target.y = ppos->y;
                    target.z = ppos->z;
                    target.y = player->mGroundY;

                    {
                        s16 base = mPrevAngleY;
                        heading = base + (s16)(0x2000 -
                                               ((((u32)RandomIntInternal(&data_0209e650) >> 8) & 3)
                                                << 0xc));
                    }

                    gust->mPrevAngleX = Vec3_VertAngle(&mPos, &target);
                    gust->mPrevAngleY = heading;
                    gust->mPrevAngleZ = 0;
                }
            }
        }
    }

    if (mAnmModel.Finished() != 0) {
        mStateTimer = 100;
        SetState(&data_ov091_021356d0);
    }

    return 1;
}

// @symbol _ZN10daHyuhyu_c16StateGustFlyInitEv
/* Gust, flying: enter. Live 40 frames. */
int daHyuhyu_c::StateGustFlyInit() {
    mStateTimer = 40;
    return 1;
}

// @symbol _ZN10daHyuhyu_c16StateGustFlyMainEv
/* Gust, flying: execute. Climb, fly 30.0 a frame along pitch and heading,
   and go away when the time runs out or the gust touches ground or wall. */
int daHyuhyu_c::StateGustFlyMain() {
    Vector3 fwd;
    fwd.x = 0;
    fwd.y = 0;
    fwd.z = 0x1e000;
    mPrevAngleX -= 0x80;
    Matrix4x3_FromRotationY(&data_020a0e68, mPrevAngleY);
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, mPrevAngleX);
    MulVec3Mat4x3(&fwd, &data_020a0e68, (Vector3 *)&mSpeedX);
    if ((u16)mStateTimer == 0 || mWithMeshClsn.IsOnGround() != 0 || mWithMeshClsn.IsOnWall() != 0 ||
        (mFlags & 8))
        MarkForDestruction();
    return 1;
}

// @symbol _ZN10daHyuhyu_c17HandleCylinderHitEv
/* What the collision cylinder hit this frame.
   The cloud: fire hurts it, an egg or a metal player pops it, and a mega
   player destroys it for a mega-kill.
   A gust: blows a player away unless metal, mega or collecting a cap.
   Wings and balloon form prevent the subsequent cap loss. When mCharacter
   and the player's param1 differ the player is switched back with
   SetNewHatCharacter; otherwise the cap is recorded as lost (once). Either
   way an OBJ_MARIO_CAP is dropped and flies off along the gust. The player
   tests go through the int-returning IsPlayerActor: a plain `!=` misses. */
void daHyuhyu_c::HandleCylinderHit() {
    u32 hitFlags;
    u32 id = mdCcAc_c.otherOwner;
    Player *player;

    if (id == 0)
        return;
    player = (Player *)dActor_c::FindWithID(id);
    if (mVariant == 0) {
        hitFlags = mdCcAc_c.hitFlags;
        if ((hitFlags & 0x40000) != 0) {
            mDeathState = 4;
            func_ov002_020aea30(this, player, 0);
            return;
        }
        if ((hitFlags & 0x2000) != 0) {
            PoofDust();
            MarkForDestruction();
            return;
        }
        if (!IsPlayerActor(player))
            return;
        if (player->mIsMetal == 1) {
            PoofDust();
            MarkForDestruction();
            return;
        }
        if ((hitFlags & 0x10) == 0)
            return;
        PoofDust();
        SpawnMegaCharParticles(*player, 0);
        player->IncMegaKillCount();
        func_02012694(0x1d, &mCamSpacePos);
        MarkForDestruction();
        return;
    }

    if (!IsPlayerActor(player))
        return;
    if (player->mIsMetal != 0)
        return;
    if (player->mIsMega != 0)
        return;
    if (player->IsCollectingCap() != 0)
        return;
    player->BlowAway(mPrevAngleY);
    {
        u8 hasWings = player->mHasWings;
        u8 character = player->mCharacter;
        if (hasWings != 0)
            return;
        if (player->mIsBalloon != 0)
            return;
        {
            u32 cur = player->param1;
            u32 capParam = 1;
            if (character != cur) {
                player->SetNewHatCharacter(character, 0, 0);
            } else {
                if (SaveData::HasPlayerLostCap() != 0)
                    return;
                SaveData::PlayerLoseCap();
            }
            {
                u32 capChar = player->param1;
                Vector3_16 rot;
                dActor_c *cap;
                rot.x = 0;
                rot.y = 0;
                rot.z = 0;
                rot.y = mPrevAngleY;
                capParam = capParam | (capChar << 8);
                cap = dActor_c::Spawn(0x10d, capParam, mPos, &rot, mAreaId, -1);
                if (cap == 0)
                    return;
                cap->mHorzSpeed = 0x32000;
                cap->mSpeedX = 0;
                cap->mVertSpeed = 0x14000;
                cap->mSpeedZ = 0;
            }
        }
    }
}

// @symbol _ZN10daHyuhyu_cD1Ev
// @symbol _ZN10daHyuhyu_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The inline destructor in the header
   emits D1 then D0 -- the cartridge's order -- and no D2. */
