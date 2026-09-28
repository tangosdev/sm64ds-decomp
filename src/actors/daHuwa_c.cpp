//cpp
/* daHuwa_c, the Spindrift (HUWAHUWA). ov081, twelve functions.
 *
 * common.h is first so Matrix4x3 stays the flat { s32 m[12]; }. The death
 * and shadow helpers index that matrix through .m. daHuwa_c.h pulls in
 * math/Matrix.h, whose nested spelling would stand instead if it were first.
 *
 * #pragma defer_codegen off emits .text in source order. One out-of-line
 * destructor is the key function, so this TU emits _ZTV/_ZTI/_ZTS and a D2
 * the cartridge has no home for (manifest: deadstrip).
 *
 * daHuwa_c_classInit and g_profile_HUWAHUWA stay in other files. The two
 * file handles are constructed by __sinit_ov081_021280e8 (file ids 813 and
 * 814); this TU only loads and releases them.
 *
 * deslop leftovers:
 * - func_ov081_021237ec SpawnCoins as dActor_c::SpawnCoins: 0x124 -> 0x130 (+12).
 *   Particle::System::NewSimple with Fix12<int> x/y/z: 0x124 -> 0x148 (+36).
 * - func_ov081_02123910 `actorID != 0xbf` instead of the int compare: 0x210 -> 0x200
 *   (-16). KillByInvincibleChar as the method: 0x210 -> 0x218 (+8). Player::Hurt
 *   with Fix12<int> knockback: 0x210 -> 0x21c (+12). SpinBounce with Fix12<int>:
 *   0x210 -> 0x21c (+12).
 * - func_ov081_02123b20 direct mFlags test instead of the int 0/1: 0xcc -> 0xc0
 *   (-12). Each DropShadowRadHeight method call: 0xcc -> 0xdc (+16).
 * - Render direct mFlags test instead of the ? 1 : 0: 0x50 -> 0x44 (-12).
 * - Behavior copying player->mPosX/Y/Z instead of int* at player+0x5c: 0x264 -> 0x260
 *   (-4).
 * - InitResources ModelAnim::SetAnim: 0x108 -> 0x114 (+12). dCcAc_c::Init:
 *   0x108 -> 0x120 (+24). dBgCh_Actr::Init stays 0x108 but the header's Fix12i
 *   parameters mangle to _ZN10dBgCh_Actr4InitEP8dActor_ciiP10Vector3_16S3_,
 *   not the ROM's 5Fix12IiE symbol.
 */

#pragma defer_codegen off

#include "common.h"
#include "decl_Enemy.h"
#include "daHuwa_c.h"
#include "Player.h"
#include "SharedFilePtr.h"

/* Model file (sinit id 813) and animation (sinit id 814). SharedFilePtr has
 * no fields; SetAnim reads the loaded BCA out of the second word. */
extern SharedFilePtr data_ov081_02128d60;
extern SharedFilePtr data_ov081_02128d68;

struct HuwaLoadedFile {
    int fileId;
    BCA_File *file;
};

extern "C" {
extern struct Matrix4x3 data_020a0e68;
extern int func_ov002_020e10a8(void *);
extern void func_0201267c(unsigned int id, const Vector3 *pos);

void Matrix4x3_FromTranslation(struct Matrix4x3 *m, int x, int y, int z);
void Matrix4x3_FromRotationY(void *m, int angle);
void MulMat4x3Mat4x3(const int *a, const int *b, int *dst);
void SubVec3(Vector3 *a, Vector3 *b, Vector3 *out);
void AddVec3(Vector3 *a, Vector3 *b, Vector3 *out);
void Vec3_LslInPlace(Vector3 *v, int shift);
int Vec3_HorzDist(const Vector3 *a, const Vector3 *b);
short Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);

/* Scalar stand-ins for Fix12<int> by-value callees. The header methods
 * mangle to these same symbols and home the argument. */
