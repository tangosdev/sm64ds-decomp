//cpp
/* daPgRcer_c -- the Cool Cool Mountain racing penguin (PENGUIN_RACER).
 * The path comes from param1. A fall past 0x7d0000 or the cheat volume
 * sets mPlayerCheated; NumStars() == 0x96 selects the big penguin.
 *
 * Remaining reconstruction:
 * - The scalar Fix12 animation, collision, sound and shadow interfaces
 *   retain their ABI declarations.
 * - Actor position and scale triples are scalar members in dActor_c;
 *   vector views retain their established layout and load grouping.
 * - SharedFilePtr has no shared field layout; LOADED reads its file word.
 * - The state table keeps its address name.
 *   The file handles, the state table, both volumes and
 *   g_profile_PENGUIN_RACER are this TU's statics; mwcc emits their
 *   initializer, __sinit_d_a_pg_rcer.cpp (0x021127a4).
 */

#include "daPgRcer_c.h"
#include "Particle__System.h"
#include "ParticleIds.h"
#include "daPly_c.h"
#include "SharedFilePtr.h"
#include "SurfaceInfo.h"
#include "dBgPi.h"
#include "Sound.h"
#include "nitro/fx/fx.h"
#include "math/Approach.h"
#include "SpawnInfo.h"
#include "nitro/fx/fx_cp.h"
#include "math/CalcLib.h"

extern SharedFilePtr *data_ov019_02112788[7];      /* the seven BCA handles */
extern SharedFilePtr *data_ov019_0211277c[3];      /* the three BTP handles */
extern unsigned char data_0209d684;                /* talk-script choice */

unsigned char NumStars(void);
void func_02038414(void *clsn);
extern "C" void Matrix4x3_FromRotationY(Matrix4x3 *m, int angle);

extern "C" void d_a_pg_rcerPlaySubHold(unsigned int a, unsigned int b, unsigned int c, int d,
                                       int e);
extern "C" void *d_a_pg_rcerNewHold(u32 handle, u32 effect, int x, int y, int z, void *dir,
                                    void *cb);

/* Resource handles owned by this TU. The static initializer constructs the
   model, seven animations and three texture sequences and registers each
   destructor. The wrappers' constructor and destructor resolve to the
   retail veneers through the manifest. */
struct PgRcerModelFile : SharedFilePtr {
    PgRcerModelFile(u32);
    ~PgRcerModelFile();
};
struct PgRcerAnimFile : SharedFilePtr {
    PgRcerAnimFile(u32);
    ~PgRcerAnimFile();
};
struct PgRcerTexSeqFile : SharedFilePtr {
    PgRcerTexSeqFile(u32);
    ~PgRcerTexSeqFile();
};

/* Runtime asset handles resolved through the overlay 0 file table. */
enum PgRcerAssetId {
    ASSET_PENGUIN_BMD = 0x03fb,
    ASSET_PENGUIN_ANGRY_BTP = 0x03fd,
    ASSET_PENGUIN_DIVE_BCA = 0x0400,
    ASSET_PENGUIN_JUMP_ED_BCA = 0x0401,
    ASSET_PENGUIN_JUMP_ST_BCA = 0x0402,
    ASSET_PENGUIN_SLIDER_BCA = 0x0403,
    ASSET_PENGUIN_SLIDER_BTP = 0x0404,
    ASSET_PENGUIN_STAND_UP_BCA = 0x0405,
    ASSET_PENGUIN_WAIT1_BCA = 0x0406,
    ASSET_PENGUIN_WAIT1_BTP = 0x0407,
    ASSET_PENGUIN_WALK_BCA = 0x0408
};

/* This TU's statics, in the order the retail initializer (0x021127a4)
   builds them. */
