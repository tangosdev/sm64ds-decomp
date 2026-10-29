//cpp
/* daGmch_c: the hopping purse that pays out coins (GAMAGUCHI), ov081.
 * Was coined Moneybag. _ZTS at 0x02128bc8, typeinfo at 0x02128bd4.
 * D1 0x02126504, D0 0x02126554, deadstripped D2. ROM-ascending under
 * defer_codegen off. The factory is the one free function.
 */
/* leftovers:
 * - common.h stays before daGmch_c.h. InitResources assigns
 *   IDENTITY_MATRIX4X3 into mMatrix; the .r/.t Matrix4x3 spelling is
 *   0x1f0 against the ROM's 0x1d4. decl_common.h is not included.
 * - Player::Bounce and Player::Hurt stay mangled (Fix12<int> by value).
 * - GetFloorResult / GetWallResult return the raw word; a dBgPi* return
 *   disagrees with the definitions, so the callers still cast.
 *   TouchesWater and cstd::fdiv are the const method and the divide.
 * - holder->mPosX/Y/Z in place of the word triple changes EnterState7 and
 *   UpdateState6.
 * - UpdateState0 keeps mPhase++. mPhase = (u8)(mPhase + 1) is 0x144
 *   against 0x158, and mSpinAngleY += 0xc00 is four words off.
 * - ApproachLinear2 steps *(int *)&mState because mState, mPhase and
 *   mTimer share one word. State numbers, mFlags bit 0x80000 and hit
 *   mask 0x66fe0 stay numeric.
 * - The loaded BCA is data_ov081_02128edc.filePtr. GmchDrawReg is the
 *   one-slot init sentinel the folded sinit needs.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daGmch_c_AssetId {
    ASSET_GAMAGUCHI_BMD = 0x02fd,
    ASSET_GAMAGUCHI_JUMP_BCA = 0x02fe,
    ASSET_GAMAGUCHI_JUMP_END_BCA = 0x02ff,
    ASSET_GAMAGUCHI_WALK_BCA = 0x0300,
    ASSET_GAMAGUCHI_WALK_END_BCA = 0x0301
};

#pragma defer_codegen off

#include "types.h"
#include "math/Approach.h"
/* BEFORE daGmch_c.h, and load-bearing: common.h and math/Matrix.h both define
   Matrix4x3 under one guard and whichever is seen FIRST stands.  InitResources
   assigns IDENTITY_MATRIX4X3 whole into mMatrix; under common.h flat s32[12]
   spelling mwccarm block-moves it, under math/Matrix.h Matrix3x3+Vector3
   spelling it splits into a 9-word ldm/stm plus a CSE tail -- measured here as
   0x1d4 against 0x1f0, +7 words.  The shards reached the flat spelling through
   decl_common.h -> common.h; this include is what replaces that route. */
#include "common.h"
#include "daGmch_c.h"
#include "dBgCh_Gnd.h"
#include "Player.h"
#include "Sound.h"
#include "dExtSimpleModel_c.h"
#include "dExtFrameCtrl_c.h"
#include "SharedFilePtr.h"
#include "SpawnInfo.h"
#include "nitro/fx/fx.h"


/* Actor ids are from symbols/actor_debug_names.tsv. */
enum {
    ACTOR_PLAYER = 191
};

/* dActor_c::mFlags bits 0x20000 / 0x40000: the yoshi-mouth states, written by
   actor code (see the bit table in dActor_c.h). */
enum {
    MFLAG_YOSHI_MOUTH_A = 0x20000,
    MFLAG_YOSHI_MOUTH_B = 0x40000
};

/* mdCcAc_c.hitFlags bit set when a mega-sized character hits the cylinder. */
enum {
    HIT_MEGA_CHARACTER = 0x10
};

/* Non-virtual state handlers. daGmch_c is already complete here; the
   pointer-to-member call through mStatePmfPair still matches. */
typedef void (daGmch_c::*StateFn)();

/* The model handle's ctor/dtor are the cartridge's func_02017acc /
 * func_02017ab4 pair; the manifest aliases these. */
struct GmchModelFileHandle : SharedFilePtr {
    GmchModelFileHandle(u32 fileID);
    ~GmchModelFileHandle();
};

/* The four animation handles construct through SharedFilePtr::Construct
 * and register SharedFilePtr_Destruct_Anim; the manifest aliases these. */
struct GmchAnimationFileHandle : SharedFilePtr {
    GmchAnimationFileHandle(u32 fileID);
    ~GmchAnimationFileHandle();
};

/* The state table's PMF type.  Sixteen of the eighteen halves return int;
   EnterState1 is the lone void (its header decl stands), so it is cast in
   below.  The invokers keep reading the pair through StateFn. */
