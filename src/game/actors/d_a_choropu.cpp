//cpp
namespace Particle {
struct Callback;
}
struct Vector3_16f;
/* Monty Mole (daChoropu_c) and the rock it throws (daChoro_Rock_c), ov080.
 * The mole stays underground until the nearest Player is 250..1500 units
 * away and it holds the turn, then emerges, throws, leaps, and dives,
 * passing the turn on. A group's knock-backs (mTimesHit) spawn a 1-Up
 * (actor 0x114) at 7. Behavior indexes data_ov080_02128438 by mState.
 *
 * daChoropu_c _ZTI 0x02127fbc, _ZTS 0x02127fc8, _ZTV 0x021280b0.
 * daChoro_Rock_c _ZTI 0x02127fb0, _ZTS 0x02127fd8, _ZTV 0x0212802c
 * (base dEnemyBase_c). Key functions are the empty out-of-line destructors
 * under defer_codegen off: each emits D1 then D0, D2 deadstripped.
 * .text 0x02123740..0x02124a20. The two classes are interleaved in ROM
 * order. Factories daChoro_Rock_c_classInit and daChoropu_c_classInit
 * are `return new`.
 */
// leftovers:
// - func_ov080_02124418 stays free. Its first argument is a daChoro_Rock_c,
//   so it is not a daChoropu_c method.
// - Player::Hurt and Player::Bounce retain scalar mangled declarations for
//   their by-value Fix12 parameters. Throw's spawn id stays
//   `py ? 0x137 : 0x137`.
// - LAUND() in func_ov080_02124088 keeps the cylinder flags, mFlags and
//   mTimesHit addresses opaque.
// - Throw reads mAngleX/Y/Z as unsigned short. Hidden reads mHasTurn as
//   signed char.
// - The model and animation handles take SharedFilePtr's fileID, numRefs
//   and filePtr. File ids: mole model
//   0x2d1, rock model 0x2d6, animations 0x2d2..0x2d5.
// - SetAnim's flag word is 0 in InitResources and 0x40000000 in the state
//   handlers. Collision words 0x200000 (mole) and 0x200004 (rock), and
//   Player::Hurt's (2, 0xc000, 1, 0, 1) / (1, 0xc000, 1, 0, 1), are still
//   literals. Sound ids 0xd2, 0xd5, 0x116, 0x1d and particles 0x29, 0x2a,
//   0x2b are named only by when they fire.
// - pad_0d0 is untouched. data_ov080_0212767c and data_ov080_021276c4
//   (leap and emerge cylinder heights) are read as int arrays. The
//   hit-flag mask 0x66fe0 stays a literal.

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daChoropu_c_AssetId {
    ASSET_CHOROPU_BMD = 0x02d1,
    ASSET_CHOROPU_END_BCA = 0x02d2,
    ASSET_CHOROPU_SEARCH_BCA = 0x02d3,
    ASSET_CHOROPU_START_BCA = 0x02d4,
    ASSET_CHOROPU_THROW_BCA = 0x02d5,
    ASSET_ROCK_BMD = 0x02d6
};

#include "decl_common.h"
#include "daChoropu_c.h"
#include "daChoro_Rock_c.h"
#include "decl_Player.h"
#include "SharedFilePtr.h"
#include "dCc_c.h"
#include "Player.h"
#include "Sound.h"

/* daChoropu_c's state table: one pointer-to-member per state, indexed by
 * the state number at +0x17c. */
typedef void (daChoropu_c::*ChoropuStateFn)();
struct ChoropuStateRow { ChoropuStateFn fn[1]; };

/* Static-init ownership (was the handwritten __sinit_ov080_021278c0 shard).
 * The file-scope objects at the end of this file construct the six
 * resource handles (model files 0x2d1, 0x2d6, animation files 0x2d2, 0x2d3,
 * 0x2d4, 0x2d5) and fill the six one-entry state rows. mwcc emits
 * __sinit_d_a_choropu.cpp from those definitions. */
struct ChoropuModelFile : SharedFilePtr {
    ChoropuModelFile(u32 fileID);
    ~ChoropuModelFile();
};

struct ChoropuAnimationFilePtr : SharedFilePtr {
    ChoropuAnimationFilePtr(u32 fileID);
    ~ChoropuAnimationFilePtr();
};

extern ChoropuModelFile data_ov080_021283c0;
extern ChoropuModelFile data_ov080_021283c8;
extern ChoropuAnimationFilePtr data_ov080_021283e8;
extern ChoropuAnimationFilePtr data_ov080_021283d0;
extern ChoropuAnimationFilePtr data_ov080_021283d8;
extern ChoropuAnimationFilePtr data_ov080_021283e0;
extern ChoropuStateRow data_ov080_02128438[6];

#define LAUND(p) ((void*)((((long long)(int)(p)))))

