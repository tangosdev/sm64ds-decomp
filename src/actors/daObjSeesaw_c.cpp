//cpp
/**
 * d_a_obj_seesaw.cpp
 * Object - Seesaw platforms
 *
 * SEESAW, BOMB_SEESAW, KM1_SEESAW, KM2_YOKOSEESAW, KM3_SEESAW,
 * KM3_YOKOSEESAW and RC_SEESAW share this body. A ground pound
 * pitches mAngleX toward the side that was hit; Behavior springs it
 * back to level. mVariant, from actorID, selects the model, the
 * collision and the CLPS. Those three symbols are one record, stride
 * 0xc, so each is indexed from its own base.
 *
 * decl_common.h stays first. It pulls common.h's flat Matrix4x3
 * (s32 m[12]). daObjSeesaw_c.h includes dBgW_KcMbg.h, which includes
 * math/Matrix.h, and that nested spelling changes UpdateClsnPosAndRot.
 *
 * #pragma defer_codegen off emits the out-of-line destructor as D1
 * then D0. The cartridge has no D2. The g_profile_* rows stay in
 * their own files.
 *
 * deslop leftovers:
 * - OnGroundPounded: `&mPosX` / `&other.mPosX` with no saved hitter
 *   pointer grew the function 0xf4 -> 0xf8 (dropped r8, added
 *   `sub sp, #4`). The `b` / `b + 0x5c` locals match.
 * - Behavior: `if ((mFlags & 8) != 0)` dropped movne/moveq/cmp and
 *   the this argument of func_ov095_021358cc. Size 0x124 -> 0x114.
 *   `(int)((mFlags & 8) != 0)` matches.
 * - InitResources: mMeshCollider.SetFile(Fix12<int> by value) grew
 *   the function 0x168 -> 0x174 and shifted the func_020393d4 call.
 *   The extern "C" int/pointer call matches.
 * - func_ov095_02135cdc / func_ov095_02135e90 keep their placeholder
 *   names: the veneer is stored into the mesh collider as a raw
 *   address, so it cannot be a member, and the ROM has no name for
 *   the tilt body it forwards to.
 */

#pragma defer_codegen off

#include "decl_common.h"
#include "daObjSeesaw_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "Sound.h"
#include "Player.h"

/* Profile actor ids (symbols/profile_reconstruction_registry.json).
 * param1 on a PLAYER (0xbf) is the character; the low two bits index it. */
enum {
    ACTOR_SEESAW = 0x1c,
    ACTOR_BOMB_SEESAW = 0x27,
    ACTOR_RC_SEESAW = 0x80,
    ACTOR_KM1_SEESAW = 0x85,
    ACTOR_KM2_YOKOSEESAW = 0x8f,
    ACTOR_KM3_SEESAW = 0x95,
    ACTOR_KM3_YOKOSEESAW = 0x96,
    ACTOR_PLAYER = 0xbf
};

/* dActor_c::mFlags bit written by the framework when the actor is off screen. */
enum { kFlagOffScreen = 8 };

/* ±45 degrees in the angle units (full circle 0x10000). */
enum { kMaxPitch = 0x2000 };

/* Fix12 2.0. Mega ground pounds use this instead of the character scale. */
enum { kMegaPoundScale = 0x2000 };

/* PlayLong bank-3 id while |mAngleXSpeed| > 0xa. */
enum { kTiltSound = 0x8b };

/* The three symbols are the first record's model, collision and CLPS.
 * Variant i lives at symbol + i * 0xc (one 3-word record). */
enum { kFileStride = 0xc };

/* File-scope resource handles: the ctor/dtor are the ROM's SharedFilePtr
 * veneer pairs (func_02017acc / func_02017ab4 for the model, func_02017b4c /
 * SharedFilePtr_Destruct_Clsn for the collision), spelled through
 * declared-only subclasses so the static initializer names the real entry
 * points. */
struct SeesawModelFilePtr : SharedFilePtr {
    u32 words[2];
    SeesawModelFilePtr(u32 fileID);
    ~SeesawModelFilePtr();
};
struct SeesawCollisionFilePtr : SharedFilePtr {
    u32 words[2];
    SeesawCollisionFilePtr(u32 fileID);
    ~SeesawCollisionFilePtr();
};

