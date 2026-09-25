//cpp
/* daBasabasa_c (BASABASA, the swoop), ov065 0x02116f98..0x02117ee8.
 *
 * RTTI names daBasabasa_c. The vtable address point is 0x0211cca0; the word
 * at -4 is _ZTI12daBasabasa_c (0x0211cc60), whose string at 0x0211cc6c is
 * "12daBasabasa_c" and whose single base is dEnemyBase_c. The factory
 * daBasabasa_c_classInit at 0x02117ee8 follows this run and is not in it.
 *
 * The out-of-line destructor is the key function, so this TU emits the
 * vtable and RTTI. Under `#pragma defer_codegen off` it comes out D1, D0,
 * then a D2 the cartridge has no home for (manifest: deadstrip), and .text
 * is laid down in source order, so the file is ROM-ascending.
 *
 * Known limits:
 * - ModelAnim::SetAnim, dCcAc_c::Init, dBgCh_Actr::Init,
 *   dEnemyBase_c::KillByInvincibleChar, dActor_c::SpawnCoins,
 *   dActor_c::DropShadowRadHeight, Player::Hurt and Player::Bounce are
 *   called by their mangled names. Each symbol carries a Fix12<int> by value.
 * - func_ov065_0211704c and func_ov065_02117994 keep C linkage under their
 *   address names, because include/decl_common.h declares them that way.
 * - The shared files and the four state records keep their address names:
 *   the static initializer that owns that .bss is another source file.
 */

#pragma defer_codegen off
#include "common.h"
#include "daBasabasa_c.h"
#include "SharedFilePtr.h"
#include "Player.h"

/* EnterDrop's unused stack temporary. The three-int constructor is what
 * schedules the stores; a plain Vector3 assignment does not. */
struct PoofPos {
    int x, y, z;
    PoofPos(int a, int b, int c) : x(a), y(b), z(c) {}
    ~PoofPos() {}
};

extern "C" {
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(ModelAnim *m, void *f, int a, int fix, unsigned sp0);
extern void func_ov002_020aea30(void *self, void *actor, void *collision);
extern void func_02012694(int a, void* b);
extern void _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(dEnemyBase_c* self, short* v, Player* p);
extern void _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(dActor_c* self, Vector3* pos, unsigned n, int fix, short s);
extern void _ZN6Player6BounceE5Fix12IiE(Player* p, int fix);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(Player* p, Vector3* pos, unsigned a, int fix, unsigned b, unsigned d, unsigned e);
int Vec3_Dist(const void* a, const void* b);
short Vec3_HorzAngle(const Vector3* a, const Vector3* b);
void Matrix4x3_FromRotationY(void* m, int angle);
void ApproachAngle(short* a, int b, int c, int d, int e);
void MulVec3Mat4x3(const void* in, const void* m, void* out);
extern Matrix4x3 data_020a0e68;
extern unsigned int RandomIntInternal(int* seed);
extern int data_0209e650;
extern void Matrix4x3_ApplyInPlaceToRotationX(void* m, short ang);
extern void Vec3_Asr(Vector3* d, Vector3* s, int sh);
extern void Matrix4x3_FromTranslation(Matrix4x3* m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void* m, int x, int y, int z);
extern void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void* self, void* sm, Matrix4x3* m, int fx, int t, unsigned int u);
extern unsigned short DecIfAbove0_Short(unsigned short *p);
extern SharedFilePtr data_ov065_0211d698;   /* flying model */
extern SharedFilePtr data_ov065_0211d6a8;   /* hanging model */
extern SharedFilePtr data_ov065_0211d690;   /* flying animation */
extern SharedFilePtr data_ov065_0211d6a0;   /* hanging animation */
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void* self, dActor_c* a, int r, int h, unsigned int e, unsigned int g);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void* self, dActor_c* a, int r, int h, Vector3_16* p, Vector3_16* q);
}

void ApproachLinear(short &value, short target, short step);

/* The four states, by the address of their .bss record. */
extern daBasabasa_c::State data_ov065_0211d6e0;   /* chase */
extern daBasabasa_c::State data_ov065_0211d6f0;   /* return */
extern daBasabasa_c::State data_ov065_0211d700;   /* perch */
extern daBasabasa_c::State data_ov065_0211d710;   /* drop */

