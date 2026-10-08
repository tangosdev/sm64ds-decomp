//cpp
/* daOnms_c -- the rolling crush box (profile ONIMASU), ov092.
 *
 * defer_codegen off emits this file in source order, which is the ROM order.
 * The out-of-line destructor is the key function (D1 then D0). daOnms_c.h is
 * included first so common.h's flat Matrix4x3 stands; the matrix copies below
 * are twelve words, not .r/.t. The factory daOnms_c_classInit
 * (0x02132018..0x02132074, historical alias ToxBox_Spawn) now appends after
 * InitResources, at the end of source order: fBase_c::operator new(size_t)
 * forwards `return new daOnms_c();` to the same _ZN7fBase_cnwEj allocator the
 * loose factory called by hand, and daOnms_c has no user-declared
 * constructor, so the inherited dBgActor_c ctor plus the vtable store plus
 * the member subobjects in field order (dBgCh_Actr, dCcAcPos_c, PathPtr)
 * come from the implicit default constructor with zero mangled calls.
 * func_ov092_021313b0 and func_ov092_02131a88 stay
 * extern "C": include/decl_common.h declares them under those names.
 * func_ov092_02131578 is the wait state, a daOnms_c member: the .data
 * member-pointer record at 0x02132280 reaches it by address, so the free
 * spelling cannot supply that descriptor.
 *
 * deslop leftovers:
 * - StateBounce: `return 0` for the tested zero, size 0x1a0 -> 0x1a8.
 *   Building the landing dust without the dead tmp copy, size 0x1a0 -> 0x190.
 * - StateKnocked: o->mPosX/Y/Z instead of int *p = &o->mPosX, size
 *   0x200 -> 0x1f8. Hoisting &floor->surface.clps.w0, 13 words (size stayed
 *   0x200); each call adds 4. mRadius = 0x78000 instead of func_0203568c
 *   (the +0x18 store), size 0x200 -> 0x1fc. UpdateContinuous() relocates to
 *   _ZN10dBgCh_Actr16UpdateContinuousEv; the ROM calls the veneer at
 *   0x020383fc (size stayed 0x200).
 * - CheckPlayerHit: `if (actorID != 0xbf) return`, size 0xd8 -> 0xcc.
 *   0xbf is PLAYER. The flag form stays.
 * - StateLand: Sound::Play(3, id, pos) instead of func_02012694, size
 *   0x74 -> 0x78. That wrapper is Sound::Play(3, id, pos).
 * - func_ov092_02131a88: UpdateClsnPosAndRot(), size 0x64 -> 0xc. The
 *   body is that function written out; the ROM does not call it.
 * - InitResources: dCcAcPos_c::Init and dBgW_KcMbg::SetFile do not compile
 *   (header Fix12<int>, and this TU's ints are not that type). A direct
 *   beforeClsnCallback store, size 0x278 -> 0x274. func_020393d4 stays;
 *   it writes dBgW + 0x18. dBgCh_Actr::Init stays the mangled free call:
 *   the header method mangles with int, the ROM symbol with Fix12<int>.
 */

#pragma defer_codegen off

#include "daOnms_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "Sound.h"

/* mMoveDir indexes the member-pointer table this file defines below. The
   compiler copies the nine pointer-to-member descriptors into it at
   overlay load. */
enum {
    kDirLand = 0,
    kDirWait = 1,
    kDirRollPosZ = 2,
    kDirRollNegZ = 3,
    kDirRollNegX = 4,
    kDirRollPosX = 5,
    kDirKnocked = 6,
    kDirBounce = 7,
    kDirSink = 8
};

/* param1 & 3. 3 follows the path in param1 bits 8..11; 0..2 index a script. */
enum { kMoveAlongPath = 3 };

/* One member pointer per mMoveDir. */
typedef void (daOnms_c::*daOnms_cState)();
struct daOnms_cStateEntry { daOnms_cState state; };

/* Vec3, not Vector3: Vec3_Asr would emit ~Vector3 if the arg were Vector3. */
typedef struct { int x, y, z; } Vec3;

#define ActorPos(actor) ((Vector3 *)&(actor)->mPosX)
#define CamPos(actor)   ((const Vector3 *)&(actor)->mCamSpacePosX)

