//cpp
/**
 * Whomp and King Whomp (profiles BATAN / BATANKING, RTTI daBtn_c). One class
 * serves both; mIsKing (actor ID 0xa5) picks the model, the animation set,
 * the hit points and most tunings.
 *
 * Behavior runs one handler per mState out of the pointer-to-member table
 * data_ov079_02128280, which the static initializer fills at load:
 *    0 func_ov079_02125b44  waiting (the king's introduction talk)
 *    1 func_ov079_021258fc  walking
 *    2 func_ov079_021256d4  the king walking
 *    3 func_ov079_021254b4  leaning in
 *    4 func_ov079_0212538c  falling on its face
 *    5 func_ov079_02125240  slamming down
 *    6 func_ov079_02124b08  lying flat, getting up
 *    7 func_ov079_021249f0  turning back
 *    8 func_ov079_021246dc  defeated
 *    9 func_ov079_021246d8  the king beaten, waiting to go
 *   10 func_ov079_02124638  wobbling from a hit in front
 *   11 func_ov079_02124530  knocked over from behind
 * The handlers are daBtn_c methods; their original names are unknown, so they
 * keep the address spellings.
 *
 * The file tables: data_ov079_021275ec holds the plain Whomp's five
 * animation handles and, from data_ov079_02127600 on, the king's six, read
 * as [mIsKing * 5 + n] (slot 3 is the walk, slot 2 the getting-up). The
 * model handles are data_ov079_02128170 (plain) / 02128168 (king), the
 * king's face texture 02128178, the collision meshes data_ov079_02127bf0[2].
 *
 * Known limits:
 * - dActor_c::SpawnCoins/Earthquake, DropShadowScaleXYZ,
 *   dBgW_KcMbg::SetFile and
 *   dBgCh_Actr::Init take Fix12<int> by value, so they stay mangled
 *   (notes/mwccarm-codegen.md 6az).
 * - func_ov079_02123804 reacts to a hit but nothing in the cartridge's
 *   relocations calls it.
 * - func_ov079_02123d4c is really a Vector3-by-value member of daBtn_c, but
 *   that spelling gives the named return local its own stack home (no NRVO,
 *   +0xc): it stays a free function and callers use the vecret view.
 * - func_ov079_0212522c is the mesh collider's contact-callback trampoline;
 *   its first parameter is the collider, not the object, so it stays free.
 *
 * FUNCTION ORDER IS THE REVERSE OF THE ROM'S -- mwccarm emits one .text
 * section per function in the reverse of source order.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_btn_AssetId {
    ASSET_BATTAN_BMD = 0x029c,
    ASSET_BATTAN_KCL = 0x029d,
    ASSET_BATTAN_ATTACK01_BCA = 0x029e,
    ASSET_BATTAN_ATTACK02_BCA = 0x029f,
    ASSET_BATTAN_RECOVER_BCA = 0x02a0,
    ASSET_BATTAN_WALK_BCA = 0x02a1,
    ASSET_BATTAN_WALKSTART_BCA = 0x02a2,
    ASSET_BATTAN_KING_BMD = 0x02a3,
    ASSET_BATTAN_KING_BTP = 0x02a4,
    ASSET_BATTAN_KING_KCL = 0x02a5,
    ASSET_BATTAN_KING_ATTACK01_BCA = 0x02a6,
    ASSET_BATTAN_KING_ATTACK02_BCA = 0x02a7,
    ASSET_BATTAN_KING_DAMAGE_BCA = 0x02a8,
    ASSET_BATTAN_KING_RECOVER_BCA = 0x02a9,
    ASSET_BATTAN_KING_WALK_BCA = 0x02aa,
    ASSET_BATTAN_KING_WALKSTART_BCA = 0x02ab
};

#include "daBtn_c.h"
#include "ParticleIds.h"
#include "math/CalcLib.h"
#include "math/MtxStack.h"
#include "daPly_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "Message.h"
#include "dCamera_c.h"
#include "nitro/fx/fx.h"
#include "Sound.h"
#include "math/Approach.h"
#include "Particle__System.h"
#include "private/mtx43.h"
#include "nitro/fx/fx_trig.h"

/* local extern: include/Sound.h does not declare the layer-3 pair yet. It
   sits outside the extern "C" block below, where the names would come out
   unmangled. */
namespace Sound {
void LoadAndSetMusic_Layer3(unsigned int);
void StopLoadedMusic_Layer3(void);
}

struct CLPS_Block;

/* The matrix copies go through Mtx43, one 0x30-byte block copy. Matrix4x3's
   own assignment goes member by member through Vector3 and comes out longer. */

extern "C" {
/* ---- this TU ---- */
void func_ov079_02123d4c(Vector3 *out, daBtn_c *self);
}

void func_ov079_0212522c(dBgW *collider, dActor_c *owner, dActor_c *other);
int IsStarCollectedInCurLevel(int a);
void *func_0200fa04(void *c, void *v, int b);
void func_02011cfc(void);
void func_02011d38(void);
void func_02012694(unsigned int a, const Vector3 *p);
extern "C" {


/* ---- arm9 / ov002 ---- */
int AngleDiff(int, int);
void Matrix4x3_ApplyInPlaceToScale(void *m, int x, int y, int z);
int func_ov002_020dd8b8(void *c);
}


/* Parameter names keep this TU's @NNN. */
typedef void BgchInitAt_(void *self, void *actor, int a, int b, void *c, void *d);
extern "C" {

extern void hold_d_a_btn(void *mc, void *kcl, void *mtx, int fix, s16 s, void *clps);
void d_a_btnEarthquakeHold(void *self, const void *v, int f);
}


/* ---- data ---- */
extern unsigned int gNextActorUpdateMask;
extern u8 data_0209d660;
extern u8 gNumPlayers;
extern u8 gCurrentStarId;
extern signed char gCurrentSublevel;
extern dCamera_c *gCamera;
extern daPly_c *gPlayers[];
/* The king's collision mesh takes Whomp's Fortress's own CLPS block. The
   relocation's target address is shared by 15 level overlays; residency
   leaves ov015 and ov026, and only ov015 holds a CLPS block there (ov026's
   words are mid-table), so the data settles it. */
extern CLPS_Block data_ov015_021135d4;

extern int data_ov079_021275dc[];
extern int data_ov079_021275e4[];
extern void *data_ov079_021275ec[];
extern void *data_ov079_02127600[];
extern CLPS_Block data_ov079_02127ba0;   /* the plain Whomp's CLPS block */
extern void *data_ov079_02127bf0[];
extern int data_ov079_02127cfc[];

/* File-scope resource handles: each ctor/dtor pair is a ROM SharedFilePtr
   veneer (func_02017acc/func_02017ab4 for the two BMDs, SharedFilePtr::
   Construct/SharedFilePtr_Destruct_TexSeq for the king's face BTP,
   Construct/Destruct_Anim for the eleven BCAs, func_02017b4c/
   SharedFilePtr_Destruct_Clsn for the two KCLs), spelled through
   declared-only subclasses so the static initializer names the real
   entry points. */
struct BtnModelFilePtr : SharedFilePtr {
    BtnModelFilePtr(u32 fileID);
    ~BtnModelFilePtr();
};
struct BtnTextureSeqFileHandle : SharedFilePtr {
    BtnTextureSeqFileHandle(u32 fileID);
    ~BtnTextureSeqFileHandle();
};
struct BtnAnimationFileHandle : SharedFilePtr {
    BtnAnimationFileHandle(u32 fileID);
    ~BtnAnimationFileHandle();
};
struct BtnCollisionFilePtr : SharedFilePtr {
    BtnCollisionFilePtr(u32 fileID);
    ~BtnCollisionFilePtr();
};

