//cpp
/**
 * d_a_holhei.cpp
 * Object - Chuckya (HOLHEI 190)
 *
 * ov062/daHolhei_c. Source order is the reverse of the ROM: mwccarm emits
 * one .text section per function backwards. The destructor stays inline in
 * include/daHolhei_c.h so the cartridge gets D1 then D0 and no D2.
 * common.h is first so Matrix4x3 is the flat s32 m[12]; the nested
 * math/Matrix.h spelling scalarizes a twelve-word copy.
 *
 * Known limits, measured in this TU:
 * - dCcAc_c::Init, dBgCh_Actr::Init, ModelAnim::SetAnim,
 *   dActor_c::DropShadowRadHeight, dActor_c::SpawnCoins and
 *   dEnemyBase_c::KillByInvincibleChar stay mangled. Each carries
 *   Fix12<int> by value (notes/mwccarm-codegen.md, wall 6az).
 * - _ZNK10dBgCh_Actr14GetFloorResultEv is not declared on dBgCh_Actr.h.
 *   func_ov062_02115f84 is the caller.
 * - The six file handles and the state records keep data_ov062_* names.
 *   __sinit_ov062_0211cf30 constructs them; symbols.txt has no other name.
 *   The second word of each file handle is the loaded BMD or BCA.
 * - g_profile_HOLHEI stays outside this text-only TU.
 * - Helpers keep func_ov062_* names. daHolhei_c_ChangeState is coined.
 * - func_ov062_02116010 keeps the address launders on mFlags and
 *   mdCc_c.flags. func_ov062_02116d28 reloads mHeld before each pos word.
 * - func_ov062_02116274 adds 0x500 through (int)this + 0x94. A member
 *   add is four bytes short. func_ov062_02116a08 copies the player's
 *   position from one base at +0x5c; three mPos loads are one instruction
 *   short. Finished and WillHitFrame take the Animation base at
 *   this+0x350 in one add; mModel+0x50 is two.
 * - The grabbed actor's +0xc8 and this object's +0xc8 are the unnamed gap
 *   before dActor_c::mAreaId. func_ov062_02116e80 stores &mHoldMtx there.
 * - func_ov062_021165e8 keeps a volatile Vector3 so the player's position
 *   occupies that stack slot.
 * - Animation::Finished / WillHitFrame are called through the Animation
 *   base at mModel+0x50. A call on mModel itself goes through the thunk.
 */

#include "common.h"
#include "daHolhei_c.h"
#include "SharedFilePtr.h"
#include "Player.h"
#include "SurfaceInfo.h"
#include "Sound.h"

/* Second word is the loaded file. SharedFilePtr.h has no fields.
   The externs below keep the spellings already banked against
   __sinit_ov062_0211cf30; this view is only a cast at the use. */
struct HolheiFile { int id; void *file; };

/* daHolhei_c_ChangeState stores a pointer to one of these and calls the
   PMF at offset 0. Behavior calls the PMF at offset 8. The records
   themselves are filled by __sinit_ov062_0211cf30. */
struct C;
typedef int (C::*PMF)();
struct C { char pad[0x364]; PMF *pp; };
struct Klass;
typedef void (Klass::*KPMF)();
struct M { char pad[8]; KPMF pmf; };

int ApproachLinear(int &ref, int target, int step);

extern "C" {
/* local extern: Fix12<int> by value, wall 6az. Header method homes the arg. */
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void* self, dActor_c* a, int r, int h, unsigned int e, unsigned int g);
/* local extern: Fix12<int> by value, wall 6az. Header method homes the arg. */
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void* self, dActor_c* a, int r, int h, Vector3_16* p, Vector3_16* q);
/* local extern: Fix12<int> by value, wall 6az. Header method homes the arg. */
extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *m, void *f, int a, int b, unsigned int e);
/* local extern: Fix12<int> by value, wall 6az. Header method homes the arg. */
extern void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void *, ShadowModel &sm, Matrix4x3 &mtx, int a, int b, unsigned int c);
/* local extern: Fix12<int> by value, wall 6az. Header method homes the arg. */
extern int _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(void* a, Vector3* v, unsigned n, int f, short s);
/* local extern: Fix12<int> by value, wall 6az. Header method homes the arg. */
extern void _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(void *c, void *v, void *a, int flag);

