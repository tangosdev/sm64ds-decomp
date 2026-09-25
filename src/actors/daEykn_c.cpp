//cpp
/* Production translation unit for ov071/daEykn_c.
 * 23 function(s), .text 0x02120668..0x021219cc. Mr. I (EYEKUN / EYEKUN_BOSS):
 * actor 0x106 is the small one, 0x107 the big one that holds a star.
 *
 * NAME: _ZTS8daEykn_c is "8daEykn_c" at ov071 0x02122cd8; _ZTI at 0x02122ce4
 * reads [__si_class_type_info, that string, _ZTI8dActor_c]. The vtable address
 * point _ZTV8daEykn_c is 0x02122d30 (offset-to-top 0 at 0x02122d28, typeinfo
 * 0x02122ce4 at 0x02122d2c). The tree previously called the class MrI (coined;
 * ov071/symbols.txt only aliased _ZTV3MrI to the same vtable address).
 *
 * The out-of-line destructor is the key function, so this TU emits _ZTV/_ZTI/
 * _ZTS (externalized to those addresses; the TU is text-only). Under
 * `#pragma defer_codegen off` it comes out D1 (0x02120668), D0 (0x021206b0),
 * then a D2 the cartridge has no home for (manifest: deadstrip); the same
 * pragma lays .text down in source order, so this file is ROM-ascending.
 *
 * Behavior runs one of three states out of the { init, exec } table at
 * data_ov071_02123088, which __sinit_ov071_021228c8 fills:
 *
 *   0 wait    init St_Wait_Init    exec St_Wait_Main
 *   1 attack  init St_Attack_Init  exec St_Attack_Main
 *   2 die     init St_Die_Init     exec St_Die_Main
 *
 * The state names are descriptive; the table has no name strings.
 *
 * InitResources (0x02121734) closes the unit. The two classInit factories
 * after it (src/d_a_eykn_eyekun_boss.c, src/d_a_eykn_eyekun.c) are separate
 * units and are not absorbed.
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
void func_0201267c(u32 id, void *pos);
void func_0200f760(void *actor, void *clsn);

/* Each of these takes a Fix12<int> by value, so they stay declared by their
   mangled names: DropShadowRadHeight as a member call measures 0xb0 bytes
   against the cartridge's 0xa0 in UpdateModelTransform. */
int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 uniqueID, u32 effectID, int x, int y, int z, const void *dir, void *callback);
u32 _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
    u32 uniqueID, u32 effectID, int x, int y, int z, const Vector3_16f *dir);
/* FindEgg keeps its mangled name: declared through dActor_c instead, the
   tree's plurality spelling of it flips and src/actors/Scuttlebug.cpp's
   declaration reads as a contradiction. */
void *_ZN8dActor_c7FindEggER5dCc_c(void *actor, void *clsn);
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
extern s8 data_0209f2f8;
extern Matrix4x3 IDENTITY_MATRIX4X3;
extern daEykn_c::State data_ov071_02123088[];
extern LoadedFile data_ov071_02123038;
extern LoadedFile data_ov071_02123040;
extern LoadedFile data_ov071_02123048;
extern SharedFilePtr data_ov002_0210da38;
extern SharedFilePtr data_ov071_02123050;
extern SharedFilePtr *data_ov071_021226a4[2];
extern SharedFilePtr *data_ov071_021226a0;
}

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
    /* The int flags here and below are load-bearing: testing actorID
       directly compiles differently. */
    isBig = (int)(actorID == 0x107);
    if (isBig != 0)
        limit = 0x190;
    else
        limit = 0x320;

    if (delta > limit) {
        if (mCircleAngle >= 0) {
            mCircleAngle += delta;
            mCircleTimer = 0x2e;
        } else {
            if (mCircleTimer == 0)
                mCircleAngle = 0;
            DecIfAbove0_Byte(&mCircleTimer);
        }
    } else if (delta < -limit) {
        if (mCircleAngle <= 0) {
            mCircleAngle += delta;
            mCircleTimer = 0x2e;
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

    if (mCircleAngle > 0x17fff || mCircleAngle < -0x17fff) {
        mCircleAngle = 0;
        mCircleTimer = 0x2e;
        return 1;
    }
    return 0;
}


// @symbol _ZN8daEykn_c13UpdateEyeAnimEv
/* Steps the eye's texture animation: close (steps 1-3), open (4-6), done (7).
 * Returns 1 on the step after the second opening finishes. */
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
    if (Vec3_Dist((Vector3 *)&mPosX, (Vector3 *)&p->mPosX) > 0x5dc000)
        return;
    if (AngleDiff(Vec3_HorzAngle((Vector3 *)&mPosX, (Vector3 *)&p->mPosX), mAngleY) > 0x190)
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
    mCircleTimer = 0x2e;
    SetState(1);
}


