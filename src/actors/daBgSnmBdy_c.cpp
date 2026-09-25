//cpp
/* daBgSnmBdy_c -- the rolling snowman's body (BIG_SNOWMAN_BODY 274), ov072.
 *
 * A snowball that talks, then rolls. State0 waits for the closest player
 * within 270.0 and starts a talk; State1 shows message 0xb0 and pauses 21
 * frames. State2 rolls along path param1 & 0xff at up to 40.0, growing to
 * scale 1.5, kicking up dust and hurting a Mario it touches while moving.
 * At the path's end it takes State3 if the talker kept up (reached course
 * point data_ov072_02122b40, IsPlayerNearCenter); otherwise State4 rolls on
 * until it falls below Y 0xfe363c80. State3 turns toward
 * data_ov072_02122b58, and within 380.0 flags BIG_SNOWMAN_HEAD (0x111,
 * unk_336 = 1), jumps, and on landing sets off an Earthquake and snaps onto
 * that point. State5 waits until its home is well away from the camera,
 * then respawns there in State0.
 *
 * ov072 is BABY_PENGUIN / BIG_SNOWMAN / SNOWMAN_HEAD / SNOWMAN_BODY. RTTI
 * ov072:0x0212278c names this class; the class run is 0x0211f000..0x0211fedc
 * (28 functions), and the factory at 0x0211fedc allocates exactly
 * sizeof(daBgSnmBdy_c) and ends at the next class's D1 at 0x0211ff34.
 *
 * DO NOT "TIDY" THESE -- each one is load-bearing:
 *   Source is highest-ROM-address first (mwccarm emits ordinary sections in
 *   reverse): the factory leads, and the inline destructor, declared last in
 *   the class, emits the retail D1/D0 pair at the bottom with no D2.
 *   The class-local operator new routes `new daBgSnmBdy_c()` to fBase_c's
 *   allocator, so the compiler owns the vptr store; no _ZTV is spelled here.
 *   common.h FIRST: InitResources assigns IDENTITY_MATRIX4X3 into mShadowMat,
 *   and only common.h's flat s32 m[12] gives the ROM's twelve-word copy.
 *   State1 / State3 bump mSubstate through (int)this + 0x3a2; the named
 *   `mSubstate = mSubstate + 1` changes the code size.
 *   AdvancePath: PathPtr::GetNode / NumNodes as methods change the code
 *   size (14 words); named &mPath / &mPosX / mRadius / mAngleY change it
 *   too (29 words);
 *   mPathNode is incremented and compared signed through (int)this + 0x388.
 *   UpdateRollAngle / InitState0 keep (long long)(int) on mRadius / mScaleX.
 *
 * WHY SOME CALLS ARE SPELLED AS MANGLED SYMBOLS:
 *   dCcAc_c::Init, dBgCh_Actr::Init, dActor_c::SetRanges /
 *   DropShadowRadHeight / Earthquake, Particle::RunningSlidingDustAt,
 *   Clipper::Func_02015560 and Player::Hurt pass Fix12<int> by value (see
 *   notes/mwccarm-codegen.md 6az); the header method forms change the code
 *   size. dBgCh_Actr::Init's header Fix12i also mangles as int where the ROM
 *   has Fix12<int>.
 *   dBgCh_Actr_UpdateContinuous_Veneer: the ROM calls the veneer, not
 *   UpdateContinuous (a direct call is WRONG-DEST).
 *   dBgCh_Actr::GetFloorResult and Sound::PlayLong: no header declares them.
 *
 * Known limits:
 *   func_0203568c / func_02035684 store the dBgCh_Actr radius and height;
 *   there is no setter. func_0201267c plays sound 0x114 in State3.
 *   data_ov072_02122b20 / 02122b40 / 02122b58 / 02122b64 (BMD handle, two
 *   course points, the twelve-PMF state table) are owned by overlay .data.
 */

#include "common.h"
#include "daBgSnmBdy_c.h"
#include "daBgSnmHed_c.h"
#include "SharedFilePtr.h"
#include "dBgCh_Gnd.h"
#include "Player.h"
#include "SurfaceInfo.h"

