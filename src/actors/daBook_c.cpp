//cpp
/* ov020/daBook_c+daBookGen_c -- the haunted books of Big Boo's Haunt
 * (daBook_c) and the generator that fires them (daBookGen_c). 31 functions,
 * .text 0x021111a0..0x02112938: the first function of ov020 .text up to
 * daChair_c (src/actors/daChair_c.cpp), which begins at 0x02112938.
 *
 * One class, three registry profiles, told apart by actorID:
 *   SHOOT_BOOK  (0x145)  a projectile. daBookGen_c spawns it when the player
 *                        comes within range; it flies straight and hurts.
 *   KILLER_BOOK (0x147)  waits on its own until the player is close and in
 *                        front, then winds up, aims and launches itself. It
 *                        drops a blue coin when the player destroys it
 *                        (attack, bump or stomp), not when it crashes into
 *                        the player or the world.
 *   BOOK_SWITCH (0xd5)   a book that waits 40 units behind its home position
 *                        and moves forward to it when hit. Three of them make
 *                        an ordered-push puzzle: each reports to the bookshelf
 *                        (daTrsTrap_c) whose actor ID is in mLinkedActorID,
 *                        and a wrong push makes a SHOOT_BOOK fire at the
 *                        player from beside the bookshelf.
 * mKind picks the state machine (flying: mState 0..5, switch: 6..10).
 *
 * SOURCE ORDER IS ROM ORDER. `defer_codegen off` makes mwccarm emit each
 * function as it is parsed. Each empty out-of-line destructor emits D1 then
 * D0; its D2 has no ROM home and is deadstripped (policy rows in the TU
 * manifest). Defining the destructors here makes this file the key-function
 * home of both vtables and their RTTI. The four registry factories come last,
 * in ROM order, and abut each other with no gap.
 *
 * common.h comes first, and that is load-bearing: it and math/Matrix.h
 * define Matrix4x3 under one guard, and only common.h's flat s32 m[12]
 * spelling makes InitResources' mShadowMat = IDENTITY_MATRIX4X3 the ROM's
 * twelve-word block copy.
 *
 * The state and helper methods keep their ROM-address names
 * (func_ov020_0211xxxx): nothing recovers what EAD called them, and the
 * name is the address. Leftover:
 *  - calls that pass Fix12<int> by value (ModelAnim::SetAnim,
 *    dBgCh_Actr::Init, dCcAcPos_c::Init, Player::Hurt, Player::Bounce,
 *    DropShadowRadHeight, cstd::atan2) stay mangled extern "C" calls: the
 *    headers declare those parameters as Fix12i (a plain s32), so a member
 *    call would mangle to a symbol the ROM does not have;
 *  - GetWallResult has no dBgCh_Actr method, and the ROM calls
 *    dBgCh_Actr_UpdateContinuous_Veneer, not UpdateContinuous;
 *  - data_ov020_02114aa0/ab8 are model handles and aa8/ab0 are animation
 *    handles. words[1] is the loaded file pointer.
 */

#include "common.h"
#include "daBook_c.h"
#include "daBookGen_c.h"
#include "daTrsTrap_c.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "SurfaceInfo.h"
#include "dCc_c.h"
#include "Sound.h"

int ApproachLinear(int &value, int target, int step);
int ApproachLinear(short &value, short target, short step);

/* Actor IDs from the ROM's actor table (symbols/actor_debug_names.tsv). */
enum {
    kPlayerActorId = 0xbf,
    kBookSwitchActorId = 0xd5,
    kBlueCoinActorId = 0x122,
    kShootBookActorId = 0x145,
    kKillerBookActorId = 0x147
};

/* Sound IDs, named by when they play. */
enum {
    kSfxSwitchPush = 0xb5,  /* BOOK_SWITCH starts sliding forward */
    kSfxImpact = 0xc5,      /* a flying book hits ground or wall */
    kSfxTakeOff = 0x166     /* a book starts to move */
};

/* What func_ov020_021115ac found when it looked at the collider. */
enum {
    HIT_NONE = 0,
    HIT_ATTACKED = 1,       /* hit by an attack, or bumped from below */
    HIT_METAL_PLAYER = 2,   /* touched by a metal player */
    HIT_TOUCHED_PLAYER = -1,
    HIT_STOMPED = -2        /* jumped on */
};

/* How fast a launched book travels: 50 units a frame. */
enum { kFlightSpeed = 0x32000 };

/* A destructor-free xyz view; func_ov020_021115ac's copy through a Vector3
 * local does not match. */
typedef struct { int x, y, z; } Vec3;

/* dEnemyBase_c declares mStateTimer as an s16, but every use here reads it as
 * an unsigned halfword (ldrh); a signed read is ldrsh and misses by a word.
 * Member-context pun, valid inside the daBook_c methods only. */
#define STATE_TIMER (*(u16 *)&mStateTimer)

/* Index of an unsigned 16-bit angle into data_02082214, the sin/cos table:
 * one s16 sin, s16 cos pair per 16 angle units. */
