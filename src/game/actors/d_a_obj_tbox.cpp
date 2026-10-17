//cpp
/* Jolly Roger Bay treasure chest (TREASURE_BOX, actor 0x0d). Each chest
 * has a 1-based order. The wrong order shocks the player and resets the
 * room. The last chest spawns the star.
 *
 * _ZTS 0x0211c4d8, _ZTI 0x0211c4cc (base _ZTI8dActor_c 0x0208e390),
 * _ZTV 0x0211c50c. Was TreasureChest. .text 0x0211a200..0x0211a930.
 * The destructor is inline in daObjTbox_c.h: D1 then D0, no D2.
 * Highest address first.
 */
// leftovers:
// - ModelAnim::SetAnim and dCcAc_c::Init stay C ABI spellings in
//   InitResources. Fix12<int> by value lowers differently as a member call.
// - func_02012790 and func_02012694 stay linker names. State0 calls them
//   with a sound id, and the second also takes a position.
// - data_ov002_0210d9a8 and data_ov002_0210da38 are ov002 handles this TU
//   borrows.
// - g_profile_TREASURE_BOX stays at 0x0211c4e8, outside this text span.

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daObjTbox_c_AssetId {
    ASSET_T_BOX_BMD = 0x04a6,
    ASSET_T_BOX_OPEN_BCA = 0x04a7
};

#include "daObjTbox_c.h"
#include "Player.h"
#include "SharedFilePtr.h"

/* The resource handles as the ROM lays them out: {id, loaded file}. The
   model handle constructs through func_02017acc and destroys through
   func_02017ab4; the animation handle constructs through
   SharedFilePtr::Construct and destroys through SharedFilePtr_Destruct_Anim.
   The manifest aliases those undefined members onto the ROM symbols. */
struct TboxModelFilePtr : SharedFilePtr {
    u32 id; void *ptr;

    TboxModelFilePtr(u32 fileID);
    ~TboxModelFilePtr();
};

struct TboxAnimationFileHandle : SharedFilePtr {
    u32 id; void *ptr;

    TboxAnimationFileHandle(u32 fileID);
    ~TboxAnimationFileHandle();
};

extern "C" {
void Matrix4x3_FromRotationY(Matrix4x3 *, s32);
/* Decrements a u16 in place and yields its new value. The two legacy shards
 * disagreed on the return type -- State0 used it as a statement and declared
 * it void, State2 tests the result. The value-returning form is the more
 * complete observation of the same arm9 routine, and both call sites compile
 * against it unchanged. */
unsigned short DecIfAbove0_Short(unsigned short *p);
void func_02012790(u32 code);
s16 Vec3_HorzAngle(const Vector3 *from, const Vector3 *to);
s32 AngleDiff(s32 a, s32 b);
void func_02012694(u32 code, const Vector3 *pos);
/* The overlay's three {init, behavior} pointer-to-member pairs, filled by the
 * static initializer this TU now carries at 0x0211b59c. */
extern daObjTbox_c::StateFunc data_ov064_0211c98c[][2];
extern TboxModelFilePtr data_ov064_0211c96c;
extern TboxAnimationFileHandle data_ov064_0211c964;
extern SharedFilePtr data_ov002_0210d9a8;
extern SharedFilePtr data_ov002_0210da38;
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
    ModelAnim *self, BCA_File *file, int flags, int speed, u32 startFrame);
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
    dCcAc_c *self, dActor_c *actor, int radius, int height,
    u32 flags, u32 vulnFlags);
}

// @symbol daObjTbox_c_classInit
/* TREASURE_BOX factory. */
extern "C" daObjTbox_c *daObjTbox_c_classInit()
{
    return new daObjTbox_c();
}

