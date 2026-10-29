//cpp
/* daBgSnmBdy_c -- the rolling snowman's body (BIG_SNOWMAN_BODY 274), ov072.
 *
 * A snowball that talks, then rolls. State0 waits for the closest player
 * within 270.0 and starts a talk; State1 shows message 0xb0 and pauses 21
 * frames. State2 rolls along path param1 & 0xff at up to 40.0, growing to
 * scale 1.5, kicking up dust and hurting a Mario it touches while moving.
 * At the path's end it takes State3 if the talker kept up (reached the
 * course point the sinit stores at gBgSnmBdyReachPoint, IsPlayerNearCenter);
 * otherwise State4 rolls on until it falls below Y 0xfe363c80. State3 turns
 * toward the landing point at gBgSnmBdyLandingPoint, and within 380.0 flags
 * BIG_SNOWMAN_HEAD (0x111, mBodyArriving = 1), jumps, and on landing sets off an
 * Earthquake and snaps onto that point. State5 waits until its home is well
 * away from the camera, then respawns there in State0.
 *
 * ov072 is BABY_PENGUIN / BIG_SNOWMAN / SNOWMAN_HEAD / SNOWMAN_BODY. RTTI
 * ov072:0x0212278c names this class; the class run is 0x0211f000..0x0211fedc
 * (28 functions), and the factory at 0x0211fedc allocates exactly
 * sizeof(daBgSnmBdy_c) and ends at the next class's D1 at 0x0211ff34.
 *
 * Source is highest-ROM-address first: mwccarm emits ordinary sections in
 * reverse, so the factory leads and the inline destructor, declared last on
 * the class, emits the retail D1/D0 pair at the bottom with no D2. The
 * class-local operator new routes `new daBgSnmBdy_c()` at fBase_c's
 * allocator, so the compiler owns the vptr store. common.h comes before
 * any matrix header: InitResources copies IDENTITY_MATRIX4X3 into
 * mShadowMat, and only common.h's flat s32 m[12] is that twelve-word copy.
 *
 * deslop leftovers:
 * - Player::Hurt (HurtPlayer): not declared on Player.h.
 * - dBgCh_Actr_UpdateContinuous_Veneer (UpdateGroundCollision): retail
 *   bl is 0x020383fc, the veneer, not UpdateContinuous at 0x020366b4.
 * - GetFloorResult returns an existing polygon through void *. The local
 *   cast names that borrowed dBgPi and accesses its real surface member.
 * - func_0203568c / func_02035684 (InitState0, State2): store mRadius /
 *   mHeight (p[6] / p[7]). Direct stores on State2 are 0x1a8 -> 0x1a0
 *   (2 words) and drop those bls. Their C definitions take int *, so
 *   the calls cast.
 * - func_0201267c (State3): Sound::Play(3, id, pos) at 0x0201267c.
 *   Sound::PlayBank3 is the twin at 0x02012664, a different function.
 * - gBgSnmBdyModelFile, gBgSnmBdyStates, gBgSnmBdyReachPoint and
 *   gBgSnmBdyLandingPoint are descriptive names. Defining them here makes
 *   mwcc emit the retail initializer (0x02122018) itself.
 * - Vec3_Dist, Vec3_HorzDist and Vec3_HorzAngle are declared here on
 *   const void *, so their calls take the address of the X field.
 *
 * The 0/1 values the ROM materializes in HurtPlayer and State5 come from
 * int-returning inline predicates, IsPlayerActor and IsOffScreen.
 * IsPlayerNearCenter's sign-extend/negate/re-extend is the AbsAngle inline.
 */

#include "common.h"
#include "daBgSnmBdy_c.h"
#include "SpawnInfo.h"
#include "daBgSnmHed_c.h"
#include "SharedFilePtr.h"
#include "dBgCh_Gnd.h"
#include "Player.h"
#include "dBgPi.h"
#include "Sound.h"
#include "dClipper.h"
#include "math/Approach.h"

/* Real C++ names. Declared here, not via decl_common.h: that header's
 * other spellings of these symbols collide with this TU. */

namespace cstd {
int fdiv(int numerator, int denominator);
}

