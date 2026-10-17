//cpp
/**
 * Koopa shell (profile SHELL) -- ov102/daShl_c.
 *
 * Idle until the player touches it. Model 0 can be jumped on and ridden
 * (it follows the rider and gets a per-frame surface splash/trail pass and
 * a looping surface sound); model 1 is kicked away by an attack. The
 * variant (param1 bit 4) is picked up and held instead. Yoshi can eat any
 * of them and spit it back out sliding, and a sliding shell that hits a
 * wall breaks.
 *
 * A small state machine drives it. mState points at one of four file-scope
 * records {enter, tick}, each a pointer-to-member; func_ov102_0214d1f8
 * switches state and runs `enter`, Behavior runs `tick` every frame. The
 * records are .bss. __sinit_d_a_shl.cpp copies the PMF constants at
 * ov102:0x0214e5d4..0x0214e614 into them:
 *
 *     data_ov102_0214ea68  idle     enter 0214d1b8  tick 0214d1b0
 *     data_ov102_0214ea48  ridden   enter 0214d0bc  tick 0214d044
 *     data_ov102_0214ea58  held     enter 0214d020  tick 0214cfe4
 *     data_ov102_0214ea78  sliding  enter 0214d148  tick 0214d114
 *
 * daShl_c_classInit is reconstructed (RTTI daShl_c, SHELL registry).
 * Retail does not store that spelling. Historical aliases: KoopaShell,
 * KoopaShell_Spawn.
 *
 * FUNCTION ORDER IS THE REVERSE OF THE ROM'S -- mwccarm emits one .text
 * section per function in the reverse of source order.
 *
 * Known limits:
 * - dCcAc_c::Init, dBgCh_Actr::Init, dActor_c::DropShadowRadHeight,
 *   dEnemyBase_c::KillByInvincibleChar, Player::Hurt and
 *   Particle::System::New stay mangled: each takes Fix12<int> by value
 *   (notes/mwccarm-codegen.md 6az). Particle::System::New is declared
 *   with the seven arguments its call sites pass.
 * - data_ov102_0214d70c is this TU's .rodata pair of model-file handles
 *   and the state records are .bss; this text-only TU claims neither.
 *
 * deslop leftovers:
 * - func_ov002_020ad660, func_ov002_020cc16c and func_020105cc are other
 *   TUs' helpers and stay free externs; the Fix12<int>-by-value callees
 *   listed above stay mangled (notes/mwccarm-codegen.md 6az).
 * - The manifest pins eight @NNN statics (@721..@735), so the count of
 *   named locals, named parameters, labels and unnamed struct types
 *   ahead of __sinit is load-bearing. Including a header moves it
 *   (measured: Player.h +94, the first pin becomes @815; Sound.h +15), so
 *   Player and Sound stay reached through local mangled externs and
 *   Player::mIsAirborne stays the raw 0x6de read.
 * - dBgCh_Gnd.h adds 9 to the count. Including it let five mangled
 *   dBgCh_Gnd externs (7 named parameters), a `surface` local in
 *   func_ov102_0214c7fc and an unnamed struct type for `dir` in
 *   func_ov102_0214cbec go, which nets to 0 (measured: same bytes,
 *   relocations and pins).
 * - The unnamed struct type of `pos` in func_ov102_0214c84c also holds
 *   the count (an unnamed struct type takes one number, measured).
 * - *(u16 *)&mStateTimer stays: the ROM loads ldrh (unsigned), the s16
 *   member spelling loads ldrsh.
 * - daShl_c_classInit stays a free factory and the destructor stays
 *   inline (D1-then-D0 cartridge order).
 */

#include "common.h"
#include "daShl_c.h"
#include "SharedFilePtr.h"
#include "dBgCh_Gnd.h"

/* Two 8-byte pointers-to-member. The second word is the this-adjustment;
   retail stores 0 for every entry. */