typedef int (daGmch_c::*StatePmf)();
struct StatePair { StatePmf enter, update; };

/* The draw offset is a plain Vector3, and `Vector3 v = {...}` cannot
   produce its init: brace init is constant initialization, so nothing runs
   in __sinit at all -- no stores, no destructor registration.  mwcc splits
   the two retail actions the way it does because the cartridge object was
   constructed: the stores come from a constructor body and the destructor
   registers as _ZN7Vector3D1Ev.  This one-slot sentinel supplies the
   stores from its ctor; the declared-ahead Vector3 then earns only the
   registration.  The sentinel's own 1-byte bss has no cartridge counterpart
   and is deadstripped in the manifest. */
struct GmchDrawReg { GmchDrawReg(Vector3 &v) { v.x = 0; v.y = 0x2000; v.z = 0; } };

/* External function bridges used by class methods, with C linkage.
 *
 * A class member function may not sit in a linkage-specification region, so
 * once a member becomes `daGmch_c::Something` a declaration written in its body
 * gets C++ linkage and the reference mangles.  The bridges used by these
 * methods are declared once here. The factory is `new daGmch_c()` and does
 * not redeclare the member constructors.
 *
 * Of the original 54 declarations, exactly NINE had more than one type spelling
 * across the 36 shards: Vec3_Dist, dActor_c::DistToCPlayer, func_02038414,
 * dExtAnmModel_c::SetAnim, Vec3_HorzAngle, DecIfAbove0_Byte, Sound::PlayBank0,
 * ApproachLinear and RandomIntInternal.  Collapsing each onto ONE spelling was
 * measured, not assumed, and eight of the nine cost nothing:
 *
 *   TWO were never different types at all.  Fix12i is `typedef s32`, so
 *   Vec3_Dist and dActor_c::DistToCPlayer spelt `Fix12i` and spelt `int` are
 *   the same declaration written twice.
 *
 *   FIVE disagreed only about something no call site can see: a return value
 *   that every caller discards or compares against zero (func_02038414,
 *   dExtAnmModel_c::SetAnim, DecIfAbove0_Byte) or which pointer type spells the same
 *   address (Vec3_HorzAngle, dExtAnmModel_c::SetAnim, and ApproachLinear, which
 *   takes the short & its mangled name encodes).
 *
 *   ONE needed a call site adapted rather than a declaration chosen.
 *   Sound::PlayBank0's position is the camera-space vector, passed as
 *   mCamSpacePos.
 *   Same address, same register, and the member still matches.
 *
 * The NINTH is genuinely load-bearing: RandomIntInternal must return
 * `unsigned int`.  Measured -- declaring it `int` leaves 36/37 matching and
 * breaks ChooseNextState alone, which is the only caller that takes `% 10` of
 * the result and so the only one that pays for a signed remainder.
 *
 * Data is different -- mwccarm leaves a file-scope variable's name unmangled in
 * C++ -- so every `data_*` declaration stays at block scope in the member that
 * recovered it.  The sinit's own five handles and the PMF state table moved
 * here wholesale as file-scope definitions: GmchModelFileHandle /
 * GmchAnimationFileHandle spell the model/animation ctors and dtors, and
 * GmchDrawReg supplies the draw offset's .init stores so the plain Vector3
 * earns its registered destructor instead of constant initialization. */
extern "C" {
extern Fix12i Vec3_Dist(const void *a, const void *b);
extern s16    Vec3_HorzAngle(const void *a, const void *b);
extern unsigned int RandomIntInternal(int *seed);
extern int    func_02038414(void *clsn);
extern void   Matrix4x3_FromRotationY(void *m, int angle);
extern void   Matrix4x3_ApplyInPlaceToTranslation(void *m, int x, int y, int z);
extern void   Matrix4x3_ApplyInPlaceToRotationX(void *m, s16 angX);
extern void   Matrix4x3_ApplyInPlaceToRotationY(void *m, s16 angY);
extern int DecIfAbove0_Byte(void *p);
namespace Particle {
struct System {
    static void NewSimple(unsigned int n, int a, int b, int c);
};
}
/* Parameter names keep this TU's @NNN. */
typedef void BgchInitAt_(void *self, void *actor, int a, int b, void *v, int c);
}

namespace cstd {
int fdiv(int a, int b);
}

// @symbol _ZN8daGmch_cD1Ev
// @symbol _ZN8daGmch_cD0Ev
/* One definition, two ROM variants (the D2 mwccarm also emits is
   deadstripped), and this is the key function: it is the first virtual
   daGmch_c.h declares out of line, so this TU is where the class's
   _ZTV/_ZTI/_ZTS are emitted.  The compiler writes both bodies -- the vptr
   store, the five members destroyed in reverse construction order, then
   ~dActor_c -- and D0 additionally reaches Memory::Deallocate through
   fBase_c's operator delete.  Nothing belongs in the braces. */