BtnModelFilePtr         data_ov079_02128170(ASSET_BATTAN_BMD);   /* the plain Whomp's BMD */
BtnModelFilePtr         data_ov079_02128168(ASSET_BATTAN_KING_BMD);   /* the king's BMD */
BtnTextureSeqFileHandle data_ov079_02128178(ASSET_BATTAN_KING_BTP);   /* the king's face BTP */
BtnAnimationFileHandle  gBattanAttack01AnimationFile(ASSET_BATTAN_ATTACK01_BCA);   /* the plain Whomp's BCAs */
BtnAnimationFileHandle  gBattanAttack02AnimationFile(ASSET_BATTAN_ATTACK02_BCA);
BtnAnimationFileHandle  gBattanRecoverAnimationFile(ASSET_BATTAN_RECOVER_BCA);
BtnAnimationFileHandle  gBattanWalkAnimationFile(ASSET_BATTAN_WALK_BCA);
BtnAnimationFileHandle  gBattanWalkstartAnimationFile(ASSET_BATTAN_WALKSTART_BCA);
BtnAnimationFileHandle  gBattanKingAttack01AnimationFile(ASSET_BATTAN_KING_ATTACK01_BCA);   /* the king's BCAs */
BtnAnimationFileHandle  gBattanKingAttack02AnimationFile(ASSET_BATTAN_KING_ATTACK02_BCA);
BtnAnimationFileHandle  gBattanKingRecoverAnimationFile(ASSET_BATTAN_KING_RECOVER_BCA);
BtnAnimationFileHandle  gBattanKingWalkAnimationFile(ASSET_BATTAN_KING_WALK_BCA);
BtnAnimationFileHandle  gBattanKingWalkstartAnimationFile(ASSET_BATTAN_KING_WALKSTART_BCA);
BtnAnimationFileHandle  data_ov079_021281b0(ASSET_BATTAN_KING_DAMAGE_BCA);
BtnCollisionFilePtr     gBattanCollisionFile(ASSET_BATTAN_KCL);   /* the plain Whomp's KCL */
BtnCollisionFilePtr     gBattanKingCollisionFile(ASSET_BATTAN_KING_KCL);   /* the king's KCL */

/* The twelve state handler records live in .data (0x02127bc0..0x02127c28);
   the static initializer copies them into the table below, one member
   pointer per mState. */

extern daBtn_c::StateFunc data_ov079_02127c00;   /* 0 func_ov079_02125b44 */
extern daBtn_c::StateFunc data_ov079_02127bd0;   /* 1 func_ov079_021258fc */
extern daBtn_c::StateFunc data_ov079_02127c10;   /* 2 func_ov079_021256d4 */
extern daBtn_c::StateFunc data_ov079_02127bd8;   /* 3 func_ov079_021254b4 */
extern daBtn_c::StateFunc data_ov079_02127bc0;   /* 4 func_ov079_0212538c */
extern daBtn_c::StateFunc data_ov079_02127bc8;   /* 5 func_ov079_02125240 */
extern daBtn_c::StateFunc data_ov079_02127c18;   /* 6 func_ov079_02124b08 */
extern daBtn_c::StateFunc data_ov079_02127c20;   /* 7 func_ov079_021249f0 */
extern daBtn_c::StateFunc data_ov079_02127be8;   /* 8 func_ov079_021246dc */
extern daBtn_c::StateFunc data_ov079_02127bf8;   /* 9 func_ov079_021246d8 */
extern daBtn_c::StateFunc data_ov079_02127be0;   /* 10 func_ov079_02124638 */
extern daBtn_c::StateFunc data_ov079_02127c08;   /* 11 func_ov079_02124530 */


daBtn_c::StateFunc data_ov079_02128280[12] = {
    data_ov079_02127c00, data_ov079_02127bd0, data_ov079_02127c10,
    data_ov079_02127bd8, data_ov079_02127bc0, data_ov079_02127bc8,
    data_ov079_02127c18, data_ov079_02127c20, data_ov079_02127be8,
    data_ov079_02127bf8, data_ov079_02127be0, data_ov079_02127c08,
};

/* OnAimedAtWithEggReturnVec's view of func_ov079_02123d4c: the (out, this)
   pair of the definition is exactly the hidden-return-pointer shape of a
   Vector3-by-value call, so this view lets the virtual forward to it. */
namespace vecret { extern "C" Vector3 func_ov079_02123d4c(void *thiz); }

daBtn_c *daBtn_c_classInit_BATAN()
{
    return new daBtn_c();
}

daBtn_c *daBtn_c_classInit_BATANKING()
{
    return new daBtn_c();
}

/* Load the class's model, animations (and the king's face texture), set up
   the shadow, the collision mesh and the wall probe, record the spawn pose,
   and start waiting (state 0) on the walk animation. The king claims its
   star marker, and does not come back once that star is collected. */
int daBtn_c::InitResources()
{
    u16 id;
    int animIdx;
    u8 idx;
    void *bmd;
    void *kcl;
    void *anim;
    int i;

    id = actorID;
    int b = (id == 0xa5);
    if (b) {
        mIsKing = 1;
        bmd = dExtSimpleModel_c::LoadFile(data_ov079_02128168);
        mAnmModel.SetFile((BMD_File *)bmd, 1, -1);
        animIdx = 0;
        do {
            anim = data_ov079_02127600[animIdx];
            dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)anim);
            animIdx += 1;
        } while (animIdx < 6);
        dExtAnmTexPat_c::LoadFile(data_ov079_02128178);
    } else {
        mIsKing = 0;
        bmd = dExtSimpleModel_c::LoadFile(data_ov079_02128170);
        mAnmModel.SetFile((BMD_File *)bmd, 1, -1);
        animIdx = 0;
        do {
            anim = data_ov079_021275ec[animIdx];
            dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)anim);
            animIdx += 1;
        } while (animIdx < 5);
    }

    idx = mIsKing;
    kcl = dBgW_Kc::LoadFile(*(SharedFilePtr *)data_ov079_02127bf0[idx]);
    (void)kcl;

    if (mShadowModel.InitCuboid() == 0) {
        return 0;
    }

    if (mIsKing != 0) {
        mHitPoints = 3;
        dExtAnmTexPat_c::Prepare(*(BMD_File *)data_ov079_02128168.filePtr,
                                 *(BTP_File *)data_ov079_02128178.filePtr);
        mTextureSequence.SetFile(*(BTP_File *)data_ov079_02128178.filePtr, 0, 0x1000, 0);
        mTextureSequence.SetFlags(0x40000000);
        mTextureSequence.speed = 0x1000;
        mTextureSequence.currFrame = 0;
        mStarID = (u8)(param1 & 0xf);
        mTrackStarID = TrackStar(mStarID, 2);
    } else {
        mHitPoints = 1;
    }

    func_ov079_02124188();
    func_ov079_02124008();

    idx = mIsKing;
    if (idx == 0) {
        ((dBgW_KcMbg *)(&mMovingMeshCollider))
            ->dBgW_KcMbg::SetFile(
                (KCL_File *)(((SharedFilePtr *)data_ov079_02127bf0[idx])->filePtr), mMeshMatrix,
                0x199, mAngleY, *(CLPS_Block *)(&data_ov079_02127ba0));
    } else {
        ((dBgW_KcMbg *)(&mMovingMeshCollider))
            ->dBgW_KcMbg::SetFile(
                (KCL_File *)(((SharedFilePtr *)data_ov079_02127bf0[idx])->filePtr), mMeshMatrix,
                0x1000, mAngleY, *(CLPS_Block *)(&data_ov015_021135d4));
    }

    mMovingMeshCollider.SetShallowOnly();
    mMovingMeshCollider.SetBeforeClsnCallback(&dBgW::UpdatePosAndAngs);

    {
        s32 sp[3];
        sp[0] = 0;
        sp[1] = 0;
        sp[2] = 0x1000;
        mMovingMeshCollider.SetDepthDir((const Vector3 *)sp);
    }

    mMovingMeshCollider.SetDepthDirMinDot(0xb50);

    mWithMeshClsn.Init(this, 0x32000, 0x32000, 0, 0);

    mState = 0;
    mTargetAngle = mPrevAngleY;
    mSubState = 0;
    mSubTimer = 0;

    idx = mIsKing;
    anim = ((SharedFilePtr *)data_ov079_021275ec[idx * 5 + 3])->filePtr;
    mAnmModel.SetAnim((BCA_File *)anim, 0, 0x1000, 0);

    mSpawnPos.x = mPosX;
    mSpawnPos.y = mPosY;
    mSpawnPos.z = mPosZ;
    mSpawnAngleX = mAngleX;
    mSpawnAngleY = mAngleY;
    mSpawnAngleZ = mAngleZ;

    if (mIsKing != 0) {
        mVertAccel = -0x8000;
    } else {
        mVertAccel = -0x4800;
    }
    mTerminalVelocity = -0x31000;

    mPounded = 0;
    mTouched = 0;

    if (mIsKing != 0) {
        mCoinsLeft = 4;
    } else {
        mCoinsLeft = 6;
    }

    mTipLift = 0;
    mShouldRender = 1;
    unk_407 = 0;
    mResumePending = 0;
    mStateTimer = 0;

    i = 0;
    do {
        mPlayers[i] = 0;
        i += 1;
    } while (i < 4);

    {
        s32 cnt = gNumPlayers;
        if (cnt > 0) {
            i = 0;
            do {
                daPly_c *v = gPlayers[i];
                mPlayers[i] = v;
                i += 1;
            } while (i < cnt);
        }
    }

    if (mIsKing != 0 && gCurrentSublevel == 7 && gCurrentStarId != 1 &&
        IsStarCollectedInCurLevel(1) != 0) {
        MarkForDestruction();
        return 0;
    }

    return 1;
}

