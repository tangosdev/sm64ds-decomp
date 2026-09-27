//cpp
/* daMip_c -- MIPS the rabbit (MIP). ov085, 32 functions.
 *
 * #pragma defer_codegen off lays .text down in source order. The destructor
 * is the key function: the cartridge has D1 below D0 and no D2.
 *
 * Known limits, from this TU:
 * - ModelAnim::SetAnim, dCcAc_c::Init, dBgCh_Actr::Init and
 *   dActor_c::DropShadowRadHeight stay mangled. Each takes Fix12 by value
 *   (notes/mwccarm-codegen.md 6az). dBgCh_Actr::Init's header spells Fix12i,
 *   which mangles as int.
 * - Sound::PlaySub and Particle::System::New stay mangled for the same reason.
 * - Animation file handles and the eight state records stay data_ov085_*.
 *   Each call keeps the extern spelling it already matched under. The static
 *   initializer that owns them is another file. g_profile_MIP stays outside.
 * - Player param1 (+8), mCharacter (+0x6d9) and mStateFlags (+0x6ce) stay
 *   raw offsets. A member load there does not match this TU.
 * - Matrix copies use a local { s32 m[12] }. math/Matrix.h is included before
 *   common.h, so Matrix4x3 embeds Vector3 and a typed copy emits ~Vector3.
 * - The mirror shadow matrix sits at this+0x3e8, in the pad after
 *   mShadowModel2. UpdateMirrorShadow keeps an overlay so the copy stays
 *   a block move. RenderMirrorImage reflects translation X at this+0x340
 *   (mat4x3 + 0x24); indexing that word as m[9] does not match.
 * - Render's material walk stays an int** over modelFile/materials.
 *   BMD_Material +0x20 is pad in the shared header, and the typed loop
 *   does not match.
 * - StateIdleMain still addresses mStateTimer (this+0x100), mTargetAngY
 *   (this+0x424) and the animation base (this+0x350) from a char*. A
 *   named store in that switch does not match.
 * - Behavior's mEatenTimer increment is a (long long)(int) round-trip.
 *   mEatenTimer + 1 changes the function. Animation::Advance offsets from
 *   this+0x350; &mModelAnim + 0x50 costs a word.
 * - StateFleeMain copies the player's position as s32[3]. A three-scalar
 *   struct scalarises; the ROM block-moves.
 * - TestWaterBelow's ground probe is a stack buffer. A dBgCh_Gnd local would
 *   run ~dBgCh_Gnd. clsnY is read at detect[12] (probe + 0x44).
 */

#include "daMip_c.h"
#include "common.h"
#include "SharedFilePtr.h"
#include "Player.h"
#include "PathPtr.h"
#include "decl_Message.h"
#include "decl_PathPtr.h"
#include "Message.h"
#include "SaveData.h"
#include "dBgCh_Gnd.h"

extern "C" {

/* ground probe / collision */
void *_ZN9dBgCh_GndC1Ev(void *);
void  _ZN9dBgCh_GndD1Ev(void *);
int   SurfaceInfo_TestFlag0x20(void *);
void  _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *, void *, s32, s32, void *, void *);
void  _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *, void *, s32, s32, u32, u32);

/* dActor_c / dEnemyBase_c, reached by their ROM names.
   dActor_c::ClosestPlayer and dActor_c::FindWithID are deliberately NOT here:
   both are declared members whose recovered signatures carry no by-value class
   parameter, so this TU calls them as members and lets the compiler produce the
   symbol. Everything else keeps the mangled spelling its legacy file matched
   under, because DropShadowRadHeight and Spawn take Fix12<int> and s8/s16 BY
   VALUE in their real declarations and mwccarm passes those differently at the
   call site. */
void  _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void *, void *, void *, int, int, u32);

/* Player / Message / Sound / SaveData */
/* The return type is load-bearing: StateTalkMain tests it in an `if`, so the
   `void` spelling three of the four legacy files used cannot be the one. */
bool  _ZN5Sound7PlaySubEjjj5Fix12IiEb(u32, u32, u32, s32, int);

/* model / animation / shadow */
void  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *, void *, int, s32, u32);

/* path, particle, construction */
void *_ZN7PathPtrC1Ev(void *);
int   _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(u32, u32, s32, s32, s32, void *, void *);

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
/* Same as Sound::PlaySub: StateTalkMain guards on the result. */
int   _Z14ApproachLinearRsss(s16 *, s16, s16);
int   _ZN4cstd4fdivEii(int, int);
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
unsigned int func_02012790(unsigned int);
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
    typedef struct Vector3 { int x, y, z; } Vector3;
    struct RG { char a[0x14]; int detect[16]; };

    struct RG rg;
    Vector3 v;
    _ZN9dBgCh_GndC1Ev(&rg);
    ((dBgCh *)&rg)->StartDetectingWater();
    int x = mPosX;
    int y = mPosY;
    int z = mPosZ;
    int yk = y + 0xc8000;
    v.x = x;
    v.y = yk;
    v.z = z;
    ((dBgCh_Gnd *)&rg)->SetObjAndPos(*(::Vector3 *)&v, this);
    if (((dBgCh_Gnd *)&rg)->dBgCh_Gnd::DetectClsn()) {
        mFloorY = rg.detect[12];
        if (SurfaceInfo_TestFlag0x20(rg.detect)) {
            _ZN9dBgCh_GndD1Ev(&rg);
            return 1;
        }
    }
    _ZN9dBgCh_GndD1Ev(&rg);
    return 0;
}