extern "C" {
namespace Particle {
struct System {
    static void NewSimple(unsigned int id, int x, int y, int z);

    static int New(unsigned, unsigned, int, int, int, const ::Vector3_16f *, Callback *);
};
}
unsigned int d_a_choropuNewHold(unsigned int a, unsigned int b, int x, int y, int z, void *vel,
                                void *cb);
/* Parameter names keep this TU's @NNN. */
typedef int BgchInitAt_(void *self, void *actor, int radius, int height, Vector3_16 *a,
                        Vector3_16 *b);
int   RandomIntInternal(int *seed);
short Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
int   Vec3_HorzDist(const Vector3 *a, const Vector3 *b);
void  Matrix4x3_FromRotationY(void *m, int ang);
void  func_02012694(int a, void *p);

void  func_ov080_02124418(daChoro_Rock_c *t);
}

extern int data_0209e650;
extern s16 data_02082214[];
extern int data_ov080_0212767c[];
extern SharedFilePtr data_ov002_0210d9d8;
extern SharedFilePtr *data_ov080_0212766c[];

#pragma defer_codegen off

// @symbol _ZN11daChoropu_cD1Ev
// @symbol _ZN11daChoropu_cD0Ev
daChoropu_c::~daChoropu_c()
{
}

// @symbol _ZN14daChoro_Rock_cD1Ev
// @symbol _ZN14daChoro_Rock_cD0Ev
daChoro_Rock_c::~daChoro_Rock_c()
{
}

// @symbol _ZN11daChoropu_c16OnAimedAtWithEggEv
/* Returns the constant 0x28000 (40.0 read as a Fix12). What the caller does with it is
 * not recovered here. */
s32 daChoropu_c::OnAimedAtWithEgg()
{
    return 0x28000;
}

// @symbol _ZN11daChoropu_c19func_ov080_02123860Ev
/* State 5 (Leap) update handler (table row 5, data_ov080_02127f90).
 *
 * dExtFrameCtrl_c file 0x2d2 (data_ov080_021283e8). The collision cylinder's height follows
 * a table of whole units indexed by frame (data_ov080_0212767c, 18 words: 90, 78,
 * 62, 116, 176, 228, 252, 264, 288, 288, 288, 248, 208, 128, 32, 0, 0, 0; frames 15 to 17
 * read 0, and the handler does not bound the index). From
 * frame 15 on the cylinder is disabled and the mFlags bit is cleared; on frame 15
 * exactly it also spawns particle effects 0x2a at the mole's position and 0x2b
 * 30 units (0x1e000) above it. When the animation finishes the mole is back in
 * state 1 (Hidden) and hands its turn on. */
void daChoropu_c::func_ov080_02123860()
{
    unsigned int idx;

    mAnmModel.Advance();
    idx = (unsigned int)(mAnmModel.currFrame << 4) >> 0x10; /* whole frame number */
    if (idx >= 0xf) {
        mdCcAc_c.flags |= daChoropu_CC_DISABLED;
        mFlags &= ~daChoropu_FLAG_EMERGED;
        if (idx == 0xf) {
            Particle::System::NewSimple(0x2a, mPosX, mPosY, mPosZ);
            Particle::System::NewSimple(0x2b, mPosX, mPosY + 0x1e000, mPosZ);
        }
    }
    {
        int raw = mAnmModel.currFrame;
        int *tbl = (int *)(int)(data_ov080_0212767c);
        idx = (unsigned int)(raw << 4) >> 0x10;
        mdCcAc_c.height = tbl[idx] << 0xc;
    }
    if (mAnmModel.Finished() == 0)
        return;
    mState = daChoropu_ST_HIDDEN;
    func_ov080_02124360();
}

// @symbol _ZN11daChoropu_c19func_ov080_02123924Ev
/* State 4 (Wait) update handler (table row 4, data_ov080_02127f98).
 *
 * dExtFrameCtrl_c file 0x2d3 (data_ov080_021283d0). Plays the animation out and then
 * goes to state 5 (Leap); it goes there early when the nearest Player is within a
 * quarter turn (< 0x4000 = 90 degrees) of the way the mole faces (mAngleY) and
 * less than 500 units (0x1f4000) away horizontally. With no Player it just waits for the
 * animation to end. */
void daChoropu_c::func_ov080_02123924()
{
    mAnmModel.Advance();
    if (mAnmModel.Finished()) {
        mState = daChoropu_ST_LEAP;
        mAnmModel.SetAnim((BCA_File *)data_ov080_021283e8.filePtr, 0x40000000, 0x1000, 0);
        return;
    }
    {
        Player *p = ClosestPlayer();
        Vector3 v;
        Fix12i *q;
        if (p == 0) return;
        q = (Fix12i *)(((int)&p->mPosX));
        v.x = q[0];
        v.y = q[1];
        v.z = q[2];
        if ((short)AngleDiff(mAngleY, Vec3_HorzAngle(&mPos, &v)) >= 0x4000)
            return;
        if (Vec3_HorzDist(&mPos, &v) >= 0x1f4000)
            return;
        mState = daChoropu_ST_LEAP;
        mAnmModel.SetAnim((BCA_File *)data_ov080_021283e8.filePtr, 0x40000000, 0x1000, 0);
    }
}