/* Per frame: the king claims the camera's target while a player is within
   0x1770000 (except once beaten), then survey the players, move, keep off
   ledges, collide with the walls, run the state handler, pose the model and
   shadow, and pose the collision mesh whenever it is live. */
int daBtn_c::Behavior()
{
    if (mIsKing != 0 && mState != 9) {
        if (DistToCPlayer() < 0x1770000) {
            /* gCamera is the camera (symbols/verified.tsv: CAMERA).
               While the player is within 0x1770000 the king asks the camera
               to frame him each frame, as daBombking_c::Behavior does. */
            gCamera->mFramedActor = this;
        }
    }

    func_ov079_02123f34();
    UpdatePos(0);

    if (mHorzSpeed != 0) {
        if (IsGoingOffCliff(mWithMeshClsn, 0x3c000, (short)0x2888, false, false, 0x32000)) {
            /* The step would leave the ledge: rewind to last frame's position. */
            mPosX = mSafePosX;
            mPosY = mSafePosY;
            mPosZ = mSafePosZ;
        } else {
            mSafePosX = mPosX;
            mSafePosY = mPosY;
            mSafePosZ = mPosZ;
        }
    } else {
        mSafePosX = mPosX;
        mSafePosY = mPosY;
        mSafePosZ = mPosZ;
    }

    UpdateWMClsn(mWithMeshClsn, 0);

    {
        int idx = mState;
        StateFunc *step = &data_ov079_02128280[idx];
        (this->**step)();

        {
            /* mStateTimer counts frames spent in the current state: it is
               incremented here and reset the moment the handler changed mState. */
            mStateTimer++;
            if (idx != mState) {
                mStateTimer = 0;
                mSubState = 0;
            }
        }
    }

    func_ov079_02124188();

    if (func_ov079_021243e0(0) == 0 || func_ov079_02123a8c() != 0) {
        func_ov079_02124008();
    }

    mTouched = 0;
    return 1;
}

int daBtn_c::Render()
{
    if (mShouldRender == 0)
        return 1;
    if (mIsKing != 0)
        mTextureSequence.Update(mAnmModel.data);
    mAnmModel.Render(0);
    return 1;
}

int daBtn_c::CleanupResources()
{
  int i;
  if(mMovingMeshCollider.IsEnabled())
    mMovingMeshCollider.Disable();
  ((SharedFilePtr *)(data_ov079_02127bf0[mIsKing]))->Release();
  if(mIsKing){
    ((SharedFilePtr *)&data_ov079_02128168)->Release();
    for(i=0;i<6;i++)
      ((SharedFilePtr *)(data_ov079_02127600[i]))->Release();
    ((SharedFilePtr *)&data_ov079_02128178)->Release();
  } else {
    ((SharedFilePtr *)&data_ov079_02128170)->Release();
    for(i=0;i<5;i++)
      ((SharedFilePtr *)(data_ov079_021275ec[i]))->Release();
  }
  return 1;
}

/* State 0, waiting. A plain Whomp wakes (state 1) when a player comes
   within 0x3e8000. The king instead holds its introduction: once the
   nearest player stands close (0x190000) and not far below it, start a
   talk, turn to face them, show message 0xa4, swap the music while the
   message box is up, and after the talk ends start walking (state 2). */
void daBtn_c::func_ov079_02125b44()
{
    Vector3 pos;

    if (mAnmModel.Finished() != 0) {
        mAnmModel.SetAnim(
            (BCA_File *)(((SharedFilePtr *)data_ov079_021275ec[mIsKing * 5 + 3])->filePtr), 0,
            0x1000, 0);
    }

    if (mIsKing != 0) {
        mAnmModel.Advance();
        mAnmModel.speed = 0x1000;

        switch (mSubState) {
        case 0:
            if (mNearestDist < 0x190000) {
                daPly_c *p = mPlayers[mNearestPlayer];
                if (p->mPosY >= mPosY - 0xa000) {
                    if (p->StartTalk(*this, true) != 0) {
                        mSubState++;
                    }
                    break;
                }
            }
            mHitPoints = 3;
            break;
        case 1:
            if (ApproachLinear(mPrevAngleY, mTargetAngle, 0x800) != 0) {
                mSubState++;
            }
            mAngleY = mPrevAngleY;
            break;
        case 2:
            if (mPlayers[mNearestPlayer]->GetTalkState() == 0) {
                u16 ang;
                int y;
                {
                    daPly_c *p = mPlayers[mNearestPlayer];
                    int *src = &p->mPosX;
                    pos.x = src[0];
                    pos.y = src[1];
                    pos.z = src[2];
                }
                ang = ::Vec3_HorzAngle(&pos, &mPos);
                y = pos.y + 0x96000;
                pos.x = FX_SinIdx((u16)ang >> 4) * 0xd7 + pos.x;
                pos.z = FX_CosIdx((u16)ang >> 4) * 0xd7 + pos.z;
                pos.y = y;
                if (mPlayers[mNearestPlayer]->ShowMessage(*this, 0xa4, &pos, 0, 0) != 0) {
                    mSubState++;
                    Message::PrepareTalk();
                    func_0201267c(0x133, &mCamSpacePos);
                }
            }
            break;
        case 3:
            if (data_0209d660 != 0) {
                mSubState++;
            }
            break;
        case 4:
            if (data_0209d660 == 0) {
                Sound::LoadAndSetMusic_Layer3(0x2d);
                func_02011d38();
                Message::EndTalk();
                mSubState++;
            }
            break;
        case 5:
            if (mPlayers[mNearestPlayer]->GetTalkState() == -1) {
                mSubState++;
            }
            break;
        case 6:
            mState = 2;
            mAnmModel.SetAnim(
                (BCA_File *)(((SharedFilePtr *)data_ov079_021275ec[mIsKing * 5 + 3])->filePtr), 0,
                0x1000, 0);
            break;
        }
        func_ov079_02125504();
    } else {
        if (mNearestDist < 0x3e8000) {
            mState = 1;
            mAnmModel.SetAnim(
                (BCA_File *)(((SharedFilePtr *)data_ov079_021275ec[mIsKing * 5 + 3])->filePtr), 0,
                0x1000, 0);
        }
        mAnmModel.Advance();
    }
}

/* State 1, the plain Whomp's walk: advance on the walk cycle (standing
   still for its first frames), play the footsteps, and head back (state 7)
   once 0x2bc000 from the spawn point -- 0xc8000 in level 0x27. A player
   dead ahead within 0xc8000 starts the lean (state 3); within 0x5dc000 the
   pace picks up. */
