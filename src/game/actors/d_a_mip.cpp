//cpp
/* daMip_c -- MIPS the rabbit (MIP). ov085.
 *
 * defer_codegen off lays .text down in source order. The destructor
 * is the key function: the cartridge has D1 below D0 and no D2.
 *
 * leftovers:
 * - g_profile_MIP stays outside this TU. The file handles, the eight
 *   state records, the carry offsets and the rabbit-count int are
 *   the folded sinit objects at the end of this file.
 * - TestWaterBelow's ground probe stays a stack buffer; clsnY is
 *   detect[12].
 * - Render writes mMaterialColor into each material's texImageParam.
 *   numMaterials is still read through the first word of ModelComponents.
 * - The mirror shadow matrix is mMirrorMtx.
 * - The read-modify-writes of the player's mStateFlags, mTargetAngY and
 *   mEatenTimer are compound assignments (`|=`, `+=`, `++`). An earlier draft
 *   spelled them through pointer locals and a (long long)(int)& launder
 *   because the expanded `x = x | y` form changes the size; the compound form
 *   reproduces (#821).
 */
/* Runtime asset handles resolved through the overlay 0 file table. */
enum daMip_c_AssetId {
    ASSET_MIP_BMD = 0x03a4,
    ASSET_MIP_HOLD_BCA = 0x03a5,
    ASSET_MIP_RUN_BCA = 0x03a6,
    ASSET_MIP_RUN_ED_BCA = 0x03a7,
    ASSET_MIP_RUN_ST_BCA = 0x03a8,
    ASSET_MIP_WAIT_BCA = 0x03a9,
    ASSET_MIP_KEY_BMD = 0x0452
};

#include "daMip_c.h"
#include "common.h"
#include "SharedFilePtr.h"
#include "daPly_c.h"
#include "PathPtr.h"
#include "decl_Message.h"
#include "Message.h"
#include "SaveData.h"
#include "dBgCh_Gnd.h"
#include "daObj_Mip_Key_c.h"
#include "Sound.h"
#include "math/CalcLib.h"
#include "Particle__System.h"
#include "ParticleIds.h"
#include "LevelRng.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_cp.h"

/* ground probe / collision */
int SurfaceInfo_TestFlag0x20(int *);

/* daPly_c / Message / Sound / SaveData */
/* The return type is load-bearing: StateTalkMain tests it in an `if`, so the
   `void` spelling three of the four legacy files used cannot be the one. */

/* model / animation / shadow */


/* maths and the still-unnamed helpers */
extern "C" void Vec3_Sub(Vector3 *, const Vector3 *, const Vector3 *);
extern "C" int AngleDiff(int, int);
extern "C" void Matrix4x3_FromTranslation(void *, int, int, int);
extern "C" void Matrix4x3_FromRotationY(void *, int);
extern "C" void Matrix4x3_ApplyInPlaceToRotationXYZExt(void *, int, int, int);
extern "C" void MTX_Concat43(void *, void *, void *);
extern "C" void MTX_Concat33(void *, void *, void *);
extern "C" void MTX_MultVec43(Vector3 *, Matrix4x3 *, Vector3 *);
u8 NumStars(void);
extern "C" int func_02013890(int, int);
void func_02013944(void);
void func_02012694(unsigned int id, const Vector3 *pos);
extern "C" void func_0203c178(void *, int, int, int);

/* The state records at 0x0213003c..0x021300bc are 8-byte Itanium
   pointer-to-member-function constants on daMip_c, so SetState and Behavior
   call them as real member pointers. The current record's address sits at
   0x364 -- mState, which the header keeps as an opaque s32 because Behavior
   compares it by ADDRESS against four ov085 objects. */
typedef int (daMip_c::*daMip_cStateFn)();

/* File-scope objects at the end of this file construct the seven resource
 * handles (model files 0x452/0x3a4, animation files 0x3a5/0x3a6/0x3a7/
 * 0x3a8/0x3a9), build the four carry-offset vectors, fill the eight state
 * pairs and read the glowing-rabbit count. mwcc emits __sinit_d_a_mip.cpp
 * from those definitions. The wrapper names are local; the handle
 * constructors and destructors are the ROM resource-family functions,
 * aliased in the manifest. The carry offsets are one Vector3[4]: mwcc
 * builds and registers each element in turn, with the destructor alias
 * pointing at the ROM's canonical Vector3 D1. (r63 measured four separate
 * Vector3 globals, which hoist each x `mov` ahead of the address load;
 * the array does not, #821 r77.) */
struct DaMipModelFile : SharedFilePtr {
    DaMipModelFile(u32 fileID);
    ~DaMipModelFile();
};

struct DaMipAnimationFilePtr : SharedFilePtr {
    DaMipAnimationFilePtr(u32 fileID);
    ~DaMipAnimationFilePtr();
};

struct MipStatePair {
    daMip_cStateFn a, b;
};

extern DaMipModelFile data_ov085_021305d8;
extern DaMipModelFile data_ov085_021305e0;
extern DaMipAnimationFilePtr data_ov085_021305b8;
extern DaMipAnimationFilePtr gMipRunAnimationFile;
extern DaMipAnimationFilePtr data_ov085_021305b0;
extern DaMipAnimationFilePtr data_ov085_021305c8;
extern DaMipAnimationFilePtr data_ov085_021305c0;
extern Vector3 data_ov085_021306ec[4];
extern MipStatePair data_ov085_021306cc;
extern MipStatePair data_ov085_0213066c;
extern MipStatePair data_ov085_0213067c;
extern MipStatePair data_ov085_0213069c;
extern MipStatePair data_ov085_021306ac;
extern MipStatePair data_ov085_021306bc;
extern MipStatePair data_ov085_021306dc;
extern MipStatePair data_ov085_0213068c;
extern daMip_cStateFn data_ov085_02130074;
extern daMip_cStateFn data_ov085_02130044;
extern daMip_cStateFn data_ov085_02130084;
extern daMip_cStateFn data_ov085_02130094;
extern daMip_cStateFn data_ov085_021300b4;
extern daMip_cStateFn data_ov085_0213003c;
extern daMip_cStateFn data_ov085_0213005c;
extern daMip_cStateFn data_ov085_02130064;
extern daMip_cStateFn data_ov085_0213007c;
extern daMip_cStateFn data_ov085_0213008c;
extern daMip_cStateFn data_ov085_02130054;
extern daMip_cStateFn data_ov085_0213009c;
extern daMip_cStateFn data_ov085_0213006c;
extern daMip_cStateFn data_ov085_0213004c;
extern daMip_cStateFn data_ov085_021300ac;
extern daMip_cStateFn data_ov085_021300a4;
extern int data_ov085_021305ac;