typedef int (daShl_c::*ShlStateFn)();
struct ShlState {
    ShlStateFn enter;  /* +0x0 */
    ShlStateFn tick;   /* +0x8 */
};
/* Last symbol in the overlay bss. dsd runs it to the module end at
   0x0214eaa0; the three tail slots stay zero and are not copied. */
struct ShlStateEnd {
    ShlState rec;
    ShlStateFn tail[3];
};

extern "C" {


/* -- the four state records (.bss) and the model handles (.rodata) -- */
extern ShlState data_ov102_0214ea48;
extern ShlState data_ov102_0214ea58;
extern ShlState data_ov102_0214ea68;
extern ShlStateEnd data_ov102_0214ea78;
extern SharedFilePtr *data_ov102_0214d70c[];

/* -- other modules -- */
int  func_ov002_020ad660(char *self, char *clsn, char *model, int kind);
int  func_ov002_020cc16c(void *player, void *actor);
void func_020105cc(void *self, u32 flags);
u8   DecIfAbove0_Byte(u8 *counter);
u16  DecIfAbove0_Short(u16 *counter);
void Matrix4x3_FromRotationY(void *m, int angle);
int  SurfaceInfo_TestFlag0x20(int *si);
int  func_02037e84(int *si);
int  func_02037e38(unsigned int *si);
u32  func_02022d00(u32 uniqueID, u32 effectID, Fix12i x, Fix12i y, Fix12i z,
                   void *dir);

/* Including Player.h renumbers the pinned @NNN statics by 94. */
int  _ZN6Player9IsOnShellEv(void *player);  /* local extern: Player.h is +94 */
int  _ZN6Player7TryGrabER8dActor_c(void *player, dActor_c &actor);  /* local extern: Player.h is +94 */
int  _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *player, const Vector3 *v,
                                            u32 a, Fix12i f, u8 b, u8 c,
                                            u8 d);
int  _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
         u32 uniqueID, u32 effectID, Fix12i x, Fix12i y, Fix12i z, void *dir,
         void *cb);
/* local extern: including Sound.h renumbers the pinned @NNN statics by 15. */
int  _ZN5Sound8PlayLongEjjjRK7Vector3s(u32 handle, u32 a, u32 soundID,
                                       const Vector3 &pos, s16 e);

void _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(
         void *self, const Vector3_16 &dir, Player &player, s32 speed);
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *self, dActor_c *owner,
                                              Fix12i radius, Fix12i height,
                                              unsigned int d, unsigned int e);
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
         void *self, dActor_c *owner, Fix12i radius, Fix12i height,
         Vector3_16 *d, int e);
void _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(
         void *self, dExtShadowModel_c *sm, Matrix4x3 *m, Fix12i radius,
         int height, u8 flags);

}

/* Plain three-word position: Vector3 members here emit an extra
   destructor function into this TU (measured). */
struct ShlPos {
    int x, y, z;
};

// @symbol daShl_c_classInit
extern "C" daShl_c *daShl_c_classInit()
{
    return new daShl_c();
}

// @symbol _ZN7daShl_c13OnYoshiTryEatEv
s32 daShl_c::OnYoshiTryEat()
{
    return mModelIndex == 0 ? 6 : 5;
}

// @symbol _ZN7daShl_c13InitResourcesEv
/* Builds the shell: model, shadow, both collision volumes, initial motion,
 * and the starting state. Any failure returns 0 and the spawn is abandoned.
 *
 * One spawn word does double duty. param1 bit 0 picks which of the two
 * shared model files to load and is kept in mModelIndex so CleanupResources
 * can release the same one; bit 4 goes to mVariant.
 */