void daBtn_c::func_ov079_021258fc()
{
    unsigned int frame;
    int king;   /* an int copy of idx: indexing data_ov079_021275dc by the u8 changes the code */
    u8 idx;
    unsigned int q;
    void *walk;
    BCA_File *cur;
    void *want;
    int lim;

    idx = mIsKing;
    q = mAnmModel.currFrame;
    walk = data_ov079_021275ec[((int)idx * 5) + 3];
    cur = mAnmModel.file;
    want = ((SharedFilePtr *)walk)->filePtr;
    frame = (unsigned short)(q >> 12);

    if (cur != want) {
        if (frame < 3) {
            mHorzSpeed = 0;
        } else {
            king = idx;
            mHorzSpeed = data_ov079_021275dc[king];
        }
        if (mAnmModel.Finished() != 0) {
            mAnmModel.SetAnim(
                (BCA_File *)(((SharedFilePtr *)data_ov079_021275ec[mIsKing * 5 + 3])->filePtr), 0,
                0x1000, 0);
        }
    } else {
        king = idx;
        mHorzSpeed = data_ov079_021275dc[king];
    }

    if (gCurrentSublevel == 0x27)
        lim = 0xc8000;
    else
        lim = 0x2bc000;
    mAnmModel.speed = 0x1000;
    if (::Vec3_HorzDist(&mPos, &mSpawnPos) > lim) {
        mState = 7;
        mAnmModel.SetAnim(
            (BCA_File *)(((SharedFilePtr *)data_ov079_021275ec[mIsKing * 5 + 3])->filePtr), 0,
            0x1000, 0);
        mFlags &= ~0x2000000;
    } else {
        if (mFrontDist < 0x5dc000) {
            mHorzSpeed = 0x9000;
            mAnmModel.speed = data_ov079_021275dc[mIsKing];
        }
        if (mAnmModel.file == ((SharedFilePtr *)data_ov079_021275ec[mIsKing * 5 + 3])->filePtr) {
            if (mFrontDist < 0xc8000) {
                mState = 3;
                mAnmModel.SetAnim(
                    (BCA_File *)(((SharedFilePtr *)data_ov079_021275ec[mIsKing * 5])->filePtr),
                    0x40000000, 0x1000, 0);
            }
        }
    }

    if (mAnmModel.file == ((SharedFilePtr *)data_ov079_021275ec[mIsKing * 5 + 3])->filePtr) {
        if (frame <= 7 || (frame >= 0x1e && frame <= 0x26)) {
            mHorzSpeed = 0;
            if (mStepPlayed == 0) {
                mStepPlayed = 1;
                func_0201267c(0xc5, &mCamSpacePos);
            }
        } else {
            mStepPlayed = 0;
        }
    }

    mAnmModel.Advance();
}

/* State 2, the king's walk: turn toward the nearest player and shove them
   aside (func_ov079_02123bcc), speed up within 0x5dc000 and lean (state 3)
   at a non-mega player dead ahead within 0x190000. If the nearest player
   ends up more than 0x37f000 below, give up: restore the music and put the
   king back where and how it spawned, waiting (state 0). */
void daBtn_c::func_ov079_021256d4()
{
    daPly_c *r;

    if (mAnmModel.file != ((SharedFilePtr *)data_ov079_021275ec[mIsKing * 5 + 3])->filePtr) {
        if (mAnmModel.Finished() != 0) {
            mAnmModel.SetAnim(
                (BCA_File *)(((SharedFilePtr *)data_ov079_021275ec[mIsKing * 5 + 3])->filePtr), 0,
                0x1000, 0);
        }
    }

    mHorzSpeed = data_ov079_021275dc[mIsKing];
    mAnmModel.speed = 0x1000;
    func_ov079_02123bcc();

    ApproachLinear(mPrevAngleY, mTargetAngle, 0x200);
    mAngleY = mPrevAngleY;

    if (mStateTimer > 0x1e) {
        if (mFrontDist < 0x5dc000) {
            mHorzSpeed = 0xd800;
            mAnmModel.speed = 0x3000;
        }
        r = mPlayers[mFrontPlayer];
        if (r->mIsMega == 0) {
            if (mFrontDist < 0x190000) {
                mAnmModel.SetAnim(
                    (BCA_File *)(((SharedFilePtr *)data_ov079_021275ec[mIsKing * 5])->filePtr),
                    0x40000000, 0x1000, 0);
                mState = 3;
            }
        }
    }

    func_ov079_02125504();
    mAnmModel.Advance();

    r = mPlayers[mNearestPlayer];
    if (r != 0) {
        if (mPosY - r->mPosY <= 0x37f000)
            return;
    }

    {
        s16 t;
        void *anim;
        int five;
        void **tab;
        dExtAnmModel_c *ma;

        Sound::StopLoadedMusic_Layer3();
        func_02011cfc();
        mState = 0;
        mPosX = mSpawnPos.x;
        mPosY = mSpawnPos.y;
        five = 5;
        mPosZ = mSpawnPos.z;
        tab = data_ov079_021275ec;
        t = mSpawnAngleX;
        ma = &mAnmModel;
        mAngleX = t;
        t = mSpawnAngleY;
        mAngleY = t;
        t = mSpawnAngleZ;
        mAngleZ = t;
        t = mSpawnAngleX;
        mPrevAngleX = t;
        t = mSpawnAngleY;
        mPrevAngleY = t;
        t = mSpawnAngleZ;
        mPrevAngleZ = t;
        mTextureSequence.currFrame = 0;
        mHorzSpeed = 0;
        anim = ((SharedFilePtr *)tab[mIsKing * five + 3])->filePtr;
        ((dExtAnmModel_c *)ma)->SetAnim((BCA_File *)anim, 0, 0x1000, 0);
    }
}

/* The walk cycle's footfalls: on the walk animation, frames 0..7 and
   0x1e..0x26 are a foot coming down -- stop, and once per footfall play the
   step, shake the camera (unless flag 0x800000 of gNextActorUpdateMask is set) and
   kick up dust beside the body, left or right by the frame. */
void daBtn_c::func_ov079_02125504()
{
    unsigned int frame;
    Vector3 v[2];
    s16 t;
    s32 px, pz;

    {
        unsigned int q;
        u8 i;
        SharedFilePtr *e;
        BCA_File *cur;
        void *want;
        i = mIsKing;
        q = mAnmModel.currFrame;
        e = (SharedFilePtr *)data_ov079_021275ec[(int)i * 5 + 3];
        cur = mAnmModel.file;
        want = e->filePtr;
        frame = (unsigned short)(q >> 12);
        if (cur != want)
            return;
    }

    if (frame <= 7 || (frame >= 0x1e && frame <= 0x26)) {
        mHorzSpeed = 0;
        if (mStepPlayed != 0)
            return;
        mStepPlayed = 1;
        func_0201267c(0xc5, &mCamSpacePos);

        if ((gNextActorUpdateMask & 0x800000) == 0) {
            v[1].x = mPosX;
            v[1].y = mPosY;
            v[1].z = mPosZ;
            Earthquake(v[1], 0x320000);
        }

        {
            s16 a0 = mAngleY;
            s16 a1 = mBodyYaw;
            s32 py;
            px = mPosX;
            v[0].x = px;
            py = mPosY;
            t = (s16)(a0 + a1 + 0x4000);
            v[0].y = py;
            pz = mPosZ;
            v[0].z = pz;
            v[0].y = py + 0x28000;
        }

        if (frame <= 7) {
            s16 sn = FX_SinIdx((u16)t >> 4);
            s16 cs = FX_CosIdx((u16)t >> 4);
            v[0].x = px - sn * 0xaa;
            v[0].z = pz - cs * 0xaa;
        } else {
            s16 sn = FX_SinIdx((u16)t >> 4);
            s16 cs = FX_CosIdx((u16)t >> 4);
            v[0].x = px + sn * 0xaa;
            v[0].z = pz + cs * 0xaa;
        }
        Particle::System::NewSimple(kPtclLandingDust, v[0].x, v[0].y, v[0].z);
    } else {
        mStepPlayed = 0;
    }
}

