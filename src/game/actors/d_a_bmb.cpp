//cpp
/* Bob-omb (BOB_OMB 206 / BOMBHEI) -- ov102/daBmb_c.
 *
 * common.h FIRST. InitResources and the carry update copy twelve matrix words.
 * math/Matrix.h's nested Matrix4x3 scalarizes that copy (Vector3 is non-POD).
 *
 * Leftovers, remeasured in this file:
 * - Behavior skips 4 bytes past GetFloorResult's dBgPi to the SurfaceInfo
 *   and tests CLPS bit 0x20 (water).
 * - States 0, 1 and 3 advance the dExtFrameCtrl_c base of mAnmModel
 *   explicitly. The qualified base call preserves the ROM entry point
 *   without a byte-offset pointer or a dExtAnmModel_c forwarding call.
 * - dActor_c::mCarryMatrix (0xc8) is the matrix 0214b53c copies into the
 *   model while mVariant is 2; on the carrier it is only tested nonzero.
 * - Behavior's helpers and the state arms are methods: every caller passes
 *   this Bob-omb as the first argument. Members whose purpose nothing in the
 *   cartridge names keep their address as the method name. Their C callees
 *   come from extern "C" file-scope declarations (this block, Sound.h,
 *   CalcLib.h): a block-scope extern inside a member gets C++ linkage and the
 *   module stops linking.
 * - func_ov102_0214ad14, 0214ae1c and 0214b384 are called from other overlays,
 *   so those labels stay. data_ov102_* are the file handles; SharedFilePtr
 *   has no fields, so the loaded BCA is still the word at +4.
 * - func_0203568c / func_02035684 are the out-of-line stores of
 *   mWithMeshClsn.mRadius / mHeight. Inlining them drops the call.
 * - func_ov102_0214b53c keeps the volatile stack pins and reads the eating
 *   player (mEatingPlayer, not the carrier): mPos, mAngleX/Y/Z, mHeadMatrix
 *   and mHeadBoneRot, which d_a_ply_methods.cpp fills from bone 15's
 *   rotX/rotY/rotZ.
 * - ov078/daBombking_c writes mHurtDamage and mDetonate by those names.
 */

#include "common.h"
#include "daBmb_c.h"
#include "SpawnInfo.h"
#include "daPly_c.h"
#include "Sound.h"
#include "SharedFilePtr.h"
#include "dBgCh_Gnd.h"
#include "dBgCh_Actr.h"
#include "math/Approach.h"
#include "math/CalcLib.h"
#include "Particle__System.h"
#include "math/MtxStack.h"

/* The three shared files this actor claims: ov002's model handle here and the
   two animation handles below.  InitResources loads them and
   CleanupResources releases them, so both C++-named members need one agreed
   spelling; the members below that read the loaded handle out of the same bytes
   take their own view with a cast rather than a second declaration. */
extern SharedFilePtr data_ov002_0210d9e0;

/* Runtime asset handles, distinct from the NitroFS file IDs. */
enum daBmb_c_AssetId { ASSET_BOMBHEI_CARRY_BCA = 0x02b7, ASSET_BOMBHEI_WALK_BCA = 0x02b8 };

/* The two animation handles reach their files through this wrapper's
   constructor and destructor veneers; the manifest maps each to the retail
   destination.  Descriptive local type, not a recovered spelling. */
struct BmbAnimFile : SharedFilePtr {
    BmbAnimFile(u32 fileID);
    ~BmbAnimFile();
};

/* This TU's own handles, in the order the retail initializer (0x0214de70)
   builds them. */
BmbAnimFile gBmbWalkAnimFile(ASSET_BOMBHEI_WALK_BCA);
BmbAnimFile gBmbCarryAnimFile(ASSET_BOMBHEI_CARRY_BCA);

/* Carry offsets, indexed by the carrier's pose (bit 0: front-sliding or
   just lost the grab; bit 1: carrier param1 == 2).  The retail initializer
   constructs and registers each element in turn. */
Vector3 gBmbCarryOffsets[4] = {
    Vector3(0x1c000, -0x14000, 0x28000),
    Vector3(0x1c000, -0x14000, 0x3c000),
    Vector3(0x2b000, -0xa000, -0x14000),
    Vector3(0x30000, -0x1e000, 0x3c000),
};

extern Matrix4x3 IDENTITY_MATRIX4X3;

/* --------------------------------------------------------------------------
 * The one file-scope extern "C" region.  Everything here is reached from a
 * C++-named member, which cannot declare it in its own body.
 * ------------------------------------------------------------------------ */
extern "C" {

/* -- this TU's own free functions, forward-declared because mwcc lays .text
      down in reverse source order and every one of these calls is a forward
      reference. */
void  func_ov102_0214ae1c(void *self);
void  func_ov102_0214b384(void *self, unsigned int level);
void  func_ov102_0214baa0(char *self);
void  func_ov102_0214beb4(void *self);

/* -- other modules -- */
void  GiveCoins(int who, int count);
int   SurfaceInfo_TestFlag0x20(int *si);
/* 0x0203b0e8. Not declared in math/CalcLib.h (a shared header); callers spell it int, int. */
int AngleDiff(int, int);
/* The out-of-line stores of mWithMeshClsn's radius and height. */
void func_0203568c(int *value, int target);
void func_02035684(int *value, int target);
/* local extern: no header declares these; each is spelled as its
   definition is (d_bg_actor.cpp, d_a_ply_methods.cpp, src/). */
int func_ov002_020ef228(dBgCh_Actr *clsn, int otherArg);
int func_ov002_020e496c(daPly_c *c);
int Math_Function_0203b14c(int *out, int a1, int a2, int a3, int a4);
void Vec3_Lsl(Vector3 *d, const Vector3 *s, int sh);
int *Vec3_LslInPlace(int *v, int sh);
/* Hoisted out of four member bodies.  A class member function may not sit in a
   block-scope linkage specification, so daBmb_c::State1/3/4/5 cannot carry their
   own extern "C" declarations the way the free members below do -- these have to
   be file-scope, and they are spelled to agree with every other user in the TU. */
void  func_ov102_0214b3b8(void *c);
/* Real definitions: func_0201267c and func_0200fc44 (arm9).
   The methods call through these so the names stay unmangled. */
int   func_0200fc44(int a, Vector3 *pos, int flag);

extern signed char gCurrentSublevel;
extern unsigned char data_0209f220;
extern unsigned char data_0209f2d8;

}