// @symbol _ZN7daMip_c10UpdateGrabEv
/* Pick-up. otherOwner is the actor touching the cylinder; actor 0xbf is
   Player. hitFlags 0x1000 is the grab bit. A rabbit already caught once
   (unk_426) goes to Released instead of Caught. */
void daMip_c::UpdateGrab()
{
    extern int data_ov085_021306ac[];
    extern int data_ov085_021306bc[];

    unsigned int id = mdCcAc_c.otherOwner;
    if (id == 0) return;
    dActor_c *o = dActor_c::FindWithID(id);
    if (o == 0) return;
    int b = (o->actorID == 0xbf);
    if (b == 0) return;
    if ((mdCcAc_c.hitFlags & 0x1000) == 0) return;
    if (((Player *)o)->TryGrab(*this) == 0) return;
    mTalkingPlayer = (Player *)o;
    mdCcAc_c.flags |= 2;
    if (unk_426 == 0) {
        SetState(data_ov085_021306ac);
    } else {
        SetState(data_ov085_021306bc);
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
    extern char data_ov085_021306bc[];

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
                    unsigned short *hp = (unsigned short *)((char *)player + 0x6ce);
                    *hp &= ~0x800;
                }
                Message::EndTalk();
                SetState(data_ov085_021306bc);
            }
        }
        break;
    case 2:
        if (data_0209d660 == 0) {
            unsigned short *hp = (unsigned short *)((char *)player + 0x6ce);
            *hp &= ~0x800;
            Message::EndTalk();
            SetState(data_ov085_021306bc);
        }
        break;
    }
    return 1;
}

// @symbol _ZN7daMip_c17StateSaveTalkInitEv
int daMip_c::StateSaveTalkInit()
{
    extern int data_ov085_021305c0[];

    mActionStep = 0;
    func_02013944();
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, (void *)data_ov085_021305c0[1], 0, 0x1000, 0);
    return 1;
}

// @symbol _ZN7daMip_c13StateTalkMainEv
/* Ordinary conversation, from StateReleasedMain once StartTalk agrees.
   Faces mSaveTalkPlayer, picks the line from mRabbitId, then Released. */
