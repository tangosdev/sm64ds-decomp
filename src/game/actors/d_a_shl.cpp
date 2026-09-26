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
 * records are .bss, filled by __sinit_ov102_0214dfac from the PMF
 * constants at ov102:0x0214e5d4..0x0214e614:
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
 * - The func_ov102_* routines keep their ROM labels and C linkage; the
 *   state functions are reached only through the pointer-to-member records.
 * - func_ov102_0214c84c's ground probe is a raw 0x50-byte dBgCh_Gnd
 *   buffer driven through its mangled entry points.
 * - data_ov102_0214d70c is this TU's .rodata pair of model-file handles
 *   and the state records are .bss; this text-only TU claims neither.
 */

#include "common.h"
#include "daShl_c.h"
#include "SharedFilePtr.h"

/* A state record: two pointers-to-member, called on the actor itself.
   The state functions have C linkage here, so the owner class is an
   empty stand-in for the actor. */
struct ShlStateHost {};
typedef int (ShlStateHost::*ShlStateFn)();
struct ShlState {
    ShlStateFn enter;  /* +0x0 */
    ShlStateFn tick;   /* +0x8 */
};

extern "C" {

/* -- this TU's own members, forward-declared: mwcc lays .text down in
      reverse source order, so every call here is a forward reference. */
void func_ov102_0214c7fc(daShl_c *self);
void func_ov102_0214c84c(daShl_c *self);
void func_ov102_0214cbec(daShl_c *self);
void func_ov102_0214ce60(daShl_c *self);
int  func_ov102_0214cf4c(daShl_c *self, dActor_c *kicker);
int  func_ov102_0214cf98(daShl_c *self, dActor_c *player);
int  func_ov102_0214d1f8(daShl_c *self, ShlState *state);

/* -- the four state records (.bss) and the model handles (.rodata) -- */
extern ShlState data_ov102_0214ea48;
extern ShlState data_ov102_0214ea58;
extern ShlState data_ov102_0214ea68;
extern ShlState data_ov102_0214ea78;
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
void *_ZNK10dBgCh_Actr14GetFloorResultEv(void *self);

void *_ZN9dBgCh_GndC1Ev(void *self);
void *_ZN9dBgCh_GndD1Ev(void *self);
void _ZN5dBgCh19StartDetectingWaterEv(void *self);
void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(void *self,
                                                      const Vector3 &pos,
                                                      dActor_c *actor);
int  _ZN9dBgCh_Gnd10DetectClsnEv(void *self);

int  _ZN6Player9IsOnShellEv(void *player);
int  _ZN6Player7TryGrabER8dActor_c(void *player, dActor_c &actor);
int  _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *player, const Vector3 *v,
                                            u32 a, Fix12i f, u8 b, u8 c,
                                            u8 d);
int  _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
         u32 uniqueID, u32 effectID, Fix12i x, Fix12i y, Fix12i z, void *dir,
         void *cb);
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
void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
         void *self, ShadowModel *sm, Matrix4x3 *m, Fix12i radius,
         int height, u8 flags);

}

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
    func_ov102_0214d1f8(this, &data_ov102_0214ea68);
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
            func_ov102_0214d1f8(this, &data_ov102_0214ea78);
            mFlags &= ~0x80000u;
            mEatenByYoshi = 0;
        }
        func_ov102_0214ce60(this);
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
        ShlState *st = (ShlState *)mState;
        int res;
        if (st->tick == 0)
            res = 1;
        else
            res = (((ShlStateHost *)this)->*st->tick)();
        if (res == 0)
            return 1;
    }

    func_ov102_0214cbec(this);

    if (mVertAccel != 0) {
        UpdatePos(&mdCc_c);
        UpdateWMClsn(mMeshClsn, 0);
        if (mMeshClsn.IsOnGround() != 0 || mMeshClsn.IsOnWall() != 0) {
            /* Hitting a wall while spat out breaks the shell. */
            if (mMeshClsn.IsOnWall() != 0) {
                if (mState == &data_ov102_0214ea78) {
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
                    func_ov102_0214c7fc(this);
            }
        }
    }

    if (mModelIndex == 0)
        func_ov102_0214c84c(this);
    func_ov102_0214ce60(this);
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

// @symbol func_ov102_0214d1f8
/* Switch state: store the record, then run its `enter` if it has one. */
int func_ov102_0214d1f8(daShl_c *self, ShlState *state)
{
    self->mState = state;
    ShlState *st = (ShlState *)self->mState;
    if (st->enter == 0)
        return 1;
    return (((ShlStateHost *)self)->*st->enter)();
}

// @symbol func_ov102_0214d1b8
/* Idle: enter. Stop sliding; the variant shell also stops falling and
   becomes grabbable. */
extern "C" int func_ov102_0214d1b8(daShl_c *self)
{
    self->mHorzSpeed = 0;

    if (self->mVariant != 0) {
        self->mFlags |= 0x80;
        self->mdCc_c.vulnFlags |= 0x1000;
        self->mVertAccel = 0;
    }

    return 1;
}

// @symbol func_ov102_0214d1b0
/* The tick of a state that has nothing to do each frame. */
extern "C" int func_ov102_0214d1b0(void)
{
    return 1;
}

// @symbol func_ov102_0214d148
/* Sliding: enter. Face mSpawnAngleY -- the kicker's heading, or the one
   Yoshi spat it along -- slide at 30.0, and arm the second cylinder. */
extern "C" int func_ov102_0214d148(daShl_c *self)
{
    self->mPrevAngleY = self->mSpawnAngleY;
    self->mAngleY = self->mPrevAngleY;
    self->mHorzSpeed = 0x1e000;
    self->mVertAccel = -0x2000;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&self->mdCc_c2, self, 0x28000, 0x3c000, 0x102002, 0);
    return 1;
}