// @symbol _ZN11daChoropu_c19func_ov080_02123a34Ev
/* State 3 (ThrowRock) update handler (table row 3, data_ov080_02127f80).
 *
 * dExtFrameCtrl_c file 0x2d5 (data_ov080_021283e0). When the animation steps across
 * frame 10 it spawns a daChoro_Rock_c (actor 0x137, spawn parameter 0 = the big
 * rock) 80 units out from the mole along (mAngleY - 0x4000), a quarter turn off the way it
 * faces, looked up in the
 * sine/cosine pair table data_02082214 (entries are Fix12, indexed by angle >> 4),
 * 10 units (0xa000) above its position, with the angle triple (mAngleX, mAngleY +
 * 0x400, mAngleZ) and the mole's area. The rock is then given horizontal speed
 * 30 units/frame (0x1e000) and vertical speed 4 units (0x4000) plus 4% of the
 * distance to the nearest Player (that distance capped at 600 units = 0x258000),
 * and the sound effect 0xd2 plays at the mole.
 * When the animation finishes: Player closer than 1000 units (0x3e8000) -> state 5
 * (Leap), otherwise state 4 (Wait). */
void daChoropu_c::func_ov080_02123a34()
{
    mAnmModel.Advance();
    if (mAnmModel.WillHitFrame(10)) {
        Vector3_16 v16;
        Vector3 pos;
        unsigned short ax, ay;
        dActor_c *a;
        int d;
        short s;
        int idx;
        int px, py, pz;
        Vector3_16 *pAng = &v16;

        ax = *(unsigned short *)&mAngleX;
        ay = *(unsigned short *)&mAngleY;
        *(volatile short *)&v16.y = (short)ay;
        *(volatile short *)&v16.x = (short)ax;
        {
            unsigned short z = *(unsigned short *)&mAngleZ;
            pAng->z = z;
            short yv = (short)v16.y;
            px = mPosX;
            pos.x = px;
            yv = (short)(yv + 0x400);
            py = mPosY;
            pos.y = py;
            pz = mPosZ;
            pAng->y = yv;
            pos.z = pz;
            pos.y = py + 0xa000;
        }

        idx = (unsigned short)(short)(mAngleY - 0x4000) >> 4;
        s = data_02082214[idx << 1];
        pos.x = s * (short)0x50 + px;

        idx = (unsigned short)(short)(mAngleY - 0x4000) >> 4;
        s = data_02082214[(idx << 1) + 1];
        pos.z = s * (short)0x50 + pz;

        a = dActor_c::Spawn(
            py ? (unsigned)daChoro_Rock_ACTOR_ID : (unsigned)daChoro_Rock_ACTOR_ID, 0, pos, pAng, mAreaId, -1);
        d = DistToCPlayer();
        if (d >= 0x258000)
            d = 0x258000;
        a->mHorzSpeed = 0x1e000;
        a->mSpeedX = 0;
        a->mVertSpeed = (d << 2) / 100 + 0x4000;
        a->mSpeedZ = 0;
        func_0201267c(0xd2, &mCamSpacePos);
    }

    if (mAnmModel.Finished() == 0)
        return;

    if (DistToCPlayer() < 0x3e8000) {
        mState = daChoropu_ST_LEAP;
        mAnmModel.SetAnim((BCA_File *)data_ov080_021283e8.filePtr, 0x40000000, 0x1000, 0);
    } else {
        mState = daChoropu_ST_WAIT;
        mAnmModel.SetAnim((BCA_File *)data_ov080_021283d0.filePtr, 0x40000000, 0x1000, 0);
    }
}

// @symbol _ZN11daChoropu_c19func_ov080_02123c24Ev
/* State 2 (Emerge) update handler (table row 2, data_ov080_02127fa8).
 *
 * dExtFrameCtrl_c file 0x2d4 (data_ov080_021283d8). The collision cylinder's height is 0
 * for frames 0..5; the cylinder is already enabled and the mFlags bit already set from
 * the start of Emerge (func_ov080_02123ecc does both), and frame 6 does both again. From
 * frame 6 through 25 the height follows data_ov080_021276c4 (24, 32, 40, 48, 56, 68,
 * 76, 80, 88, 94, 100, 100 x8, 90, 80 whole units, indexed by frame - 6) and particle
 * effect 0x29 is refreshed at the mole's position every frame (its ID is kept in
 * mEmergeSystemID); from frame 26 on the height is 80 units (0x50000).
 * When the animation finishes, the effect ID is cleared and the next state is picked:
 *   - mGroupMode == 2: random, one in three each: state 3 (ThrowRock), 4 (Wait),
 *     5 (Leap);
 *   - otherwise state 3 when there is no Player or the Player is within a quarter turn
 *     (< 0x4000) of the way the mole faces, state 4 (Wait) when it is behind. */