extern char* _ZNK10dBgCh_Actr14GetFloorResultEv(void* p);
extern short func_02010844(void* unused, Vector3* v, short angle);
extern void func_02012694(int a, void *p);
extern int daHolhei_c_ChangeState(void *c, void *p);
extern int func_ov002_020db5f4(void *c, void *arg);
extern int func_ov002_020db54c(int p, int a, int b, int s);
extern void func_0200d8c8(void* cam, void* v, int strength);
extern int data_02092138;
extern void* data_0209f318;
extern int ApproachAngle(void* angle, int target, int a, int b, int c);
extern int data_0209e650[];
extern s16 Vec3_HorzAngle(const void* v0, const void* v1);
extern int Vec3_Dist(const void* a, const void* b);
extern Matrix4x3 IDENTITY_MATRIX4X3;
extern void Matrix4x3_FromRotationY(struct Matrix4x3 *mF, s16 angY);
extern void MulMat4x3Mat4x3(const struct Matrix4x3 *m1, const struct Matrix4x3 *m0, struct Matrix4x3 *mF);
extern int Math_Function_0203b14c(void*, int, int, int, int);
extern unsigned short DecIfAbove0_Short(unsigned short* p);
extern signed char data_0209f2f8;
extern int AngleDiff(int a, int b);
extern unsigned int RandomIntInternal(void* s);

extern int data_ov062_0211ddf0[]; /* model, file id 0x327 */
extern int data_ov062_0211dde8[]; /* animation 0x328 */
extern void *data_ov062_0211dde0; /* animation 0x329 */
extern void *data_ov062_0211de00[]; /* animation 0x32a, grab */
extern int *data_ov062_0211de08[]; /* animation 0x32b */
extern void *data_ov062_0211ddf8; /* animation 0x32c, walk */

extern int data_ov062_0211dea0[];
extern int data_ov062_0211deb0[];
extern char data_ov062_0211df00[];
extern char data_ov062_0211de70[];
extern char data_ov062_0211de90[];
extern char data_ov062_0211dee0[];
extern char data_ov062_0211dec0[];
extern char data_ov062_0211ded0[];
extern int data_ov062_0211de80[];
extern int data_ov062_0211def0[];

/* x, y, z words of two Vector3s, stride 0xc. __sinit writes both triples. */
extern int data_ov062_0211df10[];
extern int data_ov062_0211df14[];
extern int data_ov062_0211df18[];

extern int func_ov062_02115f84(char* c);
void func_ov062_02116010(void* self);
void func_ov062_02116e80(void* c);
void func_ov062_02116dbc(char* c);
extern void func_ov062_02116edc(void *c);
}

// @symbol daHolhei_c_classInit
/* The cartridge's body is the one `new`: 0x438 into fBase_c::operator new,
   dEnemyBase_c's C2, this TU's vtable, then the member C1s. */
extern "C" daHolhei_c *daHolhei_c_classInit(void)
{
    return new daHolhei_c();
}

// @symbol _ZN10daHolhei_c16OnAimedAtWithEggEv
s32 daHolhei_c::OnAimedAtWithEgg() {
    return 0xca000;
}

// @symbol _ZN10daHolhei_c13InitResourcesEv
/* One model and five animations, the shadow, both collision volumes,
   home and previous positions, then the starting state. */
int daHolhei_c::InitResources()
{
    mModel.SetFile((BMD_File *)Model::LoadFile(*(SharedFilePtr *)data_ov062_0211ddf0), 1, -1);
    mShadowModel.InitCylinder();
    Animation::LoadFile(*(SharedFilePtr *)data_ov062_0211dde8);
    Animation::LoadFile(*(SharedFilePtr *)&data_ov062_0211dde0);
    Animation::LoadFile(*(SharedFilePtr *)data_ov062_0211de00);
    Animation::LoadFile(*(SharedFilePtr *)data_ov062_0211de08);
    Animation::LoadFile(*(SharedFilePtr *)&data_ov062_0211ddf8);
    mVertAccel = -0x3000;
    mTerminalVelocity = -0x1e000;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCc_c, (dActor_c*)this, 0xc8000, 0xfa000, 0x200004, 0x3010);
    mModel.speed = 0x1000;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mMeshClsn, (dActor_c*)this, 0x118000, 0x118000, 0, 0);
    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    unk_108 = 1;
    unk_10a = 4;
    mPrevPosX = mPosX;
    mPrevPosY = mPosY;
    mPrevPosZ = mPosZ;
    ::daHolhei_c_ChangeState(this, data_ov062_0211dee0);
    return 1;
}

