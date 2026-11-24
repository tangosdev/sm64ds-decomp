//cpp
/* Fly Guy (PROPELLER_HEYHO), ov070. .text 0x0211f000..0x02120570,
 * inline destructor D1 then D0, plus the 25 functions below. It wanders
 * a home position, chases a player inside 1000 units, then dives or
 * (when param1's low byte is nonzero, half the time) spits a fireball.
 * HandleContact sorts contact. Source order is the reverse of the
 * ROM, so the one .text section emits ascending.
 */
/* leftovers: Finished and WillHitFrame stay qualified through mAnmModel; a char*
 * view of that member moves a word in St_Fire_Main,
 * St_Settle_Main and St_Dive_Main. daPly_c-position reads
 * through int* in St_Wander_Main, St_Chase_Main and
 * St_Fire_Main, and the Vector3 write through ((int *)&v)[i] in
 * St_Knocked_Main, stay as written. The mPrevAngle triple keeps
 * the base-class names.
 * FlyGuy_ChangeState keeps its C-ABI name. V3w keeps the ldm/stm copy in
 * St_Dive_Main and St_Fire_Main. The six state records and
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
#include "Particle__System.h"
#include "ParticleIds.h"
#include "SpawnInfo.h"
#include "math/Approach.h"
#include "LevelRng.h"
#include "math/CalcLib.h"

/* The cartridge preserves this descriptor's C-ABI symbol but not its source
 * spelling.  Keep that evidence-bounded alias; all seven scalar words and
 * the factory relocation are covered by the TU's data claim. */
daPropeller_Heyho_c *daPropeller_Heyho_c_classInit(void);

/* Reconstructed source-style name: SM64DS proves this descriptor through
 * its registry role, the PROPELLER_HEYHO literal ROM profile ID, and the
 * factory relocation it carries; later EAD lineage supplies g_profile_.
 * Exact original spelling is not preserved. Historical alias:
 * FlyGuy_SpawnInfo. */
SpawnInfo<daPropeller_Heyho_c> g_profile_PROPELLER_HEYHO = {daPropeller_Heyho_c_classInit,
                                                            0x00e8,
                                                            0x0057,
                                                            0x10000003,
                                                            0x00064000,
                                                            0x000c8000,
                                                            0x01000000,
                                                            0x01000000};

/* Kept for the two player-position snapshots (measured r57): any Vector3 copy is
 * memberwise; a `VecFx32 tmp =` copy-init stays in registers in 0211f6e0 and in
 * 0211f48c misses the ROM's posbuf < fp < tmp frame order.
 * Kept (#821 r71): `tmp = player->mPos` gives 0211f6e0 0x3a8 (ROM 0x3a0) and
 * `posbuf = pl->mPos` gives 0211f48c 0x160 (ROM 0x164).
 * Kept (#821 r76): re-measured, same sizes; mPos is a non-POD Vector3, so no copy of it
 * is one block. */
struct V3w {
    int w[3];
};

/* Named constants.  fx32 values are written as the raw word (0x1000 = 1.0)
 * with the unit value in the comment; frame counts are game-logic frames (the frame rate is not established here). */
enum {
    kLevel0x16 = 0x16, /* LEVEL_ID (gCurrentSublevel, declared as a byte here) value at which this
                                     code skips re-anchoring the home position and the +300.0 home lift,
                                     and uses the alternate dive aim height / arrival distance */
    kPlayerActorId = 191, /* PLAYER in symbols/actor_debug_names.tsv */

    kHomeLiftEscape =
        0xc8000, /* 200.0 units added to the home Y when the chase loses the player or the dive finds the player underwater */
    kHomeLiftRecover =
        0x12c000, /* 300.0 units added to the home Y when the settle, or a dive that hurt the player, finishes */
    kAimAbovePlayer = 0xc8000, /* 200.0 units: the chase aim point is this far above the player */
    kAttackCooldown = 0x5a,    /* 90 frames: mCooldown reload when an attack/fire/settle ends */

    kWanderTurnBackDist = 0x1f4000,  /* 500.0 units from home: wander turns back toward home */
    kHomeLeashDist = 0x5dc000,       /* 1500.0 units from home: the leash for wander/chase/dive */
    kChaseTriggerDist = 0x3e8000,    /* 1000.0 units: a player closer than this starts the chase */
    kArriveDist = 0x258000,          /* 600.0 units: chase "arrived" distance to the aim point */
    kArriveDistLevel0x16 = 0x384000, /* 900.0 units: the same distance when LEVEL_ID == 0x16 */

