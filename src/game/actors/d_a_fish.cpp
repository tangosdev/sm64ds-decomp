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
 * Known limits:
 * - The seven state functions keep C linkage and placeholder names. They
 *   are pointer-to-members on the real class (ov100's static initializer
 *   copies them into data_ov100_02148a1c, which Behavior calls through),
 *   but no ROM spelling survives.
 * - ModelAnim::SetAnim stays mangled: it takes Fix12<int> by value
 *   (notes/mwccarm-codegen.md 6az).
 * - SharedFilePtr carries no fields yet, so the loaded file pointer in a
 *   handle's second word is read through a cast.
 * - (Vector3 *)&mPosX: dActor_c stores the position as three scalars.
 * - data_0209caa0 / data_0209f32c are the arm9 level-state words that
 *   carry the "has water" flag and the water height; neither is named yet.
 */

#include "daFish_c.h"
#include "common.h"
#include "Player.h"
#include "SharedFilePtr.h"

struct daFishState {
    void (daFish_c::*func)();
};

extern "C" {
extern SharedFilePtr data_ov100_021489cc;    /* the shared fish animation */
extern SharedFilePtr *data_ov100_021473a4[]; /* BMD, by mModelIndex */
extern SharedFilePtr *data_ov100_021473b0[]; /* BCA, by mModelIndex */
extern daFishState data_ov100_02148a1c[];    /* by mState */
extern int data_0209e650;                    /* the shared random seed */
extern int data_0209caa0[];
extern int data_0209f32c;
extern Matrix4x3 data_020a0e68;              /* scratch matrix */

int RandomIntInternal(int *seed);
void func_0203b9b4(s32 *value, s32 initial);
void func_0201267c(unsigned int soundID, const Vector3 *camSpacePos);
unsigned char DecIfAbove0_Byte(unsigned char *counter);
int _Z14ApproachLinearRsss(s16 *value, s16 target, s16 step);
void Vec3_Sub(Vector3 *out, Vector3 *a, Vector3 *b);
int Vec3_HorzLen(const Vector3 *v);
s16 Vec3_HorzAngle(const Vector3 *from, const Vector3 *to);
void Vec3_Asr(Vector3 *dst, Vector3 *src, int shift);
void Matrix4x3_FromTranslation(Matrix4x3 *matrix, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationY(Matrix4x3 *matrix, s16 angle);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
    ModelAnim *self, BCA_File *file, int flags, int speed,
    unsigned int startFrame);

void func_ov100_02146280(daFish_c *spawner);
void func_ov100_0214629c(daFish_c *fish, u32 spawnerID);
int func_ov100_0214639c(daFish_c *spawner);
void func_ov100_021463b0(daFish_c *self);
void func_ov100_02146468(daFish_c *self);
void func_ov100_021464f4(daFish_c *self);
void func_ov100_02146640(daFish_c *self);
void func_ov100_021467d4(daFish_c *self);
int func_ov100_021467e8(daFish_c *self);
void func_ov100_02146828(daFish_c *self);
}

enum {
    kFishActorID = 0x158,
    kFleeSound = 0xc2,
};

/* Matrix4x3 contains a non-trivial Vector3 tail, whose generated assignment
 * schedules differently. This POD view preserves the ROM's three ldm/stm copies.
 */
struct RawMatrix4x3 { int words[12]; };

/* The same for the spawn rotation: C++'s memberwise Vector3_16 copy loads
 * each field sign-extended; the ROM moves three plain halfwords. */
struct RawVector3_16 { s16 words[3]; };

/* A SharedFilePtr's second word is the file it has loaded. */
static inline BCA_File *LoadedBCA(SharedFilePtr *ptr)
{
    return (BCA_File *)((void **)ptr)[1];
}

inline daFish_c::daFish_c()
{
    func_0203b9b4(&mSeed, 1);
}

// @symbol daFish_c_classInit
extern "C" daFish_c *daFish_c_classInit()
{
    return new daFish_c();
}