/* Actor ID 0xbf is PLAYER. In dActor_c::mFlags, bit 0x100 is set while a
 * player holds the actor, 0x4000 while it is carried and 0x40000 while it
 * sits in Yoshi's mouth. These int-returning predicates are what make the
 * ROM materialize 0/1 before testing it. */
static inline int IsPlayerActor(dActor_c *actor) {
    return actor->actorID == 0xbf;
}

static inline int IsInYoshiMouth(dActor_c *actor) {
    return (actor->mFlags & 0x40000) != 0;
}

static inline int IsFlagSet(dActor_c *actor, u32 flag) {
    return (actor->mFlags & flag) != 0;
}

static inline int IsBitSet(u32 flags, u32 bit) {
    return (flags & bit) != 0;
}

#pragma defer_codegen off

/* Empty body. The compiler emits the member destructors and, for D0,
   dEnemyBase_c's operator delete. The cartridge has no D2; the one this
   TU emits is compiler-only and deadstripped. */
daMip_c::~daMip_c()
{
}

/* Probe 0xc8000 above the rabbit. StateFleeMain and StateIdleMain use the
   water flag to pick splash over dust. */
int daMip_c::TestWaterBelow()
{
    dBgCh_Gnd rg;
    rg.StartDetectingWater();
    Vector3 v(mPos.x, mPos.y + 0xc8000, mPos.z);
    rg.SetObjAndPos(v, this);
    if (rg.DetectClsn()) {
        mFloorY = rg.clsnY;
        if (SurfaceInfo_TestFlag0x20((int *)&rg.surface))
            return 1;
    }
    return 0;
}

/* Pick-up. otherOwner is the actor touching the cylinder; actor 0xbf is
   daPly_c. hitFlags 0x1000 is the grab bit. A rabbit already caught once
   (mCatchState) goes to Released instead of Caught. */
void daMip_c::UpdateGrab()
{
    extern MipStatePair data_ov085_021306ac;
    extern MipStatePair data_ov085_021306bc;

    unsigned int id = mdCcAc_c.otherOwner;
    if (id == 0) return;
    daPly_c *o = static_cast<daPly_c *>(dActor_c::FindWithID(id));
    if (o == 0) return;
    if (!IsPlayerActor(o))
        return;
    if ((mdCcAc_c.hitFlags & 0x1000) == 0) return;
    if (o->TryGrab(*this) == 0)
        return;
    mTalkingPlayer = o;
    mdCcAc_c.flags |= 2;
    if (mCatchState == 0) {
        SetState(&data_ov085_021306ac);
    } else {
        SetState(&data_ov085_021306bc);
    }
}

/* Eighth-rabbit epilogue, from StateCaughtMain once all eight glowing
   rabbits are found. Turns toward mSaveTalkPlayer, offers 0x148, then
   Message::DisplaySaving, then Released. */
int daMip_c::StateSaveTalkMain()
{
    extern unsigned char data_0209d684;
    extern unsigned char data_0209d660;
    extern MipStatePair data_ov085_021306bc;

    daPly_c *player = mSaveTalkPlayer;
    Vector3 vec;
    unsigned char gb;
    int state;

    mTargetAngY = Vec3_HorzAngle(&mPos, &player->mPos);
    ApproachAngle(&mPrevAngle.y, mTargetAngY, 1, 0x500, 0x500);

    gb = data_0209d684;
    vec = mPos;
    vec.y += 0x3c000;

    state = mActionStep;
    switch (state) {
    case 0:
        if (player->ShowMessage(*this, 0x148, &vec, 0, 0)) {
            func_02012790(0xa);
            mActionStep++;
        }
        break;
    case 1:
        if (data_0209d660 == 0) {
            if (gb == 1) {
                func_02012790(0x5e);
                Message::DisplaySaving(0x295);
                mActionStep++;
            } else if (gb == 2) {
                func_02012790(0x98);
                player->mStateFlags &= ~0x800;
                Message::EndTalk();
                SetState(&data_ov085_021306bc);
            }
        }
        break;
    case 2:
        if (data_0209d660 == 0) {
            player->mStateFlags &= ~0x800;
            Message::EndTalk();
            SetState(&data_ov085_021306bc);
        }
        break;
    }
    return 1;
}

int daMip_c::StateSaveTalkInit()
{
    extern DaMipAnimationFilePtr data_ov085_021305c0;

    mActionStep = 0;
    func_02013944();
    mAnmModel.SetAnim((BCA_File *)data_ov085_021305c0.filePtr, 0, 0x1000, 0);
    return 1;
}

/* Ordinary conversation, from StateReleasedMain once StartTalk agrees.
   Faces mSaveTalkPlayer, picks the line from mRabbitId, then Released. */
int daMip_c::StateTalkMain()
{
    extern MipStatePair data_ov085_021306bc;

    daPly_c *player;
    Vector3 pos;
    Vector3 pp;
    s16 angle;
    unsigned int id;

    player = mSaveTalkPlayer;
    const Vector3 &playerPos = player->mPos;
    pos = mPos;
    pp = playerPos;
    angle = Vec3_HorzAngle(&mPos, &pp);

    id = 0x139;
    switch (player->GetTalkState()) {
    case 0:
        pos.y = pos.y + 0x46000;
        if (mRabbitId == 7) {
            id = 0x13d;
        }
        if (mRabbitId == 6) {
            id = 0x13a;
        }
        if (ApproachLinear(mPrevAngle.y, angle, 0x800)) {
            if (Sound::PlaySub(0x26, 0x12, 0x7f, 0x15ccc, 0)) {
                Message::PrepareTalk();
                player->ShowMessage(*this, id, &pos, 0, 0);
            }
        }
        break;
    case 1:
        break;
    default:
        if (Sound::PlaySub(0x26, 0x7f, 0, 0x7444, 0)) {
            Message::EndTalk();
            SetState(&data_ov085_021306bc);
        }
        break;
    }
    return 1;
}

int daMip_c::StateTalkInit()
{
    mActionStep = 0;
    return 1;
}

/* Standing free. Drops mTalkingPlayer once the carry flags clear, skips
   the frame while the closest player is mid-message (mStateFlags 0x800),
   and re-opens the conversation through StartTalk. */