s32 daShl_c::InitResources()
{
    mModelIndex = param1 & 1;
    mVariant = (param1 >> 4) & 1;
    BMD_File *bmd = (BMD_File *)Model::LoadFile(*data_ov102_0214d70c[mModelIndex]);
    if (mModel.SetFile(bmd, 1, 1) == 0)
        return 0;
    if (mShadowModel.InitCylinder() == 0)
        return 0;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCc_c, this, 0x3c000, 0x46000, 0x100004, 0xa083c0);
    mSpawnPosX = mPosX;
    mSpawnPosY = mPosY;
    mSpawnPosZ = mPosZ;
    mVertAccel = -0x2000;          /* -2.0 */
    mTerminalVelocity = -0x32000;  /* -50.0 */
    mStateTimer = 20;
    mCarrier = 0;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mMeshClsn, this, 0x28000, 0x28000, 0, 0);
    mMeshClsn.StartDetectingWater();
    func_ov102_0214d1f8(&data_ov102_0214ea68);
    mParticleHandle_3d4 = 0;
    mParticleHandle_3d0 = mParticleHandle_3d4;
    mParticleHandle_3cc = mParticleHandle_3d0;
    mParticleHandle_3c8 = mParticleHandle_3cc;
    return 1;
}

// @symbol _ZN7daShl_c8BehaviorEv
/* One frame of the shell, in five stages: shared enemy pre-pass, Yoshi's
 * mouth, the despawn countdown, the state tick, then movement and terrain.
 */
s32 daShl_c::Behavior()
{
    if (func_ov002_020ad660((char *)this, (char *)&mMeshClsn, (char *)&mModel, 3) != 0)
        return 1;

    if (UpdateYoshiEat(mMeshClsn) != 0) {
        if (mEatenByYoshi != 0) {
            mSpawnAngleY = mPrevAngleY;
            func_ov102_0214d1f8((ShlState *)&data_ov102_0214ea78);
            mFlags &= ~0x80000u;
            mEatenByYoshi = 0;
        }
        func_ov102_0214ce60();
        mdCc_c.Clear();
        return 1;
    }

    /* The despawn countdown only runs while idle; any other state resets it,
       so a shell that is picked back up stops expiring. */
    if (mDespawnTimer != 0 && mState == &data_ov102_0214ea68) {
        if (DecIfAbove0_Byte(&mDespawnTimer) == 0) {
            MarkForDestruction();
            return 1;
        }
    } else {
        mDespawnTimer = 0;
    }

    DecIfAbove0_Short((u16 *)&mStateTimer);

    {
        ShlState *st = mState;
        int res;
        if (st->tick == 0)
            res = 1;
        else
            res = (this->*st->tick)();
        if (res == 0)
            return 1;
    }

    func_ov102_0214cbec();

    if (mVertAccel != 0) {
        UpdatePos(&mdCc_c);
        UpdateWMClsn(mMeshClsn, 0);
        if (mMeshClsn.IsOnGround() != 0 || mMeshClsn.IsOnWall() != 0) {
            /* Hitting a wall while spat out breaks the shell. */
            if (mMeshClsn.IsOnWall() != 0) {
                if (mState == (ShlState *)&data_ov102_0214ea78) {
                    PoofDust();
                    MarkForDestruction();
                    return 0;
                }
            }
            if (mMeshClsn.IsOnGround() != 0) {
                if (mState == &data_ov102_0214ea68) {
                    mVertAccel = 0;
                    mVertSpeed = 0;
                }
                if (mState != &data_ov102_0214ea48 &&
                    mState != &data_ov102_0214ea58)
                    func_ov102_0214c7fc();
            }
        }
    }

    if (mModelIndex == 0)
        func_ov102_0214c84c();
    func_ov102_0214ce60();
    mdCc_c.Clear();
    if (mState != &data_ov102_0214ea48)
        mdCc_c.Update();
    return 1;
}

// @symbol _ZN7daShl_c6RenderEv
/* Draws the shell, unless it is hidden (flag 0x40000) or blinking: below
 * 0x2d the despawn countdown skips odd frames, so the shell flashes out
 * over its last 45 frames instead of vanishing. */
s32 daShl_c::Render()
{
    int b = (mFlags & 0x40000) != 0;
    if (b != 0)
        return 1;
    {
        unsigned char v = mDespawnTimer;
        if (v < 0x2d) {
            if (v & 1)
                return 1;
        }
    }
    mModel.Render(0);
    return 1;
}