// @symbol _ZN8daFish_c13InitResourcesEv
s32 daFish_c::InitResources()
{
    u8 modelIndex;
    Animation::LoadFile(data_ov100_021489cc);
    mModelIndex = (param1 >> 4) & 7;
    modelIndex = mModelIndex;
    if (modelIndex > 2) {
        if (modelIndex < 6)
            mVariant = modelIndex - 2;
        mModelIndex = 0;
    }
    mModelAnim.SetFile(
        (BMD_File *)Model::LoadFile(*data_ov100_021473a4[mModelIndex]), 1, -1);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &mModelAnim,
        (BCA_File *)Animation::LoadFile(*data_ov100_021473b0[mModelIndex]),
        0, 0x1000, 0);
    mIsSpawner = 1;
    mSpawnerID = uniqueID;
    mState = 0;
    mSurfaceY = mPosY + 0xc8000;
    mStateTimer = 0;
    return 1;
}

// @symbol _ZN8daFish_c8BehaviorEv
s32 daFish_c::Behavior()
{
    Vector3 shiftedPos;
    daFish_c *spawner;
    if (mIsSpawner != 0) {
        (this->*data_ov100_02148a1c[mState].func)();
    } else {
        spawner = (daFish_c *)dActor_c::FindWithID(mSpawnerID);
        if (spawner == 0 || func_ov100_0214639c(spawner) != 0) {
            MarkForDestruction();
        } else {
            (this->*data_ov100_02148a1c[mState].func)();
            UpdatePos(0);
            mStateTimer++;
        }
        Vec3_Asr(&shiftedPos, (Vector3 *)&mPosX, 3);
        Matrix4x3_FromTranslation(
            &data_020a0e68, shiftedPos.x, shiftedPos.y, shiftedPos.z);
        mAngleY = mPrevAngleY;
        Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);
        *(RawMatrix4x3 *)&mModelAnim.mat4x3 =
            *(RawMatrix4x3 *)&data_020a0e68;
        mModelAnim.Advance();
    }
    return 1;
}

// @symbol _ZN8daFish_c6RenderEv
s32 daFish_c::Render()
{
    if (mIsSpawner == 0)
        mModelAnim.Render(0);
    return 1;
}

// @symbol _ZN8daFish_c16OnPendingDestroyEv
void daFish_c::OnPendingDestroy()
{
}

// @symbol _ZN8daFish_c16CleanupResourcesEv
s32 daFish_c::CleanupResources()
{
    daFish_c *spawner;
    data_ov100_021489cc.Release();
    data_ov100_021473a4[mModelIndex]->Release();
    data_ov100_021473b0[mModelIndex]->Release();
    if (mIsSpawner == 0 &&
        (spawner = (daFish_c *)dActor_c::FindWithID(mSpawnerID)) != 0)
        func_ov100_02146280(spawner);
    return 1;
}

// @symbol func_ov100_02146828
/* Spawner state 0: wait for the player, then spawn the school. The legacy
   one-function source needed `#pragma opt_loop_invariants off`; the whole
   TU matches without it. */
extern "C" void func_ov100_02146828(daFish_c *self)
{
    Vector3 diff;
    Vector3_16 rot;
    Vector3 pos;
    int n;
    int i;
    unsigned int kind;
    Player *player;

    player = self->ClosestPlayer();
    if (player == 0) return;

    Vec3_Sub(&diff, (Vector3 *)&self->mPosX, (Vector3 *)&player->mPosX);
    if (Vec3_HorzLen(&diff) >= 0x1388000) return;

    if (data_0209caa0[2] & 0x80000) {
        int lim = data_0209f32c - 0x64000;
        if (self->mSurfaceY > lim) {
            self->mSurfaceY = lim;
        }
    }

    {
        n = self->param1 & 0xf;
        *(RawVector3_16 *)&rot = *(RawVector3_16 *)&self->mPrevAngleX;
        self->mNumFish = 0;
        if (n < 1) n = 1;
    }
    if (self->mVariant != 0) {
        kind = (self->mVariant + 2) << 4;
    } else {
        kind = self->mModelIndex;
    }

    for (i = 0; i < n; i++) {
        int r;
        if (self->mVariant != 0) {
            r = RandomIntInternal(&data_0209e650);
            pos.x = self->mPosX + (((unsigned)r % 0x140) - 0xa0) * 0x1000;
            r = RandomIntInternal(&data_0209e650);
            pos.z = self->mPosZ + (((unsigned)r % 0x140) - 0xa0) * 0x1000;
        } else {
            r = RandomIntInternal(&data_0209e650);
            pos.x = self->mPosX + (((unsigned)r % 0x2bc) - 0x15e) * 0x1000;
            r = RandomIntInternal(&data_0209e650);
            pos.z = self->mPosZ + (((unsigned)r % 0x2bc) - 0x15e) * 0x1000;
        }

        r = RandomIntInternal(&data_0209e650);
        pos.y = (self->mSurfaceY - 0x64000) - (((unsigned)r >> 11 & 0xfff) * 0xc8);

        r = RandomIntInternal(&data_0209e650);
        rot.y = rot.y + (s16)((unsigned)r >> 16);

        {
            dActor_c *fish = dActor_c::Spawn(
                kFishActorID, kind, pos, &rot, self->mAreaId, -1);
            if (fish != 0) {
                func_ov100_0214629c((daFish_c *)fish, self->uniqueID);
                self->mNumFish++;
            }
        }
    }

    self->mState = 1;
}

