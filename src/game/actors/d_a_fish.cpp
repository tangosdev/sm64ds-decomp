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
 * daFish_c members under their address names; ov100's static initializer
 * copies the handler pointer-to-members into data_ov100_02148a1c, which
 * Behavior calls through.
 *
 * Known limits:
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

bool ApproachLinear(short &value, short target, short step);

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
void Vec3_Sub(Vector3 *out, Vector3 *a, Vector3 *b);
int Vec3_HorzLen(const Vector3 *v);
s16 Vec3_HorzAngle(const Vector3 *from, const Vector3 *to);
void Vec3_Asr(Vector3 *dst, Vector3 *src, int shift);
void Matrix4x3_FromTranslation(Matrix4x3 *matrix, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationY(Matrix4x3 *matrix, s16 angle);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
    ModelAnim *self, BCA_File *file, int flags, int speed,
    unsigned int startFrame);

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
    dExtFrameCtrl_c::LoadFile(data_ov100_021489cc);
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
        (BCA_File *)dExtFrameCtrl_c::LoadFile(*data_ov100_021473b0[mModelIndex]),
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
        if (spawner == 0 || spawner->func_ov100_0214639c() != 0) {
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
        spawner->func_ov100_02146280();
    return 1;
}

// @symbol _ZN8daFish_c19func_ov100_02146828Ev
/* Spawner state 0: wait for the player, then spawn the school. The legacy
   one-function source needed `#pragma opt_loop_invariants off`; the whole
   TU matches without it. */
void daFish_c::func_ov100_02146828()
{
    Vector3 diff;
    Vector3_16 rot;
    Vector3 pos;
    int n;
    int i;
    unsigned int kind;
    Player *player;

    player = this->ClosestPlayer();
    if (player == 0) return;

    Vec3_Sub(&diff, (Vector3 *)&this->mPosX, (Vector3 *)&player->mPosX);
    if (Vec3_HorzLen(&diff) >= 0x1388000) return;

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
        if (n < 1) n = 1;
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
            dActor_c *fish = dActor_c::Spawn(
                kFishActorID, kind, pos, &rot, this->mAreaId, -1);
            if (fish != 0) {
                ((daFish_c *)fish)->func_ov100_0214629c(this->uniqueID);
                this->mNumFish++;
            }
        }
    }

    this->mState = 1;
}

// @symbol _ZN8daFish_c19func_ov100_021467e8Ev
/* Spawner state 1: wait for the player to go beyond 6000.0. */
int daFish_c::func_ov100_021467e8()
{
    if (this->ClosestPlayer()) {
        int d = this->DistToCPlayer();
        if (d < 0x1770000) return d;
    }
    this->mState = 2;
    return 2;
}

// @symbol _ZN8daFish_c19func_ov100_021467d4Ev
/* Spawner state 2: wait for the last fish to be gone. */
void daFish_c::func_ov100_021467d4()
{
    if (this->mNumFish == 0)
        this->mState = 0;
}

// @symbol _ZN8daFish_c19func_ov100_02146640Ev
/* Fish state 3: idle, turning to face the player; flee once it is close. */
void daFish_c::func_ov100_02146640()
{
    Vector3 d;
    int st = this->mStateTimer;
    if (st == 0) {
        unsigned r = RandomIntInternal(&data_0209e650);
        this->mHorzSpeed = ((r >> 15) & 0x1fff) + 0x3000;
        r = RandomIntInternal(&data_0209e650);
        this->mTriggerDist = ((r % 500) + 0x96) << 0xc;
        this->mModelAnim.speed = 0x2000;
    } else if (st == 0xa) {
        this->mModelAnim.speed = 0x1000;
    }

    Player *p = this->ClosestPlayer();
    if (p) {
        s16 ang = Vec3_HorzAngle((Vector3 *)&this->mPosX, (Vector3 *)&p->mPosX);
        if (ApproachLinear(this->mPrevAngleY, ang, 0x400) != 0) {
            if (this->mModelIndex == 0) {
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
                    &this->mModelAnim, LoadedBCA(&data_ov100_021489cc), 0, 0x1000, 0);
            }
        }
        Vec3_Sub(&d, (Vector3 *)&this->mPosX, (Vector3 *)&p->mPosX);
        if (Vec3_HorzLen(&d) >= this->mTriggerDist)
            return;
        this->mStateTimer = -1;
        this->mState = 4;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
            &this->mModelAnim, LoadedBCA(data_ov100_021473b0[this->mModelIndex]), 0, 0x1000, 0);
    } else {
        this->mStateTimer = -1;
        this->mState = 4;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
            &this->mModelAnim, LoadedBCA(data_ov100_021473b0[this->mModelIndex]), 0, 0x1000, 0);
    }
}

