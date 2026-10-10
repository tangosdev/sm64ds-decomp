//cpp
/* Koopa Troopa, separable from its shell (ov062/daNknk_c). Both the
 * normal and the small registry profiles construct this same class.
 * Reactions pick a state into mDeathState; the state machine does the rest.
 *
 * Source REVERSE of ROM order (highest address first). Do not reorder.
 *
 * Leftover: Particle::System::New / SetSelfDestructFlag / FromUniqueID,
 *   ModelAnim::SetAnim, dCcAc_c::Init, dBgCh_Actr::Init,
 *   dActor_c::ReflectAngle / DropShadowRadHeight / IsTooFarAwayFromPlayer,
 *   dEnemyBase_c::KillByInvincibleChar, Player::Bounce / Hurt and
 *   Particle::RunningSlidingDustAt stay mangled. Each passes Fix12<int>
 *   by value (notes/mwccarm-codegen.md 6az), or the header declaration
 *   does not mangle to the ROM symbol. func_0201267c is the bank-3
 *   Sound::Play wrapper.
 *   GetWallResult / GetFloorResult are not methods on dBgCh_Actr.h.
 *   Particle bytes at +0x50 are Particle::System::callbackScale
 *   (func_ov062_021175c0 / func_ov062_02117724).
 * Leftover: some helpers below still read fields through raw offsets;
 *   tracked in #2871.
 */
// Inline definitions in Koopa.h, emitted here by the two factories:
#include "Koopa.h"
#include "common.h"
#include "types.h"
#include "dActor_c.h"
#include "dBgCh_Actr.h"
#include "dExtFrameCtrl_c.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
#include "decl_Model.h"
#include "decl_Enemy.h"
#include "SharedFilePtr.h"
#include "Player.h"
#include "decl_ActorBase.h"
#include "decl_ModelAnim.h"
#include "decl_dCcAc_c.h"
#include "decl_ShadowModel.h"
#include "Particle__System.h"

bool ApproachLinear(short &value, short target, short step);

/* Remaining legacy helper views are local to this translation unit. */
/* shadow struct 'Entry' */
struct Entry { char pad[4]; void *file; };

/* File-scope resource handles: the ctor/dtor are the ROM's SharedFilePtr
 * veneer pairs (func_02017acc / func_02017ab4 for the two models,
 * SharedFilePtr::Construct / SharedFilePtr_Destruct_Anim for the nine
 * animation handles), spelled through declared-only subclasses so the
 * static initializer names the real entry points. The code reaches them
 * through the data_ov062_0211ced8/cee0/cee8 pointer tables. */
struct NknkModelFilePtr : SharedFilePtr {
    u32 words[2];
    NknkModelFilePtr(u32 fileID);
    ~NknkModelFilePtr();
};
struct NknkAnimationFileHandle : SharedFilePtr {
    u32 words[2];
    NknkAnimationFileHandle(u32 fileID);
    ~NknkAnimationFileHandle();
};

/* shadow typedef 'Fix12i' */
typedef int Fix12i;

/* shadow typedef 'Vec3' */
typedef struct { s32 x, y, z; } Vec3;

struct V3 { int x, y, z; };

extern "C" {
extern int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(u32 uniqueID, u32 effectID, s32 x, s32 y, s32 z, const void *dir, void *callback);
extern void _ZN8Particle19SetSelfDestructFlagEj(u32 id);
extern void func_0201267c(unsigned int id, const Vector3 *pos);
extern s16 data_02082214[];
extern struct Entry *data_ov062_0211cee8[];
extern int data_ov062_0211cf0c[];
/* The called scalar definition takes u16 startFrame. The unused ModelAnim
 * member declaration takes Fix12<int>/u32; its measured form grew this helper. */
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(ModelAnim *anim, BCA_File *file, int flags, int speed, u16 startFrame);
extern int Vec3_Dist(void* a, void* b);
/* Reconstructed receiver/attacker/nullable collision interface; the original
 * source prototype is not uniquely recoverable from the register traffic. */
extern void func_ov002_020aea30(void* self, void* attacker, dBgCh_Actr* collision);
extern void _ZN6Player6BounceE5Fix12IiE(void* p, int f);
extern int _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(Player* p, void* v, u32 a, int f, u8 b, u8 c, u8 d);
extern "C" int _Z14ApproachLinearRiii(int *r, int target, int speed);
extern "C" void _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(int a, int b, int c);
extern int RandomIntInternal(int *seed);
extern int data_0209e650;
extern short Vec3_HorzAngle(const Vector3* a, const Vector3* b);
extern s16 _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s(void *self, int a, int b, s16 ang);
extern int _ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(void *self, int d);

extern SharedFilePtr* data_ov062_0211ced8[2];
extern SharedFilePtr* data_ov062_0211cee0[2];

extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void* self, dActor_c* a, int r, int h, unsigned int e, unsigned int g);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void* self, dActor_c* a, int r, int h, Vector3_16* p, int q);
extern void LoadBlueCoinModel(void* c);
extern void UnloadBlueCoinModel(void *c);

extern void Vec3_Asr(struct Vec3 *d, struct Vec3 *s, int sh);
extern void Matrix4x3_FromTranslation(struct Matrix4x3 *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationY(struct Matrix4x3 *m, short angY);
extern void _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(void *thiz, void *sm, struct Matrix4x3 *m, int radHeight, int a, u8 b);
extern struct Matrix4x3 data_020a0e68;
inline struct Matrix4x3 *inline_fn()
{
  return &data_020a0e68;
}

extern struct dBgCh_Actr *_ZNK10dBgCh_Actr14GetFloorResultEv(struct dBgCh_Actr *self);
extern int SurfaceInfo_TestFlag0x20(int *p);
}

