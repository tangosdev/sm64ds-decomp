//cpp
/* Enemy - Mr. I (EYEKUN / EYEKUN_BOSS). Not Eyerok.
 * The small eye and the big one that holds a star turn in place and shoot
 * EYEKUN_BEAM. Circling one of them until the yaw count passes kDizzyYaw
 * makes it die. Wait, attack, and die are the three rows of
 * data_ov071_02123088.
 *
 * deslop leftovers:
 * - TextureSequence::SetFile as a member (Fix12<int> by value):
 *   ResetEyeAnim 0x58->0x64, UpdateEyeAnim 0x168->0x178.
 *   ModelAnim::SetAnim as a member: St_Die_Init 0xd4->0xe0.
 *   InitResources' SetAnim and SetFile together 0x298->0x2ac.
 * - DropShadowRadHeight as a member: UpdateModelTransform 0xa0->0xb0.
 * - dCcAcPos_c::Init as a member, both call sites: InitResources
 *   0x298->0x2b8.
 * - Particle::System::New, NewUnkCallback818, and NewSimple, and
 *   Player::Hurt, stay scalar externs. System does not declare the
 *   three New* calls, and Player.h does not declare Hurt. A Fix12<int>
 *   parameter is the caller cost measured on SetFile, SetAnim,
 *   DropShadow, and Init above.
 * - func_0201267c is the bank-3 veneer around Sound::Play. Sound::Play(3,
 *   id, Vector3 &) is +4 a call: St_Die_Init 0xd4->0xd8, St_Attack_Main
 *   0x314->0x318, St_Die_Main (two calls) 0x3dc->0x3e4. &mCamSpacePosX
 *   does not bind to Vector3 &.
 * - func_0200f760 clears cylinder +0x18 bit 2 unless the closest player's
 *   byte at +0x6fb is set. No recovered name.
 * - UpdateCircling `if (actorID == kBigMrI)` is 0x154->0x148. The (int)
 *   flag stays. CheckAttacks' egg test and its player test are each
 *   0x17c->0x170; both at once 0x17c->0x164.
 * - St_Wait_Main `mAngleY += mHorzSpeed` is 0x44->0x48.
 * - St_Attack_Main `(u16)mAngleX/Y/Z` is 44 words off at 0x314. The
 *   unsigned-short pun is what loads them with ldrh.
 * - CheckAttacks as one || is 6 words off at 0x17c. The gotos stay.
 * - St_Die_Main's blue-coin position as plain field copies is 13 words
 *   off at 0x3dc.
 * - St_Attack_Main's three aim points written off mTarget are
 *   0x314->0x32c. LookForPlayer's eye point written off the player is
 *   27 words off at 0xcc.
 * - (Vector3 *)&mPosX: dActor_c has no Pos().
 * - The shadow matrix is twelve s32s. Assigning a Matrix4x3 in
 *   InitResources is 0x298->0x2b4.
 * - data_ov071_02123038 / 02123040 are BTP files 0x2f8 / 0x2fb,
 *   02123048 is BCA 0x2f9, 02123050 is BMD 0x2f7. 021226a4 points at
 *   the two BTPs and 021226a0 at the BCA. data_ov002_0210da38 is the
 *   shared blue-coin model. SharedFilePtr has no fields, so the loaded
 *   word is read as LoadedFile.
 * - data_0209f2f8 is the current sublevel. 0x2e maps through
 *   SUBLEVEL_LEVEL_TABLE to course 19, which is not BBH's entrance
 *   (that is sublevel 0xc). On 0x2e the kill is entered in the death
 *   table; otherwise the eye is only marked for destruction.
 * - g_profile_EYEKUN, g_profile_EYEKUN_BOSS, and the two classInit
 *   factories stay outside this TU.
 */

#pragma defer_codegen off

#include "daEykn_c.h"
#include "common.h"
#include "types.h"
#include "SharedFilePtr.h"
#include "Player.h"
#include "Sound.h"
#include "Particle__System.h"
#include "dBgCh_Gnd.h"

/* Twelve plain words: assigning a Matrix4x3 copies it member by member,
   0x1c bytes longer than the cartridge's block copy. */
struct MatrixWords {
    s32 m[12];
};

/* A loaded SharedFilePtr: the second word is the file it loaded. */
struct LoadedFile {
    u32 fileID;
    void *file;
};