/* Shared ABI seams, kept above the first `// @symbol` marker so no member is
 * charged with their mangled spellings (notes/tu-promotion-conventions.md
 * sec 6). Each is the most complete of the spellings the retired one-function
 * shards carried. */
extern "C" {
int   Vec3_Dist(const void *a, const void *b);
int   Vec3_HorzDist(const void *a, const void *b);
short Vec3_HorzAngle(const void *a, const void *b);
void  Vec3_Asr(void *dst, const void *src, int shift);
unsigned short DecIfAbove0_Short(unsigned short *timer);
void  Matrix4x3_FromRotationXYZExt(void *m, int x, int y, int z);
void  func_0203568c(int *clsn, int radius);
void  func_02035684(int *clsn, int height);


}
namespace Particle {
void RunningSlidingDustAt(int x, int y, int z);
}
extern "C" {

/* arm9 statics this TU reads but does not own. */
extern dClipper data_0209f43c;
extern Matrix4x3 data_0209b3ec;
}

/* The model handle. dExtSimpleModel_c reaches its file through this wrapper's
 * constructor and destructor veneers; the manifest maps both to the retail
 * destinations. A descriptive local type, not a recovered spelling. */
struct BgSnmBdyModelFile : SharedFilePtr {
    BgSnmBdyModelFile(u32);
    ~BgSnmBdyModelFile();
};

/* Runtime asset handle, distinct from the NitroFS file ID. */
enum BgSnmBdyAssetId { ASSET_BIG_SNOWMAN_BODY_BMD = 0x02ad };

/* This TU's statics, in the order the retail initializer builds them. */
BgSnmBdyModelFile gBgSnmBdyModelFile(ASSET_BIG_SNOWMAN_BODY_BMD);

daBgSnmBdy_c::StateEntry gBgSnmBdyStates[6] = {
    {&daBgSnmBdy_c::InitState0, &daBgSnmBdy_c::State0},
    {&daBgSnmBdy_c::InitState1, &daBgSnmBdy_c::State1},
    {&daBgSnmBdy_c::InitState2, &daBgSnmBdy_c::State2},
    {&daBgSnmBdy_c::InitState3, &daBgSnmBdy_c::State3},
    {&daBgSnmBdy_c::InitState4, &daBgSnmBdy_c::State4},
    {&daBgSnmBdy_c::InitState5, &daBgSnmBdy_c::State5},
};

/* The course point the talker must reach (State2), and the landing point
 * State3 turns toward and snaps onto (IsPlayerNearCenter measures from it). */
Vector3 gBgSnmBdyReachPoint(-0xad0000, 0x290000, -0x9d0000);
Vector3 gBgSnmBdyLandingPoint(-0x1004000, -0x591000, 0x6fe000);

extern Matrix4x3 IDENTITY_MATRIX4X3;

/* The NitroSDK fixed-point products, written as local inlines the way
 * d_s_mg_base.cpp and d_s_mg_pachinko.cpp spell FX_Mul. FX_Mul is a 20.12
 * product rounded at bit 11; FX_Mul32x64c multiplies by a 32.32 constant
 * and rounds at bit 31. */
static inline int FX_Mul(int v1, int v2) {
    return (int)(((s64)v1 * v2 + 0x800) >> 12);
}

static inline int FX_Mul32x64c(int v32, s64 v64c) {
    return (int)(((s64)v32 * v64c + 0x80000000LL) >> 32);
}

/* pi as a 32.32 constant: 0x3.243f6a89. */
#define FX64C_PI 0x00000003243F6A89LL

/* One full turn of a 16-bit angle, as a 20.12 multiplier. */
#define ANGLE_FULL_TURN 0xffff

/* dActor_c::mFlags bit 0x08 is written by the framework while the actor is
 * off screen. Actor ID 0xbf is PLAYER. These int-returning predicates are
 * what make the ROM materialize 0/1 before testing it. */
static inline int IsOffScreen(dActor_c *actor) {
    return (actor->mFlags & 8) != 0;
}

static inline int IsPlayerActor(dActor_c *actor) {
    return actor->actorID == 0xbf;
}

static inline short AbsAngle(short angle) {
    if (angle < 0)
        angle = -angle;
    return angle;
}