int daMip_c::StateTalkMain()
{
    struct V3 { int x, y, z; };
    extern int data_ov085_021306bc[];

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
        if (_Z14ApproachLinearRsss(&mPrevAngleY, angle, 0x800)) {
            if (_ZN5Sound7PlaySubEjjj5Fix12IiEb(0x26, 0x12, 0x7f, 0x15ccc, 0)) {
                Message::PrepareTalk();
                player->ShowMessage(*this, id, (Vector3 *)&pos, 0, 0);
            }
        }
        break;
    case 1:
        break;
    default:
        if (_ZN5Sound7PlaySubEjjj5Fix12IiEb(0x26, 0x7f, 0, 0x7444, 0)) {
            Message::EndTalk();
            SetState(data_ov085_021306bc);
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
    extern int data_ov085_021306dc;

    unsigned short h;
    int ok;
    dActor_c *o;
    Player *cp;
    int b;
    int a;
    int flags;
    Player *obj;

    obj = mTalkingPlayer;
    if (obj != 0) {
        flags = mFlags;
        a = (flags & 0x400) ? 1 : 0;
        if (a == 0) {
            b = (flags & 0x2000) ? 1 : 0;
            if (b == 0)
                goto after_clear;
        }
        if (a != 0) {
            if (obj != 0) {
                mPrevAngleY = obj->mAngleY;
            }
        }
        b = 0;
        mdCcAc_c.flags = mdCcAc_c.flags & ~2;
        mTalkingPlayer = (Player *)b;
    after_clear:
        a = (mFlags & 0x100) ? 1 : 0;
        if (a == 0) {
            mTalkingPlayer = 0;
        }
        obj = mTalkingPlayer;
        if (obj != 0) {
            if (*(unsigned char *)((char *)obj + 0x706) != 0) {
                mTalkingPlayer = 0;
            }
        }
    }

    cp = ClosestPlayer();
    if (cp != 0) {
        h = *(unsigned short *)((char *)cp + 0x600 + 0xce);
        h = (unsigned short)(h & 0x800);
        if (h != 0)
            return 1;
    }

    if ((mdCcAc_c.hitFlags & 0x8000000) != 0) {
        o = dActor_c::FindWithID(mdCcAc_c.otherOwner);
        if (o != 0) {
            ok = (int)(o->actorID == (unsigned short)0xbf);
            if (ok != 0) {
                mSaveTalkPlayer = (Player *)o;
                o = mSaveTalkPlayer;
                if (((Player *)o)->StartTalk(*this, 0) != 0) {
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
    struct G { int w[2]; };
    extern struct G data_ov085_021305c0;

    int *a = (int *)&mdCcAc_c.vulnFlags;
    int *b = (int *)&mdCcAc_c.flags;
    mVertAccel = -0x1000;
    unk_426 = 1;
    *a &= ~0x1000;
    *b |= 0x4000000;
    mdCcAc_c.radius = 0x78000;
    *a &= ~0x8000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, (void *)data_ov085_021305c0.w[1], 0, 0x1000, 0);
    return 1;
}

// @symbol _ZN7daMip_c15StateCaughtMainEv
/* Held: the caught conversation, the star spawn (actor 0xe5), and the
   branch that runs once all eight glowing rabbits are found. */
int daMip_c::StateCaughtMain()
{
    typedef int s32;
    typedef short s16;
    typedef unsigned int u32;
    typedef unsigned short u16;
    typedef unsigned char u8;
    typedef signed char s8;
    /* POD triple. The ::Vector3 casts below are the destructor-bearing type
       ShowMessage and Spawn take by pointer; this local is not that type. */
    typedef struct { s32 x, y, z; } Vector3;

    extern char data_ov085_021306cc[];
    extern char data_ov085_021306bc[];
    extern char data_ov085_0213068c[];
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
        SetState(data_ov085_021306cc);
        return 1;
    }

    if (mActionStep == 0) {
        {
            int *ps = (int *)&pl->mPosX;
            pv.x = ps[0];
            pv.y = ps[1];
            pv.z = ps[2];
        }

        if (unk_426 != 0) {
            s16 ang = Vec3_HorzAngle((Vector3 *)&mPosX, &pv);
            _Z14ApproachLinearRsss(&mPrevAngleY, ang, 0x800);
            if (AngleDiff(mPrevAngleY, ang) > 0x200)
                return 1;
        }

        {
            int guard = (mFlags & 0x4000) ? 1 : 0;
            if (guard == 0) {
                if (unk_426 != 2)
                    goto after_first_section;
            }
            {
                if (pl->StartTalk(*this, 1) != 0) {
                    pos.x = mPosX;
                    soundId = 0;
                    pos.y = mPosY;
                    pos.z = mPosZ;

                    if (mRabbitId == 7)
                        goto msg_13c;
                    if (func_02013890(mRabbitId, *(s32 *)((char *)pl + 8)) == 0) {
                        if (*(s32 *)((char *)pl + 8) != 3) {
                            Message::PrepareTalk();
                            {
                                int z = soundId;
                                _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x27, 0x12, 0x7f, 0x15ccc, z);
                            }
                            if (mRabbitId == 6)
                                goto msg_123a;
                            msg = (s16)(*(s32 *)((char *)pl + 8) + 0x11b);
                            soundId = 0x163;
                            goto have_msg;
                        msg_123a:
                            msg = (s16)(*(s32 *)((char *)pl + 8) + 0x123);
                            soundId = 0x161;
                            goto have_msg;
                        }
                        if (mRabbitId != 6)
                            msg = 0x12b;
                        else
                            msg = 0x12c;
                        goto have_msg;
                    } else {
                        if (*(s32 *)((char *)pl + 8) != 3) {
                            Message::PrepareTalk();
                            if (mIsGlowing == 0) {
                                {
                                    int z = soundId;
                                    _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x26, 0x12, 0x7f, 0x15ccc, z);
                                }
                                if (mRabbitId == 6)
                                    goto msg_127a;
                                msg = (s16)(*(s32 *)((char *)pl + 8) + 0x11f);
                                soundId = 0x163;
                                goto have_msg;
                            msg_127a:
                                msg = (s16)(*(s32 *)((char *)pl + 8) + 0x127);
                                soundId = 0x161;
                            } else {
                                _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x27, 0x12, 0x7f, 0x15ccc, soundId);
                                if (SaveData::NumGlowingRabbitsFound() == 7) {
                                    msg = 0x143;
                                    soundId = 0x160;
                                } else {
                                    soundId = 0x162;
                                    msg = 0x142;
                                }
                            }
                            goto have_msg;
                        }
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
                    goto have_msg;
                    msg_13c:
                        msg = 0x13c;
                    have_msg:
                    {
                        int y = pos.y;
                        int zero = 0;
                        y = y + 0x64000;
                        pos.y = y;
                        if (pl->ShowMessage(*this, (u32)msg, (::Vector3 *)&pos, zero, zero) == 1) {
                            mActionStep = 1;
                            if (soundId != 0)
                                func_02012694(soundId, (const ::Vector3 *)&mCamSpacePosX);
                        }
                    }
                    return 1;
                }
            }
        }
    }
    after_first_section:
    if (*(volatile s32 *)&mActionStep != 1)
        return 1;

    if (pl->GetTalkState() != -1) {
        if (data_0209d660 != 0) {
            if (data_0209d6bc == 9) {
                if (mRabbitId != 7) {
                    if (func_02013890(mRabbitId, *(s32 *)((char *)pl + 8)) != 0) {
                        if (mIsGlowing == 0)
                            goto talk_active_done;
                    }
                }
                {
                    int z = 0;
                    _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x27, 0x12, z, 0x8777, z);
                }
            }
        }
    talk_active_done:
        return 1;
    }

    pl->DropActor();
    {
        s32 *p128 = (s32 *)&mdCcAc_c.flags;
        *p128 = *p128 & ~2;
    }
    mHorzSpeed = 0;

    if (mRabbitId == 7) {
        pos7.x = mPosX;
        pos7.y = mPosY;
        pos7.z = mPosZ;
        pos7.y = pos7.y + 0x32000;
        {
            s8 cc = mAreaId;
            int m1 = -1;
            void *spawned = dActor_c::Spawn(0xe5, mRabbitId, *(::Vector3 *)&pos7, (Vector3_16 *)&mAngleX, cc, m1);
            if (spawned != 0)
                *(s32 *)((char *)spawned + 0x190) = uniqueID;
        }
        func_02012790(0xa);
        mTalkState = 0;
        mTalkingPlayer = 0;
        SetState(data_ov085_021306bc);
        return 1;
    }

    if (func_02013890(mRabbitId, *(s32 *)((char *)pl + 8)) != 0) {
        if (mIsGlowing == 0)
            goto no_spawn;
        if (SaveData::NumGlowingRabbitsFound() != 7)
            goto no_spawn;
    }
    {
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
                void *spawned = dActor_c::Spawn(0xe5, param, *(::Vector3 *)&posR, 0, cc, m1);
                if (spawned != 0)
                    *(s32 *)((char *)spawned + 0x190) = uniqueID;
            }
            func_02012790(0xa);
            mTalkState = 0;
            {
                u16 *pf = (u16 *)((char *)pl + 0x6ce);
                *pf = (u16)(*pf | 0x800);
            }
        }
        goto after_spawn;
    }