// @symbol _ZN10daHolhei_c8BehaviorEv
int daHolhei_c::Behavior()
{
    char* c = (char*)this;

    if (UpdateKillByInvincibleChar(mMeshClsn, mModel, 3))
        return 1;

    DecIfAbove0_Short((unsigned short *)&mStateTimer);
    DecIfAbove0_Short(&mTurnWait);
    DecIfAbove0_Short(&mChaseCooldown);

    {
        M* m = (M*)mState;
        if (m->pmf != 0)
            (((Klass*)c)->*(m->pmf))();
    }

    mAngleX = mPrevAngleX;
    mAngleY = mPrevAngleY;
    mAngleZ = mPrevAngleZ;
    UpdatePos((dCc_c*)&mdCc_c);

    if (mState != (void*)data_ov062_0211dea0) {
        if (mState != (void*)data_ov062_0211dec0
            && mState != (void*)data_ov062_0211de70) {
            int atEdge = 0;
            signed char t = data_0209f2f8;
            if (t == 0x16) {
                if (mPosX > (int)0xff95c000)
                    atEdge = 1;
            } else if (t == 0x15) {
                if (mPosZ < (int)0xff2f4000)
                    atEdge = 1;
            }
            if (atEdge != 0
                || (mHorzSpeed != 0
                    && IsGoingOffCliff(mMeshClsn, 0x3c000, (s16)0x2888, 0, 1, 0x32000))) {
                mHorzSpeed = 0;
                mEdgeStop = 1;
                mPosX = mPrevPosX;
                mPosY = mPrevPosY;
                mPosZ = mPrevPosZ;
            } else {
                if (mEdgeStop == 1)
                    mEdgeStop = 0;
            }
        }
        mPrevPosX = mPosX;
        mPrevPosY = mPosY;
        mPrevPosZ = mPosZ;
        UpdateWMClsn(mMeshClsn, 3);
    }

    if (mState == (void*)data_ov062_0211ded0
        || mState == (void*)data_ov062_0211dee0
        || mState == (void*)data_ov062_0211de90
        || mState == (void*)data_ov062_0211df00) {
        func_ov062_02116010(c);
    }

    mdCc_c.Clear();
    mModel.Advance();
    mModel.UpdateVerts();

    {
        char* p3f8 = (char*)mHeld;
        if (p3f8 != 0) {
            int flag = (mFlags & 0x4000) != 0;
            if (flag) {
                if (*(int*)(p3f8 + 0xc8) != 0) {
                    func_ov062_02116d28();
                    goto ret;
                }
            }
        }
    }

    mdCc_c.Update();
    mCarryOffsX = 0;
    mCarryOffsY = 0;
    mCarryOffsZ = 0;
    func_ov062_02116e80(c);
    func_ov062_02116dbc(c);

ret:
    return 1;
}

// @symbol _ZN10daHolhei_c6RenderEv
int daHolhei_c::Render()
{
    void *held = mHeld;
    if (held != 0) {
        int flags = mFlags;
        int flag = (flags & 0x4000) ? 1 : 0;
        if (flag != 0) {
            if (*(int*)((char*)held + 0xc8) != 0) {
                func_ov062_02116edc(this);
            }
        }
    }
    /* ModelAnim overrides Render; the ROM calls Model::Render on this member. */
    mModel.Model::Render((const Vector3 *)0);
    return 1;
}

// @symbol _ZN10daHolhei_c16OnPendingDestroyEv
void daHolhei_c::OnPendingDestroy()
{
}

// @symbol _ZN10daHolhei_c16CleanupResourcesEv
int daHolhei_c::CleanupResources()
{
    ((SharedFilePtr *)data_ov062_0211ddf0)->Release();
    ((SharedFilePtr *)data_ov062_0211dde8)->Release();
    ((SharedFilePtr *)&data_ov062_0211dde0)->Release();
    ((SharedFilePtr *)data_ov062_0211de00)->Release();
    ((SharedFilePtr *)data_ov062_0211de08)->Release();
    ((SharedFilePtr *)&data_ov062_0211ddf8)->Release();
    return 1;
}

/* Home the carry offset toward the param1-selected Vector3, then publish
   the carried matrix onto the model. */