/* Shared ABI seams, kept above the first `// @symbol` marker so no member is
 * charged with their mangled spellings (notes/tu-promotion-conventions.md
 * sec 6). Each is the most complete of the spellings the retired one-function
 * shards carried. */
extern "C" {
int   Vec3_Dist(const void *a, const void *b);
int   Vec3_HorzDist(const void *a, const void *b);
short Vec3_HorzAngle(const void *a, const void *b);
void  Vec3_Asr(void *dst, const void *src, int shift);
int   _Z11UpdateAngleRssis(short *angle, short target, int shift, short maxStep);
void  _Z14ApproachLinearRiii(int *value, int target, int step);
unsigned short DecIfAbove0_Short(unsigned short *timer);
int   _ZN4cstd4fdivEii(int a, int b);
void  Matrix4x3_FromRotationXYZExt(void *m, int x, int y, int z);
void  func_0201267c(int id, void *pos);
void  func_0203568c(int *p, int v);
void  func_02035684(int *p, int v);
void  dBgCh_Actr_UpdateContinuous_Veneer(void *self);

void      _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(void *self, int a, int b,
                                                    int c, int d);
void      _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(void *self,
                                                       const Vector3 *pos,
                                                       int strength);
void      _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
              void *self, void *shadow, void *matrix, int radius, int height,
              u32 flags);

void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(dActor_c *player,
                                             const Vector3 *pos, u32 a,
                                             int fix, u32 b, u32 c, u32 d);

char *_ZNK10dBgCh_Actr14GetFloorResultEv(void *self);
void  _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
          void *self, void *actor, int a, int b, void *v, int c);
void  _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *self, void *actor, int a,
                                                int b, u32 c, u32 d);

void _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(int x, int y, int z);
u32  _ZN5Sound8PlayLongEjjjRK7Vector3s(u32 handle, u32 a, u32 b,
                                       const Vector3 *pos, u32 e);
int  _ZN7Clipper13Func_02015560ER9Matrix4x3R7Vector35Fix12IiES3_(
         void *clipper, void *matrix, void *pos, int radius, void *result);

/* ov072 and arm9 statics this TU reads but does not own. */
extern SharedFilePtr data_ov072_02122b20;
extern Vector3       data_ov072_02122b40;
extern Vector3       data_ov072_02122b58;
extern daBgSnmBdy_c::StateFunc data_ov072_02122b64[];
extern int           data_0209f43c[];
extern int           data_0209b3ec[];
}

extern Matrix4x3 IDENTITY_MATRIX4X3;

/* The typed 0x1c actor profile: fBase_c reads the halfwords at +4/+6 as
 * behavior/render priorities. dActor_c reads actor flags at +8 and passes
 * the words at +0xc/+0x10/+0x14/+0x18 to SetRanges as clip offset Y, clip
 * radius, clip distance and far distance. These field names follow those
 * consumers; the existing widths, types and retail values are unchanged. */
struct SnmBdyProfile {
    daBgSnmBdy_c *(*classInit)();
    s16 behaviorPriority;
    s16 renderPriority;
    u32 actorFlags;
    Fix12i clipOffsetY;
    Fix12i clipRadius;
    u32 clipDistance;
    u32 farDistance;
};

typedef char SnmBdyProfile_size_must_be_0x1c[
    sizeof(SnmBdyProfile) == 0x1c ? 1 : -1];

/* Reconstructed source-style names. SM64DS preserves this class's RTTI, the
 * BIG_SNOWMAN_BODY registry ID, the descriptor relationship and the factory's
 * behavior; it preserves neither identifier below. */
// @symbol daBgSnmBdy_c_classInit
extern "C" daBgSnmBdy_c *daBgSnmBdy_c_classInit()
{
    return new daBgSnmBdy_c();
}