void daChoropu_c::func_ov080_02123c24()
{
    int amt;
    unsigned int state;
    int raw;

    mAnmModel.Advance();
    raw = mAnmModel.currFrame;
    amt = 0;
    state = ((unsigned int)raw << 4) >> 16;   /* whole frame number */

    if (state == 6) {
        u32 *p150 = (u32 *)(((int)&mdCcAc_c.flags));
        u32 *pb0 = (u32 *)(((int)&mFlags));
        *p150 = *p150 & ~daChoropu_CC_DISABLED;
        *pb0 = *pb0 | daChoropu_FLAG_EMERGED;
    }

    if (state >= 6) {
        if (state >= 0x1a) {
            amt = 0x50000;
        } else {
            amt = data_ov080_021276c4[state - 6] << 12;
            mEmergeSystemID =
                Particle::System::New(mEmergeSystemID, 0x29, mPosX, mPosY, mPosZ, 0, 0);
        }
    }

    mdCcAc_c.height = amt;

    if (mAnmModel.Finished() == 0)
        return;

    mEmergeSystemID = 0;

    if (mGroupMode == daChoropu_GROUP_RANDOM) {
        unsigned int rv = (unsigned int)RandomIntInternal(&data_0209e650) >> 8;
        unsigned int rem = (rv % 3) & 0xff;
        if (rem == 0) {
            mState = daChoropu_ST_THROW_ROCK;
            mAnmModel.SetAnim((BCA_File *)data_ov080_021283e0.filePtr, 0x40000000, 0x1000, 0);
            return;
        }
        if (rem == 1) {
            mState = daChoropu_ST_WAIT;
            mAnmModel.SetAnim((BCA_File *)data_ov080_021283d0.filePtr, 0x40000000, 0x1000, 0);
            return;
        }
        mState = daChoropu_ST_LEAP;
        mAnmModel.SetAnim((BCA_File *)data_ov080_021283e8.filePtr, 0x40000000, 0x1000, 0);
        return;
    }

    {
        Player *player = ClosestPlayer();
        if (player == 0) {
            mState = daChoropu_ST_THROW_ROCK;
            mAnmModel.SetAnim((BCA_File *)data_ov080_021283e0.filePtr, 0x40000000, 0x1000, 0);
            return;
        }
        {
            Vector3 pos;
            short horz;
            int diff;
            int *pb = (int *)(((int)&player->mPosX));
            pos.x = pb[0];
            pos.y = pb[1];
            pos.z = pb[2];
            horz = Vec3_HorzAngle(&mPos, &pos);
            diff = (short)AngleDiff(mAngleY, horz);
            if (diff < 0x4000) {
                mState = daChoropu_ST_THROW_ROCK;
                mAnmModel.SetAnim((BCA_File *)data_ov080_021283e0.filePtr, 0x40000000, 0x1000, 0);
            } else {
                mState = daChoropu_ST_WAIT;
                mAnmModel.SetAnim((BCA_File *)data_ov080_021283d0.filePtr, 0x40000000, 0x1000, 0);
            }
        }
    }
}

// @symbol _ZN11daChoropu_c19func_ov080_02123eccEv
/* State 1 (Hidden) update handler (table row 1, data_ov080_02127f88).
 *
 * The mole is underground. Nothing happens unless the nearest Player is within 1500
 * units (0x5dc000) horizontally (no Player counts as exactly 1500, so no) and this
 * mole holds the turn (mHasTurn == 1). Then:
 *   - Player 250 units (0xfa000) or more away: start emerging -- state 2, animation
 *     file 0x2d4, face the Player (mAngleY and mPrevAngleY), enable the collision
 *     cylinder, set the mFlags bit, sound effect 0x116 at the mole;
 *   - Player closer than 250 units: stay hidden and pass the turn on (func_ov080_02124360). */
void daChoropu_c::func_ov080_02123ecc()
{
    Player *p = ClosestPlayer();
    int dist;
    if (p == 0) dist = 0x5dc000;
    else
        dist = Vec3_HorzDist(&mPos, &p->mPos);
    if (dist >= 0x5dc000) return;
    if (*(signed char *)&mHasTurn != 1) return;   /* read signed, as the ROM does (ldrsb) */
    if (dist >= 0xfa000) {
        mState = daChoropu_ST_EMERGE;
        mAnmModel.SetAnim((BCA_File *)data_ov080_021283d8.filePtr, 0x40000000, 0x1000, 0);
        Player *p2 = ClosestPlayer();
        mAngleY = Vec3_HorzAngle(&mPos, &p2->mPos);
        {
            u32 *a = (u32 *)((int)&mdCcAc_c.flags);
            u32 *b = (u32 *)((int)&mFlags);
            mPrevAngleY = mAngleY;
            *a = *a & ~daChoropu_CC_DISABLED;
            *b = *b | daChoropu_FLAG_EMERGED;
            func_0201267c(0x116, &mCamSpacePos);
        }
        return;
    }
    func_ov080_02124360();
}