int _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(void *self, Vector3 *pos, unsigned int count, int spread, short angle);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void *self, ShadowModel &shadow, Matrix4x3 &mtx, int radius, int height, unsigned int opacity);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, BCA_File *file, int flags, int speed, unsigned int startFrame);
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *self, dActor_c *actor, int radius, int height, unsigned int flags, unsigned int vulnFlags);
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *self, dActor_c *actor, int radius, int height, void *a, void *b);
/* dEnemyBase_c::KillByInvincibleChar takes a Fix12<int> by value, so the header
   member form size-DIFFs (0x210 -> 0x218, see the leftover note). decl_Enemy.h
   already declares the ROM symbol with plain scalars; use that one spelling
   rather than adding a third local copy. */
void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *player, void *pos, unsigned int n, int knockback, unsigned int a, unsigned int b, unsigned int c);
void _ZN6Player10SpinBounceE5Fix12IiE(void *player, int speed);
}

int ApproachLinear(int &value, int target, int step);
int ApproachLinear(short &value, short target, short step);

/* dCc_c hitFlags bits this actor tests. 0x10 / 0x2000 / 0x4000 / 0x40000 /
 * 0x400000 are the names in dCc_c.h. 0x20000 is not in that table; this
 * function ORs it with the egg and explosion bits. 0x26fe0 is the attack
 * mask below the bounce check: spin, punch, kick, 0x100, slide, dive,
 * 0x800, egg, explosion and 0x20000. */
enum {
    kHitMega = 0x10,
    kHitEggOrBlast = 0x26000,
    kHitFire = 0x40000,
    kHitAttack = 0x26fe0,
    kHitPlayer = 0x400000,
    kFlagYoshiMouth = 0x40000,
    kPlayerActorId = 0xbf,
    kStateChase = 0,
    kStateRecoil = 1,
    kRecoilFrames = 0x14
};

// @symbol _ZN8daHuwa_cD1Ev
// @symbol _ZN8daHuwa_cD0Ev
daHuwa_c::~daHuwa_c()
{
}

/* Vtable slot 29. 0x3c000 is 60.0; KillByInvincibleChar receives it and
 * does not read it. */
// @symbol _ZN8daHuwa_c16OnAimedAtWithEggEv
s32 daHuwa_c::OnAimedAtWithEgg()
{
    return 0x3c000;
}

/* Poof, three coins, bone-1 particle, sound 0xd5, then the death table.
 * func_0201267c is Sound::Play(bank 3, id, pos). */
// @symbol func_ov081_021237ec
extern "C" void func_ov081_021237ec(daHuwa_c *self)
{
    Vector3 dust;
    Vector3 dustCopy;
    Vector3 coins;

    dust.x = self->mPosX;
    dust.y = self->mPosY;
    dust.z = self->mPosZ;
    dust.y += self->mdCcAc_c.height - 0x50000;
    dustCopy = dust;
    self->PoofDustAt(dustCopy);

    coins.x = self->mPosX;
    coins.y = self->mPosY;
    coins.z = self->mPosZ;
    _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(self, &coins, 3, 0xa000, 0);

    Matrix4x3_FromTranslation(&data_020a0e68, self->mPosX, self->mPosY, self->mPosZ);
    MulMat4x3Mat4x3((const int *)(self->mModelAnim.data.transforms + 1), data_020a0e68.m, data_020a0e68.m);

    dust.x = data_020a0e68.m[9];
    dust.y = data_020a0e68.m[10];
    dust.z = data_020a0e68.m[11];
    SubVec3(&dust, (Vector3 *)&self->mPosX, &dust);
    Vec3_LslInPlace(&dust, 3);
    AddVec3(&dust, (Vector3 *)&self->mPosX, &dust);

    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x53, dust.x, dust.y, dust.z);
    func_0201267c(0xd5, (const Vector3 *)&self->mCamSpacePosX);
    self->KillAndTrackInDeathTable();
}

/* Cylinder hits. Slot 29 is OnAimedAtWithEgg; called through dActor_c so it
 * stays a virtual call. func_ov002_020e10a8 is Player::IsState of the state
 * SpinBounce enters. */