int ApproachLinear(int &value, int target, int step);
void ApproachLinear(short &value, short target, short step);
void UpdateAngle(s16 &angle, s16 target, int div, s16 maxStep);

extern "C" {
int AngleDiff(int a, int b);
u8 DecIfAbove0_Byte(u8 *counter);
s32 Vec3_Dist(const Vector3 *a, const Vector3 *b);
s16 Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
s16 Vec3_VertAngle(const Vector3 *a, const Vector3 *b);
void Matrix4x3_FromRotationXYZExt(Matrix4x3 *m, int x, int y, int z);
void LoadBlueCoinModel(void *actor);
void UnloadBlueCoinModel(void *actor);
void func_0201267c(u32 id, void *pos); /* Sound::Play(3, id, pos) */
void func_0200f760(void *actor, void *clsn);

/* Fix12<int> by value. Scalar parameters: the member form size-DIFFs.
   See the leftover list. */
int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 uniqueID, u32 effectID, int x, int y, int z, const void *dir, void *callback);
u32 _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
    u32 uniqueID, u32 effectID, int x, int y, int z, const Vector3_16f *dir);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 effectID, int x, int y, int z);
int _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(
    Player *player, void *source, u32 damage, int speed, u8 a, u8 b, u8 c);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
    ModelAnim *anim, void *file, int flags, int speed, u32 startFrame);
void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
    TextureSequence *seq, void *file, int flags, int speed, u32 startFrame);
void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
    dCcAcPos_c *clsn, dActor_c *actor, const Vector3 *offset, int radius, int height, u32 flags, u32 vulnFlags);
void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    dActor_c *actor, ShadowModel *shadow, Matrix4x3 *matrix, int radius, int depth, u32 opacity);

/* Sine and cosine pairs indexed by (angle >> 4) * 2. */
extern s16 data_02082214[];
extern s8 data_0209f2f8; /* current sublevel id */
extern Matrix4x3 IDENTITY_MATRIX4X3;
extern daEykn_c::State data_ov071_02123088[];
extern LoadedFile data_ov071_02123038; /* BTP 0x2f8 */
extern LoadedFile data_ov071_02123040; /* BTP 0x2fb */
extern LoadedFile data_ov071_02123048; /* BCA 0x2f9 */
extern SharedFilePtr data_ov002_0210da38; /* blue-coin model */
extern SharedFilePtr data_ov071_02123050; /* BMD 0x2f7 */
extern SharedFilePtr *data_ov071_021226a4[2]; /* the two BTPs */
extern SharedFilePtr *data_ov071_021226a0; /* the BCA */
}

/* Debug-table actor ids, and the thresholds this file compares against. */
enum {
    kYoshiEgg = 9,
    kPlayer = 0xbf,
    kSmallMrI = 0x106,
    kBigMrI = 0x107,
    kMrIBeam = 0x108,
    kBlueCoin = 0x122,
    kCircleFrames = 0x2e,
    kDizzyYaw = 0x17fff,
    kBigTurnLimit = 0x190,
    kSmallTurnLimit = 0x320,
    kWatchFov = 0x190,
    kWatchRange = 0x5dc000,
    kKillHit = 0x40000,
    kDeathPtcl0 = 0x13a,
    kDeathPtcl1 = 0x13b
};

// @symbol _ZN8daEykn_cD1Ev
// @symbol _ZN8daEykn_cD0Ev
daEykn_c::~daEykn_c()
{
}


// @symbol _ZN8daEykn_c14UpdateCirclingEv
/* Accumulates the yaw the eye turns while following a player around it in one
 * direction. Turning the other way, or too slowly, for 0x2e frames starts the
 * count again. Returns 1 once the count passes 0x17fff: the eye is dizzy. */
