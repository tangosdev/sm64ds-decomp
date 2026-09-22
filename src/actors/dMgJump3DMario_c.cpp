//cpp
/* dMgJump3DMario_c -- one of the three Marios the two jump minigames
 * (Bounce and Pounce, MG_JUMP, and its sequel MG_JUMP2) construct as
 * `dMgJump3DMario_c mPlayers[3]`, ov006.
 *
 * 28 functions, .text 0x020c762c..0x020c8a30. The object is a PMF-driven
 * state machine: mState at +0x3c, installed from fifteen 8-byte ROM records
 * at 0x0213b020..0x0213b098. Every update integrates mVel into mPos, runs
 * the current State* handler, and re-projects the screen position. Tap
 * (stylus) near Mario and he bounces; miss and he damps out and respawns.
 *
 * Source order is ROM-ascending with `#pragma defer_codegen off` below; the
 * default (deferred) codegen would emit the reverse. Do not reorder. The
 * destructor is not virtual, so the key function is the first declared
 * virtual (Unk_020c76d8, slot 0) and this TU emits the vtable.
 *
 * common.h FIRST: the mat4x3 copy in func_ov006_020c76e0 wants the flat
 * s32 m[12] spelling; math/Matrix.h's nested {r, t} scalarizes it. Pinned in
 * notes/experiments/jump3d-2711-common-first-matrix.md.
 *
 * decl_common.h is NOT included: it declares only two of these members but
 * four of the data symbols this TU reads collide with it (02141a44,
 * 02141a40, 02140428, 0214042c). No header under include/ pulls it in.
 *
 * Remaining partial scope:
 * - ModelAnim::SetAnim retains a scalar-speed ABI bridge. The native
 *   Fix12<int> experiment enlarged EnterDamp from 0x88 to 0x94 bytes; see
 *   notes/experiments/jump3d-2711-setanim-member-form.md.
 * - Particle::System::NewSimple and the vector, projection, material and
 *   minigame helpers still use local ABI declarations. Their shared type
 *   recovery is separate work; data_ov006_* names remain address-derived.
 * - Eight func_ov006_* entry points still bridge twelve calls from seven
 *   unpromoted C shards. Issue #2722 tracks that migration and the unresolved
 *   wider TU boundary; this follow-up does not convert those entry points.
 * - The int* matrix zeroing, StateHold flag reuse, StateMove volatile store,
 *   and loader comparison temporary retain the measured source forms cited
 *   at their sites. These experiments constrain those substitutions, not
 *   every possible spelling.
 * - Jump3DVec scratch and members remain plain aggregates: the Vector3
 *   experiment added an unlicensed destructor despite matching function
 *   bytes. See EnterHold and include/dMgJump3DMario_c.h.
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

/* Native helper declarations match their production definitions. */
int ApproachLinear(int &value, int target, int step);
int ApproachLinear2(short &value, short target, short step);
void UpdateAngle(short &angle, short target, int divisor, short maxStep);

