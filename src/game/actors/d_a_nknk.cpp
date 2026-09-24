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
#include "Animation.h"
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

/* Remaining legacy helper views are local to this translation unit. */
/* shadow struct 'Entry' */
struct Entry { char pad[4]; void *file; };

/* shadow typedef 'Fix12i' */
typedef int Fix12i;

/* shadow typedef 'Vec3' */
typedef struct { s32 x, y, z; } Vec3;

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
extern "C" void func_ov062_02118a00(char *c);
extern "C" void func_ov062_02118718(char *c);
extern "C" void func_ov062_02118588(char *c);
extern "C" void func_ov062_02118de8(char *c);
extern "C" void func_ov062_021183e0(char *c);
extern void func_ov062_02118cdc(void *c);
extern void func_ov062_02118b4c(void *c);
extern void func_ov062_02118a50(void *c);
extern int Vec3_Dist(void* a, void* b);

void func_ov062_02117994(char* c, int idx);

extern void func_ov062_02117bf4(void* self);
/* Reconstructed receiver/attacker/nullable collision interface; the original
 * source prototype is not uniquely recoverable from the register traffic. */
extern void func_ov002_020aea30(void* self, void* attacker, dBgCh_Actr* collision);
extern void _ZN6Player6BounceE5Fix12IiE(void* p, int f);
extern int _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(Player* p, void* v, u32 a, int f, u8 b, u8 c, u8 d);
extern void func_ov062_02117b48(void* p);
extern void func_ov062_021179e4(void* c);
extern "C" int _Z14ApproachLinearRiii(int *r, int target, int speed);
extern "C" void _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(int a, int b, int c);
extern int RandomIntInternal(int *seed);
extern int data_0209e650;
extern short Vec3_HorzAngle(const Vector3* a, const Vector3* b);
extern int _Z14ApproachLinearRsss(short *a, short b, short c);
extern void *func_ov062_02117b9c(void *c);
extern void func_ov062_02118058(char *c);
extern void func_ov062_02118004(void *c, int a1);
extern void func_ov062_021175c0(void *c);
extern void func_ov062_02117724(void *c, unsigned int a, unsigned int b, unsigned int d, unsigned short e);

extern s16 _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s(void *self, int a, int b, s16 ang);
extern int func_ov062_02117b60(void *c);
extern void func_ov062_021181a0(void *c);
extern void func_ov062_0211811c(void *c);
extern void func_ov062_021180d4(void *c);
struct V3 { int x, y, z; };

extern int _ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(void *self, int d);

extern SharedFilePtr* data_ov062_0211ced8[2];
extern SharedFilePtr* data_ov062_0211cee0[2];

extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void* self, dActor_c* a, int r, int h, unsigned int e, unsigned int g);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void* self, dActor_c* a, int r, int h, Vector3_16* p, int q);
extern void LoadBlueCoinModel(void* c);
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
// Community name: Koopa_OnTurnIntoEgg
/* daNknk_c::OnTurnIntoEgg -- vtable slot 19, verified against ov062 relocs.txt:
 * _ZTV5Koopa (0x0211dab4) + 0x4c -> 0x02119628, exactly this placeholder's
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
// Community name: Koopa_OnAimedAtWithEgg
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
        Animation::LoadFile(*(SharedFilePtr *)data_ov062_0211cee8[i]);

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
    func_ov062_02117994(((char*)this), 0);

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
    LoadBlueCoinModel(((char*)this));

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
            func_ov062_021179e4(((char *)this));
            KillAndTrackInDeathTable();
        }
        if (mEatenByYoshi != 0)
            func_ov062_02117570(((char *)this));
        func_ov062_02118334(((char *)this));
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
        func_ov062_02118258(((char *)this), 0x3e8000);

        if (mState != 0)
            mModelAnim.Advance();

        kind = mKoopaVariant;
        state = mState;
        switch (kind) {
        case 0:
        case 2:
            func_ov062_02117acc(((char *)this));
            break;
        case 1:
            func_ov062_02117a3c(((char *)this));
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
        func_ov062_02117c98(((char *)this));
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
        func_ov062_02117570(((char *)this));
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

    func_ov062_02118334(((char *)this));
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
extern "C" {
extern void UnloadBlueCoinModel(void *c);
}
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

// @symbol func_ov062_02118de8
extern "C" void func_ov062_02118de8(char *c)
{
    daNknk_c *self = (daNknk_c *)c;
    self->mHorzSpeed = 0;
    if (self->mAnimIndex == 2) {
        if (self->mModelAnim.Finished() == 0) return;
        func_ov062_02117994(c, 4);
        return;
    }
    if (self->mModelAnim.WillHitFrame((unsigned short)(self->mModelAnim.GetFrameCount() - 1)) != 0) {
        u16 *hp = &self->mWalkState;
        *hp = (u16)(*hp + 1);
    } else {
        func_ov062_02118058(c);
        return;
    }
    func_ov062_02117994(c, 0);
    if (self->mKoopaVariant == 1)
        self->mState = 5;
    else
        self->mState = 2;
    if (((unsigned int)RandomIntInternal(&data_0209e650) >> 16) & 0x8000) {
        int r = ((unsigned int)RandomIntInternal(&data_0209e650) >> 16) & 0x1fff;
        self->mTargetAngle = (s16)(self->mPrevAngleY - r);
    } else {
        int r = ((unsigned int)RandomIntInternal(&data_0209e650) >> 16) & 0x1fff;
        self->mTargetAngle = (s16)(self->mPrevAngleY + r);
    }
    func_ov062_02118058(c);
}

// @symbol func_ov062_02118cdc
extern "C" {
namespace tu {  /* namespaced: the file-scope view is (void*) */
void func_ov062_02118cdc(char *c)
{
    daNknk_c *self = (daNknk_c *)c;
    if (self->mTurning) {
        self->mTurning = (_Z14ApproachLinearRsss(&self->mPrevAngleY, self->mTargetAngle, 0x200) ^ 1) != 0;
    } else {
        if (self->mPlayerDist >= 0x61a8000) {
            self->mTargetAngle = self->mAimAngle;
        }
        self->mTurning = self->AngleAwayFromWallOrCliff(self->mWithMeshClsn, self->mTargetAngle);
        _Z14ApproachLinearRsss(&self->mPrevAngleY, self->mTargetAngle, 0x200);
    }
    if (self->mAnimIndex == 1) {
        func_ov062_02117724(c, 2, 8, 0x13, 0x19);
    }
    switch (self->mWalkState) {
    case 0: func_ov062_021181a0(c); break;
    case 1: func_ov062_0211811c(c); break;
    case 2: func_ov062_021180d4(c); break;
    }
    func_ov062_02118058(c);
}
}
}  /* namespace tu */

// @symbol func_ov062_02118b4c
extern "C" {
namespace tu {  /* namespaced: the file-scope view is (void*) */
void func_ov062_02118b4c(char *self) {
    daNknk_c *obj = (daNknk_c *)self;
    if (obj->mAnimIndex == 3)
        func_ov062_02117724(self, 2, 5, 8, 0xb);

    if (obj->mModelIndex != 0) {
        if ((u16)obj->mStateTimer > 0x1e && func_ov062_02117b60(self) > 0x320000) {
            if (_Z14ApproachLinearRiii(&obj->mHorzSpeed, 0, 0x1000) == 0)
                return;
            obj->mState = 1;
            func_ov062_02117994(self, 2);
            return;
        }
        _Z14ApproachLinearRsss(&obj->mPrevAngleY, obj->mAimAngle, 0x400);
        _Z14ApproachLinearRiii(&obj->mHorzSpeed, 0x11000, 0x1000);
        return;
    }

    if (obj->mPlayerDist >= 0x61a8000) {
        s16 *aim = &obj->mAimAngle;
        *aim = (s16)(*aim + 0x8000);
        obj->mPlayerDist = 0;
    }
    if ((u16)obj->mStateTimer > 0x1e && obj->mPlayerDist > 0x320000) {
        if (_Z14ApproachLinearRiii(&obj->mHorzSpeed, 0, 0x1000) == 0)
            return;
        obj->mState = 1;
        func_ov062_02117994(self, 2);
        return;
    }
    _Z14ApproachLinearRsss(&obj->mPrevAngleY, (s16)(obj->mAimAngle + 0x8000), 0x400);
    _Z14ApproachLinearRiii(&obj->mHorzSpeed, 0x11000, 0x1000);
}
}
}  /* namespace tu */

// @symbol func_ov062_02118a50
/* ldrh cannot encode 0x3c6, so the state helpers test mTimer and mTurnTimer
 * from mModelAnim (byte offsets 0xc6 and 0xc8) and decrement through &mTimer /
 * &mTurnTimer. A single member expression folds that reload. */