daGmch_c::~daGmch_c()
{
}

// @symbol _ZN8daGmch_c13OnYoshiTryEatEv
/* Vtable slot 18.  The ROM body ignores `this` and returns a constant. */
int daGmch_c::OnYoshiTryEat()
{
    return 6;
}

// @symbol _ZN8daGmch_c16OnAimedAtWithEggEv
/* Vtable slot 29.  The ROM body ignores `this` and returns a constant. */
int daGmch_c::OnAimedAtWithEgg()
{
    return 235520;
}

// @symbol _ZN8daGmch_c15ChooseNextStateEv
void daGmch_c::ChooseNextState()
{
    extern int data_0209e650;

    Fix12i distC;
    Fix12i dist;
    int curState;

    distC = DistToCPlayer();
    dist = Vec3_Dist(&mPosX, &mSpawnPosX);
    if (dist > 0x5dc000) {
        mNextState = 5;
    } else if (distC > 0x3e8000) {
        mNextState = 0;
    } else if (distC < 0x1f4000) {
        mNextState = 3;
    } else {
        if (RandomIntInternal(&data_0209e650) % 10 < 7)
            mNextState = 3;
        else
            mNextState = 1;

        if (mStateIndex == 1 && mNextState == 1) {
            ChooseNextState();
            return;
        }
    }

    curState = mStateIndex;
    if (curState == 1 && mNextState != 1) {
        SetState(2);
        return;
    }
    if (curState == 3 && mNextState != 3) {
        SetState(4);
        return;
    }
    SetState(mNextState);
}

// @symbol _ZN8daGmch_c16SpawnCoinsAndDieEv
void daGmch_c::SpawnCoinsAndDie()
{
    Vector3 coinPos;
    coinPos.x = mPosX;
    coinPos.y = mPosY;
    coinPos.z = mPosZ;
    SpawnCoins(coinPos, 5, 0xf000, 0);
    PoofDust();
    KillAndTrackInDeathTable();
}

// @symbol _ZN8daGmch_c18CheckPlayerContactEv
/* The actor found through mdCcAc_c.otherOwner is only acted on when its
   actorID is ACTOR_PLAYER, so it is typed as one.  Bounce and Hurt stay
   bridges: Player.h does not declare them yet. */
void daGmch_c::CheckPlayerContact()
{
    Player *player;
    int cmp;

    if (FindEgg(mdCcAc_c) != 0) {
        Sound::PlayBank0(9, mCamSpacePos);
        SpawnCoinsAndDie();
        return;
    }

    {
        unsigned int otherID = mdCcAc_c.otherOwner;
        if (otherID == 0)
            return;
        player = (Player *)dActor_c::FindWithID(otherID);
    }
    if (player == 0)
        return;

    cmp = (int)(player->actorID == ACTOR_PLAYER);
    if (cmp == 0)
        return;

    cmp = (int)((mFlags & MFLAG_YOSHI_MOUTH_A) != 0);
    if (cmp != 0) {
        SetState(6);
        return;
    }

    if ((mdCcAc_c.hitFlags & 0x66fe0)
        || player->IsOnShell() != 0
        || player->mIsMetal != 0) {
        Sound::PlayBank0(9, mCamSpacePos);
        SpawnCoinsAndDie();
        return;
    }

    if (mdCcAc_c.hitFlags & HIT_MEGA_CHARACTER) {
        mPrevAngleY = Vec3_HorzAngle(&player->mPosX, &mPosX);
        mAngleY = (short)(mPrevAngleY + 0x8000);
        player->IncMegaKillCount();
        SetState(8);
        return;
    }

    if (JumpedOnByPlayer(mdCcAc_c, *player) != 0) {
        if (mStateIndex == 0)
            return;
        player->Bounce(0x28000);
        SpawnCoinsAndDie();
        return;
    }

    if (mStateIndex == 7)
        return;

    {
        Vector3 hurtPos;
        hurtPos.x = mPosX;
        hurtPos.y = mPosY;
        hurtPos.z = mPosZ;
        player->Hurt(hurtPos, 2, 0xc000, 1, 0, 1);
    }
}