/* The loaded file behind a SharedFilePtr is its second word. */
#define LOADED_FILE(ptr) ((void *)((int *)&(ptr))[1])

// @symbol _ZN12daBasabasa_cD1Ev
// @symbol _ZN12daBasabasa_cD0Ev
/* The compiler writes the whole body: the vtable store, the ShadowModel, both
 * ModelAnims, the dBgCh_Actr and the dCcAc_c in reverse declaration order,
 * then dEnemyBase_c::~dEnemyBase_c. */
daBasabasa_c::~daBasabasa_c()
{
}

// @symbol func_ov065_0211704c
/* Reacts to whatever the collision cylinder touched this frame. */
extern "C" void func_ov065_0211704c(char *c)
{
    daBasabasa_c *self = (daBasabasa_c *)c;
    Player *p;
    int flags;
    int isPlayer;

    if (self->mdCcAc_c.otherOwner == 0) return;
    p = (Player *)dActor_c::FindWithID(self->mdCcAc_c.otherOwner);
    if (p == 0) return;
    flags = self->mdCcAc_c.hitFlags;
    if (flags & 0x2400) {
        self->mIsFlying = 1;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim1, LOADED_FILE(data_ov065_0211d690), 0, 0x1000, 0);
        self->mDeathState = 2;
        func_ov002_020aea30(self, p, 0);
        func_02012694(0x112, &self->mCamSpacePosX);
        return;
    }
    if (flags & 0x10) {
        short v[3];
        v[0] = -0x2000;
        v[1] = 0;
        v[2] = 0;
        _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(self, v, p);
        func_02012694(0x112, &self->mCamSpacePosX);
        return;
    }
    if (flags & 0x40000) {
        self->mDeathState = 4;
        self->mIsFlying = 1;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim1, LOADED_FILE(data_ov065_0211d690), 0, 0x1000, 0);
        func_ov002_020aea30(self, p, 0);
        func_02012694(0x112, &self->mCamSpacePosX);
        return;
    }
    if (flags & 0x4380) {
        self->mIsFlying = 1;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim1, LOADED_FILE(data_ov065_0211d690), 0, 0x1000, 0);
        self->mDeathState = 3;
        func_ov002_020aea30(self, p, 0);
        func_02012694(0x112, &self->mCamSpacePosX);
        return;
    }
    if (flags & 0x40) {
        self->mIsFlying = 1;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim1, LOADED_FILE(data_ov065_0211d690), 0, 0x1000, 0);
        self->mDeathState = 2;
        func_ov002_020aea30(self, p, 0);
        func_02012694(0x112, &self->mCamSpacePosX);
        return;
    }
    if (flags & 0x20) {
        self->mDeathState = 1;
        func_ov002_020aea30(self, p, 0);
        func_02012694(0x112, &self->mCamSpacePosX);
        return;
    }

    /* Through an int: testing actorID in the `if` compiles differently. */
    isPlayer = (int)(p->actorID == 0xbf);
    if (isPlayer == 0)
        return;

    if (self->BumpedUnderneathByPlayer(*p) == 1 || p->IsOnShell() == 1 || p->mIsMetal == 1) {
        Vector3 pos;
        self->SmallPoofDust();
        self->KillAndTrackInDeathTable();
        func_02012694(0x112, &self->mCamSpacePosX);
        pos.x = self->mPosX;
        pos.y = self->mPosY;
        pos.z = self->mPosZ;
        _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(
            self, &pos, self->unk_10a + 1, 0xa000, 0);
        return;
    }

    if (self->JumpedOnByPlayer(self->mdCcAc_c, *p) != 0) {
        _ZN6Player6BounceE5Fix12IiE(p, 0x28000);
        self->mDeathState = 1;
        func_ov002_020aea30(self, p, 0);
        func_02012694(0x112, &self->mCamSpacePosX);
        return;
    }
    Vector3 pos;
    pos.x = self->mPosX;
    pos.y = self->mPosY;
    pos.z = self->mPosZ;
    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(p, &pos, 1, 0xc000, 1, 0, 1);
    if (self->mCurrentState != &data_ov065_0211d6e0)
        return;
    self->mPerchTimer = 0;
    self->SetState(&data_ov065_0211d6f0);
}