#define SINCOS_INDEX(angle) ((*(u16 *)&(angle) >> 4) * 2)

/* A fix12 multiply, rounded to nearest. */
#define FIX12_MUL(a, b) ((s32)(((s64)(a) * (b) + 0x800) >> 12))

extern "C" {
short _ZN4cstd5atan2E5Fix12IiES1_(int, int);
void Vec3_Sub(Vector3 *d, Vector3 *a, Vector3 *b);
int  Vec3_HorzLen(const Vector3 *);
Fix12i Vec3_Dist(const Vector3 *a, const Vector3 *b);
s16  Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
int  Vec3_HorzDist(const Vector3 *a, const Vector3 *b);
void AddVec3(void *a, void *b, void *c_);
int  RandomIntInternal(void *p);
int  _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(Player *self, const Vector3 *pos, unsigned int a, int fix, u8 b, u8 d, u8 e);
void _ZN6Player6BounceE5Fix12IiE(Player *self, int fix);
void func_ov063_0211cae8(void *found, unsigned int mask);
void func_0203568c(int *p, int v);
void func_0201267c(unsigned int id, const Vector3 *pos);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(ModelAnim *, BCA_File *, int, int, u16);
void dBgCh_Actr_UpdateContinuous_Veneer(void *c_);
void *_ZNK10dBgCh_Actr13GetWallResultEv(void *self);
void Matrix4x3_FromRotationZXYExt(void *m, int x, int y, int z);
void _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(dActor_c *thiz, dExtShadowModel_c *sm, Matrix4x3 *mtx, int radius, int depth, u8 opacity);
void UnloadBlueCoinModel(void *);
void LoadBlueCoinModel(void *c);
void func_0200f760(void *a, void *b);
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
    dBgCh_Actr *self, dActor_c *actor, int radius, int height,
    Vector3_16 *first, Vector3_16 *second);
void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
    dCcAcPos_c *self, dActor_c *actor, const Vector3 *offset,
    int radius, int height, u32 flags, u32 vulnFlags);
}

extern int data_0209e650[];     /* the random number generator's state */
extern s16 data_02082214[];     /* sin/cos table, see SINCOS_INDEX */
/* BOOK_SWITCH push order, indexed by the book's param1: the value the
 * partner's low three book bits must have when that book is pushed. {0, 1, 3}
 * is book 0, then book 1, then book 2. */
extern u8 data_ov020_02114828[];
/* Model handles construct through func_02017acc and destroy through
 * func_02017ab4. Animation handles construct through SharedFilePtr::Construct
 * and destroy through SharedFilePtr_Destruct_Anim. The manifest aliases those
 * undefined members onto the ROM symbols. */
struct BookModelFilePtr : SharedFilePtr {
    u32 words[2];

    BookModelFilePtr(u32 fileID);
    ~BookModelFilePtr();
};

struct BookAnimationFileHandle : SharedFilePtr {
    u32 words[2];

    BookAnimationFileHandle(u32 fileID);
    ~BookAnimationFileHandle();
};

/* The four slots, by how they are used below:
 *   aa0  the animated model that mModelAnim switches to (STATE_TILT_BACK)
 *   ab8  the model mModel starts with
 *   aa8  the animation that plays during STATE_WIND_UP
 *   ab0  the animation that plays in flight */
extern "C" BookModelFilePtr data_ov020_02114aa0;
extern "C" BookModelFilePtr data_ov020_02114ab8;
extern "C" BookAnimationFileHandle data_ov020_02114aa8;
extern "C" BookAnimationFileHandle data_ov020_02114ab0;
extern Matrix4x3 IDENTITY_MATRIX4X3;

#pragma defer_codegen off

// @symbol _ZN8daBook_cD1Ev
// @symbol _ZN8daBook_cD0Ev
/* One native destructor definition emits both ROM variants. D1 destroys the
 * class-typed members in reverse declaration order, then dEnemyBase_c; D0 is
 * the same destruction followed by the inherited actor-heap operator delete. */
daBook_c::~daBook_c()
{
}

#ifdef _MSC_VER
/* The host uses the flat ROM D0 name, which MSVC never emits: it folds the
 * Itanium destructor variants into the one ~daBook_c() above. This arm
 * spells out what the deleting destructor does -- the D1 body, called
 * qualified so it is a direct call, then the class-specific operator delete.
 * Nothing here reaches mwccarm. */
extern "C" daBook_c *_ZN8daBook_cD0Ev(daBook_c *thiz)
{
    thiz->daBook_c::~daBook_c();
    daBook_c::operator delete(thiz);
    return thiz;
}
#endif

// @symbol _ZN11daBookGen_cD1Ev
// @symbol _ZN11daBookGen_cD0Ev
daBookGen_c::~daBookGen_c()
{
}

#ifdef _MSC_VER
/* Host D0, as for daBook_c above. */
extern "C" daBookGen_c *_ZN11daBookGen_cD0Ev(daBookGen_c *thiz)
{
    thiz->daBookGen_c::~daBookGen_c();
    daBookGen_c::operator delete(thiz);
    return thiz;
}
#endif