// @symbol func_ov062_02116edc
extern "C" void func_ov062_02116edc(void* c_){
    daHolhei_c *self = (daHolhei_c *)c_;
    int idx = 0;
    if (self->mHeld->param1 == 2)
        idx = 1;
    int k = idx * 0xc;
    Math_Function_0203b14c(&self->mCarryOffsX, *(int*)((char*)data_ov062_0211df10 + k), 0x800, 0x3e8000, 4);
    Math_Function_0203b14c(&self->mCarryOffsY, *(int*)((char*)data_ov062_0211df14 + k), 0x800, 0x3e8000, 4);
    Math_Function_0203b14c(&self->mCarryOffsZ, *(int*)((char*)data_ov062_0211df18 + k), 0x800, 0x3e8000, 4);
    void* r = self->UpdateCarry(*(Player *)self->mHeld, *(Vector3 *)&self->mCarryOffsX);
    struct M12w { int w[12]; };
    *(M12w *)&self->mModel.mat4x3 = *(M12w *)r;
}

/* Bone 3's transform times the model's yaw, stored where a holder looks. */
// @symbol func_ov062_02116e80
extern "C" void func_ov062_02116e80(void *c) {
    daHolhei_c *self = (daHolhei_c *)c;
    Matrix4x3_FromRotationY(&self->mModel.mat4x3, self->mAngleY);
    self->mModel.mat4x3.m[9] = self->mPosX >> 3;
    self->mModel.mat4x3.m[10] = self->mPosY >> 3;
    self->mModel.mat4x3.m[11] = self->mPosZ >> 3;
    MulMat4x3Mat4x3(&self->mModel.data.transforms[3],
                    &self->mModel.mat4x3,
                    &self->mHoldMtx);
    *(int *)((char *)self + 0xc8) = (int)&self->mHoldMtx;
}

/* Shadow under this actor. Off the ground the radius grows. */
// @symbol func_ov062_02116dbc
extern "C" void func_ov062_02116dbc(char* thiz)
{
    daHolhei_c *self = (daHolhei_c *)thiz;
    struct M12w { int w[12]; };
    *(M12w *)&self->mShadowMtx = *(M12w *)&IDENTITY_MATRIX4X3;
    self->mShadowMtx.m[9] = self->mPosX >> 3;
    self->mShadowMtx.m[10] = self->mPosY >> 3;
    self->mShadowMtx.m[11] = self->mPosZ >> 3;
    if (self->mMeshClsn.IsOnGround() != 0) {
        _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
            self, self->mShadowModel, self->mShadowMtx, 0x12c000, 0x32000, 0xf);
    } else {
        _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
            self, self->mShadowModel, self->mShadowMtx, 0x12c000, 0x3e8000, 0xf);
    }
}

/* Shadow under the held actor. mHeld is reloaded before each word. */
// @symbol _ZN10daHolhei_c19func_ov062_02116d28Ev
void daHolhei_c::func_ov062_02116d28()
{
    struct M12w { int w[12]; };
    *(M12w *)&mShadowMtx = *(M12w *)&IDENTITY_MATRIX4X3;
    char *o = *(char **)((char *)this + 0x3f8);
    mShadowMtx.m[9] = *(int *)(o + 0x5c) >> 3;
    o = *(char **)((char *)this + 0x3f8);
    mShadowMtx.m[10] = *(int *)(o + 0x60) >> 3;
    o = *(char **)((char *)this + 0x3f8);
    mShadowMtx.m[11] = *(int *)(o + 0x64) >> 3;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        this, mShadowModel, mShadowMtx, 0x12c000, 0x32000, 0xf);
}

/* Coined. Calls the PMF at the front of the state record, which is the
   enter function; Behavior later calls the one at +8. */
namespace tu {
// @symbol daHolhei_c_ChangeState
extern "C" int daHolhei_c_ChangeState(C *c, PMF *p) { c->pp = p; PMF *q = c->pp; if (*q == 0) return 1; return (c->**q)(); }
}

/* Face home, on the walk animation, and wait before turning. */
// @symbol _ZN10daHolhei_c19func_ov062_02116c78Ev
int daHolhei_c::func_ov062_02116c78(){
    mTurnWait = 0x3c;
    mChargeStep = 0;
    mModel.speed = 0x1000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModel, ((HolheiFile *)&data_ov062_0211ddf8)->file, 0, 0x1000, 0);
    mTargetAngY = Vec3_HorzAngle(&mPosX, &mHomePosX);
    return 1;
}

