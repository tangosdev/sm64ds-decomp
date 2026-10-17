//cpp
/* Bob-omb (BOB_OMB 206 / BOMBHEI) -- ov102/daBmb_c.
 *
 * common.h FIRST. InitResources and the carry update copy twelve matrix words.
 * math/Matrix.h's nested Matrix4x3 scalarizes that copy (Vector3 is non-POD).
 *
 * Leftovers, remeasured in this file:
 * - ModelAnim::SetAnim, dCcAc_c::Init and DropShadowRadHeight: the header
 *   methods take Fix12<int> by value and the call grows. dBgCh_Actr::Init's
 *   header is Fix12i (mangles as i); this TU's call is Fix12<int>.
 *   The 4-arg KillByInvincibleChar stays a scalar extern; Behavior already
 *   calls the 3-arg method.
 * - Behavior skips 4 bytes past GetFloorResult's dBgPi to the SurfaceInfo
 *   and tests CLPS bit 0x20 (water).
 * - States 0, 1 and 3 advance the dExtFrameCtrl_c base of mModelAnim
 *   explicitly. The qualified base call preserves the ROM entry point
 *   without a byte-offset pointer or a ModelAnim forwarding call.
 * - The word after mClipResult (dActor_c pad, 0xc8) is a Matrix4x3* while
 *   mVariant is 2. Naming it would be a field on dActor_c. Player+0xc8 is
 *   that same pad on the carrier, tested as a nonzero int.
 * - func_ov102_0214bf64 keeps its existing free-function label over daBmb_c.
 *   func_ov102_0214bd90 and func_ov102_0214b03c are methods:
 *   the first argument is this Bob-omb. Both methods call func_0200fc44 and
 *   func_0201267c through the file-scope extern "C" prototypes: a block-scope
 *   extern inside a member gets C++ linkage and the module stops linking.
 * - func_ov102_0214ad14, 0214ae1c and 0214b384 are called from other overlays,
 *   so those labels stay. data_ov102_* are the file handles; SharedFilePtr
 *   has no fields, so the loaded BCA is still the word at +4.
 * - func_0203568c / func_02035684 are the out-of-line stores of
 *   mWithMeshClsn.mRadius / mHeight. Inlining them drops the call.
 * - func_ov102_0214b53c keeps the volatile stack pins, and the carrier's
 *   angles at +0x8c/+0x8e/+0x90, the matrix at +0x5ec, and the halfwords at
 *   +0x580/+0x582/+0x584. Those player slots are not named on Player.
 * - unk_3e0 and unk_3f6 stay. ov078/daBombking_c writes them by those names.
 */

#include "common.h"
#include "daBmb_c.h"
#include "SpawnInfo.h"
#include "Player.h"
#include "Sound.h"
#include "SharedFilePtr.h"
#include "dBgCh_Gnd.h"
#include "dBgCh_Actr.h"

bool ApproachLinear(short &value, short target, short step);

/* The three shared files this actor claims.  InitResources loads them and
   CleanupResources releases them, so both C++-named members need one agreed
   spelling; the members below that read the loaded handle out of the same bytes
   take their own view with a cast rather than a second declaration. */
extern SharedFilePtr data_ov102_0214e9c0;
extern SharedFilePtr data_ov102_0214e9c8;
extern SharedFilePtr data_ov002_0210d9e0;

extern Matrix4x3 IDENTITY_MATRIX4X3;

/* --------------------------------------------------------------------------
 * The one file-scope extern "C" region.  Everything here is reached from a
 * C++-named member, which cannot declare it in its own body.
 * ------------------------------------------------------------------------ */
extern "C" {

/* -- this TU's own members, forward-declared because mwcc lays .text down in
      reverse source order and every one of these calls is a forward reference. */
int   func_ov102_0214aa18(void *self);
int   func_ov102_0214ab1c(void *self);
void  func_ov102_0214ad40(void *self);
void  func_ov102_0214ae1c(void *self);
void  func_ov102_0214b128(void *self);
int   func_ov102_0214b248(void *self);
void  func_ov102_0214b384(void *self, unsigned int level);
void  func_ov102_0214b53c(char *self);
void  func_ov102_0214baa0(char *self);
void  func_ov102_0214beb4(void *self);
void  func_ov102_0214c0b8(void *self);

/* -- other modules -- */
void  GiveCoins(int who, int count);
int   SurfaceInfo_TestFlag0x20(int *si);

void  _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(void *, void *, void *, unsigned int);



/* dCcAc_c::Init / dBgCh_Actr::Init stay scalar: they carry Fix12<int> BY
   VALUE (6az). dBgCh_Actr::Init's header is Fix12i, which mangles as i;
   ROM is Fix12<int>. */
void  _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *, dActor_c *a, Fix12i r, Fix12i h, unsigned int d, unsigned int e);
void  _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *, dActor_c *a, Fix12i b, Fix12i c, Vector3_16 *d, Vector3_16 *e);

/* Hoisted out of four member bodies.  A class member function may not sit in a
   block-scope linkage specification, so daBmb_c::State1/3/4/5 cannot carry their
   own extern "C" declarations the way the free members below do -- these have to
   be file-scope, and they are spelled to agree with every other user in the TU. */
