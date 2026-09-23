//cpp
/* dMgJump3DMario_c: one of the three Marios the two jump minigames
 * (Bounce and Pounce, MG_JUMP and MG_JUMP2) hold as mPlayers[3].
 *
 * A pointer-to-member state machine: mState is installed from the 8-byte
 * records at data_ov006_0213b020..0213b098. Each update applies gravity,
 * integrates mVel into mPos, runs the current state and re-projects the
 * screen position. Tap near Mario and he bounces; miss and he damps out and
 * respawns.
 *
 * Source order is ROM order, which `#pragma defer_codegen off` keeps; do not
 * reorder. The destructor is not virtual, so the first declared virtual
 * (Unk_020c76d8) is the key function and this file emits the vtable.
 *
 * Include common.h first: the mat4x3 copy in func_ov006_020c76e0 needs its
 * flat s32 m[12]; math/Matrix.h's nested form scalarizes the copy.
 * decl_common.h is left out because four of the data symbols here collide
 * with its spellings.
 *
 * Still raw:
 * - The eight func_ov006_ members keep their linker names because
 *   unpromoted C files still call them by those names (issue #2722).
 * - ModelAnim::SetAnim takes Fix12<int> by value; its real spelling makes
 *   the compiler home the argument and the size changes. NewSimple takes
 *   Fix12<int> the same way and has no header. Both stay mangled.
 * - The helper functions and data_ov006_ tables have no header or name yet.
 */

#pragma defer_codegen off

#include "common.h"
#include "dMgJump3DMario_c.h"
#include "SharedFilePtr.h"
#include "OAM.h"
#include "fBase_c.h"
#include "Sound.h"

static const int kWallX = 0x6c000;   /* mPos.x clamp: the arena walls */
static const int kScreenYMax = 0xbc; /* past this mScreenY the Mario damps out */
static const int kAnimSpeed = 0x800; /* SetAnim speed every install uses */

/* OAM.h forward-declares Matrix2x2 and says the files that pass one keep
 * their own 4 x s32 definition; this is that definition. */
struct Mtx { int a, b, c, d; };

/* Everything this file calls or reads that no header declares. Keep it
 * above the first function. */
extern "C" {

/* Defined below, called from above. */
void func_ov006_020c8658(void *c);

/* Fix12<int> by value in the real signatures; see the banner. */
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *file, int a, int b, unsigned int d);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
void Sound_PlayBank1Panned(int a, int b, int c);
void Vec3_Sub(Jump3DVec *out, Jump3DVec *a, Jump3DVec *b);
void AddVec3(Jump3DVec *a, Jump3DVec *b, Jump3DVec *c);
int  NormalizeVec3IfNonZero(Jump3DVec *v);
int  RandomIntInternal(int *seed);
void func_02012718(int a, int b);
int  func_02053200(int x);
void func_02016a14(void *self, int a);
void func_02016a04(void *self, int a);
int  func_020179b4(SharedFilePtr *f, ModelBase *m, int extra);
void func_ov006_020bfec0(void *cam, Jump3DVec *pos, short *out);
void func_ov006_020e6e3c(int a, int b);
void func_ov006_020c8c78(int a, int b);
void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationY(void *m, short angY);

/* The mState records this file installs or compares. */
extern Jump3DState data_ov006_0213b020;
extern Jump3DState data_ov006_0213b028;
extern Jump3DState data_ov006_0213b030;
extern Jump3DState data_ov006_0213b038;
extern Jump3DState data_ov006_0213b048;
extern Jump3DState data_ov006_0213b050;
extern Jump3DState data_ov006_0213b058;
extern Jump3DState data_ov006_0213b060;
extern Jump3DState data_ov006_0213b068;
extern Jump3DState data_ov006_0213b070;
extern Jump3DState data_ov006_0213b078;
extern Jump3DState data_ov006_0213b080;
extern Jump3DState data_ov006_0213b088;
extern Jump3DState data_ov006_0213b090;