// @symbol func_ov102_0214d114
/* Sliding: tick. Spin, and keep the second cylinder live. */
extern "C" int func_ov102_0214d114(daShl_c *self)
{
    self->mAngleY += 0x1000;
    self->mdCc_c2.Clear();
    self->mdCc_c2.Update();
    return 1;
}

// @symbol func_ov102_0214d0bc
/* Ridden: enter. The body cylinder is re-initialised with no extent. */
extern "C" int func_ov102_0214d0bc(daShl_c *self)
{
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&self->mdCc_c, self, 0, 0, 0x100002, 0);
    self->mFlags &= ~3;
    self->mDespawnTimer = 100;
    return 1;
}

// @symbol func_ov102_0214d044
/* Ridden: tick. Follow the rider while he is still on the shell; once he
   is off, the shell breaks. */
extern "C" int func_ov102_0214d044(daShl_c *self)
{
    if (self->mCarrier != 0) {
        if (_ZN6Player9IsOnShellEv((void *)self->mCarrier) != 0)
            goto follow;
    }
    self->PoofDust();
    self->MarkForDestruction();
    return 0;
follow:
    {
        dActor_c *rider = (dActor_c *)self->mCarrier;
        s32 *riderPos = &rider->mPosX;
        s16 *angle = &self->mAngleY;
        int x = riderPos[0];
        int ret = 1;
        self->mPosX = x;
        self->mPosY = riderPos[1];
        self->mPosZ = riderPos[2];
        *angle = *angle + 0x1000;
        return ret;
    }
}

// @symbol func_ov102_0214d020
/* Held: enter. */
extern "C" int func_ov102_0214d020(daShl_c *self)
{
    self->mdCc_c.flags |= 2;
    self->mdCc_c.flags &= ~4;
    return 1;
}

// @symbol func_ov102_0214cfe4
/* Held: tick. Once the holder lets go (flag 0x100 clears), it breaks. */
extern "C" int func_ov102_0214cfe4(daShl_c *self)
{
    int b = (self->mFlags & 0x100) != 0;
    if (!b) {
        self->PoofDust();
        self->MarkForDestruction();
    }
    return 1;
}

// @symbol func_ov102_0214cf98
/* The player jumps on: remember him and switch to ridden. */
int func_ov102_0214cf98(daShl_c *self, dActor_c *player)
{
    if (self->mCarrier != 0)
        return 0;
    self->mCarrier = (s32)player;
    if (self->mState != &data_ov102_0214ea48)
        func_ov102_0214d1f8(self, &data_ov102_0214ea48);
    return 1;
}

// @symbol func_ov102_0214cf4c
/* Kicked: take the kicker's heading and switch to the sliding state. */
int func_ov102_0214cf4c(daShl_c *self, dActor_c *kicker)
{
    if (self->mState == &data_ov102_0214ea78)
        return 0;
    self->mSpawnAngleY = kicker->mAngleY;
    func_ov102_0214d1f8(self, &data_ov102_0214ea78);
    return 1;
}

// @symbol func_ov102_0214ce60
/* Model matrix and drop shadow. While carried (flag 0x4000) the model
   follows the carrier's hands; otherwise it sits at mPos turned by
   mAngleY. Flag 0x40000 (hidden) skips the shadow. */