int daMip_c::StateReleasedMain()
{
    extern MipStatePair data_ov085_021306dc;

    unsigned short h;
    daPly_c *o;
    daPly_c *cp;
    int a;
    int flags;
    daPly_c *obj;

    obj = mTalkingPlayer;
    if (obj != 0) {
        flags = mFlags;
        /* A plain int, not IsBitSet: the inline swaps obj's and flags'
           registers here. */
        a = (flags & 0x400) != 0;
        if (a || IsBitSet(flags, 0x2000)) {
            /* obj is known non-null here; the ROM still re-tests it. */
            if (a && obj != 0)
                mPrevAngle.y = obj->mAngle.y;
            mdCcAc_c.flags = mdCcAc_c.flags & ~2;
            mTalkingPlayer = 0;
        }
        if (!IsFlagSet(this, 0x100))
            mTalkingPlayer = 0;
        obj = mTalkingPlayer;
        if (obj != 0) {
            if (obj->mIsUnderwater != 0) {
                mTalkingPlayer = 0;
            }
        }
    }

    cp = ClosestPlayer();
    if (cp != 0) {
        h = cp->mStateFlags;
        h = (unsigned short)(h & 0x800);
        if (h != 0)
            return 1;
    }

    if ((mdCcAc_c.hitFlags & 0x8000000) != 0) {
        o = static_cast<daPly_c *>(dActor_c::FindWithID(mdCcAc_c.otherOwner));
        if (o != 0) {
            if (IsPlayerActor(o)) {
                mSaveTalkPlayer = o;
                o = mSaveTalkPlayer;
                if (o->StartTalk(*this, 0) != 0) {
                    SetState(&data_ov085_021306dc);
                }
            }
        }
    }

    return 1;
}

int daMip_c::StateReleasedInit()
{
    extern DaMipAnimationFilePtr data_ov085_021305c0;

    mVertAccel = -0x1000;
    mCatchState = 1;
    mdCcAc_c.vulnFlags &= ~0x1000;
    mdCcAc_c.flags |= 0x4000000;
    mdCcAc_c.radius = 0x78000;
    mdCcAc_c.vulnFlags &= ~0x8000;
    mAnmModel.SetAnim((BCA_File *)data_ov085_021305c0.filePtr, 0, 0x1000, 0);
    return 1;
}

/* Held: the caught conversation, the star spawn (actor 0xe5), and the
   branch that runs once all eight glowing rabbits are found. */
int daMip_c::StateCaughtMain()
{
    extern MipStatePair data_ov085_021306cc;
    extern MipStatePair data_ov085_021306bc;
    extern MipStatePair data_ov085_0213068c;
    extern u8 data_0209d660;
    extern u8 data_0209d6bc;

    daPly_c *pl;
    Vector3 pv;
    Vector3 pos;
    Vector3 pos7;
    Vector3 posR;
    int soundId;
    int msg;

    pl = mTalkingPlayer;
    if (pl == 0) {
        SetState(&data_ov085_021306cc);
        return 1;
    }

    if (mActionStep == 0) {
        {
            const Vector3 &playerPos = pl->mPos;
            pv = playerPos;
        }

        if (mCatchState != 0) {
            s16 ang = Vec3_HorzAngle(&mPos, &pv);
            ApproachLinear(mPrevAngle.y, ang, 0x800);
            if (AngleDiff(mPrevAngle.y, ang) > 0x200)
                return 1;
        }

        if ((IsFlagSet(this, 0x4000) || mCatchState == 2) && pl->StartTalk(*this, 1) != 0) {
            pos = mPos;
            soundId = 0;

            if (mRabbitId != 7) {
                if (func_02013890(mRabbitId, (s32)pl->param1) == 0) {
                    if ((s32)pl->param1 != 3) {
                        Message::PrepareTalk();
                        Sound::PlaySub(0x27, 0x12, 0x7f, 0x15ccc, false);
                        if (mRabbitId != 6) {
                            msg = (s16)((s32)pl->param1 + 0x11b);
                            soundId = 0x163;
                        } else {
                            msg = (s16)((s32)pl->param1 + 0x123);
                            soundId = 0x161;
                        }
                    } else {
                        if (mRabbitId != 6)
                            msg = 0x12b;
                        else
                            msg = 0x12c;
                    }
                } else {
                    if ((s32)pl->param1 != 3) {
                        Message::PrepareTalk();
                        if (mIsGlowing == 0) {
                            Sound::PlaySub(0x26, 0x12, 0x7f, 0x15ccc, false);
                            if (mRabbitId != 6) {
                                msg = (s16)((s32)pl->param1 + 0x11f);
                                soundId = 0x163;
                            } else {
                                msg = (s16)((s32)pl->param1 + 0x127);
                                soundId = 0x161;
                            }
                        } else {
                            Sound::PlaySub(0x27, 0x12, 0x7f, 0x15ccc, false);
                            if (SaveData::NumGlowingRabbitsFound() == 7) {
                                msg = 0x143;
                                soundId = 0x160;
                            } else {
                                soundId = 0x162;
                                msg = 0x142;
                            }
                        }
                    } else {
                        if (mIsGlowing == 0) {
                            if (mRabbitId != 6)
                                msg = 0x12d;
                            else
                                msg = 0x12e;
                        } else {
                            if (SaveData::NumGlowingRabbitsFound() == 7)
                                msg = 0x147;
                            else
                                msg = 0x146;
                        }
                    }
                }
            } else {
                msg = 0x13c;
            }

            pos.y += 0x64000;
            if (pl->ShowMessage(*this, (u32)msg, &pos, 0, 0) == 1) {
                mActionStep = 1;
                if (soundId != 0)
                    func_02012694(soundId, &mCamSpacePos);
            }
            return 1;
        }
    }

    if (mActionStep != 1)
        return 1;

    if (pl->GetTalkState() != -1) {
        if (data_0209d660 != 0 && data_0209d6bc == 9 &&
            (mRabbitId == 7 || func_02013890(mRabbitId, (s32)pl->param1) == 0 || mIsGlowing != 0)) {
            Sound::PlaySub(0x27, 0x12, 0, 0x8777, false);
        }
        return 1;
    }

    pl->DropActor();
    mdCcAc_c.flags = mdCcAc_c.flags & ~2;
    mHorzSpeed = 0;

    if (mRabbitId == 7) {
        pos7 = mPos;
        pos7.y += 0x32000;
        {
            dActor_c *spawned = dActor_c::Spawn(0xe5, mRabbitId, pos7, &mAngle, mAreaId, -1);
            if (spawned != 0)
                ((daObj_Mip_Key_c *)spawned)->unk_190 = uniqueID;
        }
        func_02012790(0xa);
        mTalkState = 0;
        mTalkingPlayer = 0;
        SetState(&data_ov085_021306bc);
        return 1;
    }

    if (func_02013890(mRabbitId, (s32)pl->param1) == 0 ||
        (mIsGlowing != 0 && SaveData::NumGlowingRabbitsFound() == 7)) {
        posR = mPos;
        {
            u32 param = mRabbitId;
            if (mIsGlowing != 0) {
                param = 0x4d;
                func_02013944();
            }
            posR.y += 0x32000;
            {
                dActor_c *spawned = dActor_c::Spawn(0xe5, param, posR, 0, mAreaId, -1);
                if (spawned != 0)
                    ((daObj_Mip_Key_c *)spawned)->unk_190 = uniqueID;
            }
            func_02012790(0xa);
            mTalkState = 0;
            pl->mStateFlags |= 0x800;
        }
    } else {
        if (mTalkState == 2) {
            pl->mStateFlags &= ~0x800;
            mTalkState = 0;
        }
        if (mIsGlowing == 0) {
            Sound::PlaySub(0x26, 0x7f, 0, 0x7444, 0);
            Message::EndTalk();
        }
    }

    mTalkingPlayer = 0;

    if (mIsGlowing == 0 || SaveData::NumGlowingRabbitsFound() == 8) {
        SetState(&data_ov085_021306bc);
    } else {
        pl->mStateFlags |= 0x800;
        mSaveTalkPlayer = pl;
        SetState(&data_ov085_0213068c);
    }
    return 1;
}