// @symbol daNknk_c_classInit_NOKONOKO
extern "C" {
daNknk_c *daNknk_c_classInit_NOKONOKO()
{
    return new daNknk_c;
}
}

// @symbol daNknk_c_classInit_NOKONOKO_S
extern "C" {
daNknk_c *daNknk_c_classInit_NOKONOKO_S()
{
    return new daNknk_c;
}
}

// @symbol _ZN8daNknk_c13OnYoshiTryEatEv
s32 daNknk_c::OnYoshiTryEat() {
  if(mModelIndex==0) return 6;
  return 5;
}

// @symbol _ZN8daNknk_c13OnTurnIntoEggER6Player
/* daNknk_c::OnTurnIntoEgg -- vtable slot 19, verified against ov062 relocs.txt:
 * _ZTV8daNknk_c (0x0211dab4) + 0x4c -> 0x02119628, exactly this placeholder's
 * former address (former name func_ov062_02119628).
 * Matched byte-for-byte with mwccarm 2004/b56 (ov062).
 */
void daNknk_c::OnTurnIntoEgg(Player &player)
{
    if (unk_108 == 3) {
        if (OnYoshiTryEat() == 6 && !player.IsCollectingCap()) {
            player.RegisterEggCoinCount(0, 0, 1);
        } else {
            GivePlayerCoins(player, 1, 2);
        }
    }
    KillAndTrackInDeathTable();
}

// @symbol _ZN8daNknk_c16OnAimedAtWithEggEv
/* daNknk_c::OnAimedAtWithEgg - recovered from vtable slot identity */
s32 daNknk_c::OnAimedAtWithEgg() {
    unsigned short v = actorID;
    int r;
    if (v == 0xcb) r = 1; else r = 0;
    if (r != 0) r = 0x46000; else r = 0x25800;
    return r;
}

// @symbol _ZN8daNknk_c13InitResourcesEv
int daNknk_c::InitResources()
{
    int i;
    int r, h;
    int kind;
    BMD_File* f;
    unsigned int isSpecial;

    kind = param1 & 1;
    mModelIndex = kind;

    for (i = 0; i < 9; i++)
        dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)data_ov062_0211cee8[i]);

    f = (BMD_File*)Model::LoadFile(*data_ov062_0211ced8[mModelIndex]);
    if (mModelAnim.SetFile(f, 1, -1) == 0)
        return 0;

    if (mShadowModel.InitCylinder() == 0)
        return 0;

    isSpecial = actorID == 0xcc;
    if (isSpecial)
    {
        mKoopaVariant = 2;
        mAnimSpeed = 0x2000;
        mScaleX = 0x599;
        mScaleY = 0x599;
        mScaleZ = 0x599;
        r = 0x1e000;
        h = 0x32000;
    }
    else
    {
        mKoopaVariant = 0;
        mAnimSpeed = 0x1000;
        mScaleX = 0xa66;
        mScaleY = 0xa66;
        mScaleZ = 0xa66;
        r = 0x3c000;
        h = 0x64000;
        Model::LoadFile(*data_ov062_0211cee0[mModelIndex]);
    }

    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCc_c, this, r, h, 0x200000, 0xb6efe0);

    mStateTimer = 0;
    mWalkState = 0;
    mState = 1;
    func_ov062_02117994(0);

    mCliffState = 0;
    mInvincibleTimer = 0;
    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x32000, 0x32000, 0, 0);

    mWithMeshClsn.StartDetectingWater();

    unk_108 = 3;
    LoadBlueCoinModel(this);

    return 1;
}

// @symbol _ZN8daNknk_c8BehaviorEv
int daNknk_c::Behavior()
{
    int state;
    int kind;

    if (UpdateKillByInvincibleChar(mWithMeshClsn, mModelAnim, 3) != 0)
        return 1;

    MakeVanishLuigiWork(mdCc_c);

    if (UpdateYoshiEat(mWithMeshClsn) != 0) {
        int *pb0 = (int *)((char *)&mFlags);
        *pb0 = *pb0 & ~0x10000000;
        if (SpawnParticlesIfHitOtherObj(mdCc_c) != 0) {
            PoofDust();
            func_ov062_021179e4();
            KillAndTrackInDeathTable();
        }
        if (mEatenByYoshi != 0)
            func_ov062_02117570();
        func_ov062_02118334();
        mdCc_c.Clear();
        if (mEatenByYoshi != 0 && unk_104 == 0)
            mdCc_c.Update();
        if (mKoopaVariant == 1)
            mState = 4;
        else
            mState = 1;
        if (mWithMeshClsn.IsOnGround() != 0) {
            mSafePosX = mPosX;
            mSafePosY = mPosY;
            mSafePosZ = mPosZ;
        }
        return 1;
    }

    if (mDeathState == 0) {
        if (_ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(this, 0x5dc000) != 0)
            return 1;

        {
            int *pb0 = (int *)((char *)&mFlags);
            if (mLandingDustTimer != 0)
                *(u8 *)((char *)&mLandingDustTimer) -= 1;
            *pb0 = *pb0 | 0x10000000;
        }
        func_ov062_02118258(0x3e8000);

        if (mState != 0)
            mModelAnim.Advance();

        kind = mKoopaVariant;
        state = mState;
        switch (kind) {
        case 0:
        case 2:
            func_ov062_02117acc();
            break;
        case 1:
            func_ov062_02117a3c();
            break;
        }

        {
            int ang = mPrevAngleY;
            mAngleY = (s16)ang;
            {
                u16 *p100 = (u16 *)((char *)&mStateTimer);
                *p100 = (u16)(*p100 + 1);
            }
        }
        if (state != mState || kind != mKoopaVariant) {
            mStateTimer = 0;
            mWalkState = 0;
        }
        func_ov062_02117c98();
        UpdatePos(&mdCc_c);

        if (mDeathState == 0 && mState != 0) {
            if (IsGoingOffCliff(mWithMeshClsn, 0x32000, 0x3800, false, true, 0x32000) != 0) {
                mPosX = mSafePosX;
                mPosY = mSafePosY;
                mPosZ = mSafePosZ;
            } else {
                mSafePosX = mPosX;
                mSafePosY = mPosY;
                mSafePosZ = mPosZ;
            }
        }

        UpdateWMClsn(mWithMeshClsn, 0);
        func_ov062_02117570();
        mdCc_c.Clear();
        if (mDeathState == 0) {
            if (mInvincibleTimer == 0) {
                mdCc_c.Update();
            } else {
                *(u16 *)((char *)&mInvincibleTimer) -= 1;
            }
        }
    } else {
        UpdateDeath(mWithMeshClsn);
    }

    func_ov062_02118334();
    return 1;
}

