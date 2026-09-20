//cpp
/* Koopa movement, shell reactions, animation, and drawing.
 * Retail RTTI identifies daNknk_c; both normal and small profiles construct it.
 * The inline destructor and ordinary factories emit the matching lifecycle.
 * This text-only TU supplies all 39 functions; canonical metadata stays in ROM.
 * The manifest preserves source lineage; remaining raw views are in issue #2871.
 * Source functions run in reverse ROM order for mwccarm 2004/b56 emission.
 */
// Inline definitions in Koopa.h, emitted here by the two factories:
#include "Koopa.h"
#include "common.h"
#include "types.h"
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

/* Remaining legacy helper views are local to this translation unit. */
/* shadow typedef 'u32' */
typedef unsigned int u32;

/* shadow typedef 's32' */
typedef signed int s32;

/* shadow typedef 's64' */
typedef long long s64;

/* shadow struct 'Entry' */
struct Entry { char pad[4]; void *file; };

/* shadow typedef 'Fix12i' */
typedef int Fix12i;

/* shadow typedef 'Vec3' */
typedef struct { s32 x, y, z; } Vec3;

/* shadow struct 'Particle' */
struct Particle {
    static void RunningSlidingDustAt(int a, int b, int c);
};

/* TUBUILD CONFLICT -- alternate body of typedef 'Vector3', from the legacy file for func_ov062_02117b9c, NOT applied:
typedef struct Vector3 { int x, y, z; } Vector3;
*/

/* TUBUILD CONFLICT -- alternate body of typedef 'Vector3', from the legacy file for func_ov062_02118258, NOT applied:
typedef struct Vector3 { int x, y, z; } Vector3;
*/

/* TUBUILD CONFLICT -- alternate body of struct 'dBgCh_Actr', from the legacy file for func_ov062_02118a00, NOT applied:
struct dBgCh_Actr {
    int IsOnGround() const;
};
*/

#define AT(p, off) ((void*)(int)(((long long)(int)((char*)(p) + (off)))))
/* TUBUILD CONFLICT -- alternate #define of AT, from the legacy file for func_ov062_02118258, NOT applied: #define AT(p, off) ((void*)(int)((char*)(p) + (off))) */

extern "C" {
extern int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(u32 uniqueID, u32 effectID, s32 x, s32 y, s32 z, const void *dir, void *callback);
extern void _ZN8Particle19SetSelfDestructFlagEj(u32 id);
extern void *_ZN8Particle6System12FromUniqueIDEj(u32 id);
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
extern int _ZN6Player9IsOnShellEv(void* p);
extern int _ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player(void* self, void* clsn, void* player);
extern void _ZN6Player6BounceE5Fix12IiE(void* p, int f);
extern int _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(Player* p, void* v, u32 a, int f, u8 b, u8 c, u8 d);
extern void func_ov062_02117b48(void* p);
extern void _ZN8dActor_c8PoofDustEv(void* self);
extern void func_ov062_021179e4(void* c);
extern void _ZN8dActor_c24KillAndTrackInDeathTableEv(void* self);
extern "C" int _Z14ApproachLinearRiii(int *r, int target, int speed);
extern "C" void _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(int a, int b, int c);
extern int _ZN8dActor_c14GetSubtractionEss(void*, short, short);
extern int _ZN9Animation8FinishedEv(void *);
extern u32 _ZNK9Animation13GetFrameCountEv(void *self);
extern bool _ZNK9Animation12WillHitFrameEi(void *self, int frame);
extern int RandomIntInternal(int *seed);
extern int data_0209e650;
extern void* _ZN8dActor_c13ClosestPlayerEv(void*);
extern short Vec3_HorzAngle(const Vector3* a, const Vector3* b);
extern int _Z14ApproachLinearRsss(short *a, short b, short c);
extern void *func_ov062_02117b9c(void *c);
extern int _ZN12dEnemyBase_c24AngleAwayFromWallOrCliffER10dBgCh_ActrRs(void *self, dBgCh_Actr &clsn, short &angle);
extern void func_ov062_02118058(char *c);
extern void _ZN7fBase_c18MarkForDestructionEv(void *a);
extern void func_ov062_02118004(void *c, int a1);
extern void func_ov062_021175c0(void *c);
extern void func_ov062_02117724(void *c, unsigned int a, unsigned int b, unsigned int d, unsigned short e);
extern int _ZNK10dBgCh_Actr8IsOnWallEv(void *w);

extern s16 _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s(void *self, int a, int b, s16 ang);
extern int func_ov062_02117b60(void *c);
extern void func_ov062_021181a0(void *c);
extern void func_ov062_0211811c(void *c);
extern void func_ov062_021180d4(void *c);
struct V3 { int x, y, z; };

extern void _ZN8dActor_c19MakeVanishLuigiWorkER5dCc_c(void *self, void *c);

extern void _ZN5dCc_c5ClearEv(void *c);
extern void _ZN5dCc_c6UpdateEv(void *c);
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void *c);
extern int _ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(void *self, int d);

extern SharedFilePtr* data_ov062_0211ced8[2];
extern SharedFilePtr* data_ov062_0211cee0[2];

extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void* self, dActor_c* a, int r, int h, unsigned int e, unsigned int g);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void* self, dActor_c* a, int r, int h, Vector3_16* p, int q);
extern void _ZN10dBgCh_Actr19StartDetectingWaterEv(void* self);
extern void LoadBlueCoinModel(void* c);
/* TUBUILD CONFLICT -- alternate declaration of _ZN8Particle19SetSelfDestructFlagEj, from the legacy file for func_ov062_02117724, NOT applied: extern void _ZN8Particle19SetSelfDestructFlagEj(int id); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN8Particle6System12FromUniqueIDEj, from the legacy file for func_ov062_02117724, NOT applied: extern char *_ZN8Particle6System12FromUniqueIDEj(int id); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov062_02118a00, from the legacy file for func_ov062_02117acc, NOT applied: extern void func_ov062_02118a00(void *c); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov062_02118de8, from the legacy file for func_ov062_02117acc, NOT applied: extern void func_ov062_02118de8(void *c); */
/* TUBUILD CONFLICT -- alternate declaration of Vec3_Dist, from the legacy file for func_ov062_02117b9c, NOT applied: extern Fix12i Vec3_Dist(const Vector3 *a, const Vector3 *b); */
/* TUBUILD CONFLICT -- alternate declaration of Vec3_Dist, from the legacy file for func_ov062_02118058, NOT applied: extern Fix12i Vec3_Dist(const void*, const void*); */
/* TUBUILD CONFLICT -- alternate declaration of _Z14ApproachLinearRiii, from the legacy file for func_ov062_021180d4, NOT applied: extern int _Z14ApproachLinearRiii(int *, int, int); */
/* TUBUILD CONFLICT -- alternate declaration of _Z14ApproachLinearRiii, from the legacy file for func_ov062_021181a0, NOT applied: extern int _Z14ApproachLinearRiii(int *, int, int); */
/* TUBUILD CONFLICT -- alternate declaration of Vec3_Dist, from the legacy file for func_ov062_02118258, NOT applied: extern int Vec3_Dist(const Vector3* a, const Vector3* b); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN9Animation8FinishedEv, from the legacy file for func_ov062_021183e0, NOT applied: extern int _ZN9Animation8FinishedEv(void *anim); */
/* TUBUILD CONFLICT -- alternate declaration of Vec3_Dist, from the legacy file for func_ov062_021183e0, NOT applied: extern int Vec3_Dist(void *a, void *b); */
/* TUBUILD CONFLICT -- alternate declaration of Vec3_HorzAngle, from the legacy file for func_ov062_021183e0, NOT applied: extern short Vec3_HorzAngle(void *a, void *b); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN8dActor_c14GetSubtractionEss, from the legacy file for func_ov062_021183e0, NOT applied: extern int _ZN8dActor_c14GetSubtractionEss(void *self, short a, short b); */
/* TUBUILD CONFLICT -- alternate declaration of _Z14ApproachLinearRiii, from the legacy file for func_ov062_021183e0, NOT applied: extern void _Z14ApproachLinearRiii(int *a, int b, int c); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN9Animation8FinishedEv, from the legacy file for func_ov062_02118588, NOT applied: extern int _ZN9Animation8FinishedEv(void *anim); */
/* TUBUILD CONFLICT -- alternate declaration of Vec3_Dist, from the legacy file for func_ov062_02118718, NOT applied: extern int Vec3_Dist(void *a, void *b); */
/* TUBUILD CONFLICT -- alternate declaration of Vec3_HorzAngle, from the legacy file for func_ov062_02118718, NOT applied: extern short Vec3_HorzAngle(void *a, void *b); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN8dActor_c14GetSubtractionEss, from the legacy file for func_ov062_02118718, NOT applied: extern int _ZN8dActor_c14GetSubtractionEss(void *self, short a, short b); */
/* TUBUILD CONFLICT -- alternate declaration of _Z14ApproachLinearRiii, from the legacy file for func_ov062_02118718, NOT applied: extern int _Z14ApproachLinearRiii(int *a, int b, int c); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov062_021175c0, from the legacy file for func_ov062_02118a00, NOT applied: extern void func_ov062_021175c0(void *); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN9Animation8FinishedEv, from the legacy file for func_ov062_02118a50, NOT applied: extern int _ZN9Animation8FinishedEv(void *anim); */
/* TUBUILD CONFLICT -- alternate declaration of _Z14ApproachLinearRiii, from the legacy file for func_ov062_02118b4c, NOT applied: extern int _Z14ApproachLinearRiii(int *val, int target, int step); */
/* TUBUILD CONFLICT -- alternate declaration of _Z14ApproachLinearRsss, from the legacy file for func_ov062_02118b4c, NOT applied: extern int _Z14ApproachLinearRsss(s16 *val, int target, int step); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN9Animation8FinishedEv, from the legacy file for func_ov062_02118de8, NOT applied: extern "C" int _ZN9Animation8FinishedEv(void *a); */
/* TUBUILD CONFLICT -- alternate declaration of RandomIntInternal, from the legacy file for func_ov062_02118de8, NOT applied: extern "C" int RandomIntInternal(int *seed); */
/* TUBUILD CONFLICT -- alternate declaration of data_0209e650, from the legacy file for func_ov062_02118de8, NOT applied: extern "C" int data_0209e650; */
/* TUBUILD CONFLICT -- alternate declaration of _ZN8dActor_c8PoofDustEv, from the legacy file for _ZN5Koopa8BehaviorEv, NOT applied: extern void _ZN8dActor_c8PoofDustEv(void *self); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN8dActor_c24KillAndTrackInDeathTableEv, from the legacy file for _ZN5Koopa8BehaviorEv, NOT applied: extern void _ZN8dActor_c24KillAndTrackInDeathTableEv(void *self); */
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 38 -- daNknk_c_classInit_NOKONOKO, 0x0211970c, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol daNknk_c_classInit_NOKONOKO
extern "C" {  /* .c-derived member: C linkage for the whole block */
daNknk_c *daNknk_c_classInit_NOKONOKO()
{
    return new daNknk_c;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 37 -- daNknk_c_classInit_NOKONOKO_S, 0x021196bc, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol daNknk_c_classInit_NOKONOKO_S
extern "C" {  /* .c-derived member: C linkage for the whole block */
daNknk_c *daNknk_c_classInit_NOKONOKO_S()
{
    return new daNknk_c;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 36 -- _ZN8daNknk_c13OnYoshiTryEatEv, 0x021196a8, size 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daNknk_c13OnYoshiTryEatEv
s32 daNknk_c::OnYoshiTryEat() {
    char* c = (char*)this;
  if(*(int*)(c+0x394)==0) return 6;
  return 5;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 35 -- _ZN8daNknk_c13OnTurnIntoEggER6Player, 0x02119628, size 0x80 */
/* -------------------------------------------------------------------------- */
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 34 -- _ZN8daNknk_c16OnAimedAtWithEggEv, 0x02119608, size 0x20 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daNknk_c16OnAimedAtWithEggEv
// Community name: Koopa_OnAimedAtWithEgg
/* daNknk_c::OnAimedAtWithEgg - recovered from vtable slot identity */
s32 daNknk_c::OnAimedAtWithEgg() {
    void * c = (void *)this;
    unsigned short v = *(unsigned short*)((char*)c + 0xc);
    int r;
    if (v == 0xcb) r = 1; else r = 0;
    if (r != 0) r = 0x46000; else r = 0x25800;
    return r;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 33 -- _ZN8daNknk_c13InitResourcesEv, 0x02119420, size 0x1e8 */
/* -------------------------------------------------------------------------- */
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

    unk_106 = 0;
    mInvincibleTimer = 0;
    unk_39c = mPosX;
    unk_3a0 = mPosY;
    unk_3a4 = mPosZ;
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x32000, 0x32000, 0, 0);

    _ZN10dBgCh_Actr19StartDetectingWaterEv((char*)&mWithMeshClsn);

    unk_108 = 3;
    LoadBlueCoinModel(((char*)this));

    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 32 -- _ZN8daNknk_c8BehaviorEv, 0x021190ec, size 0x334 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daNknk_c8BehaviorEv
int daNknk_c::Behavior()
{
    int state;
    int kind;

    if (UpdateKillByInvincibleChar(mWithMeshClsn, mModelAnim, 3) != 0)
        return 1;

    _ZN8dActor_c19MakeVanishLuigiWorkER5dCc_c(this, &mdCc_c);

    if (UpdateYoshiEat(mWithMeshClsn) != 0) {
        int *pb0 = (int *)((char *)&mFlags);
        *pb0 = *pb0 & ~0x10000000;
        if (_ZN12dEnemyBase_c27SpawnParticlesIfHitOtherObjER5dCc_c(this, &mdCc_c) != 0) {
            _ZN8dActor_c8PoofDustEv(this);
            func_ov062_021179e4(((char *)this));
            _ZN8dActor_c24KillAndTrackInDeathTableEv(this);
        }
        if (mEatenByYoshi != 0)
            func_ov062_02117570(((char *)this));
        func_ov062_02118334(((char *)this));
        _ZN5dCc_c5ClearEv((char *)&mdCc_c);
        if (mEatenByYoshi != 0 && unk_104 == 0)
            _ZN5dCc_c6UpdateEv((char *)&mdCc_c);
        if (mKoopaVariant == 1)
            mState = 4;
        else
            mState = 1;
        if (_ZNK10dBgCh_Actr10IsOnGroundEv((char *)&mWithMeshClsn) != 0) {
            unk_3a8 = mPosX;
            unk_3ac = mPosY;
            unk_3b0 = mPosZ;
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
                mPosX = unk_3a8;
                mPosY = unk_3ac;
                mPosZ = unk_3b0;
            } else {
                unk_3a8 = mPosX;
                unk_3ac = mPosY;
                unk_3b0 = mPosZ;
            }
        }

        UpdateWMClsn(mWithMeshClsn, 0);
        func_ov062_02117570(((char *)this));
        _ZN5dCc_c5ClearEv((char *)&mdCc_c);
        if (mDeathState == 0) {
            if (mInvincibleTimer == 0) {
                _ZN5dCc_c6UpdateEv((char *)&mdCc_c);
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 31 -- _ZN8daNknk_c6RenderEv, 0x02118f84, size 0x168 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daNknk_c6RenderEv
int daNknk_c::Render()
{
  volatile struct V3 saved;
  int b = (mFlags & 0x40000) != 0;
  if (b) return 1;
  if (mKoopaVariant == 1) {
    _ZN5Model12ShowMaterialEii(&mModelAnim, 0, 1);
    _ZN5Model12HideMaterialEii(&mModelAnim, 0, 2);
  } else {
    _ZN5Model12HideMaterialEii(&mModelAnim, 0, 1);
    _ZN5Model12ShowMaterialEii(&mModelAnim, 0, 2);
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 30 -- _ZN8daNknk_c16OnPendingDestroyEv, 0x02118f80, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daNknk_c16OnPendingDestroyEv
/* recovered: shared header, real C++ method
 *
 * fBase_c slot 12. Empty in the ROM: four bytes, `bx lr`.
 */
void daNknk_c::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 29 -- _ZN8daNknk_c16CleanupResourcesEv, 0x02118f04, size 0x7c */
/* -------------------------------------------------------------------------- */
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 28 -- func_ov062_02118de8, 0x02118de8, size 0x11c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_02118de8
extern "C" void func_ov062_02118de8(char *c)
{
    *(int*)(c + 0x98) = 0;
    if (*(unsigned char*)(c + 0x398) == 2) {
        if (_ZN9Animation8FinishedEv(c + 0x350) == 0) return;
        func_ov062_02117994(c, 4);
        return;
    }
    if (_ZNK9Animation12WillHitFrameEi(c + 0x350, (unsigned short)(_ZNK9Animation13GetFrameCountEv(c + 0x350) - 1)) != 0) {
        unsigned short *hp = (unsigned short*)(((int)c + 0x3c4));
        *hp += 1;
    } else {
        func_ov062_02118058(c);
        return;
    }
    func_ov062_02117994(c, 0);
    if (*(int*)(c + 0x390) == 1)
        *(int*)(c + 0x38c) = 5;
    else
        *(int*)(c + 0x38c) = 2;
    if (((unsigned int)RandomIntInternal(&data_0209e650) >> 16) & 0x8000) {
        int r = ((unsigned int)RandomIntInternal(&data_0209e650) >> 16) & 0x1fff;
        *(short*)(c + 0x3c2) = *(short*)(c + 0x94) - r;
    } else {
        int r = ((unsigned int)RandomIntInternal(&data_0209e650) >> 16) & 0x1fff;
        *(short*)(c + 0x3c2) = *(short*)(c + 0x94) + r;
    }
    func_ov062_02118058(c);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 27 -- func_ov062_02118cdc, 0x02118cdc, size 0x10c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_02118cdc
extern "C" {  /* .c-derived member: C linkage for the whole block */
namespace tu {  /* namespaced: the file-scope view is (void*) */
void func_ov062_02118cdc(char *c)
{
    if (*(unsigned char*)(c+0x3cc)) {
        *(unsigned char*)(c+0x3cc) = (_Z14ApproachLinearRsss((short*)(c+0x94), *(short*)(c+0x3c2), 0x200) ^ 1) != 0;
    } else {
        if (*(int*)(c+0x3b8) >= 0x61a8000) {
            *(short*)(c+0x3c2) = *(short*)(c+0x3c0);
        }
        *(unsigned char*)(c+0x3cc) = _ZN12dEnemyBase_c24AngleAwayFromWallOrCliffER10dBgCh_ActrRs(c, *(dBgCh_Actr*)(c+0x144), *(short*)(c+0x3c2));
        _Z14ApproachLinearRsss((short*)(c+0x94), *(short*)(c+0x3c2), 0x200);
    }
    if (*(unsigned char*)(c+0x398) == 1) {
        func_ov062_02117724(c, 2, 8, 0x13, 0x19);
    }
    switch (*(unsigned short*)(c+0x3c4)) {
    case 0: func_ov062_021181a0(c); break;
    case 1: func_ov062_0211811c(c); break;
    case 2: func_ov062_021180d4(c); break;
    }
    func_ov062_02118058(c);
}
}
}  /* namespace tu */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 26 -- func_ov062_02118b4c, 0x02118b4c, size 0x190 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_02118b4c
extern "C" {  /* .c-derived member: C linkage for the whole block */
namespace tu {  /* namespaced: the file-scope view is (void*) */
void func_ov062_02118b4c(char *self) {
    if (*(u8 *)(self + 0x398) == 3)
        func_ov062_02117724(self, 2, 5, 8, 0xb);

    if (*(int *)(self + 0x394) != 0) {
        if (*(u16 *)(self + 0x100) > 0x1e && func_ov062_02117b60(self) > 0x320000) {
            if (_Z14ApproachLinearRiii((int *)(self + 0x98), 0, 0x1000) == 0)
                return;
            *(int *)(self + 0x38c) = 1;
            func_ov062_02117994(self, 2);
            return;
        }
        _Z14ApproachLinearRsss((s16 *)(self + 0x94), *(s16 *)(self + 0x3c0), 0x400);
        _Z14ApproachLinearRiii((int *)(self + 0x98), 0x11000, 0x1000);
        return;
    }

    if (*(int *)(self + 0x3b8) >= 0x61a8000) {
        *(s16 *)(((int)self + 0x3c0)) += 0x8000;
        *(int *)(self + 0x3b8) = 0;
    }
    if (*(u16 *)(self + 0x100) > 0x1e && *(int *)(self + 0x3b8) > 0x320000) {
        if (_Z14ApproachLinearRiii((int *)(self + 0x98), 0, 0x1000) == 0)
            return;
        *(int *)(self + 0x38c) = 1;
        func_ov062_02117994(self, 2);
        return;
    }
    _Z14ApproachLinearRsss((s16 *)(self + 0x94), (s16)(*(s16 *)(self + 0x3c0) + 0x8000), 0x400);
    _Z14ApproachLinearRiii((int *)(self + 0x98), 0x11000, 0x1000);
}
}
}  /* namespace tu */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 25 -- func_ov062_02118a50, 0x02118a50, size 0xfc */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_02118a50
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" {  /* .c-derived member: C linkage for the whole block */
namespace tu {  /* namespaced: a conflicting file-scope view exists */
void func_ov062_02118a50(char *c)
{
    if (*(int *)(c + 0x98) != 0) {
        if (_ZNK10dBgCh_Actr8IsOnWallEv(c + 0x144) != 0) {
            void *sr = _ZNK10dBgCh_Actr13GetWallResultEv(c + 0x144);
            ((SurfaceInfo*)((char*)sr + 4))->CopyNormalTo(*(Vector3*)(c + 0xe0));
            *(s16 *)(c + 0x94) = _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s(
                c, *(int *)(c + 0xe0), *(int *)(c + 0xe8), *(s16 *)(c + 0x94));
        }
        func_ov062_02118004(c, 0x4cc);
        return;
    }

    {
        if (*(u16 *)(c + 0x3c6) != 0) {
            u16 *p = (u16 *)(c + 0x3c6);
            *p = (u16)(*p - 1);
            if (*(u16 *)((c + 0x300) + 0xc6) != 0) return;
            func_ov062_02117994(c, 8); return;
        }
    }

    if (_ZNK9Animation12WillHitFrameEi(c + 0x350, 0x1e) != 0)
        func_ov062_021175c0(c);
    if (_ZN9Animation8FinishedEv(c + 0x350) == 0)
        return;
    *(int *)(c + 0x38c) = 1;
    func_ov062_02117994(c, 2);
}
}
}  /* namespace tu */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 24 -- func_ov062_02118a00, 0x02118a00, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_02118a00
namespace tu {  /* namespaced: a conflicting file-scope view exists */
extern "C" void func_ov062_02118a00(void *c) {
    int gr = ((dBgCh_Actr *)((char *)c + 0x144))->IsOnGround();
    if (gr == 0) return;
    int v = *(int *)((char *)c + 0x390);
    if (v == 1) {
        *(int *)((char *)c + 0x38c) = 5;
    } else {
        *(int *)((char *)c + 0x38c) = 2;
    }
    func_ov062_02117994((char *)c, 0);
    func_ov062_021175c0(c);
}
}  /* namespace tu */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 23 -- func_ov062_02118718, 0x02118718, size 0x2e8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_02118718
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov062_02118718(char *c)
{
    int dist = 0x7fffffff;
    char *other;

    if (*(unsigned char*)(c + 0x398) == 3)
        func_ov062_02117724(c, 2, 5, 8, 0xb);

    if (*(unsigned char*)(c + 0x3cc)) {
        *(unsigned char*)(c + 0x3cc) =
            (_Z14ApproachLinearRsss((short*)(c + 0x94), *(short*)(c + 0x3c2), 0x200) ^ 1) != 0;
    } else {
        if (*(int*)(c + 0x3b8) >= 0x61a8000)
            *(short*)(c + 0x3c2) = *(short*)(c + 0x3c0);

        other = (char *)func_ov062_02117b9c(c);
        if (other) {
            dist = Vec3_Dist(c + 0x5c, other + 0x5c);
            *(short*)(c + 0x3c2) = Vec3_HorzAngle((const Vector3 *)(c + 0x5c), (const Vector3 *)(other + 0x5c));
        } else {
            *(unsigned char*)(c + 0x3cc) =
                _ZN12dEnemyBase_c24AngleAwayFromWallOrCliffER10dBgCh_ActrRs(c, *(dBgCh_Actr*)(c + 0x144), *(short*)(c + 0x3c2));
            if (!*(unsigned char*)(c + 0x3cc)) {
                if (*(unsigned short*)(c + 0x3c8) != 0) {
                    *(unsigned short*)(((int)c + 0x3c8)) -= 1;
                } else {
                    if (((unsigned)RandomIntInternal(&data_0209e650) >> 16) & 0x8000)
                        *(short*)(((int)c + 0x3c2)) -= ((unsigned)RandomIntInternal(&data_0209e650) >> 16) & 0x1fff;
                    else
                        *(short*)(((int)c + 0x3c2)) += ((unsigned)RandomIntInternal(&data_0209e650) >> 16) & 0x1fff;
                    *(unsigned short*)(c + 0x3c8) = 0x14;
                }
            }
        }

        if (*(int*)(c + 0x3b8) > 0x320000 ||
            (other != 0 &&
             _ZN8dActor_c14GetSubtractionEss(c, *(short*)(c + 0x3c2),
                 (short)(*(short*)(c + 0x94) + 0x8000)) < 0x2000)) {
            _Z14ApproachLinearRsss((short*)(c + 0x94), *(short*)(c + 0x3c2), 0x600);
        } else {
            if (*(int*)(c + 0x394) != 0)
                _Z14ApproachLinearRsss((short*)(c + 0x94), *(short*)(c + 0x3c0), 0x600);
            else
                _Z14ApproachLinearRsss((short*)(c + 0x94), (short)(*(short*)(c + 0x3c0) + 0x8000), 0x600);
        }
    }

    if (_Z14ApproachLinearRiii((int*)(c + 0x98), 0x14000, 0x1000) == 0)
        return;
    if (dist >= 0xc8000)
        return;
    if (_ZN8dActor_c14GetSubtractionEss(c, *(short*)(c + 0x3c2), *(short*)(c + 0x94)) >= 0xc00)
        return;

    *(short*)(c + 0x94) = *(short*)(c + 0x3c2);
    *(int*)(c + 0x38c) = 2;
    *(int*)(((int)c + 0x98)) += *(int*)(c + 0x98) / 5;
    *(int*)(c + 0xa8) = dist / 30;
    func_ov062_02117994(c, 6);
    *(short*)(c + 0x3c6) = 0x14;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 22 -- func_ov062_02118588, 0x02118588, size 0x190 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_02118588
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov062_02118588(char *c)
{
    void *found = 0;
    int match = 0;
    if (*(int *)(c + 0x38c) == 2) {
        if (*(unsigned int *)(c + 0x134) != 0) {
            found = dActor_c::FindWithID(*(unsigned int *)(c + 0x134));
            if (found != 0) {
                int t = *(unsigned short *)((char *)found + 0xc);
                t = (t == 0x11d);
                if (t != 0)
                    match = 1;
            }
        }
    }
    if (match != 0) {
        int *hp = (int *)(c + 0x98);
        *(int *)(c + 0x390) = 0;
        *(int *)(c + 0x38c) = 4;
        *hp = *hp / 2;
        _ZN7fBase_c18MarkForDestructionEv(found);
        return;
    }
    if (*(int *)(c + 0x98) != 0) {
        func_ov062_02118004(c, 0x800);
        return;
    }

    {
        if (*(unsigned short *)(c + 0x3c6) != 0) {
            unsigned short *q = (unsigned short *)(c + 0x3c6);
            char *p = c + 0x300;
            *q = (unsigned short)(*q - 1);
            if (*(unsigned short *)(p + 0xc6) != 0)
                return;
            func_ov062_02117994(c, 8);
            return;
        }
    }

    if (_ZNK9Animation12WillHitFrameEi(c + 0x350, 0x1e) != 0)
        func_ov062_021175c0(c);
    if (_ZN9Animation8FinishedEv(c + 0x350) == 0)
        return;
    if (*(int *)(c + 0x394) != 0) {
        *(int *)(c + 0x38c) = 5;
        func_ov062_02117994(c, 0);
        return;
    }
    *(int *)(c + 0x38c) = 1;
    func_ov062_02117994(c, 3);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 21 -- func_ov062_021183e0, 0x021183e0, size 0x1a8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_021183e0
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov062_021183e0(char *c)
{
    int dist = 0x7fffffff;
    char *other;

    if (_ZN9Animation8FinishedEv(c + 0x350))
        func_ov062_02117994(c, 1);

    if (*(unsigned char*)(c + 0x3cc)) {
        *(unsigned char*)(c + 0x3cc) = (_Z14ApproachLinearRsss((short*)(c + 0x94), *(short*)(c + 0x3c2), 0x200) ^ 1) != 0;
    } else {
        if (*(int*)(c + 0x3b8) >= 0x61a8000)
            *(short*)(c + 0x3c2) = *(short*)(c + 0x3c0);
        other = (char *)func_ov062_02117b9c(c);
        if (other) {
            dist = Vec3_Dist(c + 0x5c, other + 0x5c);
            if (dist < 0xc8000)
                *(short*)(c + 0x3c2) = Vec3_HorzAngle((const Vector3 *)(c + 0x5c), (const Vector3 *)(other + 0x5c));
        }
        *(unsigned char*)(c + 0x3cc) = _ZN12dEnemyBase_c24AngleAwayFromWallOrCliffER10dBgCh_ActrRs(c, *(dBgCh_Actr*)(c + 0x144), *(short*)(c + 0x3c2));
        _Z14ApproachLinearRsss((short*)(c + 0x94), *(short*)(c + 0x3c2), 0x200);
    }

    if (dist < 0xc8000 && _ZN8dActor_c14GetSubtractionEss(c, *(short*)(c + 0x3c2), *(short*)(c + 0x94)) < 0xc00) {
        *(short*)(c + 0x94) = *(short*)(c + 0x3c2);
        *(int*)(c + 0x38c) = 2;
        *(int*)(c + 0x98) = 0x18000;
        *(int*)(c + 0xa8) = dist / 30;
        func_ov062_02117994(c, 6);
        *(short*)(c + 0x3c6) = 0x14;
        return;
    }

    if (*(int*)(c + 0x390) == 2)
        _Z14ApproachLinearRiii((int*)(c + 0x98), 0x2000, 0x4cc);
    else
        _Z14ApproachLinearRiii((int*)(c + 0x98), 0x3000, 0x4cc);

    func_ov062_02118058(c);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- func_ov062_02118334, 0x02118334, size 0xac */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_02118334
/* recovered: shared common types */
extern "C" {  /* .c-derived member: C linkage for the whole block */

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
  short *new_var;
  struct Vec3 v;
  Vec3_Asr(&v, (struct Vec3 *) (c + 0x5c), 3);
  if (1)
  {
    Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
    new_var = (short *) (c + 0x8e);
    Matrix4x3_ApplyInPlaceToRotationY(inline_fn(), *new_var);
    { struct M12w { int w[12]; };  /* array-wrapper copy: keeps C's block copy under -lang c++ */
      *(M12w *)(c + 0x31c) = *(M12w *)&data_020a0e68; }
    {
      int b = (int) (((*((int *) (c + 0xb0))) & 0x40000) != 0);
      if (b != 0)
      {
        return;
      }
    }
  }
  _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(c, (void *) (c + 0x364), (struct Matrix4x3 *) (c + 0x31c), 0x50000, 0x50000, 0xf);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- func_ov062_02118258, 0x02118258, size 0xdc */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_02118258
extern "C" {  /* .c-derived member: C linkage for the whole block */
namespace tu {  /* namespaced: a (void*) view of this symbol is in scope */
void func_ov062_02118258(char* c, int lim)
{
    *(void**)(c + 0x3b4) = _ZN8dActor_c13ClosestPlayerEv(c);

    if (*(void**)(c + 0x3b4) == 0 || Vec3_Dist((Vector3*)(c + 0x5c), (Vector3*)(c + 0x39c)) > lim) {
        *(short*)(c + 0x3c0) = Vec3_HorzAngle((Vector3*)(c + 0x5c), (Vector3*)(c + 0x39c));
        *(int*)(c + 0x3b8) = 0x61a8000;
    } else {
        Vector3 v;
        int* src = (int*)AT(*(char**)(c + 0x3b4), 0x5c);
        v.x = src[0];
        v.y = src[1];
        v.z = src[2];
        if (Vec3_Dist((Vector3*)(c + 0x39c), &v) > lim) {
            *(int*)(c + 0x3b8) = 0x4e20000;
            return;
        }
        *(int*)(c + 0x3b8) = Vec3_Dist((Vector3*)(c + 0x5c), &v);
        *(short*)(c + 0x3c0) = Vec3_HorzAngle((Vector3*)(c + 0x5c), &v);
    }
}
}
}  /* namespace tu */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- func_ov062_021181a0, 0x021181a0, size 0xb8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_021181a0
extern "C" {  /* .c-derived member: C linkage for the whole block */
namespace tu {  /* namespaced: a (void*) view of this symbol is in scope */
void func_ov062_021181a0(char *c) {
    if (*(int*)(c + 0x390) == 2) {
        _Z14ApproachLinearRiii((int*)(c + 0x98), 0x2000, 0x4cc);
    } else {
        _Z14ApproachLinearRiii((int*)(c + 0x98), 0x3000, 0x4cc);
    }
    if (_ZN9Animation8FinishedEv(c + 0x350) == 0) return;
    {
        unsigned short *p = (unsigned short*)(((int)c + 0x3c4));
        *p = (unsigned short)(*p + 1);
    }
    {
        char *b = c + 0x300;
        *(unsigned short*)(b + 0xc6) =
            (unsigned short)(((unsigned int)RandomIntInternal(&data_0209e650) >> 0x10) % 70 + 30);
    }
    func_ov062_02117994(c, 1);
}
}
}  /* namespace tu */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- func_ov062_0211811c, 0x0211811c, size 0x84 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_0211811c
extern "C" {  /* .c-derived member: C linkage for the whole block */
namespace tu {  /* namespaced: a (void*) view of this symbol is in scope */
void func_ov062_0211811c(char *c) {
    char *b = c + 0x300;
    if (*(unsigned short*)(b + 0xc6) != 0) {
        unsigned short *p = (unsigned short*)(((int)c + 0x3c6));
        *p = (unsigned short)(*p - 1);
        return;
    }
    if (_ZNK9Animation12WillHitFrameEi(c + 0x350, (unsigned short)(_ZNK9Animation13GetFrameCountEv(c + 0x350) - 1)) == 0)
        return;
    {
        unsigned short *q = (unsigned short*)(((int)c + 0x3c4));
        *q = (unsigned short)(*q + 1);
    }
    func_ov062_02117994(c, 2);
}
}
}  /* namespace tu */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- func_ov062_021180d4, 0x021180d4, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_021180d4
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov062_021180d4(void *c) {
    _Z14ApproachLinearRiii((int*)((char*)c + 0x98), 0, 0x1000);
    int done = _ZN9Animation8FinishedEv((char*)c + 0x350);
    if (!done) return;
    *(int*)((char*)c + 0x38c) = 1;
    func_ov062_02117994((char*)c, 4);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- func_ov062_02118058, 0x02118058, size 0x7c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_02118058
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov062_02118058(char *c){
  void *o=*(void**)(c+0x3b4);
  int d=*(int*)(c+0x3b8);
  if(o!=0) d=Vec3_Dist(c+0x5c, (char*)o+0x5c);
  if(d>=0x12c000) return;
  int s=_ZN8dActor_c14GetSubtractionEss(c, *(short*)(c+0x3c0), *(short*)(c+0x94));
  if(s>=0x3000) return;
  if(*(int*)(c+0x390)==1) *(int*)(c+0x38c)=1;
  else *(int*)(c+0x38c)=3;
  func_ov062_02117994(c, 3);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- func_ov062_02118004, 0x02118004, size 0x54 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_02118004
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" void func_ov062_02118004(void *c, int a1) {
    int r = ((dBgCh_Actr*)((char*)c + 0x144))->IsOnGround();
    if (r == 0) return;
    _Z14ApproachLinearRiii((int*)((char*)c + 0x98), 0, a1);
    int x = *(int*)((char*)c + 0x5c);
    int y = *(int*)((char*)c + 0x60);
    int z = *(int*)((char*)c + 0x64);
    _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(x, y, z);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- func_ov062_02117c98, 0x02117c98, size 0x36c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_02117c98
extern "C" {  /* .c-derived member: C linkage for the whole block */
namespace tu {  /* namespaced: a conflicting file-scope view exists */
void func_ov062_02117c98(void* self)
{
    u8* c = (u8*)self;
    void* found;
    int r5;
    s32 flags;
    u32 id;

    id = *(u32*)(c + 0x134);
    if (id == 0)
        return;
    found = dActor_c::FindWithID(id);
    if (found == 0)
        return;

    flags = *(s32*)(c + 0x130);
    r5 = (int)(((long long)(int)0));

    if (flags & 0x10) {
        Vector3_16 v;
        v.x = (s16)-0x2000;
        v.y = (s16)r5;
        v.z = (s16)r5;
        // The member's unused Fix12<int> argument is one raw register word.
        // Native aggregate passing adds a load/copy here under 2004/b56;
        // retain decl_Enemy.h's scalar ABI bridge to the actual member symbol.
        _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(self, &v, found, 0x46000);
        return;
    }
    if (flags & 0x22400) {
        if (*(s32*)(c + 0x390) == 0) {
            func_ov062_02117bf4(self);
            return;
        }
        *(s32*)(c + 0x10c) = 5;
        func_ov002_020aea30(self, found, (dBgCh_Actr*)r5);
        return;
    }
    if (flags & 0x4000) {
        *(s32*)(c + 0x10c) = 6;
        r5 = 1;
    } else if (flags & 0x447e0) {
        if (*(s32*)(c + 0x390) == 0) {
            func_ov062_02117bf4(self);
            if (flags & 0x3c0)
                *(s16*)(c + 0x94) = *(s16*)((u8*)found + 0x8e);
        } else {
            if (flags & 0x40040)
                *(s32*)(c + 0x10c) = 2;
            else if (flags & 0x20400)
                *(s32*)(c + 0x10c) = 5;
            else if (flags & 0x380)
                *(s32*)(c + 0x10c) = 3;
            else if (flags & 0x4000)
                *(s32*)(c + 0x10c) = 6;
            else {
                *(s32*)(c + 0x10c) = 1;
                func_0201267c(0x113, (const Vector3*)(c + 0x74));
                *(s32*)(c + 0x80) = 0x1000;
                *(s32*)(c + 0x84) = 0x1000;
                *(s32*)(c + 0x88) = 0x1000;
            }
            func_ov002_020aea30(self, found, 0);
            return;
        }
    } else {
        u8* f = (u8*)found;
        struct { Vec3 sv; Vec3 hv; } L;
        int shell;
        shell = (*(u16*)(f + 0xc) == 0xbf) ? 1 : r5;
        if ((int)(((long long)shell)) == 0)
            goto tail;
        if (*(u8*)(f + 0x6f9) != 0) {
            if (*(s32*)(c + 0x390) == 0) {
                func_ov062_02117bf4(self);
                goto tail;
            }
            *(s32*)(c + 0x10c) = 6;
            func_ov002_020aea30(self, found, 0);
            return;
        }
        {
            s32* s = (s32*)AT(f, 0x5c);
            L.sv.x = s[0];
            L.sv.y = s[1];
            L.sv.z = s[2];
        }
        if (_ZN6Player9IsOnShellEv(found)) {
            *(s32*)(c + 0x10c) = 5;
            r5 = 1;
            goto tail;
        }
        if (_ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player(self, (void*)(c + 0x110), found)) {
            if (*(s32*)(c + 0x390) == 0) {
                func_ov062_02117bf4(self);
            } else {
                *(s32*)(c + 0x10c) = 1;
                func_ov002_020aea30(self, found, 0);
                *(s32*)(c + 0x80) = 0x1000;
                *(s32*)(c + 0x84) = 0x1000;
                *(s32*)(c + 0x88) = 0x1000;
                func_0201267c(0x113, (const Vector3*)(c + 0x74));
            }
            _ZN6Player6BounceE5Fix12IiE(found, 0x28000);
            return;
        }
        if (*(u8*)(f + 0x6fb) != 0)
            goto tail;
        if ((*(s32*)(c + 0x130) & 0x400000) == 0)
            goto tail;
        if (*(s32*)(c + 0x38c) == 0)
            goto tail;
        {
            s32 pw;
            L.hv.x = *(s32*)(c + 0x5c);
            L.hv.y = *(s32*)(c + 0x60);
            L.hv.z = *(s32*)(c + 0x64);
            pw = *(s32*)(c + 0x98);
            if (pw < 0xf000)
                pw = 0xf000;
            _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj((Player*)found, &L.hv, 0, pw, 1, 0, 1);
            func_ov062_02117b48(self);
            if (*(s32*)(c + 0x390) == 2) {
                _ZN8dActor_c8PoofDustEv(self);
                func_ov062_021179e4(self);
                _ZN8dActor_c24KillAndTrackInDeathTableEv(self);
            }
        }
    }

tail:
    func_ov002_020aea30(self, found, (dBgCh_Actr*)(c + 0x144));
    if (r5)
        *(u16*)(c + 0x8e) = *(s16*)(c + 0x94) + 0x8000;
}
}
}  /* namespace tu */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- func_ov062_02117bf4, 0x02117bf4, size 0xa4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_02117bf4
/* recovered: shared common types */
namespace tu {  /* namespaced: a conflicting file-scope view exists */
extern "C" void func_ov062_02117bf4(char* c){
    func_0201267c(0xed, (const Vector3*)(c + 0x74));
    *(int*)(c+0x390) = 1;
    *(int*)(c+0x38c) = 3;
    *(int*)(c+0x98) = 0x14000;
    func_ov062_02117994(c, 7);
    {
        Vector3 v;
        int yy = *(int*)(c+0x64);
        int zz = *(int*)(c+0x60) + 0x3c000;
        v.x = *(int*)(c+0x5c);
        v.y = zz;
        v.z = yy;
        dActor_c::Spawn(0x11d, *(unsigned int*)(c+0x394), v, 0, *(signed char*)(c+0xcc), -1);
    }
    *(short*)(c+0x3ca) = 0xa;
    func_0201267c(1, (const Vector3*)(c + 0x74));
}
}  /* namespace tu */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- func_ov062_02117b9c, 0x02117b9c, size 0x58 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_02117b9c
extern "C" {  /* .c-derived member: C linkage for the whole block */
void *func_ov062_02117b9c(void *c) {
    void *found = 0;
    void *best = 0;
    Fix12i bestDist = 0x7FFFFFFF;
    while (1) {
        found = dActor_c::FindWithActorID(0x11d, (dActor_c*)found);
        if (!found) break;
        Fix12i d = Vec3_Dist((Vector3*)((char*)c + 0x5c), (Vector3*)((char*)found + 0x5c));
        if (d < bestDist) {
            bestDist = d;
            best = found;
        }
    }
    return best;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- func_ov062_02117b60, 0x02117b60, size 0x3c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_02117b60
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov062_02117b60(void* c)
{
    void* r1 = *(void**)((char*)c + 0x3b4);
    if (!r1) return 0x61a8000;
    return Vec3_Dist((char*)c + 0x5c, (char*)r1 + 0x5c);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- func_ov062_02117b48, 0x02117b48, size 0x18 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_02117b48
extern "C" {  /* .c-derived member: C linkage for the whole block */
namespace tu {  /* namespaced: a conflicting file-scope view exists */
void func_ov062_02117b48(char *p)
{
    *(int *)(p + 0x38c) = 0;
    *(int *)(p + 0xa8) = 81920;
    *(int *)(p + 0x98) = 0;
}
}
}  /* namespace tu */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- func_ov062_02117acc, 0x02117acc, size 0x7c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_02117acc
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov062_02117acc(char *c){
  switch(*(int*)(c+0x38c)){
  case 0: func_ov062_02118a00(c); break;
  case 1: func_ov062_02118de8(c); break;
  case 2: func_ov062_02118cdc(c); break;
  case 3: func_ov062_02118b4c(c); break;
  case 4: func_ov062_02118a50(c); break;
  }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov062_02117a3c, 0x02117a3c, size 0x90 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_02117a3c
extern "C" void func_ov062_02117a3c(char *c)
{
    switch (*(int *)(c + 0x38c)) {
    case 0: func_ov062_02118a00(c); break;
    case 1: func_ov062_02118718(c); break;
    case 2: func_ov062_02118588(c); break;
    case 3: func_ov062_02118588(c); break;
    case 4: func_ov062_02118de8(c); break;
    case 5: func_ov062_021183e0(c); break;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov062_021179e4, 0x021179e4, size 0x58 */
/* -------------------------------------------------------------------------- */
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov062_02117994, 0x02117994, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_02117994
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov062_02117994(char *c, int idx) {
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        (ModelAnim*)(c + 0x300),
        (BCA_File*)data_ov062_0211cee8[idx]->file,
        data_ov062_0211cf0c[idx],
        *(Fix12i*)(c + 0x3bc),
        0
    );
    *(u8*)(c + 0x398) = (u8)idx;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov062_02117724, 0x02117724, size 0x270 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_02117724
// 6f: keep constant live / flip coloring
extern "C" {  /* .c-derived member: C linkage for the whole block */
namespace tu {  /* namespaced: conflicting file-scope views exist */
void func_ov062_02117724(char *t, unsigned int a1, unsigned int a2, unsigned int a3, unsigned short a4) /* uxth vs a1-a3: ROM compares h<=a4 after a halfword arg */
{
    unsigned int h = (unsigned int)(*(int *)(t + 0x358) << 4) >> 16;
    if ((h >= a1 && h <= a2) || (h >= a3 && h <= a4)) {
        Vector3 pos;
        int k, iscb, idx, uid, iscb2;
        short ang;
        char *ps;
        if (*(unsigned char *)(t + 0x3cd) != 0) return;
        *(unsigned char *)(t + 0x3cd) = 1;
        func_0201267c(0xe4, (const Vector3*)(t + 0x74));
        if (*(unsigned char *)(t + 0x398) == 1) return;
        ang = (short)(*(short *)(t + 0x8e) + 0x4000);
        pos.x = *(int *)(t + 0x5c);
        pos.y = *(int *)(t + 0x60);
        pos.z = *(int *)(t + 0x64);
        iscb = *(unsigned short *)(t + 0xc) == 0xcb;
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
        ps = (char *)_ZN8Particle6System12FromUniqueIDEj(uid);
        if (ps == 0) return;
        iscb2 = *(unsigned short *)(t + 0xc) == 0xcb;
        if (iscb2)
            *(int *)(ps + 0x50) = (short)(((long long)*(int *)(ps + 0x50) * 0x800 + 0x800) >> 12);
        else
            *(int *)(ps + 0x50) = (short)(((long long)*(int *)(ps + 0x50) * 0x500 + 0x800) >> 12);
    } else {
        *(unsigned char *)(t + 0x3cd) = 0;
    }
}
}
}  /* namespace tu */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov062_021175c0, 0x021175c0, size 0x164 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_021175c0
extern "C" {  /* .c-derived member: C linkage for the whole block */
namespace tu {  /* namespaced: conflicting file-scope views exist */
void func_ov062_021175c0(char *c)
{
    volatile Vector3 pos;
    int t;
    u32 spawned;
    void *particle;

    if (*(unsigned char *)(c + 0x3ce) != 0) return;

    pos.x = *(int *)(c + 0x5c);
    pos.y = *(int *)(c + 0x60);
    pos.z = *(int *)(c + 0x64);

    t = *(unsigned short *)(c + 0xc);
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

    *(unsigned char *)(c + 0x3ce) = 0xa;
    if (spawned == 0) return;

    particle = _ZN8Particle6System12FromUniqueIDEj(spawned);
    if (particle == 0) return;

    {
        int t2 = *(unsigned short *)(c + 0xc);
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov062_02117570, 0x02117570, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_02117570
namespace tu {  /* namespaced: conflicting file-scope views exist */
extern "C" {
struct dActor_c;
struct dBgCh_Actr;
int _ZNK10dBgCh_Actr10IsOnGroundEv(struct dBgCh_Actr *self);
struct dBgCh_Actr *_ZNK10dBgCh_Actr14GetFloorResultEv(struct dBgCh_Actr *self);
int SurfaceInfo_TestFlag0x20(int *p);
void _ZN12dEnemyBase_c9SpawnCoinEv(struct dActor_c *self);
void _ZN8dActor_c24KillAndTrackInDeathTableEv(struct dActor_c *self);
void func_ov062_02117570(struct dActor_c *self) {
    if (!_ZNK10dBgCh_Actr10IsOnGroundEv((struct dBgCh_Actr *)((char*)self + 0x144))) return;
    struct dBgCh_Actr *floor = _ZNK10dBgCh_Actr14GetFloorResultEv((struct dBgCh_Actr *)((char*)self + 0x144));
    if (!SurfaceInfo_TestFlag0x20((int*)((char*)floor + 4))) return;
    _ZN12dEnemyBase_c9SpawnCoinEv(self);
    _ZN8dActor_c24KillAndTrackInDeathTableEv(self);
}
}
}  /* namespace tu */