/* The retail static initializer constructs these two 8-byte resource handles
 * in source order (model 0x3c9, collision 0x3ca) and lets the C++ runtime
 * register their destructors. The family spellings are reconstructed; the
 * constructor/destructor addresses, file IDs, object widths, BSS order, and
 * registration topology are direct ROM evidence. The intact-TU manifest maps
 * their compiler-generated undefined member imports onto the existing
 * evidence-bounded ROM symbols. */
struct OnmsModelFilePtr : SharedFilePtr {
    u32 words[2];

    OnmsModelFilePtr(u32 fileID);
    ~OnmsModelFilePtr();
};

struct OnmsCollisionFilePtr : SharedFilePtr {
    u32 words[2];

    OnmsCollisionFilePtr(u32 fileID);
    ~OnmsCollisionFilePtr();
};

/* BMD (Model::LoadFile) and KCL (dBgW_Kc::LoadFile). Cleanup releases both. */
extern "C" OnmsModelFilePtr data_ov092_02132540;
extern "C" OnmsCollisionFilePtr data_ov092_02132548;
extern "C" daOnms_cStateEntry data_ov092_02132568[9];

extern "C" {
int func_ov002_020de328(void *player);
void dBgCh_Actr_UpdateContinuous_Veneer(void *self);
void *_ZNK10dBgCh_Actr14GetFloorResultEv(void *self);
void _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(void *v, int f);
/* Sound::Play(3, id, pos). The overlay calls this wrapper, not Sound::Play. */
void func_02012694(unsigned int id, const Vector3 *v);
int func_02037e38(void *p);
int func_02037e84(void *p);
/* Stores mRadius: p[6] = value, the word at dBgCh_Actr + 0x18. */
void func_0203568c(void *p, int v);
void *_ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int a, int b, int cc, int d);
s16 Vec3_HorzAngle(const struct Vector3 *v0, const struct Vector3 *v1);
int Vec3_HorzDist(const Vector3 *a, const Vector3 *b);
s16 data_02082214[];
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(dBgActor_c *self, int range, int offset);
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
    dBgCh_Actr *self, int a1, int a2, int a3, int sp0, int sp1);
void Vec3_Asr(Vec3 *d, Vec3 *s, int sh);
void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationX(void *m, short angX);
void Matrix4x3_ApplyInPlaceToRotationZ(void *m, short angZ);
void Matrix4x3_ApplyInPlaceToRotationY(void *m, short angY);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *file, const Matrix4x3 *mat,
    int scale, short angY, void *clps);
/* Stores beforeClsnCallback: p[6] = value, the word at dBgW + 0x18. */
void func_020393d4(int *p, int v);
void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
    dCcAcPos_c *self, dActor_c *actor, const Vector3 *offset,
    int radius, int height, unsigned int flags, unsigned int vulnFlags);
/* CLPS block ("CLPS" magic) handed to SetFile. */
extern char data_ov092_02132220;
/* Three s32* move scripts, indexed by param1 & 3. A path (kind 3) ignores it. */
extern char data_ov092_02132294;
extern struct Matrix4x3 data_020a0e68;
}

// @symbol _ZN8daOnms_cD1Ev
// @symbol _ZN8daOnms_cD0Ev
/* D1 tears down dCcAcPos_c, dBgCh_Actr and the inline dBgActor_c base.
   D0 is that teardown plus the actor-heap free. */
daOnms_c::~daOnms_c()
{
}

// @symbol _ZN8daOnms_c9StateSinkEv
/* State 8: sink, then go once 0x3e8000 below the rest height. */
void daOnms_c::StateSink()
{
    mPosY -= 0x5000;
    if (mPosY < mRestPos.y - 0x3e8000)
        MarkForDestruction();
}

