//cpp
/* daYurei_Mucho_c -- Snufit, the masked Snifit that drifts around Hazy Maze
 * Cave and shoots at you (registry name YUREI_MUCHO, ov065).
 *
 * Class identity, layout evidence and the matching experiments behind the
 * spellings below are recorded in
 * notes/agents/handoffs/daYurei_Mucho_c-ov065.md.
 *
 * 0x02115f84..0x0211696c are this TU's own state and helper methods. The
 * Idle and Shoot states take N64's snufit_act_idle / snufit_act_shoot names;
 * Chase (the wind-up DS splits out of N64's idle) and Bumped are coined from
 * their bodies. symbols.txt records the mangled spelling, which is what the
 * State records at the end of this file name.
 *
 * Remaining reconstruction under issue #2478:
 * - The V3A copy of a player's position in UpdateShoot: Vector3
 *   assignment scalarizes; UpdateChase's snapshot is a POD VecFx32
 *   copy-init.
 * - dExtAnmModel_c::SetAnim, dCcAc_c::Init, dBgCh_Actr::Init,
 *   DropShadowRadHeight, KillByInvincibleChar, SpawnCoins, daPly_c::Hurt
 *   and daPly_c::Bounce retain scalar ABI declarations. Native fixed-point
 *   call reconstruction remains separate work; the handoff records the
 *   measured Init alternatives. Hurt follows its current scalar definition.
 * - pad_3e2 is unobserved.
 */

#include "daYurei_Mucho_c.h"
#include "types.h"
#include "common.h"
#include "decl_common.h"
#include "SharedFilePtr.h"
#include "daPly_c.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "math/Approach.h"
#include "SpawnInfo.h"
#include "LevelRng.h"
#include "math/CalcLib.h"

#include "math/MtxStack.h"
/* Kept for UpdateShoot's playerPos (measured r57): any Vector3 copy is
 * memberwise, and the ROM block-moves the player's position.
 * Kept (#821 r71): `playerPos = pl->mPos` gives UpdateShoot 0x220 (ROM 0x224);
 * a VecFx32 copy-init cannot feed the Vector3 target without a cast. */
struct V3A { int w[3]; };

/* Scalar ABI declarations retained by this TU. */

/* arm9 scalar ABI and unnamed helpers; see the remaining work above. */
extern void func_02012694(unsigned int id, const Vector3 *v);

/* Runtime asset handles, distinct from the NitroFS file IDs. */
enum daYurei_Mucho_c_AssetId {
    ASSET_YUREI_MUCHO_BMD = 0x042d,
    ASSET_YUREI_MUCHO_ATTACK_BCA = 0x042e,
    ASSET_YUREI_MUCHO_BALL_BMD = 0x042f,
    ASSET_YUREI_MUCHO_WAIT_BCA = 0x0430
};

/* The handles reach their files through these wrappers' constructor and
 * destructor veneers; the manifest maps each to the retail destination.
 * Descriptive local types, not recovered spellings. */
struct YureiMuchoModelFile : SharedFilePtr {
    YureiMuchoModelFile(u32 fileID);
    ~YureiMuchoModelFile();
};

struct YureiMuchoAnimFile : SharedFilePtr {
    YureiMuchoAnimFile(u32 fileID);
    ~YureiMuchoAnimFile();
};

/* This TU's four file handles, in the order the retail initializer
 * (0x0211c110) builds them. d_a_propeller_heyho_fire.cpp also loads the
 * shot model through an extern, so that handle keeps its address name. */
YureiMuchoModelFile data_ov065_0211d610(ASSET_YUREI_MUCHO_BALL_BMD);
YureiMuchoModelFile gYureiMuchoModelFile(ASSET_YUREI_MUCHO_BMD);
YureiMuchoAnimFile gYureiMuchoWaitAnimFile(ASSET_YUREI_MUCHO_WAIT_BCA);
YureiMuchoAnimFile gYureiMuchoAttackAnimFile(ASSET_YUREI_MUCHO_ATTACK_BCA);