// @symbol _ZN8daNknk_c6RenderEv
int daNknk_c::Render()
{
  volatile struct V3 saved;
  int b = (mFlags & 0x40000) != 0;
  if (b) return 1;
  if (mKoopaVariant == 1) {
    mModelAnim.ShowMaterial(0, 1);
    mModelAnim.HideMaterial(0, 2);
  } else {
    mModelAnim.HideMaterial(0, 1);
    mModelAnim.ShowMaterial(0, 2);
  }
  saved.x = mScaleX;
  saved.y = mScaleY;
  saved.z = mScaleZ;
  if (mDeathState == 1 && mKoopaVariant == 2) {
    mScaleX = (int)(((long long)*(volatile int*)((char*)&mScaleX) * 0x800 + 0x800) >> 12);
    mScaleY = (int)(((long long)*(volatile int*)((char*)&mScaleY) * 0x800 + 0x800) >> 12);
    mScaleZ = (int)(((long long)*(volatile int*)((char*)&mScaleZ) * 0x800 + 0x800) >> 12);
  }
  mModelAnim.Render((const Vector3*)&mScaleX);
  mScaleX = saved.x;
  mScaleY = saved.y;
  mScaleZ = saved.z;
  return 1;
}

// @symbol _ZN8daNknk_c16OnPendingDestroyEv
/* recovered: shared header, real C++ method
 *
 * fBase_c slot 12. Empty in the ROM: four bytes, `bx lr`.
 */
void daNknk_c::OnPendingDestroy()
{
}

// @symbol _ZN8daNknk_c16CleanupResourcesEv
extern struct SharedFilePtr *data_ov062_0211cee0[];
extern struct SharedFilePtr *data_ov062_0211ced8[];

int daNknk_c::CleanupResources()
{
  int b = actorID == 0xcc;
  if (b == 0)
  {
    data_ov062_0211cee0[mModelIndex]->Release();
  }
  data_ov062_0211ced8[mModelIndex]->Release();
  for (int i = 0; i < 9; ++i)
  {
    ((SharedFilePtr *)data_ov062_0211cee8[i])->Release();
  }
  UnloadBlueCoinModel(this);
  return 1;
}

// @symbol _ZN8daNknk_c19func_ov062_02118de8Ev
void daNknk_c::func_ov062_02118de8()
{
    mHorzSpeed = 0;
    if (mAnimIndex == 2) {
        if (mModelAnim.Finished() == 0) return;
        func_ov062_02117994(4);
        return;
    }
    if (mModelAnim.WillHitFrame((unsigned short)(mModelAnim.GetFrameCount() - 1)) != 0) {
        u16 *hp = &mWalkState;
        *hp = (u16)(*hp + 1);
    } else {
        func_ov062_02118058();
        return;
    }
    func_ov062_02117994(0);
    if (mKoopaVariant == 1)
        mState = 5;
    else
        mState = 2;
    if (((unsigned int)RandomIntInternal(&data_0209e650) >> 16) & 0x8000) {
        int r = ((unsigned int)RandomIntInternal(&data_0209e650) >> 16) & 0x1fff;
        mTargetAngle = (s16)(mPrevAngleY - r);
    } else {
        int r = ((unsigned int)RandomIntInternal(&data_0209e650) >> 16) & 0x1fff;
        mTargetAngle = (s16)(mPrevAngleY + r);
    }
    func_ov062_02118058();
}

// @symbol _ZN8daNknk_c19func_ov062_02118cdcEv
void daNknk_c::func_ov062_02118cdc()
{
    if (mTurning) {
        mTurning = (ApproachLinear(mPrevAngleY, mTargetAngle, 0x200) ^ 1) != 0;
    } else {
        if (mPlayerDist >= 0x61a8000) {
            mTargetAngle = mAimAngle;
        }
        mTurning = AngleAwayFromWallOrCliff(mWithMeshClsn, mTargetAngle);
        ApproachLinear(mPrevAngleY, mTargetAngle, 0x200);
    }
    if (mAnimIndex == 1) {
        func_ov062_02117724(2, 8, 0x13, 0x19);
    }
    switch (mWalkState) {
    case 0: func_ov062_021181a0(); break;
    case 1: func_ov062_0211811c(); break;
    case 2: func_ov062_021180d4(); break;
    }
    func_ov062_02118058();
}

