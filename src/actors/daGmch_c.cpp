//cpp
/* The GAMAGUCHI hopping purse that pays out coins (ov081/daGmch_c),
 * 37 functions, enrolled and canonical. All 36 helpers are real class
 * methods; the factory is the one free function.
 *
 * Class name is ROM RTTI: "8daGmch_c" at 0x02128bc8, with its
 * __si_class_type_info at 0x02128bd4. The tree used to call it
 * Moneybag; that name is not in the cartridge.
 *
 * Source runs ROM-ascending under defer_codegen off. Do not reorder.
 * The ROM has D1 (0x02126504) below D0 (0x02126554) and no D2:
 * mwccarm emits D1, D0, D2 here and the D2 is deadstripped.
 * common.h comes before daGmch_c.h (flat Matrix4x3 block-move).
 * decl_common.h is deliberately NOT included (it contradicts this
 * TU's own helper spellings).
 *
 * Leftover: SetAnim / dCcAc Init / dBgCh Init / NewSimple /
 *   SpawnCoins / DropShadowRadHeight / Player::Bounce / Hurt keep
 *   computed spellings (Fix12<int> by value, wall 6az).
 * Leftover: GetFloorResult / GetWallResult / TouchesWater have no
 *   header member; the bridges stay TU-local.
 * Leftover: dActor_c has no Pos(). PlayBank0 takes the camera-space
 *   triple at mCamSpacePosX through a Vector3 pun
 *   (CheckPlayerContact, EnterState8).
 * Leftover: unk_0a4 and unk_0ac stay those names. They are
 *   dActor_c's world-velocity X/Z beside mVertSpeed (this TU
 *   multiplies them by the floor normal). The base header still
 *   spells them unk_.
 * Leftover: ModelCache wants a shared home with da1up_c's copy.
 */

#pragma defer_codegen off

#include "types.h"
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
#include "Model.h"
#include "Animation.h"
#include "SharedFilePtr.h"

/* Cached model handle: the loaded BMD file is the second word (da1up_c
 * reads the same home the same way). Wants a shared home with da1up_c's
 * ModelCache; kept file-local until then. */
struct ModelCache { int pad0; BMD_File *file; };

/* ---------------------------------------------------------------------------
 * Shadow types, one set per member that recovered one.  The tag suffix is the
 * member's ROM address.
 * ------------------------------------------------------------------------- */

/* EnterState7 -- a flat three-int vector, NOT types.h's
   Vector3: that one has a declared destructor and is not an aggregate. */
typedef struct Vec3_26e28 { int x, y, z; } Vec3_26e28;

/* CallStateUpdate and CallStateEnter, the two
   pointer-to-member invokers.  Two separate tags
   even though the windows are identical: mwccarm's pointer-to-member
   representation depends on whether the class was complete when the PMF type
   was formed, and merging two members' views of one object changes it. */
struct C_27708;
typedef void (C_27708::*PMF_27708)();
struct C_27708 { char pad[0x3dc]; PMF_27708 *pp; };

struct C_27744;
typedef void (C_27744::*PMF_27744)();
struct C_27744 { char pad[0x3dc]; PMF_27744 *pp; };

/* EnterState4 and EnterState2 -- the two-word BCA file-pointer records those members
   recovered as a struct rather than as an array.  Hoisted to file scope only
   because a block-scope tag cannot type an `extern` object once the member is a
   class method; the field expressions are untouched. */
struct Bca2 { int w[2]; };