void func_ov102_0214ce60(daShl_c *self)
{
    int carried = (self->mFlags & 0x4000) != 0;
    if (carried) {
        Vector3 offset;
        offset.x = 0x1e000;
        offset.y = -0x1e000;
        offset.z = 0x32000;
        self->mModel.mat4x3 = *self->UpdateCarry(*(Player *)self->mCarrier, offset);
    } else {
        Matrix4x3_FromRotationY(&self->mModel.mat4x3, self->mAngleY);
        self->mModel.mat4x3.m[9] = self->mPosX >> 3;
        self->mModel.mat4x3.m[10] = self->mPosY >> 3;
        self->mModel.mat4x3.m[11] = self->mPosZ >> 3;
    }
    int hidden = (self->mFlags & 0x40000) != 0;
    if (hidden)
        return;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(self, &self->mShadowModel, &self->mModel.mat4x3, 0x50000, 0x50000, 0xf);
}

// @symbol func_ov102_0214cbec
/* What touching the player does, read off the body cylinder's last hit.
   An invincible player (hit flag 0x10) smashes the shell. The variant
   shell only lets itself be picked up (0x1000) while idle. Otherwise
   model 1 is kicked away by an attack (0x3c0) and hurts the player while
   sliding; model 0 hurts while sliding and, idle, can be jumped on and
   ridden. */
void func_ov102_0214cbec(daShl_c *self)
{
    void *player;
    s32 hit;
    u32 id;
    Vector3 v1, v2;

    id = self->mdCc_c.otherOwner;
    if (id == 0)
        return;
    player = dActor_c::FindWithID(id);
    if (!player)
        return;

    {
        int isPlayer = (int)(((dActor_c *)player)->actorID == 0xbf);
        if (!isPlayer)
            return;
    }
    /* Unsigned read (ldrh): the s16 field spelling loads ldrsh. */
    if (*(u16 *)&self->mStateTimer != 0)
        return;

    hit = self->mdCc_c.hitFlags;
    if (hit & 0x10) {
        s16 dir[3];
        dir[0] = 0x2000;
        dir[1] = 0;
        dir[2] = 0;
        _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(self, *(Vector3_16 *)dir, *(Player *)player, self->mdCc_c.height >> 1);
        return;
    }

    if (self->mVariant != 0) {
        if (self->mState != &data_ov102_0214ea68)
            return;
        if (!(hit & 0x1000))
            return;
        if (_ZN6Player7TryGrabER8dActor_c(player, *self) == 0)
            return;
        self->mCarrier = (s32)player;
        func_ov102_0214d1f8(self, &data_ov102_0214ea58);
        return;
    }

    if (self->mModelIndex != 0) {
        s32 attack = hit & 0x3c0;
        if (attack) {
            func_020105cc(self, attack);
            func_ov102_0214cf4c(self, (dActor_c *)player);
            return;
        }
        if (self->mState != &data_ov102_0214ea78)
            return;
        v1.x = self->mPosX;
        v1.y = self->mPosY;
        v1.z = self->mPosZ;
        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(player, &v1, 1, 0xc000, 1, 0, 1);
        return;
    }

    if (self->mState == &data_ov102_0214ea78) {
        v2.x = self->mPosX;
        v2.y = self->mPosY;
        v2.z = self->mPosZ;
        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(player, &v2, 1, 0xc000, 1, 0, 1);
        return;
    }

    if (self->mState != &data_ov102_0214ea68)
        return;
    if (hit & 0x26fe0)
        return;
    if (func_ov002_020cc16c(player, self) != 1)
        return;
    func_ov102_0214cf98(self, (dActor_c *)player);
}

// @symbol func_ov102_0214c84c
/* Model 0 only (Behavior gates it). While ridden, probe the ground from
   50.0 above the shell and pick the trail particles and the looping
   surface sound from what it finds; otherwise just the idle trail. */