int daMip_c::StateCaughtInit()
{
    extern DaMipAnimationFilePtr data_ov085_021305b8;

    mActionStep = 0;
    mHorzSpeed = 0;
    mdCcAc_c.radius = 0x28000;
    mAnmModel.SetAnim((BCA_File *)data_ov085_021305b8.filePtr, 0, 0x1000, 0);
    return 1;
}

int daMip_c::StateRestMain()
{
    extern MipStatePair data_ov085_021306cc;

    if (mAnmModel.Finished() != 0) {
        SetState(&data_ov085_021306cc);
    }
    return 1;
}

int daMip_c::StateRestInit()
{
    extern DaMipAnimationFilePtr data_ov085_021305b0;

    mAnmModel.SetAnim((BCA_File *)data_ov085_021305b0.filePtr, 0x40000000, 0x1000, 0);
    return 1;
}

/* Runs the path away from the player. Dust or splash at a node, steps
   mPathDir, and falls back to Rest past 0x4b0000. */
int daMip_c::StateFleeMain() {
    extern MipStatePair data_ov085_0213069c;

    daPly_c *pl;
    s32 len;
    s32 lim;
    s16 ang;
    int idx;
    int ysave;
    pl = ClosestPlayer();
    if (pl != 0) {
        /* A POD copy-init is the ROM's ldm/stm; any Vector3 copy is memberwise. */
        /* Measured (#821 r68): copied as a Vector3 this is memberwise, not the ROM's ldm/stm. */
        /* Kept (#821 r77): `Vector3 v = pl->mPos;` grows StateFleeMain
           0x2a8 -> 0x2b0, so the POD snapshot and its cast into Vec3_Dist stay. */
        VecFx32 v = *(VecFx32 *)&pl->mPos;
        if (Vec3_Dist(&mPos, (const Vector3 *)&v) > 0x4b0000) {
            SetState(&data_ov085_0213069c);
            return 1;
        }
    }
    {
        s32 t = mAnmModel.currFrame >> 12;
        if (((u16)t) == 0) {
            if (TestWaterBelow() == 1) {
                func_02012694(0x124, &mCamSpacePos);
                Particle::System::NewSplash(mPos.x, mFloorY + 0x3000, mPos.z);
            } else {
                func_02012694(0x123, &mCamSpacePos);
            }
        }
    }

    if (TestWaterBelow() == 1) {
        Vector3 pos;
        s32 z = mPos.z;
        s32 y = mFloorY + 0x3000;
        s32 x = mPos.x;
        pos.x = x;
        pos.y = y;
        pos.z = z;
        mDustParticle =
            Particle::System::NewFitWater(mDustParticle, kPtclWaterSurfaceMove, pos.x, pos.y, z, 0);
    }

    PathPtr pathptr;
    Vector3 node;
    Vector3 node2;
    Vector3 delta;
    Vector3 scaled;
    pathptr.FromID(mPathId);
    pathptr.GetNode(node, mPathNodeIndex);
    ang = Vec3_HorzAngle(&mPos, &node);
    ApproachAngle(&mPrevAngle.y, ang, 1, 0x1000, 0x1000);
    idx = mPathNodeIndex + mPathDir;
    if (idx < 0) {
        idx = mNumPathNodes - 1;
    }
    if (idx >= mNumPathNodes) {
        idx = 0;
    }
    pathptr.GetNode(node2, (u32)idx);
    lim = 0x26000;
    if (mRabbitId == 7) {
        lim = lim >> 1;
    }
    {
        s32 y = mPos.y;
        node.y = y;
        Vec3_Sub(&delta, &mPos, &node);
        len = VEC_Mag(&delta);
    }
    if ((len == 0) || (len <= lim)) {
        mPos = node;
        mPathNodeIndex += mPathDir;
        if (mPathNodeIndex >= mNumPathNodes) {
            mPathNodeIndex = 0;
        }
        if (mPathNodeIndex < 0) {
            mPathNodeIndex = mNumPathNodes - 1;
        }
    } else {
        ang = Vec3_HorzAngle(&mPos, &node);
        if (AngleDiff(ang, mPrevAngle.y) < 0x2000) {
            ysave = mPos.y;
            {
                int s = FX_Div(lim, len);
                Vec3_MulScalar(&scaled, &delta, s);
                VEC_Subtract(&mPos, &scaled, &mPos);
            }
            if (mSpeed.y > 0) {
                mPos.y = ysave;
            }
            if (mActionStep == 0) {
                if (mWithMeshClsn.IsOnWall() != 0) {
                    mSpeed.y = 0xa000;
                    mActionStep = 1;
                }
            }
        }
    }
    return 1;
}

