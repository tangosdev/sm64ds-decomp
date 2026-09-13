//cpp
/**
 * Jolly Roger Bay's Bubba (`bakubaku`).
 *
 * daBakubaku_c_classInit is reconstructed (RTTI daBakubaku_c,
 * BAKUBAKU registry). Retail does not store that spelling.
 *
 * deslop
 * Leftover: func_ov032_02111254..02112044 are this TU's own
 *   state/helper bodies; the ROM symbols are still the func_ov
 *   labels. Naming them as C++ methods would emit _ZN12daBakubaku_c*
 *   and miss those labels.
 * Leftover: Klass stays incomplete (completing it as daBakubaku_c
 *   ICEs mwccarm's PMF). Tables are { PMF enter; PMF main }, 0x10
 *   apart (a7c/a8c/a9c/aac/abc). mState is a pointer to one entry.
 * Leftover: ModelAnim::SetAnim, dCcAcPos_c::Init, DropShadowRadHeight
 *   and Player::Hurt stay mangled (Fix12-by-value, wall 6az).
 *   dBgCh_Actr::Init stays mangled: header Fix12i mangles as int;
 *   ROM is Fix12<int> (method form links Undefined).
 * Leftover: common.h must be first. Nested Matrix.h spelling
 *   scalarizes the 12-word mat4x3 / mShadowMat copies.
 * Leftover: decl_common.h is here because 02113a48 / 02113a50 /
 *   02113a8c are extern int there, so BMD/BCA handles are int[]
 *   punned to SharedFilePtr and 02113a8c takes an &.
 * Leftover: BMD/BCA SharedFilePtrs still data_ov032_*; sinit file IDs
 *   661 / 662 / 663. State-table symbols still data_ov032_*.
 * Leftover: data_0209f32c is water height; particle/sound helpers
 *   func_02022c80 / 02022d00 / 02012694 / func_ov002_020c5cd8.
 * Leftover: gotos, dead-store in[2] pairs, and register-named
 *   locals (r5, v1/v2) are load-bearing S8/S15.
 */

#include "common.h"
#include "daBakubaku_c.h"
#include "types.h"
#include "decl_common.h"
#include "dCc_c.h"
#include "Player.h"
#include "SharedFilePtr.h"

struct Klass;
typedef void (Klass::*PMF)();
/* Tables are 0x10 apart. SetState calls enter at +0; Behavior calls main at +8. */
struct StateEntry { PMF enter; PMF main; };

struct BakubakuSpawnInfo {
    daBakubaku_c *(*classInit)();
    s16 executePriority; /* +4: also BAKUBAKU registry id 0x00e4 = 228 */
    s16 renderPriority;  /* +6 */
    u32 actorFlags;
    Fix12i clipOffsetY;
    Fix12i clipRadius;
    Fix12i clipDistance;
    Fix12i farDistance;
};
typedef char BakubakuSpawnInfo_size_must_be_0x1c[
    sizeof(BakubakuSpawnInfo) == 0x1c ? 1 : -1];

extern "C" {
extern int data_ov032_02113a40[];
extern int data_ov032_02113abc[];
extern int data_ov032_02113a7c[];
extern int data_0209f32c[];
extern int data_ov032_02113aac[];
extern int data_ov032_02113a9c[];
extern int data_020a0e68[];
extern int data_0209e650[];

extern Vector3 data_ov032_021137cc;
extern Vector3 data_ov032_021137d8;

int func_ov032_02111254(daBakubaku_c *self);
int func_ov032_02111350(daBakubaku_c *self);
void func_ov032_021113fc(daBakubaku_c *self);
int func_ov032_02111ff4(void *self, void *state);
void func_ov032_02112044(daBakubaku_c *self);

int Vec3_HorzDist(const Vector3 *a, const Vector3 *b);
int AngleDiff(int a, int b);
s16 Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
s16 Vec3_VertAngle(const Vector3 *a, const Vector3 *b);
void Vec3_Asr(void *d, void *s, int sh);
int RandomIntInternal(int *seed);
unsigned short DecIfAbove0_Short(unsigned short *p);

void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationXYZExt(void *m, int x, int y, int z);
void Matrix4x3_FromRotationY(void *m, s16 angY);
void Matrix4x3_ApplyInPlaceToRotationX(void *m, s16 angX);
void MulVec3Mat4x3(void *in, void *m, void *out);

void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
    void *self, void *bca, int a, int fix, unsigned int b);