int daEykn_c::UpdateCircling()
{
    int delta;
    int isBig;
    int limit;

    delta = (short)(mAngleY - mTurnRefAngleY);
    isBig = (int)(actorID == kBigMrI);
    if (isBig != 0)
        limit = kBigTurnLimit;
    else
        limit = kSmallTurnLimit;

    if (delta > limit) {
        if (mCircleAngle >= 0) {
            mCircleAngle += delta;
            mCircleTimer = kCircleFrames;
        } else {
            if (mCircleTimer == 0)
                mCircleAngle = 0;
            DecIfAbove0_Byte(&mCircleTimer);
        }
    } else if (delta < -limit) {
        if (mCircleAngle <= 0) {
            mCircleAngle += delta;
            mCircleTimer = kCircleFrames;
        } else {
            if (mCircleTimer == 0)
                mCircleAngle = 0;
            DecIfAbove0_Byte(&mCircleTimer);
        }
    } else {
        if (mCircleTimer == 0)
            mCircleAngle = 0;
        DecIfAbove0_Byte(&mCircleTimer);
    }

    if (mCircleAngle > kDizzyYaw || mCircleAngle < -kDizzyYaw) {
        mCircleAngle = 0;
        mCircleTimer = kCircleFrames;
        return 1;
    }
    return 0;
}


// @symbol _ZN8daEykn_c13UpdateEyeAnimEv
/* Steps the eye's texture animation through two identical blinks: steps 1-3
 * and steps 4-6 each play the data_ov071_02123038 sequence, then the
 * data_ov071_02123040 sequence. Step 7 resets to 0 and returns 1, one call
 * after the second blink finishes. */
int daEykn_c::UpdateEyeAnim()
{
    switch (mSubState) {
    case 0:
        return 0;
    case 1:
    case 4:
        _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(&mTextureSequence, data_ov071_02123038.file, 0, 0x1000, 0);
        mTextureSequence.SetFlags(0x40000000);
        mTextureSequence.speed = 0x1000;
        mTextureSequence.currFrame = 0;
        mSubState++;
        /* fall through */
    case 2:
    case 5:
        if (mTextureSequence.Finished() != 0) {
            _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(&mTextureSequence, data_ov071_02123040.file, 0, 0x1000, 0);
            mTextureSequence.SetFlags(0x40000000);
            mTextureSequence.speed = 0x1000;
            mTextureSequence.currFrame = 0;
            mSubState++;
        }
        mTextureSequence.Advance();
        return 0;
    case 3:
    case 6:
        if (mTextureSequence.Finished() != 0)
            mSubState++;
        mTextureSequence.Advance();
        return 0;
    case 7:
        mSubState = 0;
        return 1;
    default:
        return 0;
    }
}


// @symbol _ZN8daEykn_c12ResetEyeAnimEv
void daEykn_c::ResetEyeAnim()
{
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(&mTextureSequence, data_ov071_02123038.file, 0, 0x1000, 0);
    mTextureSequence.SetFlags(0x40000000);
    mTextureSequence.speed = 0x1000;
    mTextureSequence.currFrame = 0;
    mSubState = 0;
}


// @symbol _ZN8daEykn_c12StartEyeAnimEv
/* Starts the blink that ends in a shot. Returns 0 if one is already running. */
int daEykn_c::StartEyeAnim()
{
    if (mSubState == 0) {
        mSubState++;
        return 1;
    }

    return 0;
}


// @symbol _ZN8daEykn_c13LookForPlayerEv
/* Watches for the closest visible player within range, inside the eye's
 * field of view and with a clear line of sight, and starts the attack. */
void daEykn_c::LookForPlayer()
{
    Player *p = ClosestNonVanishPlayer();
    if (p == 0)
        return;
    if (Vec3_Dist((Vector3 *)&mPosX, (Vector3 *)&p->mPosX) > kWatchRange)
        return;
    if (AngleDiff(Vec3_HorzAngle((Vector3 *)&mPosX, (Vector3 *)&p->mPosX), mAngleY) > kWatchFov)
        return;
    int px = p->mPosX;
    int pz = p->mPosZ;
    int py = p->mPosY + 0x8c000;
    Vector3 v;
    v.x = px;
    v.y = py;
    v.z = pz;
    if (DetectRaycastClsn(v, *(Vector3 *)&mPosX, false) != 0)
        return;
    mTarget = p;
    mCircleTimer = kCircleFrames;
    SetState(1);
}


// @symbol _ZN8daEykn_c12CheckAttacksEv
/* An egg (actor 9) or an explosion kills the eye. A player (actor 0xbf)
 * touching it is hurt, unless hit flag 0x40000 is set, which kills the eye
 * with particle systems kDeathPtcl0 and kDeathPtcl1 attached. */