/* State 3, the lean: stand still until the topple animation reaches frame
   15, then start falling (state 4). */
void daBtn_c::func_ov079_021254b4()
{
    mHorzSpeed = 0;
    mAnmModel.Advance();
    if (!mAnmModel.WillHitFrame(15))
        return;
    mState = 4;
    mFlags |= 0x2000000;
}

/* State 4, the fall: keep the mesh collider live, hop on the first frame,
   and from frame 8 tip forward with a growing mPitchSpeed until the body is
   flat (mAngleX 0x4000), which lands it in state 5. The mesh is raised with
   the tilt so the face stays on the ground. */
void daBtn_c::func_ov079_0212538c()
{
    mMovingMeshCollider.ClearShallowOnly();
    mMovingMeshCollider.SetDepthByDir();
    mAnmModel.Advance();
    if (mStateTimer == 0)
        mVertSpeed = data_ov079_021275e4[mIsKing];
    if (mStateTimer < 8)
        return;
    if (mIsKing) {
        mPitchSpeed += 0x130;
    } else {
        mPitchSpeed += 0x100;
    }
    mAngleX += mPitchSpeed;
    if (mAngleX > 0x4000) {
        mPitchSpeed = 0;
        mAngleX = 0x4000;
        mState = 5;
        func_0201267c(0xc6, &mCamSpacePos);
    }
    if (mIsKing) {
        mTipLift = (s16)mAngleX * (s16)0x15;
        if (mTipLift >= 0x53000)
            mTipLift = 0x53000;
    } else {
        mTipLift = (s16)mAngleX * (s16)0xa;
        if (mTipLift >= 0x25000)
            mTipLift = 0x25000;
    }
}

/* State 5, the slam: once the face hits the ground, stop falling, freeze
   the mesh collider, shake the camera, start the lying-down animation and
   move to state 6; from then on the collider's contact callback is the
   veneer above. The king lands with a heavier dust burst. */
void daBtn_c::func_ov079_02125240(){
    Vector3 v;
    Vector3 a;
    Vector3 b;
    if (mSubState)
        return;
    if (!mWithMeshClsn.IsOnGround())
        return;
    mVertSpeed = 0;
    mMovingMeshCollider.ClearDepthByDir();
    mMovingMeshCollider.SetShallowOnly();
    mSubState++;
    mAnmModel.SetAnim(
        (BCA_File *)(((SharedFilePtr *)data_ov079_021275ec[mIsKing * 5 + 1])->filePtr), 0x40000000,
        0x1000, 0);
    v.x = mPosX;
    v.y = mPosY;
    v.z = mPosZ;
    Earthquake(v, 0x7d0000);
    mState = 6;
    mMovingMeshCollider.SetStoodOnCallback(func_ov079_0212522c);
    if (mIsKing != 0) {
        func_ov079_02123d4c(&a, this);
        func_0200fa04(this, &a, 0);
    } else {
        func_ov079_02123d4c(&b, this);
        LandingDustAt(b, false);
    }
}

/* The mesh collider's contact callback while the body lies flat: drops the
   collider argument and forwards (actor, other). long_calls emits the pooled
   `ldr ip,[pc,#8]; bx ip` absolute tail-call; bracketed closed immediately
   because it is positional. */
#pragma long_calls on
void func_ov079_0212522c(dBgW *collider, dActor_c *owner, dActor_c *other) {
    static_cast<daBtn_c *>(owner)->func_ov079_02125058(other);
}
#pragma long_calls off

/* Contact from the lying body's back. Only players count. A contact that
   func_ov002_020dd8b8 accepts is a hit: it arms mPounded and remembers the
   player (the king ignores it while a resumed state 0 is pending). Any
   other touch pays out one coin per contact while mCoinsLeft lasts -- for
   the king only when the player stands at least 0x2bc000 above it in
   states 1 and 2. */
void daBtn_c::func_ov079_02125058(dActor_c *other)
{
    /* v1/v2 take the player's position; w1/w2 are the copies handed to
       SpawnCoins. All four are stored, as in the ROM. */
    Vector3 v1, v2;
    Vector3 w1, w2;
    int eq = (int)(other->actorID == 0xbf);
    if (!eq) return;
    mTouched = 1;
    if (func_ov002_020dd8b8(other) != 0) {
        if (mResumePending != 0 && mIsKing != 0) {
            if (mResumeState == 0) return;
        }
        mPounded = 1;
        mPounder = (daPly_c *)other;
        return;
    }
    if (mCoinGiven != 0) return;
    if (mIsKing == 0) {
        if (DecIfAbove0_Byte(&mCoinsLeft) == 0) return;
        const Vector3 &p = other->mPos;
        v1 = p;
        w1 = v1;
        SpawnCoins(w1, 1, 0x8000, 0);
        mCoinGiven = 1;
        return;
    }
    {
        u32 t = mState;
        if (t - 1 > 1) return;
        const Vector3 &p = other->mPos;
        v2 = p;
        if (v2.y - mPosY < 0x2bc000)
            return;
        if (DecIfAbove0_Byte(&mCoinsLeft) == 0) return;
        w2 = v2;
        SpawnCoins(w2, 1, 0x8000, 0);
        mCoinGiven = 1;
        return;
    }
}

/* The king's lying-down phase of state 6. A ground-pound on its back
   (mPounded) costs a hit point, with a sound and a burst at the player's
   feet: the last one defeats it (state 8), any other swaps the face texture
   and moves on to the hurt shake -- ten frames of bobbing -- before phase
   10 gets it up. */
void daBtn_c::func_ov079_02124ed4()
{
    mAnmModel.Advance();
    mCoinGiven = 0;
    if (mSubState == 0) {
        if (mPounded != 0) {
            mPounded = 0;
            mHitPoints--;
            func_02012694(0x134,(const Vector3 *)(&mCamSpacePosX));
            {
                s32 *p = &mPounder->mPosX;
                Particle::System::NewSimple(kPtclWhompPounded, p[0], p[1], p[2]);
            }
            if (mHitPoints == 0) {
                mAnmModel.SetAnim((BCA_File *)data_ov079_021281b0.filePtr, 0x40000000, 0x1000, 0);
                mState = 8;
            } else {
                mAnmModel.SetAnim((BCA_File *)data_ov079_021281b0.filePtr, 0x40000000, 0x1000, 0);
                mPounder = 0;
                mTextureSequence.Advance();
                mSubState++;
            }
        }
        mSubTimer = 0;
        return;
    } else {
        unsigned short v;
        mSubTimer++;
        v = mSubTimer;
        if (v <= 10) {
            if ((int)v % 2) {
                mPosY += 0x10000;
            } else {
                mPosY -= 0x10000;
            }
            return;
        }
        mSubState = 10;
        mSubTimer = 0;
    }
}

/* The plain Whomp's lying-down phase of state 6: a ground-pound on its
   back (mPounded) pays out five coins and defeats it (state 8); a plain
   touch only advances the phase, and letting go resets it. */
void daBtn_c::func_ov079_02124dec()
{
    /* v1 is a second copy of the coin point that the ROM also stores;
       dropping it shrinks the function 0xe8 -> 0xd8. */
    Vector3 v1;
    Vector3 w1;
    mAnmModel.Advance();
    if (mSubState == 0) {
        if (mTouched != 0) {
            if (mPounded != 0) {
                int x = mPosX;
                v1.x = x;
                int y = mPosY;
                v1.y = y;
                int z = mPosZ;
                y = y + 0x78000;

                w1.x = x;
                v1.z = z;
                w1.z = z;
                v1.y = y;
                w1.y = y;

                SpawnCoins(w1, 5, 0x8000, 0);
                mState = 8;
            } else {
                mSubState++;
            }
        } else {
            mCoinGiven = 0;
        }
    } else {
        if (mTouched == 0)
            mSubState = 0;
    }
}

/* State 6, lying flat. Phases below 10 belong to the class's own handler
   (the king's hit counting, the plain Whomp's coin/defeat check); after
   100 frames (120 for the king), or 30 once unk_407 is armed, the body
   starts to get up (phase 11). Phase 10, after the king's hurt shake,
   waits 30 frames and gets up too. Phase 11 pitches back up to level, then
   either resumes the state a hit interrupted or walks again (state 1, or 2
   for the king); the mesh lift follows a per-frame table meanwhile. */