    kFireFrame = 0xd,     /* animation frame 13 of the spit animation: the fireball leaves here */
    kSndFireSpit = 0x105, /* sound id; daKrpa_c plays the same id for its spit */
    kSndMegaKill = 0x1d,  /* sound id played with IncMegaKillCount */

    /* dCc_c hitFlags bits, read with the table in include/dCc_c.h (only 0x2000 egg and
       0x4000 explosion are proven there; the rest are its plausible reading) */
    kHitMegaChar = 0x10,
    kHitSpinPound = 0x20,
    kHitFire = 0x40000,
    kHitAttackMask = 0x67c0 /* 0x40 punch | 0x80 kick | 0x100 breakdance | 0x200 slide kick
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
/* Registry factory for the PROPELLER_HEYHO profile. */
daPropeller_Heyho_c *daPropeller_Heyho_c_classInit(void) {
    return new daPropeller_Heyho_c();
}

/* -------------------------------------------------------------------------- */
/* Returns 5 to Yoshi's swallow attempt; what the value means is not recovered. */
s32 daPropeller_Heyho_c::OnYoshiTryEat() {
    return 5;
}

/* -------------------------------------------------------------------------- */
/* Vtable slot 19. _ZTV19daPropeller_Heyho_c at 0x02123168 + 0x4c is this body. */
#include "daPly_c.h"

/* Turned into an egg: the player is paid mExtraCoinCount + 1 coins
 * (InitResources seeds mExtraCoinCount to 1, so 2 coins here), then this
 * actor is killed and recorded in the death table. */
void daPropeller_Heyho_c::OnTurnIntoEgg(daPly_c &player) {
    GivePlayerCoins(player, (u8)(mExtraCoinCount + 1), 0);
    KillAndTrackInDeathTable();
}

/* -------------------------------------------------------------------------- */
/* Egg-aim query. Returns the fixed height the slot stores. */
s32 daPropeller_Heyho_c::OnAimedAtWithEgg() {
    return 0x2b000; /* fx32 egg-aim HEIGHT added to pos.y, per dEnemyBase_c.h slot-29: 43.0 units */
}

/* -------------------------------------------------------------------------- */
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
 *   data_ov070_0212359c  wander   init St_Wander_Init   main St_Wander_Main
 *   data_ov070_021235ac  chase    init St_Chase_Init   main St_Chase_Main
 *   data_ov070_021235cc  dive     init St_Dive_Init   main St_Dive_Main
 *   data_ov070_0212358c  fire     init St_Fire_Init   main St_Fire_Main
 *   data_ov070_021235dc  settle   init St_Settle_Init   main St_Settle_Main
 *   data_ov070_021235bc  knocked  init St_Knocked_Init   main St_Knocked_Main
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

int FlyGuy_ChangeState(daPropeller_Heyho_c *c, daPropeller_Heyho_c::State *p);

/* Loads the model (shared file 02123530) and six animation files, then seeds:
 * mCanSpitFire from the low byte of param1 (0xff counts as 0), the terminal fall
 * speed, the collider (dCcAc_c radius 60.0 / height 50.0; dBgCh_Actr::Init gets
 * 80.0 and 60.0), mCoinType and mExtraCoinCount = 1, and the home
 * position = the spawn position, then enters the wander state. */
int daPropeller_Heyho_c::InitResources()
{
    mAnmModel.SetFile(dExtSimpleModel_c::LoadFile(data_ov070_02123530), 1, -1);
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
    mHomePosX = mPos.x;
    mHomePosY = mPos.y;
    mHomePosZ = mPos.z;
    FlyGuy_ChangeState(this, &data_ov070_0212359c);
    return 1;
}

/* -------------------------------------------------------------------------- */

extern daPropeller_Heyho_c::State data_ov070_021235cc;
extern daPropeller_Heyho_c::State data_ov070_021235bc;

/* Per-frame update. In order: Yoshi-mouth handling, then the death sequence
 * (mDeathState != 0), then the current state's main handler, gravity clamped at
 * the terminal speed, movement and wall collision, the mPrevAngle Y/Z -> mAngle
 * Y/Z copy (skipped in the knocked state; mAngle.x is not copied), the draw
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
        UpdateModelAndShadow();
        return 1;
    }

    if (mDeathState != 0) {
        UpdateDeath(mWithMeshClsn);
        UpdateModelAndShadow();
        return 1;
    }

    if (mCurrentState != &data_ov070_021235cc) {
        DecIfAbove0_Short(&mStateTimer);
    }
    DecIfAbove0_Short(&mCooldown);

    {
        State *q = mCurrentState;
        if (q->mMain != 0) {
            (this->*(q->mMain))();
        }
    }

    {
        /* Gravity, clamped at terminal velocity. mSpeed.z is read and written
           back unchanged -- the ROM really does reload and restore it here. */
        int v = mSpeed.y + mVertAccel;
        int hi = mTerminalVelocity;
        if (v >= hi) {
            hi = v;
        }
        int speedZ = mSpeed.z;
        mSpeed.y = hi;
        mSpeed.z = speedZ;
    }

    UpdatePosWithOnlySpeed(&mdCcAc_c);
    UpdateWMClsn(mWithMeshClsn, 0);

    if (mCurrentState != &data_ov070_021235bc) {
        mAngle.y = mPrevAngle.y;
        mAngle.z = mPrevAngle.z;
    }

    UpdateModelAndShadow();

    if (mCurrentState != &data_ov070_021235bc) {
        this->HandleContact();
    }

    mdCcAc_c.Clear();
    {
        daPly_c *p = ClosestPlayer();
        if (p != 0 && p->mIsVanish == 0) {
            mdCcAc_c.Update();
        }
    }

    mAnmModel.speed = 0x1000;
    mAnmModel.Advance();
    return 1;
}

