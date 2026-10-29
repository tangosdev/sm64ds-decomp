//cpp
/* daHuwa_c, the Spindrift (HUWAHUWA). ov081, thirteen functions,
 * .text 0x02123740..0x02124090.
 *
 * common.h is first so Matrix4x3 stays the flat { s32 m[12]; }. The death
 * and shadow helpers index that matrix through .m. daHuwa_c.h pulls in
 * math/Matrix.h, whose nested spelling would stand instead if it were first.
 *
 * #pragma defer_codegen off emits .text in source order. One out-of-line
 * destructor is the key function, so this TU emits _ZTV/_ZTI/_ZTS and a D2
 * the cartridge has no home for (manifest: deadstrip).
 *
 * The registry factory daHuwa_c_classInit (0x02124040) abuts the run and is
 * written last; g_profile_HUWAHUWA stays in another file. The two file
 * handles are constructed by __sinit_ov081_021280e8 (file ids 813 and
 * 814); this TU only loads and releases them.
 *
 * Notes (measured spellings that do not match):
 * - HandleCylinderHit: Player::Hurt with Fix12<int> knockback: 0x210 -> 0x21c
 *   (+12). SpinBounce with Fix12<int>: 0x210 -> 0x21c (+12).
 * - The 0/1 the ROM materializes in HandleCylinderHit, UpdateModelTransform
 *   and Render comes from the int-returning IsPlayerActor / IsInYoshiMouth
 *   inlines. A direct `!=` or bit test is 16 / 12 / 12 bytes short.
 * - Behavior copying player->mPosX/Y/Z instead of int* at player+0x5c: 0x264 -> 0x260
 *   (-4).
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daHuwa_c_AssetId { ASSET_HUWAHUWA_MODEL_BMD = 0x032d, ASSET_HUWAHUWA_MOVE_BCA = 0x032e };

#pragma defer_codegen off

#include "common.h"
#include "decl_Enemy.h"
#include "daHuwa_c.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "Sound.h"
#include "math/Approach.h"
#include "math/MtxStack.h"
#include "Particle__System.h"

/* The resource handles as the ROM lays them out: {id, loaded file}. The
   model handle constructs through func_02017acc and destroys through
   func_02017ab4; the animation handle constructs through
   SharedFilePtr::Construct and destroys through SharedFilePtr_Destruct_Anim.
   The manifest aliases those undefined members onto the ROM symbols. */
struct HuwaModelFilePtr : SharedFilePtr {
    HuwaModelFilePtr(u32 fileID);
    ~HuwaModelFilePtr();
};

struct HuwaAnimationFileHandle : SharedFilePtr {
    HuwaAnimationFileHandle(u32 fileID);
    ~HuwaAnimationFileHandle();
};

/* dExtSimpleModel_c file (sinit id 813) and animation (sinit id 814). */
extern HuwaModelFilePtr gHuwahuwaModelModelFile;
extern HuwaAnimationFileHandle gHuwahuwaMoveAnimationFile;

/* C helpers with no shared header: the arm9 vector routines and
 * func_ov002_020e10a8, Player::IsState of the state SpinBounce enters. */
extern "C" {
extern int func_ov002_020e10a8(void *);

void SubVec3(Vector3 *a, Vector3 *b, Vector3 *out);
void AddVec3(Vector3 *a, Vector3 *b, Vector3 *out);
void Vec3_LslInPlace(Vector3 *v, int shift);
int Vec3_HorzDist(const Vector3 *a, const Vector3 *b);
short Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
}

/* dCc_c hitFlags bits this actor tests. 0x10 / 0x2000 / 0x4000 / 0x40000 /
 * 0x400000 are the names in dCc_c.h. 0x20000 is not in that table; this
 * function ORs it with the egg and explosion bits. 0x26fe0 is the attack
 * mask below the bounce check: spin, punch, kick, 0x100, slide, dive,
 * 0x800, egg, explosion and 0x20000. */
enum {
    kHitMega = 0x10,
    kHitEggOrBlast = 0x26000,
    kHitFire = 0x40000,
    kHitAttack = 0x26fe0,
    kHitPlayer = 0x400000,
    kFlagYoshiMouth = 0x40000,
    kPlayerActorId = 0xbf,
    kStateChase = 0,
    kStateRecoil = 1,
    kRecoilFrames = 0x14
};

/* int-returning predicates: they are what make the ROM materialize 0/1
 * before testing it. A direct test of the bit or the ID does not. */
static inline int IsPlayerActor(dActor_c *actor) {
    return actor->actorID == kPlayerActorId;
}

static inline int IsInYoshiMouth(dActor_c *actor) {
    return (actor->mFlags & kFlagYoshiMouth) != 0;
}

// @symbol _ZN8daHuwa_cD1Ev
// @symbol _ZN8daHuwa_cD0Ev
daHuwa_c::~daHuwa_c() {}

/* Vtable slot 29. 0x3c000 is 60.0; KillByInvincibleChar receives it and
 * does not read it. */