void  func_ov102_0214b3b8(void *c);
/* Real definitions: func_0201267c and func_0200fc44 (arm9).
   The methods call through these so the names stay unmangled. aa18, a free
   function, still redeclares func_0200fc44 at block scope. */
void  func_0201267c(unsigned int id, const Vector3 *v);
int   func_0200fc44(int a, Vector3 *pos, int flag);

extern signed char   data_0209f2f8;
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
 * `_ZTV7daBmb_c + 2` because this TU defines the vtable. The retained leaf operator new
 * forwards to the retail `_ZN7fBase_cnwEj` allocator, as does fBase_c's adapter.
 *
 * Reconstructed source-style name: SM64DS proves daBmb_c through RTTI,
 * allocation size, vtable identity, and the BOMBHEI registry profile; later EAD
 * lineage supplies classInit.  Exact original spelling is not preserved.
 * Historical alias: BobOmb_Spawn.
 * ======================================================================== */

extern "C" {

// @symbol daBmb_c_classInit
daBmb_c *daBmb_c_classInit(void)
{
    return new daBmb_c();
}

}

// @symbol g_profile_BOMBHEI
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

// @symbol _ZN7daBmb_c13OnYoshiTryEatEv
s32 daBmb_c::OnYoshiTryEat() {
    /* 263 == 0x107, dEnemyBase_c's own byte at that offset. */
    return mEatenByYoshi == 0;
}

/* ==========================================================================
 * Vtable slot 0.
 *
 * param1's low three bits pick the variant: 2 is the one that starts inert --
 * it sets the collision volume's `hit` bit, clears a behaviour flag and leaves
 * unk_108 clear -- 4 also starts clear, and everything else starts live.
 * ======================================================================== */

// @symbol _ZN7daBmb_c13InitResourcesEv
int daBmb_c::InitResources()
{
    BMD_File* bmd;

    dExtFrameCtrl_c::LoadFile(data_ov102_0214e9c0);
    dExtFrameCtrl_c::LoadFile(data_ov102_0214e9c8);
    bmd = (BMD_File*)Model::LoadFile(data_ov002_0210d9e0);
    if (mModelAnim.SetFile(bmd, 1, -1) == 0)
        return 0;
    if (mShadowModel.InitCylinder() == 0)
        return 0;

    mVariant = (unsigned char)(param1 & 7);
    mNoticeAngle = 0x2000;
    func_ov102_0214c0b8(this);
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCc_c, this, 0x3c000, 0x50000, 0x200004, 0xa6d380);
    mTerminalVelocity = -0x37000;

    if (mVariant == 2) {
        /* Variant 2 starts inert: set the cylinder's flags bit 2 and clear
           mFlags bit 0 (the profile clip-enable bit). */
        mdCc_c.flags |= 2;
        mFlags &= ~1u;
        unk_108 = 0;
    } else if (mVariant == 4) {
        unk_108 = 0;
    } else {
        unk_108 = 1;
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
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x32000, 0x32000, 0, 0);
    mWithMeshClsn.StartDetectingWater();
    mShouldRender = 1;
    /* Matrix4x3* while mVariant is 2. It lives in dActor_c's pad, the word
       after mClipResult. */
    *(int *)((char *)&mClipResult + 4) = 0;
    unk_3e0 = 2;
    unk_3f6 = 0;
    mHomeAngleY = mAngleY;
    return 1;
}

/* ==========================================================================
 * Vtable slot 6.
 * ======================================================================== */

// @symbol _ZN7daBmb_c8BehaviorEv
int daBmb_c::Behavior()
{
    int flag;
    int killResult;
    void *other;

    if (unk_3f6 != 0) {
        func_ov102_0214ae1c(this);
        return 1;
    }
    if (func_ov102_0214ab1c(this)) {
        return 1;
    }
    if (func_ov102_0214aa18(this)) {
        return 1;
    }

    killResult = UpdateKillByInvincibleChar(mWithMeshClsn, mModelAnim, 0);
    if (killResult != 0) {
        if (killResult == 2) {
            func_ov102_0214ae1c(this);
        }
        return 1;
    }

    if (mDeathState != 0) {
        UpdateDeath(mWithMeshClsn);
        func_ov102_0214b128(this);
        flag = mFlags & 0x100;
        flag = flag != 0;
        if (flag != false) {
            mDeathState = 0;
        } else if (mDeathState != 0) {
            func_ov102_0214b53c((char *)this);
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
                _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(this, v, other, 0x32000);
                UpdatePos(&mdCc_c);
                UpdateWMClsn(mWithMeshClsn, 0);
                mdCc_c.Clear();
                return 1;
            }

            UpdatePos(&mdCc_c);
            if (data_0209f2f8 == 6 && data_0209f220 == 3) {
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

        killResult = func_ov102_0214b248(this);
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

        func_ov102_0214b53c((char *)this);
        mdCc_c.Clear();
        mdCc_c.Update();
        func_ov102_0214ad40(this);
    }
    return 1;
}

/* ==========================================================================
 * Vtable slot 9.
 * ======================================================================== */

// @symbol _ZN7daBmb_c6RenderEv
int daBmb_c::Render()
{
    int result = 1;
    if (mShouldRender != 0) {
        int flags = mFlags;
        int b = (flags & 0x40000) != 0;
        if (!b) {
            mModelAnim.Render(&mScale);
        }
    }
    return result;
}

/* ==========================================================================
 * Vtable slot 3.  Releases the three shared files InitResources claimed and
 * touches no field of its own.
 * ======================================================================== */

// @symbol _ZN7daBmb_c16CleanupResourcesEv
int daBmb_c::CleanupResources()
{
    data_ov002_0210d9e0.Release();
    data_ov102_0214e9c0.Release();
    data_ov102_0214e9c8.Release();
    return 1;
}

/* ==========================================================================
 * ======================================================================== */

extern "C" {

// @symbol func_ov102_0214c0b8
void func_ov102_0214c0b8(void *cv)
{
    extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(char *anim, void *file, int a, int b, unsigned int u);
    daBmb_c *self = (daBmb_c *)cv;

    self->mState = 0;
    if (self->mVariant == 1)
        self->mHorzSpeed = 0;
    else
        self->mHorzSpeed = 0x5000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char *)&self->mModelAnim, *(void **)((char *)&data_ov102_0214e9c0 + 4), 0, 0x1000, 0);
    self->mTurnTimer = 0x200;
    self->mVertAccel = -0x2000;
    self->mCarrier = 0;
}

}