// @symbol _ZN8daGmch_c21ApplySlopeToVertSpeedEPv
/* Adjust vertical speed from the floor normal; callers need no result. */
void daGmch_c::ApplySlopeToVertSpeed(void *clsn)
{
    Vector3 floorNormal;
    Vector3 wallNormal;
    func_02038414(clsn);
    if (((dBgCh_Actr *)clsn)->IsOnGround()) {
        ((dBgCh_Actr *)clsn)->GetFloorResult()->surface.CopyNormalTo(floorNormal);
        if (floorNormal.y != 0) {
            /* mSpeedX / mSpeedZ: world velocity X/Z next to mVertSpeed. */
            s32 velX = mSpeedX;
            s32 velZ = mSpeedZ;
            int termX = FX_MUL(floorNormal.x, velX);
            int termZ = FX_MUL(floorNormal.z, velZ);
            int dot = termX + termZ;
            int slope = cstd::fdiv(dot, floorNormal.y);
            mVertSpeed = -(slope + 0x8000);
        }
    }
    if (((dBgCh_Actr *)clsn)->IsOnWall()) {
        ((dBgCh_Actr *)clsn)->GetWallResult()->surface.CopyNormalTo(wallNormal);
    }
}

// @symbol _ZN8daGmch_c18UpdateDrawMatricesEv
/* Both model matrices are edited through the shared scratch matrix
   data_020a0e68: copied out whole, transformed in place, copied back.  The
   slot-29 calls are this class's own OnAimedAtWithEgg, dispatched virtually
   exactly as the ROM does. */
void daGmch_c::UpdateDrawMatrices()
{
    extern Matrix4x3 data_020a0e68;
    extern Vector3 data_ov081_02128ef8;

    int inMouth = (int)((mFlags & MFLAG_YOSHI_MOUTH_B) != 0);
    if (inMouth)
        return;

    Matrix4x3_FromRotationY(&mAnmModel.mat4x3, mAngleY);
    mAnmModel.mat4x3.m[9] = mPosX >> 3;
    mAnmModel.mat4x3.m[10] = mPosY >> 3;
    mAnmModel.mat4x3.m[11] = mPosZ >> 3;

    mAnmModel.ApplyOpacity(mState, 0);

    if (mAngleX != 0) {
        data_020a0e68 = mAnmModel.mat4x3;
        int y1 = OnAimedAtWithEgg() >> 3;
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, y1, 0);
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, mAngleX);
        int y2 = (-OnAimedAtWithEgg()) >> 3;
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, y2, 0);
        mAnmModel.mat4x3 = data_020a0e68;
    } else {
        data_020a0e68 = mAnmModel.mat4x3;
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, data_ov081_02128ef8.x, data_ov081_02128ef8.y, data_ov081_02128ef8.z);
        Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mSpinAngleY);
        mModel.mat4x3 = data_020a0e68;
    }

    mModel.ApplyOpacity((0x20 - mState) & 0xff, 0);

    mMatrix.m[9] = mPosX >> 3;
    mMatrix.m[10] = mPosY >> 3;
    mMatrix.m[11] = mPosZ >> 3;

    int dh = (mStateIndex == 8) ? 0x258000 : 0x12c000;
    DropShadowRadHeight(mShadowModel, mMatrix, 0x78000, dh, 0xf);
}

// @symbol _ZN8daGmch_c12UpdateState8Ev
int daGmch_c::UpdateState8()
{
    mAngleX = mAngleX - 0x1000;
    mAnmModel.Advance();
    UpdatePos(&mdCcAc_c);
    func_02038414(&mWithMeshClsn);
    if (mWithMeshClsn.JustHitGround() != 0 || DecIfAbove0_Byte(&mTimer) == 0) {
        SpawnCoinsAndDie();
    }
    return 1;
}

// @symbol _ZN8daGmch_c11EnterState8Ev
int daGmch_c::EnterState8()
{
    extern GmchAnimationFileHandle data_ov081_02128edc;

    Sound::PlayBank0(9, mCamSpacePos);
    mFlags &= ~1;
    mHorzSpeed = 0xa000;
    mVertSpeed = 0x28000;
    mTimer = 0x2d;
    mAnmModel.SetAnim((BCA_File *)data_ov081_02128edc.filePtr, 0, 0x1000, 0);
    mAnmModel.speed = 0x4000;
    int yOffset1 = OnAimedAtWithEgg();
    Particle::System::NewSimple(0x43, mPosX, mPosY + yOffset1, mPosZ);
    int yOffset2 = OnAimedAtWithEgg();
    Particle::System::NewSimple(0x44, mPosX, mPosY + yOffset2, mPosZ);
    mStateIndex = 8;
    return 1;
}

// @symbol _ZN8daGmch_c12UpdateState7Ev
/* The angle reset is on the IsOnGround path, not the JustHitGround one. */
int daGmch_c::UpdateState7()
{
    func_02038414(&mWithMeshClsn);
    mAngleX = mAngleX + 0x1000;
    if (mWithMeshClsn.JustHitGround() != 0) {
        mVertSpeed = mVertSpeed * -0x3c / 100;
    } else if (mWithMeshClsn.IsOnGround() != 0) {
        mVertSpeed = 0;
        mWithMeshClsn.ClearLimMovFlag();
        {
            short heading = mPrevAngleY;
            mAngleX = 0;
            mAngleY = heading;
            mAngleZ = 0;
            ChooseNextState();
        }
    }
    UpdatePos(&mdCcAc_c);
    CheckPlayerContact();
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}