// @symbol _ZN8daHuwa_c16OnAimedAtWithEggEv
s32 daHuwa_c::OnAimedAtWithEgg() {
    return 0x3c000;
}

/* Poof, three coins, bone-1 particle, sound 0xd5, then the death table.
 * func_0201267c is Sound::Play(bank 3, id, pos). */
// @symbol _ZN8daHuwa_c13KillWithCoinsEv
void daHuwa_c::KillWithCoins() {
    Vector3 dust;
    Vector3 dustCopy;
    Vector3 coins;

    dust.x = mPosX;
    dust.y = mPosY;
    dust.z = mPosZ;
    dust.y += mdCcAc_c.height - 0x50000;
    dustCopy = dust;
    PoofDustAt(dustCopy);

    coins.x = mPosX;
    coins.y = mPosY;
    coins.z = mPosZ;
    SpawnCoins(coins, 3, 0xa000, 0);

    Matrix4x3_FromTranslation(&data_020a0e68, mPosX, mPosY, mPosZ);
    MulMat4x3Mat4x3((const int *)(mAnmModel.data.transforms + 1), data_020a0e68.m, data_020a0e68.m);

    dust.x = data_020a0e68.m[9];
    dust.y = data_020a0e68.m[10];
    dust.z = data_020a0e68.m[11];
    SubVec3(&dust, &mPos, &dust);
    Vec3_LslInPlace(&dust, 3);
    AddVec3(&dust, &mPos, &dust);

    Particle::System::NewSimple(0x53, dust.x, dust.y, dust.z);
    func_0201267c(0xd5, &mCamSpacePos);
    KillAndTrackInDeathTable();
}

/* Cylinder hits. Slot 29 is OnAimedAtWithEgg; called through dActor_c so it
 * stays a virtual call. func_ov002_020e10a8 is Player::IsState of the state
 * SpinBounce enters. */
// @symbol _ZN8daHuwa_c17HandleCylinderHitEv
void daHuwa_c::HandleCylinderHit() {
    dActor_c *other;
    unsigned int id;
    int flags;
    Player *player;
    Vector3_16 knock;
    Vector3 pos;

    id = mdCcAc_c.otherOwner;
    if (id == 0)
        return;
    other = dActor_c::FindWithID(id);
    if (other == 0)
        return;

    flags = mdCcAc_c.hitFlags;
    if ((flags & kHitMega) != 0) {
        int height;
        knock.x = (short)-0x2000;
        knock.y = 0;
        knock.z = 0;
        height = OnAimedAtWithEgg();
        KillByInvincibleChar(knock, *(Player *)other, height);
        return;
    }

    if ((flags & kHitEggOrBlast) != 0) {
        KillWithCoins();
        return;
    }

    if (!IsPlayerActor(other))
        return;

    player = (Player *)other;
    if (player->mIsMetal != 0 || player->IsOnShell() != 0 || (mdCcAc_c.hitFlags & kHitFire) != 0) {
        KillWithCoins();
        return;
    }

    if ((mdCcAc_c.hitFlags & kHitAttack) != 0) {
        if (func_ov002_020e10a8(player) == 0) {
            KillWithCoins();
            return;
        }
    }

    if (JumpedOnByPlayer(mdCcAc_c, *player) != 0) {
        player->SpinBounce(0x28000);
        KillWithCoins();
        return;
    }

    if (player->mIsVanish != 0)
        return;
    if ((mdCcAc_c.hitFlags & kHitPlayer) == 0)
        return;

    {
        unsigned char recoil = 1;
        int speed;
        mState = recoil;
        speed = 0xa000;
        mStateTimer = 0;
        mHorzSpeed = -speed;
    }
    mPrevAngleY = Vec3_HorzAngle(&mPos, &player->mPos);

    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    player->Hurt(pos, 2, 0xc000, 1, 0, 1);
}

/* Yaw the model, plant its translation at pos>>3, then the drop shadow.
 * In Yoshi's mouth (mFlags 0x40000) neither the shadow nor Render runs. */
// @symbol _ZN8daHuwa_c20UpdateModelTransformEv
void daHuwa_c::UpdateModelTransform() {
    Matrix4x3_FromRotationY(&mAnmModel.mat4x3, mAngleY);
    mAnmModel.mat4x3.m[9] = mPosX >> 3;
    mAnmModel.mat4x3.m[10] = mPosY >> 3;
    mAnmModel.mat4x3.m[11] = mPosZ >> 3;
    if (IsInYoshiMouth(this))
        return;
    if (mWithMeshClsn.IsOnGround() != 0) {
        DropShadowRadHeight(mShadowModel, mAnmModel.mat4x3, 0x50000, 0x1e000, 0xf);
    } else {
        DropShadowRadHeight(mShadowModel, mAnmModel.mat4x3, 0x50000, 0x96000, 0xf);
    }
}

// @symbol _ZN8daHuwa_c16CleanupResourcesEv
int daHuwa_c::CleanupResources() {
    gHuwahuwaModelModelFile.Release();
    gHuwahuwaMoveAnimationFile.Release();
    return 1;
}