// @symbol _ZN8daOnms_c11StateBounceEv
/* State 7: tumble until the box lands, then shake the ground. */
int daOnms_c::StateBounce()
{
    Vector3 tmp;
    Vector3 eq;
    Vector3 dust;

    Player *player = ClosestPlayer();
    if (player != 0) {
        if (func_ov002_020de328(player) != 0) {
            mdCcAcPos_c.vulnFlags |= 0x400000;
            mdCcAcPos_c.flags |= 4;
        } else {
            mdCcAcPos_c.vulnFlags &= ~0x400000;
            mdCcAcPos_c.flags &= ~4;
        }
    }

    mAngleX += mTumbleVelX;
    mAngleY += mTumbleVelY;
    mAngleZ += mTumbleVelZ;

    /* Both early exits return the zero they tested; `return 0` differs. */
    int v = mVertAccel;
    if (v == 0)
        return v;

    UpdatePos(0);
    dBgCh_Actr_UpdateContinuous_Veneer(&mWithMeshClsn);
    int g = mWithMeshClsn.IsOnGround();
    if (g == 0)
        return g;

    mTumbleVelX = mTumbleVelY = mTumbleVelZ = 0;
    mHorzSpeed = 0;
    mVertSpeed = 0;

    eq.x = mPosX;
    eq.y = mPosY;
    eq.z = mPosZ;
    ((void (*)(void *, const Vector3 *, int))_ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE)(
        this, &eq, 0x5dc000);
    func_02012694(0x46, CamPos(this));

    /* tmp is a dead copy the ROM keeps; building dust directly differs. */
    tmp.x = mPosX;
    tmp.y = mPosY;
    tmp.z = mPosZ;
    tmp.y -= 0x78000;
    dust = tmp;
    LandingDustAt(dust, 1);

    mVertAccel = 0;
    return 0;
}

// @symbol _ZN8daOnms_c12StateKnockedEv
/* State 6: fly and tumble after a hit. A wall destroys the box. Surface
   type 9 (CLPS w0 >> 19) sinks it, collision kind 8 (CLPS w0 & 31) bounces
   it, and any other floor breaks it. */
void daOnms_c::StateKnocked()
{
    Vector3 saved;
    Vector3 v1;
    Vector3 v2;

    mAngleX += mTumbleVelX;
    mAngleY += mTumbleVelY;
    mAngleZ += mTumbleVelZ;

    UpdatePos(0);

    saved.x = mPosX;
    saved.y = mPosY;
    saved.z = mPosZ;
    dBgCh_Actr_UpdateContinuous_Veneer(&mWithMeshClsn);

    if (mWithMeshClsn.IsOnWall() != 0) {
        TriplePoofDust();
        MarkForDestruction();
        Sound::PlayBank3(0x41, *CamPos(this));
        return;
    }

    if (mWithMeshClsn.IsOnGround() == 0)
        return;

    mRestPos.y = mPosY;
    /* GetFloorResult is the floor dBgPi. CLPS w0 sits at +4 (past the
       vptr). Hoisting that address DIFF 13 words in this function. */
    dBgPi *floor = (dBgPi *)_ZNK10dBgCh_Actr14GetFloorResultEv(&mWithMeshClsn);

    if (func_02037e38((char *)floor + 4) == 9) {
        mMoveDir = kDirSink;
        mHorzSpeed = 0;
        mVertSpeed = 0;
        func_02012694(0x178, CamPos(this));
    } else if (func_02037e84((char *)floor + 4) == 8) {
        /* The player's position is read through p: reading o->mPosX
           directly changes the code. */
        dActor_c *o;
        int *p;
        mMoveDir = kDirBounce;
        func_0203568c(&mWithMeshClsn, 0x78000);
        o = mPlayerActor;
        p = &o->mPosX;
        v1.x = p[0];
        v1.y = p[1];
        v1.z = p[2];
        ((void (*)(void *, const Vector3 *, int))_ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE)(
            this, &v1, 0xff8000);
        mPosX = saved.x;
        mPosY = saved.y;
        mPosZ = saved.z;
        func_02012694(0x178, CamPos(this));
    } else {
        dActor_c *o;
        int *p;
        TriplePoofDust();
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xfa, mPosX, mPosY, mPosZ);
        MarkForDestruction();
        o = mPlayerActor;
        p = &o->mPosX;
        v2.x = p[0];
        v2.y = p[1];
        v2.z = p[2];
        ((void (*)(void *, const Vector3 *, int))_ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE)(
            this, &v2, 0xff8000);
        Sound::PlayBank3(0x41, *CamPos(this));
    }
    mPlayerActor = 0;
}

