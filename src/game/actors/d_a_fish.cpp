//cpp
/**
 * A school of fish (profile FISH).
 *
 * The placed actor is an invisible spawner. States 0..2 are its own:
 * when the player comes within 5000.0 it spawns 1..15 fish (param1's low
 * nibble) below the water line, then waits for the player to go beyond
 * 6000.0, then waits for the last of its fish to be gone. Each fish follows
 * its spawner by uniqueID and removes itself once the spawner is gone or
 * back in state 2.
 *
 * States 3..6 are a fish's. An ordinary fish (mVariant 0) idles in state 3,
 * turning to face the player until it comes close, then flees in state 4
 * until it is far enough away again. The wandering kinds (mVariant 1..3)
 * swim in state 5 on a random timer, and turn about in state 6 once the
 * timer runs out or they stray 250.0 from the spawner.
 *
 * daFish_c_classInit is reconstructed (RTTI daFish_c, FISH registry).
 * Retail does not store that spelling. Historical alias: Fish_Spawn.
 *
 * FUNCTION ORDER IS THE REVERSE OF THE ROM'S -- mwccarm emits one .text
 * section per function in the reverse of source order.
 *
 * The seven state functions and the three spawner/fish helpers are
 * daFish_c members; ov100's static initializer
 * copies the handler pointer-to-members into data_ov100_02148a1c, which
 * Behavior calls through.
 *
 * What stays, and why:
 * - dExtAnmModel_c::SetAnim stays mangled: it takes Fix12<int> by value
 *   (notes/mwccarm-codegen.md 6az).
 * - SharedFilePtr::filePtr is a void *, so LoadedBCA casts it to the
 *   BCA_File * SetAnim takes.
 * - data_0209caa0 / data_0209f32c are the arm9 level-state words that
 *   carry the "has water" flag and the water height; neither is named yet.
 * - RawMatrix4x3 / RawVector3_16 are POD views whose aggregate copies
 *   keep the ROM's ldm/stm / halfword-move shapes.
 * - func_0203b9b4, Vec3_Sub and Vec3_Asr have no shared header, so they
 *   keep a local extern "C" block.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_fish_AssetId {
    ASSET_FISH_BMD = 0x0447,
    ASSET_FISH_A_BMD = 0x0448,
    ASSET_FISH_AB_SWIM_BCA = 0x0449,
    ASSET_FISH_B_BMD = 0x044a,
    ASSET_FISH_SWIM_BCA = 0x044b,
    ASSET_FISH_WAIT_BCA = 0x044c
};

#include "daFish_c.h"
#include "common.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "Sound.h"
#include "math/Approach.h"
#include "math/CalcLib.h"
#include "math/MtxStack.h"

struct daFishState {
    void (daFish_c::*func)();
};

/* File-scope objects at the end of this file construct the six resource
 * handles (model files 0x447/0x448/0x44a, animation files 0x44b/0x44c/0x449)
 * and fill the seven-entry state table. mwcc emits __sinit_d_a_fish.cpp
 * from those definitions. The wrapper names are local; the handle
 * constructors and destructors are the ROM resource-family functions,
 * aliased in the manifest. */
struct FishModelFilePtr : SharedFilePtr {
    FishModelFilePtr(u32 fileID);
    ~FishModelFilePtr();
};

struct FishAnimationFileHandle : SharedFilePtr {
    FishAnimationFileHandle(u32 fileID);
    ~FishAnimationFileHandle();
};