PgRcerModelFile gOV019PenguinModelFile(ASSET_PENGUIN_BMD);
PgRcerAnimFile gOV019PenguinWait1AnimationFile(ASSET_PENGUIN_WAIT1_BCA); /* idle */
PgRcerAnimFile gOV019PenguinWalkAnimationFile(ASSET_PENGUIN_WALK_BCA);
PgRcerAnimFile gPenguinJumpStAnimationFile(ASSET_PENGUIN_JUMP_ST_BCA);           /* jump */
PgRcerAnimFile gPenguinJumpEdAnimationFile(ASSET_PENGUIN_JUMP_ED_BCA);           /* land */
PgRcerAnimFile gPenguinDiveAnimationFile(ASSET_PENGUIN_DIVE_BCA);                /* race start */
PgRcerAnimFile gPenguinSliderAnimationFile(ASSET_PENGUIN_SLIDER_BCA);            /* race */
PgRcerAnimFile gPenguinStandUpAnimationFile(ASSET_PENGUIN_STAND_UP_BCA);         /* near finish */
PgRcerTexSeqFile gOV019PenguinAngryTextureSequenceFile(ASSET_PENGUIN_ANGRY_BTP); /* lost */
PgRcerTexSeqFile gPenguinSliderTextureSequenceFile(ASSET_PENGUIN_SLIDER_BTP);    /* race */
PgRcerTexSeqFile gOV019PenguinWait1TextureSequenceFile(ASSET_PENGUIN_WAIT1_BTP); /* neutral */

/* The six states, {init, behavior}; SetState installs one. */
daPgRcerState data_ov019_0211356c[6] = {
    {&daPgRcer_c::InitStateWaitForMario, &daPgRcer_c::StateWaitForMario},
    {&daPgRcer_c::InitStateShowInitText, &daPgRcer_c::StateShowInitText},
    {&daPgRcer_c::InitStatePrepareForRace, &daPgRcer_c::StatePrepareForRace},
    {&daPgRcer_c::InitStateRace, &daPgRcer_c::StateRace},
    {&daPgRcer_c::InitStateFinishRace, &daPgRcer_c::StateFinishRace},
    {&daPgRcer_c::InitStateShowFinalText, &daPgRcer_c::StateShowFinalText}};

/* The finish volume and the cheat volume. */
Vector3 data_ov019_021134e8(-0x18c28a1, -0x164c644, -0x18f1a11);
Vector3 data_ov019_0211353c(-0x1290000, 0x600000, 0);

daPgRcer_c *daPgRcer_c_classInit() {
    return new daPgRcer_c();
}

/* The actor profile, in SpawnInfo's layout (include/SpawnInfo.h): the
   factory, behavior and render priorities (the behavior priority equals the
   PENGUIN_RACER registry id), actor flags, then the clip offset Y, clip
   radius, clip distance and far distance passed to SetRanges. The retail
   values are unchanged. */
SpawnInfo<daPgRcer_c> g_profile_PENGUIN_RACER = {
    daPgRcer_c_classInit, 0x0103, 0x0075, 2, 0x96000, 0x96000, 0x02648000, 0x02648000};

int daPgRcer_c::InitResources()
{
    int i;

    mAnmModel.SetFile(dExtSimpleModel_c::LoadFile(gOV019PenguinModelFile), 1, 1);

    for (i = 0; i < 7; i++)
        dExtFrameCtrl_c::LoadFile(*data_ov019_02112788[i]);

    for (int j = 0; j < 3; j++) {
        SharedFilePtr *t = data_ov019_0211277c[j];
        dExtAnmTexPat_c::LoadFile(*t);
        dExtAnmTexPat_c::Prepare(*(BMD_File *)gOV019PenguinModelFile.filePtr,
                                 *(BTP_File *)t->filePtr);
    }

    if (mShadowModel.InitCylinder() == 0)
        return 0;

    if (NumStars() == 0x96) {
        mdCcAc_c.Init(this, 0xd0000, 0x12c000, 0x800004, 0);
        mScaleX = 0x1999;
        mScaleY = 0x1000;
        mScaleZ = 0x1999;
        mWithMeshClsn.Init(this, 0xd0000, 0xd0000, 0, 0);
        SetRanges(0xf0000, 0xf0000, 0x1c20000, 0x1c20000);
    } else {
        mdCcAc_c.Init(this, 0x82000, 0x12c000, 0x800004, 0);
        mScaleX = 0x1000;
        mScaleY = 0x1000;
        mScaleZ = 0x1000;
        mWithMeshClsn.Init(this, 0x82000, 0x82000, 0, 0);
    }

    mStarSlot = TrackStar((u8)((param1 >> 8) & 0xf), 2);
    SetState(0);
    mPath.FromID(param1 & 0xff);
    mPathNodeIndex = 0;
    mPath.GetNode(mPos, mPathNodeIndex);
    AdvancePathNode();
    UpdateModelMatrix();
    return 1;
}

int daPgRcer_c::Behavior()
{
    CallStateBehavior();
    mAnmModel.dExtFrameCtrl_c::Advance();
    mTextureSequence.Advance();
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    UpdateModelMatrix();
    return 1;
}