// @symbol func_ov092_021313b0
/* Path mode: step to the next path node and pick the roll that heads for it. */
extern "C" void func_ov092_021313b0(void *t)
{
    daOnms_c *self = (daOnms_c *)t;
    Vector3 old;

    old = self->mPathNode;
    self->mPathNodeIndex++;
    if (self->mPathNodeIndex >= self->mPathNodeCount)
        self->mPathNodeIndex = 0;
    self->mPathPtr.GetNode(self->mPathNode, self->mPathNodeIndex);
    if (self->mPathNode.x == old.x && self->mPathNode.z == old.z) {
        self->mMoveDir = kDirWait;
        return;
    }
    u16 a = (u16)Vec3_HorzAngle(&old, &self->mPathNode);
    if (a >= 0x2000 && a < 0x6000) self->mMoveDir = kDirRollPosX;
    else if (a >= 0x6000 && a < 0xa000) self->mMoveDir = kDirRollNegZ;
    else if (a >= 0xa000 && a < 0xe000) self->mMoveDir = kDirRollNegX;
    else self->mMoveDir = kDirRollPosZ;
}

// @symbol _ZN8daOnms_c8NextMoveEv
void daOnms_c::NextMove()
{
    if (mMoveKind == kMoveAlongPath) {
        func_ov092_021313b0(this);
    } else {
        mMoveSeqIndex++;
        s32 *seq = mMoveSeq;
        int v = seq[mMoveSeqIndex];
        if (v != -1) {
            mMoveDir = v;
        } else {
            mMoveDir = seq[0];
            mMoveSeqIndex = 0;
        }
    }
    mOrientBits = (mPrevAngleX >> 0xe) & 3;
    mOrientBits |= (mPrevAngleY >> 0xc) & 0xc;
    mOrientBits |= (mPrevAngleZ >> 0xa) & 0x30;
}

// @symbol _ZN8daOnms_c19func_ov092_02131578Ev
/* State 1: sit still, take the next move after 0x14 frames, watch for a hit. */
void daOnms_c::func_ov092_02131578()
{
    mTumbling = 0;
    if (mStateTimer == 0x14)
        NextMove();
    CheckPlayerHit();
}

// @symbol _ZN8daOnms_c9StateLandEv
/* State 0: a roll has ended. Shake the ground on the first frame, then wait. */
void daOnms_c::StateLand()
{
    mHorzSpeed = 0;
    if (!mStateTimer) {
        Vector3 v;
        v.x = mPosX; v.y = mPosY; v.z = mPosZ;
        ((void (*)(void *, const Vector3 *, int))_ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE)(
            this, &v, 0x860000);
        func_02012694(0x46, CamPos(this));
    }
    mPosY = mRestPos.y + 0x3000;
    func_ov092_02131578();
}

// @symbol _ZN8daOnms_c13StateRollPosXEv
void daOnms_c::StateRollPosX()
{
    Roll(0, 0x3e800, 0, -0x800);
}

// @symbol _ZN8daOnms_c13StateRollNegXEv
void daOnms_c::StateRollNegX()
{
    Roll(0, -0x3e800, 0, 0x800);
}

// @symbol _ZN8daOnms_c13StateRollNegZEv
void daOnms_c::StateRollNegZ()
{
    Roll(-0x3e800, 0, -0x800, 0);
}

// @symbol _ZN8daOnms_c13StateRollPosZEv
void daOnms_c::StateRollPosZ()
{
    Roll(0x3e800, 0, 0x800, 0);
}

// @symbol _ZN8daOnms_c4RollEiiis
/* One frame of an eight-frame roll: hop along the sine table, step, and
   turn the face sBankAxis says is leading. The table is an s8 index from
   &mAngleX; a negative index subtracts `bank` from that angle instead. */
void daOnms_c::Roll(s32 stepZ, s32 stepX, s32 pitch, s16 bank)
{
    int idx;
    int y;
    s16 mv;
    s16 *ang;

    {
        s16 t = (s16)((mStateTimer + 1) << 12);
        idx = ((u16)t >> 4) * 2;
    }
    y = mRestPos.y + 0x3000;
    mv = data_02082214[idx];
    mPosY = y + ((mv * 0x6793a) >> 12);

    mPosX += stepX;
    ang = &mAngleX;
    mPosZ += stepZ;

    if (pitch) {
        u8 f = mOrientBits;
        if ((f & 0x10) != 0) {
            if ((f & 0x20) != 0)
                mAngleY += pitch;
            else
                mAngleY -= pitch;
        } else {
            if ((f & 0x20) != 0)
                mAngleX -= pitch;
            else
                mAngleX += pitch;
        }
    } else {
        u8 f = mOrientBits;
        s8 k = sBankAxis[f];
        if (k < 0) {
            s16 *p = ang - ((int)k + 1);
            *p = (s16)(*p - bank);
        } else {
            s16 t = ang[k];
            ang[k] = (s16)(t + bank);
        }
    }

    if (mStateTimer != 7)
        return;

    mPrevAngleX += mAngleX;
    mPrevAngleY += mAngleY;
    mPrevAngleZ += mAngleZ;

    if (mMoveKind == kMoveAlongPath)
        mMoveDir = kDirLand;
    else
        NextMove();
    mRollDone = 1;
}