void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
    dCcAcPos_c *self, dActor_c *actor, const Vector3 &offset,
    int radius, int height, u32 d, u32 e);
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
    dBgCh_Actr *self, dActor_c *actor, int radius, int height, void *a, void *b);
void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void *self, void *sm, void *mtx, int f, int g, unsigned int h);
void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(
    void *p, void *v, u32 a, int f, u32 c, u32 d, u32 e);
int func_ov002_020c5cd8(void *a, void *self);
void func_02012694(int a, void *p);
u32 func_02022c80(u32, u32, Fix12i, Fix12i, Fix12i, const void *);
u32 func_02022d00(u32, u32, Fix12i, Fix12i, Fix12i, void *);
int _Z14ApproachLinearRsss(s16 *dst, s16 target, s16 step);
void _Z14ApproachLinearRiii(int *p, int t, int s);
}

enum { kPlayerActorId = 0xbf };

// @symbol daBakubaku_c_classInit
extern "C" daBakubaku_c *daBakubaku_c_classInit()
{
    return new daBakubaku_c();
}

// @symbol g_profile_BAKUBAKU
extern "C" BakubakuSpawnInfo g_profile_BAKUBAKU = {
    daBakubaku_c_classInit,
    0x00e4,
    0x0052,
    3,
    0,
    0x003e8000,
    0x01000000,
    0x01000000
};

// @symbol _ZN12daBakubaku_c16OnAimedAtWithEggEv
int daBakubaku_c::OnAimedAtWithEgg()
{
    return 0xa0000; /* 10.0 Fix12, egg aim height */
}

// @symbol _ZN12daBakubaku_c13InitResourcesEv
s32 daBakubaku_c::InitResources()
{
    Vector3 bodyOffset;
    Vector3 headOffset;
    void *f;

    f = Model::LoadFile(*(SharedFilePtr *)data_ov032_02113a40);
    mModelAnim.SetFile((BMD_File *)f, 1, -1);
    mShadowModel.InitCylinder();
    Animation::LoadFile(*(SharedFilePtr *)&data_ov032_02113a50);
    Animation::LoadFile(*(SharedFilePtr *)&data_ov032_02113a48);

    bodyOffset.x = data_ov032_021137cc.x;
    bodyOffset.y = data_ov032_021137cc.y;
    bodyOffset.z = data_ov032_021137cc.z;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
        &mBodyClsn, this, bodyOffset, 0x64000, 0x64000, 0x200004, 0x10);

    headOffset.x = data_ov032_021137d8.x;
    headOffset.y = data_ov032_021137d8.y;
    headOffset.z = data_ov032_021137d8.z;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
        &mHeadClsn, this, headOffset, 0x64000, 0x8c000, 0x200004, 0);

    mSpawnPosX = mPosX;
    mSpawnPosY = mPosY;
    mSpawnPosZ = mPosZ;

    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
        &mWithMeshClsn, this, 0x64000, 0, 0, 0);

    mTerminalVelocity = -0x1e000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &mModelAnim, (void *)(&data_ov032_02113a50)[1], 0, 0x1000, 0);
    mModelAnim.speed = 0x1000;

    func_ov032_02111ff4(this, &data_ov032_02113a8c);
    return 1;
}

// @symbol _ZN12daBakubaku_c8BehaviorEv
s32 daBakubaku_c::Behavior()
{
    if (UpdateKillByInvincibleChar(mWithMeshClsn, mModelAnim, 3) != 0)
        return 1;

    DecIfAbove0_Short((unsigned short *)&mStateTimer);
    DecIfAbove0_Short(&mChaseCooldown);

    StateEntry *state = (StateEntry *)mState;
    if (state->main != 0)
        (((Klass *)this)->*(state->main))();

    mAngleX = mPrevAngleX;
    mAngleY = mPrevAngleY;
    mAngleZ = mPrevAngleZ;
    UpdatePos(&mBodyClsn);
    UpdateWMClsn(mWithMeshClsn, 0);
    func_ov032_02112044(this);

    if (mState != data_ov032_02113aac) {
        mModelAnim.speed = 0x1000;
    } else {
        mModelAnim.speed = 0x2000;
    }

    mModelAnim.Advance();
    func_ov032_021113fc(this);
    mBodyClsn.Clear();
    mHeadClsn.Clear();

    Player *p = ClosestPlayer();
    if (p != 0 && p->mIsVanish == 0) {
        mBodyClsn.Update();
        mHeadClsn.Update();
    }

    return 1;
}