int daPgRcer_c::Render()
{
    mTextureSequence.Update(mAnmModel.data);
    mAnmModel.Render(&mScale);
    return 1;
}

void daPgRcer_c::OnPendingDestroy()
{
}

int daPgRcer_c::CleanupResources()
{
    gOV019PenguinModelFile.Release();
    for (int i = 0; i < 7; i++)
        data_ov019_02112788[i]->Release();
    for (int i = 0; i < 3; i++)
        data_ov019_0211277c[i]->Release();
    return 1;
}

void daPgRcer_c::SetState(int state) {
    mStateDesc = &data_ov019_0211356c[state];
    CallStateInit();
}

void daPgRcer_c::CallStateInit() {
    daPgRcerStateMethod *p = &mStateDesc->init;
    (this->**p)();
}

void daPgRcer_c::CallStateBehavior() {
    daPgRcerStateMethod *p = &mStateDesc->behavior;
    (this->**p)();
}

/* State 0: idle. */
int daPgRcer_c::InitStateWaitForMario() {
    mVertAccel = 0;
    mTerminalVelocity = 0;
    mAnmModel.SetAnim((BCA_File *)gOV019PenguinWait1AnimationFile.filePtr, 0, 0x1000, 0);
    mAnmModel.speed = 0x1000;
    mTextureSequence.SetFile(*(BTP_File *)gOV019PenguinWait1TextureSequenceFile.filePtr, 0, 0x1000,
                             0);
    mAction = 0;
    return 1;
}

int daPgRcer_c::StateWaitForMario() {
    if (DecIfAbove0_Byte(&mTalkWaitTimer))
        return 1;
    daPly_c *pl = ClosestPlayer();
    if (pl == 0)
        return 1;
    int radius = mdCcAc_c.radius;
    if (Vec3_Dist(&mPos, &pl->mPos) < radius + 0x78000) {
        if (pl->StartTalk(*this, 1)) {
            mTalkPlayer = pl;
            SetState(1);
        }
    }
    return 1;
}

/* State 1: invite. */
int daPgRcer_c::InitStateShowInitText() {
    mActionStep = 0;
    func_0201267c(0xdf, &mCamSpacePos);
    mAction = 1;
    return 1;
}

int daPgRcer_c::StateShowInitText() {
    switch (mActionStep) {
    case 0: {
        short ang = Vec3_HorzAngle(&mPos, &mTalkPlayer->mPos);
        if (ApproachLinear(mAngleY, ang, 0x514) != 0) {
            Vector3 pos;
            int msg;
            int eq;
            int y;
            int z;
            pos.x = mPosX;
            y = mPosY;
            pos.y = y;
            z = mPosZ;
            pos.z = z;
            pos.y = y + 0x190000;
            eq = (int)(NumStars() == 0x96);
            if (eq != 0)
                msg = 0xab;
            else
                msg = 0xa7;
            if (mTalkPlayer->ShowMessage(*this, (unsigned int)(short)msg, &pos, 1, 2) != 0) {
                ++mActionStep;
            }
        }
        break;
    }
    case 1:
        if (mTalkPlayer->GetTalkState() == 2) {
            unsigned char choice = data_0209d684;
            if (choice == 1) {
                func_02012790(0x98);
                SetState(2);
            } else if (choice == 2) {
                func_02012790(0x63);
                mTalkWaitTimer = 0x5a;
                mTalkPlayer->HasFinishedTalking();
                SetState(0);
            }
        }
        break;
    }
    return 1;
}

/* State 2: jump into the countdown. */
int daPgRcer_c::InitStatePrepareForRace() {
    mAnmModel.SetAnim((BCA_File *)gPenguinJumpStAnimationFile.filePtr, 0x40000000, 0x1000, 0);
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    mVertSpeed = 0x16000;
    mActionStep = 0;
    Sound::PlaySub(0x1f, 0x14, 0x7f, 0x15666, 0);
    func_0201267c(0x138, &mCamSpacePos);
    mAction = 2;
    return 1;
}