extern "C" {
extern int data_ov095_02136f58[];
extern s16 data_02082214[];
int Vec3_Dist(void *a, void *b);
s16 Vec3_HorzAngle(void *a, void *b);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(dBgActor_c *self, int radius, int yOffset);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *thiz, void *file, const Matrix4x3 *mat, int scale, short angY, void *clps);
void func_020393d4(void *clsn, void *callback);
void func_020393c4(void *clsn, void *callback);
void func_ov095_02135e90(void *collider, daObjSeesaw_c *self, dActor_c *other);
}

/* D1 stores this vtable, then dBgActor_c's (inlined), then destroys
 * mMeshCollider and mModel. D0 is that plus the inline operator delete.
 * This class adds no member with a destructor. */
// @symbol _ZN13daObjSeesaw_cD1Ev
// @symbol _ZN13daObjSeesaw_cD0Ev
daObjSeesaw_c::~daObjSeesaw_c()
{
}

/* Pound scale for the actor that hit the seesaw. data_ov095_02136f58 is
 * four fix12 weights indexed by param1 & 3 (the character on a player:
 * 0x1000, 0x800, 0x1800, 0x1000). A non-player keeps that weight. Mega
 * replaces it with kMegaPoundScale. */
// @symbol func_ov095_0213579c
extern "C" int func_ov095_0213579c(void *self, void *otherRaw)
{
    dActor_c *other = (dActor_c *)otherRaw;
    int scale = data_ov095_02136f58[(int)other->param1 & 3];
    unsigned isPlayer = (unsigned)(other->actorID == ACTOR_PLAYER);
    if (isPlayer == 0)
        return scale;
    if (((Player *)other)->mIsMega != 0)
        scale = kMegaPoundScale;
    return scale;
}

#pragma push
#pragma opt_propagation off
// @symbol _ZN13daObjSeesaw_c15OnGroundPoundedER8dActor_c
void daObjSeesaw_c::OnGroundPounded(dActor_c &other)
{
    /* b and bPos stay separate locals: mwcc keeps the hitter in r8 and
     * its position in r4. Naming both &other.mPosX recomputes the add
     * and drops r8, and the function grows by the stack adjust. */
    char *a = (char *)this;
    char *b = (char *)&other;
    char *bPos = b + 0x5c;
    int dist = Vec3_Dist(a + 0x5c, bPos);
    s16 angle = Vec3_HorzAngle(a + 0x5c, bPos);
    int n, d, idx, prod, cur, v;
    s16 *pitch;

    mPoundedThisFrame = 1;
    n = func_ov095_0213579c(a, b);
    d = AngleDiff(angle, mAngleY);
    /* data_02082214 is interleaved cos, sin. (angle >> 4) * 2 + 1 is sine. */
    idx = ((u16)(s16)d >> 4) << 1;

    prod = (int)(((s64)dist * n + 0x800) >> 12);
    idx = idx + 1;

    pitch = &mAngleX;
    cur = *pitch;
    v = (int)(((s64)prod * data_02082214[idx] + 0x800) >> 12);
    /* Signed divide by 4096, then narrow onto the s16 angle. */
    v = (v + ((unsigned)(v >> 11) >> 20)) << 4;
    *pitch = (s16)(cur + (v >> 16));
    if (mAngleX > kMaxPitch)
        mAngleX = kMaxPitch;
    if (mAngleX < -kMaxPitch)
        mAngleX = -kMaxPitch;
}
#pragma pop

/* Spring *angle toward target by *speed. Behavior calls this with the
 * seesaw, &mAngleX, &mAngleXSpeed, target 0, thresh 6, accel 3, mult 3.
 * The first argument is unused; the caller still passes it. */
#pragma push
#pragma opt_common_subs off
// @symbol func_ov095_021358cc
extern "C" int func_ov095_021358cc(int self, short *angle, short *speed, int target, short thresh, int accel, short mult)
{
    short old = angle[0];
    angle[0] = old + speed[0];
    short now = angle[0];
    if (now == target
        || ((now - target) * (old - target) < 0
            && speed[0] > -thresh && speed[0] < thresh)) {
        angle[0] = target;
        speed[0] = 0;
        return 1;
    }
    if (now >= target)
        accel = (short)-accel;
    if ((short)speed[0] * (short)accel < 0)
        accel = (short)accel * (short)mult;
    speed[0] = speed[0] + accel;
    return 0;
}
#pragma pop

/* Model matrix from the actor angles, translation = position >> 3
 * (world fix12 to the model's units). mModel.mat4x3 is at this+0xf0. */