// @symbol _ZN11daChoropu_c19func_ov080_02123fccEv
/* State 0 (Setup) update handler (table row 0, data_ov080_02127fa0); the state
 * InitResources installs.
 *
 * Disables the collision cylinder and clears the mFlags bit (the mole starts
 * underground). A mole with mGroupMode != 0 then collects the unique IDs of the other
 * daChoropu_c actors with the same mGroupId into mPartnerIDs (at most four,
 * counted in mNumPartners). Either way the next state is 1 (Hidden). */
void daChoropu_c::func_ov080_02123fcc()
{
    {
        u32 *p150 = (u32 *)(((int)&mdCcAc_c.flags));
        u32 *pb0 = (u32 *)(((int)&mFlags));
        *p150 = *p150 | daChoropu_CC_DISABLED;
        *pb0 = *pb0 & ~daChoropu_FLAG_EMERGED;
    }
    if (mGroupMode != 0) {
        dActor_c *a = 0;
        while (1) {
            a = dActor_c::FindWithActorID(daChoropu_ACTOR_ID, a);
            if (a == 0) break;
            if (a != (dActor_c *)this) {
                if (mGroupId == ((daChoropu_c *)a)->mGroupId) {
                    mPartnerIDs[mNumPartners] = ((daChoropu_c *)a)->uniqueID;
                    (mNumPartners)++;
                    if (mNumPartners == 4) break;
                }
            }
        }
        mState = daChoropu_ST_HIDDEN;
        return;
    }
    mState = daChoropu_ST_HIDDEN;
}

// @symbol _ZN11daChoropu_c19func_ov080_02124088Ev
/* The mole has been knocked back underground (called from func_ov080_02124208 on a
 * hit, a stomp, or contact while the Player is Metal or Mega).
 *
 * Back to state 1 (Hidden) with animation file 0x2d4 at frame 0, collision cylinder
 * disabled, mFlags bit cleared, sound effect 0xd5 at the mole, and a dust puff
 * (dActor_c::PoofDustAt) at the mole's position with y raised by its cylinder height
 * minus 80 units (0x50000). The turn is passed on (func_ov080_02124360).
 *
 * Then the group's hits are tallied: acc is this mole's mTimesHit plus that of every
 * partner that can still be found. While acc < 7 this mole's mTimesHit is incremented.
 * When acc == 7 exactly (this is the eighth hit counted) it spawns actor 0x114
 * (ONEUPKINOKO) 100 units (0x64000) above itself and sets mTimesHit to 8; with
 * acc > 7 nothing more happens. */
void daChoropu_c::func_ov080_02124088()
{
    Vector3 v1;
    Vector3 v3;
    Vector3 v2;
    u8 acc;
    int i;
    daChoropu_c *a;

    mState = daChoropu_ST_HIDDEN;
    mAnmModel.SetAnim((BCA_File *)data_ov080_021283d8.filePtr, 0x40000000, 0x1000, 0);

    mAnmModel.currFrame = 0;
    {
        u32 *p150 = (u32 *)LAUND(&mdCcAc_c.flags);
        *p150 |= daChoropu_CC_DISABLED;
    }
    {
        u32 *pb0 = (u32 *)LAUND(&mFlags);
        *pb0 &= ~daChoropu_FLAG_EMERGED;
    }

    func_0201267c(0xd5, &mCamSpacePos);

    v1.x = mPosX;
    {
        int y1 = mPosY;
        v1.y = y1;
        v1.z = mPosZ;
        v1.y = y1 + (mdCcAc_c.height - 0x50000);
    }
    ((int *)&v2)[0] = ((int *)&v1)[0];
    ((int *)&v2)[1] = ((int *)&v1)[1];
    ((int *)&v2)[2] = ((int *)&v1)[2];
    PoofDustAt(v2);

    func_ov080_02124360();

    acc = mTimesHit;
    i = 0;
    while (i < mNumPartners) {
        a = (daChoropu_c *)dActor_c::FindWithID(mPartnerIDs[i]);
        i = i + 1;
        if (a != 0) {
            acc = (u8)(acc + a->mTimesHit);
        }
    }

    if (acc >= 7) {
        if (acc != 7)
            return;

        v3.x = mPosX;
        {
            int y3 = mPosY;
            v3.y = y3;
            v3.z = mPosZ;
            v3.y = y3 + 0x64000;
        }
        dActor_c::Spawn(daChoropu_ACTOR_ONEUPKINOKO, 0, v3, 0, mAreaId, -1);

        mTimesHit = 8;
        return;
    }

    {
        u8 *p184 = (u8 *)LAUND(&mTimesHit);
        (*p184)++;
    }
}