// @symbol _ZN8daGmch_c11EnterState7Ev
/* mHolder is re-read before each use, as the ROM reloads it: the stores in
   between go to this object's own fields, but the shards were written with the
   reload and the bytes keep it. */
int daGmch_c::EnterState7()
{
    extern s16 data_02082214[];

    Vector3 rayStart;
    dActor_c *holder;
    s32 *holderPos;
    int zero;
    s16 holderAngle;
    int angleIdx;
    s16 trig;

    mFlags &= ~0x80000;
    zero = 0;
    holder = mHolder;
    mHorzSpeed = holder->mHorzSpeed + 0x7000;
    mVertSpeed = zero;
    holder = mHolder;
    holderAngle = holder->mAngleY;
    mAngleY = holderAngle;
    mPrevAngleY = mAngleY;

    /* Word triple through &mPosX. Naming the three fields is 0x178 vs 0x17c. */
    holder = mHolder;
    holderPos = &holder->mPosX;
    mPosX = holderPos[0];
    mPosY = holderPos[1];
    mPosZ = holderPos[2];

    angleIdx = ((u16)mAngleY >> 4);
    trig = data_02082214[angleIdx * 2];
    mPosX = mPosX + FX_MUL(trig, 0x50000);
    mPosY = mPosY + 0x50000;
    angleIdx = ((u16)mAngleY >> 4);
    trig = data_02082214[angleIdx * 2 + 1];
    mPosZ = mPosZ + FX_MUL(trig, 0x50000);

    holder = mHolder;
    {
        int y = holder->mPosY;
        int z = holder->mPosZ;
        int y2 = y + 0x14000;
        int x = holder->mPosX;
        rayStart.x = x;
        rayStart.y = y2;
        rayStart.z = z;
    }

    DetectRaycastClsn(rayStart, mPos, 1);
    mHolder = (dActor_c *)zero;
    mWithMeshClsn.SetLimMovFlag();
    mStateIndex = 7;
    return 1;
}

// @symbol _ZN8daGmch_c12UpdateState6Ev
int daGmch_c::UpdateState6()
{
    int flagSet;
    flagSet = (mFlags & MFLAG_YOSHI_MOUTH_B) != 0;
    if (flagSet) {
        /* Same word triple. Naming the three fields is 0x9c vs 0xa0. */
        s32 *sv = &mHolder->mPosX;
        mPosX = sv[0];
        mPosY = sv[1];
        mPosZ = sv[2];
    }
    flagSet = (mFlags & 0x80000) != 0;
    if (flagSet) {
        SetState(7);
    } else {
        flagSet = (mFlags & MFLAG_YOSHI_MOUTH_A) != 0;
        if (!flagSet) {
            flagSet = (mFlags & MFLAG_YOSHI_MOUTH_B) != 0;
            if (!flagSet) {
                ChooseNextState();
            }
        }
    }
    return 1;
}

// @symbol _ZN8daGmch_c11EnterState6Ev
int daGmch_c::EnterState6()
{
    mHorzSpeed = 0;
    mdCcAc_c.Clear();
    mStateIndex = 6;
    return 1;
}

// @symbol _ZN8daGmch_c12UpdateState5Ev
int daGmch_c::UpdateState5()
{
    Fix12i distToSpawn = Vec3_Dist(&mPosX, &mSpawnPosX);
    if (distToSpawn < mHorzSpeed) {
        mAngleY = Vec3_HorzAngle(&mPosX, &mSpawnPosX);
        mPrevAngleY = mAngleY;
        mHorzSpeed = distToSpawn;
    } else {
        ApproachLinear(mAngleY, Vec3_HorzAngle(&mPosX, &mSpawnPosX), 0x2bc);
        mPrevAngleY = mAngleY;
    }
    mAnmModel.Advance();
    UpdatePos(&mdCcAc_c);
    ApplySlopeToVertSpeed(&mWithMeshClsn);
    CheckPlayerContact();
    if (distToSpawn < 0xa000) {
        ChooseNextState();
    }
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}

// @symbol _ZN8daGmch_c11EnterState5Ev
int daGmch_c::EnterState5()
{
    extern GmchAnimationFileHandle data_ov081_02128edc;

    mHorzSpeed = 0xa000;
    mAnmModel.SetAnim((BCA_File *)data_ov081_02128edc.filePtr, 0, 0x1000, 0);
    mAnmModel.speed = 0x1000;
    mStateIndex = 5;
    return 1;
}