/* Reconstructed source-style names. SM64DS preserves this class's RTTI, the
 * BIG_SNOWMAN_BODY registry ID, the descriptor relationship and the factory's
 * behavior; it preserves neither identifier below. */
// @symbol daBgSnmBdy_c_classInit
extern "C" daBgSnmBdy_c *daBgSnmBdy_c_classInit()
{
    return new daBgSnmBdy_c();
}

/* The actor profile, in SpawnInfo's layout: fBase_c reads the halfwords at
 * +4/+6 as behavior/render priorities, and dActor_c reads the actor flags at
 * +8 and passes the words at +0xc/+0x10/+0x14/+0x18 to SetRanges as clip
 * offset Y, clip radius, clip distance and far distance. SpawnInfo's field
 * names follow those consumers; the retail values are unchanged. */
SpawnInfo<daBgSnmBdy_c> g_profile_BIG_SNOWMAN_BODY = {
    daBgSnmBdy_c_classInit,
    0x0112,
    0x0085,
    2,
    0x82000,
    0x82000,
    0x01000000,
    0x01000000
};

// @symbol _ZN12daBgSnmBdy_c13InitResourcesEv
int daBgSnmBdy_c::InitResources()
{
    Vector3 pos;
    void *file = dExtSimpleModel_c::LoadFile(gBgSnmBdyModelFile);
    mModel.SetFile((BMD_File *)file, 1, 1);
    if (mShadowModel.InitCylinder() == 0)
        return 0;
    mCylinder.Init(this, 0x82000, 0x104000, 0x800004, 0);
    mWithMeshClsn.Init(this, 0x82000, 0x82000, 0, 0);
    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    pos.y += 0x14000;
    dBgCh_Gnd ground;
    ground.SetObjAndPos(pos, 0);
    if (ground.DetectClsn())
        mPosY = ground.clsnY;
    else
        mPosY = pos.y;
    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    mHomeAngleX = mAngleX;
    mHomeAngleY = mAngleY;
    mHomeAngleZ = mAngleZ;
    unk_3a4 = 0x5a;
    SetState(0);
    mTalkPlayer = 0;
    mShadowMat = IDENTITY_MATRIX4X3;
    UpdateModel();
    mPath.FromID(param1 & 0xff);
    return 1;
}

/* dCc_c comes from the real dCcAc_c chain now that the header types
   mCylinder; Clear and Update are non-virtual there, so the direct bl is
   unchanged. */
// @symbol _ZN12daBgSnmBdy_c8BehaviorEv
int daBgSnmBdy_c::Behavior()
{
    CallStateBehavior();
    mCylinder.Clear();
    mCylinder.Update();
    UpdateModel();
    return 1;
}

// @symbol _ZN12daBgSnmBdy_c6RenderEv
int daBgSnmBdy_c::Render()
{
    mModel.Render(&mScale);
    return 1;
}

/* Vtable slot 12. The ROM body is empty: the override exists only to occupy
 * the slot. */
// @symbol _ZN12daBgSnmBdy_c16OnPendingDestroyEv
void daBgSnmBdy_c::OnPendingDestroy()
{
}

/* Vtable slot 3. Releases the one shared file the class holds; it never
 * touches `this`. */
// @symbol _ZN12daBgSnmBdy_c16CleanupResourcesEv
int daBgSnmBdy_c::CleanupResources()
{
    gBgSnmBdyModelFile.Release();
    return 1;
}

/* SetState and its int parameter are inferred spellings. The ROM proves
 * the table indexing, not the original identifier or int-versus-enum type. */
// @symbol _ZN12daBgSnmBdy_c8SetStateEi
void daBgSnmBdy_c::SetState(int state)
{
    mState = &gBgSnmBdyStates[state];
    CallStateInit();
}

// @symbol _ZN12daBgSnmBdy_c13CallStateInitEv
void daBgSnmBdy_c::CallStateInit()
{
    (this->*mState->init)();
}

// @symbol _ZN12daBgSnmBdy_c17CallStateBehaviorEv
void daBgSnmBdy_c::CallStateBehavior()
{
    (this->*mState->behavior)();
}