/* Slots the current mAnimIdx (0/1/2) picks the anim file from. */
extern BCA_File **data_ov006_0213b098[];

/* Tuning words, read as plain ints. */
extern int data_ov006_0213b008;
extern int data_ov006_0213b00c;
extern int data_ov006_0213b010;
extern int data_ov006_0213b018;
extern int data_ov006_0213b01c;
extern int data_ov006_02140428;
extern int data_ov006_02140434;

/* The six anim files func_ov006_020c87d0 loads. The destructor clears only
 * five of them; 0214041c survives in the ROM too. */
extern BCA_File *data_ov006_02140408;
extern BCA_File *data_ov006_0214040c;
extern BCA_File *data_ov006_0214041c;
extern BCA_File *data_ov006_02140424;
extern BCA_File *data_ov006_0214042c;
extern BCA_File *data_ov006_02140430;

/* The model file plus the six anim files. */
extern SharedFilePtr data_ov006_02140438;
extern SharedFilePtr data_ov006_02140440;
extern SharedFilePtr data_ov006_02140448;
extern SharedFilePtr data_ov006_02140450;
extern SharedFilePtr data_ov006_02140458;
extern SharedFilePtr data_ov006_02140460;
extern SharedFilePtr data_ov006_02140468;

extern u8  data_ov006_02140400;
extern u16 data_ov006_02140404;
extern void *data_ov006_02141a40;
extern void *data_ov006_02141a44;
extern OamAttr *data_ov006_02134d1c;
extern Vector3 data_ov006_0212ddd0;

extern u8 data_020a0e40;
extern u8 data_020a0de8[][4];
extern u8 data_020a0de9[][4];
extern u8 data_020a0dea[][4];
extern u8 data_020a0deb[][4];
extern Matrix4x3 data_020a0e68;
extern short data_02082214[];
extern int data_0209e650;
extern fBase_c *data_0209f5c0;

}  /* extern "C" */

void  UpdateAngle(short &angle, short target, int speed, short maxStep);
short ApproachLinear2(short &value, short target, short step);
void  ApproachLinear(int &value, int target, int step);


// @symbol _ZN16dMgJump3DMario_c12Unk_020c762cEv
/* Vtable slot 2. */
int dMgJump3DMario_c::Unk_020c762c()
{
    int r = 0;
    int m = 1;

    if (mState == data_ov006_0213b058)
        m = 0;
    if (m != 0) {
        m = 1;
        if (mState == data_ov006_0213b070)
            m = 0;
        if (m != 0)
            r = 1;
    }
    return r;
}


// @symbol _ZN16dMgJump3DMario_c12Unk_020c76d0Ev
/* Vtable slot 1. */
void *dMgJump3DMario_c::Unk_020c76d0()
{
    return &mVel;
}


// @symbol _ZN16dMgJump3DMario_c12Unk_020c76d8Ev
/* Vtable slot 0, and the key function. */
void *dMgJump3DMario_c::Unk_020c76d8()
{
    return &mPos;
}


extern "C" {
// @symbol func_ov006_020c76e0
/* Rebuilds the model matrix from mPos and mAngleY. */
void func_ov006_020c76e0(char *raw)
{
    dMgJump3DMario_c *self = (dMgJump3DMario_c *)raw;

    Matrix4x3_FromTranslation(&data_020a0e68, self->mPos.x, self->mPos.y, self->mPos.z);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, self->mAngleY);
    self->mModelAnim.mat4x3 = data_020a0e68;
}
}