int daPgRcer_c::StatePrepareForRace() {
    switch (mActionStep) {
    case 0:
        if (mTalkPlayer->Unk_020c4f40(0x5a) != 0) {
            ++mActionStep;
        }
        break;
    case 1:
        UpdateAngle(mPrevAngleY, mTargetAngY, 2, 0x800);
        mAngleY = mPrevAngleY;
        UpdatePos(&mdCcAc_c);
        UpdateFloorAndWalls(&mWithMeshClsn);
        if (mWithMeshClsn.JustHitGround() != 0) {
            mAnmModel.SetAnim((BCA_File *)gPenguinJumpEdAnimationFile.filePtr, 0x40000000, 0x1000,
                              0);
            ++mActionStep;
        }
        break;
    case 2:
        if (mAnmModel.dExtFrameCtrl_c::Finished() != 0) {
            mAnmModel.SetAnim((BCA_File *)gOV019PenguinWait1AnimationFile.filePtr, 0, 0x1000, 0);
        }
        if (mTalkPlayer->GetTalkState() == -1) {
            Sound::PlaySub(0x1f, 0x7f, 0, 0x15666, 0);
            func_0201267c(0x4d, &mCamSpacePos);
            SetState(3);
        }
        break;
    }
    return 1;
}

/* State 3: the race. */
int daPgRcer_c::InitStateRace() {
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    mAnmModel.SetAnim((BCA_File *)gPenguinDiveAnimationFile.filePtr, 0x40000000, 0x1000, 0);
    mTextureSequence.SetFile(*(BTP_File *)gPenguinSliderTextureSequenceFile.filePtr, 0, 0x1000, 0);
    mHorzSpeed = 0x30000;
    mActionStep = 0;
    mPlayerFinished = 0;
    mAction = 3;
    return 1;
}

int daPgRcer_c::StateRace() {
    switch (mActionStep) {
    case 0:
        if (FollowPath()) {
            AdvancePathNode();
        }
        if (mAnmModel.dExtFrameCtrl_c::Finished()) {
            mAnmModel.SetAnim((BCA_File *)gPenguinSliderAnimationFile.filePtr, 0, 0x1000, 0);
            mAnmModel.speed = 0x1000;
            mActionStep++;
        }
        UpdatePos(&mdCcAc_c);
        UpdateFloorAndWalls(&mWithMeshClsn);
        if (mPlayerCheated == 0 && mPlayerFinished == 0) {
            {
                Vector3 v;
                v.x = mPosX;
                v.y = mPosY;
                v.z = mPosZ;
                if (IsNearPathNode(&v, mPenguinCheckpoints)) {
                    mPenguinCheckpoints++;
                }
            }
            {
                daPly_c *o = mTalkPlayer;
                const Vector3 *p = &o->mPos;
                Vector3 v;
                v.x = p->x;
                v.y = p->y;
                v.z = p->z;
                if (IsNearPathNode(&v, mPlayerCheckpoints)) {
                    mPlayerCheckpoints++;
                }
            }
        }
        break;

    case 1:
        if (FollowPath()) {
            if (mPathNodeIndex >= (int)mPath.NumNodes() - 2) {
                mAnmModel.SetAnim((BCA_File *)gPenguinStandUpAnimationFile.filePtr, 0x40000000,
                                  0x1000, 0);
                mActionStep++;
            } else {
                AdvancePathNode();
            }
        }
        UpdatePos(&mdCcAc_c);
        UpdateFloorAndWalls(&mWithMeshClsn);
        if (mPlayerCheated == 0 && mPlayerFinished == 0 &&
            mPathNodeIndex < (int)mPath.NumNodes() - 2) {
            {
                Vector3 v;
                v.x = mPosX;
                v.y = mPosY;
                v.z = mPosZ;
                if (IsNearPathNode(&v, mPenguinCheckpoints)) {
                    mPenguinCheckpoints++;
                }
            }
            {
                daPly_c *o = mTalkPlayer;
                const Vector3 *p = &o->mPos;
                Vector3 v;
                v.x = p->x;
                v.y = p->y;
                v.z = p->z;
                if (IsNearPathNode(&v, mPlayerCheckpoints)) {
                    mPlayerCheckpoints++;
                }
            }
        }
        if (mPlayerCheckpoints > 1) {
            int val = GetTargetSpeed(mPlayerCheckpoints - mPenguinCheckpoints);
            if (mPlayerCheated != 0 || mPlayerFinished != 0) {
                val = 0x65000;
            }
            ApproachLinear(mHorzSpeed, val, 0x800);
        }
        if (mPenguinFinished == 0 && Vec3_Dist(&data_ov019_021134e8, &mPos) < 0x190000) {
            if (mPlayerFinished != 0) {
                mPenguinWon = 0;
            } else {
                mPenguinWon = 1;
            }
            mPenguinFinished = 1;
        }
        mSlideSound = Sound::PlayLong(mSlideSound, 3, 0x185, mCamSpacePos, 0);
        mDustParticle = (u32)Particle::System::New(mDustParticle, kPtclSlideDust, mPosX,
                                                   mPosY + 0x32000, mPosZ, 0, 0);
        break;

    case 2:
        if (mAnmModel.dExtFrameCtrl_c::Finished()) {
            SetState(4);
        }
        break;
    }

    CheckFallCheat();
    if (Vec3_Dist(&data_ov019_0211353c, &mTalkPlayer->mPos) < 0x280000) {
        mPlayerCheated = 1;
    }
    if (Vec3_Dist(&data_ov019_021134e8, &mTalkPlayer->mPos) < 0x190000) {
        mPlayerFinished = 1;
    }
    return 1;
}