// @symbol _ZN8daNknk_c19func_ov062_02118b4cEv
void daNknk_c::func_ov062_02118b4c() {
    if (mAnimIndex == 3)
        func_ov062_02117724(2, 5, 8, 0xb);

    if (mModelIndex != 0) {
        if ((u16)mStateTimer > 0x1e && func_ov062_02117b60() > 0x320000) {
            if (_Z14ApproachLinearRiii(&mHorzSpeed, 0, 0x1000) == 0)
                return;
            mState = 1;
            func_ov062_02117994(2);
            return;
        }
        ApproachLinear(mPrevAngleY, mAimAngle, 0x400);
        _Z14ApproachLinearRiii(&mHorzSpeed, 0x11000, 0x1000);
        return;
    }

    if (mPlayerDist >= 0x61a8000) {
        s16 *aim = &mAimAngle;
        *aim = (s16)(*aim + 0x8000);
        mPlayerDist = 0;
    }
    if ((u16)mStateTimer > 0x1e && mPlayerDist > 0x320000) {
        if (_Z14ApproachLinearRiii(&mHorzSpeed, 0, 0x1000) == 0)
            return;
        mState = 1;
        func_ov062_02117994(2);
        return;
    }
    ApproachLinear(mPrevAngleY, (s16)(mAimAngle + 0x8000), 0x400);
    _Z14ApproachLinearRiii(&mHorzSpeed, 0x11000, 0x1000);
}

// @symbol _ZN8daNknk_c19func_ov062_02118a50Ev
/* ldrh cannot encode 0x3c6, so the state helpers test mTimer and mTurnTimer
 * from mModelAnim (byte offsets 0xc6 and 0xc8) and decrement through &mTimer /
 * &mTurnTimer. A single member expression folds that reload. */
void daNknk_c::func_ov062_02118a50()
{
    if (mHorzSpeed != 0) {
        if (mWithMeshClsn.IsOnWall() != 0) {
            void *sr = _ZNK10dBgCh_Actr13GetWallResultEv(&mWithMeshClsn);
            ((SurfaceInfo*)((char*)sr + 4))->CopyNormalTo(*(Vector3*)&mWallNormalX);
            mPrevAngleY = _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s(
                this, mWallNormalX, mWallNormalZ, mPrevAngleY);
        }
        func_ov062_02118004(0x4cc);
        return;
    }

    {
        if (*(u16 *)((char *)&mModelAnim + 0xc6) != 0) {
            u16 *p = &mTimer;
            *p = (u16)(*p - 1);
            if (*(u16 *)((char *)&mModelAnim + 0xc6) != 0) return;
            func_ov062_02117994(8); return;
        }
    }

    if (mModelAnim.WillHitFrame(0x1e) != 0)
        func_ov062_021175c0();
    if (mModelAnim.Finished() == 0)
        return;
    mState = 1;
    func_ov062_02117994(2);
}

// @symbol _ZN8daNknk_c19func_ov062_02118a00Ev
void daNknk_c::func_ov062_02118a00() {
    int gr = mWithMeshClsn.IsOnGround();
    if (gr == 0) return;
    int v = mKoopaVariant;
    if (v == 1) {
        mState = 5;
    } else {
        mState = 2;
    }
    func_ov062_02117994(0);
    func_ov062_021175c0();
}

// @symbol _ZN8daNknk_c19func_ov062_02118718Ev
/* See the ldrh note at func_ov062_02118a50 for the mModelAnim + 0xc6/0xc8 reads. */
void daNknk_c::func_ov062_02118718()
{
    int dist = 0x7fffffff;
    char *other;

    if (mAnimIndex == 3)
        func_ov062_02117724(2, 5, 8, 0xb);

    if (mTurning) {
        mTurning =
            (ApproachLinear(mPrevAngleY, mTargetAngle, 0x200) ^ 1) != 0;
    } else {
        if (mPlayerDist >= 0x61a8000)
            mTargetAngle = mAimAngle;

        other = (char *)func_ov062_02117b9c();
        if (other) {
            dist = Vec3_Dist(&mPosX, &((dActor_c *)other)->mPosX);
            mTargetAngle = Vec3_HorzAngle((const Vector3 *)&mPosX, (const Vector3 *)&((dActor_c *)other)->mPosX);
        } else {
            mTurning =
                AngleAwayFromWallOrCliff(mWithMeshClsn, mTargetAngle);
            if (!mTurning) {
                if (*(u16 *)((char *)&mModelAnim + 0xc8) != 0) {
                    u16 *turn = &mTurnTimer;
                    *turn = (u16)(*turn - 1);
                } else {
                    /* (int)& forces the pool add. mTargetAngle -= folds to mModelAnim+0xc2. */
                    if (((unsigned)RandomIntInternal(&data_0209e650) >> 16) & 0x8000)
                        *(s16 *)((int)&mTargetAngle) -= ((unsigned)RandomIntInternal(&data_0209e650) >> 16) & 0x1fff;
                    else
                        *(s16 *)((int)&mTargetAngle) += ((unsigned)RandomIntInternal(&data_0209e650) >> 16) & 0x1fff;
                    mTurnTimer = 0x14;
                }
            }
        }

        if (mPlayerDist > 0x320000 ||
            (other != 0 &&
             GetSubtraction(mTargetAngle,
                 (short)(mPrevAngleY + 0x8000)) < 0x2000)) {
            ApproachLinear(mPrevAngleY, mTargetAngle, 0x600);
        } else {
            if (mModelIndex != 0)
                ApproachLinear(mPrevAngleY, mAimAngle, 0x600);
            else
                ApproachLinear(mPrevAngleY, (short)(mAimAngle + 0x8000), 0x600);
        }
    }

    if (_Z14ApproachLinearRiii(&mHorzSpeed, 0x14000, 0x1000) == 0)
        return;
    if (dist >= 0xc8000)
        return;
    if (GetSubtraction(mTargetAngle, mPrevAngleY) >= 0xc00)
        return;

    mPrevAngleY = mTargetAngle;
    mState = 2;
    mHorzSpeed += mHorzSpeed / 5;
    mVertSpeed = dist / 30;
    func_ov062_02117994(6);
    mTimer = 0x14;
}