/* Brake, then turn onto mTargetAngY and go back to the walk state. */
// @symbol _ZN10daHolhei_c19func_ov062_02116bf8Ev
int daHolhei_c::func_ov062_02116bf8(){
    ApproachLinear(mHorzSpeed, 0, 0x2000);
    if (mTurnWait == 0) {
        ApproachAngle(&mPrevAngleY, mTargetAngY, 0xa, 0x200, 0x100);
        if (AngleDiff(mTargetAngY, mAngleY) < 0x100)
            ::daHolhei_c_ChangeState(this, data_ov062_0211dee0);
    }
    return 1;
}

/* Pick a random facing and a random time on the walk animation. */
// @symbol func_ov062_02116b80
extern "C" int func_ov062_02116b80(char* c){
    daHolhei_c *self = (daHolhei_c *)c;
    self->mTargetAngY = (short)((RandomIntInternal(data_0209e650) >> 8) << 0xc);
    self->mStateTimer = (short)(((RandomIntInternal(data_0209e650) >> 8) & 0x3f) + 0x64);
    self->mModel.speed = 0x1000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModel, ((HolheiFile *)&data_ov062_0211ddf8)->file, 0, 0x1000, 0);
    return 1;
}

/* Walk. A wall or the edge bails out; a player near home pulls Chuckya off
   the path once the chase cooldown has run out. */
// @symbol func_ov062_02116a08
extern "C" int func_ov062_02116a08(char* c)
{
    daHolhei_c *self = (daHolhei_c *)c;
    struct { int vd0[3]; int vd1[3]; } L;
    int r;
    dActor_c *pl;

    L.vd0[0] = 0;
    L.vd0[1] = 0;
    L.vd0[2] = 0;

    Vec3_Dist(&self->mPosX, &self->mHomePosX);

    r = func_ov062_02115f84(c);
    if (r != 0 || self->mEdgeStop == 1) {
        if (r != 2)
            ::daHolhei_c_ChangeState(c, data_ov062_0211df00);
        else
            ::daHolhei_c_ChangeState(c, data_ov062_0211de70);
        return 1;
    }

    ApproachAngle(&self->mPrevAngleY, self->mTargetAngY, 0xa, 0x200, 0x100);

    self->mHorzSpeed = 0xa000;
    pl = self->ClosestPlayer();

    self->mMoveSound = Sound::PlayLong(self->mMoveSound, 3, 0x18a, *(const Vector3 *)&self->mCamSpacePosX, 0);

    if (pl != 0 && self->mChaseCooldown == 0) {
        int *pos = (int *)((int)pl + 0x5c);
        L.vd1[0] = pos[0];
        L.vd1[1] = pos[1];
        L.vd1[2] = pos[2];

        if (Vec3_Dist(&self->mHomePosX, L.vd1) < 0x3e8000) {
            self->mHorzSpeed = 0;
            ::daHolhei_c_ChangeState(c, data_ov062_0211de90);
        }
        return 1;
    }

    if (*(unsigned short *)&self->mStateTimer == 0)
        ::daHolhei_c_ChangeState(c, data_ov062_0211dee0);

    return 1;
}

/* Step back to the saved position and pick a new facing toward home. */
// @symbol _ZN10daHolhei_c19func_ov062_02116980Ev
int daHolhei_c::func_ov062_02116980() {
    mPosX = mPrevPosX;
    mPosY = mPrevPosY;
    mPosZ = mPrevPosZ;
    mHorzSpeed = 0;
    if (AngleDiff(mTargetAngY, Vec3_HorzAngle(&mPosX, &mHomePosX)) <= 0x2000)
        mTargetAngY = (s16)(mPrevAngleY - 0x1000);
    else
        mTargetAngY = Vec3_HorzAngle(&mPosX, &mHomePosX);
    mStateTimer = 0x46;
    return 1;
}