void daEykn_c::CheckAttacks()
{
    /* One || is 6 words off. */
    dActor_c *egg = FindEgg(mdCcAcPos_c);
    if (egg != 0) {
        int isEgg9 = (int)(egg->actorID == kYoshiEgg);
        if (isEgg9) goto playSound;
    }

    if (FindExplosionActor(mdCcAcPos_c) == 0) goto idCheck;

playSound:
    Sound::PlayBank0(9, *(Vector3 *)&mCamSpacePosX);
    SetState(2);
    return;

idCheck:

    if (mdCcAcPos_c.otherOwner == 0) return;

    dActor_c *f = FindWithID(mdCcAcPos_c.otherOwner);
    if (f == 0) return;

    int isPlayer = (int)(f->actorID == kPlayer);
    if (!isPlayer) return;

    if (mdCcAcPos_c.hitFlags & kKillHit) {
        mParticleID0 = (u32)_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(mParticleID0, kDeathPtcl0, mPosX, mPosY, mPosZ, 0, 0);
        mParticleID1 = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(mParticleID1, kDeathPtcl1, mPosX, mPosY, mPosZ, 0);
        SetState(2);
        return;
    }

    Vector3 hv;
    hv.x = mPosX;
    hv.y = mPosY;
    hv.z = mPosZ;
    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj((Player *)f, &hv, 2, 0xc000, 1, 0, 1);
}


// @symbol _ZN8daEykn_c20UpdateModelTransformEv
void daEykn_c::UpdateModelTransform()
{
    Matrix4x3_FromRotationXYZExt(&mModelAnim.mat4x3, mAngleX, mAngleY, mAngleZ);
    mModelAnim.mat4x3.t.x = mPosX >> 3;
    mModelAnim.mat4x3.t.y = mPosY >> 3;
    mModelAnim.mat4x3.t.z = mPosZ >> 3;
    mShadowMat[9] = mPosX >> 3;
    mShadowMat[10] = mPosY >> 3;
    mShadowMat[11] = mPosZ >> 3;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(this, &mShadowModel, (Matrix4x3 *)mShadowMat, mScale * 0xb4, mShadowHeight, 0xf);
}


// @symbol _ZN8daEykn_c11St_Die_MainEv
/* State 2 exec: spin with a wobble, play the death animation, shrink to scale
 * 0xa4, then despawn, spawning actor 0x122 (small one) or releasing the star
 * (big one). */
int daEykn_c::St_Die_Main()
{
    u32 id0;

    ApproachLinear(mHorzSpeed, mDeathSpinSpeed, 300);
    mAngleY = (s16)(mAngleY + mHorzSpeed);
    mDeathSpinAngle += mHorzSpeed;
    if (mDeathSpinAngle / 131070 != 0) {
        func_0201267c(0x119, &mCamSpacePosX);
    }
    mDeathSpinAngle %= 131070;

    id0 = mParticleID0;
    if (id0 != 0 && mParticleID1 != 0) {
        Particle::System *p0;
        Particle::System *p1;
        mParticleID0 = (u32)_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            id0, kDeathPtcl0, mPosX, mPosY, mPosZ, 0, 0);
        mParticleID1 = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
            mParticleID1, kDeathPtcl1, mPosX, mPosY, mPosZ, 0);
        p0 = Particle::System::FromUniqueID(mParticleID0);
        p1 = Particle::System::FromUniqueID(mParticleID1);
        if (p0 != 0) {
            p0->callbackScale = 0x7fff;
        }
        if (p1 != 0) {
            p1->callbackScale = 0x7fff;
        }
    }

    switch (mSubState) {
    case 0: {
        s16 sinv = data_02082214[((int)mWobblePhase >> 4) * 2];
        mAngleX = (s16)((int)(((s64)mWobbleAmp * sinv + 0x800) >> 12));
        mWobblePhase = (u16)((s16)mWobblePhase + 0xe000);
        ApproachLinear(mWobbleAmp, 0, 0x1b);
        if (DecIfAbove0_Byte(&mSubTimer) == 0) {
            mSubState++;
        }
        break;
    }
    case 1:
        mModelAnim.Advance();
        if (mModelAnim.Finished() != 0) {
            mSubState++;
        }
        break;
    case 2: {
        int scale;
        unsigned short kind;
        int isSmall;
        if (ApproachLinear(mScale, 0xa4, 0xa4) != 0) {
            mSubState++;
        }
        scale = mScale;
        mScaleX = scale;
        mScaleY = scale;
        mScaleZ = scale;
        kind = actorID;
        isSmall = (kind == kSmallMrI);
        if (isSmall != 0) {
            mdCcAcPos_c.radius = mScale * 0x55;
        } else {
            int isBig = (kind == kBigMrI);
            if (isBig != 0) {
                mdCcAcPos_c.radius = mScale * 0x55;
            }
        }
        break;
    }
    case 3: {
        unsigned short kind = actorID;
        int isSmall = (kind == kSmallMrI);
        if (isSmall != 0) {
            /* Plain field copies are 13 words off. */
            int yadj, zcopy, y, z, x;
            Vector3 pos;
            y = mPosY;
            z = mPosZ;
            yadj = 0x78000;
            yadj = y + yadj;
            zcopy = z;
            x = mPosX;
            pos.x = x;
            pos.z = zcopy;
            pos.y = yadj;
            Spawn(kBlueCoin, 2, pos, 0, mAreaId, -1);
            PoofDust();
        } else {
            int isBig = (kind == kBigMrI);
            if (isBig != 0) {
                unsigned char star = (unsigned char)(param1 & 0xf);
                UntrackAndSpawnStar(mStarTrackID, star, *(Vector3 *)&mPosX, 4);
                _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x124, mPosX, mPosY, mPosZ);
                _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x125, mPosX, mPosY, mPosZ);
                _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x126, mPosX, mPosY, mPosZ);
            }
        }
        func_0201267c(0xc4, &mCamSpacePosX);
        if (data_0209f2f8 == 0x2e) {
            KillAndTrackInDeathTable();
        } else {
            MarkForDestruction();
        }
        break;
    }
    }
    return 1;
}