// @symbol _ZN8daGmch_c12UpdateState4Ev
int daGmch_c::UpdateState4()
{
    mAnmModel.Advance();
    if (mAnmModel.Finished()) {
        mFlags |= 1;
        SetState(mNextState);
    }
    CheckPlayerContact();
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}

// @symbol _ZN8daGmch_c11EnterState4Ev
int daGmch_c::EnterState4()
{
    extern GmchAnimationFileHandle gGamaguchiJumpEndAnimationFile;

    mAnmModel.SetAnim((BCA_File *)gGamaguchiJumpEndAnimationFile.filePtr, 0x40000000, 0x1000, 0);
    mAnmModel.speed = 0x1000;
    mHorzSpeed = 0;
    mStateIndex = 4;
    return 1;
}

// @symbol _ZN8daGmch_c12UpdateState3Ev
int daGmch_c::UpdateState3()
{
    switch (mPhase) {
    case 0:
        if ((((unsigned int)mAnmModel.currFrame) << 4) >> 16 == 7) {
            mHorzSpeed = 0x12000;
            mVertSpeed = 0x1e000;
            func_0201267c(0x77, &mCamSpacePos);
            mPhase++;
        }
        break;
    case 1:
        if (mWithMeshClsn.JustHitGround()) {
            func_0201267c(0x71, &mCamSpacePos);
            ChooseNextState();
        }
        break;
    }
    mAnmModel.Advance();
    UpdatePos(&mdCcAc_c);
    ApplySlopeToVertSpeed(&mWithMeshClsn);
    CheckPlayerContact();
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}

// @symbol _ZN8daGmch_c11EnterState3Ev
int daGmch_c::EnterState3()
{
    extern GmchAnimationFileHandle gGamaguchiJumpAnimationFile;
    extern int data_0209e650[];

    mFlags &= ~1;
    mAnmModel.SetAnim((BCA_File *)gGamaguchiJumpAnimationFile.filePtr, 0x40000000, 0x1000, 0);
    mAnmModel.currFrame = 0;
    mAngleY = (short)RandomIntInternal(data_0209e650);
    mPrevAngleY = mAngleY;
    mHorzSpeed = 0;
    mPhase = 0;
    mStateIndex = 3;
    return 1;
}

// @symbol _ZN8daGmch_c12UpdateState2Ev
int daGmch_c::UpdateState2()
{
    if (mAnmModel.Finished()) {
        SetState(mNextState);
    }
    mAnmModel.Advance();
    CheckPlayerContact();
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}

// @symbol _ZN8daGmch_c11EnterState2Ev
int daGmch_c::EnterState2()
{
    extern GmchAnimationFileHandle gGamaguchiWalkEndAnimationFile;

    mAnmModel.SetAnim((BCA_File *)gGamaguchiWalkEndAnimationFile.filePtr, 0x40000000, 0x1000, 0);
    mAnmModel.speed = 0x1000;
    mHorzSpeed = 0;
    mStateIndex = 2;
    return 1;
}

// @symbol _ZN8daGmch_c12UpdateState1Ev
int daGmch_c::UpdateState1()
{
    ApproachLinear(mAngleY, mTargetAngleY, 0x2bc);
    mPrevAngleY = mAngleY;
    UpdatePos(&mdCcAc_c);
    ApplySlopeToVertSpeed(&mWithMeshClsn);
    CheckPlayerContact();
    mAnmModel.Advance();
    if (DecIfAbove0_Byte(&mTimer) == 0) {
        ChooseNextState();
    }
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}

// @symbol _ZN8daGmch_c11EnterState1Ev
void daGmch_c::EnterState1()
{
    extern GmchAnimationFileHandle data_ov081_02128edc;
    extern int data_0209e650[];

    mAnmModel.SetAnim((BCA_File *)data_ov081_02128edc.filePtr, 0, 0x1000, 0);
    mAnmModel.speed = 0x1000;
    mHorzSpeed = 0xa000;
    mTargetAngleY = (short)RandomIntInternal(data_0209e650);
    mTimer = (unsigned char)((unsigned int)RandomIntInternal(data_0209e650) % 0x78);
    mStateIndex = 1;
}

// @symbol _ZN8daGmch_c12UpdateState0Ev
int daGmch_c::UpdateState0()
{
    switch (mPhase) {
    case 0:
        if (ApproachLinear2(*(int *)&mState, 1, 2) != 0) {
            mdCcAc_c.Init(this, 0x4b000, 0x73000, 0x200000, 0);
            mFlags &= ~0x10000000;
            mPhase++;
        }
        break;
    case 1:
        if (DistToCPlayer() < 0x1f4000) {
            mFlags |= 0x10000000;
            func_0201267c(0x76, &mCamSpacePos);
            mPhase++;
        }
        break;
    case 2:
        if (ApproachLinear2(*(int *)&mState, 0x1f, 2) != 0) {
            mdCcAc_c.Init(this, 0x4b000, 0x73000, 0x200000, 0x6eff0);
            SetState(3);
        }
        break;
    }

    /* += 0xc00 differs by 4 words at the same 0x158. ldrsh's offset is 8
       bits, so 0x3ec is split: add this,#0x300, then [rN,#0xec]. */
    {
        s16 spin = mSpinAngleY;
        mSpinAngleY = (s16)(spin + 0xc00);
    }
    CheckPlayerContact();
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}