extern FishAnimationFileHandle data_ov100_021489cc; /* the shared fish animation */
extern FishModelFilePtr gFishModelFile;
extern FishModelFilePtr gFishAModelFile;
extern FishModelFilePtr gFishBModelFile;
extern FishAnimationFileHandle gFishSwimAnimationFile;
extern FishAnimationFileHandle gFishAbSwimAnimationFile;
extern SharedFilePtr *data_ov100_021473a4[]; /* BMD, by mModelIndex */
extern SharedFilePtr *data_ov100_021473b0[]; /* BCA, by mModelIndex */
extern daFishState data_ov100_02148a1c[];    /* by mState */
/* The ROM PMF constants the state table copies from (unlicensed .data). */
extern daFishState data_ov100_02148450;
extern daFishState data_ov100_02148460;
extern daFishState data_ov100_02148448;
extern daFishState data_ov100_02148468;
extern daFishState data_ov100_02148458;
extern daFishState data_ov100_02148470;
extern daFishState data_ov100_02148478;
extern int data_0209e650; /* the shared random seed */
extern int data_0209caa0[];
extern int data_0209f32c;

/* local extern: arm9 C helpers with no shared header yet. */
extern "C" {
void func_0203b9b4(s32 *value, s32 initial);
void Vec3_Sub(Vector3 *out, Vector3 *a, Vector3 *b);
void Vec3_Asr(Vector3 *dst, Vector3 *src, int shift);
}

enum {
    kFishActorID = 0x158,
    kFleeSound = 0xc2,
};

/* Matrix4x3 contains a non-trivial Vector3 tail, whose generated assignment
 * schedules differently. This POD view preserves the ROM's three ldm/stm copies.
 */
struct RawMatrix4x3 {
    int words[12];
};

/* The same for the spawn rotation: C++'s memberwise Vector3_16 copy loads
 * each field sign-extended; the ROM moves three plain halfwords. */
struct RawVector3_16 {
    s16 words[3];
};

/* The animation file a handle has loaded. */
static inline BCA_File *LoadedBCA(SharedFilePtr *ptr) {
    return (BCA_File *)ptr->filePtr;
}

inline daFish_c::daFish_c() {
    func_0203b9b4(&mSeed, 1);
}

// @symbol daFish_c_classInit
extern "C" daFish_c *daFish_c_classInit() {
    return new daFish_c();
}

// @symbol _ZN8daFish_c13InitResourcesEv
s32 daFish_c::InitResources() {
    u8 modelIndex;
    dExtFrameCtrl_c::LoadFile(data_ov100_021489cc);
    mModelIndex = (param1 >> 4) & 7;
    modelIndex = mModelIndex;
    if (modelIndex > 2) {
        if (modelIndex < 6)
            mVariant = modelIndex - 2;
        mModelIndex = 0;
    }
    mAnmModel.SetFile((BMD_File *)dExtSimpleModel_c::LoadFile(*data_ov100_021473a4[mModelIndex]), 1,
                      -1);
    mAnmModel.SetAnim((BCA_File *)dExtFrameCtrl_c::LoadFile(*data_ov100_021473b0[mModelIndex]), 0,
                      0x1000, 0);
    mIsSpawner = 1;
    mSpawnerID = uniqueID;
    mState = 0;
    mSurfaceY = mPosY + 0xc8000;
    mStateTimer = 0;
    return 1;
}

// @symbol _ZN8daFish_c8BehaviorEv
s32 daFish_c::Behavior() {
    Vector3 shiftedPos;
    daFish_c *spawner;
    if (mIsSpawner != 0) {
        (this->*data_ov100_02148a1c[mState].func)();
    } else {
        spawner = (daFish_c *)dActor_c::FindWithID(mSpawnerID);
        if (spawner == 0 || spawner->IsSpawnerIdle() != 0) {
            MarkForDestruction();
        } else {
            (this->*data_ov100_02148a1c[mState].func)();
            UpdatePos(0);
            mStateTimer++;
        }
        Vec3_Asr(&shiftedPos, &mPos, 3);
        Matrix4x3_FromTranslation(&data_020a0e68, shiftedPos.x, shiftedPos.y, shiftedPos.z);
        mAngleY = mPrevAngleY;
        Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);
        *(RawMatrix4x3 *)&mAnmModel.mat4x3 = *(RawMatrix4x3 *)&data_020a0e68;
        mAnmModel.Advance();
    }
    return 1;
}

// @symbol _ZN8daFish_c6RenderEv
s32 daFish_c::Render() {
    if (mIsSpawner == 0)
        mAnmModel.Render(0);
    return 1;
}