/* ---------------------------------------------------------------------------
 * External function bridges used by class methods, with C linkage.
 * ---------------------------------------------------------------------------
 * A class member function may not sit in a linkage-specification region, so
 * once a member becomes `daGmch_c::Something` a declaration written in its body
 * gets C++ linkage and the reference mangles.  The 50 bridges used by these
 * methods are declared once here; the free factory keeps its seven declarations
 * at block scope.
 *
 * Of the original 54 declarations, exactly NINE had more than one type spelling
 * across the 36 shards: Vec3_Dist, dActor_c::DistToCPlayer, func_02038414,
 * ModelAnim::SetAnim, Vec3_HorzAngle, DecIfAbove0_Byte, Sound::PlayBank0,
 * ApproachLinear and RandomIntInternal.  Collapsing each onto ONE spelling was
 * measured, not assumed, and eight of the nine cost nothing:
 *
 *   TWO were never different types at all.  Fix12i is `typedef s32`, so
 *   Vec3_Dist and dActor_c::DistToCPlayer spelt `Fix12i` and spelt `int` are
 *   the same declaration written twice.
 *
 *   FIVE disagreed only about something no call site can see: a return value
 *   that every caller discards or compares against zero (func_02038414,
 *   ModelAnim::SetAnim, DecIfAbove0_Byte) or which pointer type spells the same
 *   address (Vec3_HorzAngle, ModelAnim::SetAnim, and ApproachLinear, which
 *   keeps a short * so the turn helpers pass &mAngleY).
 *
 *   ONE needed a call site adapted rather than a declaration chosen.
 *   Sound::PlayBank0's position is the camera-space triple, passed as
 *   *(Vector3 *)&mCamSpacePosX because dActor_c has no Pos().
 *   Same address, same register, and the member still matches.
 *
 * The NINTH is genuinely load-bearing: RandomIntInternal must return
 * `unsigned int`.  Measured -- declaring it `int` leaves 36/37 matching and
 * breaks ChooseNextState alone, which is the only caller that takes `% 10` of
 * the result and so the only one that pays for a signed remainder.
 *
 * Data is different -- mwccarm leaves a file-scope variable's name unmangled in
 * C++ -- so every `data_*` declaration stays at block scope in the member that
 * recovered it, and the members that disagree about a data object's TYPE
 * (data_ov081_02128ec4 is a two-word struct to EnterState4 and a pointer
 * array to EnterState0) keep both views.
 * ------------------------------------------------------------------------- */
extern "C" {
extern Fix12i Vec3_Dist(const void *a, const void *b);
extern s16    Vec3_HorzAngle(const void *a, const void *b);
extern unsigned int RandomIntInternal(int *seed);
extern void   _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(void *self, Vector3 const &pos, unsigned int n, int fix, short s);
extern void   _ZN6Player6BounceE5Fix12IiE(void *p, int fix);
extern void   _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *p, void *pos, unsigned int a, int fix, unsigned int b, unsigned int cc, unsigned int d);
extern int    func_02038414(void *clsn);
extern void  *_ZNK10dBgCh_Actr14GetFloorResultEv(void *clsn);
extern int    _ZN4cstd4fdivEii(int a, int b);
extern void  *_ZNK10dBgCh_Actr13GetWallResultEv(void *clsn);
extern void   Matrix4x3_FromRotationY(void *m, int angle);
extern void   Matrix4x3_ApplyInPlaceToTranslation(void *m, int x, int y, int z);
extern void   Matrix4x3_ApplyInPlaceToRotationX(void *m, s16 angX);
extern void   Matrix4x3_ApplyInPlaceToRotationY(void *m, s16 angY);
extern void   _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void *self, void *shadow, void *mtx, int rad, int height, u32 flags);
extern int    DecIfAbove0_Byte(void *p);
extern void   _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *bca, int a, int fix, unsigned int j);
extern void   _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int n, int a, int b, int c);
extern void   _Z14ApproachLinearRsss(short *p, short target, short step);
extern int    _Z15ApproachLinear2Riii(int *p, int target, int step);
extern void   func_0201267c(int id, void *pos);
extern void   _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *self, void *actor, int a, int b, unsigned int c, unsigned int d);
extern int    _ZNK10dBgCh_Actr12TouchesWaterEv(char *clsn);
extern void   _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *self, void *actor, int a, int b, void *v, int c);
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
    int v;

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

    v = mStateIndex;
    if (v == 1 && mNextState != 1) {
        SetState(2);
        return;
    }
    if (v == 3 && mNextState != 3) {
        SetState(4);
        return;
    }
    SetState(mNextState);
}