/* C-linkage ABI bridges and data shared with the remaining source shards. */
extern "C" {

/* This TU's own member, forward-declared: its callers sit above it in ROM
 * order (StateFallOut, func_ov006_020c8084, StateRiseOut). */
void func_ov006_020c8658(void *c);

/* Retain scalar speed for the measured Fix12<int> member-call constraint.
 * The local bridge follows the C definition's u16 start frame; ModelAnim.h
 * still declares the native interface's start frame as u32. All calls here
 * pass zero. See notes/experiments/jump3d-2711-setanim-member-form.md. */
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(ModelAnim *self, BCA_File *file, int flags, int speed, u16 startFrame);
void *_ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
void Sound_PlayBank1Panned(int a, int b, int c);
void Vec3_Sub(Jump3DVec *out, Jump3DVec *a, Jump3DVec *b);
void AddVec3(Jump3DVec *a, Jump3DVec *b, Jump3DVec *c);
int  NormalizeVec3IfNonZero(Jump3DVec *v);
int  RandomIntInternal(int *seed);
void func_02012718(int a, int b);
int  func_02053200(int x);
/* Both material wrappers forward r1 to the per-material update. Their
 * one-argument C definitions omit that forwarded value; preserve it here. */
void func_02016a14(void *self, int a);
void func_02016a04(void *self, int a);
int  func_020179b4(SharedFilePtr *f, ModelBase *m, int extra);
void func_ov006_020bfec0(void *cam, Jump3DVec *pos, short *out);
void func_ov006_020e6e3c(int a, int b);
void func_ov006_020c8c78(int a, int b);
void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationY(void *m, short angY);

/* The mState records this TU installs or compares (0213b040's consumer sits
 * outside the run, so it is not declared here). */
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

/* The six anim files func_ov006_020c87d0 loads; the dtor clears five of them
 * (0214041c survives -- that is the ROM's shape, not an omission). */
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


/* [0] 0x020c762c -- vtable slot 2 */
// @symbol _ZN16dMgJump3DMario_c12Unk_020c762cEv
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


/* [1] 0x020c76d0 -- vtable slot 1 */
// @symbol _ZN16dMgJump3DMario_c12Unk_020c76d0Ev
void *dMgJump3DMario_c::Unk_020c76d0()
{
    return &mVel;
}


/* [2] 0x020c76d8 -- vtable slot 0, KEY FUNCTION */
// @symbol _ZN16dMgJump3DMario_c12Unk_020c76d8Ev
void *dMgJump3DMario_c::Unk_020c76d8()
{
    return &mPos;
}


/* [3] 0x020c76e0 -- rebuilds the model matrix from mPos/mAngleY */
extern "C" {
// @symbol func_ov006_020c76e0
void func_ov006_020c76e0(char *c)
{
    dMgJump3DMario_c *o = (dMgJump3DMario_c *)c;

    Matrix4x3_FromTranslation(&data_020a0e68, o->mPos.x, o->mPos.y, o->mPos.z);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, o->mAngleY);
    o->mModelAnim.mat4x3 = data_020a0e68;
}
}


/* [4] 0x020c7734 -- OAM shadow sprite plus the model render */
extern "C" {
// @symbol func_ov006_020c7734
void func_ov006_020c7734(char *c)
{
    dMgJump3DMario_c *o = (dMgJump3DMario_c *)c;
    short screenPos[2];
    int scaleX;
    int scaleY;
    int shadowPhase;
    int shadowSin;
    Mtx shadowMatrix;

    if (o->mVisible == 0)
        return;

    if (data_ov006_02140400 != 0) {
        func_ov006_020bfec0(data_ov006_02141a44, &o->mPos, screenPos);

        shadowPhase = data_ov006_02140404;
        shadowSin = data_02082214[(shadowPhase >> 4) * 2];
        screenPos[1] = screenPos[1] - (((shadowSin << 2) + 0x30000) >> 12);
        scaleX = func_02053200((shadowSin >> 2) + 0x1000);

        shadowPhase = data_ov006_02140404;
        scaleY = -func_02053200((data_02082214[(shadowPhase >> 4) * 2 + 1] >> 2) + 0x1000);

        /* Retain int* zeroing: the named-member experiment changed eleven
           words at the same 0x12c size, rather than adding eleven words.
           See notes/experiments/jump3d-2711-mtx-intptr-zeroing.md. */
        int *matrixWords = (int *)&shadowMatrix;
        matrixWords[0] = 0; matrixWords[1] = 0; matrixWords[2] = 0; matrixWords[3] = 0;
        shadowMatrix.d = scaleY;
        shadowMatrix.a = scaleX;
        OAM::Render(false, data_ov006_02134d1c, screenPos[0], screenPos[1], -1, -1, (Matrix2x2 *)&shadowMatrix);
    }

    o->mModelAnim.Render(&data_ov006_0212ddd0);
}
}