// @symbol _ZN8daOnms_c6LaunchEP8dActor_cj
/* A player punched (how 0), kicked (1) or bumped the box from below (2).
   Only a flat side (orient bits 0..3 clear) and a player whose param1 low
   bits are 2 can launch it. */
void daOnms_c::Launch(dActor_c *player, u32 how)
{
    u8 f = mOrientBits;
    int t = (f & 0xf) + ((f >> 4) & 3);
    if ((t & 3) != 0) return;
    if ((player->param1 & 3) != 2) return;
    if (Vec3_HorzDist(ActorPos(this), ActorPos(player)) >= 0xd2000) return;
    func_02012694(0x177, CamPos(this));
    mMoveDir = kDirKnocked;
    mVertAccel = -0xc000;
    mTerminalVelocity = -0x48000;
    mVertSpeed = sLaunchVertSpeed[how];
    mHorzSpeed = sLaunchHorzSpeed[how];
    if (how < 2) mPrevAngleY = player->mAngleY;
    else mPrevAngleY = Vec3_HorzAngle(ActorPos(player), ActorPos(this));

    unsigned n = ((u16)mPrevAngleY) >> 4;
    mTumbleVelX = (s16)(data_02082214[(n << 1) + 1] >> 2);
    mTumbleVelY = 0;
    mTumbleVelZ = (s16)((-(int)data_02082214[n << 1]) >> 2);
    mFlags &= ~0x2000000;
    mdCcAcPos_c.radius = 0x1a9000;
}

// @symbol _ZN8daOnms_c14CheckPlayerHitEv
void daOnms_c::CheckPlayerHit()
{
    unsigned int id = mdCcAcPos_c.otherOwner;
    if (id == 0) return;
    dActor_c *o = dActor_c::FindWithID(id);
    if (o == 0) return;
    /* PLAYER is actor 0xbf. A flag, not `if (o->actorID != 0xbf)`: the
       direct test differs. */
    int b = (o->actorID == 0xbf);
    if (b == 0) return;
    int f = mdCcAcPos_c.hitFlags;
    if (f & 0x40) {
        Launch(o, 0);
    } else if (f & (0x80 | 0x100 | 0x200)) {
        Launch(o, 1);
    } else if (BumpedUnderneathByPlayer(*(Player *)o) != 0) {
        if (o->mPosY > mPosY - 0x64000)
            Launch(o, 2);
    }
    mPlayerActor = o;
}

// @symbol func_ov092_02131a88
/* Copy the model matrix over the collider and put the actor position in
   the translation row (flat Matrix4x3 words 9..11), then Transform. */
extern "C" void func_ov092_02131a88(char *c)
{
    daOnms_c *self = (daOnms_c *)c;

    self->mClsnMat = self->mModel.mat4x3;
    self->mClsnMat.m[9] = self->mPosX;
    self->mClsnMat.m[10] = self->mPosY;
    self->mClsnMat.m[11] = self->mPosZ;
    self->mMeshCollider.Transform(self->mClsnMat, self->mAngleY);
}

// @symbol _ZN8daOnms_c14UpdateModelMtxEv
void daOnms_c::UpdateModelMtx()
{
    if (mTumbling != 0) {
        data_020a0e68 = mBaseMtx;
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, mAngleX);
        Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, mAngleZ);
        Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);
        mModel.mat4x3 = data_020a0e68;
        mModel.mat4x3.m[9] = mPosX >> 3;
        mModel.mat4x3.m[10] = mPosY >> 3;
        mModel.mat4x3.m[11] = mPosZ >> 3;
    } else {
        mModel.mat4x3 = mBaseMtx;
    }
}

// @symbol _ZN8daOnms_c16CleanupResourcesEv
int daOnms_c::CleanupResources()
{
    data_ov092_02132540.Release();
    data_ov092_02132548.Release();
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    return 1;
}

// @symbol _ZN8daOnms_c6RenderEv
int daOnms_c::Render()
{
    mModel.Render(0);
    return 1;
}