// @symbol _ZN11daChoropu_c19func_ov080_02124208Ev
/* Reacts to what touched the mole's collision cylinder this frame; run after the
 * state handler by Behavior. Does nothing without an otherOwner (mdCcAc_c.otherOwner
 * is the unique ID of whatever hit it) or when that actor cannot be found. With the
 * hit flags (mdCcAc_c.hitFlags):
 *   - any bit of 0x66fe0, the vulnerability mask InitResources registers: the mole is
 *     knocked underground (func_ov080_02124088);
 *   - otherwise nothing unless bit 0x400000 (the Player) is set;
 *   - Player stomped the mole (JumpedOnByPlayer): knocked underground, then
 *     Player::Bounce is called with 0x28000 (40.0 read as a Fix12);
 *   - Player is Vanish (mIsVanish): no effect;
 *   - Player is Metal (mIsMetal): knocked underground;
 *   - hit by a Mega Player (bit 0x10): sound effect 0x1d, IncMegaKillCount, knocked
 *     underground;
 *   - anything else: Player::Hurt, with the mole's position and the arguments
 *     (2, 0xc000, 1, 0, 1), which are not decoded here. */
void daChoropu_c::func_ov080_02124208()
{
    Player *p;
    unsigned int id = mdCcAc_c.otherOwner;

    if (id == 0)
        return;

    p = (Player *)dActor_c::FindWithID(id);
    if (p == 0)
        return;

    if ((mdCcAc_c.hitFlags & 0x66fe0) != 0) {
        func_ov080_02124088();
        return;
    }

    if ((mdCcAc_c.hitFlags & daChoropu_HIT_PLAYER) == 0)
        return;

    if (JumpedOnByPlayer(mdCcAc_c, *p)) {
        func_ov080_02124088();
        p->Bounce(0x28000);
        return;
    }

    if (p->mIsVanish != 0)
        return;

    if (p->mIsMetal != 0) {
        func_ov080_02124088();
        return;
    }

    if ((mdCcAc_c.hitFlags & daChoropu_HIT_MEGA) != 0) {
        func_02012694(0x1d, &mCamSpacePosX);
        p->IncMegaKillCount();
        func_ov080_02124088();
        return;
    }

    {
        Vector3 pos;
        pos.x = mPosX;
        pos.y = mPosY;
        pos.z = mPosZ;
        p->Hurt(pos, 2, 0xc000, 1, 0, 1);
    }
}

// @symbol _ZN11daChoropu_c19func_ov080_02124360Ev
/* Passes the turn to another mole of the group. A mole with mGroupMode == 0 returns
 * 0 and keeps its turn. Otherwise it clears its own mHasTurn and picks partners at
 * random (mPartnerIDs[r % mNumPartners]) until it finds one that still exists and does
 * not hold the turn; that one gets mHasTurn = 1 and is returned. */
void *daChoropu_c::func_ov080_02124360()
{
    unsigned char n = mGroupMode;
    daChoropu_c *obj;
    if (n == 0)
        return (void *)(unsigned int)n;
    mHasTurn = 0;
    for (;;) {
        unsigned int r = ((unsigned int)RandomIntInternal(&data_0209e650)) >> 8;
        unsigned int cnt = mNumPartners;
        unsigned int idx = r % cnt;
        unsigned int id = mPartnerIDs[idx];
        obj = (daChoropu_c *)dActor_c::FindWithID(id);
        if (obj == 0)
            continue;
        if (obj->mHasTurn != 0)
            continue;
        obj->mHasTurn = 1;
        return obj;
    }
}

// @symbol _ZN11daChoropu_c19func_ov080_021243d8Ev
/* Rebuilds the model's matrix (mAnmModel.mat4x3): a rotation about Y by mAngleY,
 * with the translation (words 9..11 of the flat 12-word spelling this TU sees) set to
 * the actor's position >> 3. */
void daChoropu_c::func_ov080_021243d8()
{
    Matrix4x3_FromRotationY(&mAnmModel.mat4x3, mAngleY);
    mAnmModel.mat4x3.m[9] = mPosX >> 3;
    mAnmModel.mat4x3.m[10] = mPosY >> 3;
    mAnmModel.mat4x3.m[11] = mPosZ >> 3;
}

// @symbol func_ov080_02124418
/* Same as func_ov080_021243d8, for the rock's model (mModel.mat4x3). */
extern "C" void func_ov080_02124418(daChoro_Rock_c *t)
{
    Matrix4x3_FromRotationY(&t->mModel.mat4x3, t->mAngleY);
    t->mModel.mat4x3.m[9] = t->mPosX >> 3;
    t->mModel.mat4x3.m[10] = t->mPosY >> 3;
    t->mModel.mat4x3.m[11] = t->mPosZ >> 3;
}

// @symbol _ZN11daChoropu_c16CleanupResourcesEv
/* Releases the shared model and animation files; never touches `this`: the model file
 * shared from ov002 (data_ov002_0210d9d8), the mole model (data_ov080_021283c0 = file
 * 0x2d1), the rock model (021283c8 = 0x2d6), then the four animation slots through
 * data_ov080_0212766c (files 0x2d2, 0x2d3, 0x2d4, 0x2d5). */
s32 daChoropu_c::CleanupResources()
{
    data_ov002_0210d9d8.Release();
    data_ov080_021283c0.Release();
    data_ov080_021283c8.Release();
    int i = 0;
    do {
        data_ov080_0212766c[i]->Release();
        i++;
    } while (i < 4);
    return 1;
}