// @symbol _ZN8daGmch_c11EnterState0Ev
int daGmch_c::EnterState0()
{
    extern GmchAnimationFileHandle gGamaguchiJumpEndAnimationFile;

    mAnmModel.SetAnim((BCA_File *)gGamaguchiJumpEndAnimationFile.filePtr, 0, 0x1000, 0);
    mAnmModel.speed = 0x1000;
    mAnmModel.currFrame = 0x6000;
    mPhase = 0;
    mStateIndex = 0;
    return 1;
}

// @symbol _ZN8daGmch_c15CallStateUpdateEv
/* Invokes PMF[1] -- the update half -- of the pair mStatePmfPair points at. */
void daGmch_c::CallStateUpdate()
{
    StateFn *p = ((StateFn *)mStatePmfPair) + 1;
    (this->*(*p))();
}

// @symbol _ZN8daGmch_c14CallStateEnterEv
/* Invokes PMF[0] -- the "enter" half -- of the current state's pair.  It is
   never BL'd: SetState takes its address from a literal pool and tail-calls
   it through `bx ip`. */
void daGmch_c::CallStateEnter()
{
    StateFn *p = (StateFn *)mStatePmfPair;
    (this->*(*p))();
}

// @symbol _ZN8daGmch_c8SetStateEi
/* The state setter: stores a pointer to state `a`'s 16-byte pair in the .bss
   mirror at 0x02128f40 and tail-calls the enter half. */
void daGmch_c::SetState(int n)
{
    extern StatePair data_ov081_02128f40[9];

    mStatePmfPair = (void *)((int)&data_ov081_02128f40 + (n << 4));
    CallStateEnter();
}

// @symbol _ZN8daGmch_c16CleanupResourcesEv
/* Vtable slot 3.  Releases one shared file, then a four-entry table; it never
   touches `this`. */
int daGmch_c::CleanupResources()
{
    extern GmchModelFileHandle data_ov081_02128ed4;
    extern void *data_ov081_021280d8[];

    ((SharedFilePtr *)&data_ov081_02128ed4)->Release();
    for (int i = 0; i < 4; i++) {
        ((SharedFilePtr *)data_ov081_021280d8[i])->Release();
    }
    return 1;
}

// @symbol _ZN8daGmch_c16OnPendingDestroyEv
/* Vtable slot 12.  The ROM body is one `bx lr`: the override exists only to
   occupy the slot. */
void daGmch_c::OnPendingDestroy()
{
}

// @symbol _ZN8daGmch_c6RenderEv
/* Vtable slot 9.  Draws the dExtAnmModel_c only above mState 1 and the dExtSimpleModel_c only at
   or below 0x1f, so the two overlap for 2..0x1f.  Both draws go through the
   models' own slot-5 Render, virtually, as the ROM dispatches them. */
int daGmch_c::Render()
{
    bool inMouth = mFlags & MFLAG_YOSHI_MOUTH_B;
    if (inMouth != 0)
        return 1;
    if (mState > 1) {
        mAnmModel.Render(0);
    }
    if (mState <= 0x1f) {
        mModel.Render(0);
    }
    return 1;
}

// @symbol _ZN8daGmch_c8BehaviorEv
/* Vtable slot 6.  Runs the current state's update half, then the shared
   per-frame work. */
int daGmch_c::Behavior()
{
    CallStateUpdate();
    MakeVanishLuigiWork(mdCcAc_c);
    if (mWithMeshClsn.GetResultFlag1() != 0) {
        if (mWithMeshClsn.TouchesWater() != 0) {
            SpawnCoinsAndDie();
        }
    }
    UpdateDrawMatrices();
    return 1;
}