extern "C" {
namespace tu {  /* namespaced: a conflicting file-scope view exists */
void func_ov062_02118a50(char *c)
{
    daNknk_c *self = (daNknk_c *)c;
    if (self->mHorzSpeed != 0) {
        if (self->mWithMeshClsn.IsOnWall() != 0) {
            void *sr = _ZNK10dBgCh_Actr13GetWallResultEv(&self->mWithMeshClsn);
            ((SurfaceInfo*)((char*)sr + 4))->CopyNormalTo(*(Vector3*)&self->mWallNormalX);
            self->mPrevAngleY = _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s(
                c, self->mWallNormalX, self->mWallNormalZ, self->mPrevAngleY);
        }
        func_ov062_02118004(c, 0x4cc);
        return;
    }

    {
        if (*(u16 *)((char *)&self->mModelAnim + 0xc6) != 0) {
            u16 *p = &self->mTimer;
            *p = (u16)(*p - 1);
            if (*(u16 *)((char *)&self->mModelAnim + 0xc6) != 0) return;
            func_ov062_02117994(c, 8); return;
        }
    }

    if (self->mModelAnim.WillHitFrame(0x1e) != 0)
        func_ov062_021175c0(c);
    if (self->mModelAnim.Finished() == 0)
        return;
    self->mState = 1;
    func_ov062_02117994(c, 2);
}
}
}  /* namespace tu */

// @symbol func_ov062_02118a00
namespace tu {  /* namespaced: a conflicting file-scope view exists */
extern "C" void func_ov062_02118a00(void *c) {
    daNknk_c *self = (daNknk_c *)c;
    int gr = self->mWithMeshClsn.IsOnGround();
    if (gr == 0) return;
    int v = self->mKoopaVariant;
    if (v == 1) {
        self->mState = 5;
    } else {
        self->mState = 2;
    }
    func_ov062_02117994((char *)c, 0);
    func_ov062_021175c0(c);
}
}  /* namespace tu */

// @symbol func_ov062_02118718
/* See the ldrh note at func_ov062_02118a50 for the mModelAnim + 0xc6/0xc8 reads. */
extern "C" {
void func_ov062_02118718(char *c)
{
    daNknk_c *self = (daNknk_c *)c;
    int dist = 0x7fffffff;
    char *other;

    if (self->mAnimIndex == 3)
        func_ov062_02117724(c, 2, 5, 8, 0xb);

    if (self->mTurning) {
        self->mTurning =
            (_Z14ApproachLinearRsss(&self->mPrevAngleY, self->mTargetAngle, 0x200) ^ 1) != 0;
    } else {
        if (self->mPlayerDist >= 0x61a8000)
            self->mTargetAngle = self->mAimAngle;

        other = (char *)func_ov062_02117b9c(c);
        if (other) {
            dist = Vec3_Dist(&self->mPosX, &((dActor_c *)other)->mPosX);
            self->mTargetAngle = Vec3_HorzAngle((const Vector3 *)&self->mPosX, (const Vector3 *)&((dActor_c *)other)->mPosX);
        } else {
            self->mTurning =
                self->AngleAwayFromWallOrCliff(self->mWithMeshClsn, self->mTargetAngle);
            if (!self->mTurning) {
                if (*(u16 *)((char *)&self->mModelAnim + 0xc8) != 0) {
                    u16 *turn = &self->mTurnTimer;
                    *turn = (u16)(*turn - 1);
                } else {
                    /* (int)& forces the pool add. mTargetAngle -= folds to mModelAnim+0xc2. */
                    if (((unsigned)RandomIntInternal(&data_0209e650) >> 16) & 0x8000)
                        *(s16 *)((int)&self->mTargetAngle) -= ((unsigned)RandomIntInternal(&data_0209e650) >> 16) & 0x1fff;
                    else
                        *(s16 *)((int)&self->mTargetAngle) += ((unsigned)RandomIntInternal(&data_0209e650) >> 16) & 0x1fff;
                    self->mTurnTimer = 0x14;
                }
            }
        }

        if (self->mPlayerDist > 0x320000 ||
            (other != 0 &&
             self->GetSubtraction(self->mTargetAngle,
                 (short)(self->mPrevAngleY + 0x8000)) < 0x2000)) {
            _Z14ApproachLinearRsss(&self->mPrevAngleY, self->mTargetAngle, 0x600);
        } else {
            if (self->mModelIndex != 0)
                _Z14ApproachLinearRsss(&self->mPrevAngleY, self->mAimAngle, 0x600);
            else
                _Z14ApproachLinearRsss(&self->mPrevAngleY, (short)(self->mAimAngle + 0x8000), 0x600);
        }
    }

    if (_Z14ApproachLinearRiii(&self->mHorzSpeed, 0x14000, 0x1000) == 0)
        return;
    if (dist >= 0xc8000)
        return;
    if (self->GetSubtraction(self->mTargetAngle, self->mPrevAngleY) >= 0xc00)
        return;

    self->mPrevAngleY = self->mTargetAngle;
    self->mState = 2;
    self->mHorzSpeed += self->mHorzSpeed / 5;
    self->mVertSpeed = dist / 30;
    func_ov062_02117994(c, 6);
    self->mTimer = 0x14;
}
}