// @symbol _ZN8daBook_c19func_ov020_021112b0Ev
/* Points the book at the closest player: mAimYaw and mAimPitch are the angles
 * from the book to the player, mAimRoll a constant quarter turn. */
void daBook_c::func_ov020_021112b0()
{
    Player *player = ClosestPlayer();
    if (!player)
        return;
    Vector3 *playerPos = (Vector3 *)&player->mPosX;
    Vector3 tmp;
    tmp.x = playerPos->x;
    tmp.y = playerPos->y;
    tmp.z = playerPos->z;
    Vector3 toPlayer;
    Vec3_Sub(&toPlayer, &tmp, (Vector3 *)&mPosX);
    mAimYaw = _ZN4cstd5atan2E5Fix12IiES1_(toPlayer.x, toPlayer.z);
    mAimPitch = _ZN4cstd5atan2E5Fix12IiES1_(toPlayer.y, Vec3_HorzLen(&toPlayer));
    mAimRoll = 0x4000;
}

// @symbol _ZN8daBook_c19func_ov020_02111340Ev
/* BOOK_SWITCH, wrong book pushed: fires a SHOOT_BOOK from beside the partner,
 * 250 units to its -x or +x side (one random bit picks it), 65 units up,
 * at the touching player's depth and turned a quarter turn to cross it. */
void daBook_c::func_ov020_02111340()
{
    int bit = ((unsigned int)RandomIntInternal(data_0209e650) >> 16) & 1;
    if (mTouchedPlayer == 0)
        return;
    {
        dActor_c *partner = dActor_c::FindWithID(mLinkedActorID);
        if (partner == 0)
            return;
        {
            struct Vector3 pos;
            struct Vector3_16 rot;
            int *fp = &partner->mPosX;
            int mul = 0x1f4 * bit;
            int x = fp[0];
            int dx = mul - 0xfa;
            pos.x = x;
            {
                int y = fp[1];
                int nx = x + (dx << 12);
                pos.y = y;
                {
                    int z = fp[2];
                    int ny = y + 0x41000;
                    pos.z = z;
                    pos.x = nx;
                    pos.y = ny;
                }
            }
            pos.z = mTouchedPlayer->mPosZ;
            rot.y = (short)((bit << 15) + 0x4000);
            rot.x = 0;
            rot.z = 0;
            dActor_c::Spawn(kShootBookActorId, 0, pos, &rot, mAreaId, -1);
        }
    }
}

// @symbol _ZN8daBook_c19func_ov020_02111418Ev
/* What a flying book does about whatever func_ov020_021115ac found. Returns 1
 * when the book is gone. The first count handed to Player::Hurt is 0 for a
 * metal player, 2 for KILLER_BOOK and 1 otherwise; touching the player also
 * clears unk_108, so no blue coin drops. */
int daBook_c::func_ov020_02111418()
{
    int r = func_ov020_021115ac();
    if (r == HIT_ATTACKED) { func_ov020_02112110(); return 1; }
    if (r == HIT_METAL_PLAYER) {
        Vector3 v;
        v.x = mPosX; v.y = mPosY; v.z = mPosZ;
        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(mTouchedPlayer, &v, 0, 0xc000, 1, 0, 1);
        func_ov020_02112110();
        return 1;
    }
    if (r == HIT_TOUCHED_PLAYER) {
        int eq = (actorID == kKillerBookActorId);
        if (eq) {
            Vector3 v;
            v.x = mPosX; v.y = mPosY; v.z = mPosZ;
            _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(mTouchedPlayer, &v, 2, 0xc000, 1, 0, 1);
        } else {
            Vector3 v;
            v.x = mPosX; v.y = mPosY; v.z = mPosZ;
            _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(mTouchedPlayer, &v, 1, 0xc000, 1, 0, 1);
        }
        unk_108 = 0;
        func_ov020_02112110();
        return 1;
    }
    if (r != HIT_STOMPED) return 0;
    _ZN6Player6BounceE5Fix12IiE(mTouchedPlayer, 0x28000);
    func_ov020_02112110();
    return 1;
}

// @symbol _ZN8daBook_c19func_ov020_021115acEv
/* Reads the collider: who touched the book (dCc_c otherOwner) and how
 * (hitFlags), and returns one of the HIT_ results. Only a player counts, and
 * a vanished player is ignored by SHOOT_BOOK straight away, and by KILLER_BOOK
 * only when nothing else applied. mTouchedPlayer is set as soon as a player is known to be
 * involved. The ROM reserves a 12-byte frame it never touches; two
 * address-taken volatile locals reserve it with no emitted code. */