/* Whichever neighbouring node is further from the player becomes mPathDir. */
int daMip_c::StateFleeInit()
{
    extern DaMipAnimationFilePtr gMipRunAnimationFile;

    Vector3 v;
    Vector3 nodes[2];
    int i;
    int d0;

    daPly_c *p = ClosestPlayer();
    mActionStep = 0;
    if (p) {
        const Vector3 &playerPos = p->mPos;
        v = playerPos;
        PathPtr pathptr;
        int indices[2];
        pathptr.FromID(mPathId);

        indices[0] = mPathNodeIndex - 1;
        if (mPathNodeIndex - 1 < 0)
            indices[0] = mNumPathNodes - 1;
        indices[1] = mPathNodeIndex + 1;
        if (mPathNodeIndex + 1 >= mNumPathNodes)
            indices[1] = 0;

        for (i = 0; i < 2; i++)
            pathptr.GetNode(nodes[i], indices[i]);

        mPathDir = 1;
        d0 = Vec3_Dist(&v, &nodes[0]);
        if (d0 > Vec3_Dist(&v, &nodes[1]))
            mPathDir = -1;
    }

    mAnmModel.SetAnim((BCA_File *)gMipRunAnimationFile.filePtr, 0, 0x1000, 0);
    return 1;
}

int daMip_c::StateStartleMain()
{
    extern MipStatePair data_ov085_0213067c;

    if (mAnmModel.Finished() != 0)
        SetState(&data_ov085_0213067c);
    return 1;
}

int daMip_c::StateStartleInit()
{
    extern DaMipAnimationFilePtr data_ov085_021305c8;

    mAnmModel.SetAnim((BCA_File *)data_ov085_021305c8.filePtr, 0x40000000, 0x1000, 0);
    return 1;
}

/* Startle when the closest player comes inside 0x3e8000 from below.
   Otherwise cycle the idle and drift facing around the spot StateIdleInit
   stored. */
int daMip_c::StateIdleMain()
{
    /* The splash position is a Vector3 built by its (x, y, z) constructor:
       that is what loads mFloorY, mPos.z, mPos.x in the ROM's order and keeps
       the three stores. Vector3's empty destructor emits no cleanup; its D1
       is already this TU's deadstrip-duplicate (Behavior's locals). */
    extern MipStatePair data_ov085_0213066c;
    extern DaMipAnimationFilePtr gMipRunAnimationFile;
    extern DaMipAnimationFilePtr data_ov085_021305b0;
    extern DaMipAnimationFilePtr data_ov085_021305c0;
    extern DaMipAnimationFilePtr data_ov085_021305c8;

    daPly_c *player = ClosestPlayer();
    if (player == 0) return 1;

    if (TestWaterBelow() == 1) {
        Vector3 sp(mPos.x, mFloorY + 0x3000, mPos.z);
        mDustParticle = Particle::System::NewFitWater(mDustParticle, kPtclWaterSurfaceMove, sp.x,
                                                      sp.y, sp.z, 0);
    }

    Vector3 pp;
    const Vector3 &playerPos = player->mPos;
    pp = playerPos;
    int lim = 0x3e8000;
    if (mRabbitId == 7) lim = 0x2ee000;

    if (Vec3_HorzDist(&mPos, &pp) < lim) {
        int t = mRabbitId;
        int cond = 0;
        if (t == 7 || t == 1 ||
            (t == 4 && mCharacterId == 1) ||
            (t == 4 && mCharacterId == 3)) {
            if (mPos.y + 0x64000 <= pp.y)
                cond = 1;
        }
        if (!cond) {
            mHorzSpeed = 0;
            SetState(&data_ov085_0213066c);
            return 1;
        }
    }

    u32 r = (u32)RandomIntInternal(&data_0209e650.value) >> 8;
    if (mRabbitId == 1) {
        if (Vec3_HorzDist(&mPos, &pp) < 0x4b0000) {
            mTargetAngY = Vec3_HorzAngle(&mPos, &pp) + 0x8000;
            mStateTimer = 30;
        }
    }

    if (mAnmModel.Finished() != 0) {
        switch (mActionStep) {
        case 1:
            mHorzSpeed = 0x4000;
            mAnmModel.SetAnim((BCA_File *)gMipRunAnimationFile.filePtr, 0x40000000, 0x1000, 0);
            (mActionStep)++;
            break;
        case 2:
            mAnmModel.SetAnim((BCA_File *)data_ov085_021305b0.filePtr, 0x40000000, 0x1000, 0);
            mHorzSpeed = 0;
            (mActionStep)++;
            break;
        case 3:
            mStateTimer = (s16)((r & 0x1f) + 30);
            mAnmModel.SetAnim((BCA_File *)data_ov085_021305c0.filePtr, 0, 0x1000, 0);
            mActionStep = 0;
            break;
        }
    }

    if (mActionStep == 0 && mStateTimer == 0) {
        mTargetAngY = Vec3_HorzAngle(&mPos, &mIdlePos);
        mTargetAngY += 0x1800 - ((r & 3) << 12);
        mAnmModel.SetAnim((BCA_File *)data_ov085_021305c8.filePtr, 0x40000000, 0x1000, 0);
        mActionStep = 1;
    }

    ApproachAngle(&mPrevAngle.y, mTargetAngY, 1, 0x500, 0x500);
    return 1;
}

int daMip_c::StateIdleInit()
{
    extern DaMipAnimationFilePtr data_ov085_021305c0;

    mAnmModel.SetAnim((BCA_File *)data_ov085_021305c0.filePtr, 0, 0x1000, 0);
    mIdlePos = mPos;
    mActionStep = 0;
    mStateTimer = 0;
    return 1;
}

/* Stores the state record at mState and runs its Init. Behavior runs the
   record's second pointer-to-member every frame. The ROM reloads mState
   after storing it, so the record is read back through the member. */
int daMip_c::SetState(void *record)
{
    MipStatePair *state;

    mState = (s32)record;
    state = (MipStatePair *)mState;
    if (state->a == 0)
        return 1;
    return (this->*(state->a))();
}

void daMip_c::UpdateMatrixAndShadow()
{
    extern Matrix4x3 data_020a0e68;

    char tmp[0x30];
    Vector3 t;
    Vec3_Asr(&t, &mPos, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, t.x, t.y, t.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, mAngle.x, mAngle.y, mAngle.z);
    mAnmModel.mat4x3 = data_020a0e68;
    MTX_Concat43(mAnmModel.data.transforms, &mAnmModel.mat4x3, tmp);
    Matrix4x3_FromTranslation(&data_020a0e68, mPos.x >> 3, (mPos.y - 0xe000) >> 3, mPos.z >> 3);
    mShadowMtx = data_020a0e68;
    DropShadowRadHeight(mShadowModel1, mShadowMtx, 0x46000, 0x258000, 0xf);
}

/* While carried, the matrix comes from UpdateCarry plus one of four hold
   offsets at data_ov085_021306ec. Sliding and param1 == 2 pick the slot. */