// @symbol func_ov062_02118588
/* See the ldrh note at func_ov062_02118a50 for the mModelAnim + 0xc6/0xc8 reads. */
extern "C" {
void func_ov062_02118588(char *c)
{
    daNknk_c *self = (daNknk_c *)c;
    dActor_c *found = 0;
    int match = 0;
    if (self->mState == 2) {
        if (self->mdCc_c.otherOwner != 0) {
            found = dActor_c::FindWithID(self->mdCc_c.otherOwner);
            if (found != 0) {
                int t = found->actorID;
                t = (t == 0x11d);
                if (t != 0)
                    match = 1;
            }
        }
    }
    if (match != 0) {
        self->mKoopaVariant = 0;
        self->mState = 4;
        self->mHorzSpeed = self->mHorzSpeed / 2;
        found->MarkForDestruction();
        return;
    }
    if (self->mHorzSpeed != 0) {
        func_ov062_02118004(c, 0x800);
        return;
    }

    {
        if (*(u16 *)((char *)&self->mModelAnim + 0xc6) != 0) {
            u16 *q = &self->mTimer;
            *q = (u16)(*q - 1);
            if (*(u16 *)((char *)&self->mModelAnim + 0xc6) != 0)
                return;
            func_ov062_02117994(c, 8);
            return;
        }
    }

    if (self->mModelAnim.WillHitFrame(0x1e) != 0)
        func_ov062_021175c0(c);
    if (self->mModelAnim.Finished() == 0)
        return;
    if (self->mModelIndex != 0) {
        self->mState = 5;
        func_ov062_02117994(c, 0);
        return;
    }
    self->mState = 1;
    func_ov062_02117994(c, 3);
}
}

// @symbol func_ov062_021183e0
extern "C" {
void func_ov062_021183e0(char *c)
{
    daNknk_c *self = (daNknk_c *)c;
    int dist = 0x7fffffff;
    char *other;

    if (self->mModelAnim.Finished())
        func_ov062_02117994(c, 1);

    if (self->mTurning) {
        self->mTurning = (_Z14ApproachLinearRsss(&self->mPrevAngleY, self->mTargetAngle, 0x200) ^ 1) != 0;
    } else {
        if (self->mPlayerDist >= 0x61a8000)
            self->mTargetAngle = self->mAimAngle;
        other = (char *)func_ov062_02117b9c(c);
        if (other) {
            dist = Vec3_Dist(&self->mPosX, &((dActor_c *)other)->mPosX);
            if (dist < 0xc8000)
                self->mTargetAngle = Vec3_HorzAngle((const Vector3 *)&self->mPosX, (const Vector3 *)&((dActor_c *)other)->mPosX);
        }
        self->mTurning = self->AngleAwayFromWallOrCliff(self->mWithMeshClsn, self->mTargetAngle);
        _Z14ApproachLinearRsss(&self->mPrevAngleY, self->mTargetAngle, 0x200);
    }

    if (dist < 0xc8000 && self->GetSubtraction(self->mTargetAngle, self->mPrevAngleY) < 0xc00) {
        self->mPrevAngleY = self->mTargetAngle;
        self->mState = 2;
        self->mHorzSpeed = 0x18000;
        self->mVertSpeed = dist / 30;
        func_ov062_02117994(c, 6);
        self->mTimer = 0x14;
        return;
    }

    if (self->mKoopaVariant == 2)
        _Z14ApproachLinearRiii(&self->mHorzSpeed, 0x2000, 0x4cc);
    else
        _Z14ApproachLinearRiii(&self->mHorzSpeed, 0x3000, 0x4cc);

    func_ov062_02118058(c);
}
}