/* After the timer, either commit the turn into a walk or keep steering. */
// @symbol _ZN10daHolhei_c19func_ov062_02116894Ev
int daHolhei_c::func_ov062_02116894(){
    if (*(unsigned short *)&mStateTimer != 0)
        return 1;
    if (mHorzSpeed == 0)
        goto angle;
    if (func_ov062_02115f84((char *)this) != 0 || mEdgeStop == 1) {
        mTargetAngY = (s16)(mPrevAngleY - 0x2000);
        goto angle;
    }
    mPrevAngleY = mTargetAngY;
    ::daHolhei_c_ChangeState(this, data_ov062_0211dee0);
    mChaseCooldown = 0x1e;
    mTargetAngY = mPrevAngleY;
    return 1;
angle:
    if (AngleDiff(mTargetAngY, mAngleY) < 0x100)
        mHorzSpeed = 0xa000;
    ApproachAngle(&mPrevAngleY, mTargetAngY, 0xa, 0x200, 0x100);
    return 1;
}

/* Stop, and aim the next turn at home. */
// @symbol func_ov062_02116850
extern "C" s16 func_ov062_02116850(void* c) {
    daHolhei_c *self = (daHolhei_c *)c;
    self->mVertSpeed = 0;
    self->mTargetAngY = Vec3_HorzAngle(&self->mPosX, &self->mHomePosX);
    self->mStateTimer = 0x14;
    self->mHorzSpeed = 0;
    return 1;
}

/* Turn toward mTargetAngY. Facing it starts the walk; otherwise the timer
   is refreshed. When the timer expires, cool down and return to the walk. */
// @symbol _ZN10daHolhei_c19func_ov062_021167c0Ev
int daHolhei_c::func_ov062_021167c0(){
    ApproachAngle(&mPrevAngleY, mTargetAngY, 0xa, 0x200, 0x100);
    if (AngleDiff(mTargetAngY, mAngleY) < 0x100)
        mHorzSpeed = 0xa000;
    else
        mStateTimer = 0x14;
    if (*(unsigned short *)&mStateTimer == 0) {
        mChaseCooldown = 0x1e;
        ::daHolhei_c_ChangeState(this, data_ov062_0211dee0);
    }
    return 1;
}

/* Restart the walk animation with the charge not yet begun. */
// @symbol func_ov062_02116784
extern "C" int func_ov062_02116784(char *c) {
    daHolhei_c *self = (daHolhei_c *)c;
    self->mChargeStep = 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModel, ((HolheiFile *)&data_ov062_0211ddf8)->file, 0, 0x1000, 0);
    return 1;
}

/* Charge the player. Step 0 turns, step 1 runs at the saved point, step 2
   gives up once Chuckya has gone past it. */
// @symbol _ZN10daHolhei_c19func_ov062_021165e8Ev
int daHolhei_c::func_ov062_021165e8()
{
    char* player;
    int r;
    volatile struct Vector3 pos;
    struct Vector3* pp;

    player = (char*)ClosestPlayer();
    r = func_ov062_02115f84((char *)this);
    if (r != 0 || mEdgeStop == 1) {
        if (r != 2)
            ::daHolhei_c_ChangeState(this, data_ov062_0211df00);
        else
            ::daHolhei_c_ChangeState(this, data_ov062_0211de70);
        return 1;
    }

    if (player == 0)
        return 1;

    pp = (struct Vector3*)(int)(player + 0x5c);
    pos.x = pp->x;
    pos.y = pp->y;
    pos.z = pp->z;

    if (mChargeStep == 0) {
        mTargetAngY = (short)HorzAngleToCPlayer();
        ApproachAngle(&mPrevAngleY, mTargetAngY, 0x80, 0x200, 0x400);
        if (AngleDiff(mTargetAngY, mAngleY) < 0x200) {
            mHorzSpeed = 0x1e000;
            mChargeStep = 1;
            mChasePosX = pos.x;
            mChasePosY = pos.y;
            mChasePosZ = pos.z;
        }
    }

    mMoveSound = Sound::PlayLong(mMoveSound, 3, 0x18a, *(const Vector3 *)&mCamSpacePosX, 0);

    {
        int s = mChargeStep;
        if (s != 0) {
            if (s == 1) {
                if (Vec3_Dist(&mPosX, &mChasePosX) < 0x3c000)
                    mChargeStep = 2;
            } else {
                if (Vec3_Dist(&mPosX, &mChasePosX) > 0xc8000)
                    ::daHolhei_c_ChangeState(this, data_ov062_0211ded0);
            }
        }
    }
    return 1;
}

// @symbol func_ov062_021165e0
extern "C" int func_ov062_021165e0(void)
{
    return 1;
}

/* While carried: play the grab animation once, then on release copy the
   holder's facing, mark the cylinder, and hop if param1 is 2. */