// @symbol _ZN8daNknk_c19func_ov062_02118588Ev
/* See the ldrh note at func_ov062_02118a50 for the mModelAnim + 0xc6/0xc8 reads. */
void daNknk_c::func_ov062_02118588()
{
    dActor_c *found = 0;
    int match = 0;
    if (mState == 2) {
        if (mdCc_c.otherOwner != 0) {
            found = dActor_c::FindWithID(mdCc_c.otherOwner);
            if (found != 0) {
                int t = found->actorID;
                t = (t == 0x11d);
                if (t != 0)
                    match = 1;
            }
        }
    }
    if (match != 0) {
        mKoopaVariant = 0;
        mState = 4;
        mHorzSpeed = mHorzSpeed / 2;
        found->MarkForDestruction();
        return;
    }
    if (mHorzSpeed != 0) {
        func_ov062_02118004(0x800);
        return;
    }

    {
        if (*(u16 *)((char *)&mModelAnim + 0xc6) != 0) {
            u16 *q = &mTimer;
            *q = (u16)(*q - 1);
            if (*(u16 *)((char *)&mModelAnim + 0xc6) != 0)
                return;
            func_ov062_02117994(8);
            return;
        }
    }

    if (mModelAnim.WillHitFrame(0x1e) != 0)
        func_ov062_021175c0();
    if (mModelAnim.Finished() == 0)
        return;
    if (mModelIndex != 0) {
        mState = 5;
        func_ov062_02117994(0);
        return;
    }
    mState = 1;
    func_ov062_02117994(3);
}

// @symbol _ZN8daNknk_c19func_ov062_021183e0Ev
void daNknk_c::func_ov062_021183e0()
{
    int dist = 0x7fffffff;
    char *other;

    if (mModelAnim.Finished())
        func_ov062_02117994(1);

    if (mTurning) {
        mTurning = (ApproachLinear(mPrevAngleY, mTargetAngle, 0x200) ^ 1) != 0;
    } else {
        if (mPlayerDist >= 0x61a8000)
            mTargetAngle = mAimAngle;
        other = (char *)func_ov062_02117b9c();
        if (other) {
            dist = Vec3_Dist(&mPosX, &((dActor_c *)other)->mPosX);
            if (dist < 0xc8000)
                mTargetAngle = Vec3_HorzAngle((const Vector3 *)&mPosX, (const Vector3 *)&((dActor_c *)other)->mPosX);
        }
        mTurning = AngleAwayFromWallOrCliff(mWithMeshClsn, mTargetAngle);
        ApproachLinear(mPrevAngleY, mTargetAngle, 0x200);
    }

    if (dist < 0xc8000 && GetSubtraction(mTargetAngle, mPrevAngleY) < 0xc00) {
        mPrevAngleY = mTargetAngle;
        mState = 2;
        mHorzSpeed = 0x18000;
        mVertSpeed = dist / 30;
        func_ov062_02117994(6);
        mTimer = 0x14;
        return;
    }

    if (mKoopaVariant == 2)
        _Z14ApproachLinearRiii(&mHorzSpeed, 0x2000, 0x4cc);
    else
        _Z14ApproachLinearRiii(&mHorzSpeed, 0x3000, 0x4cc);

    func_ov062_02118058();
}

// @symbol _ZN8daNknk_c19func_ov062_02118334Ev
void daNknk_c::func_ov062_02118334()
{
  short *new_var;
  struct Vec3 v;
  Vec3_Asr(&v, (struct Vec3 *)&mPosX, 3);
  if (1)
  {
    Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
    new_var = &mAngleY;
    Matrix4x3_ApplyInPlaceToRotationY(inline_fn(), *new_var);
    { struct M12w { int w[12]; };  /* array-wrapper copy: keeps C's block copy under -lang c++ */
      *(M12w *)&mModelAnim.mat4x3 = *(M12w *)&data_020a0e68; }
    {
      int b = (int) ((mFlags & 0x40000) != 0);
      if (b != 0)
      {
        return;
      }
    }
  }
  _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(this, &mShadowModel, &mModelAnim.mat4x3, 0x50000, 0x50000, 0xf);
}

// @symbol _ZN8daNknk_c19func_ov062_02118258Ei
void daNknk_c::func_ov062_02118258(int lim)
{
    mClosestPlayer = ClosestPlayer();

    if (mClosestPlayer == 0 || Vec3_Dist((Vector3*)&mPosX, (Vector3*)&mHomePosX) > lim) {
        mAimAngle = Vec3_HorzAngle((Vector3*)&mPosX, (Vector3*)&mHomePosX);
        mPlayerDist = 0x61a8000;
    } else {
        Vector3 v;
        int* src = &mClosestPlayer->mPosX;
        v.x = src[0];
        v.y = src[1];
        v.z = src[2];
        if (Vec3_Dist((Vector3*)&mHomePosX, &v) > lim) {
            mPlayerDist = 0x4e20000;
            return;
        }
        mPlayerDist = Vec3_Dist((Vector3*)&mPosX, &v);
        mAimAngle = Vec3_HorzAngle((Vector3*)&mPosX, &v);
    }
}