// @symbol _ZN8daFish_c16OnPendingDestroyEv
void daFish_c::OnPendingDestroy() {}

// @symbol _ZN8daFish_c16CleanupResourcesEv
s32 daFish_c::CleanupResources() {
    daFish_c *spawner;
    data_ov100_021489cc.Release();
    data_ov100_021473a4[mModelIndex]->Release();
    data_ov100_021473b0[mModelIndex]->Release();
    if (mIsSpawner == 0 && (spawner = (daFish_c *)dActor_c::FindWithID(mSpawnerID)) != 0)
        spawner->OnFishGone();
    return 1;
}

// @symbol _ZN8daFish_c18StateWaitForPlayerEv
/* Spawner state 0: wait for the player, then spawn the school. The legacy
   one-function source needed `#pragma opt_loop_invariants off`; the whole
   TU matches without it. */
void daFish_c::StateWaitForPlayer() {
    Vector3 diff;
    Vector3_16 rot;
    Vector3 pos;
    int n;
    int i;
    unsigned int kind;
    Player *player;

    player = this->ClosestPlayer();
    if (player == 0)
        return;

    Vec3_Sub(&diff, &this->mPos, &player->mPos);
    if (Vec3_HorzLen(&diff) >= 0x1388000)
        return;

    if (data_0209caa0[2] & 0x80000) {
        int lim = data_0209f32c - 0x64000;
        if (this->mSurfaceY > lim) {
            this->mSurfaceY = lim;
        }
    }

    {
        n = this->param1 & 0xf;
        *(RawVector3_16 *)&rot = *(RawVector3_16 *)&this->mPrevAngleX;
        this->mNumFish = 0;
        if (n < 1)
            n = 1;
    }
    if (this->mVariant != 0) {
        kind = (this->mVariant + 2) << 4;
    } else {
        kind = this->mModelIndex;
    }

    for (i = 0; i < n; i++) {
        int r;
        if (this->mVariant != 0) {
            r = RandomIntInternal(&data_0209e650);
            pos.x = this->mPosX + (((unsigned)r % 0x140) - 0xa0) * 0x1000;
            r = RandomIntInternal(&data_0209e650);
            pos.z = this->mPosZ + (((unsigned)r % 0x140) - 0xa0) * 0x1000;
        } else {
            r = RandomIntInternal(&data_0209e650);
            pos.x = this->mPosX + (((unsigned)r % 0x2bc) - 0x15e) * 0x1000;
            r = RandomIntInternal(&data_0209e650);
            pos.z = this->mPosZ + (((unsigned)r % 0x2bc) - 0x15e) * 0x1000;
        }

        r = RandomIntInternal(&data_0209e650);
        pos.y = (this->mSurfaceY - 0x64000) - (((unsigned)r >> 11 & 0xfff) * 0xc8);

        r = RandomIntInternal(&data_0209e650);
        rot.y = rot.y + (s16)((unsigned)r >> 16);

        {
            dActor_c *fish = dActor_c::Spawn(kFishActorID, kind, pos, &rot, this->mAreaId, -1);
            if (fish != 0) {
                ((daFish_c *)fish)->InitFromSpawner(this->uniqueID);
                this->mNumFish++;
            }
        }
    }

    this->mState = 1;
}

// @symbol _ZN8daFish_c19StateWaitPlayerAwayEv
/* Spawner state 1: wait for the player to go beyond 6000.0. */
int daFish_c::StateWaitPlayerAway() {
    if (this->ClosestPlayer()) {
        int d = this->DistToCPlayer();
        if (d < 0x1770000)
            return d;
    }
    this->mState = 2;
    return 2;
}

// @symbol _ZN8daFish_c19StateWaitSchoolGoneEv
/* Spawner state 2: wait for the last fish to be gone. */
void daFish_c::StateWaitSchoolGone() {
    if (this->mNumFish == 0)
        this->mState = 0;
}