// @symbol _ZN12daBgSnmBdy_c10InitState0Ev
int daBgSnmBdy_c::InitState0()
{
    mVertAccel = 0;
    mTerminalVelocity = 0;
    mScaleX = 0x800;
    mScaleY = 0x800;
    mScaleZ = 0x800;
    mRadius = FX_Mul(mScaleX, 0x82000);
    mCylinder.radius = mRadius;
    mCylinder.height = mRadius << 1;
    func_0203568c((int *)&mWithMeshClsn, mRadius);
    func_02035684((int *)&mWithMeshClsn, mRadius);
    SetRanges(mRadius, mRadius, 0x1000000, 0x1000000);
    mFlags |= 1;
    mStateValue = 0;
    return 1;
}

// @symbol _ZN12daBgSnmBdy_c6State0Ev
int daBgSnmBdy_c::State0()
{
    Player *player = ClosestPlayer();
    if (Vec3_HorzDist(&mPosX, &player->mPosX) < 0x10e000) {
        if (player->StartTalk(*this, 1)) {
            mTalkPlayer = player;
            SetState(1);
        }
    }
    return 1;
}

// @symbol _ZN12daBgSnmBdy_c10InitState1Ev
int daBgSnmBdy_c::InitState1()
{
    mSubstate = 0;
    mStateTimer = 0x15;
    mStateValue = 1;
    return 1;
}

// @symbol _ZN12daBgSnmBdy_c6State1Ev
int daBgSnmBdy_c::State1()
{
    Vector3 messagePos;
    messagePos.x = mPosX;
    messagePos.y = mPosY;
    messagePos.z = mPosZ;
    messagePos.y += 0x96000;
    switch (mSubstate) {
    case 0:
        if (mTalkPlayer->ShowMessage(*this, 0xb0, &messagePos, 0, 0) == 0)
            break;
        mSubstate++;
        break;
    case 1:
        if (mTalkPlayer->GetTalkState() != -1)
            break;
        mSubstate++;
        break;
    case 2:
        if (DecIfAbove0_Short(&mStateTimer) == 0)
            SetState(2);
        break;
    }
    return 1;
}

// @symbol _ZN12daBgSnmBdy_c10InitState2Ev
int daBgSnmBdy_c::InitState2()
{
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    mFlags &= ~1;
    mPathNode = 0;
    mHorzSpeed = 0;
    mStateValue = 2;
    return 1;
}

// @symbol _ZN12daBgSnmBdy_c6State2Ev
int daBgSnmBdy_c::State2()
{
    int growScale;
    int newScale;
    ApproachLinear(mHorzSpeed, 0x28000, 0x400);
    if (DecIfAbove0_Short(&mStateTimer) == 0) {
        growScale = mScaleX;
        ApproachLinear(growScale, 0x1800, 0x11);
        newScale = growScale;
        mScaleX = newScale;
        mScaleY = newScale;
        mScaleZ = newScale;
        mRadius = FX_Mul(growScale, 0x82000);
        mCylinder.radius = mRadius;
        mCylinder.height = mRadius << 1;
        func_0203568c((int *)&mWithMeshClsn, mRadius);
        func_02035684((int *)&mWithMeshClsn, mRadius);
        SetRanges(mRadius, mRadius, 0x1000000, 0x1000000);
    }
    if (AdvancePath() != 0) {
        if (mPlayerReachedPath != 0 && IsPlayerNearCenter() != 0)
            SetState(3);
        else
            SetState(4);
    }
    Particle::RunningSlidingDustAt(mPosX, mPosY, mPosZ);
    mSoundID = Sound::PlayLong(mSoundID, 3, 0x8a, mCamSpacePos, 0);
    UpdateRollAngle();
    UpdatePos(&mCylinder);
    UpdateGroundCollision(&mWithMeshClsn);
    HurtPlayer();
    if (mPlayerReachedPath == 0) {
        if (Vec3_Dist(&gBgSnmBdyReachPoint, &mTalkPlayer->mPosX) < 0x300000)
            mPlayerReachedPath = 1;
    }
    return 1;
}

// @symbol _ZN12daBgSnmBdy_c10InitState3Ev
int daBgSnmBdy_c::InitState3()
{
    mSubstate = 0;
    mStateValue = 3;
    return 1;
}