// @symbol _ZN14daChoro_Rock_c16CleanupResourcesEv
/* Releases the rock model file (data_ov080_021283c8 = file 0x2d6). */
s32 daChoro_Rock_c::CleanupResources()
{
    data_ov080_021283c8.Release();
    return 1;
}

// @symbol _ZN11daChoropu_c6RenderEv
/* Draws the mole's model (mAnmModel) with no scale argument. */
s32 daChoropu_c::Render()
{
    dExtSimpleModel_c *m = &mAnmModel;
    m->Render(0);
    return 1;
}

// @symbol _ZN14daChoro_Rock_c6RenderEv
/* Draws the rock's model, scaled by (mScaleX, mScaleY, mScaleZ). */
s32 daChoro_Rock_c::Render()
{
    dExtSimpleModel_c *m = &mModel;
    m->Render(&mScale);
    return 1;
}

// @symbol _ZN11daChoropu_c8BehaviorEv
/* The mole's per-frame update (vtable slot 6): Luigi-vanish bookkeeping for the
 * cylinder, the current state's update handler through the state table, then
 * func_ov080_02124208 (react to whatever touched the cylinder), func_ov080_021243d8
 * (rebuild the model matrix), and the cylinder's Clear and Update. */
s32 daChoropu_c::Behavior()
{
    MakeVanishLuigiWork(mdCcAc_c);
    (this->*data_ov080_02128438[mState].fn[0])();
    func_ov080_02124208();
    func_ov080_021243d8();
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}

// @symbol _ZN14daChoro_Rock_c8BehaviorEv
/* The rock's per-frame update (vtable slot 6).
 *
 * If the Player touches the rock (otherOwner found, hitFlags bit 0x400000) it is hurt
 * with Player::Hurt (arguments (1, 0xc000, 1, 0, 1), not decoded here -- the mole
 * passes 2 for the first one) when it is Metal, or when it is not Vanish. On the
 * ground (mWithMeshClsn.IsOnGround) a big rock (mIsSmall == 0) first spawns two
 * small rocks (actor 0x137, spawn parameter 1) at its own position, each with a
 * random heading (RandomIntInternal >> 8 as mPrevAngleY), half its horizontal speed
 * and vertical speed 5 units (0x5000); then, big or small, it is marked for
 * destruction. After that: UpdatePos, UpdateWMClsn, the model matrix
 * (func_ov080_02124418), and the cylinder's Clear and Update. */
s32 daChoro_Rock_c::Behavior()
{
    MakeVanishLuigiWork(mdCcAc_c);

    u32 id = mdCcAc_c.otherOwner;
    if (id != 0) {
        Player *player = (Player *)FindWithID(id);
        if (player != 0 && (mdCcAc_c.hitFlags & daChoropu_HIT_PLAYER)) {
            if (player->mIsMetal) {
                Vector3 pos;
                pos.x = mPosX;
                pos.y = mPosY;
                pos.z = mPosZ;
                player->Hurt(pos, 1, 0xc000, 1, 0, 1);
            } else if (!player->mIsVanish) {
                Vector3 pos;
                pos.x = mPosX;
                pos.y = mPosY;
                pos.z = mPosZ;
                player->Hurt(pos, 1, 0xc000, 1, 0, 1);
            }
        }
    }

    if (mWithMeshClsn.IsOnGround()) {
        if (!mIsSmall) {
            /* A big rock breaks into two small ones on landing. */
            dActor_c *s;
            int r;

            s = dActor_c::Spawn(daChoro_Rock_ACTOR_ID, 1, mPos, 0, mAreaId, -1);
            r = RandomIntInternal(&data_0209e650);
            s->mPrevAngleX = 0;
            s->mPrevAngleY = (short)((unsigned int)r >> 8);
            s->mPrevAngleZ = 0;
            s->mHorzSpeed = mHorzSpeed >> 1;
            s->mSpeedX = 0;
            s->mVertSpeed = 0x5000;
            s->mSpeedZ = 0;

            s = dActor_c::Spawn(daChoro_Rock_ACTOR_ID, 1, mPos, 0, mAreaId, -1);
            r = RandomIntInternal(&data_0209e650);
            s->mPrevAngleX = 0;
            s->mPrevAngleY = (short)((unsigned int)r >> 8);
            s->mPrevAngleZ = 0;
            s->mHorzSpeed = mHorzSpeed >> 1;
            s->mSpeedX = 0;
            s->mVertSpeed = 0x5000;
            s->mSpeedZ = 0;
        }
        MarkForDestruction();
    }

    UpdatePos(0);
    UpdateWMClsn(mWithMeshClsn, 0);
    func_ov080_02124418(this);
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}