void daBtn_c::func_ov079_02124b08()
{
    u8 st = mSubState;

    if (st < 0xa) {
        mHorzSpeed = 0;
        mPitchSpeed = 0;
        unk_3cc = 0;
        unk_3d0 = 0;
        if (mIsKing != 0)
            func_ov079_02124ed4();
        else
            func_ov079_02124dec();

        {
            int lim = mIsKing * 0x14 + 0x64;
            u16 cur = mStateTimer;
            if ((int)cur <= lim) {
                if (unk_407 != 1)
                    return;
                if (cur <= 0x1e)
                    return;
            }
            unk_407 = 0;
            mSubState = 0xb;
            mAnmModel.SetAnim(
                (BCA_File *)(((SharedFilePtr *)data_ov079_021275ec[mIsKing * 5 + 2])->filePtr),
                0x40000000, 0x1000, 0);
            mSubTimer = 0;
            return;
        }
    }


    if (st == 0xa) {
        mAnmModel.Advance();
        mSubTimer++;
        if (mSubTimer <= 0x1e)
            return;
        unk_407 = 0;
        mSubState++;
        mAnmModel.SetAnim(
            (BCA_File *)(((SharedFilePtr *)data_ov079_021275ec[mIsKing * 5 + 2])->filePtr),
            0x40000000, 0x1000, 0);
        mSubTimer = 0;
        return;
    }

    if (mAngleX > 0) {
        if (mSubTimer >= 0xc) {
            mPitchSpeed = -0x290;
            mAngleX += mPitchSpeed;
        }
        mSubTimer++;
    } else {
        mPitchSpeed = 0;
        mAngleX = 0;
        if (mAnmModel.Finished() != 0) {
            mAnmModel.SetAnim(
                (BCA_File *)(((SharedFilePtr *)data_ov079_021275ec[mIsKing * 5 + 4])->filePtr),
                0x40000000, 0x1000, 0);

            if (mIsKing == 0) {
                mMovingMeshCollider.SetStoodOnCallback(0);
            }
            mTouched = 0;
            mPounded = 0;
            if (mResumePending != 0) {
                mState = mResumeState;
                mResumePending = 0;
                if (mState == 7)
                    mResumeState = 0xb;
                return;
            }
            if (mIsKing != 0)
                mState = 2;
            else
                mState = 1;
            return;
        }
    }

    mAnmModel.Advance();
    {
        u8 v;
        unsigned int q;
        int m;
        v = mIsKing;
        q = mAnmModel.currFrame;
        m = v * 0x30;
        mTipLift = data_ov079_02127cfc[m + (int)((unsigned short)(q >> 12))];
    }
}

/* State 7, turn back: spin in place 0x400 a frame for 32 frames, then walk
   off at the class's pace and hand over to state 1 after 42. A hit that
   interrupted this state resumes it where it left off. */
void daBtn_c::func_ov079_021249f0()
{
    if ((unsigned int)(mResumeState - 10) <= 1) {
        mSubState = mSavedSubState;
        mStateTimer = mSavedStateTimer + 1;
        mResumeState = 7;
    }
    if (mSubState == 0) {
        mHorzSpeed = 0;
        mAnmModel.speed = 0x2000;
        if (mStateTimer >= 0x20) {
            mSubState++;
        } else {
            mPrevAngleY += 0x400;
            mAngleY = mPrevAngleY;
        }
    } else {
        mHorzSpeed = data_ov079_021275dc[mIsKing];
        if (mStateTimer > 0x2a) {
            mState = 1;
            mAnmModel.SetAnim(
                (BCA_File *)(((SharedFilePtr *)data_ov079_021275ec[mIsKing * 5 + 3])->filePtr), 0,
                0x1000, 0);
        }
    }
    mAnmModel.Advance();
}

/* State 8, defeat. The plain Whomp just bursts into dust and is gone. The
   king first talks: start a conversation with the player that beat it,
   show message 0xa5 beside the nearest player, wait for it to close, then
   burst, spawn its star (star mStarID, from 0x64000 above it) and go. */
void daBtn_c::func_ov079_021246dc()
{
    Vector3 pos;
    Vector3 starPos;
    Vector3 v;
    Vector3 v2;
    Vector3 dp;
    Vector3 dp2;

    if (mIsKing != 0) {
        switch (mSubState) {
        case 0:
            if (mPounder->StartTalk(*this, true) == 0)
                return;
            mSubState++;
            return;
        case 1:
            if (mPounder->GetTalkState() != 0)
                return;
            {
                int idx = mNearestPlayer;
                daPly_c *other = mPlayers[idx];
                Vector3 *op = &other->mPos;
                unsigned short ang;
                short mag = 0x10e;
                pos.x = op->x;
                pos.y = op->y;
                pos.z = op->z;
                ang = ::Vec3_HorzAngle(&pos, &mPos);
                pos.y += 0x32000;
                pos.x = FX_SinIdx(ang >> 4) * mag + pos.x;
                pos.z = FX_CosIdx(ang >> 4) * mag + pos.z;
                if (mPounder->ShowMessage(*this, 0xa5, (Vector3 *)&pos, 0, 0) == 0)
                    return;
                mSubState++;
                func_0201267c(0x133, &mCamSpacePos);
            }
            return;
        case 2:
            if (mPounder->GetTalkState() != -1)
                return;
            mSubState++;
            return;
        case 3:
            mState = 9;
            starPos.x = mPosX;
            starPos.y = mPosY;
            starPos.z = mPosZ;
            starPos.y += 0x64000;
            func_0201267c(0xc6, &mCamSpacePos);
            Sound::StopLoadedMusic_Layer3();
            func_02011cfc();
            func_ov079_02123d4c(&v, this);
            v.y += 0x50000;
            dp.x = v.x;
            dp.y = v.y;
            dp.z = v.z;
            TriplePoofDustAt(*(Vector3 *)&dp);
            Particle::System::NewSimple(kPtclWhompKingDefeat, v.x, v.y, v.z);
            mShouldRender = 0;
            UntrackAndSpawnStar(mTrackStarID, mStarID, *(Vector3 *)&starPos, 4);
            if (mMovingMeshCollider.IsEnabled() != 0)
                mMovingMeshCollider.Disable();
            MarkForDestruction();
            return;
        }
    } else {
        func_0201267c(0xc6, &mCamSpacePos);
        func_ov079_02123d4c(&v2, this);
        v2.y += 0x28000;
        dp2.x = v2.x;
        dp2.y = v2.y;
        dp2.z = v2.z;
        PoofDustAt(*(Vector3 *)&dp2);
        Particle::System::NewSimple(kPtclWhompDefeat, v2.x, v2.y, v2.z);
        MarkForDestruction();
    }
}

/* State 9: the king has been beaten and is waiting to be destroyed. */
void daBtn_c::func_ov079_021246d8()
{
}

/* State 10, the hit wobble: rock about the pitch axis for 10 of the 90
   frames the hit armed, then sit level, then put back the state the hit
   interrupted. */
void daBtn_c::func_ov079_02124638()
{
    if (mWobbleTimer-- != 0) {
        u32 t = mWobbleTimer;
        if (t <= 0x50) {
            mAngleX = 0;
            return;
        }
        {
            s32 amp = (s32)((t - 0x50) << 6);
            u16 ang = (u16)(s16)(t << 13);
            mAngleX = (s16)FX_MUL(amp, FX_SinIdx(ang >> 4));
        }
        return;
    }
    mState = mResumeState;
    if (mState == 7)
        mResumeState = 10;
    mResumePending = 0;
}

/* State 11, knocked over from behind: finish the hop, then tip forward like
   state 4 with variant-specific pitch acceleration until flat, landing in
   state 5. While the hop plays the body rises. */