/* -------------------------------------------------------------------------- */

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
/* Empty. Four bytes, bx lr. */

void daPropeller_Heyho_c::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
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
#include "common.h"
#include "math/MtxStack.h"


/* Refreshes the draw matrices: the model matrix from position >> 3 (Vec3_Asr by 3)
 * and the three angles, copied into mAnmModel.mat4x3; the shadow matrix from the
 * position with Y lowered by 10.0 units, then DropShadowRadHeight(radius 118.0,
 * depth 800.0, opacity 15). Behavior calls it on every exit path. */
void daPropeller_Heyho_c::UpdateModelAndShadow() {
    Vector3 v;
    Vec3_Asr(&v, &mPos, 3);
    Matrix4x3_FromTranslation(&gScratchMtx, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&gScratchMtx, mAngle.x, mAngle.y, mAngle.z);
    mAnmModel.mat4x3 = gScratchMtx;
    Matrix4x3_FromTranslation(&gScratchMtx, mPos.x >> 3, (mPos.y - 0xa000) >> 3, mPos.z >> 3);
    mShadowMatrix = gScratchMtx;
    DropShadowRadHeight(mShadowModel, mShadowMatrix, 0x76000, 0x320000, 0xf);
}

/* -------------------------------------------------------------------------- */
/* Installs a state record and runs its init handler if it has one. Returns the
 * init's result, or 1 with no init. */
int FlyGuy_ChangeState(daPropeller_Heyho_c *c, daPropeller_Heyho_c::State *p) {
    c->mCurrentState = p;
    daPropeller_Heyho_c::State *q = c->mCurrentState;
    if (q->mInit == 0)
        return 1;
    return (c->*(q->mInit))();
}

/* -------------------------------------------------------------------------- */

/* Wander init: heading = a random multiple of 0x1000 (one of 16 directions,
 * 22.5 degrees apart), timer = 50 + random 0..31 frames, plays the file-02123520
 * animation at speed 1.0. */
int daPropeller_Heyho_c::St_Wander_Init() {
    this->mTargetAngY =
        (short)((((unsigned int)RandomIntInternal(&data_0209e650.value) >> 8) & 0xf) << 0xc);
    this->mStateTimer =
        (short)((((unsigned int)RandomIntInternal(&data_0209e650.value) >> 8) & 0x1f) + 0x32);
    this->mAnmModel.SetAnim((BCA_File *)data_ov070_02123520.filePtr, 0, 0x1000, 0);
    return 1;
}

/* -------------------------------------------------------------------------- */

typedef short s16;
int FlyGuy_ChangeState(daPropeller_Heyho_c *c, daPropeller_Heyho_c::State *p);

extern daPropeller_Heyho_c::State data_ov070_0212359c;
extern daPropeller_Heyho_c::State data_ov070_021235ac;

/* Wander main. Heads back toward home (keeping at least 20 frames on the timer)
 * when more than 500 units from it or touching a wall. Smooths the mPrevAngle
 * triple toward the heading (Y), the climb angle to home (X, linear) and a bank of
 * half the turn (Z), then eases the mSpeed velocity words
 * toward a 10.0-unit/frame forward vector rotated by mAngle.y and the X tilt. When
 * the timer reaches 0 the state restarts (new heading). With the cooldown expired
 * and the actor within 1500 units of home, a non-vanished player within 1000 units
 * starts the chase. */