no_spawn:
    if (mTalkState == 2) {
        {
            u16 *pf = (u16 *)((char *)pl + 0x6ce);
            *pf = (u16)(*pf & ~0x800);
        }
        mTalkState = 0;
    }
    if (mIsGlowing == 0) {
        _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x26, 0x7f, 0, 0x7444, 0);
        Message::EndTalk();
    }
after_spawn:
    ;

    mTalkingPlayer = 0;

    if (mIsGlowing == 0)
        goto do_306bc;
    if (SaveData::NumGlowingRabbitsFound() != 8)
        goto flag_path;
do_306bc:
    SetState(data_ov085_021306bc);
    goto final_return;
flag_path:
    {
        u16 *pf = (u16 *)((char *)pl + 0x6ce);
        *pf = (u16)(*pf | 0x800);
    }
    mSaveTalkPlayer = pl;
    SetState(data_ov085_0213068c);
final_return:
    return 1;
}

// @symbol _ZN7daMip_c15StateCaughtInitEv
int daMip_c::StateCaughtInit()
{
    extern int data_ov085_021305b8[];

    mActionStep = 0;
    mHorzSpeed = 0;
    mdCcAc_c.radius = 0x28000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, (void*)data_ov085_021305b8[1], 0, 0x1000, 0);
    return 1;
}

// @symbol _ZN7daMip_c13StateRestMainEv
int daMip_c::StateRestMain()
{
    extern int data_ov085_021306cc[];

    if (((Animation *)((char *)this + 0x350))->Finished() != 0) {
        SetState(data_ov085_021306cc);
    }
    return 1;
}

// @symbol _ZN7daMip_c13StateRestInitEv
int daMip_c::StateRestInit()
{
    extern int *data_ov085_021305b0[];

    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, data_ov085_021305b0[1], 0x40000000, 0x1000, 0);
    return 1;
}

// @symbol _ZN7daMip_c13StateFleeMainEv
/* Runs the path away from the player. Dust or splash at a node, steps
   mPathDir, and falls back to Rest past 0x4b0000. */