// @symbol func_ov100_021467e8
/* Spawner state 1: wait for the player to go beyond 6000.0. */
extern "C" int func_ov100_021467e8(daFish_c *self)
{
    if (self->ClosestPlayer()) {
        int d = self->DistToCPlayer();
        if (d < 0x1770000) return d;
    }
    self->mState = 2;
    return 2;
}

// @symbol func_ov100_021467d4
/* Spawner state 2: wait for the last fish to be gone. */
extern "C" void func_ov100_021467d4(daFish_c *self)
{
    if (self->mNumFish == 0)
        self->mState = 0;
}

// @symbol func_ov100_02146640
/* Fish state 3: idle, turning to face the player; flee once it is close. */
extern "C" void func_ov100_02146640(daFish_c *self)
{
    Vector3 d;
    int st = self->mStateTimer;
    if (st == 0) {
        unsigned r = RandomIntInternal(&data_0209e650);
        self->mHorzSpeed = ((r >> 15) & 0x1fff) + 0x3000;
        r = RandomIntInternal(&data_0209e650);
        self->mTriggerDist = ((r % 500) + 0x96) << 0xc;
        self->mModelAnim.speed = 0x2000;
    } else if (st == 0xa) {
        self->mModelAnim.speed = 0x1000;
    }

    Player *p = self->ClosestPlayer();
    if (p) {
        s16 ang = Vec3_HorzAngle((Vector3 *)&self->mPosX, (Vector3 *)&p->mPosX);
        if (_Z14ApproachLinearRsss(&self->mPrevAngleY, ang, 0x400) != 0) {
            if (self->mModelIndex == 0) {
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
                    &self->mModelAnim, LoadedBCA(&data_ov100_021489cc), 0, 0x1000, 0);
            }
        }
        Vec3_Sub(&d, (Vector3 *)&self->mPosX, (Vector3 *)&p->mPosX);
        if (Vec3_HorzLen(&d) >= self->mTriggerDist)
            return;
        self->mStateTimer = -1;
        self->mState = 4;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
            &self->mModelAnim, LoadedBCA(data_ov100_021473b0[self->mModelIndex]), 0, 0x1000, 0);
    } else {
        self->mStateTimer = -1;
        self->mState = 4;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
            &self->mModelAnim, LoadedBCA(data_ov100_021473b0[self->mModelIndex]), 0, 0x1000, 0);
    }
}

// @symbol func_ov100_021464f4
/* Fish state 4: flee from the player until far enough away. */
extern "C" void func_ov100_021464f4(daFish_c *self)
{
    Player *pl;
    Vector3 v;
    if (self->mStateTimer == 0) {
        self->mMaxSpeed = (RandomIntInternal(&data_0209e650) & 0x3fff) + 0xd000;
        self->mTriggerDist = (((unsigned int)RandomIntInternal(&data_0209e650) % 0x12c) + 0x1f4) << 12;
        self->mTurnSpeed = (RandomIntInternal(&data_0209e650) & 0x3ff) + 0x400;
        self->mModelAnim.speed = 0x4000;
    } else if (self->mStateTimer == 0x14) {
        self->mModelAnim.speed = 0x1000;
    }
    if (self->mHorzSpeed < self->mMaxSpeed) {
        self->mHorzSpeed += 0x800;
    }
    pl = self->ClosestPlayer();
    if (pl == 0) return;
    _Z14ApproachLinearRsss(&self->mPrevAngleY,
        Vec3_HorzAngle((Vector3 *)&pl->mPosX, (Vector3 *)&self->mPosX), self->mTurnSpeed);
    Vec3_Sub(&v, (Vector3 *)&self->mPosX, (Vector3 *)&pl->mPosX);
    if (Vec3_HorzLen(&v) <= self->mTriggerDist) return;
    self->mStateTimer = -1;
    self->mState = 3;
    func_0201267c(kFleeSound, (Vector3 *)&self->mCamSpacePosX);
}