// @symbol func_ov095_0213597c
extern "C" void func_ov095_0213597c(char *raw)
{
    daObjSeesaw_c *seesaw = (daObjSeesaw_c *)raw;
    Matrix4x3_FromRotationXYZExt(&seesaw->mModel.mat4x3, seesaw->mAngleX, seesaw->mAngleY, seesaw->mAngleZ);
    seesaw->mModel.mat4x3.m[9] = seesaw->mPosX >> 3;
    seesaw->mModel.mat4x3.m[10] = seesaw->mPosY >> 3;
    seesaw->mModel.mat4x3.m[11] = seesaw->mPosZ >> 3;
}

// @symbol _ZN13daObjSeesaw_c16CleanupResourcesEv
int daObjSeesaw_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    (*(SharedFilePtr **)(data_ov095_021374a0 + mVariant * kFileStride))->Release();
    (*(SharedFilePtr **)(data_ov095_021374a4 + mVariant * kFileStride))->Release();
    return 1;
}

// @symbol _ZN13daObjSeesaw_c6RenderEv
int daObjSeesaw_c::Render()
{
    mModel.Render(0);
    return 1;
}

// @symbol _ZN13daObjSeesaw_c8BehaviorEv
int daObjSeesaw_c::Behavior()
{
    /* The 0/1 temporary is the ROM's movne/moveq/cmp. A direct test
     * of the bit is three instructions shorter. */
    int offScreen = (int)((mFlags & kFlagOffScreen) != 0);
    if (offScreen != 0) {
        if (mMeshCollider.IsEnabled())
            mMeshCollider.Disable();
        return 1;
    }
    if (mPoundedThisFrame == 0) {
        func_ov095_021358cc((int)this, &mAngleX, &mAngleXSpeed, 0, 6, 3, 3);
    }
    {
        int s = mAngleXSpeed;
        if (s < 0)
            s = (short)-s;
        if (s > 0xa) {
            mTiltSound = Sound::PlayLong(mTiltSound, 3, kTiltSound, *(Vector3 *)&mCamSpacePosX, 0);
        }
    }
    if (mAngleX > kMaxPitch)
        mAngleX = kMaxPitch;
    if (mAngleX < -kMaxPitch)
        mAngleX = -kMaxPitch;
    func_ov095_0213597c((char *)this);
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0))
        UpdateClsnPosAndRot();
    mPoundedThisFrame = 0;
    return 1;
}

// @symbol _ZN13daObjSeesaw_c13InitResourcesEv
int daObjSeesaw_c::InitResources()
{
    unsigned char idx;
    int f;
    switch (actorID) {
        case ACTOR_SEESAW: mVariant = 0; break;
        case ACTOR_BOMB_SEESAW: mVariant = 1; break;
        case ACTOR_KM1_SEESAW: mVariant = 2; break;
        case ACTOR_KM2_YOKOSEESAW: mVariant = 3; break;
        case ACTOR_KM3_SEESAW: mVariant = 4; break;
        case ACTOR_KM3_YOKOSEESAW: mVariant = 5; break;
        case ACTOR_RC_SEESAW: mVariant = 6; break;
    }
    idx = mVariant;
    f = (int)Model::LoadFile(**(SharedFilePtr **)(data_ov095_021374a0 + idx * kFileStride));
    mModel.SetFile((BMD_File *)f, 1, -1);
    func_ov095_0213597c((char *)this);
    UpdateClsnPosAndRot();
    {
        unsigned char i = mVariant;
        f = (int)dBgW_Kc::LoadFile(**(SharedFilePtr **)(data_ov095_021374a4 + i * kFileStride));
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            &mMeshCollider, (void *)f, &mClsnMat, 0x1000, mAngleY,
            *(void **)(data_ov095_021374a8 + i * kFileStride));
    }
    /* func_020393d4 stores dBgW+0x18 (beforeClsnCallback). func_020393c4
     * stores dBgW+0x1c, the stood-on callback: func_ov095_02135e90, the
     * veneer into the tilt helper below. Both are calls in the ROM; the
     * bodies are a single word store. */
    func_020393d4(&mMeshCollider, (int *)&dBgW::UpdatePosWithTransform);
    func_020393c4(&mMeshCollider, func_ov095_02135e90);
    return 1;
}

/* Stood-on tilt. The mesh collider calls this while another actor rests
 * on the seesaw: mAngleX keeps integrating mAngleXSpeed, and the stander's
 * weight (func_ov095_0213579c) and off-centre distance drive mAngleXSpeed
 * toward a clamp. Unlike OnGroundPounded it does not kick the pitch
 * directly; it accelerates the tilt speed, hard when pushing into the
 * current lean, soft when pushing out of it. */