// @symbol _ZN8daGmch_c16SpawnCoinsAndDieEv
void daGmch_c::SpawnCoinsAndDie()
{
    Vector3 t;
    t.x = mPosX;
    t.y = mPosY;
    t.z = mPosZ;
    _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(this, t, 5, 0xf000, 0);
    PoofDust();
    KillAndTrackInDeathTable();
}

// @symbol _ZN8daGmch_c18CheckPlayerContactEv
/* The actor found through mdCcAc_c.otherOwner is only acted on when its
   actorID is 0xbf, the player's, so it is typed as one.  Bounce and Hurt stay
   bridges: Player.h does not declare them yet. */
void daGmch_c::CheckPlayerContact()
{
    Player *player;
    int b;

    if (FindEgg(mdCcAc_c) != 0) {
        Sound::PlayBank0(9, *(Vector3 *)&mCamSpacePosX);
        SpawnCoinsAndDie();
        return;
    }

    {
        unsigned int id = mdCcAc_c.otherOwner;
        if (id == 0)
            return;
        player = (Player *)dActor_c::FindWithID(id);
    }
    if (player == 0)
        return;

    b = (int)(player->actorID == 0xbf);
    if (b == 0)
        return;

    b = (int)((mFlags & 0x20000) != 0);
    if (b != 0) {
        SetState(6);
        return;
    }

    if ((mdCcAc_c.hitFlags & 0x66fe0)
        || player->IsOnShell() != 0
        || player->mIsMetal != 0) {
        Sound::PlayBank0(9, *(Vector3 *)&mCamSpacePosX);
        SpawnCoinsAndDie();
        return;
    }

    if (mdCcAc_c.hitFlags & 0x10) {
        mPrevAngleY = Vec3_HorzAngle(&player->mPosX, &mPosX);
        mAngleY = (short)(mPrevAngleY + 0x8000);
        player->IncMegaKillCount();
        SetState(8);
        return;
    }

    if (JumpedOnByPlayer(mdCcAc_c, *player) != 0) {
        if (mStateIndex == 0)
            return;
        _ZN6Player6BounceE5Fix12IiE(player, 0x28000);
        SpawnCoinsAndDie();
        return;
    }

    if (mStateIndex == 7)
        return;

    {
        int v[3];
        v[0] = mPosX;
        v[1] = mPosY;
        v[2] = mPosZ;
        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(player, v, 2, 0xc000, 1, 0, 1);
    }
}

// @symbol _ZN8daGmch_c21ApplySlopeToVertSpeedEPv
/* Declared `int` and it falls off the end: the ROM leaves r0 holding whatever
   the last call left there and every caller discards the result. */