// @symbol _ZN8daFish_c9StateIdleEv
/* Fish state 3: idle, turning to face the player; flee once it is close. */
void daFish_c::StateIdle() {
    Vector3 d;
    int st = this->mStateTimer;
    if (st == 0) {
        unsigned r = RandomIntInternal(&data_0209e650);
        this->mHorzSpeed = ((r >> 15) & 0x1fff) + 0x3000;
        r = RandomIntInternal(&data_0209e650);
        this->mTriggerDist = ((r % 500) + 0x96) << 0xc;
        this->mAnmModel.speed = 0x2000;
    } else if (st == 0xa) {
        this->mAnmModel.speed = 0x1000;
    }

    Player *p = this->ClosestPlayer();
    if (p) {
        s16 ang = Vec3_HorzAngle(&this->mPos, &p->mPos);
        if (ApproachLinear(this->mPrevAngleY, ang, 0x400) != 0) {
            if (this->mModelIndex == 0) {
                this->mAnmModel.SetAnim((BCA_File *)(LoadedBCA(&data_ov100_021489cc)), 0, 0x1000,
                                        0);
            }
        }
        Vec3_Sub(&d, &this->mPos, &p->mPos);
        if (Vec3_HorzLen(&d) >= this->mTriggerDist)
            return;
        this->mStateTimer = -1;
        this->mState = 4;
        this->mAnmModel.SetAnim((BCA_File *)(LoadedBCA(data_ov100_021473b0[this->mModelIndex])), 0,
                                0x1000, 0);
    } else {
        this->mStateTimer = -1;
        this->mState = 4;
        this->mAnmModel.SetAnim((BCA_File *)(LoadedBCA(data_ov100_021473b0[this->mModelIndex])), 0,
                                0x1000, 0);
    }
}

// @symbol _ZN8daFish_c9StateFleeEv
/* Fish state 4: flee from the player until far enough away. */
void daFish_c::StateFlee() {
    Player *pl;
    Vector3 v;
    if (this->mStateTimer == 0) {
        this->mMaxSpeed = (RandomIntInternal(&data_0209e650) & 0x3fff) + 0xd000;
        this->mTriggerDist = (((unsigned int)RandomIntInternal(&data_0209e650) % 0x12c) + 0x1f4)
                             << 12;
        this->mTurnSpeed = (RandomIntInternal(&data_0209e650) & 0x3ff) + 0x400;
        this->mAnmModel.speed = 0x4000;
    } else if (this->mStateTimer == 0x14) {
        this->mAnmModel.speed = 0x1000;
    }
    if (this->mHorzSpeed < this->mMaxSpeed) {
        this->mHorzSpeed += 0x800;
    }
    pl = this->ClosestPlayer();
    if (pl == 0)
        return;
    ApproachLinear(this->mPrevAngleY, Vec3_HorzAngle(&pl->mPos, &this->mPos), this->mTurnSpeed);
    Vec3_Sub(&v, &this->mPos, &pl->mPos);
    if (Vec3_HorzLen(&v) <= this->mTriggerDist)
        return;
    this->mStateTimer = -1;
    this->mState = 3;
    func_0201267c(kFleeSound, &this->mCamSpacePos);
}

// @symbol _ZN8daFish_c11StateWanderEv
/* Fish state 5: wander until the timer runs out or it strays too far. */
void daFish_c::StateWander() {
    Vector3 v;
    dActor_c *spawner = dActor_c::FindWithID(this->mSpawnerID);
    Vec3_Sub(&v, &this->mPos, &spawner->mPos);
    if (DecIfAbove0_Byte(&this->mWanderTimer) != 0) {
        if (Vec3_HorzLen(&v) < 0xfa000)
            return;
    }
    this->mTurnSpeed = 0x800;
    this->mTargetAngle = this->mPrevAngleY + 0x8000;
    this->mAnmModel.speed = 0x2000;
    this->mState = 6;
}