// @symbol _ZN11daObjTbox_c13InitResourcesEv
int daObjTbox_c::InitResources()
{
    Model::LoadFile(data_ov002_0210d9a8);
    Model::LoadFile(data_ov002_0210da38);
    mModelAnim.SetFile(
        (BMD_File *)Model::LoadFile(data_ov064_0211c96c), 1, -1);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &mModelAnim,
        (BCA_File *)dExtFrameCtrl_c::LoadFile(data_ov064_0211c964),
        0x40000000, 0x1000, 0);
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
        &mCylinder, this, 0x96000, 0x96000, 0x200004, 0);
    UpdateModelTransform();
    mOrder = param1;
    mStarID = (u32)param1 >> 8;
    if (mStarID != 0xff) {
        mTrackedStarID = TrackStar(mStarID, 2);
    }
    return 1;
}

// @symbol _ZN11daObjTbox_c8BehaviorEv
int daObjTbox_c::Behavior()
{
    CallStateBehavior();
    mCylinder.Clear();
    mCylinder.Update();
    return 1;
}

// @symbol _ZN11daObjTbox_c6RenderEv
int daObjTbox_c::Render()
{
    Model *model = &mModelAnim;
    model->Render(0);
    return 1;
}

// @symbol _ZN11daObjTbox_c16CleanupResourcesEv
/* Four releases: two handles live in this
 * overlay and two in ov002, borrowed from the always-resident module. The
 * ROM's order is own-first, borrowed-after. */
int daObjTbox_c::CleanupResources()
{
    data_ov064_0211c96c.Release();
    data_ov064_0211c964.Release();
    data_ov002_0210d9a8.Release();
    data_ov002_0210da38.Release();
    return 1;
}

// @symbol _ZN11daObjTbox_c17CallStateBehaviorEv
void daObjTbox_c::CallStateBehavior()
{
    s32 index = mState;
    (this->*data_ov064_0211c98c[index][1])();
}

// @symbol _ZN11daObjTbox_c8SetStateEi
void daObjTbox_c::SetState(s32 state)
{
    mState = state;
    s32 index = mState;
    (this->*data_ov064_0211c98c[index][0])();
}

// @symbol _ZN11daObjTbox_c10InitState0Ev
void daObjTbox_c::InitState0()
{
    mModelAnim.currFrame = 0;
}

// @symbol _ZN11daObjTbox_c6State0Ev
void daObjTbox_c::State0()
{
    DecIfAbove0_Short(&mStateTimer);

    if (mStateTimer == 0x58)
        func_02012790(0xe);

    u32 id = mCylinder.otherOwner;
    if (id == 0)
        return;
    if (mStateTimer != 0)
        return;

    Player *player = (Player *)FindWithID(id);
    if (player == 0)
        return;

    int isPlayer = player->actorID;
    isPlayer = isPlayer == 0xbf;
    if (isPlayer != false) {
        Vector3 *playerPosPtr = &player->mPos;
        Vector3 playerPos;
        playerPos.x = playerPosPtr->x;
        playerPos.y = playerPosPtr->y;
        playerPos.z = playerPosPtr->z;
        if (AngleDiff(Vec3_HorzAngle(&mPos, &playerPos), mAngleY) < 0x4000) {
            int openCount = 0;
            int chestCount = 0;
            dActor_c *chest = FindWithActorID(0xd, 0);
            while (chest != 0) {
                chestCount++;
                if (chest != this) {
                    s32 otherState = ((daObjTbox_c *)chest)->mState;
                    int isOpen = 1;
                    if (otherState != 1) {
                        if (otherState != 2)
                            isOpen = 0;
                    }
                    if (isOpen != 0)
                        openCount++;
                }
                chest = FindWithActorID(0xd, chest);
            }

            if (openCount + 1 == mOrder) {
                if (chestCount == mOrder) {
                    SpawnSoundObj(0);
                    mIsLastChest = 1;
                } else {
                    func_02012790(0x26);
                }
                if (player->mIsUnderwater != 0)
                    func_02012694(0x22, &mCamSpacePos);
                else
                    func_02012694(0x20, &mCamSpacePos);
                SetState(1);
            } else {
                mStateTimer = 0x5a;
                if (player->mIsMetal != 0)
                    player->Shock(0);
                else
                    player->Shock(1);

                for (;;) {
                    chest = FindWithActorID(0xd, chest);
                    if (chest == 0)
                        break;
                    if (chest != this)
                        ((daObjTbox_c *)chest)->SetState(0);
                }
            }
        }
    }
}