int daGmch_c::ApplySlopeToVertSpeed(void *clsn)
{
    int n0[3];
    int n1[3];
    func_02038414(clsn);
    if (((dBgCh_Actr *)clsn)->IsOnGround()) {
        ((SurfaceInfo *)((char *)_ZNK10dBgCh_Actr14GetFloorResultEv(clsn) + 4))->CopyNormalTo(*(Vector3 *)n0);
        if (n0[1] != 0) {
            /* unk_0a4 / unk_0ac: world velocity X/Z next to mVertSpeed. */
            s32 velX = unk_0a4;
            s32 velZ = unk_0ac;
            long long a = (long long)n0[0] * (long long)velX;
            long long b = (long long)n0[2] * (long long)velZ;
            int x = (int)((a + 0x800) >> 12);
            int y = (int)((b + 0x800) >> 12);
            mVertSpeed = -(_ZN4cstd4fdivEii(x + y, n0[1]) + 0x8000);
        }
    }
    if (((dBgCh_Actr *)clsn)->IsOnWall()) {
        ((SurfaceInfo *)((char *)_ZNK10dBgCh_Actr13GetWallResultEv(clsn) + 4))->CopyNormalTo(*(Vector3 *)n1);
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

    int b = (int)((mFlags & 0x40000) != 0);
    if (b)
        return;

    Matrix4x3_FromRotationY(&mModelAnim.mat4x3, mAngleY);
    mModelAnim.mat4x3.m[9] = mPosX >> 3;
    mModelAnim.mat4x3.m[10] = mPosY >> 3;
    mModelAnim.mat4x3.m[11] = mPosZ >> 3;

    mModelAnim.ApplyOpacity(mState, 0);

    if (mAngleX != 0) {
        data_020a0e68 = mModelAnim.mat4x3;
        int y1 = OnAimedAtWithEgg() >> 3;
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, y1, 0);
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, mAngleX);
        int y2 = (-OnAimedAtWithEgg()) >> 3;
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, y2, 0);
        mModelAnim.mat4x3 = data_020a0e68;
    } else {
        data_020a0e68 = mModelAnim.mat4x3;
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, data_ov081_02128ef8.x, data_ov081_02128ef8.y, data_ov081_02128ef8.z);
        Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mSpinAngleY);
        mModel.mat4x3 = data_020a0e68;
    }

    mModel.ApplyOpacity((0x20 - mState) & 0xff, 0);

    mMatrix.m[9] = mPosX >> 3;
    mMatrix.m[10] = mPosY >> 3;
    mMatrix.m[11] = mPosZ >> 3;

    int dh = (mStateIndex == 8) ? 0x258000 : 0x12c000;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        this, &mShadowModel, &mMatrix, 0x78000, dh, 0xf);
}

// @symbol _ZN8daGmch_c12UpdateState8Ev
int daGmch_c::UpdateState8()
{
    mAngleX = mAngleX - 0x1000;
    mModelAnim.Advance();
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
    extern int data_ov081_02128edc[];

    Sound::PlayBank0(9, *(Vector3 *)&mCamSpacePosX);
    mFlags &= ~1;
    mHorzSpeed = 0xa000;
    mVertSpeed = 0x28000;
    mTimer = 0x2d;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, *(void **)((char *)data_ov081_02128edc + 4), 0, 0x1000, 0);
    mModelAnim.speed = 0x4000;
    int r1 = OnAimedAtWithEgg();
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x43, mPosX, mPosY + r1, mPosZ);
    int r2 = OnAimedAtWithEgg();
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x44, mPosX, mPosY + r2, mPosZ);
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
            short v94 = mPrevAngleY;
            mAngleX = 0;
            mAngleY = v94;
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

    u32 *pf;
    Vec3_26e28 v;
    dActor_c *holder;
    s32 *src;
    int zero;
    s16 ang;
    int idx;
    s16 s;

    pf = &mFlags;
    *pf = *pf & ~0x80000;
    zero = 0;
    holder = mHolder;
    mHorzSpeed = holder->mHorzSpeed + 0x7000;
    mVertSpeed = zero;
    holder = mHolder;
    ang = holder->mAngleY;
    mAngleY = ang;
    mPrevAngleY = mAngleY;

    holder = mHolder;
    src = &holder->mPosX;
    mPosX = src[0];
    mPosY = src[1];
    mPosZ = src[2];

    idx = ((u16)mAngleY >> 4);
    s = *(s16 *)((char *)data_02082214 + (idx << 2));
    mPosX = mPosX + (int)(((s64)s * 0x50000 + 0x800) >> 12);
    mPosY = mPosY + 0x50000;
    idx = ((u16)mAngleY >> 4);
    s = *(s16 *)((char *)data_02082214 + ((idx * 2 + 1) << 1));
    mPosZ = mPosZ + (int)(((s64)s * 0x50000 + 0x800) >> 12);

    holder = mHolder;
    {
        int y = holder->mPosY;
        int z = holder->mPosZ;
        int y2 = y + 0x14000;
        int x = holder->mPosX;
        ((int *)&v)[0] = x;
        ((int *)&v)[1] = y2;
        ((int *)&v)[2] = z;
    }

    DetectRaycastClsn(*(Vector3 *)&v, *(Vector3 *)&mPosX, 1);
    mHolder = (dActor_c *)zero;
    mWithMeshClsn.SetLimMovFlag();
    mStateIndex = 7;
    return 1;
}