void daBtn_c::func_ov079_02124530()
{
    mAnmModel.Advance();
    if (mAnmModel.Finished()) {
        if (mIsKing)
            mPitchSpeed += 0xe0;
        else
            mPitchSpeed += 0x140;
        mAngleX += mPitchSpeed;
        if (mAngleX > 0x4000) {
            mPitchSpeed = 0;
            mAngleX = 0x4000;
            mState = 5;
            func_0201267c(0xc6, &mCamSpacePos);
        }
        if (mIsKing) {
            mTipLift = (s16)mAngleX * (s16)0x15;
            if (mTipLift >= 0x53000)
                mTipLift = 0x53000;
        } else {
            mTipLift = (s16)mAngleX * (s16)0xa;
            if (mTipLift >= 0x25000)
                mTipLift = 0x25000;
        }
        return;
    }
    mPosY += (mIsKing + 1) << 15;
}

/* Keep the mesh collider enabled only near a player: always for a hidden
   body it is off, for the king outside state 0 it is on, otherwise it is on
   while the closest player is within `range` (default: eight clip radii
   from the clip centre). Returns 1 when the collider was left off. */
int daBtn_c::func_ov079_021243e0(int range)
{
    Vector3 v;
    daPly_c *player;
    int dist;
    int y;

    if (mShouldRender == 0) {
        if (mMovingMeshCollider.IsEnabled())
            mMovingMeshCollider.Disable();
        return 1;
    }

    if (mIsKing != 0 && mState != 0) {
        if (!mMovingMeshCollider.IsEnabled())
            mMovingMeshCollider.Enable(this);
        return 0;
    }

    v.x = mPosX;
    y = mPosY;
    v.y = y;
    v.z = mPosZ;
    if (range == 0) {
        int radius = mClipRadius;
        v.y = y + mClipOffsetY;
        range = radius << 3;
    }

    player = ClosestPlayer();
    dist = ::Vec3_Dist(&v, &player->mPos);
    if (dist > range) {
        if (mMovingMeshCollider.IsEnabled())
            mMovingMeshCollider.Disable();
        return 1;
    }

    if (!mMovingMeshCollider.IsEnabled())
        mMovingMeshCollider.Enable(this);
    return 0;
}

/* Pose the model from the actor (raised by mTipLift), read the first
   bone's rotation back into mBodyPitch/Yaw/Roll, and drop the shadow: its
   matrix sits under the body's centre, pushed out along the tilt, turned
   to the body's heading. */
void daBtn_c::func_ov079_02124188()
{
    Vector3 pos;
    Vector3 asr;
    int shadowRad;
    int tiltSin;
    int lean;
    char *bone;

    Matrix4x3_FromRotationXYZExt(&mAnmModel.mat4x3, mAngleX, mAngleY, mAngleZ);

    mAnmModel.mat4x3.t.x = mPosX >> 3;
    mAnmModel.mat4x3.t.y = (mPosY + mTipLift) >> 3;
    mAnmModel.mat4x3.t.z = mPosZ >> 3;

    bone = (char *)mAnmModel.data.bones;
    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;

    mBodyPitch = (short)-*(unsigned short *)(bone + 0x1c);
    mBodyYaw = *(unsigned short *)(bone + 0x1a);
    mBodyRoll = (short)(*(unsigned short *)(bone + 0x1e) - 0x4000);

    if (mIsKing != 0) {
        tiltSin = FX_SinIdx((unsigned short)mAngleX >> 4);
        lean = tiltSin * 0x190;
        shadowRad = tiltSin * 0x1a9 + 0xc8000;
    } else {
        tiltSin = FX_SinIdx((unsigned short)mAngleX >> 4);
        lean = tiltSin * 0xc8;
        shadowRad = tiltSin * 0xd2 + 0x64000;
    }

    pos.x += FX_MUL(lean, FX_SinIdx((unsigned short)mAngleY >> 4));
    pos.z += FX_MUL(lean, FX_CosIdx((unsigned short)mAngleY >> 4));

    ::Vec3_Asr((::Vector3 *)&asr, (::Vector3 *)&pos, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, asr.x, asr.y, asr.z);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68,
        (short)(mAngleY + mBodyYaw));

    *(Mtx43 *)&mShadowMatrix = *(Mtx43 *)&data_020a0e68;

    if (mShouldRender == 0)
        return;

    if (mIsKing != 0) {
        DropShadowScaleXYZ(mShadowModel, mShadowMatrix, 0x1cc000, 0x190000, shadowRad, 0xf);
    } else {
        DropShadowScaleXYZ(mShadowModel, mShadowMatrix, 0xf0000, 0x190000, shadowRad, 0xf);
    }
    return;
}

/* Pose the collision mesh: the model's matrix turned by the body's bone
   rotation (yaw always; pitch and half the roll except while walking,
   states 1 and 7), squashed vertically as the body tilts, and placed at the
   actor, raised by mTipLift and lowered again by the squash. */
void daBtn_c::func_ov079_02124008()
{
    s16 a = (s16)(mAngleX + mBodyPitch);
    int s;
    u8 n = mIsKing;
    int off;

    if (a < 0) a *= -1;
    s = (s16)(0x4000 - a);

    if (n) {
        if (mState == 0) s = s / 32 + 0x1000;
        else s = s / 48 + 0x1000;
    } else {
        s = s / 102;
        s = s + 0x1000;
    }

    off = (n + 1) * ((s - 0x1000) * 0x168);
    *(Mtx43 *)&mMeshMatrix = *(Mtx43 *)&mAnmModel.mat4x3;

    Matrix4x3_ApplyInPlaceToRotationY(&mMeshMatrix, mBodyYaw);
    if (mState != 1 && mState != 7) {
        Matrix4x3_ApplyInPlaceToRotationX(&mMeshMatrix, mBodyPitch);
        Matrix4x3_ApplyInPlaceToRotationZ(&mMeshMatrix, (s16)(mBodyRoll >> 1));
    }
    Matrix4x3_ApplyInPlaceToScale(&mMeshMatrix, 0x1000, s, 0x1000);

    mMeshMatrix.t.x = mPosX;
    mMeshMatrix.t.y = mPosY + mTipLift - off;
    mMeshMatrix.t.z = mPosZ;

    mMovingMeshCollider.Transform(mMeshMatrix, mBodyYaw);
}

/* Survey the watched players: the nearest one (index, distance, bearing into
   mTargetAngle) and the nearest one ahead, within 0x2000 of the heading. */
void daBtn_c::func_ov079_02123f34()
{
    s16 minAngle;
    int minDist, minDist2;
    int dist;
    int i;
    s16 angle;
    Vector3 v;

    mTargetAngle = mPrevAngleY;
    minAngle = mPrevAngleY;
    minDist2 = 0x7fffffff;
    minDist = 0x7fffffff;
    for (i = 0; i < 4; i++) {
        daPly_c *player = mPlayers[i];
        const Vector3 *p;
        if (player == 0)
            continue;
        p = &player->mPos;
        v.x = p->x;
        v.y = p->y;
        v.z = p->z;
        dist = ::Vec3_HorzDist(&mPos, &v);
        angle = ::Vec3_HorzAngle(&mPos, &v);
        if (dist < minDist) {
            mNearestPlayer = i;
            minDist = dist;
            minAngle = angle;
        }
        if (GetSubtraction(angle, mPrevAngleY) < 0x2000) {
            if (dist < minDist2) {
                minDist2 = dist;
                mFrontPlayer = i;
            }
        }
    }
    mNearestDist = minDist;
    mFrontDist = minDist2;
    mTargetAngle = minAngle;
}

void daBtn_c::OnHitByMegaChar(daPly_c &player) {
    if (mIsKing != 0) return;
    if (mDeathState == 8) return;
    player.IncMegaKillCount();
    func_02012694(0x1e,(const Vector3 *)(&mCamSpacePosX));
    Vector3 pos;
    func_ov079_02123d4c(&pos, this);
    Vector3 dust;
    pos.y = pos.y + 0x28000;
    dust.x = pos.x;
    dust.y = pos.y;
    dust.z = pos.z;
    PoofDustAt(dust);
    Particle::System::NewSimple(kPtclWhompDefeat, pos.x, pos.y, pos.z);
    MarkForDestruction();
    if (mMovingMeshCollider.IsEnabled() != 0)
        mMovingMeshCollider.Disable();
}