void daMip_c::UpdateCarriedMatrix()
{
    extern Matrix4x3 data_020a0e68;

    int idx;
    Matrix4x3 *res;
    if (!mTalkingPlayer) return;
    if (!mTalkingPlayer->mCarryMatrix)
        return;
    idx = 0;
    if (mTalkingPlayer->IsFrontSliding() || mTalkingPlayer->LostGrabbedObject()) {
        idx = 1;
    }
    if ((int)mTalkingPlayer->param1 == 2) {
        idx = (idx + 2) & 0xff;
    }
    res = UpdateCarry(*mTalkingPlayer, data_ov085_021306ec[idx]);
    mAnmModel.mat4x3 = *res;
    Matrix4x3_FromTranslation(&data_020a0e68, mPos.x >> 3, (mPos.y - 0xc000) >> 3, mPos.z >> 3);
    mShadowMtx = data_020a0e68;
    DropShadowRadHeight(mShadowModel1, mShadowMtx, 0x46000, 0x258000, 0xf);
}

/* Second shadow, on the far side of the mirror plane at x = 0x1086000. */
void daMip_c::UpdateMirrorShadow()
{
    extern Matrix4x3 data_020a0e68;

    Vector3 v;
    Vector3 res;
    Vector3 vd;

    v.x = 0; v.y = 0; v.z = 0;
    res.x = 0; res.y = 0; res.z = 0;
    vd.x = 0; vd.y = 0; vd.z = 0;

    vd = mPos;
    vd.x = 0x1086000;
    v.z = Vec3_HorzDist(&mPos, &vd);
    Matrix4x3_FromRotationY(&data_020a0e68, Vec3_HorzAngle(&mPos, &vd));
    MTX_MultVec43(&v, &data_020a0e68, &res);
    {
        int t;
        vd.x = vd.x + res.x;
        t = mPos.y;
        vd.y = t;
        vd.z = vd.z + res.z;
        Matrix4x3_FromTranslation(&data_020a0e68, vd.x >> 3, (t - 0xc000) >> 3, vd.z >> 3);
    }
    mMirrorMtx = data_020a0e68;
    DropShadowRadHeight(mShadowModel2, mMirrorMtx, 0x46000, 0x258000, 0xf);
}

/* Seven releases, straight-line. Not address order, so not a table walk.
   data_ov085_021305d8 is the handle the key actor releases too. */
int daMip_c::CleanupResources()
{
    extern DaMipModelFile data_ov085_021305d8;
    extern DaMipAnimationFilePtr data_ov085_021305b8;
    extern DaMipAnimationFilePtr gMipRunAnimationFile;
    extern DaMipAnimationFilePtr data_ov085_021305b0;
    extern DaMipAnimationFilePtr data_ov085_021305c8;
    extern DaMipAnimationFilePtr data_ov085_021305c0;
    extern DaMipModelFile data_ov085_021305e0;

    data_ov085_021305d8.Release();
    data_ov085_021305b8.Release();
    gMipRunAnimationFile.Release();
    data_ov085_021305b0.Release();
    data_ov085_021305c8.Release();
    data_ov085_021305c0.Release();
    data_ov085_021305e0.Release();
    return 1;
}

/* Empty. The override suppresses the base pending-destroy body. */
void daMip_c::OnPendingDestroy()
{
}

int daMip_c::Render()
{
    extern signed char gCurrentSublevel;
    extern signed char gCurrentAreaId;

    if (mIsDisabled == 1) return 1;

    if (IsInYoshiMouth(this))
        return 1;

    mScale.x = 0x1500;
    mScale.z = mScale.x;
    mScale.y = mScale.z;

    {
        /* modelFile, then materials. texImageParam is the colour word. */
        ModelComponents *components = &mAnmModel.data;
        BMD_File *modelData = components->modelFile;
        BMD_Material *mat = (BMD_Material *)components->materials;
        for (unsigned int i = 0; i < modelData->numMaterials; i++) {
            mat->texImageParam = mMaterialColor;
            mat++;
        }
    }

    if (gCurrentSublevel == 5 && gCurrentAreaId == 3) {
        RenderMirrorImage();
    }

    mAnmModel.Render(&mScale);
    return 1;
}

/* Mirrored second copy. mat4x3.m[9] is the X translation. 0x421800 is
   (0x1086000 >> 3) << 1, so the subtract reflects X across
   UpdateMirrorShadow's plane. */
void daMip_c::RenderMirrorImage()
{
    extern int data_020a0e68;

    Matrix4x3 tmp;
    tmp = mAnmModel.mat4x3;
    mAnmModel.mat4x3.m[9] = 0x421800 - mAnmModel.mat4x3.m[9];
    func_0203c178(&data_020a0e68, -0x1000, 0x1000, 0x1000);
    MTX_Concat33(&mAnmModel.mat4x3, &data_020a0e68, &mAnmModel.mat4x3);
    mAnmModel.ClearMaterialFlags(0x80);
    mAnmModel.SetMaterialFlags(0x40);
    mAnmModel.dExtAnmModel_c::Render(&mScale);
    mAnmModel.ApplyOpacity(0xff, 0);
    mAnmModel.SetMaterialFlags(0x80);
    mAnmModel.ClearMaterialFlags(0x40);
    mAnmModel.mat4x3 = tmp;
}

/* The glowing-rabbit chase, and it is mostly a conversation.
 *
 * The opening gate depends on the mode. Normally mIsDisabled latches the rabbit
 * out of the level until data_0209caa0.file.flags2 clears bit 0x20000; in mode 0x32 the
 * test inverts -- the rabbit runs only for the player whose character id matches
 * mCharacterId and latches itself off for everyone else, which is what makes one
 * actor behave differently per player.
 *
 * The talk sequence is mTalkState: 0 offers the message, 1 waits for the
 * player's talk state to end, 2 is done. Which message depends on mRabbitId, on
 * whether this rabbit has already been caught, and -- for the last one -- on
 * whether SaveData says all seven are found.
 *
 * mState is the current animation descriptor. It is compared against four ov085
 * objects rather than dereferenced as a type, so it stays an s32 and those four
 * comparisons are what identify the states.
 *
 * The block before Advance calls MipStatePair::b, the record's second
 * pointer-to-member. mState stays an s32: the four comparisons above identify
 * the states by address.
 * */