int daMip_c::StateFleeMain()
{
    /* POD triples. V3Blk's array member is what makes the player-position
       copy a block move; a three-scalar struct scalarises. */
    struct Vector3 { s32 x, y, z; };
    struct V3Blk   { s32 w[3]; };
    extern char data_ov085_0213069c[];

    Player *pl;
    char pathptr[8];
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
    if (pl != 0)
    {
      v = *((struct V3Blk *)((char *)pl + 0x5c));
      if (Vec3_Dist((struct Vector3 *)&mPosX, &v) > 0x4b0000)
      {
        SetState(data_ov085_0213069c);
        return 1;
      }
    }
    {
      s32 t = mModelAnim.currFrame >> 12;
      if (((u16) t) == 0)
      {
        if (TestWaterBelow() == 1)
        {
          func_02012694(0x124, (const ::Vector3 *)&mCamSpacePosX);
          func_02022a4c(mPosX, mFloorY + 0x3000, mPosZ);
        }
        else
        {
          func_02012694(0x123, (const ::Vector3 *)&mCamSpacePosX);
        }
      }
    }

    if (TestWaterBelow() == 1)
    {
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
        mDustParticle = func_02022cbc(mDustParticle, 0xe8, *((volatile s32 *)(&pos.x)), *(&pos.y), z, 0);
      }
    }

    _ZN7PathPtrC1Ev(pathptr);
    ((PathPtr *)pathptr)->FromID(mPathId);
    ((PathPtr *)pathptr)->GetNode(*(::Vector3 *)&node, mPathNodeIndex);
    ang = Vec3_HorzAngle((struct Vector3 *)&mPosX, &node);
    ApproachAngle(&mPrevAngleY, ang, 1, 0x1000, 0x1000);
    idx = mPathNodeIndex + mPathDir;
    if (idx < 0)
    {
      idx = mNumPathNodes - 1;
    }
    if (idx >= mNumPathNodes)
    {
      idx = 0;
    }
    ((PathPtr *)pathptr)->GetNode(*(::Vector3 *)&node2, (u32)idx);
    lim = 0x26000;
    if (mRabbitId == 7)
    {
      lim = lim >> 1;
    }
    {
      s32 y = mPosY;
      node.y = y;
      Vec3_Sub(&delta, (struct Vector3 *)&mPosX, &node);
      len = LenVec3(&delta);
    }
    if ((len == 0) || (len <= lim))
    {
      mPosX = node.x;
      mPosY = node.y;
      mPosZ = node.z;
      {
        s32 *p = &mPathNodeIndex;
        *p = (*p) + mPathDir;
      }
      if (mPathNodeIndex >= mNumPathNodes)
      {
        mPathNodeIndex = 0;
      }
      if (mPathNodeIndex < 0)
      {
        mPathNodeIndex = mNumPathNodes - 1;
      }
    }
    else
    {
      ang = Vec3_HorzAngle((struct Vector3 *)&mPosX, &node);
      if (AngleDiff(ang, mPrevAngleY) < 0x2000)
      {
        ysave = mPosY;
        {
          int s = _ZN4cstd4fdivEii(lim, len);
          Vec3_MulScalar(&scaled, &delta, s);
          SubVec3((struct Vector3 *)&mPosX, &scaled, (struct Vector3 *)&mPosX);
        }
        if (mVertSpeed > 0)
        {
          mPosY = ysave;
        }
        if (mActionStep == 0)
        {
          if (mWithMeshClsn.IsOnWall() != 0)
          {
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
   types.h Vector3's empty destructor is free in this function. */
int daMip_c::StateFleeInit()
{
  extern void *data_ov085_021305d0[];

  char pathptr[8];
  int indices[2];
  struct Vector3 v;
  struct Vector3 nodes[2];
  int i;
  int d0;
  struct Vector3 *src;

  Player *p = ClosestPlayer();
  mActionStep = 0;
  if (p)
  {
    src = (struct Vector3 *)&p->mPosX;
    v = *src;
    _ZN7PathPtrC1Ev(pathptr);
    ((PathPtr *)pathptr)->FromID(mPathId);

    indices[0] = mPathNodeIndex - 1;
    if (mPathNodeIndex - 1 < 0)
      indices[0] = mNumPathNodes - 1;
    indices[1] = mPathNodeIndex + 1;
    if (mPathNodeIndex + 1 >= mNumPathNodes)
      indices[1] = 0;

    for (i = 0; i < 2; i++)
      ((PathPtr *)pathptr)->GetNode(nodes[i], indices[i]);

    mPathDir = 1;
    d0 = Vec3_Dist(&v, &nodes[0]);
    if (d0 > Vec3_Dist(&v, &nodes[1]))
      mPathDir = -1;
  }

  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, data_ov085_021305d0[1], 0, 0x1000, 0);
  return 1;
}

// @symbol _ZN7daMip_c16StateStartleMainEv
int daMip_c::StateStartleMain()
{
    extern int data_ov085_0213067c[];

    if (((Animation *)((char *)this + 0x350))->Finished() != 0)
        SetState(data_ov085_0213067c);
    return 1;
}

// @symbol _ZN7daMip_c16StateStartleInitEv
int daMip_c::StateStartleInit()
{
    extern int *data_ov085_021305c8[];

    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, data_ov085_021305c8[1], 0x40000000, 0x1000, 0);
    return 1;
}

// @symbol _ZN7daMip_c13StateIdleMainEv
/* Startle when the closest player comes inside 0x3e8000 from below.
   Otherwise cycle the idle and drift facing around the spot StateIdleInit
   stored. mStateTimer and mTargetAngY stay addressed from this: a named
   store in this switch does not match. */
int daMip_c::StateIdleMain()
{
    extern int data_0209e650[];
    extern void *data_ov085_0213066c;
    extern void *data_ov085_021305d0[];
    extern void *data_ov085_021305b0[];
    extern void *data_ov085_021305c0[];
    extern void *data_ov085_021305c8[];
    char *c = (char *)this;

    char *player = (char *)ClosestPlayer();
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
    Vector3* ppp = (Vector3*)(player + 0x5c);
    pp.x = ppp->x;
    pp.y = ppp->y;
    pp.z = ppp->z;
    int lim = 0x3e8000;
    if (mRabbitId == 7) lim = 0x2ee000;

    if (Vec3_HorzDist((Vector3*)(c + 0x5c), &pp) < lim) {
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
        if (Vec3_HorzDist((Vector3*)(c + 0x5c), &pp) < 0x4b0000) {
            mTargetAngY = Vec3_HorzAngle((Vector3*)(c + 0x5c), &pp) + 0x8000;
            *(s16*)(c + 0x100) = 0x1e;
        }
    }

    if (((Animation *)(c + 0x350))->Finished() != 0) {
        switch (mActionStep) {
        case 1:
            mHorzSpeed = 0x4000;
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300, data_ov085_021305d0[1], 0x40000000, 0x1000, 0);
            (mActionStep)++;
            break;
        case 2:
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300, data_ov085_021305b0[1], 0x40000000, 0x1000, 0);
            mHorzSpeed = 0;
            (mActionStep)++;
            break;
        case 3:
            *(s16*)(c + 0x100) = (r & 0x1f) + 0x1e;
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300, data_ov085_021305c0[1], 0, 0x1000, 0);
            mActionStep = 0;
            break;
        }
    }

    if (mActionStep == 0 && *(u16*)(c + 0x100) == 0) {
        mTargetAngY = Vec3_HorzAngle((Vector3*)(c + 0x5c), (Vector3*)(c + 0x42c));
        s16* ang = (s16*)(c + 0x424);
        *ang = *ang + (0x1800 - ((r & 3) << 12));
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300, data_ov085_021305c8[1], 0x40000000, 0x1000, 0);
        mActionStep = 1;
    }

    ApproachAngle((short*)(c + 0x94), mTargetAngY, 1, 0x500, 0x500);
    return 1;
}