/* ==========================================================================
 *
 * The registry factory.  It is the TOP of the cartridge's contiguous run
 * (0x0214c748 is where daShl_c starts), so it is written FIRST here: mwccarm
 * lays .text down in reverse source order.
 *
 * C linkage preserves the project's existing bare registry-factory label.
 * The address is ROM-proven; the original SM64DS spelling is not preserved.
 * `return new daBmb_c()` MATCHES (size 0x50); the synthesized ctor stores
 * `_ZTV7daBmb_c + 2` because this TU defines the vtable. `new` calls the
 * inherited fBase_c::operator new, the retail `_ZN7fBase_cnwEm` allocator.
 *
 * Reconstructed source-style name: SM64DS proves daBmb_c through RTTI,
 * allocation size, vtable identity, and the BOMBHEI registry profile; later EAD
 * lineage supplies classInit.  Exact original spelling is not preserved.
 * Historical alias: BobOmb_Spawn.
 * ======================================================================== */

extern "C" {

daBmb_c *daBmb_c_classInit(void)
{
    return new daBmb_c();
}

}

extern "C" SpawnInfo<daBmb_c> g_profile_BOMBHEI = {
    daBmb_c_classInit,
    0x00ce, /* BOMBHEI registry id 206 */
    0x002d,
    0x10008083,
    0x00032000,
    0x00046000,
    0x01000000,
    0x01000000,
};

/* ==========================================================================
 * Vtable slot 18.  THE KEY FUNCTION: the first out-of-line virtual this class
 * declares, so this TU owns _ZTV7daBmb_c, _ZTI7daBmb_c and _ZTS7daBmb_c.
 * ======================================================================== */

s32 daBmb_c::OnYoshiTryEat() {
    /* 263 == 0x107, dEnemyBase_c's own byte at that offset. */
    return mEatenByYoshi == 0;
}

/* ==========================================================================
 * Vtable slot 0.
 *
 * param1's low three bits pick the variant: 2 is the one that starts inert --
 * it sets the collision volume's `hit` bit, clears a behaviour flag and leaves
 * mCoinType clear -- 4 also starts clear, and everything else starts live.
 * ======================================================================== */

int daBmb_c::InitResources()
{
    BMD_File* bmd;

    dExtFrameCtrl_c::LoadFile(gBmbWalkAnimFile);
    dExtFrameCtrl_c::LoadFile(gBmbCarryAnimFile);
    bmd = (BMD_File *)dExtSimpleModel_c::LoadFile(data_ov002_0210d9e0);
    if (mAnmModel.SetFile(bmd, 1, -1) == 0)
        return 0;
    if (mShadowModel.InitCylinder() == 0)
        return 0;

    mVariant = (unsigned char)(param1 & 7);
    mNoticeAngle = 0x2000;
    func_ov102_0214c0b8();
    mdCc_c.Init(this, 0x3c000, 0x50000, 0x200004, 0xa6d380);
    mTerminalVelocity = -0x37000;

    if (mVariant == 2) {
        /* Variant 2 starts inert: set the cylinder's flags bit 2 and clear
           mFlags bit 0 (the profile clip-enable bit). */
        mdCc_c.flags |= 2;
        mFlags &= ~1u;
        mCoinType = 0;
    } else if (mVariant == 4) {
        mCoinType = 0;
    } else {
        mCoinType = 1;
    }

    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    *(Matrix4x3 *)mMatrix = IDENTITY_MATRIX4X3;
    mTurnTimer = 0;
    mFuse = 0;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    mCarrier = 0;
    mTurnCount = 0;
    mWithMeshClsn.Init(this, 0x32000, 0x32000, 0, 0);
    mWithMeshClsn.StartDetectingWater();
    mShouldRender = 1;
    /* Matrix4x3* while mVariant is 2. It lives in dActor_c's pad, the word
       after mClipResult. */
    *(int *)((char *)&mClipResult + 4) = 0;
    mHurtDamage = 2;
    mDetonate = 0;
    mHomeAngleY = mAngleY;
    return 1;
}

/* ==========================================================================
 * Vtable slot 6.
 * ======================================================================== */