int daPropeller_Heyho_c::St_Wander_Main() {
    Vector3 in;
    Vector3 out;
    Vector3 t;
    daPly_c *p;

    in.x = 0;
    in.y = 0;
    in.z = 0;
    out.x = 0;
    out.y = 0;
    out.z = 0;

    if (Vec3_Dist(&this->mPos, &this->mHomePos) > kWanderTurnBackDist ||
        this->mWithMeshClsn.IsOnWall()) {
        this->mTargetAngY = Vec3_HorzAngle(&this->mPos, &this->mHomePos);
        if (this->mStateTimer < 20)
            this->mStateTimer = 20;
    }
    ApproachAngle(&this->mPrevAngle.y, this->mTargetAngY, 0xa, 0x200, 0x100);

    ApproachLinear(this->mPrevAngle.x, Vec3_VertAngle(&this->mPos, &this->mHomePos), 0x100);

    ApproachAngle(&this->mPrevAngle.z, (this->mPrevAngle.y - this->mTargetAngY) / 2, 0xa, 0x100,
                  0x50);

    in.z = 0xa000;
    Matrix4x3_FromRotationY(&gScratchMtx, this->mAngle.y);
    Matrix4x3_ApplyInPlaceToRotationX(&gScratchMtx, this->mPrevAngle.x);
    MTX_MultVec43(&in, &gScratchMtx, &out);

    ApproachLinear(this->mSpeed.x, out.x, 0x1000);
    ApproachLinear(this->mSpeed.y, out.y, 0x800);
    ApproachLinear(this->mSpeed.z, out.z, 0x1000);

    if (this->mStateTimer == 0) {
        FlyGuy_ChangeState(this, &data_ov070_0212359c);
        return 1;
    }
    if (this->mCooldown != 0)
        return 1;
    if (Vec3_Dist(&this->mPos, &this->mHomePos) < kHomeLeashDist) {
        p = this->ClosestNonVanishPlayer();
        if (p) {
            int *pos =
                &p->mPos
                     .x; /* int view is load-bearing: plain p->mPos.x reads change this function's size */
            t.x = pos[0];
            t.y = pos[1];
            t.z = pos[2];
            if (Vec3_Dist(&this->mPos, &t) < kChaseTriggerDist)
                FlyGuy_ChangeState(this, &data_ov070_021235ac);
        }
    }
    return 1;
}

/* -------------------------------------------------------------------------- */

/* (data_ov070_02123520: SharedFilePtr view declared earlier in this TU) */
/* Chase init: plays the file-02123520 animation (the wander one) at speed 1.0. */
int daPropeller_Heyho_c::St_Chase_Init() {
    this->mAnmModel.SetAnim((BCA_File *)data_ov070_02123520.filePtr, 0, 0x1000, 0);
    return 1;
}

/* -------------------------------------------------------------------------- */
int FlyGuy_ChangeState(daPropeller_Heyho_c *c, daPropeller_Heyho_c::State *p);

extern signed char gCurrentSublevel;
extern daPropeller_Heyho_c::State data_ov070_021235dc;
extern daPropeller_Heyho_c::State data_ov070_0212359c;
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
int daPropeller_Heyho_c::St_Chase_Main() {
    daPly_c *player;
    Vector3 vin;
    Vector3 vb;
    Vector3 vc;
    Vector3 vd;

    player = this->ClosestNonVanishPlayer();
    if (player == 0) {
        if (gCurrentSublevel != kLevel0x16) {
            this->mHomePosX = this->mPos.x;
            this->mHomePosY = this->mPos.y;
            this->mHomePosZ = this->mPos.z;
            this->mHomePosY += kHomeLiftEscape;
        }
        this->mPos.x = this->mPrevPos.x;
        this->mPos.y = this->mPrevPos.y;
        this->mPos.z = this->mPrevPos.z;
        this->mPrevAngle.x = 0;
        this->mStateTimer = 0;
        this->mSpeed.y = 0;
        FlyGuy_ChangeState(this, &data_ov070_021235dc);
        return 1;
    }

    vin.x = 0;
    vin.y = 0;
    vin.z = 0;

    {
        int *q =
            &player->mPos
                 .x; /* int view is load-bearing: plain player->mPos.x reads change this function's size */
        vb.x = q[0];
        vb.y = q[1];
        vb.z = q[2];
    }
    vb.y += kAimAbovePlayer;
    vc.x = vb.x;
    vc.y = vb.y;
    vc.z = vb.z;

    this->mTargetAngY = Vec3_HorzAngle(&this->mPos, &vc);
    ApproachAngle(&this->mPrevAngle.y, this->mTargetAngY, 1, 0x500, 0x500);

    vd.x = vb.x;
    vd.y = vb.y;
    vd.z = vb.z;
    ApproachAngle(&this->mPrevAngle.x, Vec3_VertAngle(&this->mPos, &vd), 1, 0x500, 0x500);

    ApproachAngle(&this->mPrevAngle.z, (this->mPrevAngle.y - this->mTargetAngY) / 2, 0xa, 0x100,
                  0x50);

    vin.z = 0xf000;
    Matrix4x3_FromRotationY(&gScratchMtx, this->mAngle.y);
    Matrix4x3_ApplyInPlaceToRotationX(&gScratchMtx, this->mPrevAngle.x);
    MTX_MultVec43(&vin, &gScratchMtx, &this->mSpeed);

    if (Vec3_Dist(&this->mPos, &this->mHomePos) > kHomeLeashDist) {
        FlyGuy_ChangeState(this, &data_ov070_0212359c);
        return 1;
    }

    {
        int dist = Vec3_Dist(&this->mPos, &vb);
        int thresh = kArriveDist;
        if (gCurrentSublevel == kLevel0x16)
            thresh = kArriveDistLevel0x16;
        if (dist < thresh) {
            this->mSpeed.x = 0;
            this->mSpeed.y = 0;
            this->mSpeed.z = 0;
            if (this->mCanSpitFire == 0 ||
                (((unsigned int)RandomIntInternal(&data_0209e650.value) >> 8) & 1) == 0) {
                FlyGuy_ChangeState(this, &data_ov070_021235cc);
            } else {
                FlyGuy_ChangeState(this, &data_ov070_0212358c);
            }
        }
    }

    return 1;
}