/* The point eggs aim at and dust bursts from: the body's centre, pushed out
   along the tilt (twice as far for the king) and raised by
   OnAimedAtWithEgg's height. OnAimedAtWithEggReturnVec returns exactly this.
   The real reading is a Vector3-by-value member, but spelling it that way
   gives the named local its own stack home (no NRVO, +0xc); the (out, this)
   free form is the byte-exact shape, and the callers that need the member
   view reach it through vecret below. */
extern "C" void func_ov079_02123d4c(Vector3 *out, daBtn_c *self) {
    out->x = self->mPosX;
    out->y = self->mPosY;
    out->z = self->mPosZ;
    {
        int m;
        int d;
        if (self->mIsKing) {
            m = (s16)FX_SinIdx((unsigned short)self->mAngleX >> 4) * (s16)0x190;
        } else {
            m = (s16)FX_SinIdx((unsigned short)self->mAngleX >> 4) * (s16)0xc8;
        }
        out->x = out->x + FX_MUL(m, FX_SinIdx((unsigned short)self->mAngleY >> 4));
        d = self->OnAimedAtWithEgg();
        {
            out->y += d;
            out->z = out->z + FX_MUL(m, FX_CosIdx((unsigned short)self->mAngleY >> 4));
        }
    }
}

/* The king shoves the nearest player aside: if that player is no higher
   than the king and stands inside the band ahead of it (within 0x3c000
   sideways and 0xe1000 along the line to them, measured on the bearing
   mTargetAngle), move them 5 units along the body's heading. Returns 1 when
   it did. */
int daBtn_c::func_ov079_02123bcc() {
    int v[6];
    int dist, rangle, idx, hc, hs, idx2, nx, nz;
    daPly_c *player;
    s32 *p;
    s16 cosv, sinv, sin2, cos2;
    int a0, a1, a2, raw, ang;
    player = mPlayers[mNearestPlayer];
    p = &player->mPosX;
    v[0] = p[0]; v[1] = p[1]; v[2] = p[2];
    if (v[1] > mPosY) return 0;
    dist = ::Vec3_HorzDist((const ::Vector3 *)v, &mPos);
    a0 = mAngleY; a1 = mBodyYaw; a2 = mTargetAngle;
    raw = (a0 + a1) << 16; ang = a2 + (raw >> 16); rangle = raw >> 16;
    cosv = FX_CosIdx((u16)(s16)ang >> 4);
    hc = FX_MUL(dist, cosv);
    if (hc < 0) hc = -hc;
    if (hc <= 0x3c000) {
        sinv = FX_SinIdx((u16)(s16)ang >> 4);
        hs = FX_MUL(dist, sinv);
        if (hs < 0) hs = -hs;
        if (hs <= 0xe1000) {
            int vx, vy, vz;
            int i;
            sin2 = FX_SinIdx((u16)rangle >> 4);
            cos2 = FX_CosIdx((u16)rangle >> 4);
            vx = v[0]; vy = v[1]; vz = v[2];
            nx = (s16)sin2 * 5 + vx;
            nz = (s16)cos2 * 5 + vz;
            i = mNearestPlayer;
            player = mPlayers[i];
            player->mPosX = nx;
            player->mPosY = vy;
            player->mPosZ = nz;
            return 1;
        }
    }
    return 0;
}

s32 daBtn_c::OnAimedAtWithEgg() {
    int idx;
    if(mIsKing!=0){
        idx = ((int)(unsigned short)mAngleX >> 4);
        return FX_CosIdx(idx) * 0x11d + 0x4b000;
    }
    idx = ((int)(unsigned short)mAngleX >> 4);
    return FX_CosIdx(idx) * 0x90 + 0x24000;
}

Vector3 daBtn_c::OnAimedAtWithEggReturnVec()
{
    return vecret::func_ov079_02123d4c(this);
}

/* The mesh collider also stays on while the nearest actor of ID 9 is within
   eight clip radii of the egg-aim point. Returns 1 when it did. */
int daBtn_c::func_ov079_02123a8c()
{
    Vector3 v;

    if (mShouldRender == 0)
        return 0;

    dActor_c *closest = ClosestWithActorID(9);
    if (closest) {
        v.x = mPosX;
        v.y = mPosY;
        v.z = mPosZ;
        v.y = v.y + OnAimedAtWithEgg();
        int dist = ::Vec3_Dist(&v, &closest->mPos);
        if (dist < (mClipRadius << 3)) {
            if (!mMovingMeshCollider.IsEnabled())
                mMovingMeshCollider.Enable(this);
            return 1;
        }
    }
    return 0;
}

/* A hit from `other`, judged by where it stood a frame ago (pulled back
   0x50 along its heading) against the body's facing. From the front (within
   0x3c00) the body wobbles (state 10); from behind (beyond 0x4400) it drops
   a coin at the attacker and is knocked over (state 11). Only a walking or
   turning body reacts (states 1, 2, 7, 10), or a waiting one (0) -- for the
   king only before its introduction starts. The interrupted state is saved
   for resuming. Nothing in the cartridge's relocations calls this. */
void daBtn_c::func_ov079_02123804(dActor_c *other)
{
    Vector3 v;
    s16 sinv, cosv;
    int ang;

    /* Copied through a pointer: `v = other->mPrevPos` loads the three words
       in a different order and moves the literal pool. */
    {
        int* q = &other->mPrevPosX;
        v.x = q[0];
        v.y = q[1];
        v.z = q[2];
    }

    sinv = FX_SinIdx((u16)other->mPrevAngleY >> 4);
    cosv = FX_CosIdx((u16)other->mPrevAngleY >> 4);
    v.x = v.x - sinv * 0x50;
    v.z = v.z - cosv * 0x50;

    ang = ::Vec3_HorzAngle(&mPos, &v);

    if (AngleDiff(ang, (s16)(mAngleY + mBodyYaw)) < 0x3c00) {
        int r1 = mState;
        if (!(r1 == 1 || r1 == 2 || r1 == 7 || r1 == 0xa)) {
            if (r1 != 0) return;
            if (mIsKing != 0) {
                if (mSubState != 0) return;
            }
        }
        if (mState != 0xa) {
            /* Both reads are measured. A plain mState read here or at the
               second site below shrinks the function 0x288 -> 0x284; plain
               reads at both sites give 0x280. */
            mResumeState = *(volatile s32 *)&mState;
            mSavedStateTimer = mStateTimer;
            mSavedSubState = mSubState;
        }
        mWobbleTimer = 0x5a;
        mHorzSpeed = 0;
        mState = 0xa;
        mResumePending = 1;
        return;
    }

    if (AngleDiff(ang, (s16)(mAngleY + mBodyYaw)) <= 0x4400) return;

    {
        int r1 = mState;
        if (!(r1 == 1 || r1 == 2 || r1 == 7 || r1 == 0xa)) {
            if (r1 != 0) return;
            if (mIsKing != 0) {
                if (mSubState != 0) return;
            }
        }
        if (mState != 0xa) {
            mResumeState = *(volatile s32 *)&mState;
            mSavedStateTimer = mStateTimer;
            mSavedSubState = mSubState;
        }
        {
            Vector3 pos;
            int* q = &other->mPosX;
            pos.x = q[0];
            pos.y = q[1];
            pos.z = q[2];
            SpawnCoins(pos, 1, 0, 0);
        }
        mHorzSpeed = 0;
        mAnmModel.SetAnim(
            (BCA_File *)(((SharedFilePtr *)data_ov079_021275ec[mIsKing * 5])->filePtr), 0x40000000,
            0x1000, 0);
        mAnmModel.speed = 0x4000;
        mState = 0xb;
        mResumePending = 1;
        return;
    }
}

/* NOT WRITTEN HERE ON PURPOSE. The inline destructor in the header
   emits D1 then D0 -- the cartridge's order -- and no D2. */