// @symbol _ZN8daNknk_c19func_ov062_021181a0Ev
void daNknk_c::func_ov062_021181a0() {
    if (mKoopaVariant == 2) {
        _Z14ApproachLinearRiii(&mHorzSpeed, 0x2000, 0x4cc);
    } else {
        _Z14ApproachLinearRiii(&mHorzSpeed, 0x3000, 0x4cc);
    }
    if (mModelAnim.Finished() == 0) return;
    {
        u16 *p = &mWalkState;
        *p = (u16)(*p + 1);
    }
    mTimer =
        (u16)(((unsigned int)RandomIntInternal(&data_0209e650) >> 0x10) % 70 + 30);
    func_ov062_02117994(1);
}

// @symbol _ZN8daNknk_c19func_ov062_0211811cEv
/* See the ldrh note at func_ov062_02118a50 for the mModelAnim + 0xc6/0xc8 reads. */
void daNknk_c::func_ov062_0211811c() {
    if (*(u16 *)((char *)&mModelAnim + 0xc6) != 0) {
        u16 *p = &mTimer;
        *p = (u16)(*p - 1);
        return;
    }
    if (mModelAnim.WillHitFrame((unsigned short)(mModelAnim.GetFrameCount() - 1)) == 0)
        return;
    {
        u16 *q = &mWalkState;
        *q = (u16)(*q + 1);
    }
    func_ov062_02117994(2);
}

// @symbol _ZN8daNknk_c19func_ov062_021180d4Ev
void daNknk_c::func_ov062_021180d4() {
    _Z14ApproachLinearRiii(&mHorzSpeed, 0, 0x1000);
    int done = mModelAnim.Finished();
    if (!done) return;
    mState = 1;
    func_ov062_02117994(4);
}

// @symbol _ZN8daNknk_c19func_ov062_02118058Ev
void daNknk_c::func_ov062_02118058() {
  Player *o = mClosestPlayer;
  int d = mPlayerDist;
  if(o!=0) d=Vec3_Dist(&mPosX, &o->mPosX);
  if(d>=0x12c000) return;
  int s=GetSubtraction(mAimAngle, mPrevAngleY);
  if(s>=0x3000) return;
  if(mKoopaVariant==1) mState=1;
  else mState=3;
  func_ov062_02117994(3);
}

// @symbol _ZN8daNknk_c19func_ov062_02118004Ei
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
void daNknk_c::func_ov062_02118004(int a1) {
    int r = mWithMeshClsn.IsOnGround();
    if (r == 0) return;
    _Z14ApproachLinearRiii(&mHorzSpeed, 0, a1);
    int x = mPosX;
    int y = mPosY;
    int z = mPosZ;
    _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(x, y, z);
}

// @symbol _ZN8daNknk_c19func_ov062_02117c98Ev
void daNknk_c::func_ov062_02117c98()
{
    void* found;
    int turnAround;
    s32 flags;
    u32 id;

    id = mdCc_c.otherOwner;
    if (id == 0)
        return;
    found = dActor_c::FindWithID(id);
    if (found == 0)
        return;

    flags = mdCc_c.hitFlags;
    turnAround = 0;

    if (flags & 0x10) {
        Vector3_16 v;
        v.x = (s16)-0x2000;
        v.y = (s16)turnAround;
        v.z = (s16)turnAround;
        // The member's unused Fix12<int> argument is one raw register word.
        // Native aggregate passing adds a load/copy here under 2004/b56;
        // retain decl_Enemy.h's scalar ABI bridge to the actual member symbol.
        _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(this, &v, found, 0x46000);
        return;
    }
    if (flags & 0x22400) {
        if (mKoopaVariant == 0) {
            func_ov062_02117bf4();
            return;
        }
        mDeathState = 5;
        func_ov002_020aea30(this, found, (dBgCh_Actr*)turnAround);
        return;
    }
    if (flags & 0x4000) {
        mDeathState = 6;
        turnAround = 1;
    } else if (flags & 0x447e0) {
        if (mKoopaVariant == 0) {
            func_ov062_02117bf4();
            if (flags & 0x3c0)
                mPrevAngleY = ((dActor_c *)found)->mAngleY;
        } else {
            if (flags & 0x40040)
                mDeathState = 2;
            else if (flags & 0x20400)
                mDeathState = 5;
            else if (flags & 0x380)
                mDeathState = 3;
            else if (flags & 0x4000)
                mDeathState = 6;
            else {
                mDeathState = 1;
                func_0201267c(0x113, (const Vector3*)&mCamSpacePosX);
                mScaleX = 0x1000;
                mScaleY = 0x1000;
                mScaleZ = 0x1000;
            }
            func_ov002_020aea30(this, found, 0);
            return;
        }
    } else {
        Player *player = (Player *)found;
        struct { Vec3 sv; Vec3 hv; } L;
        int isPlayer;
        isPlayer = (player->actorID == 0xbf) ? 1 : turnAround;
        if (isPlayer == 0)
            goto tail;
        if (player->mIsMetal != 0) {
            if (mKoopaVariant == 0) {
                func_ov062_02117bf4();
                goto tail;
            }
            mDeathState = 6;
            func_ov002_020aea30(this, found, 0);
            return;
        }
        {
            s32* s = &player->mPosX;
            L.sv.x = s[0];
            L.sv.y = s[1];
            L.sv.z = s[2];
        }
        if (player->IsOnShell()) {
            mDeathState = 5;
            turnAround = 1;
            goto tail;
        }
        if (JumpedOnByPlayer(mdCc_c, *player)) {
            if (mKoopaVariant == 0) {
                func_ov062_02117bf4();
            } else {
                mDeathState = 1;
                func_ov002_020aea30(this, found, 0);
                mScaleX = 0x1000;
                mScaleY = 0x1000;
                mScaleZ = 0x1000;
                func_0201267c(0x113, (const Vector3*)&mCamSpacePosX);
            }
            _ZN6Player6BounceE5Fix12IiE(found, 0x28000);
            return;
        }
        if (player->mIsVanish != 0)
            goto tail;
        if ((mdCc_c.hitFlags & 0x400000) == 0)
            goto tail;
        if (mState == 0)
            goto tail;
        {
            s32 pw;
            L.hv.x = mPosX;
            L.hv.y = mPosY;
            L.hv.z = mPosZ;
            pw = mHorzSpeed;
            if (pw < 0xf000)
                pw = 0xf000;
            _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(player, &L.hv, 0, pw, 1, 0, 1);
            func_ov062_02117b48();
            if (mKoopaVariant == 2) {
                PoofDust();
                func_ov062_021179e4();
                KillAndTrackInDeathTable();
            }
        }
    }

tail:
    func_ov002_020aea30(this, found, &mWithMeshClsn);
    if (turnAround)
        mAngleY = (u16)(mPrevAngleY + 0x8000);
}