/* -------------------------------------------------------------------------- */

/* Dive init: clears mHitDuringAttack and mStateStep, timer = 63 frames, heading
 * from HorzAngleToCPlayerOrAng, plays the file-02123518 animation (flags
 * 0x40000000) at speed 1.0. */
int daPropeller_Heyho_c::St_Dive_Init() {
    this->mHitDuringAttack = 0;
    this->mStateTimer = 63;
    this->mStateStep = 0;
    this->mTargetAngY = this->HorzAngleToCPlayerOrAng();
    this->mAnmModel.SetAnim((BCA_File *)data_ov070_02123518.filePtr, 0x40000000, 0x1000, 0);
    return 1;
}

/* -------------------------------------------------------------------------- */

typedef int s32;
typedef short s16;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef signed char s8;


/* (data_ov070_02123510: SharedFilePtr view declared earlier in this TU) */
extern s8 gCurrentSublevel;
extern daPropeller_Heyho_c::State data_ov070_0212359c;
extern daPropeller_Heyho_c::State data_ov070_021235dc;
extern s32 gWaterHeight;
int FlyGuy_ChangeState(daPropeller_Heyho_c *c, daPropeller_Heyho_c::State *p);
/* Dive main. Turns toward mTargetAngY and levels the Z bank. When the animation
 * has finished: the first time (mStateStep == 0) it starts the file-02123510
 * animation and sets mStateStep = 1; if mHitDuringAttack == 1 (set by the hit
 * reaction when a contact hurt the player) it raises home by 300 units (not when
 * LEVEL_ID == 0x16), starts the 90-frame cooldown and returns to wander.
 * It goes to the settle state, re-anchoring home at the current position (not
 * when LEVEL_ID == 0x16) with the X tilt cleared, when the timer has run out, the
 * actor touches a wall, or there is no non-vanished player. If the player is
 * flagged underwater and the water height (gWaterHeight) is above this actor's
 * Y, it instead re-anchors home 200 units above the current position (not when
 * LEVEL_ID == 0x16), restores last frame's position and zeroes the timer and
 * vertical speed before settling. Otherwise it tilts toward a point 71.0 units
 * (50.0 when LEVEL_ID == 0x16) above the player's mGroundY and sets the velocity
 * words to a 17.0-unit/frame forward vector; while at or below that point + 5.0,
 * at or below the player's Y + 5.0, or beyond 1500 units from home, the timer
 * counts down and the speed drops to 9.0. */