/* State 0: the dispatch passes this Bob-omb. Use its actual fields and
 * the ModelAnim frame-controller base. Scalar position members remain
 * borrowed Vector3 views; no owning Vector3 temporaries are introduced.
 * The existing external free-function label is retained. */

extern "C" {

// @symbol func_ov102_0214bf64
void func_ov102_0214bf64(void *ov)
{
    extern unsigned short DecIfAbove0_Short(unsigned short* p);
    extern void _Z14ApproachLinearRiii(int& v, int target, int step);
    extern short Vec3_HorzAngle(const Vector3* v0, const Vector3* v1);
    extern int Vec3_Dist(const Vector3* a, const Vector3* b);
    extern void func_ov102_0214b988(void* c);

    daBmb_c *o = (daBmb_c *)ov;

    DecIfAbove0_Short(&o->mTurnTimer);
    if (o->mVariant != 1) {
        if (o->mChasePlayer == 0) {
            _Z14ApproachLinearRiii(o->mHorzSpeed, 0x5000, 0x2aa);
            func_ov102_0214b988(o);
            if (o->mChasePlayer != 0) {
                o->mHorzSpeed = 0;
                o->mVertSpeed = 0x14000;
                o->mState = 2;
            } else {
                if (o->mVariant == 5) {
                    s16 ang = Vec3_HorzAngle(&o->mPos, (const Vector3 *)&o->mHomePosX);
                    ApproachLinear(o->mPrevAngleY, ang, 0x200);
                    o->mAngleY = o->mPrevAngleY;
                } else {
                    if (o->mTurnTimer == 0 ||
                        Vec3_Dist(&o->mPos, (const Vector3 *)&o->mHomePosX) >= 0x500000) {
                        o->mHorzSpeed = 0x5000;
                        func_ov102_0214beb4(o);
                    }
                }
            }
        } else {
            _Z14ApproachLinearRiii(o->mHorzSpeed, 0x10000, 0x1000);
            s16 target = Vec3_HorzAngle(&o->mPos, &o->mChasePlayer->mPos);
            if (o->mVariant == 3) {
                /* Player::mGrabbedByActor: stop turning toward a held player. */
                int b = (o->mChasePlayer->mGrabbedByActor != 0);
                if (b)
                    target = o->mPrevAngleY;
            }
            ApproachLinear(o->mPrevAngleY, target, 0x800);
            o->mAngleY = o->mPrevAngleY;
        }
        o->mModelAnim.speed = o->mHorzSpeed >> 3;
    }
    o->mModelAnim.dExtFrameCtrl_c::Advance();
}

}

/* ==========================================================================
 * ======================================================================== */