// @symbol _ZN7daShl_c16OnPendingDestroyEv
/* Empty on purpose: the ROM body is a single `bx lr`, overriding whatever
   the base does on pending destroy with nothing. */
void daShl_c::OnPendingDestroy()
{
}

// @symbol _ZN7daShl_c16CleanupResourcesEv
/* Releases the model file InitResources claimed, picked by the same index. */
s32 daShl_c::CleanupResources()
{
    data_ov102_0214d70c[mModelIndex]->Release();
    return 1;
}

// @symbol _ZN7daShl_c19func_ov102_0214d1f8EP8ShlState
/* Switch state: store the record, then run its `enter` if it has one. */
int daShl_c::func_ov102_0214d1f8(ShlState *state)
{
    mState = state;
    ShlState *st = mState;
    if (st->enter == 0)
        return 1;
    return (this->*st->enter)();
}

// @symbol _ZN7daShl_c19func_ov102_0214d1b8Ev
/* Idle: enter. Stop sliding; the variant shell also stops falling and
   becomes grabbable. */
int daShl_c::func_ov102_0214d1b8()
{
    mHorzSpeed = 0;

    if (mVariant != 0) {
        mFlags |= 0x80;
        mdCc_c.vulnFlags |= 0x1000;
        mVertAccel = 0;
    }

    return 1;
}

// @symbol _ZN7daShl_c19func_ov102_0214d1b0Ev
/* The tick of a state that has nothing to do each frame. */
int daShl_c::func_ov102_0214d1b0()
{
    return 1;
}

// @symbol _ZN7daShl_c19func_ov102_0214d148Ev
/* Sliding: enter. Face mSpawnAngleY -- the kicker's heading, or the one
   Yoshi spat it along -- slide at 30.0, and arm the second cylinder. */
int daShl_c::func_ov102_0214d148()
{
    mPrevAngleY = mSpawnAngleY;
    mAngleY = mPrevAngleY;
    mHorzSpeed = 0x1e000;
    mVertAccel = -0x2000;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCc_c2, this, 0x28000, 0x3c000, 0x102002, 0);
    return 1;
}

// @symbol _ZN7daShl_c19func_ov102_0214d114Ev
/* Sliding: tick. Spin, and keep the second cylinder live. */
int daShl_c::func_ov102_0214d114()
{
    mAngleY += 0x1000;
    mdCc_c2.Clear();
    mdCc_c2.Update();
    return 1;
}

// @symbol _ZN7daShl_c19func_ov102_0214d0bcEv
/* Ridden: enter. The body cylinder is re-initialised with no extent. */
int daShl_c::func_ov102_0214d0bc()
{
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCc_c, this, 0, 0, 0x100002, 0);
    mFlags &= ~3;
    mDespawnTimer = 100;
    return 1;
}

// @symbol _ZN7daShl_c19func_ov102_0214d044Ev
/* Ridden: tick. Follow the rider while he is still on the shell; once he
   is off, the shell breaks. */
int daShl_c::func_ov102_0214d044()
{
    /* The riderPos/angle/x/ret locals below are needed: without them the
       function is 4 bytes shorter. */
    if (mCarrier == 0 || _ZN6Player9IsOnShellEv(mCarrier) == 0) {
        PoofDust();
        MarkForDestruction();
        return 0;
    }
    {
        dActor_c *rider = mCarrier;
        s32 *riderPos = &rider->mPosX;
        s16 *angle = &mAngleY;
        int x = riderPos[0];
        int ret = 1;
        mPosX = x;
        mPosY = riderPos[1];
        mPosZ = riderPos[2];
        *angle = *angle + 0x1000;
        return ret;
    }
}

// @symbol _ZN7daShl_c19func_ov102_0214d020Ev
/* Held: enter. */
int daShl_c::func_ov102_0214d020()
{
    mdCc_c.flags |= 2;
    mdCc_c.flags &= ~4;
    return 1;
}