// @symbol _ZN12daBasabasa_c13ExecuteReturnEv
int daBasabasa_c::ExecuteReturn()
{
    Vector3 v;
    v.x = 0;
    v.y = 0;
    v.z = 0;
    int dist = Vec3_Dist(&mPosX, &mHomePosX);
    if (dist > 0x1f4000 || mWithMeshClsn.IsOnWall() != 0) {
        mTargetAngleY = Vec3_HorzAngle((Vector3 *)&mPosX, (Vector3 *)&mHomePosX);
        /* The states read mStateTimer unsigned (ldrh). */
        if ((u16)mStateTimer < 0x14) mStateTimer = 0x14;
    }
    if (dist < 0x1f4000 && (mFlags & 8)) {
        mPerchTimer++;
        if (mPerchTimer > 0x64) {
            mPerchTimer = 0;
            mFlags |= 1;
            mPrevAngleX = -0x8000;
            mAngleX = mPrevAngleX;
            mPosX = mHomePosX;
            mPosY = mHomePosY;
            mPosZ = mHomePosZ;
            unk_0a4 = 0;
            mVertSpeed = 0;
            unk_0ac = 0;
            SetState(&data_ov065_0211d700);
            return 1;
        }
    }

    ApproachAngle(&mPrevAngleX, 0, 0xa, 0x200, 0x100);
    ApproachAngle(&mPrevAngleY, mTargetAngleY, 0xa, 0x200, 0x100);
    v.z = 0xa000;
    Matrix4x3_FromRotationY(&data_020a0e68, mAngleY);
    MulVec3Mat4x3(&v, &data_020a0e68, &unk_0a4);
    if ((u16)mStateTimer == 0) {
        SetState(&data_ov065_0211d6f0);
    }
    return 1;
}

// @symbol _ZN12daBasabasa_c11EnterReturnEv
int daBasabasa_c::EnterReturn()
{
    unsigned int r;
    r = RandomIntInternal(&data_0209e650);
    mTargetAngleY = ((r >> 8) & 0xf) << 0xc;
    r = RandomIntInternal(&data_0209e650);
    mStateTimer = ((r >> 8) & 0x1f) + 0x32;
    func_02012694(0xf9, &mCamSpacePosX);
    mModelAnim1.speed = 0x1000;
    mFlags &= ~1;
    return 1;
}

// @symbol _ZN12daBasabasa_c12ExecuteChaseEv
int daBasabasa_c::ExecuteChase()
{
    Vector3 v;
    v.x = 0;
    v.y = 0;
    v.z = 0;
    mTargetAngleY = HorzAngleToCPlayer();
    ApproachLinear(mPrevAngleY, mTargetAngleY, 0x500);
    ApproachLinear(mPrevAngleX, 0x100, 0x500);
    v.z = 0xa000;
    Matrix4x3_FromRotationY(&data_020a0e68, mAngleY);
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, mPrevAngleX);
    MulVec3Mat4x3(&v, &data_020a0e68, &unk_0a4);
    if (Vec3_Dist(&mPosX, &mHomePosX) > 0x5dc000 || (u16)mStateTimer == 0) {
        mPerchTimer = 0;
        SetState(&data_ov065_0211d6f0);
    }
    return 1;
}

// @symbol _ZN12daBasabasa_c10EnterChaseEv
int daBasabasa_c::EnterChase()
{
    mVertAccel = 0;
    mIsFlying = 1;
    mFlags = 0x10000003;
    mStateTimer = 0x64;
    mModelAnim1.speed = 0x2000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim1, LOADED_FILE(data_ov065_0211d690), 0, 0x1000, 0);
    mTargetAngleY = ((RandomIntInternal(&data_0209e650) >> 8) & 0xf) << 0xc;
    return 1;
}