int daBook_c::func_ov020_021115ac()
{
    u32 id;
    dActor_c *found;
    int t;
    volatile int t1, t2;
    (void)&t1; (void)&t2;

    id = mdCcAcPos_c.otherOwner;
    if (id == 0)
        return HIT_NONE;
    found = dActor_c::FindWithID(id);
    if (found == 0)
        return HIT_NONE;

    t = (int)(found->actorID == kPlayerActorId);
    if (t == 0)
        return HIT_NONE;

    /* 0x8000 is the Yoshi-tongue hit bit */
    if ((mdCcAcPos_c.hitFlags & 0x8000) != 0)
        return HIT_NONE;

    if (((Player *)found)->mIsVanish != 0) {
        t = (int)(actorID == kShootBookActorId);
        if (t != 0)
            return HIT_NONE;
    }

    mTouchedPlayer = (Player *)found;

    /* the attack bits the profile made the collider vulnerable to */
    if ((mdCcAcPos_c.hitFlags & 0x26fe0) != 0)
        return HIT_ATTACKED;

    if (BumpedUnderneathByPlayer(*(Player *)found) != 0) {
        /* the bump also cancels the player's vertical speed */
        Vec3 *p = (Vec3 *)&found->unk_0a4;
        Vec3 v;
        v.z = p->z;
        v.x = p->x;
        v.y = 0;
        found->unk_0a4 = v.x;
        found->mVertSpeed = v.y;
        found->unk_0ac = v.z;
        mTouchedPlayer = (Player *)found;
        return HIT_ATTACKED;
    }

    if (JumpedOnByPlayer(mdCcAcPos_c, *(Player *)found) != 0) {
        mTouchedPlayer = (Player *)found;
        return HIT_STOMPED;
    }

    if (((Player *)found)->mIsMetal != 0)
        return HIT_METAL_PLAYER;

    if (((Player *)found)->mIsVanish != 0) {
        t = (int)(actorID == kKillerBookActorId);
        if (t != 0)
            return HIT_NONE;
    }
    return HIT_TOUCHED_PLAYER;
}

// @symbol _ZN8daBook_c19func_ov020_0211174cEv
/* The BOOK_SWITCH state machine (mState 6..10): the partner arms the puzzle,
 * the book slides into the shelf, waits to be hit, slides out, reports itself
 * to the partner, then follows the partner's verdict. The partner is the
 * daTrsTrap_c bookshelf: the low three bits of its mBookFlags record which
 * books have been pushed, bit 3 is the puzzle armed, and its mState is the
 * verdict (2 slides the shelf away, 3 removes it). */
void daBook_c::func_ov020_0211174c()
{
    daTrsTrap_c *partner;

    /* looked up twice; the first result is unused */
    dActor_c::FindWithID(mLinkedActorID);
    partner = (daTrsTrap_c *)dActor_c::FindWithID(mLinkedActorID);
    if (partner == 0)
        return;

    switch (mState) {
    case STATE_SWITCH_WAIT:
        if ((partner->mBookFlags & 8) == 0)
            return;
        mState = STATE_SWITCH_RETRACT;
        mStateTimer = 0;
        return;
    case STATE_SWITCH_RETRACT:
    {
        /* waits out mStateTimer, then slides back 4 units a frame until it
         * is 40 units behind its home position */
        u16 *ctr = &STATE_TIMER;
        s32 *z;
        u32 *fl;
        if (*ctr != 0) {
            *ctr -= 1;
            return;
        }
        z = &mPosZ;
        *z -= 0x4000;
        if (mHomePosZ - mPosZ < 0x28000)
            return;
        mPosZ = mHomePosZ - 0x28000;
        fl = &mdCcAcPos_c.flags;
        mState = STATE_SWITCH_READY;
        *fl &= ~1;
        return;
    }
    case STATE_SWITCH_READY:
    {
        /* an attack or a metal-player touch turns the collider on (clears
         * flags bit 0) and starts the push */
        u32 *fl;
        if (func_ov020_021115ac() <= 0)
            return;
        fl = &mdCcAcPos_c.flags;
        mState = STATE_SWITCH_PUSH;
        *fl |= 1;
        Sound::PlayBank0(kSfxSwitchPush, *(Vector3 *)&mCamSpacePosX);
        return;
    }
    case STATE_SWITCH_PUSH:
    {
        /* slides forward 10 units a frame; once home, checks the push order
         * and tells the partner this book was pushed */
        s32 *z = &mPosZ;
        *z += 0xa000;
        if (mPosZ < mHomePosZ)
            return;
        {
            int idx = param1;
            if ((partner->mBookFlags & 7) != data_ov020_02114828[idx])
                func_ov020_02111340();
        }
        mTouchedPlayer = 0;
        func_ov063_0211cae8(partner, (1u << param1) & 0xff);
        mPosZ = mHomePosZ;
        mState = STATE_SWITCH_DONE;
        return;
    }
    case STATE_SWITCH_DONE:
    {
        /* if the partner cleared this book's bit, retract and try again;
         * phase 2 sends the book off along +x until x reaches -1500 units,
         * phase 3 removes it at once */
        s32 *x;
        if ((partner->mBookFlags & (1 << param1)) == 0) {
            mState = STATE_SWITCH_RETRACT;
            mStateTimer = 0xa;
            return;
        }
        if (partner->mState == 2) {
            x = &mPosX;
            *x += 0x5000;
            if (mPosX < -0x5dc000)
                return;
            MarkForDestruction();
            return;
        }
        if (partner->mState != 3)
            return;
        MarkForDestruction();
        return;
    }
    default:
        return;
    }
}