// @symbol func_ov062_02118334
extern "C" {

void Vec3_Asr(struct Vec3 *d, struct Vec3 *s, int sh);
void Matrix4x3_FromTranslation(struct Matrix4x3 *m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationY(struct Matrix4x3 *m, short angY);
void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void *thiz, void *sm, struct Matrix4x3 *m, int radHeight, int a, u8 b);
extern struct Matrix4x3 data_020a0e68;
inline struct Matrix4x3 *inline_fn()
{
  return &data_020a0e68;
}

void func_ov062_02118334(char *c)
{
  daNknk_c *self = (daNknk_c *)c;
  short *new_var;
  struct Vec3 v;
  Vec3_Asr(&v, (struct Vec3 *)&self->mPosX, 3);
  if (1)
  {
    Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
    new_var = &self->mAngleY;
    Matrix4x3_ApplyInPlaceToRotationY(inline_fn(), *new_var);
    { struct M12w { int w[12]; };  /* array-wrapper copy: keeps C's block copy under -lang c++ */
      *(M12w *)&self->mModelAnim.mat4x3 = *(M12w *)&data_020a0e68; }
    {
      int b = (int) ((self->mFlags & 0x40000) != 0);
      if (b != 0)
      {
        return;
      }
    }
  }
  _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(c, &self->mShadowModel, &self->mModelAnim.mat4x3, 0x50000, 0x50000, 0xf);
}
}

// @symbol func_ov062_02118258
extern "C" {
namespace tu {  /* namespaced: a (void*) view of this symbol is in scope */
void func_ov062_02118258(char* c, int lim)
{
    daNknk_c *self = (daNknk_c *)c;
    self->mClosestPlayer = self->ClosestPlayer();

    if (self->mClosestPlayer == 0 || Vec3_Dist((Vector3*)&self->mPosX, (Vector3*)&self->mHomePosX) > lim) {
        self->mAimAngle = Vec3_HorzAngle((Vector3*)&self->mPosX, (Vector3*)&self->mHomePosX);
        self->mPlayerDist = 0x61a8000;
    } else {
        Vector3 v;
        int* src = &self->mClosestPlayer->mPosX;
        v.x = src[0];
        v.y = src[1];
        v.z = src[2];
        if (Vec3_Dist((Vector3*)&self->mHomePosX, &v) > lim) {
            self->mPlayerDist = 0x4e20000;
            return;
        }
        self->mPlayerDist = Vec3_Dist((Vector3*)&self->mPosX, &v);
        self->mAimAngle = Vec3_HorzAngle((Vector3*)&self->mPosX, &v);
    }
}
}
}  /* namespace tu */

// @symbol func_ov062_021181a0
extern "C" {
namespace tu {  /* namespaced: a (void*) view of this symbol is in scope */
void func_ov062_021181a0(char *c) {
    daNknk_c *self = (daNknk_c *)c;
    if (self->mKoopaVariant == 2) {
        _Z14ApproachLinearRiii(&self->mHorzSpeed, 0x2000, 0x4cc);
    } else {
        _Z14ApproachLinearRiii(&self->mHorzSpeed, 0x3000, 0x4cc);
    }
    if (self->mModelAnim.Finished() == 0) return;
    {
        u16 *p = &self->mWalkState;
        *p = (u16)(*p + 1);
    }
    self->mTimer =
        (u16)(((unsigned int)RandomIntInternal(&data_0209e650) >> 0x10) % 70 + 30);
    func_ov062_02117994(c, 1);
}
}
}  /* namespace tu */

// @symbol func_ov062_0211811c
/* See the ldrh note at func_ov062_02118a50 for the mModelAnim + 0xc6/0xc8 reads. */
extern "C" {
namespace tu {  /* namespaced: a (void*) view of this symbol is in scope */
void func_ov062_0211811c(char *c) {
    daNknk_c *self = (daNknk_c *)c;
    if (*(u16 *)((char *)&self->mModelAnim + 0xc6) != 0) {
        u16 *p = &self->mTimer;
        *p = (u16)(*p - 1);
        return;
    }
    if (self->mModelAnim.WillHitFrame((unsigned short)(self->mModelAnim.GetFrameCount() - 1)) == 0)
        return;
    {
        u16 *q = &self->mWalkState;
        *q = (u16)(*q + 1);
    }
    func_ov062_02117994(c, 2);
}
}
}  /* namespace tu */

// @symbol func_ov062_021180d4
extern "C" {
void func_ov062_021180d4(void *c) {
    daNknk_c *self = (daNknk_c *)c;
    _Z14ApproachLinearRiii(&self->mHorzSpeed, 0, 0x1000);
    int done = self->mModelAnim.Finished();
    if (!done) return;
    self->mState = 1;
    func_ov062_02117994((char*)c, 4);
}
}

