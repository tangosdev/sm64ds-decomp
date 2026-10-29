//cpp
struct Vector3_16f;
namespace Particle {
struct Callback;
struct System {
    static int New(unsigned, unsigned, int, int, int, const ::Vector3_16f *, Callback *);
};
}
/* daMip_c -- MIPS the rabbit (MIP). ov085.
 *
 * defer_codegen off lays .text down in source order. The destructor
 * is the key function: the cartridge has D1 below D0 and no D2.
 *
 * leftovers:
 * - g_profile_MIP stays outside this TU. The file handles, the eight
 *   state records, the carry offsets and the rabbit-count int are
 *   the folded sinit objects at the end of this file.
 * - Matrix copies use a local { s32 m[12] }. Matrix4x3 embeds
 *   Vector3, and a typed copy emits ~Vector3. StateFleeMain copies
 *   the player position through V3Blk for the same reason.
 *   TestWaterBelow's ground probe stays a stack buffer; clsnY is
 *   detect[12].
 * - Render stores the colour at BMD_Material+0x20,
 *   which is pad, and walks materials as an int**. Naming that walk
 *   off data.modelFile shrinks Render, so only the int** slot is
 *   retyped to BMD_File for numMaterials.
 * - UpdateMirrorShadow keeps its Obj overlay. Spelling the position
 *   and a Mtx43* at pad_3e8 moves 34 words.
 * - StateSaveTalkMain and StateCaughtMain write the player's mStateFlags
 *   through a u16 pointer. The direct member form changes the size.
 *   StateIdleMain updates mTargetAngY through a pointer, and
 *   Behavior increments mEatenTimer through (long long)(int)&.
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
#include "Player.h"
#include "PathPtr.h"
#include "decl_Message.h"
#include "Message.h"
#include "SaveData.h"
#include "dBgCh_Gnd.h"
#include "daObj_Mip_Key_c.h"
#include "Sound.h"
#include "cstd.h"
#include "math/Approach.h"

namespace Sound {
bool PlaySub(u32, u32, u32, int, bool);
}

extern "C" {

/* ground probe / collision */
int   SurfaceInfo_TestFlag0x20(void *);

/* dActor_c / dEnemyBase_c, reached by their ROM names.
   dActor_c::ClosestPlayer and dActor_c::FindWithID are deliberately NOT here:
   both are declared members whose recovered signatures carry no by-value class
   parameter, so this TU calls them as members and lets the compiler produce the
   symbol. Everything else keeps the mangled spelling its legacy file matched
   under, because DropShadowRadHeight and Spawn take Fix12<int> and s8/s16 BY
   VALUE in their real declarations and mwccarm passes those differently at the
   call site. */

/* Player / Message / Sound / SaveData */
/* The return type is load-bearing: StateTalkMain tests it in an `if`, so the
   `void` spelling three of the four legacy files used cannot be the one. */

/* model / animation / shadow */


/* maths and the still-unnamed helpers */
s16   Vec3_HorzAngle(const void *, const void *);
s32   Vec3_HorzDist(const void *, const void *);
s32   Vec3_Dist(const void *, const void *);
void  Vec3_Sub(void *, const void *, const void *);
void  Vec3_MulScalar(void *, const void *, int);
void  Vec3_Asr(void *, void *, int);
void  SubVec3(void *, void *, void *);
s32   LenVec3(const void *);
int   AngleDiff(int, int);
int   ApproachAngle(void *, s16, int, int, int);
void  Matrix4x3_FromTranslation(void *, int, int, int);
void  Matrix4x3_FromRotationY(void *, int);
void  Matrix4x3_ApplyInPlaceToRotationXYZExt(void *, int, int, int);
void  MulMat4x3Mat4x3(void *, void *, void *);
void  MulMat3x3Mat3x3(void *, void *, void *);
void  MulVec3Mat4x3(const void *, const void *, void *);
/* Unsigned, and that is measured: InitResources shifts the result right by 8
   and the ROM uses LSR. Declared `int`, the same expression comes out ASR. */
u32   RandomIntInternal(int *);
u8    NumStars(void);
u16   DecIfAbove0_Short(void *);
int   func_02013890(int, int);
void  func_02013944(void);
void func_02012694(unsigned int id, const Vector3 *pos);
void  func_02016acc(void *, int);
void  func_02016b24(void *, int);
u32   func_02022a4c(s32, s32, s32);
u32   func_02022cbc(int, int, s32, s32, s32, const void *);
void  func_0203c178(void *, int, int, int);

}

/* The state records at 0x0213003c..0x021300bc are 8-byte Itanium
   pointer-to-member-function constants, so SetState reaches one through a real
   PMF. The self type is a shadow rather than daMip_c because the record's
   pointer sits at 0x364 -- mState, which the header keeps as an opaque s32
   precisely because Behavior compares it by ADDRESS against four ov085 objects
   rather than dereferencing it as a type. */
struct daMip_cSelf;
typedef int (daMip_cSelf::*daMip_cStateFn)();
struct daMip_cSelf { char pad[0x364]; daMip_cStateFn *pp; };

/* File-scope objects at the end of this file construct the seven resource
 * handles (model files 0x452/0x3a4, animation files 0x3a5/0x3a6/0x3a7/
 * 0x3a8/0x3a9), build the four carry-offset vectors, fill the eight state
 * pairs and read the glowing-rabbit count. mwcc emits __sinit_d_a_mip.cpp
 * from those definitions. The wrapper names are local; the handle
 * constructors and destructors are the ROM resource-family functions,
 * aliased in the manifest. MipVec3's plain overload builds the first vector
 * with ordinary stores; the (int, int, long) overload builds the other
 * three through a volatile lvalue, exactly as retail. Both share the one
 * declared-not-defined destructor (never ~Vector3: a Vector3 here would
 * instantiate its vague-linkage D1 into this TU, which deliberately avoids
 * it); the destructor alias points at the ROM's canonical copy. */