extern "C" {

// @symbol func_ov102_0214beb4
void func_ov102_0214beb4(void *cv)
{
    extern short Vec3_HorzAngle(const void* a, const void* b);
    extern int Vec3_Dist(const void* a, const void* b);
    extern int RandomIntInternal(int* seed);
    extern int data_0209e650;

    daBmb_c *self = (daBmb_c *)cv;

    self->mState = 1;
    self->mTurnCount += 1;
    Player *o = self->mChasePlayer;
    if (o == 0) {
        *(short *)&self->mTargetAngY = Vec3_HorzAngle(&self->mPosX, &self->mHomePosX);
        if (Vec3_Dist(&self->mPosX, &self->mHomePosX) >= 0x500000) return;
        unsigned int r = RandomIntInternal(&data_0209e650);
        *(short *)&self->mTargetAngY += (short)(r >> 0x10);
        return;
    }
    *(short *)&self->mTargetAngY = Vec3_HorzAngle(&self->mPosX, &o->mPosX);
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

// @symbol _ZN7daBmb_c6State1Ev
void daBmb_c::State1() {
    if (mChasePlayer == 0) {
        mModelAnim.speed = 0x800;
        ApproachLinear(mPrevAngleY, *(short *)&mTargetAngY, 0x400);
    } else {
        mModelAnim.speed = 0x1000;
        ApproachLinear(mPrevAngleY, *(short *)&mTargetAngY, 0x800);
    }
    mAngleY = mPrevAngleY;
    mModelAnim.dExtFrameCtrl_c::Advance();
    if (mState == 2) return;
    if (mPrevAngleY != *(short *)&mTargetAngY) return;
    func_ov102_0214c0b8(this);
}

/* ==========================================================================
 *
 * mState arm 2. The first argument is this Bob-omb. It calls func_0200fc44
 * through the file-scope three-argument prototype, the definition's own.
 * ======================================================================== */

// @symbol _ZN7daBmb_c19func_ov102_0214bd90Ev
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

// @symbol func_ov102_0214bd20
void func_ov102_0214bd20(char* c)
{
    extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* anim, void* file, int a, int b, unsigned int u);
    daBmb_c *self = (daBmb_c *)c;

    self->mState = 3;
    self->mVertAccel = 0;
    self->mHorzSpeed = 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim, *(void **)((char *)&data_ov102_0214e9c8 + 4), 0, 0x1000, 0);
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

// @symbol _ZN7daBmb_c6State3Ev
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
            func_ov102_0214c0b8(this);
        }
    }
    mModelAnim.dExtFrameCtrl_c::Advance();
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

// @symbol func_ov102_0214bc20
void func_ov102_0214bc20(char* c)
{
    extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(char *anim, void *file, int a, int b, unsigned int u);
    daBmb_c *self = (daBmb_c *)c;

    Quad a1 = data_ov102_0214e514;
    Quad a2 = data_ov102_0214e524;
    Player *p = self->mCarrier;
    int idx = 0;
    if (p != 0) {
        idx = p->param1;
        if ((unsigned)idx > 4) idx = 4;
    }
    self->mHorzSpeed = a1.v[idx];
    self->mVertSpeed = a2.v[idx];
    self->mState = 4;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char *)&self->mModelAnim, *(void **)((char *)&data_ov102_0214e9c0 + 4), 0, 0x1000, 0);
    self->mVertAccel = -0x2000;
}

}

/* ==========================================================================
 *
 * mState arm 4 (COINED NAME -- ov102 carries this address and no identifier;
 * func_ov102_0214b03c switching on mState is what proves the index): the thrown arm: unless the collision actor reports neither ground nor wall contact
 * it calls func_ov102_0214b384(4) and zeroes the forward speed.
 * ======================================================================== */

// @symbol _ZN7daBmb_c6State4Ev
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

// @symbol func_ov102_0214baa0
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

// @symbol _ZN7daBmb_c6State5Ev
void daBmb_c::State5()
{
    if (DistToCPlayer() < 0x7d0000)
        return;
    if ((mFlags & 8) == 0) {
        int b = (data_0209f2d8 == 1);
        if (b == 0)
            return;
    }
    func_ov102_0214c0b8(this);
    mFlags |= 0x10000001;
    mShouldRender = 1;
}

/* ==========================================================================
 * ======================================================================== */

extern "C" {

// @symbol func_ov102_0214b988
void func_ov102_0214b988(void *thiz)
{
    extern int Vec3_Dist(const struct Vector3 *a, const struct Vector3 *b);
    extern short Vec3_HorzAngle(const struct Vector3 *a, const struct Vector3 *b);
    extern int AngleDiff(int, int);

    daBmb_c *self = (daBmb_c *)thiz;
    Player *pl;
    struct Vector3 v;

    self->mChasePlayer = 0;
    pl = self->ClosestPlayer();
    if (!pl) return;
    if (Vec3_Dist(&self->mPos, &pl->mPos) > 0x190000)
        return;
    {
        int *src = (int *)&pl->mPosX;
        v.x = src[0];
        v.y = src[1];
        v.z = src[2];
    }
    if (AngleDiff(Vec3_HorzAngle(&self->mPos, &v), self->mAngleY) >= self->mNoticeAngle)
        return;
    self->mChasePlayer = pl;
    func_ov102_0214b384(self, 0x96);
}

}

/* ==========================================================================
 *
 * The volatile pins are load-bearing: they hold the cartridge's stack layout.
 * Do not simplify them.  The shard's chained `& 0xFFFFFFFFu` masks were
 * C-only and were dropped; the plain `&&` MATCHES.
 * ======================================================================== */