int daMip_c::Behavior()
{
    extern s8 gCurrentSublevel;
    extern s8 gCurrentAreaId;
    extern void* data_0209f33c;
    extern DaMipAnimationFilePtr data_ov085_021305c0;
    extern MipStatePair data_ov085_0213068c;
    extern MipStatePair data_ov085_021306ac;
    extern MipStatePair data_ov085_021306bc;
    extern MipStatePair data_ov085_021306dc;

    daPly_c *player;

    if (gCurrentSublevel != 0x32) {
        if (mIsDisabled == 1) {
            if (data_0209caa0.file.flags2 & 0x20000)
                mIsDisabled = 0;
            return 1;
        }
    } else {
        player = ClosestPlayer();
        if (player == 0)
            return 1;
        if (!(data_0209caa0.file.flags2 & 0x20000) || mCharacterId != player->mCharacter) {
            mIsDisabled = 1;
            return 1;
        }
        mIsDisabled = 0;
    }

    if (mRabbitId == 7 && !(data_0209caa0.file.flags1 & 0x40))
        data_0209f33c = this;

    if (mIsGlowing != 0) {
        Vector3 pv = mPos;
        pv.y += 0x3c000;
        mGlowParticle = Particle::System::New(mGlowParticle, 0x10d, pv.x, pv.y, pv.z, 0, 0);
    }

    {
        u32 talkStep = mTalkState;
        if (talkStep < 2) {
            if (IsInYoshiMouth(this) == 1) {
                daPly_c *talker = mTalkingPlayer;
                if (talker != 0) {
                    if (talkStep == 0) {
                        Vector3 pos = mPos;
                        int soundId;
                        int messageId;
                        int t;
                        Message::PrepareTalk();
                        t = mRabbitId;
                        if (t != 7) {
                            if (func_02013890(t, (s32)talker->param1) == 0) {
                                Sound::PlaySub(0x27, 0x12, 0x7f, 0x15ccc, 0);
                                if (mRabbitId != 6) { soundId = 0x162; messageId = 0x11e; }
                                else { soundId = 0x160; messageId = 0x126; }
                            } else if (mIsGlowing == 0) {
                                Sound::PlaySub(0x26, 0x12, 0x7f, 0x15ccc, 0);
                                if (mRabbitId != 6) { soundId = 0x162; messageId = 0x122; }
                                else { soundId = 0x160; messageId = 0x12a; }
                            } else {
                                Sound::PlaySub(0x27, 0x12, 0x7f, 0x15ccc, 0);
                                if (SaveData::NumGlowingRabbitsFound() == 7) { messageId = 0x145; soundId = 0x160; }
                                else { soundId = 0x162; messageId = 0x144; }
                            }
                        } else {
                            Sound::PlaySub(0x27, 0x12, 0x7f, 0x15ccc, 0);
                            messageId = 0x13b; soundId = 0x160;
                        }
                        pos.y += 0x64000;
                        if (talker->ShowMessage(*this, messageId, 0, 0, 0) == 1) {
                            func_02012694(soundId, &mCamSpacePos);
                            mTalkState = 1;
                        }
                    } else if (talkStep == 1 && talker->GetTalkState() == -1) {
                        talker->DropActor();
                        mTalkState = 2;
                        talker->mStateFlags |= 0x800;
                    }
                }
            }
        }
    }

    if (UpdateYoshiEat(mWithMeshClsn) != 0) {
        mdCcAc_c.Clear();
        if (mEatenByYoshi != 0) {
            if (mSpitOutTimer == 5)
                mHorzSpeed = 0;
            if (mSpitOutTimer == 0)
                mdCcAc_c.Update();
        }
        if (mEatenByYoshi == 1) {
            mEatenTimer++;
            if (mEatenTimer > 150) {
                mEatenByYoshi = 0;
                mEatenTimer = 0;
            }
        }
        UpdateMatrixAndShadow();
        mCatchState = 2;
        SetState(&data_ov085_021306ac);
        mAnmModel.SetAnim((BCA_File *)data_ov085_021305c0.filePtr, 0, 0x1000, 0);
        if (mTalkingPlayer == 0)
            mTalkingPlayer = ClosestPlayer();
        return 1;
    }

    if (mCatchState != 0) {
        void *p = (void *)mState;
        if (p != &data_ov085_021306ac && p != &data_ov085_0213068c && p != &data_ov085_021306bc &&
            p != &data_ov085_021306dc) {
            SetState(&data_ov085_021306ac);
            mAnmModel.SetAnim((BCA_File *)data_ov085_021305c0.filePtr, 0, 0x1000, 0);
        }
        mHorzSpeed = 0;
    }

    DecIfAbove0_Short(&mStateTimer);

    {
        MipStatePair *state = (MipStatePair *)mState;
        if (state->b != 0)
            (this->*(state->b))();
    }

    mAnmModel.Advance();

    if (IsFlagSet(this, 0x4000))
        UpdateCarriedMatrix();
    else
        UpdateMatrixAndShadow();

    if (gCurrentSublevel == 5 && gCurrentAreaId == 3)
        UpdateMirrorShadow();

    if (!IsFlagSet(this, 0x4000)) {
        if ((void *)mState != &data_ov085_021306ac || mCatchState != 0)
            UpdatePos(&mdCcAc_c);
        mAngle.x = mPrevAngle.x;
        mAngle.y = mPrevAngle.y;
        mAngle.z = mPrevAngle.z;
        UpdateWMClsn(mWithMeshClsn, 0);
        if ((void *)mState != &data_ov085_021306ac)
            UpdateGrab();
    }

    if ((void *)mState != &data_ov085_021306ac) {
        mdCcAc_c.Clear();
        mdCcAc_c.Update();
    }

    return 1;
}