// @symbol _ZN8daGmch_c12UpdateState6Ev
int daGmch_c::UpdateState6()
{
    int b;
    b = (mFlags & 0x40000) != 0;
    if (b) {
        s32 *sv = &mHolder->mPosX;
        mPosX = sv[0];
        mPosY = sv[1];
        mPosZ = sv[2];
    }
    b = (mFlags & 0x80000) != 0;
    if (b) {
        SetState(7);
    } else {
        b = (mFlags & 0x20000) != 0;
        if (!b) {
            b = (mFlags & 0x40000) != 0;
            if (!b) {
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
    Fix12i d = Vec3_Dist(&mPosX, &mSpawnPosX);
    if (d < mHorzSpeed) {
        mAngleY = Vec3_HorzAngle(&mPosX, &mSpawnPosX);
        mPrevAngleY = mAngleY;
        mHorzSpeed = d;
    } else {
        _Z14ApproachLinearRsss(&mAngleY, Vec3_HorzAngle(&mPosX, &mSpawnPosX), 0x2bc);
        mPrevAngleY = mAngleY;
    }
    mModelAnim.Advance();
    UpdatePos(&mdCcAc_c);
    ApplySlopeToVertSpeed(&mWithMeshClsn);
    CheckPlayerContact();
    if (d < 0xa000) {
        ChooseNextState();
    }
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}

// @symbol _ZN8daGmch_c11EnterState5Ev
int daGmch_c::EnterState5()
{
    extern int data_ov081_02128edc[];

    mHorzSpeed = 0xa000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, (void *)data_ov081_02128edc[1], 0, 0x1000, 0);
    mModelAnim.speed = 0x1000;
    mStateIndex = 5;
    return 1;
}

// @symbol _ZN8daGmch_c12UpdateState4Ev
int daGmch_c::UpdateState4()
{
    mModelAnim.Advance();
    if (mModelAnim.Finished()) {
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
    extern Bca2 data_ov081_02128ec4;

    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, (void *)data_ov081_02128ec4.w[1], 0x40000000, 0x1000, 0);
    mModelAnim.speed = 0x1000;
    mHorzSpeed = 0;
    mStateIndex = 4;
    return 1;
}

// @symbol _ZN8daGmch_c12UpdateState3Ev
int daGmch_c::UpdateState3()
{
    switch (mPhase) {
    case 0:
        if ((((unsigned int)mModelAnim.currFrame) << 4) >> 16 == 7) {
            mHorzSpeed = 0x12000;
            mVertSpeed = 0x1e000;
            func_0201267c(0x77, &mCamSpacePosX);
            mPhase++;
        }
        break;
    case 1:
        if (mWithMeshClsn.JustHitGround()) {
            func_0201267c(0x71, &mCamSpacePosX);
            ChooseNextState();
        }
        break;
    }
    mModelAnim.Advance();
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
    extern int data_ov081_02128ecc[];
    extern int data_0209e650[];

    u32 *base = &mFlags;
    *base = *base & ~1;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, (void *)data_ov081_02128ecc[1], 0x40000000, 0x1000, 0);
    mModelAnim.currFrame = 0;
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
    if (mModelAnim.Finished()) {
        SetState(mNextState);
    }
    mModelAnim.Advance();
    CheckPlayerContact();
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}

// @symbol _ZN8daGmch_c11EnterState2Ev
int daGmch_c::EnterState2()
{
    extern Bca2 data_ov081_02128ee4;

    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, (void *)data_ov081_02128ee4.w[1], 0x40000000, 0x1000, 0);
    mModelAnim.speed = 0x1000;
    mHorzSpeed = 0;
    mStateIndex = 2;
    return 1;
}