/* State 4: walk up to the player. */
int daPgRcer_c::InitStateFinishRace() {
    mAnmModel.SetAnim((BCA_File *)gOV019PenguinWalkAnimationFile.filePtr, 0, 0x1000, 0);
    mAnmModel.speed = 0x1000;
    mTextureSequence.SetFile(*(BTP_File *)gOV019PenguinWait1TextureSequenceFile.filePtr, 0, 0x1000,
                             0);
    mActionStep = 0;
    mHorzSpeed = 0x5000;
    mAction = 4;
    return 1;
}

int daPgRcer_c::StateFinishRace() {
    Vector3 node;
    switch (mActionStep) {
    case 0: {
        int n = mPath.NumNodes();
        mPath.GetNode(node, n - 1);
        mPathDist = Vec3_Dist(&mPos, &node);
        mTargetAngY = Vec3_HorzAngle(&mPos, &node);
        ApproachLinear(mAngleY, mTargetAngY, 0x200);
        mPrevAngleY = mAngleY;
        if (mPathDist < mHorzSpeed) {
            mHorzSpeed = mPathDist;
            ++mActionStep;
        }
        UpdatePos(&mdCcAc_c);
        UpdateFloorAndWalls(&mWithMeshClsn);
        {
            unsigned int frame = ((unsigned int)mAnmModel.currFrame << 4) >> 0x10;
            if (frame == 9 || frame == 0x15) {
                func_0201267c(0xde, &mCamSpacePos);
            }
        }
        break;
    }
    case 1: {
        short ang = Vec3_HorzAngle(&mPos, &mTalkPlayer->mPos);
        if (ApproachLinear(mAngleY, ang, 0x514) != 0) {
            SetState(5);
        }
        break;
    }
    }
    {
        unsigned int frame = ((unsigned int)mAnmModel.currFrame << 4) >> 0x10;
        if (frame == 9 || frame == 0x15) {
            func_0201267c(0xf3, &mCamSpacePos);
        }
    }
    return 1;
}

/* State 5: result, and the star if the penguin did not win. */
int daPgRcer_c::InitStateShowFinalText() {
    mAnmModel.SetAnim((BCA_File *)gOV019PenguinWait1AnimationFile.filePtr, 0, 0x1000, 0);
    mAnmModel.speed = 0x1000;
    mActionStep = 0;
    mAction = 5;
    return 1;
}

int daPgRcer_c::StateShowFinalText() {
    switch (mActionStep) {
    case 0: {
        int radius = mdCcAc_c.radius;
        int d = Vec3_Dist(&mPos, &mTalkPlayer->mPos);
        if (d < radius + 0x78000) {
            if (mTalkPlayer->StartTalk(*this, 1) != 0) {
                ++mActionStep;
            }
        }
        break;
    }
    case 1: {
        short ang = Vec3_HorzAngle(&mPos, &mTalkPlayer->mPos);
        if (ApproachLinear(mAngleY, ang, 0x514) != 0) {
            unsigned int id;
            Vector3 v;
            int z = mPosZ;
            int y = mPosY + 0x190000;
            int x = mPosX;
            v.x = x;
            v.y = y;
            v.z = z;
            if (mPlayerCheated != 0) {
                id = 0xa9;
                mTextureSequence.SetFile(*(BTP_File *)gOV019PenguinAngryTextureSequenceFile.filePtr,
                                         0, 0x1000, 0);
                mPenguinWon = 1;
            } else {
                if (mPenguinWon != 0)
                    id = 0xaa;
                else
                    id = 0xa8;
            }
            if (mTalkPlayer->ShowMessage(*this, id, &v, 1, 2) != 0) {
                func_0201267c(0xdf, &mCamSpacePos);
                ++mActionStep;
            }
        }
        break;
    }
    case 2:
        if (mTalkPlayer->GetTalkState() == 2) {
            if (mPenguinWon == 0) {
                UntrackAndSpawnStar(mStarSlot, (unsigned int)(unsigned char)((param1 >> 8) & 0xf),
                                    mPos, 4);
            }
            mTalkPlayer->HasFinishedTalking();
            mTextureSequence.SetFile(*(BTP_File *)gOV019PenguinWait1TextureSequenceFile.filePtr, 0,
                                     0x1000, 0);
            ++mActionStep;
        }
        break;
    }
    return 1;
}