// @symbol _ZN8daEykn_c11St_Die_InitEv
/* State 2 init: spin on in the direction of the last turn. */
int daEykn_c::St_Die_Init()
{
    short diff = mAngleY - mTurnRefAngleY;
    mHorzSpeed = diff;
    if (mHorzSpeed > 0)
        mDeathSpinSpeed = 0x2500;
    else
        mDeathSpinSpeed = -0x2500;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, data_ov071_02123048.file, 0, 0x1000, 0);
    mModelAnim.SetFlags(0x40000000);
    mModelAnim.speed = 0x2800;
    mModelAnim.currFrame = 0;
    mSubState = 0;
    mSubTimer = kCircleFrames;
    mWobblePhase = 0;
    func_0201267c(0x119, &mCamSpacePosX);
    mFlags &= ~1;
    mStateID = 2;
    mWobbleAmp = 0x500;
    return 1;
}


// @symbol _ZN8daEykn_c14St_Attack_MainEv
/* State 1 exec: turn toward the target, blink and fire a bullet (actor 0x108)
 * each time the blink reaches step 7, and fall back to waiting when the target
 * leaves range, vanishes or goes behind terrain. The three aim points
 * are built through locals; written off mTarget the function grows
 * 0x314->0x32c. */
int daEykn_c::St_Attack_Main()
{
    Vector3_16 rot;
    Vector3 target1;
    Vector3 target2;
    Vector3 pos;
    Vector3 target3;
    s16 ang;

    {
        Player *pl;
        s32 py;
        s32 pz;
        s32 yoff;

        pl = mTarget;
        py = pl->mPosY;
        pz = pl->mPosZ;
        yoff = py + 0x78000;
        target1.x = pl->mPosX;
        target1.y = yoff;
        target1.z = pz;
    }

    Vec3_HorzAngle((Vector3 *)&mPosX, &target1);
    ang = Vec3_VertAngle((Vector3 *)&mPosX, &target1);
    UpdateAngle(mAngleX, ang, 2, 0x320);
    ang = Vec3_HorzAngle((Vector3 *)&mPosX, &target1);
    UpdateAngle(mAngleY, ang, 2, 0x8fc);

    if ((s16)(mAngleY - mTurnRefAngleY) == 0) {
        if (DecIfAbove0_Byte(&mShotTimer) == 0 && StartEyeAnim() != 0)
            mShotTimer = 0x53;
        unk_212 = 0xf0;
    }

    if (mSubState == 7) {
        Player *pl2;
        s32 py2;
        s32 pz2;
        s32 yoff2;

        pl2 = mTarget;
        py2 = pl2->mPosY;
        pz2 = pl2->mPosZ;
        yoff2 = py2 + 0x4b000;
        target2.x = pl2->mPosX;
        target2.y = yoff2;
        target2.z = pz2;

        {
            s32 px;
            s32 pz;
            s32 scale;
            s32 round;
            int idx;
            s16 s;
            int isBig;
            int param;

            px = mPosX;
            scale = 0x50000;
            pos.x = px;
            pos.y = mPosY;
            pz = mPosZ;
            round = 0x800;
            pos.z = pz;

            {
                /* ldrh. A (u16) cast is 44 words off. */
                unsigned short rx = *(unsigned short *)&mAngleX;
                unsigned short ry = *(unsigned short *)&mAngleY;
                rot.y = ry;
                rot.x = rx;
                unsigned short rz = *(unsigned short *)&mAngleZ;
                rot.z = rz;
            }

            idx = (u16)mAngleY >> 4;
            s = data_02082214[idx * 2];
            pos.x = px + (s32)(((s64)s * scale + round) >> 12);

            idx = (u16)mAngleY >> 4;
            s = data_02082214[idx * 2 + 1];
            pos.z = pz + (s32)(((s64)s * scale + round) >> 12);

            rot.x = Vec3_VertAngle(&pos, &target2);

            isBig = (actorID == kBigMrI);
            if (isBig != 0)
                param = 1;
            else
                param = 0;
            Spawn(kMrIBeam, param, pos, &rot, mAreaId, -1);
            func_0201267c(0x165, &mCamSpacePosX);
            mCircleTimer = kCircleFrames;
            unk_212 = 0xf0;
            mShotTimer = 0x53;
            mCircleAngle = 0;
            ResetEyeAnim();
        }
    }

    UpdateEyeAnim();

    {
        Player *p3;
        s32 y3;
        s32 z3;
        s32 yoff3;

        p3 = mTarget;
        y3 = p3->mPosY;
        z3 = p3->mPosZ;
        yoff3 = y3 + 0x8c000;
        target3.x = p3->mPosX;
        target3.y = yoff3;
        target3.z = z3;
    }

    if (UpdateCircling() != 0) {
        SetState(2);
    } else if (Vec3_Dist((Vector3 *)&mPosX, &target1) > kWatchRange) {
        SetState(0);
    } else if (mTarget->mIsVanish != 0) {
        SetState(0);
    } else if (DetectRaycastClsn(target3, *(Vector3 *)&mPosX, false) != 0) {
        SetState(0);
    }

    CheckAttacks();
    return 1;
}