int daBmb_c::Behavior()
{
    int flag;
    int killResult;
    void *other;

    if (mDetonate != 0) {
        func_ov102_0214ae1c(this);
        return 1;
    }
    if (func_ov102_0214ab1c()) {
        return 1;
    }
    if (func_ov102_0214aa18()) {
        return 1;
    }

    killResult = UpdateKillByInvincibleChar(mWithMeshClsn, mAnmModel, 0);
    if (killResult != 0) {
        if (killResult == 2) {
            func_ov102_0214ae1c(this);
        }
        return 1;
    }

    if (mDeathState != 0) {
        UpdateDeath(mWithMeshClsn);
        func_ov102_0214b128();
        flag = mFlags & 0x100;
        flag = flag != 0;
        if (flag != false) {
            mDeathState = 0;
        } else if (mDeathState != 0) {
            func_ov102_0214b53c();
            mdCc_c.Clear();
            mdCc_c.Update();
            return 1;
        }
    }

    func_ov102_0214b03c();
    if (mState != 5) {
        if (mVertAccel != 0) {
            if ((mdCc_c.hitFlags & 0x10) != 0) {
                short v[3];
                other = dActor_c::FindWithID(mdCc_c.otherOwner);
                v[0] = -0x2000;
                v[1] = 0;
                v[2] = 0;
                KillByInvincibleChar(*(Vector3_16 *)v, *(daPly_c *)other, 0x32000);
                UpdatePos(&mdCc_c);
                UpdateWMClsn(mWithMeshClsn, 0);
                mdCc_c.Clear();
                return 1;
            }

            UpdatePos(&mdCc_c);
            if (gCurrentSublevel == 6 && data_0209f220 == 3) {
                if (mHorzSpeed == 0x5000) {
                    UpdateWMClsn(mWithMeshClsn, 3);
                } else {
                    UpdateWMClsn(mWithMeshClsn, 2);
                }
            } else {
                UpdateWMClsn(mWithMeshClsn, 2);
            }

            if (mWithMeshClsn.IsOnGround()) {
                if (SurfaceInfo_TestFlag0x20((int *)((char *)mWithMeshClsn.GetFloorResult() + 4))) {
                    func_ov102_0214ae1c(this);
                    return 1;
                }
                if (mWithMeshClsn.IsOnWall() && mState == 0) {
                    func_ov102_0214beb4(this);
                }
            }
        }

        killResult = UpdateFuse();
        if (killResult == 0) {
            return 0;
        }

        if (mdCc_c.otherOwner != 0) {
            if ((mdCc_c.hitFlags & 0x4000) != 0) {
                func_ov102_0214b384(this, 4);
            }
            if (mState == 4) {
                unsigned char b = mVariant;
                if (b == 2 || b == 3) {
                    other = dActor_c::FindWithID(mdCc_c.otherOwner);
                    if (other != 0) {
                        int flag2 = ((dActor_c *)other)->actorID;
                        flag2 = flag2 == 0xbd;
                        if (flag2 != false) {
                            func_ov102_0214b384(this, 2);
                            mdCc_c.flags |= 0x4000;
                        }
                    }
                }
            }
        }

        func_ov102_0214b53c();
        mdCc_c.Clear();
        mdCc_c.Update();
        func_ov102_0214ad40();
    }
    return 1;
}

/* ==========================================================================
 * Vtable slot 9.
 * ======================================================================== */

int daBmb_c::Render()
{
    int result = 1;
    if (mShouldRender != 0) {
        int flags = mFlags;
        int b = (flags & 0x40000) != 0;
        if (!b) {
            mAnmModel.Render(&mScale);
        }
    }
    return result;
}

/* ==========================================================================
 * Vtable slot 3.  Releases the three shared files InitResources claimed and
 * touches no field of its own.
 * ======================================================================== */

int daBmb_c::CleanupResources()
{
    data_ov002_0210d9e0.Release();
    gBmbWalkAnimFile.Release();
    gBmbCarryAnimFile.Release();
    return 1;
}

/* ==========================================================================
 *
 * Back to mState arm 0: walk animation, walking speed (none for variant 1),
 * mTurnTimer 0x200, gravity on, no carrier.
 * ======================================================================== */

void daBmb_c::func_ov102_0214c0b8() {
    mState = 0;
    if (mVariant == 1)
        mHorzSpeed = 0;
    else
        mHorzSpeed = 0x5000;
    mAnmModel.SetAnim((BCA_File *)gBmbWalkAnimFile.filePtr, 0, 0x1000, 0);
    mTurnTimer = 0x200;
    mVertAccel = -0x2000;
    mCarrier = 0;
}

/* ==========================================================================
 *
 * mState arm 0 (COINED NAME -- ov102 carries this address and no identifier;
 * func_ov102_0214b03c switching on mState is what proves the index): the walk.
 * With nobody to chase, variant 5 steers toward home and the others hand off
 * to arm 1 when mTurnTimer runs out or they stray 0x500000 from home.  A
 * player inside the notice window starts a hop (arm 2); while chasing, it
 * runs at the player.  Variant 1 stands still.  Scalar position members remain borrowed
 * Vector3 views; no owning Vector3 temporaries are introduced.
 * ======================================================================== */

void daBmb_c::State0() {
    DecIfAbove0_Short(&mTurnTimer);
    if (mVariant != 1) {
        if (mChasePlayer == 0) {
            ApproachLinear(mHorzSpeed, 0x5000, 0x2aa);
            func_ov102_0214b988();
            if (mChasePlayer != 0) {
                mHorzSpeed = 0;
                mVertSpeed = 0x14000;
                mState = 2;
            } else {
                if (mVariant == 5) {
                    s16 ang = Vec3_HorzAngle(&mPos, (const Vector3 *)&mHomePosX);
                    ApproachLinear(mPrevAngleY, ang, 0x200);
                    mAngleY = mPrevAngleY;
                } else {
                    if (mTurnTimer == 0 ||
                        Vec3_Dist(&mPos, (const Vector3 *)&mHomePosX) >= 0x500000) {
                        mHorzSpeed = 0x5000;
                        func_ov102_0214beb4(this);
                    }
                }
            }
        } else {
            ApproachLinear(mHorzSpeed, 0x10000, 0x1000);
            s16 target = Vec3_HorzAngle(&mPos, &mChasePlayer->mPos);
            if (mVariant == 3) {
                /* daPly_c::mGrabbedByActor: stop turning toward a held player. */
                int b = (mChasePlayer->mGrabbedByActor != 0);
                if (b)
                    target = mPrevAngleY;
            }
            ApproachLinear(mPrevAngleY, target, 0x800);
            mAngleY = mPrevAngleY;
        }
        mAnmModel.speed = mHorzSpeed >> 3;
    }
    mAnmModel.dExtFrameCtrl_c::Advance();
}

/* ==========================================================================
 * ======================================================================== */

extern "C" {

void func_ov102_0214beb4(void *cv)
{
    extern int data_0209e650;

    daBmb_c *self = (daBmb_c *)cv;

    self->mState = 1;
    self->mTurnCount += 1;
    daPly_c *o = self->mChasePlayer;
    if (o == 0) {
        *(short *)&self->mTargetAngY =
            Vec3_HorzAngle((const Vector3 *)&self->mPosX, (const Vector3 *)&self->mHomePosX);
        if (Vec3_Dist((const Vector3 *)&self->mPosX, (const Vector3 *)&self->mHomePosX) >= 0x500000)
            return;
        unsigned int r = RandomIntInternal(&data_0209e650);
        *(short *)&self->mTargetAngY += (short)(r >> 0x10);
        return;
    }
    *(short *)&self->mTargetAngY =
        Vec3_HorzAngle((const Vector3 *)&self->mPosX, (const Vector3 *)&o->mPosX);
}

}