extern "C" {
// @symbol func_ov006_020c7734
/* OAM shadow sprite plus the model render. */
void func_ov006_020c7734(char *raw)
{
    dMgJump3DMario_c *self = (dMgJump3DMario_c *)raw;
    short screen[2];
    int scaleX;
    int scaleY;
    int angle;
    int wave;
    Mtx mtx;

    if (self->mVisible == 0)
        return;

    if (data_ov006_02140400 != 0) {
        func_ov006_020bfec0(data_ov006_02141a44, &self->mPos, screen);

        angle = data_ov006_02140404;
        wave = data_02082214[(angle >> 4) * 2];
        screen[1] = screen[1] - (((wave << 2) + 0x30000) >> 12);
        scaleX = func_02053200((wave >> 2) + 0x1000);

        angle = data_ov006_02140404;
        scaleY = -func_02053200((data_02082214[(angle >> 4) * 2 + 1] >> 2) + 0x1000);

        /* Zero it through `int *`: writing mtx.a..mtx.d instead costs 11 words
           (notes/experiments/jump3d-2711-mtx-intptr-zeroing.md). */
        int *mp = (int *)&mtx;
        mp[0] = 0; mp[1] = 0; mp[2] = 0; mp[3] = 0;
        mtx.d = scaleY;
        mtx.a = scaleX;
        OAM::Render(false, data_ov006_02134d1c, screen[0], screen[1], -1, -1, (Matrix2x2 *)&mtx);
    }

    self->mModelAnim.Render(&data_ov006_0212ddd0);
}
}


extern "C" {
// @symbol func_ov006_020c7860
/* The per-frame update: gravity, integrate, dispatch mState. */
void func_ov006_020c7860(char *raw)
{
    dMgJump3DMario_c *self = (dMgJump3DMario_c *)raw;

    ApproachLinear(self->mVel.y, data_ov006_0213b010, data_ov006_0213b018);
    AddVec3(&self->mPos, &self->mVel, &self->mPos);
    (self->*self->mState)();
    func_ov006_020bfec0(data_ov006_02141a40, &self->mPos, &self->mScreenX);
    func_ov006_020c76e0(raw);
    self->mModelAnim.Animation::Advance();
}
}


// @symbol _ZN16dMgJump3DMario_c9StateDampEv
void dMgJump3DMario_c::StateDamp()
{
    mTimer -= 1;
    if (mTimer == 0) {
        ApproachLinear(data_ov006_02140428, 0, 1);
        Sound::PlayBank2_2D(0x130);
        func_ov006_020c8c78(mScreenX, 0xc0);
        EnterRespawn();
        return;
    }
    mVel.x = (int)(((s64)mVel.x * 0xc00 + 0x800) >> 12);
    if (mPos.x < -kWallX) {
        mPos.x = -kWallX;
        return;
    }
    if (mPos.x > kWallX)
        mPos.x = kWallX;
}


// @symbol _ZN16dMgJump3DMario_c9EnterDampEv
void dMgJump3DMario_c::EnterDamp()
{
    if (data_ov006_02140428 > 1)
        Sound::PlayBank2_2D(0x1ca);
    else
        Sound::PlayBank2_2D(0x1c9);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, data_ov006_0214042c, 0, kAnimSpeed, 0);
    mTimer = 0x28;
    mState = data_ov006_0213b030;
}


// @symbol _ZN16dMgJump3DMario_c9StateHoldEv
void dMgJump3DMario_c::StateHold()
{
    u32 idx = data_020a0e40;
    int flag = 0;

    if (data_020a0de8[idx][0] != 0) {
        if (data_020a0de9[idx][0] != 0) {
            flag = 1;
        }
    }
    if (flag != 0) {
        /* Keep reusing `flag` here: reading the table straight into `b`
           costs 5 words (notes/experiments/jump3d-2711-statehold-flag-reuse.md). */
        flag = data_020a0deb[idx][0];
        int b = flag;
        int x = mScreenY - 0x20;
        int dz = mScreenX - data_020a0dea[idx][0];
        if (dz < 0) {
            dz = -dz;
        }
        if (dz < 0x18) {
            int dx = x - b;
            if (dx < 0) {
                dx = -dx;
            }
            if (dx < 0x26) {
                ApproachLinear2(mTimer, 0, 8);
            }
        }
    }
    {
        int v = mPos.x;
        if (v < -kWallX) {
            v = -kWallX;
        } else if (v > kWallX) {
            v = kWallX;
        }
        mPos.x = v;
    }
    if (mCommand == 1) {
        mCommand = 0;
        mVel.y = data_ov006_0213b008;
        EnterHit();
    } else if (ApproachLinear2(mTimer, 0, 1)) {
        mCommand = 0;
        if (mVel.y > 0) {
            EnterBounce();
            StateBounce();
        } else {
            EnterMove();
            StateMove();
        }
    } else {
        if (mScreenY < kScreenYMax) {
            return;
        }
        EnterDamp();
    }
}