void daPgRcer_c::UpdateModelMatrix() {
    Matrix4x3_FromRotationY(&mAnmModel.mat4x3, mAngleY);
    mAnmModel.mat4x3.t.x = mPosX >> 3;
    mAnmModel.mat4x3.t.y = mPosY >> 3;
    mAnmModel.mat4x3.t.z = mPosZ >> 3;
    DropShadowRadHeight(mShadowModel, mAnmModel.mat4x3, 0x140000, 0x50000, 0xf);
}

int daPgRcer_c::UpdateFloorAndWalls(dBgCh_Actr *clsn) {
    Vector3 floorNormal;
    Vector3 wallNormal;
    func_02038414(clsn);
    if (clsn->IsOnGround()) {
        clsn->GetFloorResult()->surface.CopyNormalTo(floorNormal);
        if (floorNormal.y != 0) {
            int x = FX_MUL(floorNormal.x, mSpeedX);
            int y = FX_MUL(floorNormal.z, mSpeedZ);
            mVertSpeed = -(FX_Div(x + y, floorNormal.y) + 0x8000);
            if (mVertSpeed > 0)
                mVertSpeed = 0;
        }
    }
    if (clsn->IsOnWall()) {
        clsn->GetWallResult()->surface.CopyNormalTo(wallNormal);
    }
}

void daPgRcer_c::AdvancePathNode() {
    Vector3 node;
    mPathNodeIndex = mPathNodeIndex + 1;
    mPath.GetNode(node, mPathNodeIndex);
    mPathDist = Vec3_Dist(&mPos, &node);
    mTargetAngY = Vec3_HorzAngle(&mPos, &node);
}

int daPgRcer_c::FollowPath() {
    UpdateAngle(mPrevAngleY, mTargetAngY, 2, 0x600);
    int n = mPathNodeIndex;
    if (n >= (int)mPath.NumNodes() - 2) {
        mAngleY = mPrevAngleY;
    } else {
        Vector3 node;
        mPath.GetNode(node, n + 1);
        mAngleY = Vec3_HorzAngle(&mPos, &node);
    }
    mPathDist = mPathDist - mHorzSpeed;
    return mPathDist < 0;
}

void daPgRcer_c::CheckFallCheat() {
    if (mPlayerCheated != 0)
        return;
    daPly_c *t = mTalkPlayer;
    if (t->mIsAirborne != 0) {
        int vertSpeed = t->mVertSpeed;
        int terminalVel = t->mTerminalVelocity;
        if (vertSpeed > terminalVel)
            return;
        {
            int d = vertSpeed;
            if (d < 0)
                d = -d;
            mFallAccum = mFallAccum + d;
            if (mFallAccum > 0x7d0000)
                mPlayerCheated = 1;
        }
        return;
    }
    mFallAccum = 0;
}

int daPgRcer_c::IsNearPathNode(Vector3 *pos, unsigned int nodeIndex) {
    Vector3 node;
    mPath.GetNode(node, nodeIndex);
    Fix12i dist = Vec3_HorzDist(&node, pos);
    return dist <= 0x320000;
}

/* The further ahead the player is, in path nodes, the faster the penguin
 * runs. Clamped to 0x30000..0x65000. */
int daPgRcer_c::GetTargetSpeed(int playerLead) {
    int speed = playerLead * 0x54cc + 0x4a800;

    if (speed > 0x65000) {
        speed = 0x65000;
    }
    if (speed < 0x30000) {
        speed = 0x30000;
    }

    return speed;
}