// @symbol _ZN8daBook_c19func_ov020_021119dcEv
/* STATE_YOSHI_SKID: brakes the book to a standstill at 0x800 a frame, with
 * its collider off while unk_104 counts down, then resumes mSavedState (a
 * book that was in flight goes back to winding up). func_0203568c stores its
 * second argument in word 6 of mWithMeshClsn (the radius, as daBgSnmBdy_c
 * reads it): 100 units on every skid frame, back to Init's 50 when the skid
 * ends. */
void daBook_c::func_ov020_021119dc()
{
    func_0203568c((int *)&mWithMeshClsn, 0x64000);
    if (unk_104 != 0) {
        unsigned short *p = &unk_104;
        *p = (unsigned short)(*p - 1);
        if (unk_104 != 0) {
            u32 *q = &mdCcAcPos_c.flags;
            *q = *q | 1;
        } else {
            u32 *q = &mdCcAcPos_c.flags;
            *q = *q & ~1;
        }
    }
    ApproachLinear(mVertSpeed, 0, 0x800);
    ApproachLinear(mHorzSpeed, 0, 0x800);
    if (mVertSpeed == 0 && mHorzSpeed == 0) {
        func_0203568c((int *)&mWithMeshClsn, 0x32000);
        mState = mSavedState;
        if (mState == STATE_FLY)
            mState = STATE_WIND_UP;
    }
    UpdatePos(0);
}

// @symbol _ZN8daBook_c19func_ov020_02111aa8Ev
/* STATE_SPAWNED: a SHOOT_BOOK's first three frames. It never drops a coin;
 * on the third frame it becomes STATE_FLY at full speed with the collider on. */
void daBook_c::func_ov020_02111aa8()
{
    unk_108 = 0;
    STATE_TIMER++;
    if (STATE_TIMER >= 3) {
        int *bf;
        int t;
        int sid;
        mState = STATE_FLY;
        mHorzSpeed = kFlightSpeed;
        bf = (int *)&mdCcAcPos_c.flags;
        t = *bf;
        sid = kSfxTakeOff;
        *bf = t & ~1;
        func_0201267c(sid, (const Vector3 *)&mCamSpacePosX);
    }
    ApproachLinear(mHorzSpeed, kFlightSpeed, 0x1000);
    UpdatePos(0);
}

// @symbol _ZN8daBook_c19func_ov020_02111b28Ev
/* STATE_FLY: moves by the velocity in unk_0a4/mVertSpeed/unk_0ac and is gone
 * when it hits the player, the ground, or a wall that faces it (the wall's
 * normal more than a quarter turn from the book's heading). */
void daBook_c::func_ov020_02111b28()
{
  if (mUsesModelAnim != 0) {
    mModelAnim.Advance();
  }
  AddVec3(&mPosX, &unk_0a4, &mPosX);
  if (func_ov020_02111418() != 0) return;
  dBgCh_Actr_UpdateContinuous_Veneer(&mWithMeshClsn);
  if (mWithMeshClsn.IsOnGround() != 0) {
    unk_108 = 0;
    func_ov020_02112110();
    func_0201267c(kSfxImpact, (const Vector3 *)&mCamSpacePosX);
    return;
  }
  if (mWithMeshClsn.IsOnWall() == 0) return;
  {
    Vector3 normal;
    void* w = _ZNK10dBgCh_Actr13GetWallResultEv(&mWithMeshClsn);
    ((SurfaceInfo *)((char *)w + 4))->CopyNormalTo(normal);
    short angle = _ZN4cstd5atan2E5Fix12IiES1_(normal.x, normal.z);
    if (GetSubtraction(mPrevAngleY, angle) <= 0x4000) return;
  }
  unk_108 = 0;
  func_ov020_02112110();
  func_0201267c(kSfxImpact, (const Vector3 *)&mCamSpacePosX);
}

// @symbol _ZN8daBook_c19func_ov020_02111c30Ev
/* STATE_WIND_UP, by frames of mStateTimer after the speed has ramped to 10
 * units: from 5 it re-aims every frame and turns to face the player, from 9
 * it rolls, from 0x13 it swells toward 1.5 times size, growing the collider
 * with it. When the animation finishes it switches to the flight animation,
 * aims one last time and launches along that direction at kFlightSpeed. */