// @symbol _ZN7daShl_c19func_ov102_0214cfe4Ev
/* Held: tick. Once the holder lets go (flag 0x100 clears), it breaks. */
int daShl_c::func_ov102_0214cfe4()
{
    int b = (mFlags & 0x100) != 0;
    if (!b) {
        PoofDust();
        MarkForDestruction();
    }
    return 1;
}

// @symbol _ZN7daShl_c19func_ov102_0214cf98EP8dActor_c
/* The player jumps on: remember him and switch to ridden. */
int daShl_c::func_ov102_0214cf98(dActor_c *player)
{
    if (mCarrier != 0)
        return 0;
    mCarrier = player;
    if (mState != &data_ov102_0214ea48)
        func_ov102_0214d1f8(&data_ov102_0214ea48);
    return 1;
}

// @symbol _ZN7daShl_c19func_ov102_0214cf4cEP8dActor_c
/* Kicked: take the kicker's heading and switch to the sliding state. */
int daShl_c::func_ov102_0214cf4c(dActor_c *kicker)
{
    if (mState == (ShlState *)&data_ov102_0214ea78)
        return 0;
    mSpawnAngleY = kicker->mAngleY;
    func_ov102_0214d1f8((ShlState *)&data_ov102_0214ea78);
    return 1;
}

// @symbol _ZN7daShl_c19func_ov102_0214ce60Ev
/* Model matrix and drop shadow. While carried (flag 0x4000) the model
   follows the carrier's hands; otherwise it sits at mPos turned by
   mAngleY. Flag 0x40000 (hidden) skips the shadow. */
void daShl_c::func_ov102_0214ce60()
{
    int carried = (mFlags & 0x4000) != 0;
    if (carried) {
        Vector3 offset;
        offset.x = 0x1e000;
        offset.y = -0x1e000;
        offset.z = 0x32000;
        mModel.mat4x3 = *UpdateCarry(*(Player *)mCarrier, offset);
    } else {
        Matrix4x3_FromRotationY(&mModel.mat4x3, mAngleY);
        mModel.mat4x3.m[9] = mPosX >> 3;
        mModel.mat4x3.m[10] = mPosY >> 3;
        mModel.mat4x3.m[11] = mPosZ >> 3;
    }
    int hidden = (mFlags & 0x40000) != 0;
    if (hidden)
        return;
    _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(this, &mShadowModel, &mModel.mat4x3, 0x50000, 0x50000, 0xf);
}

// @symbol _ZN7daShl_c19func_ov102_0214cbecEv
/* What touching the player does, read off the body cylinder's last hit.
   An invincible player (hit flag 0x10) smashes the shell. The variant
   shell only lets itself be picked up (0x1000) while idle. Otherwise
   model 1 is kicked away by an attack (0x3c0) and hurts the player while
   sliding; model 0 hurts while sliding and, idle, can be jumped on and
   ridden. */