/* ==========================================================================
 *
 * mState arm 1 (COINED NAME -- ov102 carries this address and no identifier;
 * func_ov102_0214b03c switching on mState is what proves the index): turns the facing angle (+0x94) toward the stored target angle (+0x3ee) at 0x400 a
 * frame with no target actor and 0x800 with one, mirrors it into +0x8e, advances
 * the animation, and once the two angles meet -- and only while mState is not 2 --
 * hands off to func_ov102_0214c0b8.
 * ======================================================================== */

void daBmb_c::State1() {
    if (mChasePlayer == 0) {
        mAnmModel.speed = 0x800;
        ApproachLinear(mPrevAngleY, *(short *)&mTargetAngY, 0x400);
    } else {
        mAnmModel.speed = 0x1000;
        ApproachLinear(mPrevAngleY, *(short *)&mTargetAngY, 0x800);
    }
    mAngleY = mPrevAngleY;
    mAnmModel.dExtFrameCtrl_c::Advance();
    if (mState == 2) return;
    if (mPrevAngleY != *(short *)&mTargetAngY) return;
    func_ov102_0214c0b8();
}

/* ==========================================================================
 *
 * mState arm 2. The first argument is this Bob-omb. It calls func_0200fc44
 * through the file-scope three-argument prototype, the definition's own.
 * ======================================================================== */

void daBmb_c::func_ov102_0214bd90(){
  if (mWithMeshClsn.JustHitGround()) {
    struct Vector3 v;
    v.x = mPosX;
    v.y = mPosY;
    v.z = mPosZ;
    func_0200fc44((int)this, &v, 1);
  }
  if (mWithMeshClsn.IsOnGround()) {
    if (mChasePlayer == 0) {
      mHorzSpeed = 0x5000;
    }
    func_ov102_0214beb4(this);
  } else {
    State1();
  }
}

/* ==========================================================================
 * ======================================================================== */

extern "C" {

void func_ov102_0214bd20(char *c) {
    daBmb_c *self = (daBmb_c *)c;

    self->mState = 3;
    self->mVertAccel = 0;
    self->mHorzSpeed = 0;
    self->mAnmModel.SetAnim((BCA_File *)gBmbCarryAnimFile.filePtr, 0, 0x1000, 0);
    func_ov102_0214b384(c, 0x96);
    self->mdCc_c.flags |= 2;
    self->mdCc_c.flags &= ~4;
}
}

/* ==========================================================================
 *
 * mState arm 3 (COINED NAME -- ov102 carries this address and no identifier;
 * func_ov102_0214b03c switching on mState is what proves the index): the held arm: on mFlags bit 0x400 it runs func_ov102_0214b3b8 (which checks
 * mState == 3, runs func_ov102_0214bc20, copies the carrier's facing angle and
 * clears mCarrier), on bit 0x100 it does nothing at all, and otherwise it hands off
 * to func_ov102_0214c0b8.  Always advances the animation.
 * ======================================================================== */

void daBmb_c::State3()
{
    /* Nothing names these two mFlags bits yet, so they keep neutral names rather
     * than a guessed meaning. Both booleans are materialized on purpose: the
     * cartridge tests the flag word into a register and then re-tests that, and
     * folding either into its `if` collapses the pair.
     *
     * The empty 0x100 arm is real -- on that bit this state does nothing at all,
     * and the else-branch is what carries the work. */
    int flags = mFlags;
    int flag0x400 = (int) ((flags & 0x400) != 0);
    if (flag0x400)
    {
        func_ov102_0214b3b8(this);
    }
    else
    {
        int flag0x100 = (int) ((flags & 0x100) != 0);
        if (flag0x100)
        {
        }
        else
        {
            func_ov102_0214c0b8();
        }
    }
    mAnmModel.dExtFrameCtrl_c::Advance();
}

/* ==========================================================================
 * ======================================================================== */

extern "C" {

/* THE QUAD VIEW AND ITS TWO TABLES SIT AT FILE SCOPE. They used to be declared
   inside the body below, but C++ gives a function-local class no linkage, so an
   `extern` of that type is ill-formed (MSVC C2624) even though mwccarm accepts
   it. Hoisting the type changes nothing the compiler emits: the object is
   byte-identical under 2004/b56. */
typedef struct { int v[4]; } Quad;
/* Thrown-bob-omb launch speeds, indexed by the carrier's param1 (clamped to 4):
   horizontal, then vertical. */
extern "C" Quad data_ov102_0214e514 = {{0x28000, 0x28000, 0x50000, 0x28000}};
extern "C" Quad data_ov102_0214e524 = {{0xa000, 0xa000, 0x1a000, 0xa000}};

void func_ov102_0214bc20(char *c) {
    daBmb_c *self = (daBmb_c *)c;

    Quad a1 = data_ov102_0214e514;
    Quad a2 = data_ov102_0214e524;
    daPly_c *p = self->mCarrier;
    int idx = 0;
    if (p != 0) {
        idx = p->param1;
        if ((unsigned)idx > 4) idx = 4;
    }
    self->mHorzSpeed = a1.v[idx];
    self->mVertSpeed = a2.v[idx];
    self->mState = 4;
    self->mAnmModel.SetAnim((BCA_File *)gBmbWalkAnimFile.filePtr, 0, 0x1000, 0);
    self->mVertAccel = -0x2000;
}
}

/* ==========================================================================
 *
 * mState arm 4 (COINED NAME -- ov102 carries this address and no identifier;
 * func_ov102_0214b03c switching on mState is what proves the index): the thrown arm: unless the collision actor reports neither ground nor wall contact
 * it calls func_ov102_0214b384(4) and zeroes the forward speed.
 * ======================================================================== */