// @symbol func_ov062_02118058
extern "C" {
void func_ov062_02118058(char *c){
    daNknk_c *self = (daNknk_c *)c;
  Player *o = self->mClosestPlayer;
  int d = self->mPlayerDist;
  if(o!=0) d=Vec3_Dist(&self->mPosX, &o->mPosX);
  if(d>=0x12c000) return;
  int s=self->GetSubtraction(self->mAimAngle, self->mPrevAngleY);
  if(s>=0x3000) return;
  if(self->mKoopaVariant==1) self->mState=1;
  else self->mState=3;
  func_ov062_02117994(c, 3);
}
}

// @symbol func_ov062_02118004
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" void func_ov062_02118004(void *c, int a1) {
    daNknk_c *self = (daNknk_c *)c;
    int r = self->mWithMeshClsn.IsOnGround();
    if (r == 0) return;
    _Z14ApproachLinearRiii(&self->mHorzSpeed, 0, a1);
    int x = self->mPosX;
    int y = self->mPosY;
    int z = self->mPosZ;
    _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(x, y, z);
}

// @symbol func_ov062_02117c98
extern "C" {
namespace tu {  /* namespaced: a conflicting file-scope view exists */
void func_ov062_02117c98(void* self)
{
    daNknk_c *koopa = (daNknk_c *)self;
    void* found;
    int turnAround;
    s32 flags;
    u32 id;

    id = koopa->mdCc_c.otherOwner;
    if (id == 0)
        return;
    found = dActor_c::FindWithID(id);
    if (found == 0)
        return;

    flags = koopa->mdCc_c.hitFlags;
    turnAround = (int)(((long long)(int)0));

    if (flags & 0x10) {
        Vector3_16 v;
        v.x = (s16)-0x2000;
        v.y = (s16)turnAround;
        v.z = (s16)turnAround;
        // The member's unused Fix12<int> argument is one raw register word.
        // Native aggregate passing adds a load/copy here under 2004/b56;
        // retain decl_Enemy.h's scalar ABI bridge to the actual member symbol.
        _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(self, &v, found, 0x46000);
        return;
    }
    if (flags & 0x22400) {
        if (koopa->mKoopaVariant == 0) {
            func_ov062_02117bf4(self);
            return;
        }
        koopa->mDeathState = 5;
        func_ov002_020aea30(self, found, (dBgCh_Actr*)turnAround);
        return;
    }
    if (flags & 0x4000) {
        koopa->mDeathState = 6;
        turnAround = 1;
    } else if (flags & 0x447e0) {
        if (koopa->mKoopaVariant == 0) {
            func_ov062_02117bf4(self);
            if (flags & 0x3c0)
                koopa->mPrevAngleY = ((dActor_c *)found)->mAngleY;
        } else {
            if (flags & 0x40040)
                koopa->mDeathState = 2;
            else if (flags & 0x20400)
                koopa->mDeathState = 5;
            else if (flags & 0x380)
                koopa->mDeathState = 3;
            else if (flags & 0x4000)
                koopa->mDeathState = 6;
            else {
                koopa->mDeathState = 1;
                func_0201267c(0x113, (const Vector3*)&koopa->mCamSpacePosX);
                koopa->mScaleX = 0x1000;
                koopa->mScaleY = 0x1000;
                koopa->mScaleZ = 0x1000;
            }
            func_ov002_020aea30(self, found, 0);
            return;
        }
    } else {
        Player *player = (Player *)found;
        struct { Vec3 sv; Vec3 hv; } L;
        int isPlayer;
        isPlayer = (player->actorID == 0xbf) ? 1 : turnAround;
        if ((int)(((long long)isPlayer)) == 0)
            goto tail;
        if (player->mIsMetal != 0) {
            if (koopa->mKoopaVariant == 0) {
                func_ov062_02117bf4(self);
                goto tail;
            }
            koopa->mDeathState = 6;
            func_ov002_020aea30(self, found, 0);
            return;
        }
        {
            s32* s = &player->mPosX;
            L.sv.x = s[0];
            L.sv.y = s[1];
            L.sv.z = s[2];
        }
        if (player->IsOnShell()) {
            koopa->mDeathState = 5;
            turnAround = 1;
            goto tail;
        }
        if (koopa->JumpedOnByPlayer(koopa->mdCc_c, *player)) {
            if (koopa->mKoopaVariant == 0) {
                func_ov062_02117bf4(self);
            } else {
                koopa->mDeathState = 1;
                func_ov002_020aea30(self, found, 0);
                koopa->mScaleX = 0x1000;
                koopa->mScaleY = 0x1000;
                koopa->mScaleZ = 0x1000;
                func_0201267c(0x113, (const Vector3*)&koopa->mCamSpacePosX);
            }
            _ZN6Player6BounceE5Fix12IiE(found, 0x28000);
            return;
        }
        if (player->mIsVanish != 0)
            goto tail;
        if ((koopa->mdCc_c.hitFlags & 0x400000) == 0)
            goto tail;
        if (koopa->mState == 0)
            goto tail;
        {
            s32 pw;
            L.hv.x = koopa->mPosX;
            L.hv.y = koopa->mPosY;
            L.hv.z = koopa->mPosZ;
            pw = koopa->mHorzSpeed;
            if (pw < 0xf000)
                pw = 0xf000;
            _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(player, &L.hv, 0, pw, 1, 0, 1);
            func_ov062_02117b48(self);
            if (koopa->mKoopaVariant == 2) {
                koopa->PoofDust();
                func_ov062_021179e4(self);
                koopa->KillAndTrackInDeathTable();
            }
        }
    }

tail:
    func_ov002_020aea30(self, found, &koopa->mWithMeshClsn);
    if (turnAround)
        koopa->mAngleY = (u16)(koopa->mPrevAngleY + 0x8000);
}
}
}  /* namespace tu */