// @symbol _ZN8daGmch_c12UpdateState1Ev
int daGmch_c::UpdateState1()
{
    _Z14ApproachLinearRsss(&mAngleY, mTargetAngleY, 0x2bc);
    mPrevAngleY = mAngleY;
    UpdatePos(&mdCcAc_c);
    ApplySlopeToVertSpeed(&mWithMeshClsn);
    CheckPlayerContact();
    mModelAnim.Advance();
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
    extern int data_ov081_02128edc[];
    extern int data_0209e650[];

    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, (void *)data_ov081_02128edc[1], 0, 0x1000, 0);
    mModelAnim.speed = 0x1000;
    mHorzSpeed = 0xa000;
    mTargetAngleY = (short)RandomIntInternal(data_0209e650);
    mTimer = (unsigned char)((unsigned int)RandomIntInternal(data_0209e650) % 0x78);
    mStateIndex = 1;
}

// @symbol _ZN8daGmch_c12UpdateState0Ev
int daGmch_c::UpdateState0()
{
    u8 *pstate;
    u32 *pflg;

    switch (mPhase) {
    case 0:
        if (_Z15ApproachLinear2Riii((int *)&mState, 1, 2) != 0) {
            _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0x4b000, 0x73000, 0x200000, 0);
            pflg = &mFlags;
            *pflg = *pflg & ~0x10000000;
            pstate = &mPhase;
            *pstate = *pstate + 1;
        }
        break;
    case 1:
        if (DistToCPlayer() < 0x1f4000) {
            pflg = &mFlags;
            *pflg = *pflg | 0x10000000;
            func_0201267c(0x76, &mCamSpacePosX);
            pstate = &mPhase;
            *pstate = *pstate + 1;
        }
        break;
    case 2:
        if (_Z15ApproachLinear2Riii((int *)&mState, 0x1f, 2) != 0) {
            _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0x4b000, 0x73000, 0x200000, 0x6eff0);
            SetState(3);
        }
        break;
    }

    /* Written as an explicit read-modify-write, not `+= 0xc00`: `ldrsh`/`strh`
       carry only an 8-bit offset, so 0x3ec cannot be encoded and the address
       MUST be split; the ROM splits it `add rN,this,#0x300` + `[rN,#0xec]`.
       Under 2004/b56 the compound-assignment form is the one spelling that
       instead materialises the whole 0x3ec and loads through `[rN]`. */
    {
        s16 v = mSpinAngleY;
        mSpinAngleY = (s16)(v + 0xc00);
    }
    CheckPlayerContact();
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}

// @symbol _ZN8daGmch_c11EnterState0Ev
int daGmch_c::EnterState0()
{
    extern void *data_ov081_02128ec4[];

    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, data_ov081_02128ec4[1], 0, 0x1000, 0);
    mModelAnim.speed = 0x1000;
    mModelAnim.currFrame = 0x6000;
    mPhase = 0;
    mStateIndex = 0;
    return 1;
}

// @symbol _ZN8daGmch_c15CallStateUpdateEv
/* Invokes PMF[1] -- the "update" half -- of the current state's 16-byte pair,
   whose address this+0x3dc holds. */
void daGmch_c::CallStateUpdate()
{
    C_27708 *c = (C_27708 *)this;
    PMF_27708 *p = c->pp + 1;
    (c->**p)();
}

// @symbol _ZN8daGmch_c14CallStateEnterEv
/* Invokes PMF[0] -- the "enter" half -- of the current state's pair.  It is
   never BL'd: SetState takes its address from a literal pool and tail-calls
   it through `bx ip`. */
void daGmch_c::CallStateEnter()
{
    C_27744 *c = (C_27744 *)this;
    PMF_27744 *p = c->pp;
    (c->**p)();
}