// @symbol _ZN8daFish_c19func_ov100_021464f4Ev
/* Fish state 4: flee from the player until far enough away. */
void daFish_c::func_ov100_021464f4()
{
    Player *pl;
    Vector3 v;
    if (this->mStateTimer == 0) {
        this->mMaxSpeed = (RandomIntInternal(&data_0209e650) & 0x3fff) + 0xd000;
        this->mTriggerDist = (((unsigned int)RandomIntInternal(&data_0209e650) % 0x12c) + 0x1f4) << 12;
        this->mTurnSpeed = (RandomIntInternal(&data_0209e650) & 0x3ff) + 0x400;
        this->mModelAnim.speed = 0x4000;
    } else if (this->mStateTimer == 0x14) {
        this->mModelAnim.speed = 0x1000;
    }
    if (this->mHorzSpeed < this->mMaxSpeed) {
        this->mHorzSpeed += 0x800;
    }
    pl = this->ClosestPlayer();
    if (pl == 0) return;
    ApproachLinear(this->mPrevAngleY,
        Vec3_HorzAngle((Vector3 *)&pl->mPosX, (Vector3 *)&this->mPosX), this->mTurnSpeed);
    Vec3_Sub(&v, (Vector3 *)&this->mPosX, (Vector3 *)&pl->mPosX);
    if (Vec3_HorzLen(&v) <= this->mTriggerDist) return;
    this->mStateTimer = -1;
    this->mState = 3;
    func_0201267c(kFleeSound, (Vector3 *)&this->mCamSpacePosX);
}

// @symbol _ZN8daFish_c19func_ov100_02146468Ev
/* Fish state 5: wander until the timer runs out or it strays too far. */
void daFish_c::func_ov100_02146468()
{
    Vector3 v;
    dActor_c *spawner = dActor_c::FindWithID(this->mSpawnerID);
    Vec3_Sub(&v, (Vector3 *)&this->mPosX, (Vector3 *)&spawner->mPosX);
    if (DecIfAbove0_Byte(&this->mWanderTimer) != 0) {
        if (Vec3_HorzLen(&v) < 0xfa000) return;
    }
    this->mTurnSpeed = 0x800;
    this->mTargetAngle = this->mPrevAngleY + 0x8000;
    this->mModelAnim.speed = 0x2000;
    this->mState = 6;
}

// @symbol _ZN8daFish_c19func_ov100_021463b0Ev
/* Fish state 6: turn about, then wander again once back in range. */
void daFish_c::func_ov100_021463b0()
{
    Vector3 v;
    if (!ApproachLinear(this->mPrevAngleY, this->mTargetAngle, this->mTurnSpeed))
        return;
    {
        dActor_c *spawner = dActor_c::FindWithID(this->mSpawnerID);
        Vec3_Sub(&v, (Vector3 *)&this->mPosX, (Vector3 *)&spawner->mPosX);
    }
    if (Vec3_HorzLen(&v) >= 0xfa000) return;
    this->mHorzSpeed = (((unsigned)RandomIntInternal(&data_0209e650) >> 15) & 0x1fff) + 0x2000;
    this->mModelAnim.speed = 0x1000;
    this->mWanderTimer = (unsigned char)((((unsigned)RandomIntInternal(&data_0209e650) >> 15) & 0x7f) + 0x3c);
    this->mState = 5;
}

// @symbol _ZN8daFish_c19func_ov100_0214639cEv
/* Is the spawner back in its idle wait? Its fish leave when it is. */
int daFish_c::func_ov100_0214639c()
{
    return this->mState == 2;
}

// @symbol _ZN8daFish_c19func_ov100_0214629cEj
/* Set up a fish its spawner has just created. */
void daFish_c::func_ov100_0214629c(u32 spawnerID)
{
    this->mVertAccel = 0;
    this->mTerminalVelocity = -0x1e000;
    unsigned int r = RandomIntInternal(&data_0209e650);
    int fc = this->mModelAnim.GetFrameCount();
    this->mModelAnim.currFrame = (unsigned short)(r % (unsigned)fc) << 12;
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
        this->mWanderTimer = (unsigned char)((((unsigned)RandomIntInternal(&data_0209e650) >> 15) & 0x7f) + 0x3c);
        this->mState = 5;
    } else {
        this->mState = 3;
    }
}

// @symbol _ZN8daFish_c19func_ov100_02146280Ev
/* A fish is gone: one fewer for its spawner. */
void daFish_c::func_ov100_02146280()
{
    if (this->mNumFish != 0) {
        this->mNumFish--;
    }
}

// @symbol _ZN8daFish_cD1Ev
// @symbol _ZN8daFish_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The inline destructor in the header
   emits D1 then D0 -- the cartridge's order -- and no D2. */