extern "C" {

// @symbol func_ov102_0214b53c
void func_ov102_0214b53c(char *c)
{
  extern void *func_ov002_020e496c(char *c);
  extern void Math_Function_0203b14c(void *out, int a1, int a2, int a3, int a4);
  extern void Matrix4x3_FromRotationY(void *m, int angle);
  extern void MulMat4x3Mat4x3(void *a, void *b, void *out);
  extern void Vec3_Lsl(Vector3 *d, Vector3 *s, int sh);
  extern void Vec3_LslInPlace(Vector3 *v, int sh);
  extern int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(int sys, unsigned int kind, int fx, int a1, int a2, const void *v, void *cb);
  extern void func_ov102_0214b444(void *c);
  extern int data_ov102_0214ea18[];
  extern int data_ov102_0214ea1c[];
  extern int data_ov102_0214ea20[];
  extern Matrix4x3 data_020a0e68;
  extern s16 data_02082214[];

  daBmb_c *self = (daBmb_c *)c;
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
  if ((self->mVariant == 2) && ((src = *((char **) ((char *)&self->mClipResult + 4))) != 0))
  {
    self->mModelAnim.mat4x3 = *((Matrix4x3 *) src);
  }
  else
  {
    int haveMtx = (int) ((self->mFlags & 0x4000) != 0);
    Player *player0;
    if (((haveMtx != 0) && ((player0 = self->mCarrier) != 0)) && ((*((int *) ((char *)&player0->mClipResult + 4))) != 0))
    {
      char *ret = (char *) func_ov002_020e496c((char *)player0);
      u8 idx = 0;
      if (self->mCarrier->IsFrontSliding() != 0)
      {
        idx = 1;
      }
      if ((self->mCarrier->LostGrabbedObject() != 0) && (((u32) (((u32) ((*((int *) (ret + 0x58))) << 4)) >> 0x10)) < 0xe))
      {
        idx = 1;
      }
      if (self->mCarrier->param1 == 2)
      {
        idx = (u8) (idx + 2);
      }
      int off = idx * 0xc;
      Math_Function_0203b14c(self->mCarryOff, *((int *) (((char *) data_ov102_0214ea18) + off)), 0x800, 0x3e8000, 4);
      Math_Function_0203b14c(&self->mCarryOff[1], *((int *) (((char *) data_ov102_0214ea1c) + off)), 0x800, 0x3e8000, 4);
      Math_Function_0203b14c(&self->mCarryOff[2], *((int *) (((char *) data_ov102_0214ea20) + off)), 0x800, 0x3e8000, 4);
      Matrix4x3 *rmtx = self->UpdateCarry(*self->mCarrier, *(Vector3 *)self->mCarryOff);
      self->mModelAnim.mat4x3 = *rmtx;
    }
    else
    {
      self->mCarryOff[0] = 0;
      self->mCarryOff[1] = 0;
      self->mCarryOff[2] = 0;
      Matrix4x3_FromRotationY(&self->mModelAnim.mat4x3, self->mAngleY);
      self->mModelAnim.mat4x3.m[9] = self->mPosX >> 3;
      self->mModelAnim.mat4x3.m[10] = self->mPosY >> 3;
      self->mModelAnim.mat4x3.m[11] = self->mPosZ >> 3;
    }
  }
  int haveMtx2 = (int) ((self->mFlags & 0x4000) != 0);
  if ((haveMtx2 != 0) || (self->mFuse != 0))
  {
    {
      Matrix4x3 *g = &data_020a0e68;
      Matrix4x3 *m = &self->mModelAnim.mat4x3;
      p.x = 0;
      p.y = 0;
      p.z = 0;
      *g = *m;
    }
    MulMat4x3Mat4x3((char *)self->mModelAnim.data.transforms + 0x60, &data_020a0e68, &data_020a0e68);
    {
      int ty = *((int *) (((char *) (&data_020a0e68)) + 0x28));
      int tx = *((int *) (((char *) (&data_020a0e68)) + 0x24));
      p.y = ty;
      int tz = *((int *) (((char *) (&data_020a0e68)) + 0x2c));
      p.z = tz;
      p.x = tx;
      Vec3_Lsl(&d, &p, 3);
    }
    {
      int dy = *((int *) (((char *) (&d)) + 4));
      int dx = *((int *) (((char *) (&d)) + 0));
      x0 = dx;
      p.y = dy;
      int dz = *((int *) (((char *) (&d)) + 8));
      p.x = x0;
      p.z = dz;
    }
    int haveMtx3 = (int) ((self->mFlags & 0x4000) != 0);
    if (haveMtx3 != 0)
    {
      self->mCarryParticle = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(self->mCarryParticle, 0x13, p.x, p.y, p.z, 0, 0);
    }
    if (self->mFuse != 0)
    {
      int haveShadow = (int) ((self->mFlags & 0x40000) != 0);
      if (haveShadow != 0)
      {
        char *obj = (char *)self->mEatingPlayer;
        if (obj != 0)
        {
          char *pos = obj + 0x5c;
          int dx = *((int *) pos);
          int gbase = (int) &data_020a0e68;
          dead.x = dx;
          int dy = *((int *) (pos + 4));
          int off5ec = 0x5ec;
          dead.y = dy;
          int dz = *((int *) (pos + 8));
          dead.z = dz;
          data_020a0e68 = *((Matrix4x3 *) ((char *)self->mEatingPlayer + off5ec));
          {
            int tx = *(volatile int *) ((char *) &data_020a0e68 + 0x24); /* pin mtx translation loads */
            int ty = *(volatile int *) ((char *) &data_020a0e68 + 0x28);
            int tz = *(volatile int *) ((char *) &data_020a0e68 + 0x2c);
            p.x = tx;
            p.z = tz;
            pPos = (char *) &p;
            p.y = ty;
            Vec3_LslInPlace((Vector3 *) pPos, 3);
          }
          char *p2 = (char *)self->mEatingPlayer;
          u16 a8c = *((volatile u16 *) (p2 + 0x8c)); /* pin angle loads */
          u16 a8e = *((volatile u16 *) (p2 + 0x8e));
          tmp[1] = a8e;
          tmp[0] = a8c;
          xSaved = p.x;
          x0 = *((volatile u16 *) (p2 + 0x90));
          s16 angY = (s16) tmp[1];
          tmp[2] = (u16) x0;
          u16 v580 = *((volatile u16 *) (p2 + 0x580));
          tmp[4] = *((u16 *) (p2 + 0x582));
          tmp[3] = v580;
          s16 v582s = (s16) tmp[4];
          tmp[5] = *((u16 *) (p2 + 0x584));
          int diff = (angY + 0x1800) - v582s;
          tmp[1] = (u16) diff;
          {
            u16 ang8e = tmp[1];
            u16 v582 = (u16) v582s;
            int py = p.y + 0x14000;
            p.y = py;
            p.y = py + (((int) data_02082214[(v582 >> 4) * 2]) * 0x1e);
            int idxX2 = (ang8e >> 4) * 2;
            p.x = xSaved + (((int) data_02082214[idxX2]) * 0x3c);
            p.z = p.z + (((int) data_02082214[idxX2 + 1]) * 0x3c);
          }
        }
      }
      self->mFuseParticle = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(self->mFuseParticle, 0x19, p.x, p.y, p.z, 0, 0);
    }
  }
  func_ov102_0214b444(c);
}

}