int daPropeller_Heyho_c::St_Dive_Main() {
    daPly_c *player;
    Vector3 abovePlayer;
    Vector3 v;
    Vector3 aim;
    s16 vAngle;
    s16 half;
    s32 z;

    ApproachAngle(&mPrevAngle.y, mTargetAngY, 0x100, 0x1000, 0x1000);
    ApproachAngle(&mPrevAngle.z, 0, 0x100, 0x1000, 0x1000);

    if (mAnmModel.dExtFrameCtrl_c::Finished()) {
        if (mStateStep == 0) {
            mAnmModel.SetAnim((BCA_File *)data_ov070_02123510.filePtr, 0, 0x1000, 0);
            mStateStep = 1;
        }
        if (mHitDuringAttack == 1) {
            if (gCurrentSublevel != kLevel0x16)
                mHomePosY += kHomeLiftRecover;
            mStateStep = 0;
            mCooldown = kAttackCooldown;
            FlyGuy_ChangeState(this, &data_ov070_0212359c);
            return 1;
        }
    }

    if (mStateTimer == 0 || mWithMeshClsn.IsOnWall()) {
        if (gCurrentSublevel != kLevel0x16) {
            mHomePosX = mPos.x;
            mHomePosY = mPos.y;
            mHomePosZ = mPos.z;
        }
        mPrevAngle.x = 0;
        FlyGuy_ChangeState(this, &data_ov070_021235dc);
        return 1;
    }

    player = ClosestNonVanishPlayer();
    if (player == 0) {
        if (gCurrentSublevel != kLevel0x16) {
            mHomePosX = mPos.x;
            mHomePosY = mPos.y;
            mHomePosZ = mPos.z;
        }
        mPrevAngle.x = 0;
        FlyGuy_ChangeState(this, &data_ov070_021235dc);
        return 1;
    }

    if (player->mIsUnderwater != 0 && gWaterHeight > mPos.y) {
        if (gCurrentSublevel != kLevel0x16) {
            mHomePosX = mPos.x;
            mHomePosY = mPos.y;
            mHomePosZ = mPos.z;
            mHomePosY += kHomeLiftEscape;
        }
        mPos.x = mPrevPos.x;
        mPos.y = mPrevPos.y;
        mPos.z = mPrevPos.z;
        mPrevAngle.x = 0;
        mStateTimer = 0;
        mSpeed.y = 0;
        FlyGuy_ChangeState(this, &data_ov070_021235dc);
        return 1;
    }

    *(V3w *)&abovePlayer =
        *(V3w *)&player->mPos.x; /* array-wrapper keeps the ldm/stm block copy under -lang c++ */
    z = 0;
    v.x = z;
    v.y = z;
    v.z = z;
    abovePlayer.y = player->mGroundY;
    if (gCurrentSublevel == kLevel0x16)
        abovePlayer.y += 0x32000;
    else
        abovePlayer.y += 0x47000;
    {
        int tx = abovePlayer.x;
        int ty = abovePlayer.y;
        int tz = abovePlayer.z;
        aim.x = tx;
        aim.y = ty;
        aim.z = tz;
    }
    vAngle = Vec3_VertAngle(&mPos, &aim);
    ApproachAngle(&mPrevAngle.x, vAngle, 0xa, 0x200, 0x100);

    v.z = 0x11000;
    if (mPos.y <= abovePlayer.y + 0x5000 || mPos.y <= player->mPos.y + 0x5000 ||
        Vec3_Dist(&mPos, &mHomePos) > kHomeLeashDist) {
        DecIfAbove0_Short(&mStateTimer);
        v.z = 0x9000;
    }

    half = (mPrevAngle.y - mTargetAngY) / 2;
    ApproachAngle(&mPrevAngle.z, half, 0xa, 0x100, 0x50);

    Matrix4x3_FromRotationY(&gScratchMtx, mAngle.y);
    Matrix4x3_ApplyInPlaceToRotationX(&gScratchMtx, mPrevAngle.x);
    MTX_MultVec43(&v, &gScratchMtx, &mSpeed);

    return 1;
}

/* -------------------------------------------------------------------------- */

/* Settle init: mStateStep = 0; plays the file-02123508 animation (flags
 * 0x40000000) unless mHitDuringAttack is set. */