void daShl_c::func_ov102_0214cbec()
{
    dActor_c *player;
    s32 hit;
    u32 id;
    Vector3 v1, v2;

    id = mdCc_c.otherOwner;
    if (id == 0)
        return;
    player = dActor_c::FindWithID(id);
    if (!player)
        return;

    {
        int isPlayer = (int)(player->actorID == 0xbf);
        if (!isPlayer)
            return;
    }
    /* Unsigned read (ldrh): the s16 field spelling loads ldrsh. */
    if (*(u16 *)&mStateTimer != 0)
        return;

    hit = mdCc_c.hitFlags;
    if (hit & 0x10) {
        Vector3_16 dir;
        dir.x = 0x2000;
        dir.y = 0;
        dir.z = 0;
        _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(this, dir, *(Player *)player, mdCc_c.height >> 1);
        return;
    }

    if (mVariant != 0) {
        if (mState != &data_ov102_0214ea68)
            return;
        if (!(hit & 0x1000))
            return;
        if (_ZN6Player7TryGrabER8dActor_c(player, *this) == 0)
            return;
        mCarrier = player;
        func_ov102_0214d1f8(&data_ov102_0214ea58);
        return;
    }

    if (mModelIndex != 0) {
        s32 attack = hit & 0x3c0;
        if (attack) {
            func_020105cc(this, attack);
            func_ov102_0214cf4c(player);
            return;
        }
        if (mState != (ShlState *)&data_ov102_0214ea78)
            return;
        v1.x = mPosX;
        v1.y = mPosY;
        v1.z = mPosZ;
        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(player, &v1, 1, 0xc000, 1, 0, 1);
        return;
    }

    if (mState == (ShlState *)&data_ov102_0214ea78) {
        v2.x = mPosX;
        v2.y = mPosY;
        v2.z = mPosZ;
        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(player, &v2, 1, 0xc000, 1, 0, 1);
        return;
    }

    if (mState != &data_ov102_0214ea68)
        return;
    if (hit & 0x26fe0)
        return;
    if (func_ov002_020cc16c(player, this) != 1)
        return;
    func_ov102_0214cf98(player);
}

// @symbol _ZN7daShl_c19func_ov102_0214c84cEv
/* Model 0 only (Behavior gates it). While ridden, probe the ground from
   50.0 above the shell and pick the trail particles and the looping
   surface sound from what it finds; otherwise just the idle trail. Water
   detection is on, so ground.clsnY is the hit height of a water surface
   or a floor. */
void daShl_c::func_ov102_0214c84c()
{
    /* One object: as two separate locals the function is 64 bytes
       shorter (measured). */
    struct { ShlPos trail; ShlPos probe; } pos;

    pos.trail.x = mPosX;
    pos.trail.y = mPosY;
    pos.trail.z = mPosZ;

    if (mState == &data_ov102_0214ea48
        && mCarrier != 0
        && *((u8 *)mCarrier + 0x6de) == 0)  /* Player::mIsAirborne */
    {
        dBgCh_Gnd ground;
        ground.StartDetectingWater();
        {
            int vx = mPosX;
            int vz = mPosZ;
            int vy = mPosY + 0x32000;
            pos.probe.x = vx;
            pos.probe.y = vy;
            pos.probe.z = vz;
        }
        ground.SetObjAndPos(*(Vector3 *)&pos.probe, this);
        if (ground.DetectClsn())
        {
            if (SurfaceInfo_TestFlag0x20((int *)&ground.surface))
            {
                pos.trail.y = ground.clsnY + 0x3c000;
                mParticleHandle_3d4 = 0;
                mParticleHandle_3c8 = mParticleHandle_3d4;
                mParticleHandle_3cc = func_02022d00(mParticleHandle_3cc, 0xe2, pos.trail.x, pos.trail.y, pos.trail.z, 0);
                mParticleHandle_3d0 = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                    mParticleHandle_3d0, 0xe3, pos.trail.x, pos.trail.y, pos.trail.z, 0, 0);
                if (mSoundID != func_02037e84((int *)&ground.surface) + 0xf2)
                    mSoundHandle = 0;
                mSoundID = func_02037e84((int *)&ground.surface) + 0xf2;
                mSoundHandle =
                    _ZN5Sound8PlayLongEjjjRK7Vector3s(mSoundHandle, 0, mSoundID, mCamSpacePos, 0);
                return;
            }
            if (func_02037e38((unsigned int *)&ground.surface) == 1)
            {
                if (func_02037e84((int *)&ground.surface) == 7)
                {
                    pos.trail.y = ground.clsnY + 0x3c000;
                    mParticleHandle_3d4 = 0;
                    mParticleHandle_3c8 = mParticleHandle_3d4;
                    mParticleHandle_3cc = func_02022d00(mParticleHandle_3cc, 0xe2, pos.trail.x, pos.trail.y, pos.trail.z, 0);
                    mParticleHandle_3d0 = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                        mParticleHandle_3d0, 0xe3, pos.trail.x, pos.trail.y, pos.trail.z, 0, 0);
                    if (mSoundID != func_02037e84((int *)&ground.surface) + 0xf2)
                        mSoundHandle = 0;
                    mSoundID = func_02037e84((int *)&ground.surface) + 0xf2;
                    mSoundHandle = _ZN5Sound8PlayLongEjjjRK7Vector3s(mSoundHandle, 0, mSoundID,
                                                                     mCamSpacePos, 0);
                }
                else
                {
                    pos.trail.y = ground.clsnY + 0xa000;
                    mParticleHandle_3d0 = 0;
                    mParticleHandle_3cc = mParticleHandle_3d0;
                    mParticleHandle_3d4 = mParticleHandle_3cc;
                    mParticleHandle_3c8 = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                        mParticleHandle_3c8, 0xe1, pos.trail.x, pos.trail.y, pos.trail.z, 0, 0);
                    if (mSoundID != 0x102)
                        mSoundHandle = 0;
                    mSoundID = 0x102;
                    mSoundHandle = _ZN5Sound8PlayLongEjjjRK7Vector3s(mSoundHandle, 0, mSoundID,
                                                                     mCamSpacePos, 0);
                }
                return;
            }
            if (mSoundID != func_02037e84((int *)&ground.surface) + 0xf2)
                mSoundHandle = 0;
            mSoundID = func_02037e84((int *)&ground.surface) + 0xf2;
            mSoundHandle =
                _ZN5Sound8PlayLongEjjjRK7Vector3s(mSoundHandle, 0, mSoundID, mCamSpacePos, 0);
        }
        else
        {
            mSoundHandle = 0;
            mSoundID = 0;
        }
    }

    mParticleHandle_3d0 = 0;
    mParticleHandle_3cc = mParticleHandle_3d0;
    mParticleHandle_3c8 = mParticleHandle_3cc;
    pos.trail.y = pos.trail.y + 0x1e000;
    mParticleHandle_3d4 = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        mParticleHandle_3d4, 0xe0, pos.trail.x, pos.trail.y, pos.trail.z, 0, 0);
}