// @symbol func_ov095_02135cdc
extern "C" void func_ov095_02135cdc(daObjSeesaw_c *self, dActor_c *other)
{
    Vector3 *op = (Vector3 *)&other->mPosX;
    int dist = Vec3_Dist((Vector3 *)&self->mPosX, op);
    int ang = Vec3_HorzAngle((Vector3 *)&self->mPosX, op);
    self->mPoundedThisFrame = 1;
    int r = func_ov095_0213579c(self, other);

    self->mAngleX += self->mAngleXSpeed;
    int x = (int)(((s64)dist * r + 0x800) >> 12);
    s16 d = (s16)AngleDiff(ang, self->mAngleY);
    int y = (int)(((s64)x * data_02082214[((u16)d >> 4) * 2 + 1] + 0x800) >> 12);

    int acc;
    if (self->mAngleXSpeed * x < 0)
        acc = (int)(((s64)y * 0xa3LL + 0x800) >> 12);
    else
        acc = (int)(((s64)y * 0x51LL + 0x800) >> 12);

    s16 dv = acc / 4096;
    self->mAngleXSpeed += dv;
    s16 lim = (int)(((s64)r * 0x32000LL + 0x800) >> 12) / 4096;
    if (self->mAngleXSpeed > lim)
        self->mAngleXSpeed = lim;
    if (self->mAngleXSpeed < -lim)
        self->mAngleXSpeed = -lim;
}

/* The collider callback slot gets three arguments; the tilt helper
 * wants the last two. long_calls keeps the pooled absolute tail call. */
#pragma push
#pragma long_calls on
// @symbol func_ov095_02135e90
extern "C" void func_ov095_02135e90(void *collider, daObjSeesaw_c *self, dActor_c *other)
{
    func_ov095_02135cdc(self, other);
}
#pragma pop

// @symbol daObjSeesaw_c_classInit_RC_SEESAW
extern "C" daObjSeesaw_c *daObjSeesaw_c_classInit_RC_SEESAW()
{
    return new daObjSeesaw_c();
}

// @symbol daObjSeesaw_c_classInit_KM3_YOKOSEESAW
extern "C" daObjSeesaw_c *daObjSeesaw_c_classInit_KM3_YOKOSEESAW()
{
    return new daObjSeesaw_c();
}

// @symbol daObjSeesaw_c_classInit_KM3_SEESAW
extern "C" daObjSeesaw_c *daObjSeesaw_c_classInit_KM3_SEESAW()
{
    return new daObjSeesaw_c();
}

// @symbol daObjSeesaw_c_classInit_KM2_YOKOSEESAW
extern "C" daObjSeesaw_c *daObjSeesaw_c_classInit_KM2_YOKOSEESAW()
{
    return new daObjSeesaw_c();
}

// @symbol daObjSeesaw_c_classInit_KM1_SEESAW
extern "C" daObjSeesaw_c *daObjSeesaw_c_classInit_KM1_SEESAW()
{
    return new daObjSeesaw_c();
}

// @symbol daObjSeesaw_c_classInit_BOMB_SEESAW
extern "C" daObjSeesaw_c *daObjSeesaw_c_classInit_BOMB_SEESAW()
{
    return new daObjSeesaw_c();
}

// @symbol daObjSeesaw_c_classInit_SEESAW
extern "C" daObjSeesaw_c *daObjSeesaw_c_classInit_SEESAW()
{
    return new daObjSeesaw_c();
}

/* Seven variants, each a model/collision handle pair; the pointer table at
 * data_ov095_021374a0 indexes them through kFileStride. */
SeesawModelFilePtr data_ov095_02137790(1162);
SeesawModelFilePtr data_ov095_02137780(1445);
SeesawModelFilePtr data_ov095_02137798(1623);
SeesawModelFilePtr data_ov095_021377b0(1645);
SeesawModelFilePtr data_ov095_02137788(1669);
SeesawModelFilePtr data_ov095_021377d8(1671);
SeesawModelFilePtr data_ov095_021377b8(1709);
SeesawCollisionFilePtr data_ov095_021377c8(1163);
SeesawCollisionFilePtr data_ov095_021377a0(1446);
SeesawCollisionFilePtr data_ov095_021377e0(1624);
SeesawCollisionFilePtr data_ov095_021377d0(1646);
SeesawCollisionFilePtr data_ov095_021377e8(1670);
SeesawCollisionFilePtr data_ov095_021377a8(1672);
SeesawCollisionFilePtr data_ov095_021377c0(1710);