void daBook_c::func_ov020_02111c30()
{
    if (func_ov020_02111418())
        return;

    mModelAnim.Advance();

    {
        if (ApproachLinear(mHorzSpeed, 0xa000, 0xa00) == 0)
            return;
    }

    if (mModelAnim.Finished()) {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, (BCA_File*)data_ov020_02114ab0.words[1], 0, 0x1000, 0);
        mState = STATE_FLY;
        mHorzSpeed = 0;
        func_ov020_021112b0();

        mPrevAngleX = -mAimPitch;
        mAngleX = mPrevAngleX;
        mPrevAngleY = mAimYaw;
        mAngleY = mPrevAngleY;

        {
            /* velocity = kFlightSpeed along (yaw, pitch) */
            s32 r = FIX12_MUL(data_02082214[SINCOS_INDEX(mAngleX) + 1], 0x32000);
            unk_0a4 = FIX12_MUL(r, data_02082214[SINCOS_INDEX(mAngleY)]);
            mVertSpeed = FIX12_MUL(data_02082214[SINCOS_INDEX(mAngleX)], -0x32000);
            unk_0ac = FIX12_MUL(r, data_02082214[SINCOS_INDEX(mAngleY) + 1]);
        }
        return;
    }

    STATE_TIMER++;
    if (STATE_TIMER < 5)
        return;

    func_ov020_021112b0();
    ApproachLinear(mAngleY, mAimYaw, 0x7d0);
    ApproachLinear(mAngleX, -mAimPitch, 0x7d0);

    if (STATE_TIMER < 9)
        return;

    ApproachLinear(mAngleZ, mAimRoll, 0x7d0);

    if (STATE_TIMER < 0x13)
        return;

    /* collider radius is 50 units and height 100 units at scale 1.0 */
    ApproachLinear(mUniformScale, 0x1800, 0x19a);
    mdCcAcPos_c.radius = mUniformScale * 0x32;
    mdCcAcPos_c.height = mUniformScale * 0x64;
    mClsnOffset.y = mUniformScale * -0x32;
    {
        s32 v = mUniformScale;
        mScaleX = v;
        mScaleY = v;
        mScaleZ = v;
    }
}

// @symbol _ZN8daBook_c19func_ov020_02111ee0Ev
/* STATE_TILT_BACK: tips the book back by 45 degrees, then switches mModelAnim
 * to the animated model, starts its animation, lifts the book 50 units and
 * moves the collider down 25. */
void daBook_c::func_ov020_02111ee0()
{
  int r = func_ov020_02111418();
  if(r) return;
  if(ApproachLinear(mAngleX, -0x2000, 0x200)){
    int s;
    mHorzSpeed = 0;
    s = mModelAnim.SetFile((BMD_File*)data_ov020_02114aa0.words[1], 1, -1);
    if(s == 0) return;
    mState = STATE_WIND_UP;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, (BCA_File*)data_ov020_02114aa8.words[1], 0x40000000, 0x1000, 0);
    mUsesModelAnim = 1;
    mStateTimer = 0;
    {
      int* p60 = &mPosY;
      *p60 = *p60 + 0x32000;
    }
    mClsnOffset.y = -0x19000;
  }
  UpdatePos(0);
}

// @symbol _ZN8daBook_c19func_ov020_02111fc4Ev
/* STATE_WAIT (KILLER_BOOK): when the closest player is within 400 units and
 * within 0x3000 (67.5 degrees) of the book's heading, starts tilting back and
 * switches its collider on. */
void daBook_c::func_ov020_02111fc4()
{
    Vector3 v;
    Player *p = ClosestPlayer();
    {
        int* s = &p->mPosX;
        v.x = s[0];
        v.y = s[1];
        v.z = s[2];
    }
    if (Vec3_Dist((Vector3*)&mPosX, &v) >= 0x190000) return;
    {
        short ang = Vec3_HorzAngle((Vector3*)&mPosX, &v);
        if (GetSubtraction(mPrevAngleY, ang) >= 0x3000) return;
    }
    mState = STATE_TILT_BACK;
    mHorzSpeed = 0x5000;
    mStateTimer = 0;
    {
        u32* p234 = &mdCcAcPos_c.flags;
        *p234 = *p234 & ~1;
    }
    func_0201267c(kSfxTakeOff, (const Vector3*)&mCamSpacePosX);
}

// @symbol _ZN8daBook_c19func_ov020_02112080Ev
/* The flying books' state machine; mState 6..10 belong to
 * func_ov020_0211174c. */
void daBook_c::func_ov020_02112080()
{
    switch (mState) {
    case STATE_WAIT: func_ov020_02111fc4(); break;
    case STATE_TILT_BACK: func_ov020_02111ee0(); break;
    case STATE_WIND_UP: func_ov020_02111c30(); break;
    case STATE_FLY: func_ov020_02111b28(); break;
    case STATE_SPAWNED: func_ov020_02111aa8(); break;
    case STATE_YOSHI_SKID: func_ov020_021119dc(); break;
    }
}

// @symbol _ZN8daBook_c19func_ov020_02112110Ev
/* The end of a book: drops a blue coin if unk_108 is set, puffs smoke and
 * removes the actor. */
void daBook_c::func_ov020_02112110()
{
  if (unk_108) {
    int param = mAreaId;
    dActor_c::Spawn(kBlueCoinActorId, 2, *(const Vector3*)&mPosX, 0, param, -1);
  }
  PoofDust();
  MarkForDestruction();
}