// @symbol _ZN8daNknk_c19func_ov062_02117bf4Ev
void daNknk_c::func_ov062_02117bf4(){
    func_0201267c(0xed, (const Vector3*)&mCamSpacePosX);
    mKoopaVariant = 1;
    mState = 3;
    mHorzSpeed = 0x14000;
    func_ov062_02117994(7);
    {
        Vector3 v;
        int yy = mPosZ;
        int zz = mPosY + 0x3c000;
        v.x = mPosX;
        v.y = zz;
        v.z = yy;
        dActor_c::Spawn(0x11d, mModelIndex, v, 0, mAreaId, -1);
    }
    mInvincibleTimer = 0xa;
    func_0201267c(1, (const Vector3*)&mCamSpacePosX);
}

// @symbol _ZN8daNknk_c19func_ov062_02117b9cEv
void *daNknk_c::func_ov062_02117b9c() {
    dActor_c *found = 0;
    dActor_c *best = 0;
    Fix12i bestDist = 0x7FFFFFFF;
    while (1) {
        found = dActor_c::FindWithActorID(0x11d, found);
        if (!found) break;
        Fix12i d = Vec3_Dist((Vector3*)&mPosX, (Vector3*)&found->mPosX);
        if (d < bestDist) {
            bestDist = d;
            best = found;
        }
    }
    return best;
}

// @symbol _ZN8daNknk_c19func_ov062_02117b60Ev
int daNknk_c::func_ov062_02117b60()
{
    Player *target = mClosestPlayer;
    if (!target) return 0x61a8000;
    return Vec3_Dist(&mPosX, &target->mPosX);
}

// @symbol _ZN8daNknk_c19func_ov062_02117b48Ev
void daNknk_c::func_ov062_02117b48()
{
    mState = 0;
    mVertSpeed = 81920;
    mHorzSpeed = 0;
}

// @symbol _ZN8daNknk_c19func_ov062_02117accEv
void daNknk_c::func_ov062_02117acc(){
  switch(mState){
  case 0: func_ov062_02118a00(); break;
  case 1: func_ov062_02118de8(); break;
  case 2: func_ov062_02118cdc(); break;
  case 3: func_ov062_02118b4c(); break;
  case 4: func_ov062_02118a50(); break;
  }
}

// @symbol _ZN8daNknk_c19func_ov062_02117a3cEv
void daNknk_c::func_ov062_02117a3c()
{
    switch (mState) {
    case 0: func_ov062_02118a00(); break;
    case 1: func_ov062_02118718(); break;
    case 2: func_ov062_02118588(); break;
    case 3: func_ov062_02118588(); break;
    case 4: func_ov062_02118de8(); break;
    case 5: func_ov062_021183e0(); break;
    }
}

// @symbol _ZN8daNknk_c19func_ov062_021179e4Ev
void daNknk_c::func_ov062_021179e4() {
    int x = mPosX;
    int z = mPosZ;
    int y = mPosY + 0x78000;
    Vector3 pos;
    pos.x = x;
    pos.y = y;
    pos.z = z;
    dActor_c::Spawn(0x122, 2, pos, 0, mAreaId, -1);
}

// @symbol _ZN8daNknk_c19func_ov062_02117994Ei
void daNknk_c::func_ov062_02117994(int idx) {
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &mModelAnim,
        (BCA_File*)data_ov062_0211cee8[idx]->file,
        data_ov062_0211cf0c[idx],
        mAnimSpeed,
        0
    );
    mAnimIndex = (u8)idx;
}