int daMip_c::InitResources()
{
    extern DaMipAnimationFilePtr data_ov085_021305b8;
    extern DaMipAnimationFilePtr gMipRunAnimationFile;
    extern DaMipAnimationFilePtr data_ov085_021305b0;
    extern DaMipAnimationFilePtr data_ov085_021305c8;
    extern DaMipAnimationFilePtr data_ov085_021305c0;
    extern DaMipModelFile data_ov085_021305d8;
    extern DaMipModelFile data_ov085_021305e0;
    extern MipStatePair data_ov085_021306cc;
    extern s32 data_ov085_021305ac;
    extern s8 gCurrentSublevel;

    void* player;
    void* closest;
    int rabbitId;

    dExtFrameCtrl_c::LoadFile(data_ov085_021305b8);
    dExtFrameCtrl_c::LoadFile(gMipRunAnimationFile);
    dExtFrameCtrl_c::LoadFile(data_ov085_021305b0);
    dExtFrameCtrl_c::LoadFile(data_ov085_021305c8);
    dExtFrameCtrl_c::LoadFile(data_ov085_021305c0);
    dExtSimpleModel_c::LoadFile(data_ov085_021305d8);
    mAnmModel.SetFile(dExtSimpleModel_c::LoadFile(data_ov085_021305e0), 1, -1);
    mShadowModel1.InitCylinder();
    mShadowModel2.InitCylinder();

    mPathId = param1 & 0xff;
    if (mPathId == 0xff)
        mPathId = 0;

    /* dActor_c declares param1 u32, but the ROM shifts these two with ASR, not
       LSR -- so this call site reads it signed. Without the casts the function
       comes out two words different; the flat header called 0x008 an s32, which
       is why this was invisible before the rebase. */
    mRabbitId = ((s32)param1 & 0xf00) >> 8;
    if (mRabbitId == 0xff)
        mRabbitId = 0;

    mCharacterId = ((s32)param1 & 0xf000) >> 0xc;
    if (mCharacterId == 0xf)
        mCharacterId = 0;

    rabbitId = mRabbitId;
    if (rabbitId != 7) {
        if (!(data_0209caa0.file.flags1 & 0x40000000))
            return 0;
    }

    if ((rabbitId == 5 && mCharacterId == 0) || (rabbitId == 1 && mCharacterId == 1) ||
        (rabbitId == 6 && mCharacterId == 3)) {
        if (!(data_0209caa0.file.flags2 & 0x80000))
            return 0;
    }

    mVertAccel = -0x1000;
    mTerminalVelocity = -0x1e000;
    mdCcAc_c.Init(this, 0x50000, 0x64000, 0xb00004, 0x9000);
    mTalkingPlayer = 0;
    mAnmModel.speed = 0x1000;
    mWithMeshClsn.Init(this, 0x28000, 0x28000, 0, 0);
    {
        PathPtr path;
        path.FromID(mPathId);
        mPathNodeIndex = 1;
        path.GetNode(mPos, mPathNodeIndex);
        mNumPathNodes = path.NumNodes();
    }
    UpdateMatrixAndShadow();
    mEatingPlayer = 0;
    mScale.x = 0x1000;
    mScale.z = mScale.x;
    mScale.y = mScale.z;

    if (mRabbitId == 7) {
        if (data_0209caa0.file.flags1 & 0x40)
            return 0;
        mIsDisabled = 1;
    } else {
        player = ClosestPlayer();
        if (player == 0)
            return 0;
        if (gCurrentSublevel != 0x32) {
            if (mCharacterId != ((daPly_c *)player)->mCharacter)
                return 0;
        }
    }

    mColorVariant = mCharacterId + 1;
    if (mCharacterId == 3)
        mColorVariant = 0;

    if (mRabbitId == 7) {
        mColorVariant = 0;
    } else {
        closest = ClosestPlayer();
        if (closest != 0) {
            int v;
            v = (s32)((daPly_c *)closest)->param1;
            if (gCurrentSublevel == 0x32)
                v = 1;
            if (func_02013890(mRabbitId, v) != 0 && data_ov085_021305ac < 8) {
                u32 rnd = (u32)RandomIntInternal(&data_0209e650.value) >> 8;
                if (NumStars() >= 0x51) {
                    if ((s32)((daPly_c *)closest)->param1 == 3) {
                        if ((rnd & 0xf) == 0)
                            mColorVariant = 5;
                    } else {
                        if ((rnd & 7) == 0)
                            mColorVariant = 5;
                    }
                } else if (NumStars() >= 0x28) {
                    if ((s32)((daPly_c *)closest)->param1 == 3) {
                        if ((rnd & 0x1f) == 0)
                            mColorVariant = 5;
                    } else {
                        if ((rnd & 0xf) == 0)
                            mColorVariant = 5;
                    }
                }
                if (mColorVariant == 5) {
                    data_ov085_021305ac += 1;
                    mIsGlowing = 1;
                }
            }
        }
    }

    if (gCurrentSublevel == 5) {
        if (mAreaId == 3)
            mFlags = 0x8280;
    }
    mMaterialColor = (mColorVariant << 1) + mAnmModel.data.materials->texImageParam;
    SetState(&data_ov085_021306cc);
    return 1;
}

s32 daMip_c::OnYoshiTryEat() {
  unsigned char v = mEatenByYoshi;
  if (v != 0) return 0;
  return 7;
}

/* Registry factory for the MIP profile. `return new daMip_c()`. */
daMip_c *daMip_c_classInit(void) {
    return new daMip_c();
}

/* Static-init globals (was the handwritten __sinit_ov085_0212f5ec shard).
 * Definition order is the retail initializer's construction order: the two
 * model handles (files 0x452/0x3a4), the five animation handles (files
 * 0x3a5/0x3a6/0x3a7/0x3a8/0x3a9), the four carry-offset vectors (first plain,
 * the rest volatile), the eight state pairs copied from the ROM PMF
 * constants, then the glowing-rabbit count. */
DaMipModelFile data_ov085_021305d8(ASSET_MIP_KEY_BMD);
DaMipModelFile data_ov085_021305e0(ASSET_MIP_BMD);
DaMipAnimationFilePtr data_ov085_021305b8(ASSET_MIP_HOLD_BCA);
DaMipAnimationFilePtr gMipRunAnimationFile(ASSET_MIP_RUN_BCA);
DaMipAnimationFilePtr data_ov085_021305b0(ASSET_MIP_RUN_ED_BCA);
DaMipAnimationFilePtr data_ov085_021305c8(ASSET_MIP_RUN_ST_BCA);
DaMipAnimationFilePtr data_ov085_021305c0(ASSET_MIP_WAIT_BCA);
Vector3 data_ov085_021306ec[4] = {
    Vector3(0x1c000, -0x2a000, 0x18000),
    Vector3(0x1c000, -0x14000, 0x32000),
    Vector3(0x2b000, -0x2a000, 0x18000),
    Vector3(0x30000, -0xa000, 0x32000),
};
MipStatePair data_ov085_021306cc = {data_ov085_02130074, data_ov085_02130044};
MipStatePair data_ov085_0213066c = {data_ov085_02130084, data_ov085_02130094};
MipStatePair data_ov085_0213067c = {data_ov085_021300b4, data_ov085_0213003c};
MipStatePair data_ov085_0213069c = {data_ov085_0213005c, data_ov085_02130064};
MipStatePair data_ov085_021306ac = {data_ov085_0213007c, data_ov085_0213008c};
MipStatePair data_ov085_021306bc = {data_ov085_02130054, data_ov085_0213009c};
MipStatePair data_ov085_021306dc = {data_ov085_0213006c, data_ov085_0213004c};
MipStatePair data_ov085_0213068c = {data_ov085_021300ac, data_ov085_021300a4};
int data_ov085_021305ac = SaveData::NumGlowingRabbitsFound();