// @symbol _ZN12daBakubaku_c6RenderEv
s32 daBakubaku_c::Render()
{
    /* Temporary is load-bearing: ROM materialises 0/1 then cmp. */
    int b = ((mFlags & 0x40000) != 0);
    if (b) return 1;
    mModelAnim.Render(0);
    return 1;
}

// @symbol _ZN12daBakubaku_c16OnPendingDestroyEv
void daBakubaku_c::OnPendingDestroy()
{
}

// @symbol _ZN12daBakubaku_c16CleanupResourcesEv
s32 daBakubaku_c::CleanupResources()
{
    ((SharedFilePtr *)data_ov032_02113a40)->Release();
    ((SharedFilePtr *)&data_ov032_02113a50)->Release();
    ((SharedFilePtr *)&data_ov032_02113a48)->Release();
    return 1;
}

/* Update model matrix, shadow matrix, drop shadow. */
// @symbol func_ov032_02112044
extern "C" void func_ov032_02112044(daBakubaku_c *self)
{
    Vector3 v;
    Vec3_Asr(&v, &self->mPosX, 3);
    Matrix4x3_FromTranslation(data_020a0e68, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(
        data_020a0e68, self->mAngleX, self->mAngleY, self->mAngleZ);
    self->mModelAnim.mat4x3 = *(Matrix4x3 *)data_020a0e68;
    Matrix4x3_FromTranslation(
        data_020a0e68, self->mPosX >> 3,
        (self->mPosY - 0x5a000) >> 3, self->mPosZ >> 3);
    *(Matrix4x3 *)self->mShadowMat = *(Matrix4x3 *)data_020a0e68;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        self, &self->mShadowModel, self->mShadowMat, 0xfa000, 0x258000, 0xf);
}

/* Install mState (PMF table entry) and run its enter function. */
struct SC;
typedef int (SC::*SPMF)();
struct SC { char pad[0x3b0]; SPMF *pp; };
// @symbol func_ov032_02111ff4
extern "C" int func_ov032_02111ff4(void *cv, void *pv)
{
    daBakubaku_c *c = (daBakubaku_c *)cv;
    SPMF *p = (SPMF *)pv;
    c->mState = p;
    SPMF *q = (SPMF *)c->mState;
    if (*q == 0) return 1;
    return (((SC *)c)->*(*q))();
}

/* Wander-enter: random yaw and state timer. */
// @symbol func_ov032_02111f9c
extern "C" int func_ov032_02111f9c(daBakubaku_c *self)
{
    unsigned int r = RandomIntInternal(data_0209e650);
    self->mAngTarget = ((r >> 8) & 0xf) << 0xc;
    r = RandomIntInternal(data_0209e650);
    self->mStateTimer = ((r >> 8) & 0x3f) + 0x32;
    self->mModelAnim.speed = 0x1000;
    return 1;
}