// @symbol _ZN16dMgJump3DMario_c9EnterHoldEv
void dMgJump3DMario_c::EnterHold()
{
    /* Jump3DVec, not Vector3: Vector3 matches here too, but it makes this
       file emit Vector3's destructor, which the manifest does not license
       (notes/experiments/jump3d-2711-scratch-vector3.md). */
    Jump3DVec v;

    Vec3_Sub(&v, &mPos, &mAnchor);
    if (NormalizeVec3IfNonZero(&v) != 0) {
        mVel.x = v.x;
        mVel.y = v.y;
        mVel.z = v.z;
    } else {
        mVel.x = -mVel.x;
        mVel.y = -mVel.y;
    }
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, data_ov006_02140408, 0x40000000, kAnimSpeed, 0);
    mModelAnim.Animation::currFrame = 0;
    Sound_PlayBank1Panned(0, 6, mPos.x);
    mTimer = 0x20;
    mState = data_ov006_0213b028;
}


// @symbol _ZN16dMgJump3DMario_c9StateMoveEv
void dMgJump3DMario_c::StateMove()
{
    u16 st = mCommand;

    if (st == 1) {
        mCommand = 0;
        mVel.y = data_ov006_0213b008;
        if (mAnimIdx != 0)
            mAnimIdx = 0;
        else
            mAnimIdx = 1;
        EnterHit();
        return;
    }
    if (st == 2) {
        mCommand = 0;
        EnterHold();
        return;
    }

    {
        int flag = 0;
        u8 idx = data_020a0e40;
        if (data_020a0de8[idx][0] != 0 && data_020a0de9[idx][0] != 0)
            flag = 1;

        if (flag != 0) {
            s16 y = mScreenY;
            int ax = (int)data_020a0dea[idx][0];
            int az = (int)data_020a0deb[idx][0];
            int dx = (int)mScreenX - ax;
            int ym = (int)y - 0x20;
            int t = dx < 0 ? -dx : dx;

            if (t < 0x18) {
                t = ym - az;
                if (t < 0) t = -t;
                if (t < 0x26 && (int)y < kScreenYMax) {
                    int v[3];
                    int *p = &data_ov006_0213b00c;
                    int t0 = (ax - 0x80) << 12;
                    int t1 = (-az) << 12;
                    v[0] = t0;
                    v[1] = t1;
                    /* Keep the volatile store: a plain `v[2] = 0;` shrinks the
                       function to 0x3a8 bytes against the ROM's 0x3c4
                       (notes/experiments/jump3d-2711-statemove-volatile-store.md). */
                    *(volatile int *)&v[2] = 0;
                    mVel.y = *p;
                    mVel.x = data_ov006_0213b01c * dx;
                    if (mAnimIdx != 0)
                        mAnimIdx = 0;
                    else
                        mAnimIdx = 1;
                    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, *data_ov006_0213b098[mAnimIdx], 0x40000000, kAnimSpeed, 0);
                    mModelAnim.Animation::currFrame = 0;
                    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xed, mPos.x << 3, mPos.y << 3, mPos.z << 3);
                    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xee, v[0] << 3, v[1] << 3, v[2] << 3);
                    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xef, v[0] << 3, v[1] << 3, v[2] << 3);
                    Sound_PlayBank1Panned(0, mAnimIdx, mPos.x);
                    func_02012718(0x1c6, mScreenX << 12);
                    EnterBounce();
                }
            }
        }
    }

    if (mScreenY > kScreenYMax) {
        EnterDamp();
        return;
    }

    {
        int pos = mPos.x;
        if (pos < -kWallX && mVel.x < 0) {
            mVel.x = -(int)(((s64)mVel.x * 0xd00 + 0x800) >> 12);
            if (mVel.y > 0) {
                mAnimIdx = 2;
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, *data_ov006_0213b098[mAnimIdx], 0x40000000, kAnimSpeed, 0);
                mModelAnim.Animation::currFrame = 0;
                Sound_PlayBank1Panned(0, mAnimIdx, mPos.x);
            }
        } else if (pos > kWallX && mVel.x > 0) {
            mVel.x = -(int)(((s64)mVel.x * 0xd00 + 0x800) >> 12);
            if (mVel.y > 0) {
                mAnimIdx = 2;
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, *data_ov006_0213b098[mAnimIdx], 0x40000000, kAnimSpeed, 0);
                mModelAnim.Animation::currFrame = 0;
                Sound_PlayBank1Panned(0, mAnimIdx, mPos.x);
            }
        }
    }

    if (mVel.x > 0) {
        UpdateAngle(mAngleY, 0x2800, 2, 0x1000);
        return;
    }
    UpdateAngle(mAngleY, -0x2800, 2, 0x1000);
}