// @symbol _ZN7daMip_c13StateIdleInitEv
int daMip_c::StateIdleInit()
{
  extern char data_ov085_021305c0;

  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, *(void**)((char*)&data_ov085_021305c0+4), 0, 0x1000, 0);
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
    Vec3_Asr(&t, (struct Vector3*)&mPosX, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, t.x, t.y, t.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, mAngleX, mAngleY, mAngleZ);
    *(struct Mtx43 *)&mModelAnim.mat4x3 = data_020a0e68;
    MulMat4x3Mat4x3(mModelAnim.data.transforms, &mModelAnim.mat4x3, tmp);
    Matrix4x3_FromTranslation(&data_020a0e68,
        mPosX >> 3,
        (mPosY - 0xe000) >> 3,
        mPosZ >> 3);
    *(struct Mtx43 *)mShadowMtx = data_020a0e68;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        this, &mShadowModel1, mShadowMtx, 0x46000, 0x258000, 0xf);
}

// @symbol _ZN7daMip_c19UpdateCarriedMatrixEv
/* While carried, the matrix comes from UpdateCarry plus one of four hold
   offsets at data_ov085_021306ec. Sliding and param1 == 2 pick the slot.
   The carrier's +0xc8 word is unnamed on Player. */
void daMip_c::UpdateCarriedMatrix()
{
    struct Mtx43 { s32 m[12]; };
    extern char data_ov085_021306ec[];
    extern int data_020a0e68[];

    int idx;
    void* res;
    if (!mTalkingPlayer) return;
    if (!*(int *)((char *)mTalkingPlayer + 0xc8)) return;
    idx = 0;
    if (mTalkingPlayer->IsFrontSliding() || mTalkingPlayer->LostGrabbedObject()) {
        idx = 1;
    }
    if (*(int *)((char *)mTalkingPlayer + 8) == 2) {
        idx = (idx + 2) & 0xff;
    }
    res = UpdateCarry(*mTalkingPlayer, *(Vector3 *)(data_ov085_021306ec + idx * 0xc));
    *(struct Mtx43 *)&mModelAnim.mat4x3 = *(struct Mtx43 *)res;
    Matrix4x3_FromTranslation(data_020a0e68, mPosX >> 3, (mPosY - 0xc000) >> 3, mPosZ >> 3);
    *(struct Mtx43 *)mShadowMtx = *(struct Mtx43 *)data_020a0e68;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        this, &mShadowModel1, mShadowMtx, 0x46000, 0x258000, 0xf);
}

// @symbol _ZN7daMip_c18UpdateMirrorShadowEv
/* Second shadow, on the far side of the mirror plane at x = 0x1086000.
   The matrix is the 0x30 bytes at this+0x3e8, in the pad after mShadowModel2.
   A named member store does not match; the overlay keeps the block move. */
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
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(c, &c->shadowmodel, &c->mtx, 0x46000, 0x258000, 0xf);
}

// @symbol _ZN7daMip_c16CleanupResourcesEv
/* Seven releases, straight-line. Not address order, so not a table walk.
   data_ov085_021305d8 is the handle the key actor releases too. */
int daMip_c::CleanupResources()
{
    extern char data_ov085_021305d8;
    extern char data_ov085_021305b8;
    extern char data_ov085_021305d0;
    extern char data_ov085_021305b0;
    extern char data_ov085_021305c8;
    extern char data_ov085_021305c0;
    extern char data_ov085_021305e0;

    ((SharedFilePtr *)(&data_ov085_021305d8))->Release();
    ((SharedFilePtr *)(&data_ov085_021305b8))->Release();
    ((SharedFilePtr *)(&data_ov085_021305d0))->Release();
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

    {
        int b = (mFlags & 0x40000) != 0;
        if (b) return 1;
    }

    mScaleX = 0x1500;
    mScaleZ = mScaleX;
    mScaleY = mScaleZ;

    {
        /* modelFile, then materials. +0x24 of the BMD is numMaterials.
           +0x20 of each 0x30-byte material is still pad in BMD_Material;
           walking it as BMD_Material:: does not match this loop. */
        int** base = (int**)&mModelAnim.data;
        int* modelData = base[0];
        char* mat = (char*)base[1];
        for (unsigned int i = 0; i < *(unsigned int*)((char*)modelData + 0x24); i++) {
            *(int*)(mat + 0x20) = mMaterialColor;
            mat += 0x30;
        }
    }

    if (data_0209f2f8 == 5 && data_02092120 == 3) {
        RenderMirrorImage();
    }

    mModelAnim.Render((Vector3 *)&mScaleX);
    return 1;
}

// @symbol _ZN7daMip_c17RenderMirrorImageEv
/* Mirrored second copy. 0x340 is mat4x3 + 0x24, the translation X.
   0x421800 is (0x1086000 >> 3) << 1, so the subtract reflects that
   component across UpdateMirrorShadow's plane. Indexing m[9] does not match. */