// @symbol func_ov081_02123910
extern "C" void func_ov081_02123910(daHuwa_c *self)
{
    dActor_c *other;
    unsigned int id;
    int flags;
    Player *player;
    Vector3_16 knock;
    Vector3 pos;

    id = self->mdCcAc_c.otherOwner;
    if (id == 0)
        return;
    other = dActor_c::FindWithID(id);
    if (other == 0)
        return;

    flags = self->mdCcAc_c.hitFlags;
    if ((flags & kHitMega) != 0) {
        int height;
        knock.x = (short)-0x2000;
        knock.y = 0;
        knock.z = 0;
        height = ((dActor_c *)self)->OnAimedAtWithEgg();
        _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(self, &knock, other, height);
        return;
    }

    if ((flags & kHitEggOrBlast) != 0) {
        func_ov081_021237ec(self);
        return;
    }

    int isPlayer = (int)(other->actorID == kPlayerActorId);
    if (isPlayer == 0)
        return;

    player = (Player *)other;
    if (player->mIsMetal == 0) {
        if (player->IsOnShell() == 0) {
            flags = self->mdCcAc_c.hitFlags;
            if ((flags & kHitFire) == 0)
                goto cont;
        }
    }
    func_ov081_021237ec(self);
    return;

cont:
    if ((flags & kHitAttack) != 0) {
        if (func_ov002_020e10a8(player) == 0) {
            func_ov081_021237ec(self);
            return;
        }
    }

    if (self->JumpedOnByPlayer(self->mdCcAc_c, *player) != 0) {
        _ZN6Player10SpinBounceE5Fix12IiE(player, 0x28000);
        func_ov081_021237ec(self);
        return;
    }

    if (player->mIsVanish != 0)
        return;
    if ((self->mdCcAc_c.hitFlags & kHitPlayer) == 0)
        return;

    {
        unsigned char recoil = 1;
        int speed;
        self->mState = recoil;
        speed = 0xa000;
        self->mStateTimer = 0;
        self->mHorzSpeed = -speed;
    }
    self->mPrevAngleY = Vec3_HorzAngle((const Vector3 *)&self->mPosX, (const Vector3 *)&player->mPosX);

    pos.x = self->mPosX;
    pos.y = self->mPosY;
    pos.z = self->mPosZ;
    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(player, &pos, 2, 0xc000, 1, 0, 1);
}

/* Yaw the model, plant its translation at pos>>3, then the drop shadow.
 * In Yoshi's mouth (mFlags 0x40000) neither the shadow nor Render runs. */
// @symbol func_ov081_02123b20
extern "C" void func_ov081_02123b20(daHuwa_c *self)
{
    Matrix4x3_FromRotationY(&self->mModelAnim.mat4x3, self->mAngleY);
    self->mModelAnim.mat4x3.m[9] = self->mPosX >> 3;
    self->mModelAnim.mat4x3.m[10] = self->mPosY >> 3;
    self->mModelAnim.mat4x3.m[11] = self->mPosZ >> 3;
    {
        int hidden = (int)((self->mFlags & kFlagYoshiMouth) != 0);
        if (hidden != 0)
            return;
    }
    if (self->mWithMeshClsn.IsOnGround() != 0) {
        _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
            self, self->mShadowModel, self->mModelAnim.mat4x3, 0x50000, 0x1e000, 0xf);
    } else {
        _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
            self, self->mShadowModel, self->mModelAnim.mat4x3, 0x50000, 0x96000, 0xf);
    }
}

// @symbol _ZN8daHuwa_c16CleanupResourcesEv
int daHuwa_c::CleanupResources()
{
    data_ov081_02128d60.Release();
    data_ov081_02128d68.Release();
    return 1;
}

// @symbol _ZN8daHuwa_c6RenderEv
int daHuwa_c::Render()
{
    int hidden = (mFlags & kFlagYoshiMouth) ? 1 : 0;
    if (hidden)
        return 1;
    mModelAnim.Render(0);
    return 1;
}