int daPropeller_Heyho_c::St_Settle_Init() {
    this->mStateStep = 0;
    if (this->mHitDuringAttack == 0) {
        this->mAnmModel.SetAnim((BCA_File *)data_ov070_02123508.filePtr, 0x40000000, 0x1000, 0);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */

extern signed char gCurrentSublevel;
int FlyGuy_ChangeState(daPropeller_Heyho_c *c, daPropeller_Heyho_c::State *p);
extern daPropeller_Heyho_c::State data_ov070_0212359c;

/* Settle main: once the animation has finished, raises home by 300 units (not
 * when LEVEL_ID == 0x16), clears mStateStep, starts the 90-frame cooldown and
 * returns to wander. */
int daPropeller_Heyho_c::St_Settle_Main() {
    if (mAnmModel.dExtFrameCtrl_c::Finished() != 0) {
        if (gCurrentSublevel != kLevel0x16)
            mHomePosY += kHomeLiftRecover;
        mStateStep = 0;
        mCooldown = kAttackCooldown;
        FlyGuy_ChangeState(this, &data_ov070_0212359c);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
struct BCA_File;
/* (dExtAnmModel_c: real header type in scope) */

/* (data_ov070_02123500: SharedFilePtr view declared earlier in this TU) */

/* Fire init: plays the file-02123500 animation (flags 0x40000000) at speed 1.0. */
int daPropeller_Heyho_c::St_Fire_Init() {
    unsigned int flags = 0;
    BCA_File *file = (BCA_File *)data_ov070_02123500.filePtr;
    this->mAnmModel.SetAnim(file, 0x40000000, 0x1000, flags);
    return 1;
}

/* -------------------------------------------------------------------------- */
void func_02012694(unsigned int a, const Vector3 *p);
int FlyGuy_ChangeState(daPropeller_Heyho_c* c, daPropeller_Heyho_c::State* p);
extern daPropeller_Heyho_c::State data_ov070_0212359c;

/* Fire main. While the animation frame (integer part of the 20.12 counter) is
 * below 13 it steers mPrevAngle.y toward mTargetAngY, which is refreshed from the
 * nearest player when there is one. When the animation is about to hit frame 13
 * (WillHitFrame) it spawns the fireball at its own position (SpawnFireball with
 * 30.0 / 10.0 in its two fx32 slots and param1 = 1) with a rotation copied from
 * mAngle.x/.y/.z whose X is the vertical angle to the player, and plays sound 0x105.
 * When the animation has finished it resets the frame to 0, starts the 90-frame
 * cooldown and returns to wander. */
int daPropeller_Heyho_c::St_Fire_Main() {
    daPly_c *pl;
    struct Vector3 posbuf;
    struct Vector3 fp;
    struct Vector3 target;

    pl = ClosestPlayer();
    if ((unsigned)(mAnmModel.currFrame << 4) >> 0x10 < kFireFrame) {
        if (pl != 0) {
            *(V3w *)&posbuf = *(V3w *)&pl->mPos.x;
            target.x = posbuf.x;
            target.y = posbuf.y;
            target.z = posbuf.z;
            mTargetAngY = Vec3_HorzAngle(&mPos, &target);
        }
        ApproachAngle(&mPrevAngle.y, mTargetAngY, 0xa, 0x400, 0x200);
    }

    if (mAnmModel.dExtFrameCtrl_c::WillHitFrame(kFireFrame) != 0) {
        Vector3_16 vel = mAngle;
        if (pl != 0) {
            Vector3 *base = &pl->mPos;
            fp.x = base->x;
            fp.y = base->y;
            fp.z = base->z;
            vel.x = Vec3_VertAngle(&mPos, &fp);
        }
        SpawnFireball(mPos, &vel, 0x1e000, 0xa000, 1);
        func_02012694(kSndFireSpit, &mCamSpacePos);
    }
    if (mAnmModel.dExtFrameCtrl_c::Finished() != 0) {
        mAnmModel.currFrame = 0;
        mCooldown = kAttackCooldown;
        FlyGuy_ChangeState(this, &data_ov070_0212359c);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */

/* Knocked init: zeroes the mSpeed velocity words, then
 * mSpeed.y = +50.0 units/frame (upward pop), mVertAccel = -5.0, timer = 3
 * frames and mFlags = 0. */
short daPropeller_Heyho_c::St_Knocked_Init() {
    this->mSpeed.x = 0;
    this->mSpeed.y = 0;
    this->mSpeed.z = 0;
    this->mSpeed.y = 0x32000;
    this->mVertAccel = -0x5000;
    this->mStateTimer = 3;
    this->mFlags = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */


/* Knocked main. While mStateStep is non-zero (normally a fire hit; the
 * mega-character hit leaves it unchanged) it refreshes the effects 0x13a and
 * 0x13b at the position + 80.0 units up each frame (their handles are fed back). Tilts mAngle.x toward
 * -0x4000 (a quarter turn) with ApproachAngle and then ApproachLinear. When the
 * timer reaches 0 it runs the defeat helper func_ov070_0211f0a4. */
int daPropeller_Heyho_c::St_Knocked_Main() {
    if (this->mStateStep != 0) {
        Vector3 v;
        int x, y, z;
        x = this->mPos.x;
        z = this->mPos.z;
        y = this->mPos.y + 0x50000;
        v.x = x;
        v.y = y;
        v.z = z;
        this->mParticle0 =
            Particle::System::New(this->mParticle0, kPtclDeathA, v.x, v.y, v.z, 0, 0);
        this->mParticle1 =
            Particle::System::NewCleanParticle(this->mParticle1, kPtclDeathB, v.x, v.y, v.z, 0);
    }
    ApproachAngle(&this->mAngle.x, -0x4000, 0xa, 0x200, 0x100);
    ApproachLinear(this->mAngle.x, -0x4000, 0x200);
    if (this->mStateTimer == 0)
        this->func_ov070_0211f0a4();
    return 1;
}

/* -------------------------------------------------------------------------- */
typedef int s32;
typedef short s16;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

struct BCA_File;

extern daPropeller_Heyho_c::State data_ov070_021235bc;
extern daPropeller_Heyho_c::State data_ov070_021235cc;
/* (data_ov070_02123528: SharedFilePtr view declared earlier in this TU) */

int FlyGuy_ChangeState(daPropeller_Heyho_c *c, daPropeller_Heyho_c::State *p);
extern void func_02012694(unsigned int a, const Vector3 *b);
/* Hit reaction, run once per frame outside the knocked state. Takes the colliding
 * actor from the collider (mdCcAc_c.otherOwner) and reads hitFlags: fire sends it
 * to the knocked state with mStateStep = 1 (particles); 0x20 kills it through
 * KillByAttack with mDeathState = 1; the punch/kick/egg/explosion mask sends it to the knocked
 * state with mStateStep = 0. After that only the player actor counts, and only a
 * non-vanished one: JumpedOnByPlayer bounces the player (SpinBounce 40.0) and
 * kills as above; a mega-character hit spawns the mega particles, counts the
 * kill and plays sound 0x1d before the knocked state; a metal player or one on a
 * shell sends it to the knocked state with mStateStep = 0; any other contact
 * hurts the player (daPly_c::Hurt) and, in the dive state with mHitDuringAttack
 * still 0, plays the file-02123528 animation and sets mStateStep =
 * mHitDuringAttack = 1. */
void daPropeller_Heyho_c::HandleContact() {
    daPly_c *hitPlayer;
    s32 hitFlags;

    if (this->mdCcAc_c.otherOwner == 0)
        return;
    hitPlayer = (daPly_c *)dActor_c::FindWithID(this->mdCcAc_c.otherOwner);
    if (!hitPlayer)
        return;

    hitFlags = this->mdCcAc_c.hitFlags;
    if (hitFlags & kHitFire) {
        this->mStateStep = 1;
        FlyGuy_ChangeState(this, &data_ov070_021235bc);
        return;
    }
    if (hitFlags & kHitSpinPound) {
        this->mDeathState = 1;
        this->KillByAttack(hitPlayer, 0);
        return;
    }
    if (hitFlags & kHitAttackMask) {
        this->mStateStep = 0;
        FlyGuy_ChangeState(this, &data_ov070_021235bc);
        return;
    }

    if (!IsPlayerActor(hitPlayer))
        return;
    if (hitPlayer->mIsVanish != 0)
        return;

    if (this->JumpedOnByPlayer(this->mdCcAc_c, *hitPlayer)) {
        hitPlayer->SpinBounce(0x28000);
        this->mDeathState = 1;
        this->KillByAttack(hitPlayer, 0);
        return;
    }

    if (hitFlags & kHitMegaChar) {
        this->SpawnMegaCharParticles(*hitPlayer, (char *)0);
        hitPlayer->IncMegaKillCount();
        func_02012694(kSndMegaKill, &this->mCamSpacePos);
        FlyGuy_ChangeState(this, &data_ov070_021235bc);
        return;
    }

    if (hitPlayer->mIsMetal == 1 || hitPlayer->IsOnShell() == 1) {
        this->mStateStep = 0;
        FlyGuy_ChangeState(this, &data_ov070_021235bc);
        return;
    }

    {
        Vector3 v;
        v.x = this->mPos.x;
        v.y = this->mPos.y;
        v.z = this->mPos.z;
        hitPlayer->Hurt(v, 2, 0xc000, 1, 0, 1);
    }
    if (this->mHitDuringAttack != 0)
        return;
    if (this->mCurrentState != &data_ov070_021235cc)
        return;

    this->mAnmModel.SetAnim((BCA_File *)data_ov070_02123528.filePtr, 0x40000000, 0x1000, 0);
    this->mStateStep = 1;
    this->mHitDuringAttack = 1;
}

/* -------------------------------------------------------------------------- */

/* (dActor_c: real header type in scope) */

/* Defeat helper: a small puff of dust, then mExtraCoinCount + 1 coins spread
 * 10.0 units from the actor, then the actor is killed and recorded in the
 * death table. */
void daPropeller_Heyho_c::func_ov070_0211f0a4() {
    this->SmallPoofDust();
    Vector3 pos;
    pos.x = this->mPos.x;
    pos.y = this->mPos.y;
    pos.z = this->mPos.z;
    unsigned int coins = this->mExtraCoinCount + 1;
    this->SpawnCoins(pos, coins, 0xa000, 0);
    this->KillAndTrackInDeathTable();
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