int daBmb_c::State4() {
    if (mWithMeshClsn.IsOnGround() == 0) {
        int r = mWithMeshClsn.IsOnWall();
        if (r == 0) return r;
    }
    func_ov102_0214b384(this, 4);
    mHorzSpeed = 0;
    return 0;
}

/* Void on purpose. An int return keeps r0 live, so the final param1 = 0
   lands in r1 and the function no longer matches. */
extern "C" {

void func_ov102_0214baa0(char *raw)
{
    extern void func_0203568c(int *value, int target);
    extern void func_02035684(int *value, int target);

    daBmb_c *self = (daBmb_c *)raw;
    u8 variant;

    self->mFlags &= ~0x10000000;
    variant = self->mVariant;
    if (!(variant < 2 || variant == 5)) {
        self->KillAndTrackInDeathTable();
        return;
    }

    self->mState = 5;
    self->mHorzSpeed = 0;
    self->mdCc_c.Clear();

    self->mFlags &= 0xfff1fffe;
    self->mPosX = self->mHomePosX;
    self->mPosY = self->mHomePosY;
    self->mPosZ = self->mHomePosZ;
    self->mShouldRender = 0;
    self->mChasePlayer = 0;
    self->mCarrier = 0;
    self->mFuse = 0;
    self->mScaleX = 0x1000;
    self->mScaleY = 0x1000;
    self->mScaleZ = 0x1000;
    self->mAngleX = 0;
    self->mAngleY = 0;
    self->mAngleZ = 0;
    self->mPrevAngleX = 0;
    self->mPrevAngleY = 0;
    self->mPrevAngleZ = 0;

    {
        s16 angle = *(s16 *)&self->mHomeAngleY;
        self->mAngleY = angle;
        s16 copy = self->mAngleY;
        self->mPrevAngleY = copy;
    }

    self->mdCc_c.flags &= ~0x4002;
    self->mdCc_c.flags |= 4;
    self->mdCc_c.radius = 0x3c000;
    self->mdCc_c.height = 0x50000;

    func_0203568c((int *)&self->mWithMeshClsn, 0x32000);
    func_02035684((int *)&self->mWithMeshClsn, 0x32000);
    self->param1 = 0;
}

}

/* ==========================================================================
 *
 * mState arm 5 (COINED NAME -- ov102 carries this address and no identifier;
 * func_ov102_0214b03c switching on mState is what proves the index): the dormant arm, which func_ov102_0214baa0 puts the actor into after clearing
 * mShouldRender: once the camera player is at least 0x7d0000 away -- or the actor is
 * off-camera and data_0209f2d8 is 1 -- it runs func_ov102_0214c0b8, sets mFlags
 * 0x10000001 and turns rendering back on.
 * ======================================================================== */

void daBmb_c::State5()
{
    if (DistToCPlayer() < 0x7d0000)
        return;
    if ((mFlags & 8) == 0) {
        int b = (data_0209f2d8 == 1);
        if (b == 0)
            return;
    }
    func_ov102_0214c0b8();
    mFlags |= 0x10000001;
    mShouldRender = 1;
}

/* ==========================================================================
 * ======================================================================== */

void daBmb_c::func_ov102_0214b988() {
    daPly_c *pl;
    struct Vector3 v;

    mChasePlayer = 0;
    pl = ClosestPlayer();
    if (!pl) return;
    if (Vec3_Dist(&mPos, &pl->mPos) > 0x190000)
        return;
    {
        int *src = (int *)&pl->mPosX;
        v.x = src[0];
        v.y = src[1];
        v.z = src[2];
    }
    if (AngleDiff(Vec3_HorzAngle(&mPos, &v), mAngleY) >= mNoticeAngle)
        return;
    mChasePlayer = pl;
    func_ov102_0214b384(this, 0x96);
}

/* ==========================================================================
 *
 * The volatile pins are load-bearing: they hold the cartridge's stack layout.
 * Do not simplify them.  The shard's chained `& 0xFFFFFFFFu` masks were
 * C-only and were dropped; the plain `&&` MATCHES.
 * ======================================================================== */