// @symbol _ZN8daOnms_c8BehaviorEv
int daOnms_c::Behavior()
{
    mTumbling = 1;
    mRollDone = 0;

    s32 oldState = mMoveDir;
    (this->*data_ov092_02132568[oldState].state)();
    mStateTimer++;
    if (oldState != mMoveDir)
        mStateTimer = 0;

    UpdateModelMtx();
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0) != 0)
        func_ov092_02131a88((char *)this);

    if (mRollDone == 1) {
        if (mAngleX != 0 || mAngleY != 0 || mAngleZ != 0) {
            mBaseMtx = mModel.mat4x3;
            mAngleX = 0;
            mAngleY = 0;
            mAngleZ = 0;
        }
    }

    mdCcAcPos_c.Clear();
    Vector3 offset;
    offset.x = 0;
    offset.y = -0xfa000;
    offset.z = 0;
    mdCcAcPos_c.SetPosRelativeToActor(offset);
    mdCcAcPos_c.Update();
    return 1;
}

// @symbol _ZN8daOnms_c13InitResourcesEv
int daOnms_c::InitResources()
{
    void *f;
    Vec3 tmp;

    f = Model::LoadFile(data_ov092_02132540);
    mModel.SetFile((BMD_File *)f, 1, -1);

    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
        &mWithMeshClsn, (int)this, 0xfa000, 0, 0, 0);
    mWithMeshClsn.SetLimMovFlag();

    mMoveKind = param1 & 3;
    if (mMoveKind != kMoveAlongPath) {
        mMoveSeq = ((s32 **)&data_ov092_02132294)[mMoveKind];
        mMoveSeqIndex = 0;
        mMoveDir = *mMoveSeq;
    } else {
        mPathPtr.FromID((param1 >> 8) & 0xf);
        mPathNodeCount = mPathPtr.NumNodes();
        mPathNodeIndex = 0;
        mPathPtr.GetNode(mPathNode, mPathNodeIndex);
        func_ov092_021313b0(this);
    }

    mPosY += 0xfa000;
    mRestPos.x = mPosX;
    mRestPos.y = mPosY;
    mRestPos.z = mPosZ;
    Vec3_Asr(&tmp, (Vec3 *)ActorPos(this), 3);

    Matrix4x3_FromTranslation(&data_020a0e68, tmp.x, tmp.y, tmp.z);
    mModel.mat4x3 = data_020a0e68;
    mBaseMtx = mModel.mat4x3;

    func_ov092_02131a88((char *)this);

    f = dBgW_Kc::LoadFile(data_ov092_02132548);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, (KCL_File *)f, &mClsnMat,
        0x1000, mAngleY, &data_ov092_02132220);
    func_020393d4((int *)&mMeshCollider, (int)&dBgW::UpdatePosWithTransform);

    mOrientBits = (unsigned char)((mAngleX >> 0xe) & 3);
    mOrientBits |= (mAngleY >> 0xc) & 0xc;
    mOrientBits |= (mAngleZ >> 0xa) & 0x30;

    {
        Vector3 offset;
        offset.x = 0;
        offset.y = -0xfa000;
        offset.z = 0;
        _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
            &mdCcAcPos_c, this, &offset, 0xc8000, 0x190000, 2, 0x6003c0);
    }

    mPlayerActor = 0;
    return 1;
}

// @symbol daOnms_c_classInit
extern "C" daOnms_c *daOnms_c_classInit()
{
    return new daOnms_c();
}

/* __sinit_daOnms_c.cpp constructs the two file handles in retail order,
 * then copies the nine anonymous pointer-to-member descriptors into the
 * state table Behavior indexes by mMoveDir. StateBounce returns int, so
 * its descriptor needs the cast; the emitted pair is still {addr, 0}. */
OnmsModelFilePtr data_ov092_02132540(0x3c9);
OnmsCollisionFilePtr data_ov092_02132548(0x3ca);
daOnms_cStateEntry data_ov092_02132568[9] = {
    &daOnms_c::StateLand,
    &daOnms_c::func_ov092_02131578,
    &daOnms_c::StateRollPosZ,
    &daOnms_c::StateRollNegZ,
    &daOnms_c::StateRollNegX,
    &daOnms_c::StateRollPosX,
    &daOnms_c::StateKnocked,
    reinterpret_cast<daOnms_cState>(&daOnms_c::StateBounce),
    &daOnms_c::StateSink,
};