/* The four State records, defined at the end of this file. */
extern daYurei_Mucho_c::State data_ov065_0211d670;
extern daYurei_Mucho_c::State data_ov065_0211d680;
extern daYurei_Mucho_c::State data_ov065_0211d650;
extern daYurei_Mucho_c::State data_ov065_0211d660;

/* -------------------------------------------------------------------------- */
/* The registry factory behind the YUREI_MUCHO / SNUFIT profile. `return new
   daYurei_Mucho_c()` MATCHES (size 0x50); the synthesized ctor stores
   `_ZTV15daYurei_Mucho_c + 2`. classInit is a reconstructed source-style
   name, not a preserved identifier. */
daYurei_Mucho_c *daYurei_Mucho_c_classInit(void) {
    return new daYurei_Mucho_c();
}

/* -------------------------------------------------------------------------- */
/* dActor_c vtable slot 18. */
s32 daYurei_Mucho_c::OnYoshiTryEat()
{
    return 4;
}

/* -------------------------------------------------------------------------- */
/* dActor_c vtable slot 19, confirmed by address: _ZTV15daYurei_Mucho_c
 * (0x0211cba4) + 0x4c -> 0x02116f14.
 *
 * The `R6Player` reference spelling in the mangled name is a coined guess: a
 * reference and a pointer generate identical ARM for this body, so the bytes
 * cannot distinguish them. */
void daYurei_Mucho_c::OnTurnIntoEgg(daPly_c &player) {
    GivePlayerCoins(player, (mExtraCoinCount + 1), 0);
    KillAndTrackInDeathTable();
}

/* -------------------------------------------------------------------------- */
/* dActor_c vtable slot 29. */
s32 daYurei_Mucho_c::OnAimedAtWithEgg()
{
    return 0;
}

/* -------------------------------------------------------------------------- */
/* Both Init methods are declared. The current dCcAc_c fixed-point aggregate
   arguments add 16 bytes here; dBgCh_Actr's scalar declaration emits a name
   absent from symbols.txt. Retain these ABI calls pending interface repair. */