void daBmb_c::func_ov102_0214b53c() {
    extern Matrix4x3 data_020a0e68;
    extern s16 data_02082214[];

    volatile u16 tmp[6]; /* angle-home pins */
    Vector3 p;
    volatile Vector3 dead; /* stack-layout placeholder; never a live value */
    int x0;
    Vector3 d;
    int xSaved;
    char *pPos;
    char *src;
    /* Plain short-circuit.  A chain of `& 0xFFFFFFFFu` masks is C-only: under
     -lang c++ the `&&` is a bool and the mask costs three instructions. */
    if ((mVariant == 2) && ((src = (char *)mCarryMatrix) != 0)) {
        mAnmModel.mat4x3 = *((Matrix4x3 *)src);
    } else {
        int haveMtx = (int)((mFlags & 0x4000) != 0);
        daPly_c *player0;
        if (((haveMtx != 0) && ((player0 = mCarrier) != 0)) && (player0->mCarryMatrix != 0)) {
            char *ret = (char *)func_ov002_020e496c(player0);
            u8 idx = 0;
            if (mCarrier->IsFrontSliding() != 0) {
                idx = 1;
            }
            if ((mCarrier->LostGrabbedObject() != 0) &&
                (((u32)(((u32)(((dExtAnmModel_c *)ret)->currFrame << 4)) >> 0x10)) < 0xe)) {
                idx = 1;
            }
            if (mCarrier->param1 == 2) {
                idx = (u8)(idx + 2);
            }
            Math_Function_0203b14c(mCarryOff, gBmbCarryOffsets[idx].x, 0x800, 0x3e8000, 4);
            Math_Function_0203b14c(&mCarryOff[1], gBmbCarryOffsets[idx].y, 0x800, 0x3e8000, 4);
            Math_Function_0203b14c(&mCarryOff[2], gBmbCarryOffsets[idx].z, 0x800, 0x3e8000, 4);
            Matrix4x3 *rmtx = UpdateCarry(*mCarrier, *(Vector3 *)mCarryOff);
            mAnmModel.mat4x3 = *rmtx;
        } else {
            mCarryOff[0] = 0;
            mCarryOff[1] = 0;
            mCarryOff[2] = 0;
            Matrix4x3_FromRotationY(&mAnmModel.mat4x3, mAngleY);
            mAnmModel.mat4x3.m[9] = mPosX >> 3;
            mAnmModel.mat4x3.m[10] = mPosY >> 3;
            mAnmModel.mat4x3.m[11] = mPosZ >> 3;
        }
    }
    int haveMtx2 = (int)((mFlags & 0x4000) != 0);
    if ((haveMtx2 != 0) || (mFuse != 0)) {
        {
            Matrix4x3 *g = &data_020a0e68;
            Matrix4x3 *m = &mAnmModel.mat4x3;
            p.x = 0;
            p.y = 0;
            p.z = 0;
            *g = *m;
        }
        MulMat4x3Mat4x3(&mAnmModel.data.transforms[2], &data_020a0e68, &data_020a0e68);
        {
            int ty = *((int *)(((char *)(&data_020a0e68)) + 0x28));
            int tx = *((int *)(((char *)(&data_020a0e68)) + 0x24));
            p.y = ty;
            int tz = *((int *)(((char *)(&data_020a0e68)) + 0x2c));
            p.z = tz;
            p.x = tx;
            Vec3_Lsl(&d, &p, 3);
        }
        {
            int dy = *((int *)(((char *)(&d)) + 4));
            int dx = *((int *)(((char *)(&d)) + 0));
            x0 = dx;
            p.y = dy;
            int dz = *((int *)(((char *)(&d)) + 8));
            p.x = x0;
            p.z = dz;
        }
        int haveMtx3 = (int)((mFlags & 0x4000) != 0);
        if (haveMtx3 != 0) {
            mCarryParticle = Particle::System::New(mCarryParticle, 0x13, p.x, p.y, p.z, 0, 0);
        }
        if (mFuse != 0) {
            int haveShadow = (int)((mFlags & 0x40000) != 0);
            if (haveShadow != 0) {
                char *obj = (char *)mEatingPlayer;
                if (obj != 0) {
                    char *pos = (char *)&((daPly_c *)obj)->mPos;
                    int dx = *((int *)pos);
                    int gbase = (int)&data_020a0e68;
                    dead.x = dx;
                    int dy = *((int *)(pos + 4));
                    dead.y = dy;
                    int dz = *((int *)(pos + 8));
                    dead.z = dz;
                    data_020a0e68 = mEatingPlayer->mHeadMatrix;
                    {
                        /* pin mtx translation loads */
                        int tx = *(volatile int *)((char *)&data_020a0e68 + 0x24);
                        int ty = *(volatile int *)((char *)&data_020a0e68 + 0x28);
                        int tz = *(volatile int *)((char *)&data_020a0e68 + 0x2c);
                        p.x = tx;
                        p.z = tz;
                        pPos = (char *)&p;
                        p.y = ty;
                        Vec3_LslInPlace((int *)pPos, 3);
                    }
                    daPly_c *eater = mEatingPlayer;
                    u16 a8c = *((volatile u16 *)&eater->mAngleX); /* pin angle loads */
                    u16 a8e = *((volatile u16 *)&eater->mAngleY);
                    tmp[1] = a8e;
                    tmp[0] = a8c;
                    xSaved = p.x;
                    x0 = *((volatile u16 *)&eater->mAngleZ);
                    s16 angY = (s16)tmp[1];
                    tmp[2] = (u16)x0;
                    u16 v580 = *((volatile u16 *)&eater->mHeadBoneRot[0]);
                    tmp[4] = eater->mHeadBoneRot[1];
                    tmp[3] = v580;
                    s16 v582s = (s16)tmp[4];
                    tmp[5] = eater->mHeadBoneRot[2];
                    int diff = (angY + 0x1800) - v582s;
                    tmp[1] = (u16)diff;
                    {
                        u16 ang8e = tmp[1];
                        u16 v582 = (u16)v582s;
                        int py = p.y + 0x14000;
                        p.y = py;
                        p.y = py + (((int)data_02082214[(v582 >> 4) * 2]) * 0x1e);
                        int idxX2 = (ang8e >> 4) * 2;
                        p.x = xSaved + (((int)data_02082214[idxX2]) * 0x3c);
                        p.z = p.z + (((int)data_02082214[idxX2 + 1]) * 0x3c);
                    }
                }
            }
            mFuseParticle = Particle::System::New(mFuseParticle, 0x19, p.x, p.y, p.z, 0, 0);
        }
    }
    UpdateShadow();
}

/* ==========================================================================
 * ======================================================================== */

void daBmb_c::UpdateShadow() {
    int v;
    int b = (mFlags & 0x40000) != 0;
    if (b)
        return;

    v = mPosY;
    if (mWithMeshClsn.IsOnGround() == 0) {
        dBgCh_Gnd rg;
        rg.SetObjAndPos(mPos, (dActor_c *)0);
        if (rg.DetectClsn() != 0)
            v = rg.clsnY;
    }

    {
        int b2 = (mFlags & 0x4000) != 0;
        if (b2) {
            daPly_c *p = mCarrier;
            if (p != 0) {
                if (*(int *)((char *)&p->mClipResult + 4) != 0)
                    v = p->mPosY;
            }
        }
    }

    mMatrix[9] = mPosX >> 3;
    mMatrix[10] = v >> 3;
    mMatrix[11] = mPosZ >> 3;

    DropShadowRadHeight(mShadowModel, *(Matrix4x3 *)mMatrix, mScaleX * 0x50, 0x1e000, 0xf);
}

/* ==========================================================================
 * ======================================================================== */

extern "C" {

void func_ov102_0214b3f0(void *c, int a1) {
    extern void func_ov102_0214bd20(void *c);
    daBmb_c *self = (daBmb_c *)c;

    unsigned int v = self->mState - 3;
    if (v <= 1u) return;
    unsigned short h = self->mFuse;
    if (h > 4u) goto do_call;
    if (h != 0u) return;
do_call:
    self->mCarrier = (daPly_c *)a1;
    func_ov102_0214bd20(c);
}

}