// @symbol _ZN8daBook_c19func_ov020_0211216cEv
/* Rebuilds the model matrix from the book's rotation and position (model
 * matrices are in eighths of a unit, so position >> 3), in the animated model
 * once it is in use. KILLER_BOOK also places its drop shadow, which sits at
 * the home height. */
void daBook_c::func_ov020_0211216c()
{
    Matrix4x3 *m = (mUsesModelAnim != 0) ? &mModelAnim.mat4x3 : &mModel.mat4x3;
    Matrix4x3_FromRotationZXYExt(m, mAngleX, mAngleY, mAngleZ);
    m->m[9] = mPosX >> 3;
    m->m[10] = mPosY >> 3;
    m->m[11] = mPosZ >> 3;
    int b = (actorID == kKillerBookActorId);
    if (b == 0) return;
    mShadowMat.m[9] = mPosX >> 3;
    mShadowMat.m[10] = mHomePosY >> 3;
    mShadowMat.m[11] = mPosZ >> 3;
    _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(
        this, &mShadowModel, &mShadowMat, mScaleX * 0x64, 0x12c000, 0xf);
}

// @symbol _ZN8daBook_c16CleanupResourcesEv
int daBook_c::CleanupResources()
{
    ((SharedFilePtr *)(&data_ov020_02114aa0))->Release();
    ((SharedFilePtr *)(&data_ov020_02114ab8))->Release();
    ((SharedFilePtr *)(&data_ov020_02114aa8))->Release();
    ((SharedFilePtr *)(&data_ov020_02114ab0))->Release();
    UnloadBlueCoinModel(this);
    return 1;
}

// @symbol _ZN11daBookGen_c16CleanupResourcesEv
int daBookGen_c::CleanupResources()
{
    ((SharedFilePtr *)&data_ov020_02114aa0)->Release();
    ((SharedFilePtr *)&data_ov020_02114ab8)->Release();
    UnloadBlueCoinModel(this);
    return 1;
}

// @symbol _ZN8daBook_c6RenderEv
int daBook_c::Render()
{
    /* mFlags bit 0x40000 is one of the Yoshi-mouth states */
    bool isHidden = mFlags & 0x40000;
    if (isHidden != 0)
        return 1;

    if (mUsesModelAnim != 0)
        mModelAnim.Render((Vector3 *)&mScaleX);
    else
        mModel.Render((Vector3 *)&mScaleX);

    return 1;
}

// @symbol _ZN8daBook_c8BehaviorEv
int daBook_c::Behavior()
{
    func_0200f760(this, &mdCcAcPos_c);
    if (UpdateYoshiEat(mWithMeshClsn) != 0) {
        /* Yoshi released the book (mEatenByYoshi, unk_104 == 5): save the
         * state, give it 0x8000 forward speed and let STATE_YOSHI_SKID bring
         * it to rest */
        if (mEatenByYoshi != 0 && unk_104 == 5) {
            mSavedState = mState;
            mState = STATE_YOSHI_SKID;
            mEatenByYoshi = 0;
            mVertSpeed = 0;
            mHorzSpeed = 0x8000;
        }
        func_ov020_0211216c();
        return 1;
    }
    switch (mKind) {
    case KIND_FLYING_BOOK:
        func_ov020_02112080();
        break;
    case KIND_SWITCH_BOOK:
        func_ov020_0211174c();
        break;
    }
    func_ov020_0211216c();
    mdCcAcPos_c.Clear();
    mdCcAcPos_c.SetPosRelativeToActor(mClsnOffset);
    mdCcAcPos_c.Update();
    return 1;
}

// @symbol _ZN11daBookGen_c8BehaviorEv
/* Once mSpawnTimer exceeds 0x28 (about 41 frames after the last shot), fires
 * a SHOOT_BOOK from its own position, with its mPrevAngle triple as the
 * rotation, at a player who is within 600 units horizontally and within 0x2000
 * (45 degrees) of the way it faces. A shot clears mSpawnTimer. */
int daBookGen_c::Behavior()
{
    if (mSpawnTimer > 0x28) {
        Player *player = ClosestPlayer();
        if (player != 0) {

            Vector3 tmp;
            Vector3 *ps = (Vector3 *)&player->mPosX;
            tmp = *ps;

            if (Vec3_HorzDist((Vector3 *)&mPosX, &tmp) < 0x258000) {
                short angle = Vec3_HorzAngle((Vector3 *)&mPosX, &tmp);
                if (GetSubtraction(mAngleY, angle) < 0x2000) {
                    signed char sc = mAreaId;
                    dActor_c::Spawn(
                        kShootBookActorId, 0, *(Vector3 *)&mPosX, (Vector3_16 *)&mPrevAngleX,
                        sc, -1);
                    mSpawnTimer = 0;
                }
            }
        }
    } else {
        u16 *timer = &mSpawnTimer;
        *timer = *timer + 1;
    }
    return 1;
}