// @symbol _ZN16dMgJump3DMario_c9EnterMoveEv
void dMgJump3DMario_c::EnterMove()
{
    mState = data_ov006_0213b020;
}


// @symbol _ZN16dMgJump3DMario_c12StateFallOutEv
void dMgJump3DMario_c::StateFallOut()
{
    if (mPos.y >= -0x120000)
        return;
    mVel.y = 0;
    func_ov006_020c8658(this);
}


extern "C" {
// @symbol func_ov006_020c8084
void func_ov006_020c8084(char *raw)
{
    dMgJump3DMario_c *self = (dMgJump3DMario_c *)raw;

    if (self->mState == data_ov006_0213b088) {
        self->mVel.y = 0;
        func_ov006_020c8658(raw);
    } else {
        self->mVel.x = 0;
        self->mVel.y = 0x2000;
        Sound::PlayBank2_2D(0x1c9);
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim, data_ov006_0214042c, 0, kAnimSpeed, 0);
        self->mModelAnim.Animation::currFrame = 0;
        self->mState = data_ov006_0213b090;
    }
}
}


// @symbol _ZN16dMgJump3DMario_c12StateRiseOutEv
void dMgJump3DMario_c::StateRiseOut()
{
    if (mModelAnim.file == data_ov006_0214041c)
    {
        if (mModelAnim.Animation::WillHitFrame(0xc) ||
            mModelAnim.Animation::WillHitFrame(0x18))
            func_ov006_020e6e3c(0x1b5, mPos.x);
    }
    if (mPos.y <= 0x180000)
        return;
    mVel.y = 0;
    data_ov006_02140434++;
    func_ov006_020c8658(this);
}


extern "C" {
// @symbol func_ov006_020c81e0
void func_ov006_020c81e0(char *raw)
{
    dMgJump3DMario_c *self = (dMgJump3DMario_c *)raw;

    self->mVel.x = 0;
    self->mVel.y = data_ov006_0213b00c;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim, data_ov006_0214041c, 0x40000000, kAnimSpeed, 0);
    self->mModelAnim.Animation::currFrame = 0;
    Sound::PlayBank2_2D(0x10f);
    func_02012718(0x1b5, self->mScreenX << 12);
    self->mState = data_ov006_0213b080;
}
}