extern "C" SnmBdyProfile g_profile_BIG_SNOWMAN_BODY = {
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
    void *file = Model::LoadFile(data_ov072_02122b20);
    mModel.SetFile((BMD_File *)file, 1, 1);
    if (mShadowModel.InitCylinder() == 0)
        return 0;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mCylinder, this, 0x82000,
                                             0x104000, 0x800004, 0);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
        &mWithMeshClsn, this, 0x82000, 0x82000, 0, 0);
    {
        int posY;
        pos.x = mPosX;
        posY = mPosY;
        pos.y = posY;
        pos.z = mPosZ;
        pos.y = posY + 0x14000;
    }
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
    mModel.Render((Vector3 *)&mScaleX);
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
    data_ov072_02122b20.Release();
    return 1;
}

/* SetState and its int parameter are inferred spellings. The ROM proves
 * the table indexing, not the original identifier or int-versus-enum type. */
// @symbol _ZN12daBgSnmBdy_c8SetStateEi
void daBgSnmBdy_c::SetState(int state)
{
    mStateFuncs = data_ov072_02122b64 + state * 2;
    CallStateInit();
}

// @symbol _ZN12daBgSnmBdy_c13CallStateInitEv
void daBgSnmBdy_c::CallStateInit()
{
    StateFunc *func = mStateFuncs;
    (this->**func)();
}

// @symbol _ZN12daBgSnmBdy_c17CallStateBehaviorEv
void daBgSnmBdy_c::CallStateBehavior()
{
    StateFunc *func = mStateFuncs + 1;
    (this->**func)();
}