/* ==========================================================================
 * ======================================================================== */

extern "C" {

// @symbol func_ov102_0214b444
void func_ov102_0214b444(void *cv)
{
    extern void _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(
        int self, int sm, int mat, int fix, int t, unsigned int j);

    daBmb_c *self = (daBmb_c *)cv;
    int c = (int)self;
    int v;
    int b = (self->mFlags & 0x40000) != 0;
    if (b)
        return;

    v = self->mPosY;
    if (self->mWithMeshClsn.IsOnGround() == 0) {
        dBgCh_Gnd rg;
        rg.SetObjAndPos(self->mPos, (dActor_c *)0);
        if (rg.DetectClsn() != 0)
            v = rg.clsnY;
    }

    {
        int b2 = (self->mFlags & 0x4000) != 0;
        if (b2) {
            Player *p = self->mCarrier;
            if (p != 0) {
                if (*(int *)((char *)&p->mClipResult + 4) != 0)
                    v = p->mPosY;
            }
        }
    }

    self->mMatrix[9] = self->mPosX >> 3;
    self->mMatrix[10] = v >> 3;
    self->mMatrix[11] = self->mPosZ >> 3;

    _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(
        c, (int)&self->mShadowModel, (int)self->mMatrix, self->mScaleX * 0x50, 0x1e000, 0xf);
}

}

/* ==========================================================================
 * ======================================================================== */

extern "C" {

// @symbol func_ov102_0214b3f0
void func_ov102_0214b3f0(void *c, int a1) {
    extern void func_ov102_0214bd20(void *c);
    daBmb_c *self = (daBmb_c *)c;

    unsigned int v = self->mState - 3;
    if (v <= 1u) return;
    unsigned short h = self->mFuse;
    if (h > 4u) goto do_call;
    if (h != 0u) return;
do_call:
    self->mCarrier = (Player *)a1;
    func_ov102_0214bd20(c);
}

}

/* ==========================================================================
 * ======================================================================== */