// @symbol _ZN16dMgJump3DMario_c8EnterHitEv
void dMgJump3DMario_c::EnterHit()
{
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, data_ov006_0214041c, 0x40000000, kAnimSpeed, 0);
    mModelAnim.Animation::currFrame = 0;
    if (mState == data_ov006_0213b068) {
        func_02012718(0x110, mScreenX << 12);
    } else {
        Sound_PlayBank1Panned(0, 4, mPos.x);
    }
    func_02012718(0x1b5, mScreenX << 12);
    mState = data_ov006_0213b078;
}


// @symbol _ZN16dMgJump3DMario_c11StateBounceEv
void dMgJump3DMario_c::StateBounce()
{
    if (mVel.y < 0) {
        EnterMove();
        StateMove();
        return;
    }

    if (mModelAnim.file == data_ov006_0214041c) {
        if (mModelAnim.Animation::WillHitFrame(0xc) != 0 ||
            mModelAnim.Animation::WillHitFrame(0x18) != 0)
            func_ov006_020e6e3c(0x1b5, mPos.x);
    }

    if (mPos.x < -kWallX && mVel.x < 0) {
        mVel.x = -(int)(((s64)mVel.x * 0xd00 + 0x800) >> 12);
        if (mVel.y > 0) {
            mAnimIdx = 2;
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, *data_ov006_0213b098[mAnimIdx], 0x40000000, kAnimSpeed, 0);
            mModelAnim.Animation::currFrame = 0;
            Sound_PlayBank1Panned(0, mAnimIdx, mPos.x);
        }
    } else if (mPos.x > kWallX && mVel.x > 0) {
        mVel.x = -(int)(((s64)mVel.x * 0xd00 + 0x800) >> 12);
        if (mVel.y > 0) {
            mAnimIdx = 2;
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, *data_ov006_0213b098[mAnimIdx], 0x40000000, kAnimSpeed, 0);
            mModelAnim.Animation::currFrame = 0;
            Sound_PlayBank1Panned(0, mAnimIdx, mPos.x);
        }
    } else {
        u16 t = mCommand;
        if (t == 1) {
            mCommand = 0;
            mVel.y = data_ov006_0213b008;
            EnterHit();
            return;
        }
        if (t == 2) {
            mCommand = 0;
            EnterHold();
            return;
        }
    }

    if (mVel.x > 0)
        UpdateAngle(mAngleY, 0x2800, 2, 0x1000);
    else
        UpdateAngle(mAngleY, -0x2800, 2, 0x1000);
}


// @symbol _ZN16dMgJump3DMario_c11EnterBounceEv
void dMgJump3DMario_c::EnterBounce()
{
    mState = data_ov006_0213b060;
}


// @symbol _ZN16dMgJump3DMario_c11StateWindUpEv
void dMgJump3DMario_c::StateWindUp()
{
    if (ApproachLinear2(mTimer, 0, 1) == 0) {
        mPos.y = 0;
        return;
    }
    mVisible = 1;
    mPos.y = 0;
    mVel.x = mVel.x << 1;
    mVel.y = data_ov006_0213b00c;
    mCommand = 0;
    EnterHit();
}


extern "C" {
// @symbol func_ov006_020c862c
void func_ov006_020c862c(int *raw, int timer)
{
    dMgJump3DMario_c *self = (dMgJump3DMario_c *)raw;

    self->mTimer = (s16)timer;
    self->mState = data_ov006_0213b050;
}
}


// @symbol _ZN16dMgJump3DMario_c9StateIdleEv
void dMgJump3DMario_c::StateIdle()
{
    mPos.y = 0;
}


extern "C" {
// @symbol func_ov006_020c8658
void func_ov006_020c8658(void *raw)
{
    dMgJump3DMario_c *self = (dMgJump3DMario_c *)raw;

    self->mVisible = 0;
    self->mPos.y = 0;
    self->mState = data_ov006_0213b048;
}
}