// @symbol _ZN10daHolhei_c19func_ov062_021164e8Ev
int daHolhei_c::func_ov062_021164e8()
{
    int flag;
    int t;
    if (mGrabAnim == 0) {
        t = (mFlags & 0x4000) != 0;
        if (t) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModel, ((HolheiFile *)data_ov062_0211de00)->file, 0, 0x1000, 0);
            mGrabAnim = 1;
        }
    }
    flag = mFlags;
    t = (flag & 0x400) != 0;
    if (t) goto do_block;
    t = (flag & 0x2000) != 0;
    if (t) goto do_block;
    t = (flag & 0x100) != 0;
    if (t) goto done;
do_block:
    {
        dActor_c *held = mHeld;
        mPrevAngleY = held->mAngleY;
        mdCc_c.flags |= 2;
        ::daHolhei_c_ChangeState(this, data_ov062_0211dec0);
        if (mHeld->param1 == 2) {
            mVertSpeed = 0x50000;
            mHorzSpeed = 0x14000;
        }
        mHeld = 0;
    }
done:
    return 1;
}

/* Pop off: gravity, a launch, and a clear of the runtime flags. */
// @symbol func_ov062_02116498
extern "C" int func_ov062_02116498(char* c){
    daHolhei_c *self = (daHolhei_c *)c;
    self->mVertAccel = -0x4000;
    self->mVertSpeed = 0x3c000;
    self->mHorzSpeed = 0xa000;
    self->mTerminalVelocity = -0x64000;
    func_02012694(0xf7, &self->mCamSpacePosX);
    self->mFlags = 0;
    return 1;
}

/* Below the death plane, or on landing: poof, shake, drop coins, die. */
// @symbol func_ov062_021163b0
extern "C" int func_ov062_021163b0(char* c)
{
    daHolhei_c *self = (daHolhei_c *)c;
    Vector3 v[2];
    if (data_02092138 > self->mPosY) {
        self->KillAndTrackInDeathTable();
        return 1;
    }
    if (self->mMeshClsn.IsOnGround() != 0) {
        self->TriplePoofDust();
        self->KillAndTrackInDeathTable();
        func_02012694(0x125, &self->mCamSpacePosX);
        func_0200d8c8(data_0209f318, &self->mPosX, 0x7d0000);
        v[0].x = self->mPosX;
        v[0].y = self->mPosY;
        v[0].z = self->mPosZ;
        v[0].y += 0x32000;
        v[1].x = self->mPosX;
        v[1].y = self->mPosY;
        v[1].z = self->mPosZ;
        _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(c, &v[1], self->unk_10a + 1, 0xa000, 0);
    }
    return 1;
}

// @symbol func_ov062_02116368
extern "C" int func_ov062_02116368(void* c) {
    daHolhei_c *self = (daHolhei_c *)c;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModel, ((HolheiFile *)data_ov062_0211dde8)->file, 0x40000000, 0x1000, 0);
    self->mChargeStep = 0;
    return 1;
}

/* When the animation finishes, leave this state. */
// @symbol func_ov062_0211632c
extern "C" int func_ov062_0211632c(void* c){
    daHolhei_c *self = (daHolhei_c *)c;
    if (((Animation *)((char *)c + 0x350))->Finished()) {
        self->mChargeStep = 0;
        ::daHolhei_c_ChangeState(c, data_ov062_0211de80);
    }
    return 1;
}

/* Start an animation, then wait a random few frames facing a random way. */
// @symbol func_ov062_021162b8
extern "C" int func_ov062_021162b8(char* c){
    daHolhei_c *self = (daHolhei_c *)c;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModel, ((HolheiFile *)&data_ov062_0211dde0)->file, 0, 0x1000, 0);
    self->mStateTimer = (short)(((RandomIntInternal(data_0209e650) >> 8) & 0x1f) + 0x14);
    self->mTargetAngY = (short)((RandomIntInternal(data_0209e650) >> 8) << 0xc);
    return 1;
}

/* Spin, and leave once the timer reaches zero. */
// @symbol func_ov062_02116274
extern "C" int func_ov062_02116274(unsigned char *c)
{
    /* mPrevAngleY and mStateTimer. The (int) cast is what this function's bytes do. */
    short *p94 = (short *)((int)c + 0x94);
    short val = *p94;
    unsigned short *p100 = (unsigned short *)((int)c + 0x100);
    val += 0x500;
    *p94 = val;
    if (*p100 == 0)
        ::daHolhei_c_ChangeState(c, data_ov062_0211deb0);
    return 1;
}