// @symbol func_ov062_02117bf4
namespace tu {  /* namespaced: a conflicting file-scope view exists */
extern "C" void func_ov062_02117bf4(char* c){
    daNknk_c *self = (daNknk_c *)c;
    func_0201267c(0xed, (const Vector3*)&self->mCamSpacePosX);
    self->mKoopaVariant = 1;
    self->mState = 3;
    self->mHorzSpeed = 0x14000;
    func_ov062_02117994(c, 7);
    {
        Vector3 v;
        int yy = self->mPosZ;
        int zz = self->mPosY + 0x3c000;
        v.x = self->mPosX;
        v.y = zz;
        v.z = yy;
        dActor_c::Spawn(0x11d, self->mModelIndex, v, 0, self->mAreaId, -1);
    }
    self->mInvincibleTimer = 0xa;
    func_0201267c(1, (const Vector3*)&self->mCamSpacePosX);
}
}  /* namespace tu */

// @symbol func_ov062_02117b9c
extern "C" {
void *func_ov062_02117b9c(void *c) {
    daNknk_c *self = (daNknk_c *)c;
    dActor_c *found = 0;
    dActor_c *best = 0;
    Fix12i bestDist = 0x7FFFFFFF;
    while (1) {
        found = dActor_c::FindWithActorID(0x11d, found);
        if (!found) break;
        Fix12i d = Vec3_Dist((Vector3*)&self->mPosX, (Vector3*)&found->mPosX);
        if (d < bestDist) {
            bestDist = d;
            best = found;
        }
    }
    return best;
}
}

// @symbol func_ov062_02117b60
extern "C" {
int func_ov062_02117b60(void* c)
{
    daNknk_c *self = (daNknk_c *)c;
    Player *target = self->mClosestPlayer;
    if (!target) return 0x61a8000;
    return Vec3_Dist(&self->mPosX, &target->mPosX);
}
}

// @symbol func_ov062_02117b48
extern "C" {
namespace tu {  /* namespaced: a conflicting file-scope view exists */
void func_ov062_02117b48(char *p)
{
    daNknk_c *self = (daNknk_c *)p;
    self->mState = 0;
    self->mVertSpeed = 81920;
    self->mHorzSpeed = 0;
}
}
}  /* namespace tu */

// @symbol func_ov062_02117acc
extern "C" {
void func_ov062_02117acc(char *c){
    daNknk_c *self = (daNknk_c *)c;
  switch(self->mState){
  case 0: func_ov062_02118a00(c); break;
  case 1: func_ov062_02118de8(c); break;
  case 2: func_ov062_02118cdc(c); break;
  case 3: func_ov062_02118b4c(c); break;
  case 4: func_ov062_02118a50(c); break;
  }
}
}