// @symbol _ZN11daChoropu_c13InitResourcesEv
/* Loads the four animation files through data_ov080_0212766c, the ov002 shared model
 * file, the rock model and the mole model (data_ov080_021283c0, set on mAnmModel with
 * flags 1 and -1). Starts animation file 0x2d4 (data_ov080_021283d8) with flags 0, speed
 * 1.0 (0x1000) from frame 0, and initialises the collision cylinder: radius 80 units
 * (0x50000), height 100 units (0x64000), flags 0x200000 (not decoded here), vulnerability
 * mask 0x66fe0. Then unpacks param1:
 *   bits 0..3  mGroupMode (0 = solo)
 *   bits 4..7  mGroupId
 *   bit  8     initial mHasTurn for a grouped mole (a solo mole always starts with it)
 * and starts in state 0 (Setup) with no hits tallied, no partners and no particle ID. */
s32 daChoropu_c::InitResources()
{
    int i;
    for (i = 0; i < 4; i++) dExtFrameCtrl_c::LoadFile(*data_ov080_0212766c[i]);
    dExtSimpleModel_c::LoadFile(data_ov002_0210d9d8);
    dExtSimpleModel_c::LoadFile(data_ov080_021283c8);
    mAnmModel.SetFile((BMD_File *)dExtSimpleModel_c::LoadFile(data_ov080_021283c0), 1, -1);
    mAnmModel.SetAnim((BCA_File *)data_ov080_021283d8.filePtr, 0, 0x1000, 0);
    mdCcAc_c.Init(this, 0x50000, 0x64000, 0x200000, 0x66fe0);
    mGroupMode = param1 & 0xf;
    mState = daChoropu_ST_SETUP;
    if (mGroupMode == daChoropu_GROUP_SOLO) mHasTurn = 1;
    else mHasTurn = (param1 >> 8) & 1;
    mGroupId = (param1 >> 4) & 0xf;
    mTimesHit = 0;
    mNumPartners = 0;
    for (i = 0; i < 4; i++) mPartnerIDs[i] = 0;
    mEmergeSystemID = 0;
    return 1;
}

// @symbol _ZN14daChoro_Rock_c13InitResourcesEv
/* Fails (returns 0) if the rock model cannot be set. Otherwise: collision cylinder with
 * radius 30 units and height 30 units (0x1e000), flags 0x200004 (not decoded here) and
 * no vulnerability mask; mIsSmall = param1 & 1; the mesh collision object with both size
 * arguments 30 units; gravity -2 units/frame^2 (mVertAccel -0x2000) and terminal fall
 * speed -60 units/frame (-0x3c000); scale 1.0 (0x1000) on all three axes for a big rock,
 * 0.5 (0x800) for a small one. */
s32 daChoro_Rock_c::InitResources()
{
    if (!mModel.SetFile((BMD_File *)dExtSimpleModel_c::LoadFile(data_ov080_021283c8), 1, -1))
        return 0;
    mdCcAc_c.Init(this, 0x1e000, 0x1e000, 0x200004, 0);
    mIsSmall = param1 & 1;
    mWithMeshClsn.Init(this, 0x1e000, 0x1e000, 0, 0);
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    if (mIsSmall == 0) {
        mScaleX = 0x1000;
        mScaleY = 0x1000;
        mScaleZ = 0x1000;
    } else {
        mScaleX = 0x800;
        mScaleY = 0x800;
        mScaleZ = 0x800;
    }
    return 1;
}

// @symbol daChoro_Rock_c_classInit
/* Registry factory for the CHORO_ROCK profile. */
extern "C" daChoro_Rock_c *daChoro_Rock_c_classInit()
{
    return new daChoro_Rock_c();
}

// @symbol daChoropu_c_classInit
/* Registry factory for the CHOROPU profile. */
extern "C" daChoropu_c *daChoropu_c_classInit()
{
    return new daChoropu_c();
}

/* Static-init globals (was the handwritten __sinit_ov080_021278c0 shard).
 * Definition order is the retail initializer's construction order: the two
 * model handles, then the four animation handles, then the six state rows in
 * Setup, Hidden, Emerge, ThrowRock, Wait, Leap order. */
ChoropuModelFile data_ov080_021283c0(ASSET_CHOROPU_BMD);
ChoropuModelFile data_ov080_021283c8(ASSET_ROCK_BMD);
ChoropuAnimationFilePtr data_ov080_021283e8(ASSET_CHOROPU_END_BCA);
ChoropuAnimationFilePtr data_ov080_021283d0(ASSET_CHOROPU_SEARCH_BCA);
ChoropuAnimationFilePtr data_ov080_021283d8(ASSET_CHOROPU_START_BCA);
ChoropuAnimationFilePtr data_ov080_021283e0(ASSET_CHOROPU_THROW_BCA);

ChoropuStateRow data_ov080_02128438[6] = {
    {{&daChoropu_c::func_ov080_02123fcc}},
    {{&daChoropu_c::func_ov080_02123ecc}},
    {{&daChoropu_c::func_ov080_02123c24}},
    {{&daChoropu_c::func_ov080_02123a34}},
    {{&daChoropu_c::func_ov080_02123924}},
    {{&daChoropu_c::func_ov080_02123860}},
};