// @symbol _ZN12daBgSnmBdy_c6State3Ev
int daBgSnmBdy_c::State3() {
    switch (mSubstate) {
    case 0: {
        int distToLanding = Vec3_HorzDist(&gBgSnmBdyLandingPoint, &mPosX);
        UpdateAngle(mAngleY, Vec3_HorzAngle(&mPosX, &gBgSnmBdyLandingPoint), 2, 0x600);
        mPrevAngleY = mAngleY;
        Particle::RunningSlidingDustAt(mPosX, mPosY, mPosZ);
        mSoundID = Sound::PlayLong(mSoundID, 3, 0x8a, mCamSpacePos, 0);
        if (distToLanding < 0x17c000) {
            daBgSnmHed_c *head =
                (daBgSnmHed_c *)dActor_c::FindWithActorID(0x111, 0); /* BIG_SNOWMAN_HEAD */
            head->mBodyArriving = 1;
            func_0201267c(0x114, &mCamSpacePos);
            mVertSpeed = 0x1d000;
            mHorzSpeed = 0xe000;
            mSubstate++;
        }
    } break;
    case 1:
        if (mWithMeshClsn.JustHitGround() != 0) {
            Vector3 landPos;
            landPos.x = mPosX;
            landPos.y = mPosY;
            landPos.z = mPosZ;
            Earthquake(landPos, 0x5dc000);
            mPosX = gBgSnmBdyLandingPoint.x;
            mPosY = gBgSnmBdyLandingPoint.y;
            mPosZ = gBgSnmBdyLandingPoint.z;
            mAngleX = 0;
            mAngleY = (short)-0x4000;
            mHorzSpeed = 0;
            mSubstate++;
        }
        break;
    case 2:
    default:
        break;
    }

    UpdateRollAngle();
    UpdatePos(&mCylinder);
    UpdateGroundCollision(&mWithMeshClsn);
    HurtPlayer();
    return 1;
}

// @symbol _ZN12daBgSnmBdy_c10InitState4Ev
int daBgSnmBdy_c::InitState4()
{
    mFlags &= ~1;
    mStateValue = 4;
    return 1;
}

// @symbol _ZN12daBgSnmBdy_c6State4Ev
int daBgSnmBdy_c::State4()
{
    ApproachLinear(mHorzSpeed, 0x28000, 0x400);
    Particle::RunningSlidingDustAt(mPosX, mPosY, mPosZ);
    mSoundID = Sound::PlayLong(mSoundID, 3, 0x8a, mCamSpacePos, 0);
    UpdateRollAngle();
    UpdatePos(&mCylinder);
    UpdateGroundCollision(&mWithMeshClsn);
    HurtPlayer();
    if (mPosY < (int)0xfe363c80)
        SetState(5);
    return 1;
}

// @symbol _ZN12daBgSnmBdy_c10InitState5Ev
int daBgSnmBdy_c::InitState5()
{
    mFlags &= ~1;
    mStateValue = 5;
    return 1;
}

// @symbol _ZN12daBgSnmBdy_c6State5Ev
int daBgSnmBdy_c::State5()
{
    if (!IsOffScreen(this))
        return 1;
    Vector3 clipResult;
    Vector3 homePos;
    Vec3_Asr(&homePos, &mHomePosX, 3);
    if (data_0209f43c.TransformAndClip(data_0209b3ec, homePos, 0x1f400, clipResult) < 0x1194000)
        return 1;
    mPosX = mHomePosX;
    mPosY = mHomePosY;
    mPosZ = mHomePosZ;
    mAngleX = mHomeAngleX;
    mAngleY = mHomeAngleY;
    mAngleZ = mHomeAngleZ;
    mPrevAngleX = mAngleX;
    mPrevAngleY = mAngleY;
    mPrevAngleZ = mAngleZ;
    mTalkPlayer = 0;
    SetState(0);
    return 1;
}

// @symbol _ZN12daBgSnmBdy_c11UpdateModelEv
void daBgSnmBdy_c::UpdateModel()
{
    Matrix4x3_FromRotationXYZExt(&mModel.mat4x3, mAngleX, mAngleY, mAngleZ);
    mModel.mat4x3.m[9] = mPosX >> 3;
    mModel.mat4x3.m[10] = (mPosY + mRadius) >> 3;
    mModel.mat4x3.m[11] = mPosZ >> 3;
    mShadowMat.m[9] = mPosX >> 3;
    mShadowMat.m[10] = (mPosY + mRadius) >> 3;
    mShadowMat.m[11] = mPosZ >> 3;
    DropShadowRadHeight(mShadowModel, mShadowMat, mRadius << 1, mRadius << 1, 0xf);
}