// @symbol _ZN7daShl_c19func_ov102_0214c7fcEv
/* Landed on a floor whose CLPS entry has bit 0x20 set: poof and go. Bit
   0x20 is water per CLPS.h, and this file agrees: InitResources turns on
   mMeshClsn.StartDetectingWater(), and func_ov102_0214c84c's probe calls
   StartDetectingWater before it tests the same bit. The floor result is
   a dBgPi. */
void daShl_c::func_ov102_0214c7fc()
{
    if (!mMeshClsn.IsOnGround())
        return;
    dBgPi *floor = (dBgPi *)mMeshClsn.GetFloorResult();
    if (!SurfaceInfo_TestFlag0x20((int *)&floor->surface))
        return;
    PoofDust();
    MarkForDestruction();
}

// @symbol _ZN7daShl_cD1Ev
// @symbol _ZN7daShl_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The inline destructor in the header
   emits D1 then D0 -- the cartridge's order -- and no D2. */

/* Source order is the retail copy order. Each field is an 8-byte
   {function, 0} descriptor; __sinit_d_a_shl.cpp copies them in. */
extern "C" ShlState data_ov102_0214ea68 = {
    &daShl_c::func_ov102_0214d1b8,
    &daShl_c::func_ov102_0214d1b0,
};
extern "C" ShlStateEnd data_ov102_0214ea78 = {
    { &daShl_c::func_ov102_0214d148, &daShl_c::func_ov102_0214d114 },
};
extern "C" ShlState data_ov102_0214ea48 = {
    &daShl_c::func_ov102_0214d0bc,
    &daShl_c::func_ov102_0214d044,
};
extern "C" ShlState data_ov102_0214ea58 = {
    &daShl_c::func_ov102_0214d020,
    &daShl_c::func_ov102_0214cfe4,
};