// @symbol _ZN8daGmch_c8SetStateEi
/* The state setter: stores a pointer to state `a`'s 16-byte pair in the .bss
   mirror at 0x02128f40 and tail-calls the enter half. */
void daGmch_c::SetState(int a)
{
    extern char data_ov081_02128f40;

    mStatePmfPair = (void *)((int)&data_ov081_02128f40 + (a << 4));
    CallStateEnter();
}

// @symbol _ZN8daGmch_c16CleanupResourcesEv
/* Vtable slot 3.  Releases one shared file, then a four-entry table; it never
   touches `this`. */
int daGmch_c::CleanupResources()
{
    extern void *data_ov081_02128ed4;
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
/* Vtable slot 9.  Draws the ModelAnim only above mState 1 and the Model only at
   or below 0x1f, so the two overlap for 2..0x1f.  Both draws go through the
   models' own slot-5 Render, virtually, as the ROM dispatches them. */
int daGmch_c::Render()
{
    bool b = mFlags & 0x40000;
    if (b != 0)
        return 1;
    if (mState > 1) {
        mModelAnim.Render(0);
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
        if (_ZNK10dBgCh_Actr12TouchesWaterEv((char *)&mWithMeshClsn) != 0) {
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
    extern void *data_ov081_02128ed4;
    extern void *data_ov081_021280d8[];
    extern ModelCache data_ov002_0210d9b8;
    extern Matrix4x3 IDENTITY_MATRIX4X3;

    Vector3 pos;
    void *m = Model::LoadFile(*(SharedFilePtr *)&data_ov081_02128ed4);
    mModelAnim.SetFile((BMD_File *)m, 1, 1);
    if (mModel.SetFile(data_ov002_0210d9b8.file, 1, 1) == 0)
        return 0;
    for (int i = 0; i < 4; i++)
        Animation::LoadFile(*(SharedFilePtr *)data_ov081_021280d8[i]);
    if (mShadowModel.InitCylinder() == 0)
        return 0;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0x4b000, 0x73000, 0x200000, 0x6eff0);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x4b000, 0x4b000, 0, 0);
    mWithMeshClsn.StartDetectingWater();
    SetState(0);
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    {
        int p60;
        pos.x = mPosX;
        p60 = mPosY;
        pos.y = p60;
        pos.z = mPosZ;
        pos.y = p60 + 0x14000;
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
extern "C" {
/* The GAMAGUCHI factory: allocates 0x3f4, runs dActor_c's constructor, installs
 * this class's vtable and constructs the five member objects.
 *
 * Reconstructed source-style name: SM64DS proves daGmch_c through RTTI,
 * allocation size, vtable identity and the GAMAGUCHI registry profile; later
 * EAD lineage supplies classInit.  Exact original spelling is not preserved.
 * Historical alias: Moneybag_Spawn. */
int *daGmch_c_classInit(void)
{
    extern void *_ZN7fBase_cnwEj(unsigned int size);
    extern void *_ZN8dActor_cC2Ev(void *p);
    extern void *_ZN9ModelAnimC1Ev(void *p);
    extern void *_ZN5ModelC1Ev(void *p);
    extern void *_ZN11ShadowModelC1Ev(void *p);
    extern void *_ZN7dCcAc_cC1Ev(void *p);
    extern void *_ZN10dBgCh_ActrC1Ev(void *p);
    /* _ZTV8daGmch_c is declared in daGmch_c.h -- see the note there. */

    int *p = (int *)_ZN7fBase_cnwEj(1012);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)&_ZTV8daGmch_c[2];
        _ZN9ModelAnimC1Ev((char *)p + 0xd4);
        _ZN5ModelC1Ev((char *)p + 0x138);
        _ZN11ShadowModelC1Ev((char *)p + 0x188);
        _ZN7dCcAc_cC1Ev((char *)p + 0x1b0);
        _ZN10dBgCh_ActrC1Ev((char *)p + 0x1e4);
    }
    return p;
}
}