// @symbol _ZN16dMgJump3DMario_c12StateRespawnEv
void dMgJump3DMario_c::StateRespawn()
{
    unsigned int rnd;
    int speed;

    mTimer -= 1;
    if (mTimer == 0) {
        mPos.y = 0x100000;
        rnd = ((unsigned int)RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 0x13;
        mPos.x = ((int)rnd - 0x800) * 0xc0;
        mVel.y = 0;
        speed = data_ov006_0213b01c;
        rnd = ((unsigned int)RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 0x13;
        mVel.x = (int)(((s64)(((int)rnd - 0x800) << 1) * speed + 0x800) >> 12);
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, data_ov006_02140424, 0x40000000, kAnimSpeed, 0);
        EnterMove();
        return;
    }
    mPos.y = 0x100000;
    mVel.y = 0;
}


// @symbol _ZN16dMgJump3DMario_c12EnterRespawnEv
void dMgJump3DMario_c::EnterRespawn()
{
    mTimer = 0x3c;
    mPos.y = 0x100000;
    mVel.y = 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, data_ov006_02140424, 0x40000000, kAnimSpeed, 0);
    mState = data_ov006_0213b038;
}


extern "C" {
// @symbol func_ov006_020c87d0
/* Loads the model and the six anims. */
int func_ov006_020c87d0(char *raw)
{
    dMgJump3DMario_c *self = (dMgJump3DMario_c *)raw;

    /* Keep `t`: folding the comparison into the `if` below shrinks
       the function to 0x160 bytes against the ROM's 0x16c
       (notes/experiments/jump3d-2711-87d0-int-temp.md). */
    int t;

    if (func_020179b4(&data_ov006_02140450, &self->mModelAnim, 1) == 0)
        return 0;

    data_ov006_02140430 = (BCA_File *)Animation::LoadFile(data_ov006_02140460);
    data_ov006_0214040c = (BCA_File *)Animation::LoadFile(data_ov006_02140468);
    data_ov006_0214041c = (BCA_File *)Animation::LoadFile(data_ov006_02140458);
    data_ov006_02140424 = (BCA_File *)Animation::LoadFile(data_ov006_02140438);
    data_ov006_02140408 = (BCA_File *)Animation::LoadFile(data_ov006_02140440);
    data_ov006_0214042c = (BCA_File *)Animation::LoadFile(data_ov006_02140448);

    self->mAnimIdx = 0;
    if (data_ov006_02141a40 != 0)
        func_ov006_020bfec0(data_ov006_02141a40, &self->mPos, &self->mScreenX);

    t = data_0209f5c0->actorID == 0x175;
    if (t != 0) {
        func_02016a14(&self->mModelAnim, 0x7fff);
        func_02016a04(&self->mModelAnim, 0x210);
    }
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &self->mModelAnim, data_ov006_02140430, 0x40000000, kAnimSpeed, 0);

    func_ov006_020c8658(raw);
    return 1;
}
}


// @symbol _ZN16dMgJump3DMario_cD1Ev
dMgJump3DMario_c::~dMgJump3DMario_c()
{
    data_ov006_02140450.Release();
    data_ov006_02140460.Release();
    data_ov006_02140468.Release();
    data_ov006_02140458.Release();
    data_ov006_02140438.Release();
    data_ov006_02140440.Release();
    data_ov006_02140448.Release();
    data_ov006_02140430 = 0;
    data_ov006_0214040c = 0;
    data_ov006_02140424 = 0;
    data_ov006_02140408 = 0;
    data_ov006_0214042c = 0;
}


// @symbol _ZN16dMgJump3DMario_cC1Ev
/* Empty: the base constructor, the vptr store and the ModelAnim member
 * construction are all generated. */
dMgJump3DMario_c::dMgJump3DMario_c()
{
}