// @symbol _ZN12daBgSnmBdy_c21UpdateGroundCollisionEP10dBgCh_Actr
// The P10dBgCh_Actr in that spelling asserts a POINTER parameter. The bytes
// cannot distinguish a pointer from a reference here -- R10dBgCh_Actr would have
// matched equally well -- so the parameter type is a disclosed guess.
void daBgSnmBdy_c::UpdateGroundCollision(dBgCh_Actr *mc)
{
    Vector3 normal;
    dBgPi *floorResult;
    dBgCh_Actr_UpdateContinuous_Veneer(mc);
    if (mc->IsOnGround() == 0) return;
    floorResult = static_cast<dBgPi *>(mc->GetFloorResult());
    floorResult->surface.CopyNormalTo(normal);
    if (normal.y == 0) return;
    {
        int alongX = FX_Mul(normal.x, mSpeedX);
        int alongZ = FX_Mul(normal.z, mSpeedZ);
        mVertSpeed = -(cstd::fdiv(alongX + alongZ, normal.y) + 0x8000);
    }
}

// @symbol _ZN12daBgSnmBdy_c10HurtPlayerEv
int daBgSnmBdy_c::HurtPlayer()
{
    dActor_c *actor;
    u32 id;
    Vector3 pos;

    id = mCylinder.otherOwner;
    if (id == 0) return 0;
    actor = dActor_c::FindWithID(id);
    if (actor == 0 || !IsPlayerActor(actor))
        return 0;
    if (mHorzSpeed != 0) {
        pos.x = mPosX;
        pos.y = mPosY;
        pos.z = mPosZ;
        static_cast<Player *>(actor)->Hurt(pos, 2, 0xc000, 1, 0, 1);
    }
    return 1;
}

// @symbol _ZN12daBgSnmBdy_c11AdvancePathEv
int daBgSnmBdy_c::AdvancePath()
{
    Vector3 node;
    int numNodes;
    mPath.GetNode(node, mPathNode);
    int dist = Vec3_HorzDist(&mPosX, &node);
    UpdateAngle(mAngleY, Vec3_HorzAngle(&mPosX, &node), 2, 0x600);
    mPrevAngleY = mAngleY;
    if (dist < mRadius) {
        numNodes = mPath.NumNodes();
        /* Signed compare. mPathNode is u32; comparing it as u32 is unsigned. */
        mPathNode = mPathNode + 1;
        if ((int)mPathNode >= numNodes - 1) return 1;
    }
    return 0;
}

// @symbol _ZN12daBgSnmBdy_c15UpdateRollAngleEv
void daBgSnmBdy_c::UpdateRollAngle()
{
    int circumference = FX_Mul32x64c(mRadius << 1, FX64C_PI);
    Fix12i turns = cstd::fdiv(mHorzSpeed, circumference);
    mAngleX = (short)(mAngleX + FX_Mul(turns, ANGLE_FULL_TURN));
}

// @symbol _ZN12daBgSnmBdy_c18IsPlayerNearCenterEv
int daBgSnmBdy_c::IsPlayerNearCenter()
{
    int bodyDist = Vec3_Dist(&gBgSnmBdyLandingPoint, &mPosX);
    int playerDist = Vec3_Dist(&gBgSnmBdyLandingPoint, &mTalkPlayer->mPosX);
    int bodyAngle = Vec3_HorzAngle(&gBgSnmBdyLandingPoint, &mPosX);
    int playerAngle = Vec3_HorzAngle(&gBgSnmBdyLandingPoint, &mTalkPlayer->mPosX);
    int angleDiff = bodyAngle - playerAngle;
    if ((playerDist < bodyDist && AbsAngle((short)angleDiff) < 0x700) ||
        Vec3_Dist(&gBgSnmBdyLandingPoint, &mTalkPlayer->mPosX) < 0x300000)
        return 1;
    return 0;
}