/* Wander: turn toward spawn, maybe chase. */
// @symbol func_ov032_02111e24
extern "C" int func_ov032_02111e24(daBakubaku_c *self)
{
    int in[3];
    int out[3];
    short ang;

    _Z14ApproachLinearRiii(&self->mHorzSpeed, 0x5000, 0x333);
    if (func_ov032_02111350(self) == 1) {
        self->mStateTimer = 0x28;
        self->mChaseCooldown = 0x28;
        ang = Vec3_HorzAngle(
            (const Vector3 *)&self->mPosX,
            (const Vector3 *)&self->mSpawnPosX);
        self->mAngTarget = ang;
    }
    _Z14ApproachLinearRsss(&self->mPrevAngleY, self->mAngTarget, 0x100);
    ang = Vec3_VertAngle(
        (const Vector3 *)&self->mPosX,
        (const Vector3 *)&self->mSpawnPosX);
    _Z14ApproachLinearRsss(&self->mPrevAngleX, ang, 0x100);
    in[2] = 0;
    in[2] = 0x5000;
    in[0] = 0;
    in[1] = 0;
    out[0] = 0;
    out[1] = 0;
    out[2] = 0;
    Matrix4x3_FromRotationY(data_020a0e68, self->mAngleY);
    Matrix4x3_ApplyInPlaceToRotationX(data_020a0e68, self->mAngleX);
    MulVec3Mat4x3(in, data_020a0e68, out);
    self->mVertSpeed = out[1];
    if (func_ov032_02111254(self) == 1) {
        func_ov032_02111ff4(self, data_ov032_02113aac);
        return 1;
    }
    /* Measured: signed mStateTimer load DIFFs 1 word (ldrh vs ldrsh). */
    if (*(unsigned short *)&self->mStateTimer == 0) {
        unsigned int r = (unsigned int)RandomIntInternal(data_0209e650);
        if (((r >> 8) & 3) == 0) {
            func_ov032_02111ff4(self, data_ov032_02113a9c);
        } else {
            func_ov032_02111ff4(self, &data_ov032_02113a8c);
        }
    }
    return 1;
}

/* Pause-enter: short timer, clear vertical motion. */
// @symbol func_ov032_02111dd8
extern "C" int func_ov032_02111dd8(daBakubaku_c *self)
{
    unsigned int r = RandomIntInternal(data_0209e650);
    self->mStateTimer = ((r >> 8) & 0x1f) + 0x14;
    self->mModelAnim.speed = 0x1000;
    self->unk_0a4 = 0;
    self->mVertSpeed = 0;
    self->unk_0ac = 0;
    return 1;
}

/* Pause: give up to chase, else back to wander. */
// @symbol func_ov032_02111d7c
extern "C" int func_ov032_02111d7c(daBakubaku_c *self)
{
    if (func_ov032_02111254(self) == 1) {
        func_ov032_02111ff4(self, data_ov032_02113aac);
        return 1;
    }
    /* Measured: signed mStateTimer load DIFFs 1 word (ldrh vs ldrsh). */
    if (*(unsigned short *)&self->mStateTimer == 0)
        func_ov032_02111ff4(self, &data_ov032_02113a8c);
    return 1;
}

/* Surface-enter. */
// @symbol func_ov032_02111d58
extern "C" int func_ov032_02111d58(daBakubaku_c *self)
{
    self->mStateTimer = 0x12c;
    self->mModelAnim.speed = 0x2000;
    self->mHorzSpeed = 0xa000;
    return 1;
}

/* Chase: close on stored player pos, then lunge up or down. */
// @symbol func_ov032_02111b9c
extern "C" int func_ov032_02111b9c(daBakubaku_c *self)
{
    /* Measured: signed mStateTimer load DIFFs 1 word (ldrh vs ldrsh). */
    if (*(unsigned short *)&self->mStateTimer != 0) {
        if (func_ov032_02111350(self) == 1) goto init;
        if (func_ov032_02111254(self) != 0) goto track;
    }
init:
    {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
            &self->mModelAnim,
            *(BCA_File **)((unsigned char *)&data_ov032_02113a50 + 4),
            0, 0x1000, 0);
        self->mChaseCooldown = 0x64;
        func_ov032_02111ff4(self, &data_ov032_02113a8c);
        return 1;
    }