// @symbol _ZN8daHuwa_c8BehaviorEv
int daHuwa_c::Behavior()
{
    int killed = UpdateKillByInvincibleChar(mWithMeshClsn, mModelAnim, 3);
    if (killed != 0) {
        if (killed == 2)
            func_ov081_021237ec(this);
        return 1;
    }

    if (UpdateYoshiEat(mWithMeshClsn) != 0) {
        if (SpawnParticlesIfHitOtherObj(mdCcAc_c) != 0)
            func_ov081_021237ec(this);
        func_ov081_02123b20(this);
        mdCcAc_c.Clear();
        if (mEatenByYoshi != 0 && unk_104 == 0)
            mdCcAc_c.Update();
        return 1;
    }

    MakeVanishLuigiWork(mdCcAc_c);
    func_ov081_02123910(this);

    switch (mState) {
    case kStateChase: {
        Player *player;
        ApproachLinear(mHorzSpeed, 0x4000, 0x1000);
        player = (Player *)ClosestPlayer();
        if (player != 0) {
            int *src = (int *)((int)player + 0x5c);
            int playerPos[3];
            playerPos[0] = src[0];
            playerPos[1] = src[1];
            playerPos[2] = src[2];
            if (Vec3_HorzDist((const Vector3 *)&mHomePosX, (const Vector3 *)playerPos) > 0x3e8000)
                mTargetAngY = Vec3_HorzAngle((const Vector3 *)&mPosX, (const Vector3 *)&mHomePosX);
            else if (Vec3_HorzDist((const Vector3 *)&mPosX, (const Vector3 *)playerPos) > 0x12c000)
                mTargetAngY = Vec3_HorzAngle((const Vector3 *)&mPosX, (const Vector3 *)playerPos);
        } else {
            mTargetAngY = Vec3_HorzAngle((const Vector3 *)&mPosX, (const Vector3 *)&mHomePosX);
        }
        ApproachLinear(mAngleY, mTargetAngY, 0x200);
        mPrevAngleY = mAngleY;
        break;
    }
    case kStateRecoil:
        *(unsigned short *)&mStateTimer += 1;
        if (*(unsigned short *)&mStateTimer >= kRecoilFrames)
            mState = kStateChase;
        break;
    }

    mModelAnim.Advance();
    UpdatePos(0);
    UpdateWMClsn(mWithMeshClsn, 0);

    if (IsGoingOffCliff(mWithMeshClsn, 0x3c000, 0x2888, 1, 1, 0x32000) != 0) {
        mPosX = mPrevPosX;
        mPosY = mPrevPosY;
        mPosZ = mPrevPosZ;
    }
    func_ov081_02123b20(this);
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}

// @symbol _ZN8daHuwa_c13InitResourcesEv
int daHuwa_c::InitResources()
{
    void *modelFile = Model::LoadFile(data_ov081_02128d60);
    mModelAnim.ModelBase::SetFile((BMD_File *)modelFile, 1, -1);
    Animation::LoadFile(data_ov081_02128d68);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &mModelAnim, ((HuwaLoadedFile *)&data_ov081_02128d68)->file, 0, 0x1000, 0);
    if (mShadowModel.InitCylinder() == 0)
        return 0;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0x3c000, 0x78000, 0x200000, 0xa6efe0);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x3c000, 0x3c000, 0, 0);
    mWithMeshClsn.StartDetectingWater();
    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    mState = 0;
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    return 1;
}

// @symbol _ZN8daHuwa_c13OnTurnIntoEggER6Player
void daHuwa_c::OnTurnIntoEgg(Player &player)
{
    if (!player.IsCollectingCap())
        player.RegisterEggCoinCount(3, 0, 0);
    else
        GivePlayerCoins(player, 3, 0);
    KillAndTrackInDeathTable();
}

// @symbol _ZN8daHuwa_c13OnYoshiTryEatEv
s32 daHuwa_c::OnYoshiTryEat()
{
    return 6;
}