// @symbol _ZN12daBgSnmBdy_c10InitState0Ev
int daBgSnmBdy_c::InitState0()
{
    mVertAccel = 0;
    mTerminalVelocity = 0;
    mScaleX = 0x800;
    mScaleY = 0x800;
    mScaleZ = 0x800;
    /* MATCH: (long long)(int)mScaleX keeps the ROM's widen-from-int shape. */
    mRadius = (int)(((long long)(int)mScaleX * 0x82000 + 0x800) >> 12);
    mCylinder.radius = mRadius;
    mCylinder.height = mRadius << 1;
    func_0203568c((int *)&mWithMeshClsn, mRadius);
    func_02035684((int *)&mWithMeshClsn, mRadius);
    _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(
        this, mRadius, mRadius, 0x1000000, 0x1000000);
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
    int messagePos[3];
    unsigned char *state;
    messagePos[0] = mPosX;
    int y = mPosY;
    messagePos[1] = y;
    messagePos[2] = mPosZ;
    messagePos[1] = y + 0x96000;
    switch (mSubstate) {
    case 0:
        if (mTalkPlayer->ShowMessage(*this, 0xb0, (const Vector3 *)messagePos, 0, 0) == 0)
            break;
        /* MATCH: (int)this+0x3a2; named mSubstate = mSubstate + 1 size-DIFFs. */
        state = (unsigned char *)((int)this + 0x3a2);
        *state = *state + 1;
        break;
    case 1:
        if (mTalkPlayer->GetTalkState() != -1)
            break;
        state = (unsigned char *)((int)this + 0x3a2);
        *state = *state + 1;
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
    _Z14ApproachLinearRiii(&mHorzSpeed, 0x28000, 0x400);
    if (DecIfAbove0_Short(&mStateTimer) == 0) {
        growScale = mScaleX;
        _Z14ApproachLinearRiii(&growScale, 0x1800, 0x11);
        newScale = growScale;
        mScaleX = newScale;
        mScaleY = newScale;
        mScaleZ = newScale;
        /* MATCH: (long long)growScale is already int; keep the 0x82000 scale of mRadius. */
        mRadius = (int)(((long long)growScale * 0x82000 + 0x800) >> 12);
        mCylinder.radius = mRadius;
        mCylinder.height = mRadius << 1;
        func_0203568c((int *)&mWithMeshClsn, mRadius);
        func_02035684((int *)&mWithMeshClsn, mRadius);
        _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(
            this, mRadius, mRadius, 0x1000000, 0x1000000);
    }
    if (AdvancePath() != 0) {
        if (mPlayerReachedPath != 0 && IsPlayerNearCenter() != 0)
            SetState(3);
        else
            SetState(4);
    }
    _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(mPosX, mPosY, mPosZ);
    mSoundID = _ZN5Sound8PlayLongEjjjRK7Vector3s(
        mSoundID, 3, 0x8a, (const Vector3 *)&mCamSpacePosX, 0);
    UpdateRollAngle();
    UpdatePos(&mCylinder);
    UpdateGroundCollision(&mWithMeshClsn);
    HurtPlayer();
    if (mPlayerReachedPath == 0) {
        if (Vec3_Dist(&data_ov072_02122b40, &mTalkPlayer->mPosX) < 0x300000)
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
int daBgSnmBdy_c::State3()
{
    unsigned char *state;
    switch (mSubstate) {
    case 0:
        {
            int distToLanding = Vec3_HorzDist(&data_ov072_02122b58, &mPosX);
            _Z11UpdateAngleRssis(&mAngleY,
                Vec3_HorzAngle(&mPosX, &data_ov072_02122b58),
                2, 0x600);
            mPrevAngleY = mAngleY;
            _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(
                mPosX, mPosY, mPosZ);
            mSoundID = _ZN5Sound8PlayLongEjjjRK7Vector3s(
                mSoundID, 3, 0x8a,
                (const Vector3 *)&mCamSpacePosX, 0);
            if (distToLanding < 0x17c000) {
                dActor_c *head = dActor_c::FindWithActorID(0x111, 0); /* BIG_SNOWMAN_HEAD */
                ((daBgSnmHed_c *)head)->unk_336 = 1;
                func_0201267c(0x114, &mCamSpacePosX);
                mVertSpeed = 0x1d000;
                mHorzSpeed = 0xe000;
                /* MATCH: (int)this+0x3a2; named mSubstate = mSubstate + 1 size-DIFFs. */
                state = (unsigned char *)((int)this + 0x3a2);
                *state = *state + 1;
            }
        }
        break;
    case 1:
        if (mWithMeshClsn.JustHitGround() != 0) {
            Vector3 landPos;
            landPos.x = mPosX;
            landPos.y = mPosY;
            landPos.z = mPosZ;
            _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(this, &landPos, 0x5dc000);
            mPosX = data_ov072_02122b58.x;
            mPosY = data_ov072_02122b58.y;
            mPosZ = data_ov072_02122b58.z;
            mAngleX = 0;
            mAngleY = (short)-0x4000;
            mHorzSpeed = 0;
            state = (unsigned char *)((int)this + 0x3a2);
            *state = *state + 1;
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
    _Z14ApproachLinearRiii(&mHorzSpeed, 0x28000, 0x400);
    _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(mPosX, mPosY, mPosZ);
    mSoundID = _ZN5Sound8PlayLongEjjjRK7Vector3s(
        mSoundID, 3, 0x8a, (const Vector3 *)&mCamSpacePosX, 0);
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
    int b = (int)((mFlags & 8) != 0);
    if (b == 0) return 1;
    int clipResult[3];
    int homePos[3];
    Vec3_Asr(homePos, &mHomePosX, 3);
    if (_ZN7Clipper13Func_02015560ER9Matrix4x3R7Vector35Fix12IiES3_(
            data_0209f43c, data_0209b3ec, homePos, 0x1f400, clipResult) < 0x1194000)
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
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        this, &mShadowModel, &mShadowMat, mRadius << 1, mRadius << 1, 0xf);
}

// @symbol _ZN12daBgSnmBdy_c21UpdateGroundCollisionEP10dBgCh_Actr
// The P10dBgCh_Actr in that spelling asserts a POINTER parameter. The bytes
// cannot distinguish a pointer from a reference here -- R10dBgCh_Actr would have
// matched equally well -- so the parameter type is a disclosed guess.
void daBgSnmBdy_c::UpdateGroundCollision(dBgCh_Actr *mc)
{
    Vector3 normal;
    char *floorResult;
    dBgCh_Actr_UpdateContinuous_Veneer(mc);
    if (mc->IsOnGround() == 0) return;
    floorResult = _ZNK10dBgCh_Actr14GetFloorResultEv(mc);
    ((SurfaceInfo *)(floorResult + 4))->CopyNormalTo(normal);
    if (normal.y == 0) return;
    {
        int a = (int)(((long long)normal.x * unk_0a4 + 0x800) >> 12);
        int b = (int)(((long long)normal.z * unk_0ac + 0x800) >> 12);
        mVertSpeed = -(_ZN4cstd4fdivEii(a + b, normal.y) + 0x8000);
    }
}

// @symbol _ZN12daBgSnmBdy_c10HurtPlayerEv
int daBgSnmBdy_c::HurtPlayer()
{
    dActor_c *actor;
    u32 id;
    int isPlayer;
    Vector3 pos;

    id = mCylinder.otherOwner;
    if (id == 0) return 0;
    actor = dActor_c::FindWithID(id);
    if (actor == 0) goto fail;
    isPlayer = (int)(actor->actorID == 0xbf);
    if (isPlayer != 0) goto body;
fail:
    return 0;
body:
    if (mHorzSpeed != 0) {
        pos.x = mPosX;
        pos.y = mPosY;
        pos.z = mPosZ;
        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(
            actor, &pos, 2, 0xc000, 1, 0, 1);
    }
    return 1;
}

// @symbol _ZN12daBgSnmBdy_c11AdvancePathEv
int daBgSnmBdy_c::AdvancePath()
{
    char *c = (char *)this;
    int node[3];
    int *idx;
    int numNodes;
    /* MATCH: named &mPath / &mPosX / mRadius / mAngleY size-DIFF (29 words). */
    ((PathPtr *)(c + 0x380))->GetNode(*(Vector3 *)node, *(int *)(c + 0x388));
    int dist = Vec3_HorzDist(c + 0x5c, node);
    _Z11UpdateAngleRssis((short *)(c + 0x8e),
                        Vec3_HorzAngle(c + 0x5c, node), 2, 0x600);
    *(short *)(c + 0x94) = *(short *)(c + 0x8e);
    if (dist < *(int *)(c + 0x398)) {
        numNodes = ((PathPtr *)(c + 0x380))->NumNodes();
        /* MATCH: signed increment/compare of mPathNode; u32 ++ size-DIFFs. */
        idx = (int *)(((int)c + 0x388));
        *idx = *idx + 1;
        if (*(int *)(c + 0x388) >= numNodes - 1) return 1;
    }
    return 0;
}

// @symbol _ZN12daBgSnmBdy_c15UpdateRollAngleEv
void daBgSnmBdy_c::UpdateRollAngle()
{
    /* MATCH: (long long)(int)(mRadius << 1) keeps the ROM's widen-from-int shape. */
    int circumference = (int)(((long long)((int)mRadius << 1) *
                       0x3243F6A89LL + 0x80000000LL) >> 32);
    Fix12i turns = _ZN4cstd4fdivEii(mHorzSpeed, circumference);
    mAngleX = (short)(mAngleX +
        (int)(((long long)turns * 0xffff + 0x800) >> 12));
}

// @symbol _ZN12daBgSnmBdy_c18IsPlayerNearCenterEv
int daBgSnmBdy_c::IsPlayerNearCenter()
{
    int bodyDist = Vec3_Dist(&data_ov072_02122b58, &mPosX);
    int playerDist = Vec3_Dist(&data_ov072_02122b58, &mTalkPlayer->mPosX);
    int bodyAngle = Vec3_HorzAngle(&data_ov072_02122b58, &mPosX);
    int playerAngle = Vec3_HorzAngle(&data_ov072_02122b58, &mTalkPlayer->mPosX);
    int sub = bodyAngle - playerAngle;
    if (playerDist < bodyDist) {
        short diff = (short)sub;
        if (diff < 0) diff = -diff;
        if (diff < 0x700) goto ret1;
    }
    if (Vec3_Dist(&data_ov072_02122b58, &mTalkPlayer->mPosX) >= 0x300000)
        goto ret0;
ret1:
    return 1;
ret0:
    return 0;
}
