//cpp
struct Vector3_16f;
namespace Particle {
struct Callback;
struct System {
    static int New(unsigned, unsigned, int, int, int, const ::Vector3_16f *, Callback *);
    static unsigned NewUnkCallback818(unsigned, unsigned, int, int, int, const ::Vector3_16f *);
};
}
/* Fly Guy (PROPELLER_HEYHO), ov070. .text 0x0211f000..0x02120570,
 * inline destructor D1 then D0, plus the 25 functions below. It wanders
 * a home position, chases a player inside 1000 units, then dives or
 * (when param1's low byte is nonzero, half the time) spits a fireball.
 * func_ov070_0211f100 sorts contact. Source order is the reverse of the
 * ROM, so the one .text section emits ascending.
 */
/* leftovers: Player::SpinBounce and Player::Hurt stay mangled: each takes
 * Fix12<int> by value.
 * Finished and WillHitFrame stay qualified through mAnmModel; a char*
 * view of that member moves a word in func_ov070_0211f48c,
 * func_ov070_0211f62c and func_ov070_0211f6e0. Player-position reads
 * through int* in func_ov070_0211fd98, func_ov070_0211fae4 and
 * func_ov070_0211f48c, and the Vector3 write through ((int *)&v)[i] in
 * func_ov070_0211f368, stay as written. The mPrevAngle triple keeps
 * the base-class names.
 * FlyGuy_ChangeState keeps its C-ABI name. ApproachAngleInt is the
 * int-target overload. V3w and V3h keep the ldm/stm copy in
 * func_ov070_0211f6e0 and func_ov070_0211f48c. The six state records and
 * the file handles keep address names; this file's static initializer
 * fills them. daPropeller_Heyho_Fire_c is a separate TU.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_propeller_heyho_AssetId {
    ASSET_PROPELLER_HEYHO_BMD = 0x0411,
    ASSET_PROPELLER_HEYHO_ATTACK01_BCA = 0x0412,
    ASSET_PROPELLER_HEYHO_ATTACK02_BCA = 0x0413,
    ASSET_PROPELLER_HEYHO_ATTACK03_BCA = 0x0414,
    ASSET_PROPELLER_HEYHO_ATTACK04_BCA = 0x0415,
    ASSET_PROPELLER_HEYHO_FIRE_ATTACK_BCA = 0x0416,
    ASSET_PROPELLER_HEYHO_WAIT_BCA = 0x0417
};

#include "daPropeller_Heyho_c.h"
#include "SpawnInfo.h"
#include "math/Approach.h"

/* The cartridge preserves this descriptor's C-ABI symbol but not its source
 * spelling.  Keep that evidence-bounded alias; all seven scalar words and
 * the factory relocation are covered by the TU's data claim. */
extern "C" daPropeller_Heyho_c *daPropeller_Heyho_c_classInit(void);

/* Reconstructed source-style name: SM64DS proves this descriptor through
 * its registry role, the PROPELLER_HEYHO literal ROM profile ID, and the
 * factory relocation it carries; later EAD lineage supplies g_profile_.
 * Exact original spelling is not preserved. Historical alias:
 * FlyGuy_SpawnInfo. */
extern "C" SpawnInfo<daPropeller_Heyho_c> g_profile_PROPELLER_HEYHO = {
    daPropeller_Heyho_c_classInit,
    0x00e8,
    0x0057,
    0x10000003,
    0x00064000,
    0x000c8000,
    0x01000000,
    0x01000000
};

struct V3w { int w[3]; };  /* array-wrapper: C++ scalarizes a plain struct copy; this form keeps the C front end's ldm/stm block copy */
struct V3h { short h[3]; };

/* Named constants.  Fix12 values are written as the raw word (0x1000 = 1.0)
 * with the unit value in the comment; frame counts are game-logic frames (the frame rate is not established here). */
enum {
    kLevel0x16 = 0x16,            /* LEVEL_ID (data_0209f2f8, declared as a byte here) value at which this
                                     code skips re-anchoring the home position and the +300.0 home lift,
                                     and uses the alternate dive aim height / arrival distance */
    kPlayerActorId = 191,         /* PLAYER in symbols/actor_debug_names.tsv */

    kHomeLiftEscape   = 0xc8000,  /* 200.0 units added to the home Y when the chase loses the player or the dive finds the player underwater */
    kHomeLiftRecover  = 0x12c000, /* 300.0 units added to the home Y when the settle, or a dive that hurt the player, finishes */
    kAimAbovePlayer   = 0xc8000,  /* 200.0 units: the chase aim point is this far above the player */
    kAttackCooldown   = 0x5a,     /* 90 frames: mCooldown reload when an attack/fire/settle ends */

    kWanderTurnBackDist = 0x1f4000, /* 500.0 units from home: wander turns back toward home */
    kHomeLeashDist      = 0x5dc000, /* 1500.0 units from home: the leash for wander/chase/dive */
    kChaseTriggerDist   = 0x3e8000, /* 1000.0 units: a player closer than this starts the chase */
    kArriveDist         = 0x258000, /* 600.0 units: chase "arrived" distance to the aim point */
    kArriveDistLevel0x16 = 0x384000, /* 900.0 units: the same distance when LEVEL_ID == 0x16 */

    kFireFrame = 0xd,             /* animation frame 13 of the spit animation: the fireball leaves here */
    kSndFireSpit = 0x105,         /* sound id; daKrpa_c plays the same id for its spit */
    kSndMegaKill = 0x1d,          /* sound id played with IncMegaKillCount */

    /* dCc_c hitFlags bits, read with the table in include/dCc_c.h (only 0x2000 egg and
       0x4000 explosion are proven there; the rest are its plausible reading) */
    kHitMegaChar   = 0x10,
    kHitSpinPound  = 0x20,
    kHitFire       = 0x40000,
    kHitAttackMask = 0x67c0       /* 0x40 punch | 0x80 kick | 0x100 breakdance | 0x200 slide kick
                                     | 0x400 (player's) dive | 0x2000 egg | 0x4000 explosion */
};

/* dActor_c::mFlags bit 0x40000 is set while the actor is in Yoshi's mouth.
 * These int-returning predicates are what make the ROM materialize 0/1
 * before testing it. */
static inline int IsInYoshiMouth(dActor_c *actor) {
    return (actor->mFlags & 0x40000) != 0;
}

static inline int IsPlayerActor(dActor_c *actor) {
    return actor->actorID == kPlayerActorId;
}