/* ==========================================================================
 * ======================================================================== */

extern "C" {

void func_ov102_0214b3b8(void *c)
{
    extern void func_ov102_0214bc20(void *c);
    daBmb_c *self = (daBmb_c *)c;

    if (self->mState != 3) return;
    func_ov102_0214bc20(c);
    self->mPrevAngleY = self->mCarrier->mAngleY;
    self->mCarrier = 0;
}

}

/* ==========================================================================
 *
 * CALLED FROM OUTSIDE THIS TU: ov078 0x0212519c, and ov102's own
 * daObjHatenaBlock_c run at 0x02149278.  It keeps external linkage.
 * ======================================================================== */

extern "C" {

void func_ov102_0214b384(void *arg0, unsigned int arg1) {
    daBmb_c *self = (daBmb_c *)arg0;
    unsigned int cur = self->mFuse;
    if (cur == 0 || cur > arg1) {
        self->mFuse = (unsigned short)arg1;
    }
    self->mFlags &= ~1u;
}

}

/* ==========================================================================
 *
 * The fuse.  Each frame it counts mFuse down and keeps the fuse-tick sound
 * going.  Over the last four counts it swells the body, the hit cylinder and
 * the mesh-collision size; at 1 it detonates and returns 0.  With no fuse lit it
 * hands the frame to the hit reactions (func_ov102_0214b128).
 * ======================================================================== */

int daBmb_c::UpdateFuse() {
    if (DecIfAbove0_Short(&mFuse)) {
        mFuseSound = Sound::PlayLong(mFuseSound, 3, 0x188, mCamSpacePos, 0);
    }

    {
        unsigned short st = mFuse;
        if (st != 0 && st <= 4) {
            mdCc_c.flags &= ~0x8000;
            if (mFuse == 1) {
                func_ov102_0214ae1c(this);
                return 0;
            }
            {
                int v = ((5 - mFuse) << 12) / 4 + 0x1000;
                mScaleZ = v;
                mScaleY = mScaleZ;
                mScaleX = mScaleY;
                mdCc_c.radius = v * 0x3c;
                mdCc_c.height = v * 0x50;
                func_0203568c((int *)&mWithMeshClsn, v * 0x3c);
                func_02035684((int *)&mWithMeshClsn, v * 0x3c);
                func_ov002_020ef228(&mWithMeshClsn, (int)this);
            }
            {
                mdCc_c.flags &= ~4;
                if (mFuse == 2) {
                    mdCc_c.flags |= 0x4000;
                }
            }
        } else {
            func_ov102_0214b128();
        }
    }
    return 1;
}

/* ==========================================================================
 * ======================================================================== */

void daBmb_c::func_ov102_0214b128() {
    int t;
    if (FindEgg(mdCc_c) || (mdCc_c.hitFlags & 0x20000)) {
        mFuse = 4;
        mFlags &= ~1;
        mHorzSpeed = 0;
        return;
    }
    t = (int)func_02010304(&mdCc_c);
    if (t) {
        func_ov102_0214b3f0(this, t);
        return;
    }
    t = mdCc_c.hitFlags;
    if (t & 0x40000) {
        func_ov102_0214b384(this, 4);
        return;
    }
    if (!(t & 0x380)) return;
    t = mState;
    if (t < 0) return;
    if (t > 2) return;
    {
        dActor_c *hitter = dActor_c::FindWithID(mdCc_c.otherOwner);
        if (!hitter) return;
        func_ov102_0214bc20((char *)this);
        mPrevAngleY = hitter->mAngleY;
        UpdatePos(&mdCc_c);
        mWithMeshClsn.ClearGroundFlag();
    }
}

/* ==========================================================================
 * ======================================================================== */

void daBmb_c::func_ov102_0214b03c() {
    if (mState < 2 && mAnmModel.file == (BCA_File *)gBmbWalkAnimFile.filePtr) {
        if (mAnmModel.WillHitFrame(0) != 0 || mAnmModel.WillHitFrame(0x10) != 0) {
            func_0201267c(0x132, &mCamSpacePos);
        }
    }
    switch (mState) {
    case 0:
        State0();
        return;
    case 1:
        State1();
        return;
    case 2:
        func_ov102_0214bd90();
        return;
    case 3:
        State3();
        return;
    case 4:
        State4();
        return;
    case 5:
        State5();
        return;
    }
}

/* ==========================================================================
 *
 * CALLED FROM OUTSIDE THIS TU: ov014 0x021122c0 and ov098 0x0213ad80.  It
 * keeps external linkage.
 * ======================================================================== */

extern "C" {

void func_ov102_0214ae1c(void *cv) {
    extern void d_a_bmbNewSimpleHold(unsigned int id, int x, int y, int z);

    daBmb_c *self = (daBmb_c *)cv;
    Vector3 v;
    int y, z, w;
    self->mdCc_c.flags &= ~0x8000;
    y = self->mPosY;
    z = self->mPosZ;
    w = y + 0x78000;
    v.x = self->mPosX;
    v.y = w;
    v.z = z;
    if (self->mCoinType) {
        dActor_c::Spawn(0x120, 2, v, 0, self->mAreaId, -1);
        self->mCoinType = 0;
    }
    func_0201267c(0x10c, &self->mCamSpacePos);
    v.y = self->mPosY + 0x64000;
    Particle::System::NewSimple(0x14, v.x, v.y, v.z);
    Particle::System::NewSimple(0x15, v.x, v.y, v.z);
    Particle::System::NewSimple(0x16, v.x, v.y, v.z);
    Particle::System::NewSimple(0x17, v.x, v.y, v.z);
    Particle::System::NewSimple(0x18, v.x, v.y, v.z);
    func_ov002_020ef228(&self->mWithMeshClsn, (int)self);
    if (self->mCarrier) {
        Vector3 w;
        self->mCarrier->DropActor();
        w.x = self->mPosX;
        w.y = self->mPosY;
        w.z = self->mPosZ;
        self->mCarrier->Hurt(w, 2, 0xc000, 1, 0, 1);
        self->mCarrier = 0;
    } else if (self->mdCc_c.otherOwner) {
        dActor_c *a = dActor_c::FindWithID(self->mdCc_c.otherOwner);
        if (a) {
            if (self->mdCc_c.hitFlags & 0x400000) {
                Vector3 w;
                w.x = self->mPosX;
                w.y = self->mPosY;
                w.z = self->mPosZ;
                ((daPly_c *)a)->Hurt(w, self->mHurtDamage, 0xc000, 1, 0, 1);
            }
        }
    }
    if (self->mEatingPlayer) {
        Vector3 w;
        w.x = self->mPosX;
        w.y = self->mPosY;
        w.z = self->mPosZ;
        self->mEatingPlayer->Hurt(w, self->mHurtDamage, 0xc000, 1, 0, 1);
        self->mEatingPlayer->func_ov002_020d718c();
        self->mEatingPlayer = 0;
    }
    func_ov102_0214baa0((char *)self);
}

}