// @symbol _ZN8daNknk_c19func_ov062_02117724Ejjjt
// 6f: keep constant live / flip coloring
void daNknk_c::func_ov062_02117724(unsigned int a1, unsigned int a2, unsigned int a3, unsigned short a4) /* uxth vs a1-a3: ROM compares h<=a4 after a halfword arg */
{
    unsigned int h = (unsigned int)(mModelAnim.currFrame << 4) >> 16;
    if ((h >= a1 && h <= a2) || (h >= a3 && h <= a4)) {
        Vector3 pos;
        int k, iscb, idx, uid, iscb2;
        short ang;
        char *ps;
        if (mFootstep != 0) return;
        mFootstep = 1;
        func_0201267c(0xe4, (const Vector3*)&mCamSpacePosX);
        if (mAnimIndex == 1) return;
        ang = (short)(mAngleY + 0x4000);
        pos.x = mPosX;
        pos.y = mPosY;
        pos.z = mPosZ;
        iscb = actorID == 0xcb;
        k = iscb ? 0xf : 0xa;
        *(volatile int *)&pos.y = pos.y + (k << 12); /* pin y add dest */
        if (h <= a2) {
            int i2, i1, v, w;
            idx = (unsigned short)ang >> 4;
            i2 = idx * 2;
            i1 = i2 + 1;
            i2 = i2 * 2;  /* byte offsets scaled in place: the ROM self-updates the register */
            i1 = i1 * 2;
            v = *(short *)((char *)data_02082214 + i2);
            v = pos.x - k * v;
            w = *(short *)((char *)data_02082214 + i1);
            w = pos.z - k * w;
            pos.x = v;
            pos.z = w;
        } else {
            int i2, i1, ev, ew;
            idx = (unsigned short)ang >> 4;
            i2 = idx * 2;
            i1 = i2 + 1;
            i2 = i2 * 2;  /* mirrors the taken branch: scaled in place, load interleaved with the mla */
            i1 = i1 * 2;
            pos.x = k * *(short *)((char *)data_02082214 + i2) + pos.x;
            pos.z = k * *(short *)((char *)data_02082214 + i1) + pos.z;
        }
        uid = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(0, 0xf9, pos.x, pos.y, pos.z, 0, 0);
        _ZN8Particle19SetSelfDestructFlagEj(0xf9);
        if (uid == 0) return;
        ps = (char *)Particle::System::FromUniqueID(uid);
        if (ps == 0) return;
        iscb2 = actorID == 0xcb;
        if (iscb2)
            *(int *)(ps + 0x50) = (short)(((long long)*(int *)(ps + 0x50) * 0x800 + 0x800) >> 12);
        else
            *(int *)(ps + 0x50) = (short)(((long long)*(int *)(ps + 0x50) * 0x500 + 0x800) >> 12);
    } else {
        mFootstep = 0;
    }
}

// @symbol _ZN8daNknk_c19func_ov062_021175c0Ev
void daNknk_c::func_ov062_021175c0()
{
    volatile Vector3 pos;
    int t;
    u32 spawned;
    void *particle;

    if (mLandingDustTimer != 0) return;

    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;

    t = actorID;
    t = t == 0xcb;
    {
        int zArg = pos.z;
        if (t != 0) {
            pos.y = pos.y + 0xf000;
        } else {
            pos.y = pos.y + 0xa000;
        }

        spawned = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            0, 0xb2, pos.x, pos.y, zArg, (void *)0, (void *)0);
    }

    _ZN8Particle19SetSelfDestructFlagEj(0xb2);

    mLandingDustTimer = 0xa;
    if (spawned == 0) return;

    particle = Particle::System::FromUniqueID(spawned);
    if (particle == 0) return;

    {
        int t2 = actorID;
        t2 = t2 == 0xcb;
        if (t2 != 0) {
            int v = *(int *)((char *)particle + 0x50);
            int r = (int)((((s64)v) * 0x800 + 0x800) >> 12);
            *(int *)((char *)particle + 0x50) = (short)r;
        } else {
            int v = *(int *)((char *)particle + 0x50);
            int r = (int)((((s64)v) * 0x500 + 0x800) >> 12);
            *(int *)((char *)particle + 0x50) = (short)r;
        }
    }
}

// @symbol _ZN8daNknk_c19func_ov062_02117570Ev
void daNknk_c::func_ov062_02117570() {
    if (!mWithMeshClsn.IsOnGround()) return;
    struct dBgCh_Actr *floor = _ZNK10dBgCh_Actr14GetFloorResultEv(&mWithMeshClsn);
    if (!SurfaceInfo_TestFlag0x20((int*)((char*)floor + 4))) return;
    SpawnCoin();
    KillAndTrackInDeathTable();
}

/* Two model handles (param1 & 1 selects the variant through
 * data_ov062_0211ced8) and nine animation handles (the shared noko-noko
 * clip set indexed by data_ov062_0211cee8). */
NknkModelFilePtr data_ov062_0211df40(0x3ba);
NknkModelFilePtr data_ov062_0211df48(0x3c0);
NknkAnimationFileHandle data_ov062_0211df70(0x3c6);
NknkAnimationFileHandle data_ov062_0211df38(0x3c4);
NknkAnimationFileHandle data_ov062_0211df50(0x3c5);
NknkAnimationFileHandle data_ov062_0211df60(0x3c1);
NknkAnimationFileHandle data_ov062_0211df78(0x3c2);
NknkAnimationFileHandle data_ov062_0211df68(0x3c3);
NknkAnimationFileHandle data_ov062_0211df28(0x3bd);
NknkAnimationFileHandle data_ov062_0211df30(0x3bb);
NknkAnimationFileHandle data_ov062_0211df58(0x3bc);