extern "C" {

// @symbol func_ov102_0214b3b8
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

// @symbol func_ov102_0214b384
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
 * ======================================================================== */

extern "C" {

// @symbol func_ov102_0214b248
int func_ov102_0214b248(void *cv)
{
    extern unsigned short DecIfAbove0_Short(unsigned short *p);
    extern void func_0203568c(int *p, int v);
    extern void func_02035684(int *p, int v);
    extern void func_ov002_020ef228(void *c, int arg);

    daBmb_c *self = (daBmb_c *)cv;

    if (DecIfAbove0_Short(&self->mFuse)) {
        self->mFuseSound = Sound::PlayLong(self->mFuseSound, 3, 0x188, self->mCamSpacePos, 0);
    }

    {
        unsigned short st = self->mFuse;
        if (st != 0 && st <= 4) {
            self->mdCc_c.flags &= ~0x8000;
            if (self->mFuse == 1) {
                func_ov102_0214ae1c(self);
                return 0;
            }
            {
                int v = ((5 - self->mFuse) << 12) / 4 + 0x1000;
                self->mScaleZ = v;
                self->mScaleY = self->mScaleZ;
                self->mScaleX = self->mScaleY;
                self->mdCc_c.radius = v * 0x3c;
                self->mdCc_c.height = v * 0x50;
                func_0203568c((int *)&self->mWithMeshClsn, v * 0x3c);
                func_02035684((int *)&self->mWithMeshClsn, v * 0x3c);
                func_ov002_020ef228(&self->mWithMeshClsn, (int)self);
            }
            {
                self->mdCc_c.flags &= ~4;
                if (self->mFuse == 2) {
                    self->mdCc_c.flags |= 0x4000;
                }
            }
        } else {
            func_ov102_0214b128(self);
        }
    }
    return 1;
}

}

/* ==========================================================================
 * ======================================================================== */

extern "C" {

// @symbol func_ov102_0214b128
void func_ov102_0214b128(void *cv) {
    extern int func_02010304(void* target, char* p);
    extern void func_ov102_0214b3f0(void* c, int a1);
    extern void func_ov102_0214bc20(void* c);

    daBmb_c *self = (daBmb_c *)cv;
    int t;
    if (self->FindEgg(self->mdCc_c) || (self->mdCc_c.hitFlags & 0x20000)) {
        self->mFuse = 4;
        self->mFlags &= ~1;
        self->mHorzSpeed = 0;
        return;
    }
    t = func_02010304(self, (char *)&self->mdCc_c);
    if (t) {
        func_ov102_0214b3f0(self, t);
        return;
    }
    t = self->mdCc_c.hitFlags;
    if (t & 0x40000) {
        func_ov102_0214b384(self, 4);
        return;
    }
    if (!(t & 0x380)) return;
    t = self->mState;
    if (t < 0) return;
    if (t > 2) return;
    {
        dActor_c *hitter = dActor_c::FindWithID(self->mdCc_c.otherOwner);
        if (!hitter) return;
        func_ov102_0214bc20(self);
        self->mPrevAngleY = hitter->mAngleY;
        self->UpdatePos(&self->mdCc_c);
        self->mWithMeshClsn.ClearGroundFlag();
    }
}

}

/* ==========================================================================
 * ======================================================================== */

// @symbol _ZN7daBmb_c19func_ov102_0214b03cEv
void daBmb_c::func_ov102_0214b03c() {
    if (mState < 2 && mModelAnim.file == *((BCA_File **)((char *)&data_ov102_0214e9c0 + 4))) {
        if (((dExtFrameCtrl_c *)((char *)this + 0x350))->WillHitFrame(0) != 0 ||
            ((dExtFrameCtrl_c *)((char *)this + 0x350))->WillHitFrame(0x10) != 0) {
            func_0201267c(0x132, &mCamSpacePos);
        }
    }
    switch (mState) {
    case 0:
        func_ov102_0214bf64(this);
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

// @symbol func_ov102_0214ae1c
void func_ov102_0214ae1c(void *cv) {
    typedef struct Bmb_Ae1cVec3 { int x, y, z; } Vec3;
    extern void func_0201267c(int a, void *b);
    extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
    extern void func_ov002_020ef228(void *a, void *b);
    extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *self, Vec3 *v, unsigned int a, int fix, unsigned int b, unsigned int d, unsigned int e);

    daBmb_c *self = (daBmb_c *)cv;
    Vec3 v;
    int y, z, w;
    self->mdCc_c.flags &= ~0x8000;
    y = self->mPosY;
    z = self->mPosZ;
    w = y + 0x78000;
    v.x = self->mPosX;
    v.y = w;
    v.z = z;
    if (self->unk_108) {
        dActor_c::Spawn(0x120, 2, *(Vector3 *)&v, 0, self->mAreaId, -1);
        self->unk_108 = 0;
    }
    func_0201267c(0x10c, &self->mCamSpacePosX);
    v.y = self->mPosY + 0x64000;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x14, v.x, v.y, v.z);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x15, v.x, v.y, v.z);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x16, v.x, v.y, v.z);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x17, v.x, v.y, v.z);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x18, v.x, v.y, v.z);
    func_ov002_020ef228(&self->mWithMeshClsn, self);
    if (self->mCarrier) {
        Vec3 w;
        self->mCarrier->DropActor();
        w.x = self->mPosX;
        w.y = self->mPosY;
        w.z = self->mPosZ;
        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(self->mCarrier, &w, 2, 0xc000, 1, 0, 1);
        self->mCarrier = 0;
    } else if (self->mdCc_c.otherOwner) {
        dActor_c *a = dActor_c::FindWithID(self->mdCc_c.otherOwner);
        if (a) {
            if (self->mdCc_c.hitFlags & 0x400000) {
                Vec3 w;
                w.x = self->mPosX;
                w.y = self->mPosY;
                w.z = self->mPosZ;
                _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(a, &w, self->unk_3e0, 0xc000, 1, 0, 1);
            }
        }
    }
    if (self->mEatingPlayer) {
        Vec3 w;
        w.x = self->mPosX;
        w.y = self->mPosY;
        w.z = self->mPosZ;
        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj((Player *)self->mEatingPlayer, &w, self->unk_3e0, 0xc000, 1, 0, 1);
        ((Player *)self->mEatingPlayer)->func_ov002_020d718c();
        self->mEatingPlayer = 0;
    }
    func_ov102_0214baa0((char *)self);
}

}

/* ==========================================================================
 * Vtable slot 19.
 * ======================================================================== */

// @symbol _ZN7daBmb_c13OnTurnIntoEggER6Player
void daBmb_c::OnTurnIntoEgg(Player &player)
{
    if (unk_108 == 1) {
        Sound::PlayBank3(0x11, mCamSpacePos);
        GiveCoins(player.mPlayerNo, 1);
        player.Heal(0x100);
    }
    func_ov102_0214baa0((char *)this);
}

/* ==========================================================================
 * ======================================================================== */