// @symbol _ZN8daFish_c14StateTurnAboutEv
/* Fish state 6: turn about, then wander again once back in range. */
void daFish_c::StateTurnAbout() {
    Vector3 v;
    if (!ApproachLinear(this->mPrevAngleY, this->mTargetAngle, this->mTurnSpeed))
        return;
    {
        dActor_c *spawner = dActor_c::FindWithID(this->mSpawnerID);
        Vec3_Sub(&v, &this->mPos, &spawner->mPos);
    }
    if (Vec3_HorzLen(&v) >= 0xfa000)
        return;
    this->mHorzSpeed = (((unsigned)RandomIntInternal(&data_0209e650) >> 15) & 0x1fff) + 0x2000;
    this->mAnmModel.speed = 0x1000;
    this->mWanderTimer =
        (unsigned char)((((unsigned)RandomIntInternal(&data_0209e650) >> 15) & 0x7f) + 0x3c);
    this->mState = 5;
}

// @symbol _ZN8daFish_c13IsSpawnerIdleEv
/* Is the spawner back in its idle wait? Its fish leave when it is. */
int daFish_c::IsSpawnerIdle() {
    return this->mState == 2;
}

// @symbol _ZN8daFish_c15InitFromSpawnerEj
/* Set up a fish its spawner has just created. */
void daFish_c::InitFromSpawner(u32 spawnerID) {
    this->mVertAccel = 0;
    this->mTerminalVelocity = -0x1e000;
    unsigned int r = RandomIntInternal(&data_0209e650);
    int fc = this->mAnmModel.GetFrameCount();
    this->mAnmModel.currFrame = (unsigned short)(r % (unsigned)fc) << 12;
    this->mIsSpawner = 0;
    this->mSpawnerID = spawnerID;
    if (this->mVariant != 0) {
        s16 *angle = &this->mPrevAngleY;
        *angle = *angle & 0x8000;
        unsigned char v = this->mVariant;
        if (v == 3) {
            *angle = *angle + 0x2000;
        } else if (v == 2) {
            *angle = *angle + 0x6000;
        } else {
            *angle = *angle | 0x4000;
        }
        this->mHorzSpeed = (((unsigned)RandomIntInternal(&data_0209e650) >> 15) & 0x1fff) + 0x2000;
        this->mWanderTimer =
            (unsigned char)((((unsigned)RandomIntInternal(&data_0209e650) >> 15) & 0x7f) + 0x3c);
        this->mState = 5;
    } else {
        this->mState = 3;
    }
}

// @symbol _ZN8daFish_c10OnFishGoneEv
/* A fish is gone: one fewer for its spawner. */
void daFish_c::OnFishGone() {
    if (this->mNumFish != 0) {
        this->mNumFish--;
    }
}

// @symbol _ZN8daFish_cD1Ev
// @symbol _ZN8daFish_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The inline destructor in the header
   emits D1 then D0 -- the cartridge's order -- and no D2. */

/* Static-init globals (was the handwritten __sinit_ov100_02147bc0 shard).
 * Definition order is the retail initializer's construction order: the
 * three model handles (files 0x447/0x448/0x44a), the three animation
 * handles (files 0x44b/0x44c/0x449), then the seven state records copied
 * from the ROM PMF constants. */
FishModelFilePtr gFishModelFile(ASSET_FISH_BMD);
FishModelFilePtr gFishAModelFile(ASSET_FISH_A_BMD);
FishModelFilePtr gFishBModelFile(ASSET_FISH_B_BMD);
FishAnimationFileHandle gFishSwimAnimationFile(ASSET_FISH_SWIM_BCA);
FishAnimationFileHandle data_ov100_021489cc(ASSET_FISH_WAIT_BCA);
FishAnimationFileHandle gFishAbSwimAnimationFile(ASSET_FISH_AB_SWIM_BCA);
daFishState data_ov100_02148a1c[7] = {
    data_ov100_02148450, data_ov100_02148460, data_ov100_02148448, data_ov100_02148468,
    data_ov100_02148458, data_ov100_02148470, data_ov100_02148478,
};