// @symbol _ZN8daHuwa_c6RenderEv
int daHuwa_c::Render() {
    if (IsInYoshiMouth(this))
        return 1;
    mAnmModel.Render(0);
    return 1;
}

// @symbol _ZN8daHuwa_c8BehaviorEv
int daHuwa_c::Behavior() {
    int killed = UpdateKillByInvincibleChar(mWithMeshClsn, mAnmModel, 3);
    if (killed != 0) {
        if (killed == 2)
            KillWithCoins();
        return 1;
    }

    if (UpdateYoshiEat(mWithMeshClsn) != 0) {
        if (SpawnParticlesIfHitOtherObj(mdCcAc_c) != 0)
            KillWithCoins();
        UpdateModelTransform();
        mdCcAc_c.Clear();
        if (mEatenByYoshi != 0 && mSpitOutTimer == 0)
            mdCcAc_c.Update();
        return 1;
    }

    MakeVanishLuigiWork(mdCcAc_c);
    HandleCylinderHit();

    switch (mState) {
    case kStateChase: {
        Player *player;
        ApproachLinear(mHorzSpeed, 0x4000, 0x1000);
        player = ClosestPlayer();
        if (player != 0) {
            int *src = (int *)((int)player + 0x5c);
            int playerPos[3];
            playerPos[0] = src[0];
            playerPos[1] = src[1];
            playerPos[2] = src[2];
            if (Vec3_HorzDist((const Vector3 *)&mHomePosX, (const Vector3 *)playerPos) > 0x3e8000)
                mTargetAngY = Vec3_HorzAngle(&mPos, (const Vector3 *)&mHomePosX);
            else if (Vec3_HorzDist(&mPos, (const Vector3 *)playerPos) > 0x12c000)
                mTargetAngY = Vec3_HorzAngle(&mPos, (const Vector3 *)playerPos);
        } else {
            mTargetAngY = Vec3_HorzAngle(&mPos, (const Vector3 *)&mHomePosX);
        }
        ApproachLinear(mAngleY, mTargetAngY, 0x200);
        mPrevAngleY = mAngleY;
        break;
    }
    case kStateRecoil:
        *(unsigned short *)&mStateTimer += 1;
        if (*(unsigned short *)&mStateTimer >= kRecoilFrames)
            mState = kStateChase;
        break;
    }

    mAnmModel.Advance();
    UpdatePos(0);
    UpdateWMClsn(mWithMeshClsn, 0);

    if (IsGoingOffCliff(mWithMeshClsn, 0x3c000, 0x2888, 1, 1, 0x32000) != 0) {
        mPosX = mPrevPosX;
        mPosY = mPrevPosY;
        mPosZ = mPrevPosZ;
    }
    UpdateModelTransform();
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}

// @symbol _ZN8daHuwa_c13InitResourcesEv
int daHuwa_c::InitResources() {
    void *modelFile = dExtSimpleModel_c::LoadFile(gHuwahuwaModelModelFile);
    mAnmModel.dExtModel_c::SetFile((BMD_File *)modelFile, 1, -1);
    dExtFrameCtrl_c::LoadFile(gHuwahuwaMoveAnimationFile);
    mAnmModel.SetAnim((BCA_File *)gHuwahuwaMoveAnimationFile.filePtr, 0, 0x1000, 0);
    if (mShadowModel.InitCylinder() == 0)
        return 0;
    mdCcAc_c.Init(this, 0x3c000, 0x78000, 0x200000, 0xa6efe0);
    mWithMeshClsn.Init(this, 0x3c000, 0x3c000, 0, 0);
    mWithMeshClsn.StartDetectingWater();
    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    mState = 0;
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    return 1;
}

// @symbol _ZN8daHuwa_c13OnTurnIntoEggER6Player
void daHuwa_c::OnTurnIntoEgg(Player &player) {
    if (!player.IsCollectingCap())
        player.RegisterEggCoinCount(3, 0, 0);
    else
        GivePlayerCoins(player, 3, 0);
    KillAndTrackInDeathTable();
}

// @symbol _ZN8daHuwa_c13OnYoshiTryEatEv
s32 daHuwa_c::OnYoshiTryEat() {
    return 6;
}

// @symbol daHuwa_c_classInit
extern "C" daHuwa_c *daHuwa_c_classInit() {
    return new daHuwa_c();
}

/* The two file-scope resource handles, in retail initializer order: the
   model file (func_02017acc / func_02017ab4), then the animation file
   (SharedFilePtr::Construct / SharedFilePtr_Destruct_Anim). The manifest
   aliases each wrapper ctor/dtor onto those ROM symbols. */
HuwaModelFilePtr gHuwahuwaModelModelFile(ASSET_HUWAHUWA_MODEL_BMD);
HuwaAnimationFileHandle gHuwahuwaMoveAnimationFile(ASSET_HUWAHUWA_MOVE_BCA);