// @symbol func_ov062_02117a3c
extern "C" void func_ov062_02117a3c(char *c)
{
    daNknk_c *self = (daNknk_c *)c;
    switch (self->mState) {
    case 0: func_ov062_02118a00(c); break;
    case 1: func_ov062_02118718(c); break;
    case 2: func_ov062_02118588(c); break;
    case 3: func_ov062_02118588(c); break;
    case 4: func_ov062_02118de8(c); break;
    case 5: func_ov062_021183e0(c); break;
    }
}

// @symbol func_ov062_021179e4
namespace tu {
extern "C" void func_ov062_021179e4(daNknk_c* c) {
    int x = c->mPosX;
    int z = c->mPosZ;
    int y = c->mPosY + 0x78000;
    Vector3 pos;
    pos.x = x;
    pos.y = y;
    pos.z = z;
    dActor_c::Spawn(0x122, 2, pos, 0, c->mAreaId, -1);
}
}

// @symbol func_ov062_02117994
extern "C" {
void func_ov062_02117994(char *c, int idx) {
    daNknk_c *self = (daNknk_c *)c;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &self->mModelAnim,
        (BCA_File*)data_ov062_0211cee8[idx]->file,
        data_ov062_0211cf0c[idx],
        self->mAnimSpeed,
        0
    );
    self->mAnimIndex = (u8)idx;
}
}

// @symbol func_ov062_02117724
// 6f: keep constant live / flip coloring
extern "C" {
namespace tu {  /* namespaced: conflicting file-scope views exist */
void func_ov062_02117724(char *t, unsigned int a1, unsigned int a2, unsigned int a3, unsigned short a4) /* uxth vs a1-a3: ROM compares h<=a4 after a halfword arg */
{
    daNknk_c *self = (daNknk_c *)t;
    unsigned int h = (unsigned int)(self->mModelAnim.currFrame << 4) >> 16;
    if ((h >= a1 && h <= a2) || (h >= a3 && h <= a4)) {
        Vector3 pos;
        int k, iscb, idx, uid, iscb2;
        short ang;
        char *ps;
        if (self->mFootstep != 0) return;
        self->mFootstep = 1;
        func_0201267c(0xe4, (const Vector3*)&self->mCamSpacePosX);
        if (self->mAnimIndex == 1) return;
        ang = (short)(self->mAngleY + 0x4000);
        pos.x = self->mPosX;
        pos.y = self->mPosY;
        pos.z = self->mPosZ;
        iscb = self->actorID == 0xcb;
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
        iscb2 = self->actorID == 0xcb;
        if (iscb2)
            *(int *)(ps + 0x50) = (short)(((long long)*(int *)(ps + 0x50) * 0x800 + 0x800) >> 12);
        else
            *(int *)(ps + 0x50) = (short)(((long long)*(int *)(ps + 0x50) * 0x500 + 0x800) >> 12);
    } else {
        self->mFootstep = 0;
    }
}
}
}  /* namespace tu */

// @symbol func_ov062_021175c0
extern "C" {
namespace tu {  /* namespaced: conflicting file-scope views exist */
void func_ov062_021175c0(char *c)
{
    daNknk_c *self = (daNknk_c *)c;
    volatile Vector3 pos;
    int t;
    u32 spawned;
    void *particle;

    if (self->mLandingDustTimer != 0) return;

    pos.x = self->mPosX;
    pos.y = self->mPosY;
    pos.z = self->mPosZ;

    t = self->actorID;
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

    self->mLandingDustTimer = 0xa;
    if (spawned == 0) return;

    particle = Particle::System::FromUniqueID(spawned);
    if (particle == 0) return;

    {
        int t2 = self->actorID;
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
}
}  /* namespace tu */

// @symbol func_ov062_02117570
namespace tu {  /* namespaced: conflicting file-scope views exist */
extern "C" {
struct dActor_c;
struct dBgCh_Actr;
struct dBgCh_Actr *_ZNK10dBgCh_Actr14GetFloorResultEv(struct dBgCh_Actr *self);
int SurfaceInfo_TestFlag0x20(int *p);
void func_ov062_02117570(struct dActor_c *self) {
    daNknk_c *koopa = (daNknk_c *)self;
    if (!koopa->mWithMeshClsn.IsOnGround()) return;
    struct dBgCh_Actr *floor = _ZNK10dBgCh_Actr14GetFloorResultEv((struct dBgCh_Actr *)&koopa->mWithMeshClsn);
    if (!SurfaceInfo_TestFlag0x20((int*)((char*)floor + 4))) return;
    koopa->SpawnCoin();
    koopa->KillAndTrackInDeathTable();
}
}
}  /* namespace tu */