/* ==========================================================================
 * Vtable slot 19.
 * ======================================================================== */

void daBmb_c::OnTurnIntoEgg(daPly_c &player) {
    if (mCoinType == 1) {
        Sound::PlayBank3(0x11, mCamSpacePos);
        GiveCoins(player.mPlayerNo, 1);
        player.Heal(0x100);
    }
    func_ov102_0214baa0((char *)this);
}

/* ==========================================================================
 * ======================================================================== */

void daBmb_c::func_ov102_0214ad40() {
    if ((unsigned char)(mVariant + 0xfe) > 1)
        return;
    if (mPosY >= mHomePosY - 0x12c000)
        return;
    {
        unsigned short v = mFuse;
        if (v > 4)
            ;
        else if (v != 0)
            return;
    }
    func_ov102_0214b384(this, 4);
    mVariant = 3;
    mdCc_c.flags &= ~2;
    func_ov102_0214c0b8();
}

/* ==========================================================================
 *
 * CALLED FROM OUTSIDE THIS TU: ov002 0x020f1740, ov078 0x02125284, and
 * ov102's own daObjHatenaBlock_c run at 0x0214926c.  It keeps external linkage.
 * ======================================================================== */

extern "C" {

void func_ov102_0214ad14(void *c)
{
    daBmb_c *self = (daBmb_c *)c;

    func_ov102_0214b384(c, 0x96);
    self->mChasePlayer = self->ClosestPlayer();
    self->func_ov102_0214c0b8();
}

}

/* ==========================================================================
 * ======================================================================== */

int daBmb_c::func_ov102_0214ab1c() {
    daPly_c *player;

    int eatState = UpdateYoshiEat(mWithMeshClsn);
    if (eatState == 0) {
        goto ret0;
    }

    mFlags &= ~0x10000000u;

    player = mEatingPlayer;
    if (player != 0) {
        if (player->IsInsideOfCannon() != 0) {
            goto shocked;
        }
        if (player->IsBeingShotOutOfCannon() != 0) {
            goto shocked;
        }
        if (UpdateFuse() == 0) {
            mFlags &= ~0xe0000u;
            mEatenByYoshi = 0;
            return 1;
        }
        goto after_cannon;
    shocked:
        func_ov102_0214b384(this, 3);
    }
after_cannon:

    if (mVariant == 2) {
        mVariant = 3;
        mState = 0;
        mAnmModel.SetAnim((BCA_File *)gBmbWalkAnimFile.filePtr, 0, 0x1000, 0);
        mTurnTimer = 0x200;
        mVertAccel = -0x2000;
        mdCc_c.flags &= ~2u;
    }

    if (eatState == 2) {
        func_ov102_0214b384(this, 0x50);
    }

    if (eatState == 3) {
        if (mWithMeshClsn.IsOnGround() != 0) {
            unk_3f4 = 3;
        }
    }

    if (mEatenByYoshi != 0) {
        if (mSpitOutTimer == 5) {
            mdCc_c.flags &= ~0x8000u;
        }
    }

    if (SpawnParticlesIfHitOtherObj(mdCc_c) != 0) {
        goto hit;
    }
    if (eatState != 3 || mWithMeshClsn.IsOnGround() == 0) {
        goto skip_hit;
    }

hit:
    func_ov102_0214b384(this, 4);
    mEatenByYoshi = 0;
    return 1;

skip_hit:
    mdCc_c.Clear();
    if (mEatenByYoshi != 0) {
        if (mSpitOutTimer == 0) {
            mdCc_c.dCc_c::Update();
        }
    }
    func_ov102_0214b53c();
    return 1;

ret0:
    return 0;
}

/* ==========================================================================
 * ======================================================================== */

int daBmb_c::func_ov102_0214aa18() {
    if (mVariant == 2) {
        if (*(int *)((char *)&mClipResult + 4) != 0) {
            func_ov102_0214b53c();
            return 1;
        }
        UpdatePos(&mdCc_c);
        UpdateWMClsn(mWithMeshClsn, 0);
        func_ov102_0214b53c();
        func_ov102_0214ad40();
        mdCc_c.Clear();
        mdCc_c.dCc_c::Update();
        if (mWithMeshClsn.JustHitGround()) {
            Vector3 v;
            v.x = mPosX;
            v.y = mPosY;
            v.z = mPosZ;
            func_0200fc44((int)this, &v, 1);
        }
        if (mWithMeshClsn.IsOnGround() == 0)
            return 1;
        mdCc_c.flags &= ~2;
        mVariant = 3;
        func_ov102_0214c0b8();
        return 1;
    }
    return 0;
}

/* ==========================================================================
 * Vtable slot 29.
 *
 * Below this, at 0x0214a9b4 and 0x0214a96c, the compiler emits D0 and D1 from
 * the header's inline destructor body.  Nothing is written for them here.
 * ======================================================================== */

s32 daBmb_c::OnAimedAtWithEgg() {
    return 0x32000;
}