void daMip_c::RenderMirrorImage()
{
    struct Mtx43 { s32 m[12]; };
    extern int data_020a0e68;
    char *c = (char *)this;

    struct Mtx43 tmp;
    tmp = *(struct Mtx43 *)(c + 0x31c);
    *(int *)(c + 0x340) = 0x421800 - *(int *)(c + 0x340);
    func_0203c178(&data_020a0e68, -0x1000, 0x1000, 0x1000);
    MulMat3x3Mat3x3((void *)(c + 0x31c), &data_020a0e68, (void *)(c + 0x31c));
    func_02016acc((void *)(c + 0x300), 0x80);
    func_02016b24((void *)(c + 0x300), 0x40);
    ((ModelAnim *)((void *)(c + 0x300)))->ModelAnim::Render((const Vector3 *)(c + 0x80));
    mModelAnim.ApplyOpacity(0xff, 0);
    func_02016b24((void *)(c + 0x300), 0x80);
    func_02016acc((void *)(c + 0x300), 0x40);
    *(struct Mtx43 *)(c + 0x31c) = tmp;
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
 * The block before Animation::Advance is the ARM/Itanium pointer-to-member
 * sequence written out -- adjustment word, virtual bit, vtable index or direct
 * address -- run on whatever mState points at. It is kept verbatim because there
 * is no recovered type for the descriptor to call a member through.
 *
 * `((Animation *)((char *)this + 0x350))->Advance()` must offset from THIS.
 * 0x350 is mModelAnim's Animation base at +0x50; `(char *)&mModelAnim + 0x50`
 * costs a word.
 *
 * The `(long long)(int)` round-trip on c+0x42a (mEatenTimer) is measured:
 * replacing it with `c + 0x42a` or `mEatenTimer = mEatenTimer + 1` size-DIFFs
 * Behavior (0x5cc). The same round-trip on c+0x448 is a no-op and was dropped. */
int daMip_c::Behavior()
{
    /* A LOCAL COORDINATE TRIPLE, NOT A Vector3 OBJECT. Vector3 declares a
       destructor (see include/types.h -- the ROM's __cxa_vec_cleanup calls prove the
       type has one), so a Vector3 local would be destroyed at scope exit and this
       function would come out 8 bytes long. The ROM emits no cleanup for either of
       these, which is itself the evidence that they were never Vector3s: they are
       scratch x/y/z the code fills and reads back. */
    typedef volatile struct { Fix12i x, y, z; } Vec3Scratch;

    extern s8 data_0209f2f8;
    extern s8 data_02092120;
    extern int data_0209caa0[];
    extern void* data_0209f33c;
    extern char data_ov085_021305c0;
    extern char data_ov085_0213068c;
    extern char data_ov085_021306ac;
    extern char data_ov085_021306bc;
    extern char data_ov085_021306dc;

    char* c = (char*)this;
    void* r0p;

    if (data_0209f2f8 != 0x32) {
        if (mIsDisabled == 1) {
            if (data_0209caa0[2] & 0x20000)
                mIsDisabled = 0;
            return 1;
        }
    } else {
        r0p = ClosestPlayer();
        if (r0p == 0)
            return 1;
        if (!(data_0209caa0[2] & 0x20000) || mCharacterId != *(u8*)((char*)r0p + 0x6d9)) {
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
        mGlowParticle = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(volatile u32*)&mGlowParticle, 0x10d, pv.x, pv.y, mPosZ, 0, 0);
    }

    {
        u32 temp_r1 = mTalkState;
        if (temp_r1 < 2) {
            int v = (mFlags & 0x40000) ? 1 : 0;
            if (v == 1) {
                void* temp_r4 = *(void**)&mTalkingPlayer;
                if (temp_r4 != 0) {
                    if (temp_r1 == 0) {
                        Vec3Scratch pos;
                        int var_r6;
                        int var_r2;
                        int t;
                        pos.x = mPosX;
                        pos.y = mPosY;
                        pos.z = mPosZ;
                        Message::PrepareTalk();
                        t = mRabbitId;
                        if (t != 7) {
                            if (func_02013890(t, *(s32*)((char*)temp_r4 + 8)) == 0) {
                                _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x27, 0x12, 0x7f, 0x15ccc, 0);
                                if (mRabbitId != 6) { var_r6 = 0x162; var_r2 = 0x11e; }
                                else { var_r6 = 0x160; var_r2 = 0x126; }
                            } else if (mIsGlowing == 0) {
                                _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x26, 0x12, 0x7f, 0x15ccc, 0);
                                if (mRabbitId != 6) { var_r6 = 0x162; var_r2 = 0x122; }
                                else { var_r6 = 0x160; var_r2 = 0x12a; }
                            } else {
                                _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x27, 0x12, 0x7f, 0x15ccc, 0);
                                if (SaveData::NumGlowingRabbitsFound() == 7) { var_r2 = 0x145; var_r6 = 0x160; }
                                else { var_r6 = 0x162; var_r2 = 0x144; }
                            }
                        } else {
                            _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x27, 0x12, 0x7f, 0x15ccc, 0);
                            var_r2 = 0x13b; var_r6 = 0x160;
                        }
                        pos.y += 0x64000;
                        if (((Player *)temp_r4)->ShowMessage(*(fBase_c *)c, var_r2, 0, 0, 0) == 1) {
                            func_02012694(var_r6, (const ::Vector3 *)(&mCamSpacePosX));
                            mTalkState = 1;
                        }
                    } else if (temp_r1 == 1 && ((Player *)temp_r4)->GetTalkState() == -1) {
                        ((Player *)temp_r4)->DropActor();
                        mTalkState = 2;
                        *(u16*)((char*)temp_r4 + 0x6ce) |= 0x800;
                    }
                }
            }
        }
    }

    if (UpdateYoshiEat(mWithMeshClsn) != 0) {
        mdCcAc_c.Clear();
        if (mEatenByYoshi != 0) {
            if (unk_104 == 5)
                mHorzSpeed = 0;
            if (unk_104 == 0)
                mdCcAc_c.Update();
        }
        if (mEatenByYoshi == 1) {
            *(u8*)((long long)(int)(c + 0x42a)) = *(u8*)((long long)(int)(c + 0x42a)) + 1;
            if (mEatenTimer > 0x96) {
                mEatenByYoshi = 0;
                mEatenTimer = 0;
            }
        }
        UpdateMatrixAndShadow();
        unk_426 = 2;
        SetState(&data_ov085_021306ac);
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, *(void**)((char*)&data_ov085_021305c0 + 4), 0, 0x1000, 0);
        if (*(void**)&mTalkingPlayer == 0)
            *(void**)&mTalkingPlayer = ClosestPlayer();
        return 1;
    }

    if (unk_426 != 0) {
        void* p = *(void**)&mState;
        if (p != (void*)&data_ov085_021306ac && p != (void*)&data_ov085_0213068c &&
            p != (void*)&data_ov085_021306bc && p != (void*)&data_ov085_021306dc) {
            SetState(&data_ov085_021306ac);
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, *(void**)((char*)&data_ov085_021305c0 + 4), 0, 0x1000, 0);
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

    ((Animation *)(c + 0x350))->Advance();

    {
        int v = (mFlags & 0x4000) ? 1 : 0;
        if (v != 0)
            UpdateCarriedMatrix();
        else
            UpdateMatrixAndShadow();
    }

    if (data_0209f2f8 == 5 && data_02092120 == 3)
        UpdateMirrorShadow();

    {
        int v = (mFlags & 0x4000) ? 1 : 0;
        if (v == 0) {
            if (*(void**)&mState != (void*)&data_ov085_021306ac || unk_426 != 0)
                UpdatePos(&mdCcAc_c);
            mAngleX = mPrevAngleX;
            mAngleY = mPrevAngleY;
            mAngleZ = mPrevAngleZ;
            UpdateWMClsn(mWithMeshClsn, 0);
            if (*(void**)&mState != (void*)&data_ov085_021306ac)
                UpdateGrab();
        }
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
    extern char data_ov085_021305b8;
    extern char data_ov085_021305d0;
    extern char data_ov085_021305b0;
    extern char data_ov085_021305c8;
    extern char data_ov085_021305c0;
    extern char data_ov085_021305d8;
    extern char data_ov085_021305e0;
    extern char data_ov085_021306cc;
    extern s32 data_ov085_021305ac;
    extern int data_0209caa0[];
    extern s8 data_0209f2f8;
    extern int data_0209e650;

    void* player;
    void* closest;
    int rabbitId;

    Animation::LoadFile(*(SharedFilePtr *)&data_ov085_021305b8);
    Animation::LoadFile(*(SharedFilePtr *)&data_ov085_021305d0);
    Animation::LoadFile(*(SharedFilePtr *)&data_ov085_021305b0);
    Animation::LoadFile(*(SharedFilePtr *)&data_ov085_021305c8);
    Animation::LoadFile(*(SharedFilePtr *)&data_ov085_021305c0);
    Model::LoadFile(*(SharedFilePtr *)&data_ov085_021305d8);
    mModelAnim.SetFile((BMD_File *)Model::LoadFile(*(SharedFilePtr *)&data_ov085_021305e0), 1, -1);
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

    if (rabbitId == 5 && mCharacterId == 0)
        goto check18;
    if (rabbitId == 1 && mCharacterId == 1)
        goto check18;
    if (rabbitId != 6)
        goto skip17;
    if (mCharacterId != 3)
        goto skip17;

check18:
    if (!(data_0209caa0[2] & 0x80000))
        return 0;

skip17:
    mVertAccel = -0x1000;
    mTerminalVelocity = -0x1e000;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, ((char*)this), 0x50000, 0x64000, 0xb00004, 0x9000);
    mTalkingPlayer = 0;
    mModelAnim.speed = 0x1000;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, ((char*)this), 0x28000, 0x28000, 0, 0);
    {
        PathPtr path;
        path.FromID(mPathId);
        mPathNodeIndex = 1;
        path.GetNode(*(Vector3 *)&mPosX, mPathNodeIndex);
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
        goto block_26;
    }

    player = ClosestPlayer();
    if (player == 0)
        return 0;
    if (data_0209f2f8 != 0x32) {
        if (mCharacterId != *(u8*)((char*)player + 0x6d9))
            return 0;
    }

block_26:
    mColorVariant = mCharacterId + 1;
    if (mCharacterId == 3)
        mColorVariant = 0;

    if (mRabbitId == 7) {
        mColorVariant = 0;
        goto block_out;
    }

    closest = ClosestPlayer();
    if (closest == 0)
        goto block_out;

    {
        int v;
        v = *(s32*)((char*)closest + 8);
        if (data_0209f2f8 == 0x32)
            v = 1;
        if (func_02013890(mRabbitId, v) != 0 && data_ov085_021305ac < 8) {
            u32 rnd = RandomIntInternal(&data_0209e650) >> 8;
            if (NumStars() >= 0x51) {
                if (*(s32*)((char*)closest + 8) == 3) {
                    if ((rnd & 0xf) == 0)
                        mColorVariant = 5;
                } else {
                    if ((rnd & 7) == 0)
                        mColorVariant = 5;
                }
            } else if (NumStars() >= 0x28) {
                if (*(s32*)((char*)closest + 8) == 3) {
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

block_out:
    if (data_0209f2f8 == 5) {
        if (mAreaId == 3)
            mFlags = 0x8280;
    }
    mMaterialColor = (mColorVariant << 1) + *(s32*)((char*)mModelAnim.data.materials + 0x20);
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