// @symbol _ZN12daBasabasa_c11ExecuteDropEv
int daBasabasa_c::ExecuteDrop()
{
    int floorY, posY;
    Player *p;
    mPrevAngleY += 0x1000;
    p = ClosestPlayer();
    posY = mPosY;
    floorY = posY - 0xc8000;
    if (p != 0) {
        floorY = p->mGroundY + 0x50000;
    }
    if ((u16)mStateTimer == 0 || floorY > posY) {
        SetState(&data_ov065_0211d6e0);
    }
    return 1;
}

// @symbol _ZN12daBasabasa_c9EnterDropEv
int daBasabasa_c::EnterDrop()
{
    mVertAccel = -0x1000;
    mTerminalVelocity = -0xa000;
    mIsFlying = 0;
    mModelAnim2.speed = 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim2, LOADED_FILE(data_ov065_0211d6a0), 0, 0x1000, 0);
    PoofPos vec(mPosX, mPosY + 0x64000, mPosZ);
    mStateTimer = 0x19;
    if (mPosY == 0x898000)
        mStateTimer = 0x32;
    func_02012694(0xf9, &mCamSpacePosX);
    return 1;
}

// @symbol _ZN12daBasabasa_c12ExecutePerchEv
int daBasabasa_c::ExecutePerch()
{
    Player *p = ClosestNonVanishPlayer();
    if (p) {
        Vector3 v;
        /* One base pointer: reading p->mPosX and so on directly compiles differently. */
        int *q = &p->mPosX;
        v.x = q[0];
        v.y = q[1];
        v.z = q[2];
        if (Vec3_Dist(&mPosX, &v) < 0x3e8000) {
            mStateTimer = 0x14;
            SetState(&data_ov065_0211d710);
        }
    }
    return 1;
}

// @symbol _ZN12daBasabasa_c10EnterPerchEv
int daBasabasa_c::EnterPerch()
{
    mFlags = 3;
    mIsFlying = 0;
    mModelAnim2.speed = 0x1000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim2, LOADED_FILE(data_ov065_0211d6a0), 0, 0x1000, 0);
    return 1;
}

// @symbol _ZN12daBasabasa_c8SetStateEPNS_5StateE
int daBasabasa_c::SetState(State *state)
{
    mCurrentState = state;
    if (mCurrentState->enter == 0) return 1;
    return (this->*mCurrentState->enter)();
}

// @symbol func_ov065_02117994
/* Places both models and the drop shadow at the current position. */
extern "C" void func_ov065_02117994(char *c)
{
    daBasabasa_c *self = (daBasabasa_c *)c;
    Vector3 v;
    Vec3_Asr(&v, (Vector3 *)&self->mPosX, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68,
        self->mAngleX, self->mAngleY, self->mAngleZ);
    if (self->mIsFlying == 1) {
        self->mModelAnim1.mat4x3 = data_020a0e68;
    } else {
        self->mModelAnim2.mat4x3 = data_020a0e68;
    }
    Matrix4x3_FromTranslation(&data_020a0e68,
        self->mPosX >> 3,
        (self->mPosY - 0x18000) >> 3,
        self->mPosZ >> 3);
    self->mShadowMatrix = data_020a0e68;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        self, &self->mShadowModel, &self->mShadowMatrix, 0x32000, 0x258000, 0xf);
}

// @symbol _ZN12daBasabasa_c16CleanupResourcesEv
/* Releases the four files InitResources claimed. */
int daBasabasa_c::CleanupResources()
{
    data_ov065_0211d698.Release();
    data_ov065_0211d6a8.Release();
    data_ov065_0211d690.Release();
    data_ov065_0211d6a0.Release();
    return 1;
}

// @symbol _ZN12daBasabasa_c16OnPendingDestroyEv
void daBasabasa_c::OnPendingDestroy()
{
}

// @symbol _ZN12daBasabasa_c6RenderEv
int daBasabasa_c::Render()
{
    /* Through an int: testing mFlags in the `if` compiles differently. */
    int flag = (mFlags & 0x40000) != 0;
    if (flag != 0) return 1;
    if (mIsFlying == 1) {
        mModelAnim1.Render(0);
    } else {
        mModelAnim2.Render(0);
    }
    return 1;
}