track:
    {
        self->mAngTarget = Vec3_HorzAngle(
            (Vector3 *)&self->mPosX, (Vector3 *)&self->mTargetPosX);
        unsigned int r = (unsigned int)RandomIntInternal(data_0209e650);
        int s5 = (int)(((r >> 8) & 3) << 0x1c) >> 0x10;
        _Z14ApproachLinearRsss(&self->mPrevAngleY, self->mAngTarget, 0x200);
        if (Vec3_HorzDist(
                (Vector3 *)&self->mPosX,
                (Vector3 *)&self->mTargetPosX) < 0x258000) {
            int d = AngleDiff(self->mAngleY, self->mAngTarget);
            if (d < (int)(short)(s5 + 0x200)) {
                self->mAngTarget = Vec3_VertAngle(
                    (Vector3 *)&self->mPosX,
                    (Vector3 *)&self->mTargetPosX);
                self->mFlags = 0;
                self->mModelAnim.speed = 0x1000;
                self->mDiveStartY = self->mPosY;
                self->mStateTimer = 0;
                self->mHorzSpeed = 0x14000;
                self->unk_429 = 0;
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
                    &self->mModelAnim,
                    *(BCA_File **)((unsigned char *)&data_ov032_02113a48 + 4),
                    0x40000000, 0x1000, 0);
                if (self->mAngTarget > 0)
                    func_ov032_02111ff4(self, data_ov032_02113a7c);
                else
                    func_ov032_02111ff4(self, data_ov032_02113abc);
                return 1;
            }
        }
        return 1;
    }
}

/* Lunge-enter: pitch down, enable char-movement clsn bit. */
// @symbol func_ov032_02111b50
extern "C" int func_ov032_02111b50(daBakubaku_c *self)
{
    self->mAngTarget = -0x4000;
    self->mBodyClsn.flags |= 2;
    self->mHeadClsn.flags |= 2;
    self->mMouthOpen = 0;
    self->mHorzSpeed = 0xa000;
    self->mLungePhase = 0;
    return 1;
}

/* Lunge: splash particles, then return to wander. */
// @symbol func_ov032_02111830
extern "C" int func_ov032_02111830(daBakubaku_c *self)
{
    s16 speed;
    speed = 0x3000;
    if (data_0209f32c[0] - 0x64000 > self->mPosY)
        speed = 0;
    if (self->mLungePhase == 0) {
        if (data_0209f32c[0] > self->mPosY) {
            if (self->mAngTarget > 0)
                self->mLungePhase = 1;
        }
    }
    if (self->mLungePhase > 0 && self->mLungePhase < 5) {
        self->mLungePhase++;
        if (self->mLungePhase == 4) {
            typedef struct { int x, y, z; } V3;
            V3 v[3];
            v[0].x = self->mPosX;
            v[0].y = self->mPosY;
            v[0].z = self->mPosZ;
            v[1].z = 0;
            v[1].x = 0;
            v[1].y = 0;
            v[2].x = 0;
            v[2].y = 0;
            v[2].z = 0;
            v[1].z = 0xa0000;
            Matrix4x3_FromRotationY(data_020a0e68, self->mAngleY);
            MulVec3Mat4x3(&v[1], data_020a0e68, &v[2]);
            v[0].x += v[2].x;
            v[0].z += v[2].z;
            func_02022c80(0, 0x55, v[0].x, v[0].y, v[0].z, 0);
            self->mSplashParticle = func_02022d00(
                self->mSplashParticle, 0x56, v[0].x, data_0209f32c[0],
                v[0].z, 0);
            v[0].y += 0x4b000;
            func_02022c80(0, 0x54, v[0].x, v[0].y, v[0].z, 0);
        }
    }

    if (func_ov032_02111350(self) == 1)
        goto zeroblock;
    if (func_ov032_02111254(self) != 0)
        goto matrixblock;
zeroblock:
    speed = 0;
    self->mHorzSpeed = 0;
    self->unk_0a4 = 0;
    self->mVertSpeed = 0;
    self->unk_0ac = 0;
    goto afterblock;
matrixblock:
    {
        int in2[3];
        int out2[3];
        in2[2] = 0;
        in2[2] = 0x14000;
        in2[0] = 0;
        in2[1] = 0;
        out2[0] = 0;
        out2[1] = 0;
        out2[2] = 0;
        Matrix4x3_FromRotationY(data_020a0e68, self->mAngleY);
        Matrix4x3_ApplyInPlaceToRotationX(data_020a0e68, self->mAngleX);
        MulVec3Mat4x3(in2, data_020a0e68, out2);
        self->mVertSpeed = out2[1];
    }
afterblock: ;
    _Z14ApproachLinearRsss(&self->mAngTarget, speed, 0x200);
    _Z14ApproachLinearRsss(&self->mPrevAngleX, self->mAngTarget, 0x200);
    if (self->mModelAnim.Finished() != 0) {
        self->mMouthOpen = 0;
        if (speed == 0) {
            s16 a = self->mPrevAngleX;
            if (a < 0) a = -a;
            if (a < 0x100) {
                self->mChaseCooldown = 0x64;
                self->mAngTarget = self->mAngleY;
                self->mFlags = 3;
                self->mBodyClsn.flags &= ~2;
                self->mHeadClsn.flags &= ~2;
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
                    &self->mModelAnim, (void *)(&data_ov032_02113a50)[1],
                    0, 0x1000, 0);
                func_ov032_02111ff4(self, &data_ov032_02113a8c);
                return 1;
            }
        }
    } else {
        unsigned int t = (unsigned int)(self->mModelAnim.currFrame << 4) >> 0x10;
        if (t > 0x14 && t < 0x3c)
            self->mMouthOpen = 1;
        else
            self->mMouthOpen = 0;
    }
    return 1;
}