struct DaMipModelFile : SharedFilePtr {
    DaMipModelFile(u32 fileID);
    ~DaMipModelFile();
};

struct DaMipAnimationFilePtr : SharedFilePtr {
    DaMipAnimationFilePtr(u32 fileID);
    ~DaMipAnimationFilePtr();
};

struct MipVec3 {
    int x, y, z;
    MipVec3(int a, int b, int c)
    {
        x = a;
        y = b;
        z = c;
    }
    MipVec3(int a, int b, long c)
    {
        volatile int *p = (volatile int *)&x;
        p[0] = a;
        p[1] = b;
        p[2] = (int)c;
    }
    ~MipVec3();
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
extern MipVec3 data_ov085_021306ec;
extern MipVec3 data_ov085_021306f8;
extern MipVec3 data_ov085_02130704;
extern MipVec3 data_ov085_02130710;
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

// @symbol _ZN7daMip_cD1Ev
// @symbol _ZN7daMip_cD0Ev
/* Empty body. The compiler emits the member destructors and, for D0,
   dEnemyBase_c's operator delete. The cartridge has no D2; the one this
   TU emits is compiler-only and deadstripped. */
daMip_c::~daMip_c()
{
}

// @symbol _ZN7daMip_c14TestWaterBelowEv
/* Probe 0xc8000 above the rabbit. StateFleeMain and StateIdleMain use the
   water flag to pick splash over dust. */
int daMip_c::TestWaterBelow()
{
    /* A POD twin: a real Vector3 local emits the vague-linkage ~Vector3. */
    typedef struct Vector3 { int x, y, z; } Vector3;

    dBgCh_Gnd rg;
    Vector3 v;
    rg.StartDetectingWater();
    int x = mPosX;
    int y = mPosY;
    int z = mPosZ;
    int yk = y + 0xc8000;
    v.x = x;
    v.y = yk;
    v.z = z;
    rg.SetObjAndPos(*(::Vector3 *)&v, this);
    if (rg.DetectClsn()) {
        mFloorY = rg.clsnY;
        if (SurfaceInfo_TestFlag0x20(&rg.surface))
            return 1;
    }
    return 0;
}

// @symbol _ZN7daMip_c10UpdateGrabEv
/* Pick-up. otherOwner is the actor touching the cylinder; actor 0xbf is
   Player. hitFlags 0x1000 is the grab bit. A rabbit already caught once
   (mCatchState) goes to Released instead of Caught. */
void daMip_c::UpdateGrab()
{
    extern MipStatePair data_ov085_021306ac;
    extern MipStatePair data_ov085_021306bc;

    unsigned int id = mdCcAc_c.otherOwner;
    if (id == 0) return;
    Player *o = static_cast<Player *>(dActor_c::FindWithID(id));
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

// @symbol _ZN7daMip_c17StateSaveTalkMainEv
/* Eighth-rabbit epilogue, from StateCaughtMain once all eight glowing
   rabbits are found. Turns toward mSaveTalkPlayer, offers 0x148, then
   Message::DisplaySaving, then Released. */
int daMip_c::StateSaveTalkMain()
{
    /* POD triple. types.h Vector3 has a destructor this function does not emit. */
    struct V3 { int x, y, z; };
    extern unsigned char data_0209d684;
    extern unsigned char data_0209d660;
    extern MipStatePair data_ov085_021306bc;

    Player *player = mSaveTalkPlayer;
    struct V3 vec;
    unsigned char gb;
    int state;

    mTargetAngY = Vec3_HorzAngle((struct V3 *)&mPosX, (struct V3 *)&player->mPosX);
    ApproachAngle(&mPrevAngleY, mTargetAngY, 1, 0x500, 0x500);

    gb = data_0209d684;
    vec.x = mPosX;
    vec.y = mPosY;
    vec.z = mPosZ;
    vec.y += 0x3c000;

    state = mActionStep;
    switch (state) {
    case 0:
        if (player->ShowMessage(*this, 0x148, (Vector3 *)&vec, 0, 0)) {
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
                {
                    unsigned short *hp = &player->mStateFlags;
                    *hp &= ~0x800;
                }
                Message::EndTalk();
                SetState(&data_ov085_021306bc);
            }
        }
        break;
    case 2:
        if (data_0209d660 == 0) {
            unsigned short *hp = &player->mStateFlags;
            *hp &= ~0x800;
            Message::EndTalk();
            SetState(&data_ov085_021306bc);
        }
        break;
    }
    return 1;
}

// @symbol _ZN7daMip_c17StateSaveTalkInitEv
int daMip_c::StateSaveTalkInit()
{
    extern DaMipAnimationFilePtr data_ov085_021305c0;

    mActionStep = 0;
    func_02013944();
    mAnmModel.SetAnim((BCA_File *)data_ov085_021305c0.filePtr, 0, 0x1000, 0);
    return 1;
}

// @symbol _ZN7daMip_c13StateTalkMainEv
/* Ordinary conversation, from StateReleasedMain once StartTalk agrees.
   Faces mSaveTalkPlayer, picks the line from mRabbitId, then Released. */
int daMip_c::StateTalkMain()
{
    struct V3 { int x, y, z; };
    extern MipStatePair data_ov085_021306bc;

    Player *player;
    int *pq;
    struct V3 pos;
    struct V3 pp;
    s16 angle;
    unsigned int id;

    player = mSaveTalkPlayer;
    pq = (int *)&player->mPosX;
    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    pp.x = pq[0];
    pp.y = pq[1];
    pp.z = pq[2];
    angle = Vec3_HorzAngle((struct V3 *)&mPosX, &pp);

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
        if (ApproachLinear(mPrevAngleY, angle, 0x800)) {
            if (Sound::PlaySub(0x26, 0x12, 0x7f, 0x15ccc, 0)) {
                Message::PrepareTalk();
                player->ShowMessage(*this, id, (Vector3 *)&pos, 0, 0);
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

// @symbol _ZN7daMip_c13StateTalkInitEv
int daMip_c::StateTalkInit()
{
    mActionStep = 0;
    return 1;
}

// @symbol _ZN7daMip_c17StateReleasedMainEv
/* Standing free. Drops mTalkingPlayer once the carry flags clear, skips
   the frame while the closest player is mid-message (mStateFlags 0x800),
   and re-opens the conversation through StartTalk. */
int daMip_c::StateReleasedMain()
{
    extern MipStatePair data_ov085_021306dc;

    unsigned short h;
    Player *o;
    Player *cp;
    int a;
    int flags;
    Player *obj;

    obj = mTalkingPlayer;
    if (obj != 0) {
        flags = mFlags;
        /* A plain int, not IsBitSet: the inline swaps obj's and flags'
           registers here. */
        a = (flags & 0x400) != 0;
        if (a || IsBitSet(flags, 0x2000)) {
            /* obj is known non-null here; the ROM still re-tests it. */
            if (a && obj != 0)
                mPrevAngleY = obj->mAngleY;
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
        o = static_cast<Player *>(dActor_c::FindWithID(mdCcAc_c.otherOwner));
        if (o != 0) {
            if (IsPlayerActor(o)) {
                mSaveTalkPlayer = o;
                o = static_cast<Player *>(mSaveTalkPlayer);
                if (o->StartTalk(*this, 0) != 0) {
                    SetState(&data_ov085_021306dc);
                }
            }
        }
    }

    return 1;
}

// @symbol _ZN7daMip_c17StateReleasedInitEv
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

// @symbol _ZN7daMip_c15StateCaughtMainEv
/* Held: the caught conversation, the star spawn (actor 0xe5), and the
   branch that runs once all eight glowing rabbits are found. */
int daMip_c::StateCaughtMain()
{
    /* POD triple. The ::Vector3 casts below are the destructor-bearing type
       ShowMessage and Spawn take by pointer; this local is not that type. */
    typedef struct { s32 x, y, z; } Vector3;

    extern MipStatePair data_ov085_021306cc;
    extern MipStatePair data_ov085_021306bc;
    extern MipStatePair data_ov085_0213068c;
    extern u8 data_0209d660;
    extern u8 data_0209d6bc;

    Player *pl;
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
            int *ps = (int *)&pl->mPosX;
            pv.x = ps[0];
            pv.y = ps[1];
            pv.z = ps[2];
        }

        if (mCatchState != 0) {
            s16 ang = Vec3_HorzAngle(&mPos, &pv);
            ApproachLinear(mPrevAngleY, ang, 0x800);
            if (AngleDiff(mPrevAngleY, ang) > 0x200)
                return 1;
        }

        if ((IsFlagSet(this, 0x4000) || mCatchState == 2) && pl->StartTalk(*this, 1) != 0) {
            pos.x = mPosX;
            soundId = 0;
            pos.y = mPosY;
            pos.z = mPosZ;

            if (mRabbitId != 7) {
                if (func_02013890(mRabbitId, (s32)pl->param1) == 0) {
                    if ((s32)pl->param1 != 3) {
                        Message::PrepareTalk();
                        {
                            int z = soundId;
                            Sound::PlaySub(0x27, 0x12, 0x7f, 0x15ccc, z);
                        }
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
                            {
                                int z = soundId;
                                Sound::PlaySub(0x26, 0x12, 0x7f, 0x15ccc, z);
                            }
                            if (mRabbitId != 6) {
                                msg = (s16)((s32)pl->param1 + 0x11f);
                                soundId = 0x163;
                            } else {
                                msg = (s16)((s32)pl->param1 + 0x127);
                                soundId = 0x161;
                            }
                        } else {
                            Sound::PlaySub(0x27, 0x12, 0x7f, 0x15ccc, soundId);
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

            {
                int y = pos.y;
                int zero = 0;
                y = y + 0x64000;
                pos.y = y;
                if (pl->ShowMessage(*this, (u32)msg, (::Vector3 *)&pos, zero, zero) == 1) {
                    mActionStep = 1;
                    if (soundId != 0)
                        func_02012694(soundId, &mCamSpacePos);
                }
            }
            return 1;
        }
    }

    if (*(volatile s32 *)&mActionStep != 1)
        return 1;

    if (pl->GetTalkState() != -1) {
        if (data_0209d660 != 0 && data_0209d6bc == 9 &&
            (mRabbitId == 7 || func_02013890(mRabbitId, (s32)pl->param1) == 0 || mIsGlowing != 0)) {
            int z = 0;
            Sound::PlaySub(0x27, 0x12, z, 0x8777, z);
        }
        return 1;
    }

    pl->DropActor();
    mdCcAc_c.flags = mdCcAc_c.flags & ~2;
    mHorzSpeed = 0;

    if (mRabbitId == 7) {
        pos7.x = mPosX;
        pos7.y = mPosY;
        pos7.z = mPosZ;
        pos7.y = pos7.y + 0x32000;
        {
            s8 cc = mAreaId;
            int m1 = -1;
            dActor_c *spawned =
                dActor_c::Spawn(0xe5, mRabbitId, *(::Vector3 *)&pos7, &mAngle, cc, m1);
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
        posR.x = mPosX;
        posR.y = mPosY;
        posR.z = mPosZ;
        {
            u32 param = mRabbitId;
            if (mIsGlowing != 0) {
                param = 0x4d;
                func_02013944();
            }
            posR.y = posR.y + 0x32000;
            {
                s8 cc = mAreaId;
                int m1 = -1;
                dActor_c *spawned = dActor_c::Spawn(0xe5, param, *(::Vector3 *)&posR, 0, cc, m1);
                if (spawned != 0)
                    ((daObj_Mip_Key_c *)spawned)->unk_190 = uniqueID;
            }
            func_02012790(0xa);
            mTalkState = 0;
            {
                u16 *pf = &pl->mStateFlags;
                *pf = (u16)(*pf | 0x800);
            }
        }
    } else {
        if (mTalkState == 2) {
            {
                u16 *pf = &pl->mStateFlags;
                *pf = (u16)(*pf & ~0x800);
            }
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
        {
            u16 *pf = &pl->mStateFlags;
            *pf = (u16)(*pf | 0x800);
        }
        mSaveTalkPlayer = pl;
        SetState(&data_ov085_0213068c);
    }
    return 1;
}

// @symbol _ZN7daMip_c15StateCaughtInitEv
int daMip_c::StateCaughtInit()
{
    extern DaMipAnimationFilePtr data_ov085_021305b8;

    mActionStep = 0;
    mHorzSpeed = 0;
    mdCcAc_c.radius = 0x28000;
    mAnmModel.SetAnim((BCA_File *)data_ov085_021305b8.filePtr, 0, 0x1000, 0);
    return 1;
}

// @symbol _ZN7daMip_c13StateRestMainEv
int daMip_c::StateRestMain()
{
    extern MipStatePair data_ov085_021306cc;

    if (mAnmModel.Finished() != 0) {
        SetState(&data_ov085_021306cc);
    }
    return 1;
}

// @symbol _ZN7daMip_c13StateRestInitEv
int daMip_c::StateRestInit()
{
    extern DaMipAnimationFilePtr data_ov085_021305b0;

    mAnmModel.SetAnim((BCA_File *)data_ov085_021305b0.filePtr, 0x40000000, 0x1000, 0);
    return 1;
}

// @symbol _ZN7daMip_c13StateFleeMainEv
/* Runs the path away from the player. Dust or splash at a node, steps
   mPathDir, and falls back to Rest past 0x4b0000. */
int daMip_c::StateFleeMain() {
    /* POD triples. V3Blk's array member is what makes the player-position
       copy a block move; a three-scalar struct scalarises. */
    struct Vector3 {
        s32 x, y, z;
    };
    struct V3Blk {
        s32 w[3];
    };
    extern MipStatePair data_ov085_0213069c;

    Player *pl;
    struct V3Blk v;
    struct Vector3 pos;
    struct Vector3 node;
    struct Vector3 node2;
    struct Vector3 delta;
    struct Vector3 scaled;
    s32 len;
    s32 lim;
    s16 ang;
    int idx;
    int ysave;
    pl = ClosestPlayer();
    if (pl != 0) {
        v = *((struct V3Blk *)&pl->mPosX);
        if (Vec3_Dist(&mPos, &v) > 0x4b0000) {
            SetState(&data_ov085_0213069c);
            return 1;
        }
    }
    {
        s32 t = mAnmModel.currFrame >> 12;
        if (((u16)t) == 0) {
            if (TestWaterBelow() == 1) {
                func_02012694(0x124, &mCamSpacePos);
                func_02022a4c(mPosX, mFloorY + 0x3000, mPosZ);
            } else {
                func_02012694(0x123, &mCamSpacePos);
            }
        }
    }

    if (TestWaterBelow() == 1) {
        s32 pair[2];
        pair[0] = mPosX;
        pair[1] = mFloorY;
        {
            s32 z = mPosZ;
            s32 y = pair[1] + 0x3000;
            s32 x = pair[0];
            *((volatile s32 *)(&pos.x)) = x;
            *((volatile s32 *)(&pos.y)) = y;
            *((volatile s32 *)(&pos.z)) = z;
            mDustParticle =
                func_02022cbc(mDustParticle, 0xe8, *((volatile s32 *)(&pos.x)), *(&pos.y), z, 0);
        }
    }

    PathPtr pathptr;
    pathptr.FromID(mPathId);
    pathptr.GetNode(*(::Vector3 *)&node, mPathNodeIndex);
    ang = Vec3_HorzAngle(&mPos, &node);
    ApproachAngle(&mPrevAngleY, ang, 1, 0x1000, 0x1000);
    idx = mPathNodeIndex + mPathDir;
    if (idx < 0) {
        idx = mNumPathNodes - 1;
    }
    if (idx >= mNumPathNodes) {
        idx = 0;
    }
    pathptr.GetNode(*(::Vector3 *)&node2, (u32)idx);
    lim = 0x26000;
    if (mRabbitId == 7) {
        lim = lim >> 1;
    }
    {
        s32 y = mPosY;
        node.y = y;
        Vec3_Sub(&delta, &mPos, &node);
        len = LenVec3(&delta);
    }
    if ((len == 0) || (len <= lim)) {
        mPosX = node.x;
        mPosY = node.y;
        mPosZ = node.z;
        {
            s32 *p = &mPathNodeIndex;
            *p = (*p) + mPathDir;
        }
        if (mPathNodeIndex >= mNumPathNodes) {
            mPathNodeIndex = 0;
        }
        if (mPathNodeIndex < 0) {
            mPathNodeIndex = mNumPathNodes - 1;
        }
    } else {
        ang = Vec3_HorzAngle(&mPos, &node);
        if (AngleDiff(ang, mPrevAngleY) < 0x2000) {
            ysave = mPosY;
            {
                int s = cstd::fdiv(lim, len);
                Vec3_MulScalar(&scaled, &delta, s);
                SubVec3(&mPos, &scaled, &mPos);
            }
            if (mVertSpeed > 0) {
                mPosY = ysave;
            }
            if (mActionStep == 0) {
                if (mWithMeshClsn.IsOnWall() != 0) {
                    mVertSpeed = 0xa000;
                    mActionStep = 1;
                }
            }
        }
    }
    return 1;
}

// @symbol _ZN7daMip_c13StateFleeInitEv
/* Whichever neighbouring node is further from the player becomes mPathDir.
   The scratch triple is a local Vector3 shadow, not types.h Vector3: the
   ROM emits no scope-exit cleanup here. */
int daMip_c::StateFleeInit()
{
    typedef struct Vector3 {
        int x, y, z;
    } Vector3;
    extern DaMipAnimationFilePtr gMipRunAnimationFile;

    struct Vector3 v;
    struct Vector3 nodes[2];
    int i;
    int d0;
    struct Vector3 *src;

    Player *p = ClosestPlayer();
    mActionStep = 0;
    if (p) {
        src = (struct Vector3 *)&p->mPosX;
        v = *src;
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
            pathptr.GetNode(*(::Vector3 *)&nodes[i], indices[i]);

        mPathDir = 1;
        d0 = Vec3_Dist(&v, &nodes[0]);
        if (d0 > Vec3_Dist(&v, &nodes[1]))
            mPathDir = -1;
    }

    mAnmModel.SetAnim((BCA_File *)gMipRunAnimationFile.filePtr, 0, 0x1000, 0);
    return 1;
}

// @symbol _ZN7daMip_c16StateStartleMainEv
int daMip_c::StateStartleMain()
{
    extern MipStatePair data_ov085_0213067c;

    if (mAnmModel.Finished() != 0)
        SetState(&data_ov085_0213067c);
    return 1;
}

// @symbol _ZN7daMip_c16StateStartleInitEv
int daMip_c::StateStartleInit()
{
    extern DaMipAnimationFilePtr data_ov085_021305c8;

    mAnmModel.SetAnim((BCA_File *)data_ov085_021305c8.filePtr, 0x40000000, 0x1000, 0);
    return 1;
}

// @symbol _ZN7daMip_c13StateIdleMainEv
/* Startle when the closest player comes inside 0x3e8000 from below.
   Otherwise cycle the idle and drift facing around the spot StateIdleInit
   stored. */
int daMip_c::StateIdleMain()
{
    /* Scratch triples are a local Vector3 shadow, not types.h Vector3: the
       ROM emits no scope-exit cleanup here, so the genuine destructor must
       not instantiate (its vague-linkage D1 lived in this TU only for
       these two locals). */
    typedef struct Vector3 { int x, y, z; } Vector3;
    extern int data_0209e650[];
    extern MipStatePair data_ov085_0213066c;
    extern DaMipAnimationFilePtr gMipRunAnimationFile;
    extern DaMipAnimationFilePtr data_ov085_021305b0;
    extern DaMipAnimationFilePtr data_ov085_021305c0;
    extern DaMipAnimationFilePtr data_ov085_021305c8;

    Player *player = ClosestPlayer();
    if (player == 0) return 1;

    if (TestWaterBelow() == 1) {
        Vector3 sp;
        s32 pair[2];
        pair[0] = mPosX;
        pair[1] = mFloorY;
        s32 z = mPosZ;
        s32 y = pair[1] + 0x3000;
        s32 x = pair[0];
        *((volatile s32*)(&sp.x)) = x;
        *((volatile s32*)(&sp.y)) = y;
        *((volatile s32*)(&sp.z)) = z;
        mDustParticle = func_02022cbc(mDustParticle, 0xe8,
            *((volatile s32*)(&sp.x)), *(&sp.y), z, 0);
    }

    Vector3 pp;
    const ::Vector3 *ppp = &player->mPos;
    pp.x = ppp->x;
    pp.y = ppp->y;
    pp.z = ppp->z;
    int lim = 0x3e8000;
    if (mRabbitId == 7) lim = 0x2ee000;

    if (Vec3_HorzDist(&mPos, &pp) < lim) {
        int t = mRabbitId;
        int cond = 0;
        if (t == 7 || t == 1 ||
            (t == 4 && mCharacterId == 1) ||
            (t == 4 && mCharacterId == 3)) {
            if (mPosY + 0x64000 <= pp.y) cond = 1;
        }
        if (!cond) {
            mHorzSpeed = 0;
            SetState(&data_ov085_0213066c);
            return 1;
        }
    }

    u32 r = (u32)RandomIntInternal(data_0209e650) >> 8;
    if (mRabbitId == 1) {
        if (Vec3_HorzDist(&mPos, &pp) < 0x4b0000) {
            mTargetAngY = Vec3_HorzAngle(&mPos, &pp) + 0x8000;
            mStateTimer = 0x1e;
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
            mStateTimer = (s16)((r & 0x1f) + 0x1e);
            mAnmModel.SetAnim((BCA_File *)data_ov085_021305c0.filePtr, 0, 0x1000, 0);
            mActionStep = 0;
            break;
        }
    }

    if (mActionStep == 0 && *(u16 *)&mStateTimer == 0) {
        mTargetAngY = Vec3_HorzAngle(&mPos, (Vector3 *)&mIdlePosX);
        {
            s16 *ang = &mTargetAngY;
            *ang = *ang + (0x1800 - ((r & 3) << 12));
        }
        mAnmModel.SetAnim((BCA_File *)data_ov085_021305c8.filePtr, 0x40000000, 0x1000, 0);
        mActionStep = 1;
    }

    ApproachAngle(&mPrevAngleY, mTargetAngY, 1, 0x500, 0x500);
    return 1;
}

// @symbol _ZN7daMip_c13StateIdleInitEv
int daMip_c::StateIdleInit()
{
    extern DaMipAnimationFilePtr data_ov085_021305c0;

    mAnmModel.SetAnim((BCA_File *)(*(void **)((char *)&data_ov085_021305c0 + 4)), 0, 0x1000, 0);
    mIdlePosX = mPosX;
    mIdlePosY = mPosY;
    mIdlePosZ = mPosZ;
    mActionStep = 0;
    mStateTimer = 0;
    return 1;
}

// @symbol _ZN7daMip_c8SetStateEPv
/* Stores the state record at mState and runs its Init. Behavior runs the
   record's second pointer-to-member every frame. mState stays an s32 because
   Behavior compares the pointer by address, so the call goes through the
   daMip_cSelf shadow. */
int daMip_c::SetState(void *record)
{
    daMip_cSelf *c = (daMip_cSelf *)this;
    daMip_cStateFn *p = (daMip_cStateFn *)record;
    c->pp = p; daMip_cStateFn *q = c->pp; if (*q == 0) return 1; return (c->**q)();
}

// @symbol _ZN7daMip_c21UpdateMatrixAndShadowEv
void daMip_c::UpdateMatrixAndShadow()
{
    struct Vector3 { s32 x, y, z; };
    struct Mtx43 { s32 m[12]; };
    extern struct Mtx43 data_020a0e68;

    char tmp[0x30];
    struct Vector3 t;
    Vec3_Asr(&t, &mPos, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, t.x, t.y, t.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, mAngleX, mAngleY, mAngleZ);
    *(struct Mtx43 *)&mAnmModel.mat4x3 = data_020a0e68;
    MulMat4x3Mat4x3(mAnmModel.data.transforms, &mAnmModel.mat4x3, tmp);
    Matrix4x3_FromTranslation(&data_020a0e68,
        mPosX >> 3,
        (mPosY - 0xe000) >> 3,
        mPosZ >> 3);
    *(struct Mtx43 *)mShadowMtx = data_020a0e68;
    DropShadowRadHeight(mShadowModel1, *(Matrix4x3 *)mShadowMtx, 0x46000, 0x258000, 0xf);
}

// @symbol _ZN7daMip_c19UpdateCarriedMatrixEv
/* While carried, the matrix comes from UpdateCarry plus one of four hold
   offsets at data_ov085_021306ec. Sliding and param1 == 2 pick the slot. */
void daMip_c::UpdateCarriedMatrix()
{
    struct Mtx43 { s32 m[12]; };
    extern MipVec3 data_ov085_021306ec;
    extern int data_020a0e68[];

    int idx;
    void* res;
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
    res = UpdateCarry(*mTalkingPlayer, *(Vector3 *)((char *)&data_ov085_021306ec + idx * 0xc));
    *(struct Mtx43 *)&mAnmModel.mat4x3 = *(struct Mtx43 *)res;
    Matrix4x3_FromTranslation(data_020a0e68, mPosX >> 3, (mPosY - 0xc000) >> 3, mPosZ >> 3);
    *(struct Mtx43 *)mShadowMtx = *(struct Mtx43 *)data_020a0e68;
    DropShadowRadHeight(mShadowModel1, *(Matrix4x3 *)mShadowMtx, 0x46000, 0x258000, 0xf);
}

// @symbol _ZN7daMip_c18UpdateMirrorShadowEv
/* Second shadow, on the far side of the mirror plane at x = 0x1086000.
   The matrix is the 0x30 bytes at this+0x3e8, in the pad after mShadowModel2.
   Naming mPosX and mShadowModel2, with the matrix as its own Mtx43*, stays
   the same size and differs by 34 words. The overlay keeps the block move. */
void daMip_c::UpdateMirrorShadow()
{
    struct Mtx43 { s32 m[12]; };
    struct Vector3_local { int x; int y; int z; };
    struct Obj {
        char pad5c[0x5c];
        int v5c;
        int v60;
        int v64;
        char pad3c0[0x3c0 - 0x68];
        char shadowmodel[0x28];
        struct Mtx43 mtx;
    };
    extern struct Mtx43 data_020a0e68;

    struct Obj *c = (struct Obj *)this;

    struct Vector3_local v;
    struct Vector3_local res;
    struct Vector3_local vd;

    v.x = 0; v.y = 0; v.z = 0;
    res.x = 0; res.y = 0; res.z = 0;
    vd.x = 0; vd.y = 0; vd.z = 0;

    vd.x = c->v5c;
    vd.y = c->v60;
    vd.z = c->v64;
    vd.x = 0x1086000;
    v.z = Vec3_HorzDist((struct Vector3_local *)&c->v5c, &vd);
    Matrix4x3_FromRotationY(&data_020a0e68, Vec3_HorzAngle((struct Vector3_local *)&c->v5c, &vd));
    MulVec3Mat4x3(&v, &data_020a0e68, &res);
    {
        int t;
        vd.x = vd.x + res.x;
        t = c->v60;
        vd.y = t;
        vd.z = vd.z + res.z;
        Matrix4x3_FromTranslation(&data_020a0e68, vd.x >> 3, (t - 0xc000) >> 3, vd.z >> 3);
    }
    c->mtx = data_020a0e68;
    ((dActor_c *)c)
        ->DropShadowRadHeight(*(dExtShadowModel_c *)&c->shadowmodel, *(Matrix4x3 *)&c->mtx, 0x46000,
                              0x258000, 0xf);
}

// @symbol _ZN7daMip_c16CleanupResourcesEv
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

    ((SharedFilePtr *)(&data_ov085_021305d8))->Release();
    ((SharedFilePtr *)(&data_ov085_021305b8))->Release();
    ((SharedFilePtr *)(&gMipRunAnimationFile))->Release();
    ((SharedFilePtr *)(&data_ov085_021305b0))->Release();
    ((SharedFilePtr *)(&data_ov085_021305c8))->Release();
    ((SharedFilePtr *)(&data_ov085_021305c0))->Release();
    ((SharedFilePtr *)(&data_ov085_021305e0))->Release();
    return 1;
}

// @symbol _ZN7daMip_c16OnPendingDestroyEv
/* Empty. The override suppresses the base pending-destroy body. */
void daMip_c::OnPendingDestroy()
{
}

// @symbol _ZN7daMip_c6RenderEv
int daMip_c::Render()
{
    extern signed char data_0209f2f8;
    extern signed char data_02092120;

    if (mIsDisabled == 1) return 1;

    if (IsInYoshiMouth(this))
        return 1;

    mScaleX = 0x1500;
    mScaleZ = mScaleX;
    mScaleY = mScaleZ;

    {
        /* modelFile, then materials.
           +0x20 of each 0x30-byte material is still pad in BMD_Material;
           walking it as BMD_Material:: does not match this loop. */
        int **base = (int **)&mAnmModel.data;
        BMD_File *modelData = (BMD_File *)base[0];
        char* mat = (char*)base[1];
        for (unsigned int i = 0; i < modelData->numMaterials; i++) {
            *(int*)(mat + 0x20) = mMaterialColor;
            mat += 0x30;
        }
    }

    if (data_0209f2f8 == 5 && data_02092120 == 3) {
        RenderMirrorImage();
    }

    mAnmModel.Render(&mScale);
    return 1;
}

// @symbol _ZN7daMip_c17RenderMirrorImageEv
/* Mirrored second copy. mat4x3.t is the translation. 0x421800 is
   (0x1086000 >> 3) << 1, so the subtract reflects X across
   UpdateMirrorShadow's plane. The saved and restored matrix is a flat
   { s32 m[12] } so the copy stays a block move. */
void daMip_c::RenderMirrorImage()
{
    struct Mtx43 { s32 m[12]; };
    extern int data_020a0e68;

    struct Mtx43 tmp;
    tmp = *(struct Mtx43 *)&mAnmModel.mat4x3;
    mAnmModel.mat4x3.t.x = 0x421800 - mAnmModel.mat4x3.t.x;
    func_0203c178(&data_020a0e68, -0x1000, 0x1000, 0x1000);
    MulMat3x3Mat3x3(&mAnmModel.mat4x3, &data_020a0e68, &mAnmModel.mat4x3);
    func_02016acc(&mAnmModel, 0x80);
    func_02016b24(&mAnmModel, 0x40);
    mAnmModel.dExtAnmModel_c::Render(&mScale);
    mAnmModel.ApplyOpacity(0xff, 0);
    func_02016b24(&mAnmModel, 0x80);
    func_02016acc(&mAnmModel, 0x40);
    *(struct Mtx43 *)&mAnmModel.mat4x3 = tmp;
}

// @symbol _ZN7daMip_c8BehaviorEv
/* The glowing-rabbit chase, and it is mostly a conversation.
 *
 * The opening gate depends on the mode. Normally mIsDisabled latches the rabbit
 * out of the level until data_0209caa0[2] clears bit 0x20000; in mode 0x32 the
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
 * The block before dExtFrameCtrl_c::Advance is the ARM/Itanium pointer-to-member
 * sequence written out -- adjustment word, virtual bit, vtable index or direct
 * address -- run on whatever mState points at. It is kept verbatim because there
 * is no recovered type for the descriptor to call a member through. `c` is the
 * char* base that sequence adds the adjustment to.
 *
 * mEatenTimer's increment is a byte update through (long long)(int)&mEatenTimer.
 * `mEatenTimer = mEatenTimer + 1` shrinks Behavior from 0x5cc to 0x5c0. */
int daMip_c::Behavior()
{
    /* A LOCAL COORDINATE TRIPLE, NOT A Vector3 OBJECT. Vector3 declares a
       destructor (see include/types.h -- the ROM's __cxa_vec_cleanup calls prove the
       type has one), so a Vector3 local would be destroyed at scope exit and this
       function would come out 8 bytes longer. The ROM emits no cleanup for either of
       these, which is itself the evidence that they were never Vector3s: they are
       scratch x/y/z the code fills and reads back. */
    typedef volatile struct { Fix12i x, y, z; } Vec3Scratch;

    extern s8 data_0209f2f8;
    extern s8 data_02092120;
    extern int data_0209caa0[];
    extern void* data_0209f33c;
    extern DaMipAnimationFilePtr data_ov085_021305c0;
    extern MipStatePair data_ov085_0213068c;
    extern MipStatePair data_ov085_021306ac;
    extern MipStatePair data_ov085_021306bc;
    extern MipStatePair data_ov085_021306dc;

    char* c = (char*)this;
    Player *player;

    if (data_0209f2f8 != 0x32) {
        if (mIsDisabled == 1) {
            if (data_0209caa0[2] & 0x20000)
                mIsDisabled = 0;
            return 1;
        }
    } else {
        player = ClosestPlayer();
        if (player == 0)
            return 1;
        if (!(data_0209caa0[2] & 0x20000) || mCharacterId != player->mCharacter) {
            mIsDisabled = 1;
            return 1;
        }
        mIsDisabled = 0;
    }

    if (mRabbitId == 7 && !(data_0209caa0[1] & 0x40))
        data_0209f33c = c;

    if (mIsGlowing != 0) {
        Vec3Scratch pv;
        pv.x = mPosX;
        pv.y = mPosY;
        pv.z = mPosZ;
        pv.y = mPosY + 0x3c000;
        mGlowParticle =
            Particle::System::New(*(volatile u32 *)&mGlowParticle, 0x10d, pv.x, pv.y, mPosZ, 0, 0);
    }

    {
        u32 talkStep = mTalkState;
        if (talkStep < 2) {
            if (IsInYoshiMouth(this) == 1) {
                Player *talker = mTalkingPlayer;
                if (talker != 0) {
                    if (talkStep == 0) {
                        Vec3Scratch pos;
                        int soundId;
                        int messageId;
                        int t;
                        pos.x = mPosX;
                        pos.y = mPosY;
                        pos.z = mPosZ;
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
                        if (talker->ShowMessage(*(fBase_c *)c, messageId, 0, 0, 0) == 1) {
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
            *(u8 *)((long long)(int)&mEatenTimer) = *(u8 *)((long long)(int)&mEatenTimer) + 1;
            if (mEatenTimer > 0x96) {
                mEatenByYoshi = 0;
                mEatenTimer = 0;
            }
        }
        UpdateMatrixAndShadow();
        mCatchState = 2;
        SetState(&data_ov085_021306ac);
        mAnmModel.SetAnim((BCA_File *)(*(void **)((char *)&data_ov085_021305c0 + 4)), 0, 0x1000, 0);
        if (*(void**)&mTalkingPlayer == 0)
            *(void**)&mTalkingPlayer = ClosestPlayer();
        return 1;
    }

    if (mCatchState != 0) {
        void* p = *(void**)&mState;
        if (p != (void*)&data_ov085_021306ac && p != (void*)&data_ov085_0213068c &&
            p != (void*)&data_ov085_021306bc && p != (void*)&data_ov085_021306dc) {
            SetState(&data_ov085_021306ac);
            mAnmModel.SetAnim((BCA_File *)(*(void **)((char *)&data_ov085_021305c0 + 4)), 0, 0x1000,
                              0);
        }
        mHorzSpeed = 0;
    }

    DecIfAbove0_Short(&mStateTimer);

    {
        int* p = *(int**)&mState;
        if (p[2] != 0) {
            int* q = p + 2;
            int adj = q[1];
            void* thiz = c + (adj >> 1);
            void (*fn)(void*);
            if (adj & 1)
                fn = *(void(**)(void*))(*(char**)thiz + q[0]);
            else
                fn = (void(*)(void*))q[0];
            fn(thiz);
        }
    }

    mAnmModel.Advance();

    if (IsFlagSet(this, 0x4000))
        UpdateCarriedMatrix();
    else
        UpdateMatrixAndShadow();

    if (data_0209f2f8 == 5 && data_02092120 == 3)
        UpdateMirrorShadow();

    if (!IsFlagSet(this, 0x4000)) {
        if (*(void **)&mState != (void *)&data_ov085_021306ac || mCatchState != 0)
            UpdatePos(&mdCcAc_c);
        mAngleX = mPrevAngleX;
        mAngleY = mPrevAngleY;
        mAngleZ = mPrevAngleZ;
        UpdateWMClsn(mWithMeshClsn, 0);
        if (*(void **)&mState != (void *)&data_ov085_021306ac)
            UpdateGrab();
    }

    if (*(void**)&mState != (void*)&data_ov085_021306ac) {
        mdCcAc_c.Clear();
        mdCcAc_c.Update();
    }

    return 1;
}

// @symbol _ZN7daMip_c13InitResourcesEv
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
    extern int data_0209caa0[];
    extern s8 data_0209f2f8;
    extern int data_0209e650;

    void* player;
    void* closest;
    int rabbitId;

    dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)&data_ov085_021305b8);
    dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)&gMipRunAnimationFile);
    dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)&data_ov085_021305b0);
    dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)&data_ov085_021305c8);
    dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)&data_ov085_021305c0);
    dExtSimpleModel_c::LoadFile(*(SharedFilePtr *)&data_ov085_021305d8);
    mAnmModel.SetFile(
        (BMD_File *)dExtSimpleModel_c::LoadFile(*(SharedFilePtr *)&data_ov085_021305e0), 1, -1);
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
        if (!(data_0209caa0[1] & 0x40000000))
            return 0;
    }

    if ((rabbitId == 5 && mCharacterId == 0) || (rabbitId == 1 && mCharacterId == 1) ||
        (rabbitId == 6 && mCharacterId == 3)) {
        if (!(data_0209caa0[2] & 0x80000))
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
    mScaleX = 0x1000;
    mScaleZ = mScaleX;
    mScaleY = mScaleZ;

    if (mRabbitId == 7) {
        if (data_0209caa0[1] & 0x40)
            return 0;
        mIsDisabled = 1;
    } else {
        player = ClosestPlayer();
        if (player == 0)
            return 0;
        if (data_0209f2f8 != 0x32) {
            if (mCharacterId != ((Player *)player)->mCharacter)
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
            v = (s32)((Player *)closest)->param1;
            if (data_0209f2f8 == 0x32)
                v = 1;
            if (func_02013890(mRabbitId, v) != 0 && data_ov085_021305ac < 8) {
                u32 rnd = RandomIntInternal(&data_0209e650) >> 8;
                if (NumStars() >= 0x51) {
                    if ((s32)((Player *)closest)->param1 == 3) {
                        if ((rnd & 0xf) == 0)
                            mColorVariant = 5;
                    } else {
                        if ((rnd & 7) == 0)
                            mColorVariant = 5;
                    }
                } else if (NumStars() >= 0x28) {
                    if ((s32)((Player *)closest)->param1 == 3) {
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

    if (data_0209f2f8 == 5) {
        if (mAreaId == 3)
            mFlags = 0x8280;
    }
    mMaterialColor = (mColorVariant << 1) + *(s32 *)((char *)mAnmModel.data.materials + 0x20);
    SetState(&data_ov085_021306cc);
    return 1;
}

// @symbol _ZN7daMip_c13OnYoshiTryEatEv
s32 daMip_c::OnYoshiTryEat() {
  unsigned char v = mEatenByYoshi;
  if (v != 0) return 0;
  return 7;
}

// @symbol daMip_c_classInit
/* Registry factory for the MIP profile. `return new daMip_c()`. */
extern "C" daMip_c *daMip_c_classInit(void)
{
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
MipVec3 data_ov085_021306ec(0x1c000, -0x2a000, 0x18000);
MipVec3 data_ov085_021306f8(0x1c000, -0x14000, (long)0x32000);
MipVec3 data_ov085_02130704(0x2b000, -0x2a000, (long)0x18000);
MipVec3 data_ov085_02130710(0x30000, -0xa000, (long)0x32000);
MipStatePair data_ov085_021306cc = {data_ov085_02130074, data_ov085_02130044};
MipStatePair data_ov085_0213066c = {data_ov085_02130084, data_ov085_02130094};
MipStatePair data_ov085_0213067c = {data_ov085_021300b4, data_ov085_0213003c};
MipStatePair data_ov085_0213069c = {data_ov085_0213005c, data_ov085_02130064};
MipStatePair data_ov085_021306ac = {data_ov085_0213007c, data_ov085_0213008c};
MipStatePair data_ov085_021306bc = {data_ov085_02130054, data_ov085_0213009c};
MipStatePair data_ov085_021306dc = {data_ov085_0213006c, data_ov085_0213004c};
MipStatePair data_ov085_0213068c = {data_ov085_021300ac, data_ov085_021300a4};
int data_ov085_021305ac = SaveData::NumGlowingRabbitsFound();