// @symbol _ZN11daObjTbox_c10InitState1Ev
void daObjTbox_c::InitState1()
{
    if (mIsLastChest != 0)
        mStateTimer = 0x2d;

    mFlags &= ~1;
}

// @symbol _ZN11daObjTbox_c6State1Ev
void daObjTbox_c::State1()
{
    mModelAnim.Advance();
    if (((u32)(mModelAnim.currFrame << 4) >> 0x10) == 0x14) {
        Player *player = ClosestPlayer();

        Vector3 spawnPos;
        spawnPos.x = mPosX;
        spawnPos.y = mPosY;
        spawnPos.z = mPosZ;
        spawnPos.y += 0xc8000;

        if (mIsLastChest == 0) {
            Vector3 numberPos;
            numberPos.x = spawnPos.x;
            numberPos.y = spawnPos.y;
            numberPos.z = spawnPos.z;
            SpawnNumber(numberPos, mOrder, false, 0, 0);
        }

        if (player->mIsUnderwater != 0) {
            dActor_c *bubble = dActor_c::Spawn(
                0x123, 0, spawnPos, 0, mAreaId, -1);
            bubble->unk_0a4 = 0;
            bubble->mVertSpeed = 0x800;
            bubble->unk_0ac = 0;
        }
    }

    if (mModelAnim.Finished() != 0)
        SetState(2);
}

// @symbol _ZN11daObjTbox_c10InitState2Ev
void daObjTbox_c::InitState2()
{
    if (mIsLastChest == 0)
        mFlags |= 1;
}

// @symbol _ZN11daObjTbox_c6State2Ev
void daObjTbox_c::State2()
{
    if (mIsLastChest == 0)
        return;
    if (mStateTimer == 0)
        return;
    if (DecIfAbove0_Short(&mStateTimer) != 0)
        return;

    if (mStarID != 0xff) {
        Vector3 spawnPos;
        spawnPos.x = mPosX;
        spawnPos.y = mPosY;
        spawnPos.z = mPosZ;
        spawnPos.y += 0xc8000;
        UntrackAndSpawnStar(mTrackedStarID, mStarID, spawnPos, 4);
    }

    mFlags |= 1;
}

// @symbol _ZN11daObjTbox_c20UpdateModelTransformEv
void daObjTbox_c::UpdateModelTransform()
{
    Matrix4x3_FromRotationY(&mModelAnim.mat4x3, mAngleY);
    mModelAnim.mat4x3.t.x = mPosX >> 3;
    mModelAnim.mat4x3.t.y = mPosY >> 3;
    mModelAnim.mat4x3.t.z = mPosZ >> 3;
}

// @symbol _ZN11daObjTbox_cD1Ev
// @symbol _ZN11daObjTbox_cD0Ev
/* D1 and D0 come from the inline destructor in daObjTbox_c.h. */

/* The two file-scope resource handles, in retail initializer order: the
   model file (func_02017acc / func_02017ab4), then the animation file
   (SharedFilePtr::Construct / SharedFilePtr_Destruct_Anim). The manifest
   aliases each wrapper ctor/dtor onto those ROM symbols. */
TboxModelFilePtr data_ov064_0211c96c(ASSET_T_BOX_BMD);
TboxAnimationFileHandle data_ov064_0211c964(ASSET_T_BOX_OPEN_BCA);

/* One {init, behavior} pointer-to-member pair per state. mwcc cannot
   link-time-initialize them, so the initializer copies six descriptor
   records in. */
daObjTbox_c::StateFunc data_ov064_0211c98c[3][2] = {
    { &daObjTbox_c::InitState0, &daObjTbox_c::State0 },
    { &daObjTbox_c::InitState1, &daObjTbox_c::State1 },
    { &daObjTbox_c::InitState2, &daObjTbox_c::State2 },
};