// @symbol _ZN12daBasabasa_c8BehaviorEv
int daBasabasa_c::Behavior()
{
    if (UpdateYoshiEat(mWithMeshClsn) != 0) {
        mdCcAc_c.Clear();
        if (mEatenByYoshi != 0) {
            if (unk_104 == 0) {
                mdCcAc_c.Update();
            }
        }
        func_ov065_02117994((char *)this);
        return 1;
    }
    if (UpdateKillByInvincibleChar(mWithMeshClsn, mModelAnim1, 3) != 0) {
        return 1;
    }
    if (mDeathState != 0) {
        UpdateDeath(mWithMeshClsn);
        func_ov065_02117994((char *)this);
        return 1;
    }
    DecIfAbove0_Short((unsigned short *)&mStateTimer);
    if (mCurrentState->execute != 0)
        (this->*mCurrentState->execute)();
    if (mCurrentState == &data_ov065_0211d6e0 || mCurrentState == &data_ov065_0211d6f0) {
        /* Three wingbeat frames, one flap sound. */
        if (mModelAnim1.WillHitFrame(3) != 0 ||
            mModelAnim1.WillHitFrame(0xf) != 0 ||
            mModelAnim1.WillHitFrame(0x1b) != 0) {
            func_02012694(0xe1, &mCamSpacePosX);
        }
    }
    {
        /* Gravity, clamped at terminal velocity. unk_0ac is reloaded and
           stored back unchanged, as in the ROM. */
        int fallSpeed = mVertSpeed + mVertAccel;
        int clamped = mTerminalVelocity;
        if (fallSpeed >= clamped)
            clamped = fallSpeed;
        int keep = unk_0ac;
        mVertSpeed = clamped;
        unk_0ac = keep;
    }
    UpdatePosWithOnlySpeed(&mdCcAc_c);
    UpdateWMClsn(mWithMeshClsn, 0);
    mAngleX = mPrevAngleX;
    mAngleY = mPrevAngleY;
    mAngleZ = mPrevAngleZ;
    func_ov065_02117994((char *)this);
    if (mIsFlying == 1) {
        func_ov065_0211704c((char *)this);
    }
    mdCcAc_c.Clear();
    {
        Player *p = ClosestPlayer();
        if (p != 0 && p->mIsVanish == 0) {
            mdCcAc_c.Update();
        }
    }
    if (mIsFlying == 1) {
        mModelAnim1.Advance();
    } else {
        mModelAnim2.Advance();
    }
    return 1;
}

// @symbol _ZN12daBasabasa_c13InitResourcesEv
/* Both ModelAnims are loaded here, the other half of the evidence that 0x300
 * and 0x364 are two members. */
int daBasabasa_c::InitResources()
{
    mModelAnim1.SetFile((BMD_File *)Model::LoadFile(data_ov065_0211d698), 1, -1);
    mModelAnim2.SetFile((BMD_File *)Model::LoadFile(data_ov065_0211d6a8), 1, -1);
    mShadowModel.InitCylinder();
    Animation::LoadFile(data_ov065_0211d690);
    Animation::LoadFile(data_ov065_0211d6a0);
    mTerminalVelocity = -0xa000;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0x28000, 0x28000, 0x200000, 0x7eff0);
    mAngleY = mPrevAngleY;
    mPrevAngleX = -0x8000;
    mAngleX = mPrevAngleX;
    mIsFlying = 0;
    mModelAnim1.speed = 0x1000;
    mModelAnim2.speed = 0x1000;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x28000, 0x1e000, 0, 0);
    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    unk_108 = 1;
    SetState(&data_ov065_0211d700);
    return 1;
}

// @symbol _ZN12daBasabasa_c16OnAimedAtWithEggEv
s32 daBasabasa_c::OnAimedAtWithEgg()
{
    return 0;
}

// @symbol _ZN12daBasabasa_c13OnTurnIntoEggER6Player
void daBasabasa_c::OnTurnIntoEgg(Player &player)
{
    GivePlayerCoins(player, (unsigned char)(unk_10a + 1), 0);
    KillAndTrackInDeathTable();
}

// @symbol _ZN12daBasabasa_c13OnYoshiTryEatEv
s32 daBasabasa_c::OnYoshiTryEat()
{
    return 4;
}