// @symbol _ZN8daEykn_c14St_Attack_InitEv
int daEykn_c::St_Attack_Init()
{
    unk_212 = 0xf0;
    mShotTimer = 0;
    mCircleAngle = 0;
    mStateID = 1;
    ResetEyeAnim();
    return 1;
}


// @symbol _ZN8daEykn_c12St_Wait_MainEv
/* State 0 exec: level out, keep turning, and look for a player. */
int daEykn_c::St_Wait_Main()
{
    ApproachLinear(mAngleX, 0, 0x320);
    mAngleY = mAngleY + mHorzSpeed;
    LookForPlayer();
    CheckAttacks();
    return 1;
}


// @symbol _ZN8daEykn_c12St_Wait_InitEv
/* State 0 init: drift on in the direction of the last turn. */
int daEykn_c::St_Wait_Init()
{
    short v = mAngleY - mTurnRefAngleY;
    if (v >= 0) mHorzSpeed = 0xc8;
    else mHorzSpeed = -0xc8;
    ResetEyeAnim();
    mTarget = 0;
    mStateID = 0;
    return 1;
}


// @symbol _ZN8daEykn_c8RunStateEv
void daEykn_c::RunState()
{
    (this->*mState->exec)();
}


// @symbol _ZN8daEykn_c12RunStateInitEv
void daEykn_c::RunStateInit()
{
    (this->*mState->init)();
}


// @symbol _ZN8daEykn_c8SetStateEi
void daEykn_c::SetState(int state)
{
    mState = &data_ov071_02123088[state];
    RunStateInit();
}