/* Lunge-reset enter. */
// @symbol func_ov032_02111814
extern "C" int func_ov032_02111814(daBakubaku_c *self)
{
    self->mLungePhase = 0;
    self->mMouthOpen = 0;
    self->mHorzSpeed = 0xa000;
    return 1;
}

/* Surface toward player, then wander. */
// @symbol func_ov032_02111620
extern "C" int func_ov032_02111620(daBakubaku_c *self)
{
    Player *r5 = self->ClosestPlayer();
    if (r5 == 0)
        return 1;

    if (func_ov032_02111350(self) == 1)
        goto zeroblock;
    if (func_ov032_02111254(self) != 0)
        goto matrixblock;
zeroblock:
    self->mHorzSpeed = 0;
    self->unk_0a4 = 0;
    self->mVertSpeed = 0;
    self->unk_0ac = 0;
    goto afterblock;
matrixblock:
    {
        int in[3];
        int out[3];
        in[2] = 0;
        in[2] = 0x14000;
        in[0] = 0;
        in[1] = 0;
        out[0] = 0;
        out[1] = 0;
        out[2] = 0;
        Matrix4x3_FromRotationY(data_020a0e68, self->mAngleY);
        Matrix4x3_ApplyInPlaceToRotationX(data_020a0e68, self->mAngleX);
        MulVec3Mat4x3(in, data_020a0e68, out);
        self->mVertSpeed = out[1];
    }
afterblock: ;

    if (self->mLungePhase == 0) {
        if (self->mModelAnim.Finished() == 0)
            goto player_path;
    }

    _Z14ApproachLinearRsss(&self->mPrevAngleX, 0, 0x200);
    self->mLungePhase = 1;
    self->mMouthOpen = 0;
    if (AngleDiff(self->mPrevAngleX, 0) < 0x200) {
        self->mPrevAngleX = 0;
        self->mChaseCooldown = 0x64;
        self->mAngTarget = self->mAngleY;
        self->mFlags = 3;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
            &self->mModelAnim, (void *)(&data_ov032_02113a50)[1],
            0, 0x1000, 0);
        func_ov032_02111ff4(self, &data_ov032_02113a8c);
    }
    goto end;

player_path:
    {
        int vec[3];
        unsigned int n;
        int *p = (int *)&r5->mPosX;
        vec[0] = p[0];
        vec[1] = p[1];
        vec[2] = p[2];
        if (data_0209f32c[0] > self->mPosY) {
            _Z14ApproachLinearRsss(&self->mPrevAngleX, 0, 0x200);
        } else {
            _Z14ApproachLinearRsss(
                &self->mPrevAngleX,
                Vec3_VertAngle((const Vector3 *)&self->mPosX, (const Vector3 *)vec),
                0x200);
        }
        n = (unsigned int)(self->mModelAnim.currFrame << 4) >> 0x10;
        if (n > 0x14 && n < 0x3c)
            self->mMouthOpen = 1;
        else
            self->mMouthOpen = 0;
    }
end:
    return 1;
}