/* -------------------------------------------------------------------------- */
// @symbol daPropeller_Heyho_c_classInit
/* Registry factory for the PROPELLER_HEYHO profile. */
extern "C" daPropeller_Heyho_c *daPropeller_Heyho_c_classInit(void)
{
    return new daPropeller_Heyho_c();
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN19daPropeller_Heyho_c13OnYoshiTryEatEv
/* Returns 5 to Yoshi's swallow attempt; what the value means is not recovered. */
s32 daPropeller_Heyho_c::OnYoshiTryEat() {
    return 5;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN19daPropeller_Heyho_c13OnTurnIntoEggER6Player
/* Vtable slot 19. _ZTV19daPropeller_Heyho_c at 0x02123168 + 0x4c is this body. */
#include "Player.h"

/* Turned into an egg: the player is paid mExtraCoinCount + 1 coins
 * (InitResources seeds mExtraCoinCount to 1, so 2 coins here), then this
 * actor is killed and recorded in the death table. */
void daPropeller_Heyho_c::OnTurnIntoEgg(Player &player)
{
    GivePlayerCoins(player, (u8)(mExtraCoinCount + 1), 0);
    KillAndTrackInDeathTable();
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN19daPropeller_Heyho_c16OnAimedAtWithEggEv
/* Egg-aim query. Returns the fixed height the slot stores. */
s32 daPropeller_Heyho_c::OnAimedAtWithEgg() {
    return 0x2b000; /* Fix12 egg-aim HEIGHT added to pos.y, per dEnemyBase_c.h slot-29: 43.0 units */
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN19daPropeller_Heyho_c13InitResourcesEv
#include "SharedFilePtr.h"
/* File-scope objects at the end of this file construct the seven resource
 * handles (model file 0x411, animation files 0x417/0x412/0x413/0x414/0x415/
 * 0x416) and fill the six state records. mwcc emits __sinit_d_a_propeller_heyho.cpp
 * from those definitions. The wrapper names are local; the handle
 * constructors and destructors are the ROM resource-family functions,
 * aliased in the manifest. */
struct HeyhoModelFile : SharedFilePtr {
    HeyhoModelFile(u32 fileID);
    ~HeyhoModelFile();
};

struct HeyhoAnimationFilePtr : SharedFilePtr {
    HeyhoAnimationFilePtr(u32 fileID);
    ~HeyhoAnimationFilePtr();
};

extern HeyhoModelFile data_ov070_02123530;
extern HeyhoAnimationFilePtr data_ov070_02123520;
extern HeyhoAnimationFilePtr data_ov070_02123518;
extern HeyhoAnimationFilePtr data_ov070_02123510;
extern HeyhoAnimationFilePtr data_ov070_02123528;
extern HeyhoAnimationFilePtr data_ov070_02123508;
extern HeyhoAnimationFilePtr data_ov070_02123500;
/* The six state records (each {init PMF, main PMF}, built by __sinit_ov070_02122afc
 * from PMF literals in .data). The role names below are read off the handler
 * bodies:
 *   data_ov070_0212359c  wander   init func_ov070_0211ffa8   main func_ov070_0211fd98
 *   data_ov070_021235ac  chase    init func_ov070_0211fd60   main func_ov070_0211fae4
 *   data_ov070_021235cc  dive     init func_ov070_0211fa80   main func_ov070_0211f6e0
 *   data_ov070_0212358c  fire     init func_ov070_0211f5f0   main func_ov070_0211f48c
 *   data_ov070_021235dc  settle   init func_ov070_0211f694   main func_ov070_0211f62c
 *   data_ov070_021235bc  knocked  init func_ov070_0211f450   main func_ov070_0211f368
 * "dive" is the swoop at the player, "fire" the fireball spit, "settle" the
 * wind-down back to wander after a dive or when the player is lost, "knocked" a
 * short (3-frame) pop-up and tilt that ends in coins and death. */
extern daPropeller_Heyho_c::State data_ov070_0212359c;
/* The ROM PMF constants the six state records copy from (unlicensed .data):
 * init slots return int, main slots return void. */
extern int (daPropeller_Heyho_c::*data_ov070_021230e8)();
extern void (daPropeller_Heyho_c::*data_ov070_021230c8)();
extern int (daPropeller_Heyho_c::*data_ov070_021230d8)();
extern void (daPropeller_Heyho_c::*data_ov070_021230f0)();
extern int (daPropeller_Heyho_c::*data_ov070_021230c0)();
extern void (daPropeller_Heyho_c::*data_ov070_02123118)();
extern int (daPropeller_Heyho_c::*data_ov070_02123108)();
extern void (daPropeller_Heyho_c::*data_ov070_021230d0)();
extern int (daPropeller_Heyho_c::*data_ov070_02123110)();
extern void (daPropeller_Heyho_c::*data_ov070_021230e0)();
extern int (daPropeller_Heyho_c::*data_ov070_021230f8)();
extern void (daPropeller_Heyho_c::*data_ov070_02123100)();
extern "C" {
/* Parameter names keep this TU's @NNN. */
typedef void BgchInitAt_(void *self, dActor_c *a, int r, int h, Vector3_16 *p, Vector3_16 *q);
extern int FlyGuy_ChangeState(daPropeller_Heyho_c* c, daPropeller_Heyho_c::State* p);
}

/* Loads the model (shared file 02123530) and six animation files, then seeds:
 * mCanSpitFire from the low byte of param1 (0xff counts as 0), the terminal fall
 * speed, the collider (dCcAc_c radius 60.0 / height 50.0; dBgCh_Actr::Init gets
 * 80.0 and 60.0), mCoinType and mExtraCoinCount = 1, and the home
 * position = the spawn position, then enters the wander state. */
int daPropeller_Heyho_c::InitResources()
{
    mAnmModel.SetFile((BMD_File *)dExtSimpleModel_c::LoadFile(data_ov070_02123530), 1, -1);
    mShadowModel.InitCylinder();
    dExtFrameCtrl_c::LoadFile(data_ov070_02123520);
    dExtFrameCtrl_c::LoadFile(data_ov070_02123518);
    dExtFrameCtrl_c::LoadFile(data_ov070_02123510);
    dExtFrameCtrl_c::LoadFile(data_ov070_02123528);
    dExtFrameCtrl_c::LoadFile(data_ov070_02123508);
    dExtFrameCtrl_c::LoadFile(data_ov070_02123500);
    mCanSpitFire = param1 & 0xff;
    if (mCanSpitFire == 0xff) mCanSpitFire = 0;
    mTerminalVelocity = -0x1e000; /* -30.0 units/frame */
    mdCcAc_c.Init(this, 0x3c000, 0x32000, 0x200000, 0x7eff0);
    mWithMeshClsn.Init(this, 0x50000, 0x3c000, 0, 0);
    mCoinType = 1;
    mExtraCoinCount = 1;
    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    FlyGuy_ChangeState(this, &data_ov070_0212359c);
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN19daPropeller_Heyho_c8BehaviorEv

extern daPropeller_Heyho_c::State data_ov070_021235cc;
extern daPropeller_Heyho_c::State data_ov070_021235bc;
extern "C" {
extern unsigned short DecIfAbove0_Short(unsigned short *p);
extern void func_ov070_02120070(daPropeller_Heyho_c *c);
extern void func_ov070_0211f100(daPropeller_Heyho_c *c);
}

/* Per-frame update. In order: Yoshi-mouth handling, then the death sequence
 * (mDeathState != 0), then the current state's main handler, gravity clamped at
 * the terminal speed, movement and wall collision, the mPrevAngle Y/Z -> mAngle
 * Y/Z copy (skipped in the knocked state; mAngleX is not copied), the draw
 * matrices, the hit reaction (skipped in the knocked state), the collider refresh
 * (only while a non-vanished player exists) and the animation advance at speed
 * 1.0. mStateTimer is counted down every frame except in the dive state, whose
 * main handler decrements it only in some conditions (see there); mCooldown is
 * counted down every frame. */
int daPropeller_Heyho_c::Behavior()
{
    if (UpdateYoshiEat(mWithMeshClsn) != 0) {
        mdCcAc_c.Clear();
        if (mEatenByYoshi != 0) {
            if (mSpitOutTimer == 0) {
                mdCcAc_c.Update();
            }
        }
        func_ov070_02120070(this);
        return 1;
    }

    if (mDeathState != 0) {
        UpdateDeath(mWithMeshClsn);
        func_ov070_02120070(this);
        return 1;
    }

    if (mCurrentState != &data_ov070_021235cc) {
        DecIfAbove0_Short((unsigned short *)&mStateTimer);
    }
    DecIfAbove0_Short(&mCooldown);

    {
        State *q = mCurrentState;
        /* Reads the handler's pointer word directly rather than as `&q->mMain`:
           taking the ADDRESS of a pointer-to-member makes mwcc materialise the
           whole 8-byte pmf. Reading one to CALL it is free. */
        if (*(int *)((char *)q + 8) != 0) {
            (this->*(q->mMain))();
        }
    }

    {
        /* Gravity, clamped at terminal velocity. mSpeedZ is read and written
           back unchanged -- the ROM really does reload and restore it here. */
        int v = mVertSpeed + mVertAccel;
        int hi = mTerminalVelocity;
        if (v >= hi) {
            hi = v;
        }
        int tmp = mSpeedZ;
        mVertSpeed = hi;
        mSpeedZ = tmp;
    }

    UpdatePosWithOnlySpeed(&mdCcAc_c);
    UpdateWMClsn(mWithMeshClsn, 0);

    if (mCurrentState != &data_ov070_021235bc) {
        mAngleY = mPrevAngleY;
        mAngleZ = mPrevAngleZ;
    }

    func_ov070_02120070(this);

    if (mCurrentState != &data_ov070_021235bc) {
        func_ov070_0211f100(this);
    }

    mdCcAc_c.Clear();
    {
        Player *p = ClosestPlayer();
        if (p != 0 && p->mIsVanish == 0) {
            mdCcAc_c.Update();
        }
    }

    mAnmModel.speed = 0x1000;
    mAnmModel.Advance();
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN19daPropeller_Heyho_c6RenderEv

/* Draws the model unless mFlags bit 0x40000 is set (dActor_c.h lists 0x020000
 * and 0x040000 as the yoshi-mouth state bits). */
int daPropeller_Heyho_c::Render()
{
    if (IsInYoshiMouth(this))
        return 1;
    mAnmModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN19daPropeller_Heyho_c16OnPendingDestroyEv
/* Empty. Four bytes, bx lr. */

void daPropeller_Heyho_c::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN19daPropeller_Heyho_c16CleanupResourcesEv
/* Releases the seven files InitResources claimed. The body does not read this. */


int daPropeller_Heyho_c::CleanupResources()
{
    data_ov070_02123530.Release();
    data_ov070_02123520.Release();
    data_ov070_02123518.Release();
    data_ov070_02123510.Release();
    data_ov070_02123528.Release();
    data_ov070_02123508.Release();
    data_ov070_02123500.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov070_02120070
#include "common.h"

extern "C" {

extern void Vec3_Asr(void* d, void* s, int sh);
extern void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void* m, int x, int y, int z);
extern int data_020a0e68[];

typedef struct { int w[12]; } M48;

/* Refreshes the draw matrices: the model matrix from position >> 3 (Vec3_Asr by 3)
 * and the three angles, copied into mAnmModel.mat4x3; the shadow matrix from the
 * position with Y lowered by 10.0 units, then DropShadowRadHeight(radius 118.0,
 * depth 800.0, opacity 15). Behavior calls it on every exit path. */
void func_ov070_02120070(daPropeller_Heyho_c* c)
{
    Vector3 v;
    Vec3_Asr(&v, &c->mPosX, 3);
    Matrix4x3_FromTranslation(data_020a0e68, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(data_020a0e68, c->mAngleX, c->mAngleY, c->mAngleZ);
    *(M48 *)&c->mAnmModel.mat4x3 = *(M48 *)data_020a0e68;
    Matrix4x3_FromTranslation(data_020a0e68, c->mPosX >> 3, (c->mPosY - 0xa000) >> 3, c->mPosZ >> 3);
    *(M48*)c->mShadowMatrix = *(M48*)data_020a0e68;
    c->DropShadowRadHeight(c->mShadowModel, *(Matrix4x3 *)c->mShadowMatrix, 0x76000, 0x320000, 0xf);
}
}

/* -------------------------------------------------------------------------- */
// @symbol FlyGuy_ChangeState
/* Installs a state record and runs its init handler if it has one. Returns the
 * init's result, or 1 with no init. */
extern "C" int FlyGuy_ChangeState(daPropeller_Heyho_c *c, daPropeller_Heyho_c::State *p)
{
    c->mCurrentState = p;
    daPropeller_Heyho_c::State *q = c->mCurrentState;
    if (q->mInit == 0)
        return 1;
    return (c->*(q->mInit))();
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov070_0211ffa8
extern "C" {
extern unsigned int RandomIntInternal(void* s);
extern int data_0209e650[];
/* Wander init: heading = a random multiple of 0x1000 (one of 16 directions,
 * 22.5 degrees apart), timer = 50 + random 0..31 frames, plays the file-02123520
 * animation at speed 1.0. */
int func_ov070_0211ffa8(daPropeller_Heyho_c* c){
    c->mTargetAngY = (short)(((RandomIntInternal(data_0209e650) >> 8) & 0xf) << 0xc);
    c->mStateTimer = (short)(((RandomIntInternal(data_0209e650) >> 8) & 0x1f) + 0x32);
    c->mAnmModel.SetAnim((BCA_File *)data_ov070_02123520.filePtr, 0, 0x1000, 0);
    return 1;
}
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov070_0211fd98
extern "C" {
typedef short s16;

extern int Vec3_Dist(void *a, void *b);
extern short Vec3_HorzAngle(void *a, void *b);
extern void ApproachAngle(s16 *dst, s16 target, int a, int b, int c);
extern short Vec3_VertAngle(void *a, void *b);
extern void Matrix4x3_FromRotationY(void *m, int angle);
extern void Matrix4x3_ApplyInPlaceToRotationX(void *m, short ang);
extern void MulVec3Mat4x3(void *in, void *m, void *out);
extern int FlyGuy_ChangeState(daPropeller_Heyho_c *c, daPropeller_Heyho_c::State *p);

extern int data_020a0e68[];
extern daPropeller_Heyho_c::State data_ov070_0212359c;
extern daPropeller_Heyho_c::State data_ov070_021235ac;

/* Wander main. Heads back toward home (keeping at least 20 frames on the timer)
 * when more than 500 units from it or touching a wall. Smooths the mPrevAngle
 * triple toward the heading (Y), the climb angle to home (X, linear) and a bank of
 * half the turn (Z), then eases the 0x0a4 / mVertSpeed / 0x0ac velocity words
 * toward a 10.0-unit/frame forward vector rotated by mAngleY and the X tilt. When
 * the timer reaches 0 the state restarts (new heading). With the cooldown expired
 * and the actor within 1500 units of home, a non-vanished player within 1000 units
 * starts the chase. */
int func_ov070_0211fd98(daPropeller_Heyho_c *c)
{
    int in[3];
    int out[3];
    Vector3 t;
    Player *p;

    in[0] = 0; in[1] = 0; in[2] = 0;
    out[0] = 0; out[1] = 0; out[2] = 0;

    if (Vec3_Dist(&c->mPosX, &c->mHomePosX) > kWanderTurnBackDist ||
        c->mWithMeshClsn.IsOnWall()) {
        c->mTargetAngY = Vec3_HorzAngle(&c->mPosX, &c->mHomePosX);
        if ((u16)c->mStateTimer < 0x14)
            c->mStateTimer = 0x14;
    }
    ApproachAngle(&c->mPrevAngleY, c->mTargetAngY, 0xa, 0x200, 0x100);

    ApproachLinear(c->mPrevAngleX, Vec3_VertAngle(&c->mPosX, &c->mHomePosX), 0x100);

    ApproachAngle(&c->mPrevAngleZ,
                  (c->mPrevAngleY - c->mTargetAngY) / 2,
                  0xa, 0x100, 0x50);

    in[2] = 0xa000;
    Matrix4x3_FromRotationY(data_020a0e68, c->mAngleY);
    Matrix4x3_ApplyInPlaceToRotationX(data_020a0e68, c->mPrevAngleX);
    MulVec3Mat4x3(in, data_020a0e68, out);

    ApproachLinear(c->mSpeedX, out[0], 0x1000);
    ApproachLinear(c->mVertSpeed, out[1], 0x800);
    ApproachLinear(c->mSpeedZ, out[2], 0x1000);

    if ((u16)c->mStateTimer == 0) {
        FlyGuy_ChangeState(c, &data_ov070_0212359c);
        return 1;
    }
    if (c->mCooldown != 0)
        return 1;
    if (Vec3_Dist(&c->mPosX, &c->mHomePosX) < kHomeLeashDist) {
        p = c->ClosestNonVanishPlayer();
        if (p) {
            int *pos = (int *)&p->mPosX; /* int view is load-bearing: plain p->mPosX reads change this function's size */
            t.x = pos[0];
            t.y = pos[1];
            t.z = pos[2];
            if (Vec3_Dist(&c->mPosX, &t) < kChaseTriggerDist)
                FlyGuy_ChangeState(c, &data_ov070_021235ac);
        }
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov070_0211fd60
extern "C" {
/* (data_ov070_02123520: SharedFilePtr view declared earlier in this TU) */
/* Chase init: plays the file-02123520 animation (the wander one) at speed 1.0. */
int func_ov070_0211fd60(daPropeller_Heyho_c *p) {
    p->mAnmModel.SetAnim((BCA_File *)(((void **)&data_ov070_02123520)[1]), 0, 0x1000, 0);
    return 1;
}
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov070_0211fae4
extern "C" {

extern short Vec3_HorzAngle(void *v0, void *v1);
extern short Vec3_VertAngle(void *v1, void *v0);
extern void ApproachAngle(s16 *cur, s16 target, int a, int b, int c);
extern void Matrix4x3_FromRotationY(void *m, int ang);
extern void Matrix4x3_ApplyInPlaceToRotationX(void *m, short ang);
extern void MulVec3Mat4x3(void *a, void *m, void *out);
extern int Vec3_Dist(void *a, void *b);
extern unsigned int RandomIntInternal(void *seed);
extern int FlyGuy_ChangeState(daPropeller_Heyho_c *c, daPropeller_Heyho_c::State *p);

extern signed char data_0209f2f8;
extern daPropeller_Heyho_c::State data_ov070_021235dc;
extern int data_020a0e68[];
extern daPropeller_Heyho_c::State data_ov070_0212359c;
extern int data_0209e650[];
extern daPropeller_Heyho_c::State data_ov070_021235cc;
extern daPropeller_Heyho_c::State data_ov070_0212358c;

/* Chase main. With no non-vanished player: re-anchors home to the current position
 * plus 200 units up (not when LEVEL_ID == 0x16), puts the position back to last
 * frame's, clears the X tilt, timer and vertical speed and goes to the settle
 * state. Otherwise aims at the point 200 units above the player: heading and tilt
 * ease toward it, and the velocity words are set directly to a 15.0-unit/frame
 * forward vector. Beyond 1500 units from home it gives up (wander); within 600
 * units of the aim point (900 when LEVEL_ID == 0x16) it zeroes the velocity words
 * and picks the dive or, when mCanSpitFire is non-zero and a random bit is 1, the
 * fire state. */
int func_ov070_0211fae4(daPropeller_Heyho_c *c)
{
    Player *player;
    Vector3 vin;
    Vector3 vb;
    Vector3 vc;
    Vector3 vd;

    player = c->ClosestNonVanishPlayer();
    if (player == 0) {
        if (data_0209f2f8 != kLevel0x16) {
            c->mHomePosX = c->mPosX;
            c->mHomePosY = c->mPosY;
            c->mHomePosZ = c->mPosZ;
            c->mHomePosY += kHomeLiftEscape;
        }
        c->mPosX = c->mPrevPosX;
        c->mPosY = c->mPrevPosY;
        c->mPosZ = c->mPrevPosZ;
        c->mPrevAngleX = 0;
        c->mStateTimer = 0;
        c->mVertSpeed = 0;
        FlyGuy_ChangeState(c, &data_ov070_021235dc);
        return 1;
    }

    vin.x = 0;
    vin.y = 0;
    vin.z = 0;

    {
    int *q = (int *)&player->mPosX; /* int view is load-bearing: plain player->mPosX reads change this function's size */
    vb.x = q[0];
    vb.y = q[1];
    vb.z = q[2];
    }
    vb.y += kAimAbovePlayer;
    vc.x = vb.x;
    vc.y = vb.y;
    vc.z = vb.z;

    c->mTargetAngY = Vec3_HorzAngle(&c->mPosX, &vc);
    ApproachAngle(&c->mPrevAngleY, c->mTargetAngY, 1, 0x500, 0x500);

    vd.x = vb.x;
    vd.y = vb.y;
    vd.z = vb.z;
    ApproachAngle(&c->mPrevAngleX, Vec3_VertAngle(&c->mPosX, &vd), 1, 0x500, 0x500);

    ApproachAngle(&c->mPrevAngleZ,
                  (c->mPrevAngleY - c->mTargetAngY) / 2,
                  0xa, 0x100, 0x50);

    vin.z = 0xf000;
    Matrix4x3_FromRotationY(data_020a0e68, c->mAngleY);
    Matrix4x3_ApplyInPlaceToRotationX(data_020a0e68, c->mPrevAngleX);
    MulVec3Mat4x3(&vin, data_020a0e68, &c->mSpeedX);

    if (Vec3_Dist(&c->mPosX, &c->mHomePosX) > kHomeLeashDist) {
        FlyGuy_ChangeState(c, &data_ov070_0212359c);
        return 1;
    }

    {
    int dist = Vec3_Dist(&c->mPosX, &vb);
    int thresh = kArriveDist;
    if (data_0209f2f8 == kLevel0x16) thresh = kArriveDistLevel0x16;
    if (dist < thresh) {
        c->mSpeedX = 0;
        c->mVertSpeed = 0;
        c->mSpeedZ = 0;
        if (c->mCanSpitFire == 0 ||
            (((unsigned int)RandomIntInternal(&data_0209e650) >> 8) & 1) == 0) {
            FlyGuy_ChangeState(c, &data_ov070_021235cc);
        } else {
            FlyGuy_ChangeState(c, &data_ov070_0212358c);
        }
    }
    }

    return 1;
}
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov070_0211fa80
extern "C" {
/* Dive init: clears mHitDuringAttack and mStateStep, timer = 63 frames, heading
 * from HorzAngleToCPlayerOrAng, plays the file-02123518 animation (flags
 * 0x40000000) at speed 1.0. */
int func_ov070_0211fa80(daPropeller_Heyho_c *c) {
    c->mHitDuringAttack = 0;
    c->mStateTimer = 0x3f;
    c->mStateStep = 0;
    c->mTargetAngY = c->HorzAngleToCPlayerOrAng();
    c->mAnmModel.SetAnim((BCA_File *)data_ov070_02123518.filePtr, 0x40000000, 0x1000, 0);
    return 1;
}
}

/* ApproachAngle, int-target view (byte-load-bearing: the target is not
 * narrowed to short). The file-scope C declarations take a short target. A
 * block-scope extern inside a member function gets C++ linkage and names a
 * symbol nothing defines, so the view is a C-linkage redeclaration in its own
 * namespace, used by func_ov070_0211f6e0 and func_ov070_0211f48c. */
namespace ApproachAngleInt {
extern "C" int ApproachAngle(s16 *angle, int target, int step, int maxDelta, int minDelta);
}

/* -------------------------------------------------------------------------- */
extern "C" {
typedef int s32;
typedef short s16;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef signed char s8;


/* (data_ov070_02123510: SharedFilePtr view declared earlier in this TU) */
extern s8 data_0209f2f8;
extern daPropeller_Heyho_c::State data_ov070_0212359c;
extern daPropeller_Heyho_c::State data_ov070_021235dc;
extern s32 data_0209f32c;
extern int data_020a0e68[];

/* (ApproachAngle: this file's own int-target view, namespace ApproachAngleInt above) */
extern int FlyGuy_ChangeState(daPropeller_Heyho_c* c, daPropeller_Heyho_c::State* p);
extern short Vec3_VertAngle(void* v1, void* v0);
extern int Vec3_Dist(void* a, void* b);
extern u16 DecIfAbove0_Short(u16* p);
extern void Matrix4x3_FromRotationY(void* m, int angle);
extern void Matrix4x3_ApplyInPlaceToRotationX(void* mF, s16 angX);
extern void MulVec3Mat4x3(void* v, void* m, void* res);
}

// @symbol _ZN19daPropeller_Heyho_c19func_ov070_0211f6e0Ev
/* Dive main. Turns toward mTargetAngY and levels the Z bank. When the animation
 * has finished: the first time (mStateStep == 0) it starts the file-02123510
 * animation and sets mStateStep = 1; if mHitDuringAttack == 1 (set by the hit
 * reaction when a contact hurt the player) it raises home by 300 units (not when
 * LEVEL_ID == 0x16), starts the 90-frame cooldown and returns to wander.
 * It goes to the settle state, re-anchoring home at the current position (not
 * when LEVEL_ID == 0x16) with the X tilt cleared, when the timer has run out, the
 * actor touches a wall, or there is no non-vanished player. If the player is
 * flagged underwater and the water height (data_0209f32c) is above this actor's
 * Y, it instead re-anchors home 200 units above the current position (not when
 * LEVEL_ID == 0x16), restores last frame's position and zeroes the timer and
 * vertical speed before settling. Otherwise it tilts toward a point 71.0 units
 * (50.0 when LEVEL_ID == 0x16) above the player's mGroundY and sets the velocity
 * words to a 17.0-unit/frame forward vector; while at or below that point + 5.0,
 * at or below the player's Y + 5.0, or beyond 1500 units from home, the timer
 * counts down and the speed drops to 9.0. */
int daPropeller_Heyho_c::func_ov070_0211f6e0()
{
    Player* player;
    Vector3 tmp;
    Vector3 v;
    Vector3 aim;
    s16 vAngle;
    s16 half;
    s32 z;

    ApproachAngleInt::ApproachAngle(&mPrevAngleY, mTargetAngY, 0x100, 0x1000, 0x1000);
    ApproachAngleInt::ApproachAngle(&mPrevAngleZ, 0, 0x100, 0x1000, 0x1000);

    if (mAnmModel.dExtFrameCtrl_c::Finished()) {
        if (mStateStep == 0) {
            mAnmModel.SetAnim((BCA_File *)data_ov070_02123510.filePtr, 0, 0x1000, 0);
            mStateStep = 1;
        }
        if (mHitDuringAttack == 1) {
            if (data_0209f2f8 != kLevel0x16)
                mHomePosY += kHomeLiftRecover;
            mStateStep = 0;
            mCooldown = kAttackCooldown;
            FlyGuy_ChangeState(this, &data_ov070_0212359c);
            return 1;
        }
    }

    if ((u16)mStateTimer == 0 || mWithMeshClsn.IsOnWall()) {
        if (data_0209f2f8 != kLevel0x16) {
            mHomePosX = mPosX;
            mHomePosY = mPosY;
            mHomePosZ = mPosZ;
        }
        mPrevAngleX = 0;
        FlyGuy_ChangeState(this, &data_ov070_021235dc);
        return 1;
    }

    player = ClosestNonVanishPlayer();
    if (player == 0) {
        if (data_0209f2f8 != kLevel0x16) {
            mHomePosX = mPosX;
            mHomePosY = mPosY;
            mHomePosZ = mPosZ;
        }
        mPrevAngleX = 0;
        FlyGuy_ChangeState(this, &data_ov070_021235dc);
        return 1;
    }

    if (player->mIsUnderwater != 0 && data_0209f32c > mPosY) {
        if (data_0209f2f8 != kLevel0x16) {
            mHomePosX = mPosX;
            mHomePosY = mPosY;
            mHomePosZ = mPosZ;
            mHomePosY += kHomeLiftEscape;
        }
        mPosX = mPrevPosX;
        mPosY = mPrevPosY;
        mPosZ = mPrevPosZ;
        mPrevAngleX = 0;
        mStateTimer = 0;
        mVertSpeed = 0;
        FlyGuy_ChangeState(this, &data_ov070_021235dc);
        return 1;
    }

    *(V3w*)&tmp = *(V3w*)&player->mPosX;  /* array-wrapper keeps the ldm/stm block copy under -lang c++ */
    z = 0;
    v.x = z;
    v.y = z;
    v.z = z;
    tmp.y = player->mGroundY;
    if (data_0209f2f8 == kLevel0x16)
        tmp.y += 0x32000;
    else
        tmp.y += 0x47000;
    {
        int tx = tmp.x;
        int ty = tmp.y;
        int tz = tmp.z;
        aim.x = tx;
        aim.y = ty;
        aim.z = tz;
    }
    vAngle = Vec3_VertAngle(&mPosX, &aim);
    ApproachAngleInt::ApproachAngle(&mPrevAngleX, vAngle, 0xa, 0x200, 0x100);

    v.z = 0x11000;
    if (mPosY <= tmp.y + 0x5000 ||
        mPosY <= player->mPosY + 0x5000 ||
        Vec3_Dist(&mPosX, &mHomePosX) > kHomeLeashDist) {
        DecIfAbove0_Short((u16*)&mStateTimer);
        v.z = 0x9000;
    }

    half = (mPrevAngleY - mTargetAngY) / 2;
    ApproachAngleInt::ApproachAngle(&mPrevAngleZ, half, 0xa, 0x100, 0x50);

    Matrix4x3_FromRotationY(data_020a0e68, mAngleY);
    Matrix4x3_ApplyInPlaceToRotationX(data_020a0e68, mPrevAngleX);
    MulVec3Mat4x3(&v, data_020a0e68, &mSpeedX);

    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov070_0211f694
extern "C" {
/* Settle init: mStateStep = 0; plays the file-02123508 animation (flags
 * 0x40000000) unless mHitDuringAttack is set. */
int func_ov070_0211f694(daPropeller_Heyho_c *c) {
    c->mStateStep = 0;
    if (c->mHitDuringAttack == 0) {
        c->mAnmModel.SetAnim((BCA_File *)data_ov070_02123508.filePtr, 0x40000000, 0x1000, 0);
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
extern "C" {
extern signed char data_0209f2f8;
extern int FlyGuy_ChangeState(daPropeller_Heyho_c *c, daPropeller_Heyho_c::State *p);
extern daPropeller_Heyho_c::State data_ov070_0212359c;
}

// @symbol _ZN19daPropeller_Heyho_c19func_ov070_0211f62cEv
/* Settle main: once the animation has finished, raises home by 300 units (not
 * when LEVEL_ID == 0x16), clears mStateStep, starts the 90-frame cooldown and
 * returns to wander. */
int daPropeller_Heyho_c::func_ov070_0211f62c()
{
    if (mAnmModel.dExtFrameCtrl_c::Finished() != 0) {
        if (data_0209f2f8 != kLevel0x16)
            mHomePosY += kHomeLiftRecover;
        mStateStep = 0;
        mCooldown = kAttackCooldown;
        FlyGuy_ChangeState(this, &data_ov070_0212359c);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov070_0211f5f0
struct BCA_File;
/* (dExtAnmModel_c: real header type in scope) */
/* SetAnim takes Fix12<int> by value, so this call stays the mangled extern. */

/* (data_ov070_02123500: SharedFilePtr view declared earlier in this TU) */

/* Fire init: plays the file-02123500 animation (flags 0x40000000) at speed 1.0. */
extern "C" int func_ov070_0211f5f0(daPropeller_Heyho_c *c) {
    unsigned int flags = 0;
    BCA_File *file = (BCA_File *)data_ov070_02123500.filePtr;
    c->mAnmModel.SetAnim((BCA_File *)file, 0x40000000, 0x1000, flags);
    return 1;
}

/* -------------------------------------------------------------------------- */
extern "C" {
short Vec3_HorzAngle(void* a, void* b);
/* (ApproachAngle: this file's own int-target view, namespace ApproachAngleInt above) */
short Vec3_VertAngle(void* a, void* b);
void func_02012694(int a, void* p);
int FlyGuy_ChangeState(daPropeller_Heyho_c* c, daPropeller_Heyho_c::State* p);
extern daPropeller_Heyho_c::State data_ov070_0212359c;
}

#define M(p) (p)

// @symbol _ZN19daPropeller_Heyho_c19func_ov070_0211f48cEv
/* Fire main. While the animation frame (integer part of the 20.12 counter) is
 * below 13 it steers mPrevAngleY toward mTargetAngY, which is refreshed from the
 * nearest player when there is one. When the animation is about to hit frame 13
 * (WillHitFrame) it spawns the fireball at its own position (SpawnFireball with
 * 30.0 / 10.0 in its two Fix12 slots and param1 = 1) with a rotation copied from
 * mAngleX/Y/Z whose X is the vertical angle to the player, and plays sound 0x105.
 * When the animation has finished it resets the frame to 0, starts the 90-frame
 * cooldown and returns to wander. */
int daPropeller_Heyho_c::func_ov070_0211f48c() {
    Player* pl;
    struct Vector3_16 vel;
    struct Vector3 posbuf;
    struct Vector3 fp;
    struct Vector3 tmp;

    pl = ClosestPlayer();
    if ((unsigned)(mAnmModel.currFrame << 4) >> 0x10 < kFireFrame) {
        if (pl != 0) {
            *(V3w *)&posbuf = *(V3w *)&pl->mPosX;
            tmp.x = posbuf.x;
            tmp.y = posbuf.y;
            tmp.z = posbuf.z;
            mTargetAngY = Vec3_HorzAngle(&mPosX, &tmp);
        }
        ApproachAngleInt::ApproachAngle(&mPrevAngleY, mTargetAngY, 0xa, 0x400, 0x200);
    }

    if (mAnmModel.dExtFrameCtrl_c::WillHitFrame(kFireFrame) != 0) {
        *(V3h*)&vel = *(V3h*)&mAngleX;
        if (pl != 0) {
            int *base = (int *)(int)M(&pl->mPosX);
            fp.x = base[0];
            fp.y = base[1];
            fp.z = base[2];
            vel.x = Vec3_VertAngle(&mPosX, &fp);
        }
        SpawnFireball(*(const Vector3 *)&mPosX, &vel, 0x1e000, 0xa000, 1);
        func_02012694(kSndFireSpit, &mCamSpacePosX);
    }
    if (mAnmModel.dExtFrameCtrl_c::Finished() != 0) {
        mAnmModel.currFrame = 0;
        mCooldown = kAttackCooldown;
        FlyGuy_ChangeState(this, &data_ov070_0212359c);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov070_0211f450
extern "C" {
/* Knocked init: zeroes the 0x0a4 / mVertSpeed / 0x0ac velocity words, then
 * mVertSpeed = +50.0 units/frame (upward pop), mVertAccel = -5.0, timer = 3
 * frames and mFlags = 0. */
short func_ov070_0211f450(daPropeller_Heyho_c *c) {
    c->mSpeedX = 0;
    c->mVertSpeed = 0;
    c->mSpeedZ = 0;
    c->mVertSpeed = 0x32000;
    c->mVertAccel = -0x5000;
    c->mStateTimer = 3;
    c->mFlags = 0;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov070_0211f368
extern "C" int func_ov070_0211f0a4(daPropeller_Heyho_c *c);
typedef int Fix12i;

struct Vector3_16f;
extern "C" unsigned d_a_propeller_heyhoNewHold(unsigned a, unsigned b, Fix12i c, Fix12i d, Fix12i e,
                                               void *f, void *g);
extern "C" u32 d_a_propeller_heyhoNewUnkCallback818Hold(u32 a, u32 b, Fix12i c, Fix12i d, Fix12i e,
                                                        const Vector3_16f *f);
extern "C" void ApproachAngle(short* v, short a, int b, int c, int d);

/* Knocked main. While mStateStep is non-zero (normally a fire hit; the
 * mega-character hit leaves it unchanged) it refreshes the effects 0x13a and
 * 0x13b at the position + 80.0 units up each frame (their handles are fed back). Tilts mAngleX toward
 * -0x4000 (a quarter turn) with ApproachAngle and then ApproachLinear. When the
 * timer reaches 0 it runs the defeat helper func_ov070_0211f0a4. */
extern "C" int func_ov070_0211f368(daPropeller_Heyho_c* c)
{
    if (c->mStateStep != 0) {
        Vector3 v;
        int x, y, z;
        x = c->mPosX;
        z = c->mPosZ;
        y = c->mPosY + 0x50000;
        ((int*)&v)[0] = x;
        ((int*)&v)[1] = y;
        ((int*)&v)[2] = z;
        c->mParticle0 = Particle::System::New(c->mParticle0, 0x13a, v.x, v.y, v.z, 0, 0);
        c->mParticle1 = Particle::System::NewUnkCallback818(c->mParticle1, 0x13b, v.x, v.y, v.z, 0);
    }
    ApproachAngle(&c->mAngleX, -0x4000, 0xa, 0x200, 0x100);
    ApproachLinear(c->mAngleX, -0x4000, 0x200);
    if ((u16)c->mStateTimer == 0)
        func_ov070_0211f0a4(c);
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov070_0211f100
typedef int s32;
typedef short s16;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

struct BCA_File;

extern daPropeller_Heyho_c::State data_ov070_021235bc;
extern daPropeller_Heyho_c::State data_ov070_021235cc;
/* (data_ov070_02123528: SharedFilePtr view declared earlier in this TU) */

extern "C" {
extern int FlyGuy_ChangeState(daPropeller_Heyho_c* c, daPropeller_Heyho_c::State* p);
extern void func_ov002_020aea30(void *self, void *actor, void *collision);
extern void func_02012694(int a, void* b);
}

/* Hit reaction, run once per frame outside the knocked state. Takes the colliding
 * actor from the collider (mdCcAc_c.otherOwner) and reads hitFlags: fire sends it
 * to the knocked state with mStateStep = 1 (particles); 0x20 kills it through
 * func_ov002_020aea30 (verified.tsv spells that address Enemy::KillByAttack) with
 * mDeathState = 1 (name only; arity differs); the punch/kick/egg/explosion mask sends it to the knocked
 * state with mStateStep = 0. After that only the player actor counts, and only a
 * non-vanished one: JumpedOnByPlayer bounces the player (SpinBounce 40.0) and
 * kills as above; a mega-character hit spawns the mega particles, counts the
 * kill and plays sound 0x1d before the knocked state; a metal player or one on a
 * shell sends it to the knocked state with mStateStep = 0; any other contact
 * hurts the player (Player::Hurt) and, in the dive state with mHitDuringAttack
 * still 0, plays the file-02123528 animation and sets mStateStep =
 * mHitDuringAttack = 1. */
extern "C" void func_ov070_0211f100(daPropeller_Heyho_c* c)
{
    Player* hitPlayer;
    s32 hitFlags;

    if (c->mdCcAc_c.otherOwner == 0)
        return;
    hitPlayer = (Player *)dActor_c::FindWithID(c->mdCcAc_c.otherOwner);
    if (!hitPlayer)
        return;

    hitFlags = (s32)c->mdCcAc_c.hitFlags;
    if (hitFlags & kHitFire) {
        c->mStateStep = 1;
        FlyGuy_ChangeState(c, &data_ov070_021235bc);
        return;
    }
    if (hitFlags & kHitSpinPound) {
        c->mDeathState = 1;
        func_ov002_020aea30(c, hitPlayer, 0);
        return;
    }
    if (hitFlags & kHitAttackMask) {
        c->mStateStep = 0;
        FlyGuy_ChangeState(c, &data_ov070_021235bc);
        return;
    }

    if (!IsPlayerActor(hitPlayer))
        return;
    if (hitPlayer->mIsVanish != 0)
        return;

    if (c->JumpedOnByPlayer(c->mdCcAc_c, *hitPlayer)) {
        hitPlayer->SpinBounce(0x28000);
        c->mDeathState = 1;
        func_ov002_020aea30(c, hitPlayer, 0);
        return;
    }

    if (hitFlags & kHitMegaChar) {
        c->SpawnMegaCharParticles(*hitPlayer, (char*)0);
        hitPlayer->IncMegaKillCount();
        func_02012694(kSndMegaKill, &c->mCamSpacePosX);
        FlyGuy_ChangeState(c, &data_ov070_021235bc);
        return;
    }

    if (hitPlayer->mIsMetal == 1 || hitPlayer->IsOnShell() == 1) {
        c->mStateStep = 0;
        FlyGuy_ChangeState(c, &data_ov070_021235bc);
        return;
    }

    {
        Vector3 v;
        v.x = c->mPosX;
        v.y = c->mPosY;
        v.z = c->mPosZ;
        hitPlayer->Hurt(v, 2, 0xc000, 1, 0, 1);
    }
    if (c->mHitDuringAttack != 0)
        return;
    if (c->mCurrentState != &data_ov070_021235cc)
        return;

    c->mAnmModel.SetAnim((BCA_File *)data_ov070_02123528.filePtr, 0x40000000, 0x1000, 0);
    c->mStateStep = 1;
    c->mHitDuringAttack = 1;
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov070_0211f0a4


/* (dActor_c: real header type in scope; SpawnCoins goes through the mangled extern below) */
/* SpawnCoins takes Fix12<int> by value, so this call stays the mangled extern. */

/* Defeat helper: a small puff of dust, then mExtraCoinCount + 1 coins spread
 * 10.0 units from the actor, then the actor is killed and recorded in the
 * death table. */
extern "C" int func_ov070_0211f0a4(daPropeller_Heyho_c *c) {
    c->SmallPoofDust();
    Vector3 pos;
    pos.x = c->mPosX;
    pos.y = c->mPosY;
    pos.z = c->mPosZ;
    unsigned int coins = (unsigned char)c->mExtraCoinCount + 1;
    c->SpawnCoins(pos, coins, 0xa000, 0);
    c->KillAndTrackInDeathTable();
}

/* Static-init globals (was the handwritten __sinit_ov070_02122afc shard).
 * Definition order is the retail initializer's construction order: the
 * model handle (file 0x411), the six animation handles (files 0x417/0x412/
 * 0x413/0x414/0x415/0x416), then the six state records copied from the ROM
 * PMF constants. */
HeyhoModelFile data_ov070_02123530(ASSET_PROPELLER_HEYHO_BMD);
HeyhoAnimationFilePtr data_ov070_02123520(ASSET_PROPELLER_HEYHO_WAIT_BCA);
HeyhoAnimationFilePtr data_ov070_02123518(ASSET_PROPELLER_HEYHO_ATTACK01_BCA);
HeyhoAnimationFilePtr data_ov070_02123510(ASSET_PROPELLER_HEYHO_ATTACK02_BCA);
HeyhoAnimationFilePtr data_ov070_02123528(ASSET_PROPELLER_HEYHO_ATTACK03_BCA);
HeyhoAnimationFilePtr data_ov070_02123508(ASSET_PROPELLER_HEYHO_ATTACK04_BCA);
HeyhoAnimationFilePtr data_ov070_02123500(ASSET_PROPELLER_HEYHO_FIRE_ATTACK_BCA);
daPropeller_Heyho_c::State data_ov070_0212359c = {data_ov070_021230e8, data_ov070_021230c8};
daPropeller_Heyho_c::State data_ov070_021235ac = {data_ov070_021230d8, data_ov070_021230f0};
daPropeller_Heyho_c::State data_ov070_021235cc = {data_ov070_021230c0, data_ov070_02123118};
daPropeller_Heyho_c::State data_ov070_021235dc = {data_ov070_02123108, data_ov070_021230d0};
daPropeller_Heyho_c::State data_ov070_0212358c = {data_ov070_02123110, data_ov070_021230e0};
daPropeller_Heyho_c::State data_ov070_021235bc = {data_ov070_021230f8, data_ov070_02123100};