// @symbol func_ov062_02116238
extern "C" int func_ov062_02116238(char *c){
    daHolhei_c *self = (daHolhei_c *)c;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModel, ((HolheiFile *)data_ov062_0211de08)->file, 0x40000000, 0x1000, 0);
    return 1;
}

/* On frame 0x14 of the throw animation, launch whoever is held. When the
   animation finishes, clear the cylinder bit and leave. */
// @symbol func_ov062_021161a8
extern "C" int func_ov062_021161a8(char *c)
{
    daHolhei_c *self = (daHolhei_c *)c;
    if (self->mHeld != 0 && ((Animation *)((char *)c + 0x350))->WillHitFrame(0x14)) {
        func_ov002_020db54c((int)self->mHeld, 0x28000, 0x50000, self->mAngleY);
        self->mHeld = 0;
        func_02012694(0x126, &self->mCamSpacePosX);
    }
    if (((Animation *)((char *)c + 0x350))->Finished()) {
        self->mdCc_c.flags &= ~2;
        ::daHolhei_c_ChangeState(c, data_ov062_0211ded0);
    }
    return 1;
}

/* A mega hit kills. A player in front can grab; otherwise Chuckya grabs
   the player. The three masked stores are the bytes' own address math. */
extern "C" {
namespace tu {
// @symbol func_ov062_02116010
void func_ov062_02116010(char *c)
{
    daHolhei_c *self = (daHolhei_c *)c;
    u32 id;
    dActor_c *a;
    int b;
    int angle;
    struct Vector3_16 v;

    id = self->mdCc_c.otherOwner;
    if (id != 0 &&
        (a = dActor_c::FindWithID(id)) != 0 &&
        (self->mdCc_c.hitFlags & 0x10) != 0) {
        v.x = 0x1000;
        v.y = 0;
        v.z = 0;
        _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(c, &v, a, 0);
        func_02012694(0x125, &self->mCamSpacePosX);
        return;
    }

    *(int *)(((long long)(int)(c + 0xb0)) & 0xFFFFFFFFFFFFFFFFLL) &= ~0x80;

    id = self->mdCc_c.otherOwner;
    if (id == 0)
        return;
    a = dActor_c::FindWithID(id);
    if (a == 0)
        return;
    b = (int)(a->actorID == 0xbf);
    if (b == 0)
        return;
    angle = self->HorzAngleToCPlayer();
    angle = AngleDiff(angle, self->mAngleY);
    if (angle > 0x2000) {
        if ((self->mdCc_c.hitFlags & 0x1000) == 0)
            return;
        *(int *)(((int)c + 0xb0) & 0xFFFFFFFFFFFFFFFFULL) |= 0x80;
        if (((Player *)a)->TryGrab(*(dActor_c *)c) == 0)
            return;
        self->mHeld = a;
        *(int *)(((long long)(int)(c + 0x128)) & 0xFFFFFFFFFFFFFFFFLL) |= 2;
        self->mHorzSpeed = 0;
        ::daHolhei_c_ChangeState(c, data_ov062_0211dea0);
        return;
    }
    if (func_ov002_020db5f4(a, c) == 0)
        return;
    self->mHeld = a;
    self->mHorzSpeed = 0;
    ::daHolhei_c_ChangeState(c, data_ov062_0211def0);
}
}
}

/* 1 on a wall, 2 on a slope steeper than 0x1000, else 0. */
extern "C" {
namespace tu {
// @symbol func_ov062_02115f84
int func_ov062_02115f84(char* c) {
    daHolhei_c *self = (daHolhei_c *)c;
    Vector3 v;
    short slope = 0;
    if (self->mMeshClsn.IsOnGround()) {
        char* floorResult = _ZNK10dBgCh_Actr14GetFloorResultEv(&self->mMeshClsn);
        ((SurfaceInfo *)(floorResult+4))->CopyNormalTo(v);
        slope = func_02010844(c, &v, self->mAngleY);
    }
    if (self->mMeshClsn.IsOnWall())
        return 1;
    if (slope < 0)
        slope = (short)-slope;
    if (slope > 0x1000)
        return 2;
    return 0;
}
}
}