/* Hit cylinders: mega-kill, bite, or Hurt Mario. */
// @symbol func_ov032_021113fc
extern "C" void func_ov032_021113fc(daBakubaku_c *self)
{
    Vector3 v1;
    v1.x = data_ov032_021137cc.x;
    v1.y = data_ov032_021137cc.y;
    v1.z = data_ov032_021137cc.z;
    self->mBodyClsn.SetPosRelativeToActor(v1);

    u32 id1 = self->mBodyClsn.otherOwner;
    if (id1 != 0) {
        Player *f = (Player *)dActor_c::FindWithID(id1);
        /* Temporary is load-bearing: ROM materialises 0/1 then cmp. */
        int isPlayer = (int)(f->actorID == kPlayerActorId);
        if (isPlayer) {
            if (f->mIsVanish != 0) return;
            if (self->mBodyClsn.hitFlags & 0x10) {
                self->SpawnMegaCharParticles(*f, 0);
                self->PoofDust();
                f->IncMegaKillCount();
                func_02012694(0x1e, &self->mCamSpacePosX);
                self->KillAndTrackInDeathTable();
                return;
            }
            if (self->mMouthOpen != 0) {
                if (func_ov002_020c5cd8(f, self) == 1) {
                    func_02012694(0xf6, &self->mCamSpacePosX);
                    return;
                }
            }
        }
    }

    Vector3 v2;
    v2.x = data_ov032_021137d8.x;
    v2.y = data_ov032_021137d8.y;
    v2.z = data_ov032_021137d8.z;
    self->mHeadClsn.SetPosRelativeToActor(v2);

    u32 id2 = self->mHeadClsn.otherOwner;
    if (id2 == 0) return;
    Player *f2 = (Player *)dActor_c::FindWithID(id2);
    int isPlayer2 = (int)(f2->actorID == kPlayerActorId);
    if (isPlayer2 == 0) return;

    if (self->mHeadClsn.hitFlags & 0x10) {
        self->SpawnMegaCharParticles(*f2, 0);
        self->PoofDust();
        f2->IncMegaKillCount();
        self->KillAndTrackInDeathTable();
        return;
    }

    if (self->mState == data_ov032_02113abc) return;
    if (self->mState == data_ov032_02113a7c) return;

    Vector3 hv;
    hv.x = self->mPosX;
    hv.y = self->mPosY;
    hv.z = self->mPosZ;
    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(f2, &hv, 2, 0xc000, 1, 0, 1);
}

/* True if Bubba should abort chase (no player, wall/floor, too far). */
// @symbol func_ov032_02111350
extern "C" int func_ov032_02111350(daBakubaku_c *self)
{
    if (self->ClosestPlayer() == 0) return 1;
    if (self->mWithMeshClsn.IsOnWall() != 0
        || self->mWithMeshClsn.IsOnGround() != 0)
        return 1;
    if (Vec3_HorzDist(
            (struct Vector3 *)&self->mSpawnPosX,
            (struct Vector3 *)&self->mPosX) > 0x4b0000)
        return 1;
    if (self->mState != data_ov032_02113abc) {
        if (data_0209f32c[0] < self->mPosY) return 1;
    }
    return 0;
}

/* True if Mario is a chase target; stores his position. */
// @symbol func_ov032_02111254
extern "C" int func_ov032_02111254(daBakubaku_c *self)
{
    Player *pl = self->ClosestPlayer();
    int *s;
    void *t;
    int d;
    if (pl == 0 || self->mChaseCooldown != 0)
        return 0;
    s = (int *)&pl->mPosX;
    self->mTargetPosX = s[0];
    self->mTargetPosY = s[1];
    self->mTargetPosZ = s[2];
    t = self->mState;
    if (t != data_ov032_02113abc && t != data_ov032_02113a7c) {
        if (pl->mIsUnderwater == 0)
            return 0;
    }
    d = pl->mGroundY - data_0209f32c[0];
    if (d < 0) d = -d;
    if (d < 0xb4000)
        return 0;
    if (t != data_ov032_02113abc && t != data_ov032_02113a7c) {
        if (Vec3_HorzDist(
                (const Vector3 *)&self->mSpawnPosX,
                (const Vector3 *)&self->mTargetPosX) > 0x4b0000)
            return 0;
    }
    return 1;
}