extern "C" {

// @symbol func_ov102_0214ad40
void func_ov102_0214ad40(void *cv)
{
    daBmb_c *self = (daBmb_c *)cv;

    if ((unsigned char)(self->mVariant + 0xfe) > 1)
        return;
    if (self->mPosY >= self->mHomePosY - 0x12c000)
        return;
    {
        unsigned short v = self->mFuse;
        if (v > 4)
            ;
        else if (v != 0)
            return;
    }
    func_ov102_0214b384(cv, 4);
    self->mVariant = 3;
    self->mdCc_c.flags &= ~2;
    func_ov102_0214c0b8(cv);
}

}

/* ==========================================================================
 *
 * CALLED FROM OUTSIDE THIS TU: ov002 0x020f1740, ov078 0x02125284, and
 * ov102's own daObjHatenaBlock_c run at 0x0214926c.  It keeps external linkage.
 * ======================================================================== */

extern "C" {

// @symbol func_ov102_0214ad14
void func_ov102_0214ad14(void *c)
{
    daBmb_c *self = (daBmb_c *)c;

    func_ov102_0214b384(c, 0x96);
    self->mChasePlayer = self->ClosestPlayer();
    func_ov102_0214c0b8(c);
}

}

/* ==========================================================================
 * ======================================================================== */

extern "C" {

// @symbol func_ov102_0214ab1c
int func_ov102_0214ab1c(void *selfv)
{
    extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *m, void *f, int i, int fx, unsigned int j);

    daBmb_c *self = (daBmb_c *)selfv;
    Player *player;

    int eatState = self->UpdateYoshiEat(self->mWithMeshClsn);
    if (eatState == 0) {
        goto ret0;
    }

    self->mFlags &= ~0x10000000u;

    player = (Player *)self->mEatingPlayer;
    if (player != 0) {
        if (player->IsInsideOfCannon() != 0) {
            goto shocked;
        }
        if (player->IsBeingShotOutOfCannon() != 0) {
            goto shocked;
        }
        if (func_ov102_0214b248(self) == 0) {
            self->mFlags &= ~0xe0000u;
            self->mEatenByYoshi = 0;
            return 1;
        }
        goto after_cannon;
    shocked:
        func_ov102_0214b384(self, 3);
    }
after_cannon:

    if (self->mVariant == 2) {
        self->mVariant = 3;
        self->mState = 0;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim, *(void **)((char *)&data_ov102_0214e9c0 + 4), 0, 0x1000, 0);
        self->mTurnTimer = 0x200;
        self->mVertAccel = -0x2000;
        self->mdCc_c.flags &= ~2u;
    }

    if (eatState == 2) {
        func_ov102_0214b384(self, 0x50);
    }

    if (eatState == 3) {
        if (self->mWithMeshClsn.IsOnGround() != 0) {
            self->unk_3f4 = 3;
        }
    }

    if (self->mEatenByYoshi != 0) {
        if (self->unk_104 == 5) {
            self->mdCc_c.flags &= ~0x8000u;
        }
    }

    if (self->SpawnParticlesIfHitOtherObj(self->mdCc_c) != 0) {
        goto hit;
    }
    if (eatState != 3 || self->mWithMeshClsn.IsOnGround() == 0) {
        goto skip_hit;
    }

hit:
    func_ov102_0214b384(self, 4);
    self->mEatenByYoshi = 0;
    return 1;

skip_hit:
    self->mdCc_c.Clear();
    if (self->mEatenByYoshi != 0) {
        if (self->unk_104 == 0) {
            self->mdCc_c.dCc_c::Update();
        }
    }
    func_ov102_0214b53c((char *)self);
    return 1;

ret0:
    return 0;
}

}

/* ==========================================================================
 * ======================================================================== */

extern "C" {

// @symbol func_ov102_0214aa18
int func_ov102_0214aa18(void *selfv)
{
    extern void func_0200fc44(char *c, void *v, int x);

    daBmb_c *self = (daBmb_c *)selfv;
    if (self->mVariant == 2) {
        if (*(int *)((char *)&self->mClipResult + 4) != 0) {
            func_ov102_0214b53c((char *)self);
            return 1;
        }
        self->UpdatePos(&self->mdCc_c);
        self->UpdateWMClsn(self->mWithMeshClsn, 0);
        func_ov102_0214b53c((char *)self);
        func_ov102_0214ad40(self);
        self->mdCc_c.Clear();
        self->mdCc_c.dCc_c::Update();
        if (self->mWithMeshClsn.JustHitGround()) {
            Vector3 v;
            v.x = self->mPosX;
            v.y = self->mPosY;
            v.z = self->mPosZ;
            func_0200fc44((char *)self, &v, 1);
        }
        if (self->mWithMeshClsn.IsOnGround() == 0)
            return 1;
        self->mdCc_c.flags &= ~2;
        self->mVariant = 3;
        func_ov102_0214c0b8(self);
        return 1;
    }
    return 0;
}

}

/* ==========================================================================
 * Vtable slot 29.
 *
 * Below this, at 0x0214a9b4 and 0x0214a96c, the compiler emits D0 and D1 from
 * the header's inline destructor body.  Nothing is written for them here.
 * ======================================================================== */

// @symbol _ZN7daBmb_c16OnAimedAtWithEggEv
s32 daBmb_c::OnAimedAtWithEgg() {
    return 204800;
}