// @symbol _ZN8daEykn_c16CleanupResourcesEv
s32 daEykn_c::CleanupResources()
{
    UnloadBlueCoinModel(this);
    data_ov002_0210da38.Release();
    data_ov071_02123050.Release();
    for (s32 i = 0; i < 2; ++i) {
        data_ov071_021226a4[i]->Release();
    }
    data_ov071_021226a0->Release();
    return 1;
}


// @symbol _ZN8daEykn_c16OnPendingDestroyEv
/* daEykn_c::OnPendingDestroy -- vtable slot 12. The ROM body is empty: the
 * override exists only to occupy the slot. */
void daEykn_c::OnPendingDestroy()
{
}


// @symbol _ZN8daEykn_c6RenderEv
int daEykn_c::Render()
{
    mTextureSequence.Update(mModelAnim.data);
    mModelAnim.Render((Vector3 *)&mScaleX);
    return 1;
}


// @symbol _ZN8daEykn_c8BehaviorEv
int daEykn_c::Behavior()
{
    RunState();
    func_0200f760(this, &mdCcAcPos_c);
    mTurnRefAngleY = mAngleY;
    mdCcAcPos_c.Clear();
    mdCcAcPos_c.Update();
    UpdateModelTransform();
    return 1;
}

// @symbol _ZN8daEykn_c13InitResourcesEv
/* LoadBlueCoinModel ignores its argument, but every caller passes the actor
 * in r0; declared (void) the entry block's argument setup comes out rotated. */
s32 daEykn_c::InitResources()
{
    BMD_File *bmd;
    LoadBlueCoinModel(this);

    Model::LoadFile(data_ov002_0210da38);
    bmd = (BMD_File *)Model::LoadFile(data_ov071_02123050);
    ((ModelBase *)&mModelAnim)->SetFile(bmd, 1, 1);

    int i;
    for (i = 0; i < 2; i++) {
        SharedFilePtr *seq = data_ov071_021226a4[i];
        TextureSequence::LoadFile(*seq);
        BMD_File *bmd2 = (BMD_File *)((LoadedFile *)&data_ov071_02123050)->file;
        BTP_File *btp = (BTP_File *)((LoadedFile *)seq)->file;
        TextureSequence::Prepare(*bmd2, *btp);
    }

    Animation::LoadFile(*data_ov071_021226a0);

    if (!mShadowModel.InitCylinder())
        return 0;

    unsigned short kind = actorID;
    int isSmall = (kind == kSmallMrI);
    if (isSmall) {
        Vector3 v;
        v.x = 0;
        v.y = -0x4b000;
        v.z = 0;
        _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(&mdCcAcPos_c, this, &v, 0x55000, 0x96000, 0x200004, 0x42000);
        mScaleX = 0x1000;
        mScaleY = 0x1000;
        mScaleZ = 0x1000;
        mScale = 0x1000;
    } else {
        int isBig = (kind == kBigMrI);
        if (isBig) {
            Vector3 v;
            v.x = 0;
            v.y = -0x96000;
            v.z = 0;
            _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(&mdCcAcPos_c, this, &v, 0xaa000, 0x12c000, 0x200004, 0);
            mScaleX = 0x2000;
            mScaleY = 0x2000;
            mScaleZ = 0x2000;
            mScale = 0x2000;
            {
                unsigned char starID = (unsigned char)(param1 & 0xf);
                mStarTrackID = TrackStar(starID, 2);
            }
        }
    }

    mVertAccel = 0;
    mTerminalVelocity = 0;
    SetState(0);

    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, data_ov071_02123048.file, 0, 0x1000, 0);

    mModelAnim.speed = 0x1000;
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(&mTextureSequence, data_ov071_02123038.file, 0, 0x1000, 0);

    mTextureSequence.speed = 0x1000;
    mTarget = 0;
    mCircleTimer = kCircleFrames;

    *(MatrixWords *)mShadowMat = *(MatrixWords *)&IDENTITY_MATRIX4X3;

    dBgCh_Gnd ray;
    ray.SetObjAndPos(*(Vector3 *)&mPosX, this);
    int y;
    if (ray.DetectClsn()) {
        y = (mPosY - ray.clsnY) + 0x1e000;
    } else {
        y = 0x12c000;
    }
    mShadowHeight = y;
    UpdateModelTransform();

    return 1;
}