// @symbol _ZN8daBook_c13InitResourcesEv
/* Loads both models and both animations, builds the collision objects, and
 * sets the profile's starting state by actorID. A book starts at half size;
 * unk_108 is nonzero only for the KILLER_BOOK; it is what makes destroying
 * the book drop a blue coin, and touching the player or the world clears it.
 * The vulnFlags are dCc_c hit bits (dCc_c.h's best-effort reading):
 * the shot and killer books take the player's attacks, the killer book also
 * Yoshi's tongue, and the switch takes punch, kick, breakdance and slide
 * kick. */
int daBook_c::InitResources()
{
    Model::LoadFile(*(SharedFilePtr *)&data_ov020_02114aa0);
    Model::LoadFile(*(SharedFilePtr *)&data_ov020_02114ab8);
    dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)&data_ov020_02114aa8);
    dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)&data_ov020_02114ab0);
    LoadBlueCoinModel(this);

    if (mShadowModel.InitCylinder() == 0)
        return 0;

    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x32000, 0x32000, 0, 0);

    mClsnOffset.x = 0;
    mClsnOffset.y = 0;
    mClsnOffset.z = 0;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(&mdCcAcPos_c, this, &mClsnOffset, 0x19000, 0x32000, 0x200001, 0);

    mLinkedActorID = 0;
    mTouchedPlayer = 0;
    mScaleX = 0x800;
    mScaleY = 0x800;
    mScaleZ = 0x800;
    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;

    if (mModel.SetFile((BMD_File*)data_ov020_02114ab8.words[1], 1, -1) == 0)
        return 0;

    mShadowMat = IDENTITY_MATRIX4X3;
    mUsesModelAnim = 0;
    mAngleY += 0x8000;
    mUniformScale = 0x800;

    {
        unsigned short id = actorID;
        unsigned int match1 = (id == kShootBookActorId);

        if (match1)
        {
            mKind = KIND_FLYING_BOOK;
            mState = STATE_SPAWNED;
            unk_108 = 0;
            mdCcAcPos_c.vulnFlags |= 0x26fe0;
            goto success;
        }
        {
            unsigned int match2 = (id == kKillerBookActorId);
            if (match2)
            {
                mKind = KIND_FLYING_BOOK;
                mState = STATE_WAIT;
                unk_108 = 3;
                mdCcAcPos_c.vulnFlags |= 0x2efe0;
                goto success;
            }
        }
        {
            unsigned int match3 = (id == kBookSwitchActorId);
            if (match3)
            {
                mKind = KIND_SWITCH_BOOK;
                mState = STATE_SWITCH_WAIT;
                unk_108 = 0;
                mdCcAcPos_c.flags |= 4;
                mdCcAcPos_c.vulnFlags |= 0x3c0;
                mScaleX = 0x1000;
                mScaleY = 0x800;
                mScaleZ = 0x800;
                goto success;
            }
        }
    }
    return 0;

success:
    return 1;
}

// @symbol _ZN11daBookGen_c13InitResourcesEv
int daBookGen_c::InitResources()
{
    mSpawnTimer = 0;
    Model::LoadFile(*(SharedFilePtr *)&data_ov020_02114aa0);
    Model::LoadFile(*(SharedFilePtr *)&data_ov020_02114ab8);
    LoadBlueCoinModel(this);
    return 1;
}

// @symbol _ZN8daBook_c16OnAimedAtWithEggEv
/* KILLER_BOOK cannot be aimed at with an egg; the others report 0x19000. */
s32 daBook_c::OnAimedAtWithEgg()
{
    int eq = (actorID == kKillerBookActorId);
    if(eq) return 0;
    return 0x19000;
}

// @symbol _ZN8daBook_c13OnYoshiTryEatEv
/* KILLER_BOOK answers 2, the others 0. */
s32 daBook_c::OnYoshiTryEat()
{
    unsigned int b = actorID==kKillerBookActorId; return b ? 2 : 0;
}

// @symbol daBook_c_classInit_BOOK_SWITCH
extern "C" daBook_c *daBook_c_classInit_BOOK_SWITCH()
{
    return new daBook_c;
}

// @symbol daBookGen_c_classInit
extern "C" daBookGen_c *daBookGen_c_classInit()
{
    return new daBookGen_c;
}

// @symbol daBook_c_classInit_KILLER_BOOK
extern "C" daBook_c *daBook_c_classInit_KILLER_BOOK()
{
    return new daBook_c;
}

// @symbol daBook_c_classInit_SHOOT_BOOK
extern "C" daBook_c *daBook_c_classInit_SHOOT_BOOK()
{
    return new daBook_c;
}

/* Order is the retail initializer: model 0x2c8, model 0x2cb, anim 0x2c9,
 * anim 0x2ca. mwcc emits __sinit_daBook_c.cpp from these four definitions. */
BookModelFilePtr data_ov020_02114aa0(0x2c8);
BookModelFilePtr data_ov020_02114ab8(0x2cb);
BookAnimationFileHandle data_ov020_02114aa8(0x2c9);
BookAnimationFileHandle data_ov020_02114ab0(0x2ca);