void func_ov102_0214c84c(daShl_c *self)
{
    int pv[6];
    char rg[0x50];

    pv[0] = self->mPosX;
    pv[1] = self->mPosY;
    pv[2] = self->mPosZ;

    if (self->mState == &data_ov102_0214ea48
        && self->mCarrier != 0
        && *((u8 *)self->mCarrier + 0x6de) == 0)
    {
        _ZN9dBgCh_GndC1Ev(rg);
        _ZN5dBgCh19StartDetectingWaterEv(rg);
        {
            int vx = self->mPosX;
            int vz = self->mPosZ;
            int vy = self->mPosY + 0x32000;
            pv[3] = vx;
            pv[4] = vy;
            pv[5] = vz;
        }
        _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(rg, *(Vector3 *)&pv[3], self);
        if (_ZN9dBgCh_Gnd10DetectClsnEv(rg))
        {
            if (SurfaceInfo_TestFlag0x20((int *)(rg + 0x14)))
            {
                pv[1] = *(int *)(rg + 0x44) + 0x3c000;
                self->mParticleHandle_3d4 = 0;
                self->mParticleHandle_3c8 = self->mParticleHandle_3d4;
                self->mParticleHandle_3cc = func_02022d00(self->mParticleHandle_3cc, 0xe2, pv[0], pv[1], pv[2], 0);
                self->mParticleHandle_3d0 = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                    self->mParticleHandle_3d0, 0xe3, pv[0], pv[1], pv[2], 0, 0);
                if (self->mSoundID != func_02037e84((int *)(rg + 0x14)) + 0xf2)
                    self->mSoundHandle = 0;
                self->mSoundID = func_02037e84((int *)(rg + 0x14)) + 0xf2;
                self->mSoundHandle = _ZN5Sound8PlayLongEjjjRK7Vector3s(
                    self->mSoundHandle, 0, self->mSoundID, *(Vector3 *)&self->mCamSpacePosX, 0);
                _ZN9dBgCh_GndD1Ev(rg);
                return;
            }
            if (func_02037e38((unsigned int *)(rg + 0x14)) == 1)
            {
                if (func_02037e84((int *)(rg + 0x14)) == 7)
                {
                    pv[1] = *(int *)(rg + 0x44) + 0x3c000;
                    self->mParticleHandle_3d4 = 0;
                    self->mParticleHandle_3c8 = self->mParticleHandle_3d4;
                    self->mParticleHandle_3cc = func_02022d00(self->mParticleHandle_3cc, 0xe2, pv[0], pv[1], pv[2], 0);
                    self->mParticleHandle_3d0 = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                        self->mParticleHandle_3d0, 0xe3, pv[0], pv[1], pv[2], 0, 0);
                    if (self->mSoundID != func_02037e84((int *)(rg + 0x14)) + 0xf2)
                        self->mSoundHandle = 0;
                    self->mSoundID = func_02037e84((int *)(rg + 0x14)) + 0xf2;
                    self->mSoundHandle = _ZN5Sound8PlayLongEjjjRK7Vector3s(
                        self->mSoundHandle, 0, self->mSoundID, *(Vector3 *)&self->mCamSpacePosX, 0);
                }
                else
                {
                    pv[1] = *(int *)(rg + 0x44) + 0xa000;
                    self->mParticleHandle_3d0 = 0;
                    self->mParticleHandle_3cc = self->mParticleHandle_3d0;
                    self->mParticleHandle_3d4 = self->mParticleHandle_3cc;
                    self->mParticleHandle_3c8 = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                        self->mParticleHandle_3c8, 0xe1, pv[0], pv[1], pv[2], 0, 0);
                    if (self->mSoundID != 0x102)
                        self->mSoundHandle = 0;
                    self->mSoundID = 0x102;
                    self->mSoundHandle = _ZN5Sound8PlayLongEjjjRK7Vector3s(
                        self->mSoundHandle, 0, self->mSoundID, *(Vector3 *)&self->mCamSpacePosX, 0);
                }
                _ZN9dBgCh_GndD1Ev(rg);
                return;
            }
            if (self->mSoundID != func_02037e84((int *)(rg + 0x14)) + 0xf2)
                self->mSoundHandle = 0;
            self->mSoundID = func_02037e84((int *)(rg + 0x14)) + 0xf2;
            self->mSoundHandle = _ZN5Sound8PlayLongEjjjRK7Vector3s(
                self->mSoundHandle, 0, self->mSoundID, *(Vector3 *)&self->mCamSpacePosX, 0);
        }
        else
        {
            self->mSoundHandle = 0;
            self->mSoundID = 0;
        }
        _ZN9dBgCh_GndD1Ev(rg);
    }

    self->mParticleHandle_3d0 = 0;
    self->mParticleHandle_3cc = self->mParticleHandle_3d0;
    self->mParticleHandle_3c8 = self->mParticleHandle_3cc;
    pv[1] = pv[1] + 0x1e000;
    self->mParticleHandle_3d4 = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        self->mParticleHandle_3d4, 0xe0, pv[0], pv[1], pv[2], 0, 0);
}

// @symbol func_ov102_0214c7fc
/* Landed on a floor whose surface tests flag 0x20: poof and go. */
void func_ov102_0214c7fc(daShl_c *self)
{
    if (!self->mMeshClsn.IsOnGround())
        return;
    void *floor = _ZNK10dBgCh_Actr14GetFloorResultEv(&self->mMeshClsn);
    if (!SurfaceInfo_TestFlag0x20((int *)((char *)floor + 4)))
        return;
    self->PoofDust();
    self->MarkForDestruction();
}

// @symbol _ZN7daShl_cD1Ev
// @symbol _ZN7daShl_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The inline destructor in the header
   emits D1 then D0 -- the cartridge's order -- and no D2. */