// @symbol _ZN8daEykn_c12CheckAttacksEv
/* An egg (actor 9) or an explosion kills the eye. A player (actor 0xbf)
 * touching it is hurt, unless hit flag 0x40000 is set, which kills the eye
 * with particle systems 0x13a and 0x13b attached. */
void daEykn_c::CheckAttacks()
{
    /* The gotos are load-bearing: the same test as one || condition misses. */
    dActor_c *egg = (dActor_c *)_ZN8dActor_c7FindEggER5dCc_c(this, &mdCcAcPos_c);
    if (egg != 0) {
        int isEgg9 = (int)(egg->actorID == 9);
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

    int isPlayer = (int)(f->actorID == 0xbf);
    if (!isPlayer) return;

    if (mdCcAcPos_c.hitFlags & 0x40000) {
        mParticleID0 = (u32)_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(mParticleID0, 0x13a, mPosX, mPosY, mPosZ, 0, 0);
        mParticleID1 = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(mParticleID1, 0x13b, mPosX, mPosY, mPosZ, 0);
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
/* State 2 exec: spin with a wobble, swell, then vanish, spawning actor 0x122
 * (small one) or releasing the star (big one). */
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
            id0, 0x13a, mPosX, mPosY, mPosZ, 0, 0);
        mParticleID1 = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
            mParticleID1, 0x13b, mPosX, mPosY, mPosZ, 0);
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
        /* Read back signed (ldrsh); a plain += misses. */
        *(s16 *)&mWobblePhase = (s16)(*(s16 *)&mWobblePhase + 0xe000);
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
        isSmall = (int)(kind == 0x106);
        if (isSmall != 0) {
            mdCcAcPos_c.radius = mScale * 0x55;
        } else {
            int isBig = (int)(kind == 0x107);
            if (isBig != 0) {
                mdCcAcPos_c.radius = mScale * 0x55;
            }
        }
        break;
    }
    case 3: {
        unsigned short kind = actorID;
        int isSmall = (int)(kind == 0x106);
        if (isSmall != 0) {
            /* This load order is the cartridge's; plain field copies miss. */
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
            Spawn(0x122, 2, pos, 0, mAreaId, -1);
            PoofDust();
        } else {
            int isBig = (int)(kind == 0x107);
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
    mSubTimer = 0x2e;
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
 * leaves range, vanishes or goes behind terrain. Each target vector is built
 * through locals; spelled straight off mTarget, the second or the third misses. */
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
                /* Read unsigned: the cartridge loads these with ldrh. */
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

            isBig = (int)(actorID == 0x107);
            if (isBig != 0)
                param = 1;
            else
                param = 0;
            Spawn(0x108, param, pos, &rot, mAreaId, -1);
            func_0201267c(0x165, &mCamSpacePosX);
            mCircleTimer = 0x2e;
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
    } else if (Vec3_Dist((Vector3 *)&mPosX, &target1) > 0x5dc000) {
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
        BMD_File *bmd2 = *(BMD_File **)((char *)&data_ov071_02123050 + 4);
        BTP_File *btp = *(BTP_File **)((char *)seq + 4);
        TextureSequence::Prepare(*bmd2, *btp);
    }

    Animation::LoadFile(*data_ov071_021226a0);

    if (!mShadowModel.InitCylinder())
        return 0;

    unsigned short kind = actorID;
    int isSmall = (kind == 0x106);
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
        int isBig = (kind == 0x107);
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
    mCircleTimer = 0x2e;

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