// @symbol _ZN8daGmch_c13InitResourcesEv
/* Vtable slot 0. */
int daGmch_c::InitResources()
{
    extern GmchModelFileHandle data_ov081_02128ed4;
    extern void *data_ov081_021280d8[];
    extern SharedFilePtr data_ov002_0210d9b8;
    extern Matrix4x3 IDENTITY_MATRIX4X3;

    Vector3 pos;
    void *modelFile = dExtSimpleModel_c::LoadFile(*(SharedFilePtr *)&data_ov081_02128ed4);
    mAnmModel.SetFile((BMD_File *)modelFile, 1, 1);
    if (mModel.SetFile((BMD_File *)data_ov002_0210d9b8.filePtr, 1, 1) == 0)
        return 0;
    for (int i = 0; i < 4; i++)
        dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)data_ov081_021280d8[i]);
    if (mShadowModel.InitCylinder() == 0)
        return 0;
    mdCcAc_c.Init(this, 0x4b000, 0x73000, 0x200000, 0x6eff0);
    mWithMeshClsn.Init(this, 0x4b000, 0x4b000, 0, 0);
    mWithMeshClsn.StartDetectingWater();
    SetState(0);
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    {
        int curY;
        pos.x = mPosX;
        curY = mPosY;
        pos.y = curY;
        pos.z = mPosZ;
        pos.y = curY + 0x14000;
    }
    dBgCh_Gnd ground;
    ground.SetObjAndPos(pos, 0);
    if (ground.DetectClsn())
        mPosY = ground.clsnY;
    else
        mPosY = pos.y;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    mSpawnPosX = mPosX;
    mSpawnPosY = mPosY;
    mSpawnPosZ = mPosZ;
    mState = 1;
    mMatrix = IDENTITY_MATRIX4X3;
    UpdateDrawMatrices();
    return 1;
}

// @symbol _ZN8daGmch_c13OnTurnIntoEggER6Player
/* Vtable slot 19.  Gives the player 5 coins -- as cap-collection coins if Yoshi
   is wearing the cap, otherwise as egg coins -- then kills this actor and
   tracks it in the death table. */
void daGmch_c::OnTurnIntoEgg(Player &player)
{
    if (player.IsCollectingCap())
        GivePlayerCoins(player, 5, 0);
    else
        player.RegisterEggCoinCount(5, 0, 0);
    KillAndTrackInDeathTable();
}

// @symbol daGmch_c_classInit
/* The GAMAGUCHI factory. `new` calls fBase_c::operator new (_ZN7fBase_cnwEm),
 * and daGmch_c has no user-declared constructor, so `new` is the base
 * constructor, the vtable store and the five member subobjects in field
 * order (mAnmModel, mModel, mShadowModel, mdCcAc_c, mWithMeshClsn).
 *
 * Reconstructed source-style name: SM64DS proves daGmch_c through RTTI,
 * allocation size, vtable identity and the GAMAGUCHI registry profile; later
 * EAD lineage supplies classInit. Exact original spelling is not preserved.
 * Historical alias: Moneybag_Spawn. */
extern "C" daGmch_c *daGmch_c_classInit(void)
{
    return new daGmch_c();
}

/* Construction order is source order: the five file handles first (model,
 * then the four animations), then the nine {enter, update} state pairs,
 * then the draw offset, whose destructor registers last. The
 * registration nodes and the PMF literals are compiler temporaries. */
GmchModelFileHandle data_ov081_02128ed4(ASSET_GAMAGUCHI_BMD);
GmchAnimationFileHandle data_ov081_02128edc(ASSET_GAMAGUCHI_WALK_BCA);
GmchAnimationFileHandle gGamaguchiWalkEndAnimationFile(ASSET_GAMAGUCHI_WALK_END_BCA);
GmchAnimationFileHandle gGamaguchiJumpAnimationFile(ASSET_GAMAGUCHI_JUMP_BCA);
GmchAnimationFileHandle gGamaguchiJumpEndAnimationFile(ASSET_GAMAGUCHI_JUMP_END_BCA);

StatePair data_ov081_02128f40[9] = {
    { &daGmch_c::EnterState0, &daGmch_c::UpdateState0 },
    { (StatePmf)&daGmch_c::EnterState1, &daGmch_c::UpdateState1 },
    { &daGmch_c::EnterState2, &daGmch_c::UpdateState2 },
    { &daGmch_c::EnterState3, &daGmch_c::UpdateState3 },
    { &daGmch_c::EnterState4, &daGmch_c::UpdateState4 },
    { &daGmch_c::EnterState5, &daGmch_c::UpdateState5 },
    { &daGmch_c::EnterState6, &daGmch_c::UpdateState6 },
    { &daGmch_c::EnterState7, &daGmch_c::UpdateState7 },
    { &daGmch_c::EnterState8, &daGmch_c::UpdateState8 },
};

extern Vector3 data_ov081_02128ef8;
GmchDrawReg gmchDrawReg(data_ov081_02128ef8);
Vector3 data_ov081_02128ef8;

/* Registry priorities, flags and clipping ranges use the shared actor profile contract. */
extern "C" SpawnInfo<daGmch_c> g_profile_GAMAGUCHI = {
    daGmch_c_classInit, 0x105, 0x77, 0x10000007, 0x3f000, 0x50000, 0x1000000, 0xa28000};