/* [5] 0x020c7860 -- the per-frame update: gravity, integrate, dispatch mState */
extern "C" {
// @symbol func_ov006_020c7860
void func_ov006_020c7860(char *c)
{
    dMgJump3DMario_c *o = (dMgJump3DMario_c *)c;

    ApproachLinear(o->mVel.y, data_ov006_0213b010, data_ov006_0213b018);
    AddVec3(&o->mPos, &o->mVel, &o->mPos);
    (o->*o->mState)();
    func_ov006_020bfec0(data_ov006_02141a40, &o->mPos, &o->mScreenX);
    func_ov006_020c76e0(c);
    o->mModelAnim.Animation::Advance();
}
}


/* [6] 0x020c78ec */
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


/* [7] 0x020c79a8 */
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


/* [8] 0x020c7a30 */
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
        /* Retain flag reuse: reading touchY directly changed five words
           at the same 0x174 size in
           notes/experiments/jump3d-2711-statehold-flag-reuse.md. */
        flag = data_020a0deb[idx][0];
        int touchY = flag;
        int hitCenterY = mScreenY - 0x20;
        int distanceX = mScreenX - data_020a0dea[idx][0];
        if (distanceX < 0) {
            distanceX = -distanceX;
        }
        if (distanceX < 0x18) {
            int distanceY = hitCenterY - touchY;
            if (distanceY < 0) {
                distanceY = -distanceY;
            }
            if (distanceY < 0x26) {
                ApproachLinear2(mTimer, 0, 8);
            }
        }
    }
    {
        int clampedX = mPos.x;
        if (clampedX < -kWallX) {
            clampedX = -kWallX;
        } else if (clampedX > kWallX) {
            clampedX = kWallX;
        }
        mPos.x = clampedX;
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


/* [9] 0x020c7ba4 */
// @symbol _ZN16dMgJump3DMario_c9EnterHoldEv
void dMgJump3DMario_c::EnterHold()
{
    /* Plain Jump3DVec, not types.h's Vector3. MEASURED and pinned in
       notes/experiments/jump3d-2711-scratch-vector3.md: the Vector3 spelling
       leaves this member byte-identical, so a per-function compare cannot
       see the wall at all -- but the TU then defines `_ZN7Vector3D1Ev`, no
       compiler_only_output row in
       config/tu_manifest.d/ov006/dMgJump3DMario_c.json licenses that symbol,
       and packaging refuses the emission. Retained substitution, accepted as
       partial scope alongside issue #2722. */
    Jump3DVec direction;

    Vec3_Sub(&direction, &mPos, &mAnchor);
    if (NormalizeVec3IfNonZero(&direction) != 0) {
        mVel.x = direction.x;
        mVel.y = direction.y;
        mVel.z = direction.z;
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


/* [10] 0x020c7c68 */
// @symbol _ZN16dMgJump3DMario_c9StateMoveEv
void dMgJump3DMario_c::StateMove()
{
    u16 command = mCommand;

    if (command == 1) {
        mCommand = 0;
        mVel.y = data_ov006_0213b008;
        if (mAnimIdx != 0)
            mAnimIdx = 0;
        else
            mAnimIdx = 1;
        EnterHit();
        return;
    }
    if (command == 2) {
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
            s16 screenY = mScreenY;
            int touchX = (int)data_020a0dea[idx][0];
            int touchY = (int)data_020a0deb[idx][0];
            int deltaX = (int)mScreenX - touchX;
            int hitCenterY = (int)screenY - 0x20;
            int distance = deltaX < 0 ? -deltaX : deltaX;

            if (distance < 0x18) {
                distance = hitCenterY - touchY;
                if (distance < 0) distance = -distance;
                if (distance < 0x26 && (int)screenY < kScreenYMax) {
                    int particlePos[3];
                    int *bounceSpeed = &data_ov006_0213b00c;
                    int particleX = (touchX - 0x80) << 12;
                    int particleY = (-touchY) << 12;
                    particlePos[0] = particleX;
                    particlePos[1] = particleY;
                    /* Retain the volatile store: a plain particlePos[2] = 0 shrank
                       StateMove from 0x3c4 to 0x3a8 bytes. The experiment
                       did not measure individual relocation differences.
                       See notes/experiments/jump3d-2711-statemove-volatile-store.md. */
                    *(volatile int *)&particlePos[2] = 0;
                    mVel.y = *bounceSpeed;
                    mVel.x = data_ov006_0213b01c * deltaX;
                    if (mAnimIdx != 0)
                        mAnimIdx = 0;
                    else
                        mAnimIdx = 1;
                    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, *data_ov006_0213b098[mAnimIdx], 0x40000000, kAnimSpeed, 0);
                    mModelAnim.Animation::currFrame = 0;
                    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xed, mPos.x << 3, mPos.y << 3, mPos.z << 3);
                    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xee, particlePos[0] << 3, particlePos[1] << 3, particlePos[2] << 3);
                    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xef, particlePos[0] << 3, particlePos[1] << 3, particlePos[2] << 3);
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


/* [11] 0x020c802c */
// @symbol _ZN16dMgJump3DMario_c9EnterMoveEv
void dMgJump3DMario_c::EnterMove()
{
    mState = data_ov006_0213b020;
}


/* [12] 0x020c8048 */
// @symbol _ZN16dMgJump3DMario_c12StateFallOutEv
void dMgJump3DMario_c::StateFallOut()
{
    if (mPos.y >= -0x120000)
        return;
    mVel.y = 0;
    func_ov006_020c8658(this);
}


/* [13] 0x020c8084 */
extern "C" {
// @symbol func_ov006_020c8084
void func_ov006_020c8084(char *c)
{
    dMgJump3DMario_c *o = (dMgJump3DMario_c *)c;

    if (o->mState == data_ov006_0213b088) {
        o->mVel.y = 0;
        func_ov006_020c8658(c);
    } else {
        o->mVel.x = 0;
        o->mVel.y = 0x2000;
        Sound::PlayBank2_2D(0x1c9);
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&o->mModelAnim, data_ov006_0214042c, 0, kAnimSpeed, 0);
        o->mModelAnim.Animation::currFrame = 0;
        o->mState = data_ov006_0213b090;
    }
}
}


/* [14] 0x020c814c */
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


/* [15] 0x020c81e0 */
extern "C" {
// @symbol func_ov006_020c81e0
void func_ov006_020c81e0(char *c)
{
    dMgJump3DMario_c *o = (dMgJump3DMario_c *)c;

    o->mVel.x = 0;
    o->mVel.y = data_ov006_0213b00c;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&o->mModelAnim, data_ov006_0214041c, 0x40000000, kAnimSpeed, 0);
    o->mModelAnim.Animation::currFrame = 0;
    Sound::PlayBank2_2D(0x10f);
    func_02012718(0x1b5, o->mScreenX << 12);
    o->mState = data_ov006_0213b080;
}
}


/* [16] 0x020c8270 */
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


/* [17] 0x020c833c */
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


/* [18] 0x020c85a0 */
// @symbol _ZN16dMgJump3DMario_c11EnterBounceEv
void dMgJump3DMario_c::EnterBounce()
{
    mState = data_ov006_0213b060;
}


/* [19] 0x020c85bc */
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


/* [20] 0x020c862c */
extern "C" {
// @symbol func_ov006_020c862c
void func_ov006_020c862c(int *c, int v)
{
    dMgJump3DMario_c *o = (dMgJump3DMario_c *)c;

    o->mTimer = (s16)v;
    o->mState = data_ov006_0213b050;
}
}


/* [21] 0x020c864c */
// @symbol _ZN16dMgJump3DMario_c9StateIdleEv
void dMgJump3DMario_c::StateIdle()
{
    mPos.y = 0;
}


/* [22] 0x020c8658 */
extern "C" {
// @symbol func_ov006_020c8658
void func_ov006_020c8658(void *c)
{
    dMgJump3DMario_c *o = (dMgJump3DMario_c *)c;

    o->mVisible = 0;
    o->mPos.y = 0;
    o->mState = data_ov006_0213b048;
}
}


/* [23] 0x020c8680 */
// @symbol _ZN16dMgJump3DMario_c12StateRespawnEv
void dMgJump3DMario_c::StateRespawn()
{
    unsigned int randomSample;
    int horizontalSpeedScale;

    mTimer -= 1;
    if (mTimer == 0) {
        mPos.y = 0x100000;
        randomSample = ((unsigned int)RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 0x13;
        mPos.x = ((int)randomSample - 0x800) * 0xc0;
        mVel.y = 0;
        horizontalSpeedScale = data_ov006_0213b01c;
        randomSample = ((unsigned int)RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 0x13;
        mVel.x = (int)(((s64)(((int)randomSample - 0x800) << 1) * horizontalSpeedScale + 0x800) >> 12);
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, data_ov006_02140424, 0x40000000, kAnimSpeed, 0);
        EnterMove();
        return;
    }
    mPos.y = 0x100000;
    mVel.y = 0;
}


/* [24] 0x020c8768 */
// @symbol _ZN16dMgJump3DMario_c12EnterRespawnEv
void dMgJump3DMario_c::EnterRespawn()
{
    mTimer = 0x3c;
    mPos.y = 0x100000;
    mVel.y = 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, data_ov006_02140424, 0x40000000, kAnimSpeed, 0);
    mState = data_ov006_0213b038;
}


/* [25] 0x020c87d0 -- loads the model + six anims; the one-minigame setup */
extern "C" {
// @symbol func_ov006_020c87d0
int func_ov006_020c87d0(char *c)
{
    dMgJump3DMario_c *o = (dMgJump3DMario_c *)c;

    /* Keep the separate int comparison result: folding it into the if
       shrank the loader from 0x16c to 0x160 bytes in
       notes/experiments/jump3d-2711-87d0-int-temp.md. */
    int applyMaterialOverrides;

    if (func_020179b4(&data_ov006_02140450, &o->mModelAnim, 1) == 0)
        return 0;

    data_ov006_02140430 = (BCA_File *)Animation::LoadFile(data_ov006_02140460);
    data_ov006_0214040c = (BCA_File *)Animation::LoadFile(data_ov006_02140468);
    data_ov006_0214041c = (BCA_File *)Animation::LoadFile(data_ov006_02140458);
    data_ov006_02140424 = (BCA_File *)Animation::LoadFile(data_ov006_02140438);
    data_ov006_02140408 = (BCA_File *)Animation::LoadFile(data_ov006_02140440);
    data_ov006_0214042c = (BCA_File *)Animation::LoadFile(data_ov006_02140448);

    o->mAnimIdx = 0;
    if (data_ov006_02141a40 != 0)
        func_ov006_020bfec0(data_ov006_02141a40, &o->mPos, &o->mScreenX);

    applyMaterialOverrides = data_0209f5c0->actorID == 0x175;
    if (applyMaterialOverrides != 0) {
        func_02016a14(&o->mModelAnim, 0x7fff);
        func_02016a04(&o->mModelAnim, 0x210);
    }
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &o->mModelAnim, data_ov006_02140430, 0x40000000, kAnimSpeed, 0);

    func_ov006_020c8658(c);
    return 1;
}
}


/* [26] 0x020c893c -- D1 */
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


/* [27] 0x020c8a04 -- C1. Empty: the base ctor, the vptr store and the
 * ModelAnim member construction are all synthesized. */
// @symbol _ZN16dMgJump3DMario_cC1Ev
dMgJump3DMario_c::dMgJump3DMario_c()
{
}