// @symbol func_ov100_02146468
/* Fish state 5: wander until the timer runs out or it strays too far. */
extern "C" void func_ov100_02146468(daFish_c *self)
{
    Vector3 v;
    dActor_c *spawner = dActor_c::FindWithID(self->mSpawnerID);
    Vec3_Sub(&v, (Vector3 *)&self->mPosX, (Vector3 *)&spawner->mPosX);
    if (DecIfAbove0_Byte(&self->mWanderTimer) != 0) {
        if (Vec3_HorzLen(&v) < 0xfa000) return;
    }
    self->mTurnSpeed = 0x800;
    self->mTargetAngle = self->mPrevAngleY + 0x8000;
    self->mModelAnim.speed = 0x2000;
    self->mState = 6;
}

// @symbol func_ov100_021463b0
/* Fish state 6: turn about, then wander again once back in range. */
extern "C" void func_ov100_021463b0(daFish_c *self)
{
    Vector3 v;
    if (!_Z14ApproachLinearRsss(&self->mPrevAngleY, self->mTargetAngle, self->mTurnSpeed))
        return;
    {
        dActor_c *spawner = dActor_c::FindWithID(self->mSpawnerID);
        Vec3_Sub(&v, (Vector3 *)&self->mPosX, (Vector3 *)&spawner->mPosX);
    }
    if (Vec3_HorzLen(&v) >= 0xfa000) return;
    self->mHorzSpeed = (((unsigned)RandomIntInternal(&data_0209e650) >> 15) & 0x1fff) + 0x2000;
    self->mModelAnim.speed = 0x1000;
    self->mWanderTimer = (unsigned char)((((unsigned)RandomIntInternal(&data_0209e650) >> 15) & 0x7f) + 0x3c);
    self->mState = 5;
}

// @symbol func_ov100_0214639c
/* Is the spawner back in its idle wait? Its fish leave when it is. */
extern "C" int func_ov100_0214639c(daFish_c *spawner)
{
    return spawner->mState == 2;
}

// @symbol func_ov100_0214629c
/* Set up a fish its spawner has just created. */
extern "C" void func_ov100_0214629c(daFish_c *fish, u32 spawnerID)
{
    fish->mVertAccel = 0;
    fish->mTerminalVelocity = -0x1e000;
    unsigned int r = RandomIntInternal(&data_0209e650);
    int fc = fish->mModelAnim.GetFrameCount();
    fish->mModelAnim.currFrame = (unsigned short)(r % (unsigned)fc) << 12;
    fish->mIsSpawner = 0;
    fish->mSpawnerID = spawnerID;
    if (fish->mVariant != 0) {
        s16 *angle = &fish->mPrevAngleY;
        *angle = *angle & 0x8000;
        unsigned char v = fish->mVariant;
        if (v == 3) {
            *angle = *angle + 0x2000;
        } else if (v == 2) {
            *angle = *angle + 0x6000;
        } else {
            *angle = *angle | 0x4000;
        }
        fish->mHorzSpeed = (((unsigned)RandomIntInternal(&data_0209e650) >> 15) & 0x1fff) + 0x2000;
        fish->mWanderTimer = (unsigned char)((((unsigned)RandomIntInternal(&data_0209e650) >> 15) & 0x7f) + 0x3c);
        fish->mState = 5;
    } else {
        fish->mState = 3;
    }
}

// @symbol func_ov100_02146280
/* A fish is gone: one fewer for its spawner. */
extern "C" void func_ov100_02146280(daFish_c *spawner)
{
    if (spawner->mNumFish != 0) {
        spawner->mNumFish--;
    }
}

// @symbol _ZN8daFish_cD1Ev
// @symbol _ZN8daFish_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The inline destructor in the header
   emits D1 then D0 -- the cartridge's order -- and no D2. */