int daYurei_Mucho_c::InitResources()
{
    mAnmModel.SetFile(dExtSimpleModel_c::LoadFile(gYureiMuchoModelFile), 1, -1);
    dExtSimpleModel_c::LoadFile(data_ov065_0211d610);
    mShadowModel.InitCylinder();
    dExtFrameCtrl_c::LoadFile(gYureiMuchoWaitAnimFile);
    dExtFrameCtrl_c::LoadFile(gYureiMuchoAttackAnimFile);
    mTerminalVelocity = -0x1e000;
    mdCcAc_c.Init(this, 0x38000, 0x7e000, 0x200000, 0x7eff0);
    mAngle.y = mPrevAngle.y;
    mWithMeshClsn.Init(this, 0x46000, 0, 0, 0);
    mCoinType = 1;
    mExtraCoinCount = 1;
    mHomePos.x = mPos.x;
    mHomePos.y = mPos.y;
    mHomePos.z = mPos.z;
    mAnmModel.speed = 0x1000;
    ChangeState(&data_ov065_0211d670);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* dActor_c vtable slot 6. */
int daYurei_Mucho_c::Behavior()
{
    if (UpdateYoshiEat(mWithMeshClsn) != 0) {
        mdCcAc_c.Clear();
        if (mEatenByYoshi != 0) {
            if (mSpitOutTimer == 0) {
                mdCcAc_c.Update();
            }
        }
        UpdateModelTransform();
        mHomePos.x = mPos.x;
        mHomePos.y = mPos.y;
        mHomePos.z = mPos.z;
        ChangeState(&data_ov065_0211d670);
        return 1;
    }
    if (UpdateKillByInvincibleChar(mWithMeshClsn, mAnmModel, 3) != 0) {
        return 1;
    }
    if (mDeathState != 0) {
        ApproachAngle(&mAngle.x, -0x4000, 0xa, 0x200, 0x100);
        UpdateDeath(mWithMeshClsn);
        UpdateModelTransform();
        return 1;
    }
    DecIfAbove0_Short(&mStateTimer);
    {
        State *q = mCurrentState;
        if (q->mMain != 0) {
            (this->*(q->mMain))();
        }
    }
    {
        /* Gravity, clamped at terminal velocity. mSpeed.z is read and written
           back unchanged -- the ROM really does reload and restore it here. */
        int fallSpeed = mSpeed.y + mVertAccel;
        int clamped = mTerminalVelocity;
        if (fallSpeed >= clamped) {
            clamped = fallSpeed;
        }
        int keep = mSpeed.z;
        mSpeed.y = clamped;
        mSpeed.z = keep;
    }
    if (mCurrentState != &data_ov065_0211d650) {
        int ang;
        fx16 sine;
        int result;
        mBobAngle += 0x200;
        ang = mBobAngle;
        /* The shift must be LOGICAL so the angle wraps -- writing it on the
           signed s16 would read the wrong table entry for negative angles. */
        sine = FX_SinIdx((unsigned short)(short)ang >> 4);
        result = FX_MUL(sine, 0x46000);
        ApproachLinear(mPos.y, mHomePos.y + (result + 0xb4000), 0x3000);
    }
    UpdatePosWithOnlySpeed(&mdCcAc_c);
    UpdateWMClsn(mWithMeshClsn, 0);
    UpdateModelTransform();
    if (mCurrentState != &data_ov065_0211d660) {
        mAngle.x = mPrevAngle.x;
        mAngle.y = mPrevAngle.y;
        mAngle.z = mPrevAngle.z;
        HandleCollision();
    }
    mdCcAc_c.Clear();
    {
        daPly_c *p = ClosestPlayer();
        if (p != 0 && p->mIsVanish == 0) {
            mdCcAc_c.Update();
        }
    }
    mAnmModel.Advance();
    return 1;
}

static inline int IsInYoshiMouth(dActor_c *actor) {
    return (actor->mFlags & 0x40000) != 0;
}

/* -------------------------------------------------------------------------- */
/* dActor_c vtable slot 9. */
int daYurei_Mucho_c::Render()
{
    if (IsInYoshiMouth(this))
        return 1;
    mAnmModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* fBase_c slot 12. Empty in the ROM: four bytes, `bx lr`. */
void daYurei_Mucho_c::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
/* dActor_c vtable slot 3. Releases the four files InitResources claimed.
 *
 * The body does not read the incoming object pointer or access instance
 * fields. Spelling it as the class method preserves the ROM bytes. */
int daYurei_Mucho_c::CleanupResources()
{
    data_ov065_0211d610.Release();
    gYureiMuchoModelFile.Release();
    gYureiMuchoWaitAnimFile.Release();
    gYureiMuchoAttackAnimFile.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* Rebuilds the model matrix from the actor's position and Z/X/Y angles, then
   drops the shadow. Called three times from Behavior. */
void daYurei_Mucho_c::UpdateModelTransform() {
    Vector3 v;

    Vec3_Asr(&v, &mPos, 3);
    Matrix4x3_FromTranslation(&gScratchMtx, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&gScratchMtx, mAngle.x, mAngle.y, mAngle.z);

    mAnmModel.mat4x3 = gScratchMtx;
    mShotPos.x = 0;
    mShotPos.y = 0;
    mShotPos.z = 0;
    gScratchMtx = mAnmModel.mat4x3;

    MTX_Concat43(&mAnmModel.data.transforms[4], &gScratchMtx, &gScratchMtx);

    mShotPos.x = gScratchMtx.t.x;
    mShotPos.y = gScratchMtx.t.y;
    mShotPos.z = gScratchMtx.t.z;
    mShotPos.x <<= 3;
    mShotPos.y <<= 3;
    mShotPos.y -= 0xa000;
    mShotPos.z <<= 3;

    Matrix4x3_FromTranslation(&gScratchMtx, mPos.x >> 3, (mPos.y - 0x18000) >> 3, mPos.z >> 3);

    mShadowMat = gScratchMtx;

    DropShadowRadHeight(mShadowModel, mShadowMat, 0x64000, 0x258000, 0xf);
}

/* -------------------------------------------------------------------------- */
/* The state setter. Stores the State into mCurrentState, RE-READS it -- the ROM
   really does reload the field it has just written -- null-tests the entry hook
   at +0x00 and calls it through the object. Called eight times inside this run.

   The pointer-to-member call is the ROM's own `ldr r2,[r3]; cmp r2,#0;
   ldr r1,[r3,#4]; add r0,r0,r1,asr #1; ands r1,r1,#1; ldrne r1,[r0];
   ldrne r1,[r1,r2]; ldreq r1,[r3]; blx r1` sequence, i.e. the Itanium
   {ptr_or_vtable_offset, adj*2|isVirtual} encoding. All eight records in
   ov065's .data carry adj word 0: non-virtual, no this-adjustment. */
int daYurei_Mucho_c::ChangeState(State *s) {
    mCurrentState = s;
    State *q = mCurrentState;
    if (q->mEnter == 0) {
        return 1;
    }
    return (this->*(q->mEnter))();
}

/* -------------------------------------------------------------------------- */
/* State entry hook at 0x0211d670 +0x00: randomise the facing angle and the
   timer, then start the wait animation. The BCA file is read straight out of
   the SharedFilePtr's filePtr -- the ROM does `ldr r1,[r0,#4]` off the
   literal at 0x02116918, not a call, so the field read is what reproduces. */
int daYurei_Mucho_c::EnterIdle() {
    mTargetAngle = (((u32)RandomIntInternal(&data_0209e650.value) >> 8) << 0xc);
    mStateTimer = ((((u32)RandomIntInternal(&data_0209e650.value) >> 8) & 0x1f) + 50);
    mAnmModel.SetAnim((BCA_File *)gYureiMuchoWaitAnimFile.filePtr, 0, 0x1000, 0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* State main hook at 0x0211d670 +0x08: drift, and switch to the attack state
   when a player comes close enough. */
int daYurei_Mucho_c::UpdateIdle() {
    Vector3 in;
    Vector3 dp;
    daPly_c *pl;
    int *pos;

    in.x = 0;
    in.y = 0;
    in.z = 0;

    if (Vec3_Dist(&mPos, &mHomePos) > 0x1f4000 || mWithMeshClsn.IsOnWall() != 0) {
        mTargetAngle = Vec3_HorzAngle(&mPos, &mHomePos);
        if (mStateTimer < 20)
            mStateTimer = 20;
    }

    ApproachAngle(&mPrevAngle.y, mTargetAngle, 0xa, 0x200, 0x100);
    ApproachAngle(&mPrevAngle.x, 0, 1, 0x500, 0x500);

    in.z = 0xa000;
    Matrix4x3_FromRotationY(&gScratchMtx, mAngle.y);
    MTX_MultVec43(&in, &gScratchMtx, &mSpeed);

    if (mStateTimer == 0) {
        ChangeState(&data_ov065_0211d670);
        return 1;
    }

    pl = ClosestNonVanishPlayer();
    if (pl != 0) {
        pos = &pl->mPos.x;
        dp.x = pos[0];
        dp.y = pos[1];
        dp.z = pos[2];

        if (Vec3_Dist(&mPos, &dp) < 0x3e8000) {
            mStateTimer = 20;
            ChangeState(&data_ov065_0211d680);
        }
    }

    return 1;
}

/* -------------------------------------------------------------------------- */
/* State entry hook at 0x0211d680 +0x00: stop the actor dead. */
int daYurei_Mucho_c::EnterChase() {
    mSpeed.x = 0;
    mSpeed.y = 0;
    mSpeed.z = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* State main hook at 0x0211d680 +0x08: steer toward the nearest non-vanishing
   player.

   dActor_c::ClosestNonVanishPlayer takes `this` in r0; the ROM emits no `mov`
   before the `bl` at 0x021165e8 because r0 still holds the incoming object. */
int daYurei_Mucho_c::UpdateChase() {
    short pitch = 0;
    daPly_c *p = ClosestNonVanishPlayer();
    if (p != 0) {
        /* POD: the ROM copy-inits with ldm/stm; a Vector3 copy is memberwise. */
        /* Measured (#821 r68): copied as a Vector3 this is memberwise, not the ROM's ldm/stm. */
        /* Kept (#821 r77): the snapshot reads through `*(VecFx32 *)&` and goes
           back to Vec3_Dist as `const Vector3 *`. `Vector3 playerPos = p->mPos;`
           gives 0x150 (ROM 0x154); passing the Vector3 copy b instead of playerPos
           gives 0x14c. */
        VecFx32 playerPos = *(VecFx32 *)&p->mPos;
        Vector3 v;
        Vector3 a;
        a.x = playerPos.x;
        a.y = playerPos.y;
        a.z = playerPos.z;
        mTargetAngle = Vec3_HorzAngle(&mPos, &a);
        Vector3 b;
        b.x = playerPos.x;
        b.y = playerPos.y;
        b.z = playerPos.z;
        pitch = Vec3_VertAngle(&mPos, &b);
        if (Vec3_Dist(&mPos, (const Vector3 *)&playerPos) >= 0x1f4000) {
            mSpeed.x = 0;
            mSpeed.y = 0;
            mSpeed.z = 0;
        } else {
            v.z = 0;
            v.z = -0x1000;
            v.x = 0;
            v.y = 0;
            Matrix4x3_FromRotationY(&gScratchMtx, mAngle.y);
            MTX_MultVec43(&v, &gScratchMtx, &mSpeed);
        }
    } else {
        mStateTimer = pitch;
    }
    ApproachAngle(&mPrevAngle.y, mTargetAngle, 1, 0x500, 0x500);
    ApproachAngle(&mPrevAngle.x, pitch, 1, 0x500, 0x500);
    if (mStateTimer == 0)
        ChangeState(&data_ov065_0211d650);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* State entry hook at 0x0211d650 +0x00: start the attack animation. As at
   ordinal 11, the BCA file is the SharedFilePtr's filePtr, read directly
   (`ldr r1,[r0,#4]` at 0x021165a4 off the literal at 0x021165d4). */
short daYurei_Mucho_c::EnterShoot() {
    mShotCount = 0;
    mAnmModel.SetAnim((BCA_File *)gYureiMuchoAttackAnimFile.filePtr, 0x40000000, 0x1000, 0);
    mStateTimer = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* State main hook at 0x0211d650 +0x08: aim, and spawn the projectile (profile
   0xe9) up to three times before returning to a wait state. */
int daYurei_Mucho_c::UpdateShoot() {
    Vector3 playerPos, shotSpeed, shotVel, shotPos, target;
    daPly_c *pl = ClosestNonVanishPlayer();
    if (pl != 0) {
        *(V3A *)(&playerPos) = *(V3A *)(&pl->mPos.x);
        target = playerPos;
        mTargetAngle = Vec3_HorzAngle(&mPos, &target);
        ApproachAngle(&mPrevAngle.y, mTargetAngle, 1, 0x500, 0x500);

        if ((((u32)mAnmModel.currFrame << 4) >> 16) >= 0xf && mStateTimer == 0 && mShotCount < 3) {
            void *spawned = dActor_c::Spawn(0xe9, 1, mShotPos, 0, mAreaId, -1);
            if (spawned != 0) {
                dActor_c *shot = (dActor_c *)spawned;
                func_02012694(0xfb, &mCamSpacePos);
                shotSpeed.x = 0;
                shotSpeed.y = 0;
                shotSpeed.z = 0x1e000;
                shotVel.x = 0;
                shotVel.y = 0;
                shotVel.z = 0;
                Matrix4x3_FromRotationY(&gScratchMtx, mAngle.y);
                Matrix4x3_ApplyInPlaceToRotationX(&gScratchMtx, mAngle.x);
                MTX_MultVec43(&shotSpeed, &gScratchMtx, &shotVel);
                shot->mSpeed.x = shotVel.x;
                shot->mSpeed.y = shotVel.y;
                shot->mSpeed.z = shotVel.z;
                mShotCount += 1;
                mStateTimer = 4;
            }
        }
    }

    if (mAnmModel.Finished() != 0) {
        if (pl != 0) {
            Vector3 *dsrc = &pl->mPos;
            shotPos.x = dsrc->x;
            shotPos.y = dsrc->y;
            shotPos.z = dsrc->z;
            if (Vec3_Dist(&mPos, &shotPos) > 0x3e8000) {
                mStateTimer = 0;
                ChangeState(&data_ov065_0211d670);
            } else {
                mStateTimer = 50;
                mAnmModel.SetAnim((BCA_File *)gYureiMuchoWaitAnimFile.filePtr, 0, 0x1000, 0);
                ChangeState(&data_ov065_0211d680);
            }
        } else {
            mStateTimer = 0;
            ChangeState(&data_ov065_0211d670);
        }
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* State entry hook at 0x0211d660 +0x00: the bumped-from-below pop. */
int daYurei_Mucho_c::EnterBumped() {
    mSpeed.x = 0;
    mSpeed.y = 0;
    mSpeed.z = 0;
    mSpeed.y = 0x32000;
    mVertAccel = -0x5000;
    mStateTimer = 10;
    mFlags = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* State main hook at 0x0211d660 +0x08.
 *
 * CORRECTED HERE. The retired shard func_ov065_021162c0.c called `_ZN10daIwante_cD0Ev`, which is
 * ov066's name for 0x02115f84. This module's own name for that address is
 * KillWithCoins, the death helper two ordinals below, and ov065 is the
 * module this branch links in. match.py wildcards every relocated word, so the
 * wrong callee still reproduced the bytes; the ROM's own `bl 0x02115f84` at
 * 0x02116314 decides it. */
int daYurei_Mucho_c::UpdateBumped() {
    ApproachAngle(&mAngle.x, -0x4000, 0xa, 0x200, 0x100);
    ApproachLinear(mAngle.x, -0x4000, 0x200);
    if (mStateTimer == 0)
        KillWithCoins();
    return 1;
}

static inline int IsPlayerActor(dActor_c *actor) {
    return actor->actorID == 0xbf;
}

/* -------------------------------------------------------------------------- */
/* The collision response, called once from Behavior. */
void daYurei_Mucho_c::HandleCollision() {
    Vector3_16 v;
    Vector3 hv;
    daPly_c *p;
    int flags;
    unsigned int id;

    id = mdCcAc_c.otherOwner;
    if (id == 0) return;
    p = (daPly_c *)dActor_c::FindWithID(id);
    if (p == 0) return;
    flags = mdCcAc_c.hitFlags;

    if (flags & 0x40000) {
        mDeathState = 4;
        KillByAttack(p, 0);
        func_02012694(0x11e, &mCamSpacePos);
        return;
    }
    if (flags & 0x2400) {
        mDeathState = 2;
        KillByAttack(p, 0);
        func_02012694(0x11e, &mCamSpacePos);
        return;
    }
    if (flags & 0x4380) {
        mDeathState = 3;
        KillByAttack(p, 0);
        func_02012694(0x11e, &mCamSpacePos);
        return;
    }
    if (BumpedUnderneathByPlayer(*p) == 1) {
        func_02012694(0x11e, &mCamSpacePos);
        ChangeState(&data_ov065_0211d660);
        return;
    }
    if (flags & 0x10) {
        v.x = -0x2000;
        v.y = 0;
        v.z = 0;
        KillByInvincibleChar(v, *p, 0);
        func_02012694(0x11e, &mCamSpacePos);
        return;
    }
    if (flags & 0x40) {
        mDeathState = 2;
        KillByAttack(p, 0);
        func_02012694(0x11e, &mCamSpacePos);
        return;
    }
    if (flags & 0x20) {
        mDeathState = 1;
        KillByAttack(p, 0);
        func_02012694(0x11e, &mCamSpacePos);
        return;
    }
    {
        if (!IsPlayerActor(p))
            return;
    }
    if (p->mIsMetal == 1 || p->IsOnShell() == 1) {
        KillWithCoins();
        return;
    }
    if (JumpedOnByPlayer(mdCcAc_c, *p)) {
        p->Bounce(0x28000);
        mDeathState = 1;
        KillByAttack(p, 0);
        func_02012694(0x11e, &mCamSpacePos);
        return;
    }
    hv.x = mPos.x;
    hv.y = mPos.y;
    hv.z = mPos.z;
    p->Hurt(hv, 2, 0xc000, 1, 0, 1);
}

/* -------------------------------------------------------------------------- */
/* The death helper: poof, drop the coins, unregister. */
void daYurei_Mucho_c::KillWithCoins() {
    Vector3 v;
    SmallPoofDust();
    v.x = mPos.x;
    v.y = mPos.y;
    v.z = mPos.z;
    SpawnCoins(v, mExtraCoinCount + 1, 0xa000, 0);
    KillAndTrackInDeathTable();
    func_02012694(0x11e, &mCamSpacePos);
}

/* -------------------------------------------------------------------------- */
/* No separate body lives here. The inline virtual destructor in the directly
 * included class header makes mwccarm emit retail's D1 then D0 pair without
 * the otherwise homeless D2 variant an out-of-line definition produces.
 *
 * D1 stores the vptr, then destroys the dExtShadowModel_c at 0x364, the dExtAnmModel_c at
 * 0x300, the dBgCh_Actr at 0x144 and the dCcAc_c at 0x110 in reverse
 * declaration order, and tails into ov002 _ZN12dEnemyBase_cD2Ev. D0 repeats
 * that body verbatim -- it does NOT call D1 -- and then hands the object back
 * to the game heap. All of that is a consequence of the class declaration;
 * none of it is written out. */

/* The four {enter, main} State records, in the order the retail initializer
 * fills them. EnterShoot returns short, so its pointer is cast to
 * the record's int-returning type. */
typedef int (daYurei_Mucho_c::*YureiMuchoStateFn)();

daYurei_Mucho_c::State data_ov065_0211d670 = {&daYurei_Mucho_c::EnterIdle,
                                              &daYurei_Mucho_c::UpdateIdle};
daYurei_Mucho_c::State data_ov065_0211d680 = {&daYurei_Mucho_c::EnterChase,
                                              &daYurei_Mucho_c::UpdateChase};
daYurei_Mucho_c::State data_ov065_0211d650 = {(YureiMuchoStateFn)&daYurei_Mucho_c::EnterShoot,
                                              &daYurei_Mucho_c::UpdateShoot};
daYurei_Mucho_c::State data_ov065_0211d660 = {&daYurei_Mucho_c::EnterBumped,
                                              &daYurei_Mucho_c::UpdateBumped};

/* The actor profile, in SpawnInfo's layout (include/SpawnInfo.h). The
 * retail values are unchanged. */
SpawnInfo<daYurei_Mucho_c> g_profile_YUREI_MUCHO = {
    daYurei_Mucho_c_classInit, /* classInit */
    0x00ec,                    /* behaviorPriority */
    0x005b,                    /* renderPriority */
    0x10000003,                /* actorFlags */
    0x64000,                   /* clipOffsetY */
    0xc8000,                   /* clipRadius */
    0x01000000,                /* clipDistance */
    0x01000000                 /* farDistance */
};
