//cpp
/* daCoin_c -- the coin actor: yellow, red (actor 0x121) and blue (0x122).
 *
 * ROM evidence: _ZTS8daCoin_c at ov002:0x02108778, _ZTI8daCoin_c at 0x02108784,
 * _ZTV8daCoin_c at 0x021087ec. The coined vtable name _ZTV4Coin was the same
 * object; this TU keeps the ROM name. The run is 0x020b0f54..0x020b2ba0, 31
 * functions, ending with the three registry factories
 * (daCoin_c_classInit_BLUE_COIN, _RED_COIN, _COIN), each `new daCoin_c()`.
 *
 * One out-of-line destructor is the key function. Under defer_codegen off
 * it emits D1, D0, then a D2 the cartridge does not keep, and .text follows
 * source order. opt_common_subs off is bracketed around InitResources only.
 */
// leftovers:
// - The FL_COIN puzzle manager (actor 0x4f) has no header, so its live-coin
//   count at +0xd6 stays a raw offset.
// - ReflectAngle and DropShadowRadHeight stay mangled (Fix12<int> by value).
//   GetClsnPos keeps the free (res, self) spelling: the real member returns
//   Vector3 by value. GetWallResult and GetFloorResult have no header member.
//   dBgCh_Lin and dBgPi C1/D1 run on raw char buffers.
// - func_ov002_020b18f0 stays a free extern "C": GiveCoins passes a Player,
//   not a coin.
// - DiscreteNoLava_veneer keeps a local extern. Its definition drops the r0
//   it forwards.
// - Several bodies keep `(int)ptr + off` and `(long long)` casts. PMF records
//   stay on the sinit side, not this TU's .data.


#pragma defer_codegen off

#include "common.h"
#include "daCoin_c.h"
#include "SharedFilePtr.h"
#include "Model.h"
#include "Player.h"
#include "daStar_c.h"
#include "daStarBase_c.h"
#include "daObjBlockL_c.h"
#include "SurfaceInfo.h"

namespace Event { s32 GetBit(u32 bit); void SetBit(u32 bit); }

/* mCoinType. */
enum {
    COIN_YELLOW = 0,
    COIN_RED = 1,
    COIN_BLUE = 2
};

/* mBehaviorType picks the per-frame handler. The movement handlers switch a
 * coin to this state once it has stopped. */
enum {
    BEHAVIOR_RESTING = 4
};

/* CoinFlagBits::floorState. */
enum {
    FLOOR_DYNAMIC = 0,   /* the shadow is ray-cast down every frame */
    FLOOR_FIXED = 1,     /* mFloorPosY holds the floor height */
    FLOOR_NO_SHADOW = 2, /* set for behavior 8; no shadow is drawn */
    FLOOR_UNPROBED = 7   /* set for behaviors 1 and 7 until the floor is probed */
};

/* The byte at mCoinFlags (+0x3ae). */
struct CoinFlagBits {
    u8 active : 1;             /* Render draws nothing while it is clear */
    u8 launchSoundPending : 1; /* Behavior clears it and plays sound 0x30 */
    u8 bounces : 3;            /* bounces so far; scales the next one */
    u8 floorState : 3;         /* FLOOR_* */
};

/* Registry profile payloads for the three coin actors. */
struct CoinSpawnInfo {
    daCoin_c *(*classInit)();
    s16 behaviorPriority;
    s16 renderPriority;
    u32 actorFlags;
    s32 clipOffsetY;
    s32 clipRadius;
    s32 clipDistance;
    s32 farDistance;
};

#ifndef SM64DS_PLATFORM_PC
typedef char CoinSpawnInfo_size_must_be_0x1c[sizeof(CoinSpawnInfo) == 0x1c ? 1 : -1];
#endif

/* ROM symbols. Mangled spellings live inside extern "C" so the C++ front end
 * does not mangle them a second time. */
extern "C" {
extern int Vec3_Dist(const Vector3 *a, const Vector3 *b);
extern Fix12i Vec3_HorzDist(const Vector3* a, const Vector3* b);
extern int Vec3_HorzLen(void *v);
extern int LenVec3(void* v);
extern unsigned char DecIfAbove0_Byte(unsigned char* p);
extern unsigned short DecIfAbove0_Short(unsigned short* p);
extern void GiveCoins(int idx, int amount);
extern int GiveRedCoins(int i, int amt);
extern s8 NumRedCoins(void);
extern unsigned int func_02012790(unsigned int a);
extern void func_02012694(unsigned int id, const Vector3 *v);
extern int func_02037e58(unsigned int *p);
extern int SurfaceInfo_TestFlag0x20(void* p);
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, const Vector3 &pos);
extern void _ZN8Particle6System12NewBigSplashE5Fix12IiES2_S2_(int a, int b, int c);
extern short _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s(void*, int, int, short);
extern void _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(
    void *c, void *sm, void *mtx, int a, int b, unsigned char u);
extern void *_ZN9dBgCh_LinC1Ev(dBgCh_Lin* self);
extern dBgCh_Lin *_ZN9dBgCh_LinD1Ev(void* self);
extern void *_ZN5dBgPiC1Ev(void* self);
extern void _ZN5dBgPiD1Ev(void* self);
extern void* _ZNK10dBgCh_Actr13GetWallResultEv(void*);
extern void* _ZNK10dBgCh_Actr14GetFloorResultEv(void *w);
/* local extern: the veneer forwards r0 to UpdateDiscreteNoLava; its own
 * definition file declares it void(void), which drops the object argument. */
extern void dBgCh_Actr_UpdateDiscreteNoLava_veneer(void* p);
extern void dBgCh_Actr_UpdateContinuous_Veneer(void* p);
extern void Matrix4x3_FromRotationY(void *m, int angle);
extern short _ZN4cstd5atan2E5Fix12IiES1_(int a, int b);
extern int _ZN4cstd4fdivEii(int a, int b);
}

// @symbol _ZN8daCoin_cD1Ev
// @symbol _ZN8daCoin_cD0Ev
daCoin_c::~daCoin_c()
{
}

/* Red coin, first frame: looks for a BLOCK_L actor within 150 units. If there is
 * one the coin is marked as sitting in it (mInBrickBlock) and the block keeps a
 * pointer back to the coin. */
// @symbol _ZN8daCoin_c19func_ov002_020b1008Ev
void daCoin_c::func_ov002_020b1008()
{
    dActor_c *block;

    if (mBlockScanned) return;
    if (mCoinType != COIN_RED) return;
    block = dActor_c::FindWithActorID(0xf, 0);
    while (block) {
        if (Vec3_Dist((Vector3 *)&mPosX, (Vector3 *)&block->mPosX) < 0x96000) {
            mInBrickBlock = 1;
            ((daObjBlockL_c *)block)->mLinkedActor = this;
            break;
        }
        block = dActor_c::FindWithActorID(0xf, block);
    }
    mBlockScanned = 1;
}

/* If this coin's death-table bit is set, lets its star marker react (see
 * func_ov002_020b1328) and kills the coin. Returns 1 when it did. */
// @symbol _ZN8daCoin_c19func_ov002_020b10a0Ev
int daCoin_c::func_ov002_020b10a0()
{
    daStarBase_c *marker;

    if (GetBitInDeathTable() == 0) return 0;
    marker = func_ov002_020b1328();
    if (marker) marker->SpawnRedCoinStarIfNecessary();
    KillAndTrackInDeathTable();
    return 1;
}

/* Floor probe for a coin whose floorState is FLOOR_UNPROBED. In levels 0x1c and
 * 0x27 it first looks for an RC_TIKUWA, RC_KAITEN or KM3_KAITENDAI actor (0x7e,
 * 0x81, 0x9c) just below the coin and takes its top as the floor. Failing that
 * it casts a ray 500 units down: a hit on a surface owned by an actor sets
 * FLOOR_DYNAMIC, any other hit records the hit height and sets FLOOR_FIXED. */
// @symbol _ZN8daCoin_c19func_ov002_020b10e4Ev
void daCoin_c::func_ov002_020b10e4()
{
    extern signed char data_0209f2f8;

    int offScreen;
    dActor_c* platform;

    offScreen = (int)((mFlags & 8) != 0);
    if (offScreen) return;

    if (((CoinFlagBits*)&mCoinFlags)->floorState != FLOOR_UNPROBED) return;

    if (data_0209f2f8 == 0x1c || data_0209f2f8 == 0x27) {
        platform = dActor_c::Next(0);
        if (platform) {
            do {
                u16 type = platform->actorID;
                if (type == 0x7e || type == 0x81 || type == 0x9c) {
                    int dy = mPosY - platform->mPosY;
                    int radius = platform->mClipRadius;
                    int dist = Vec3_HorzDist((Vector3*)&mPosX, (Vector3*)&platform->mPosX);
                    if (dist < (radius << 3) && dy <= 0x1f4000 && dy >= 0) {
                        mFloorPosY = platform->mPosY + 0x32000;
                        ((CoinFlagBits*)((long long)&mCoinFlags))->floorState = FLOOR_FIXED;
                        return;
                    }
                }
                platform = dActor_c::Next(platform);
            } while (platform);
        }
    }

    {
        char ray[0x78];
        char hit[0x28];
        Vector3 rayTop, rayBottom;
        _ZN9dBgCh_LinC1Ev((dBgCh_Lin*)ray);
        _ZN5dBgPiC1Ev(hit);
        rayBottom.x = mPosX;
        rayBottom.y = mPosY;
        rayBottom.z = mPosZ;
        rayTop.x = rayBottom.x;
        rayTop.y = rayBottom.y;
        rayTop.z = rayBottom.z;
        rayTop.y += 0x14000;
        rayBottom.y -= 0x1f4000;
        ((dBgCh_Lin*)ray)->SetObjAndLine(rayTop, rayBottom, this);
        if (((dBgCh_Lin*)ray)->DetectClsn()) {
            ((dBgPi*)(ray + 0x10))->CopyTo(*(dBgPi*)hit);
            if (((dBgPi*)hit)->GetClsnID() != (u32)-1 &&
                dActor_c::FindWithID(((dBgPi*)hit)->GetClsnID()) != 0) {
                ((CoinFlagBits*)((long long)&mCoinFlags))->floorState = FLOOR_DYNAMIC;
            } else {
                Vector3 pos = ((dBgCh_Lin*)ray)->GetClsnPos();
                mFloorPosY = pos.y;
                ((CoinFlagBits*)((long long)&mCoinFlags))->floorState = FLOOR_FIXED;
            }
        }
        _ZN5dBgPiD1Ev(hit);
        _ZN9dBgCh_LinD1Ev(ray);
    }
}

/* Yoshi-mouth check: returns 1 if any of the mFlags bits 0x20000, 0x40000 or
 * 0x80000 is set. Otherwise it zeroes mEatingPlayer, clears those bits (already
 * clear on this path) and returns 0. */
// @symbol _ZN8daCoin_c19func_ov002_020b12ecEv
int daCoin_c::func_ov002_020b12ec()
{
    unsigned int flagBits;
    unsigned int inMouth;
    unsigned int zero;
    u32 *flagsPtr;
    unsigned int flagsNow;

    flagBits = mFlags;
    if ((flagBits & 0xe0000) != 0) {
        inMouth = 1;
    } else {
        inMouth = 0;
    }
    if (inMouth != 0) {
        return 1;
    }
    zero = 0;
    mEatingPlayer = zero;
    flagsPtr = &mFlags;
    flagsNow = *flagsPtr;
    flagsNow &= ~0xe0000u;
    *flagsPtr = flagsNow;
    return zero;
}

/* Finds the STARBASE actor (0xb4, a daStarBase_c) whose star id equals this
 * coin's mSpawnFilter and whose state is 0. Null if there is none. */
// @symbol _ZN8daCoin_c19func_ov002_020b1328Ev
daStarBase_c *daCoin_c::func_ov002_020b1328()
{
    daStarBase_c *marker = 0;
    while (1) {
        marker = (daStarBase_c *)dActor_c::FindWithActorID(0xb4, marker);
        if (!marker) break;
        if (mSpawnFilter == marker->mStarID)
            if (marker->mState == 0)
                return marker;
    }
    return 0;
}

/* Bounce off a wall: turns the heading (mPrevAngleY) around the wall normal. */
// @symbol _ZN8daCoin_c19func_ov002_020b1384Ev
void daCoin_c::func_ov002_020b1384()
{
    if (mWithMeshClsn.IsOnWall() == 0) return;
    int normal[3];
    void* wall = _ZNK10dBgCh_Actr13GetWallResultEv(&mWithMeshClsn);
    ((SurfaceInfo*)((char*)wall + 4))->CopyNormalTo(*(Vector3*)normal);
    mPrevAngleY = _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s(this, normal[0], normal[2], mPrevAngleY);
}

/* Shared per-frame tail of the movement handlers. Counts down the no-collision
 * timer and the disappear timer (the coin is killed when the latter reaches 1),
 * moves the coin, and runs the mesh collision -- the continuous variant when
 * the speed vector (unk_0a4, mVertSpeed, unk_0ac) is longer than the collision
 * radius. Landing on a floor with surface flag 0x20 while falling makes a big
 * splash and leaves the coin sinking slowly (gravity -0x800, terminal velocity
 * -0x5000). */
// @symbol _ZN8daCoin_c19func_ov002_020b13e0Ev
void daCoin_c::func_ov002_020b13e0()
{
    DecIfAbove0_Byte(&mNoClsnTimer);
    if (DecIfAbove0_Short(&mDisappearTimer) == 1) {
        KillAndTrackInDeathTable();
        return;
    }
    UpdatePos(&mdCc_c);
    if (LenVec3(&unk_0a4) > mWithMeshClsn.mRadius)
        dBgCh_Actr_UpdateContinuous_Veneer(&mWithMeshClsn);
    else
        dBgCh_Actr_UpdateDiscreteNoLava_veneer(&mWithMeshClsn);
    if (!mWithMeshClsn.IsOnGround()) return;
    if (SurfaceInfo_TestFlag0x20((char*)_ZNK10dBgCh_Actr14GetFloorResultEv(&mWithMeshClsn) + 4) == 0) return;
    if (mVertSpeed > 0) return;
    func_02012694(0xe2, (const Vector3 *)&mCamSpacePosX);
    _ZN8Particle6System12NewBigSplashE5Fix12IiES2_S2_(mPosX, mPosY, mPosZ);
    mHorzSpeed = 0;
    mVertAccel = -0x800;
    mTerminalVelocity = -0x5000;
    mWithMeshClsn.StopDetectingWater();
}

/* Refreshes the model matrices (rotation from mAngleY, translation as position
 * >> 3) and the shadow, once per frame. The shadow is skipped while the
 * 0x40000 yoshi-mouth state bit is set, for an inactive coin, and for a red
 * coin sitting in a block.
 * FLOOR_DYNAMIC drops it 500 units with a fixed radius; FLOOR_FIXED sizes it
 * from the distance down to mFloorPosY. */
// @symbol _ZN8daCoin_c19func_ov002_020b14d8Ev
void daCoin_c::func_ov002_020b14d8()
{
    extern int IDENTITY_MATRIX4X3[];

    int inYoshiMouth;
    int floorState;

    Matrix4x3_FromRotationY(&mCommonModel1.mat4x3, mAngleY);
    mCommonModel1.mat4x3.m[9] = mPosX >> 3;
    mCommonModel1.mat4x3.m[10] = mPosY >> 3;
    mCommonModel1.mat4x3.m[11] = mPosZ >> 3;
    mCommonModel2.mat4x3 = mCommonModel1.mat4x3;
    *(struct Matrix4x3*)&mShadowMat = *(struct Matrix4x3*)IDENTITY_MATRIX4X3;
    mShadowMat.m[9] = mPosX >> 3;
    mShadowMat.m[10] = mPosY >> 3;
    mShadowMat.m[11] = mPosZ >> 3;

    inYoshiMouth = mFlags & 0x40000;
    inYoshiMouth = inYoshiMouth != 0;
    if (inYoshiMouth) return;

    if (((CoinFlagBits*)&mCoinFlags)->active == 0) return;

    if (mCoinType == COIN_RED) {
        if (mInBrickBlock != 0) return;
    }

    floorState = ((CoinFlagBits*)&mCoinFlags)->floorState;
    if (floorState == FLOOR_DYNAMIC) {
        _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(
            this, &mShadowModel, &mShadowMat, 0x50000, 0x1f4000, 0xf);
        mClipRadius = 0x3e800;
        return;
    }
    if (floorState != FLOOR_FIXED) return;

    {
        int depth = mPosY - mFloorPosY;
        mClipRadius = (depth + 0x50000) >> 3;
        _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(
            this, &mShadowModel, &mShadowMat, 0x50000, depth + 0x28000, 0xf);
    }
}

/* Blue coin picked up: 5 coins and 0x500 health for the player. */
// @symbol _ZN8daCoin_c19func_ov002_020b1674EP6Player
void daCoin_c::func_ov002_020b1674(Player *player)
{
    mDisappearTimer = 0;
    KillAndTrackInDeathTable();
    func_02012694(0x1c, (const Vector3 *)&mCamSpacePosX);
    GiveCoins(player->mPlayerNo, 5);
    player->Heal(0x500);
}

/* Red coin picked up: plays bank-3 sound 0x11 (0x12 for a swimming player),
 * gives 2 coins and 0x200 health. For a coin with a spawn filter of 1..7 it
 * also counts the red coin, puts up the running total as a score popup and plays
 * sound 0x2f + total. When the eighth is collected it spawns a STAR (0xb2) 120
 * units above the coin's STARBASE and hands that marker over to the star. */
// @symbol _ZN8daCoin_c19func_ov002_020b16c4EP6Player
#define LAUNDER(p) ((int)(p))
void daCoin_c::func_ov002_020b16c4(Player *player)
{
    volatile Vector3 unusedPos; /* written but never read; the ROM stores it */
    Vector3 starPos;
    Vector3 popupPos;
    daStarBase_c *marker;
    daStar_c *star;
    Vector3 *markerPos;
    int x, y, z;
    u8 filter;

    mDisappearTimer = 0;
    KillAndTrackInDeathTable();
    if (player->mIsUnderwater != 0)
        _ZN5Sound9PlayBank3EjRK7Vector3(0x12, *(const Vector3 *)&mCamSpacePosX);
    else
        _ZN5Sound9PlayBank3EjRK7Vector3(0x11, *(const Vector3 *)&mCamSpacePosX);

    filter = mSpawnFilter;
    if (filter != 0 && filter <= 7) {
        GiveRedCoins(player->mPlayerNo, 1);
        x = mPosX;
        unusedPos.x = x;
        y = mPosY;
        unusedPos.y = y;
        z = mPosZ;
        unusedPos.z = z;
        y += 0x64000;
        unusedPos.y = y;
        popupPos.x = x;
        popupPos.y = y;
        popupPos.z = z;
        SpawnNumber(popupPos, NumRedCoins(), 0, 0, 0);
        func_02012790(NumRedCoins() + 0x2f);
    }

    GiveCoins(player->mPlayerNo, 2);
    player->Heal(0x200);
    if (NumRedCoins() != 8) return;
    marker = func_ov002_020b1328();
    if (marker == 0) return;
    markerPos = (Vector3 *)LAUNDER(&marker->mPosX);
    x = markerPos->x;
    *(volatile int *)&starPos.x = x;
    y = markerPos->y;
    *(volatile int *)&starPos.y = y;
    z = markerPos->z;
    *(volatile int *)&starPos.z = z;
    y += 0x78000;
    *(volatile int *)&starPos.y = y;
    star = (daStar_c *)dActor_c::Spawn(
        0xb2, mSpawnFilter | 0x40, starPos, 0, marker->mAreaId, -1);
    if (star == 0) return;
    if (mAreaId != star->mAreaId)
        star->mAreaId = -1;
    star->AddStarMarker();
    *(unsigned short *)LAUNDER(&star->unk_4a2) |= 0x1000;
    *(int *)((char *)star + 0x434) = marker->uniqueID;
    *(u8 *)LAUNDER(&marker->mFlags) |= 4;
}
#undef LAUNDER

/* Yellow coin picked up: same sound choice as the red coin, 1 coin and 0x100
 * health. */
// @symbol _ZN8daCoin_c19func_ov002_020b1884EP6Player
void daCoin_c::func_ov002_020b1884(Player *player)
{
    mDisappearTimer = 0;
    KillAndTrackInDeathTable();
    if (player->mIsUnderwater)
        _ZN5Sound9PlayBank3EjRK7Vector3(0x12, *(const Vector3 *)&mCamSpacePosX);
    else
        _ZN5Sound9PlayBank3EjRK7Vector3(0x11, *(const Vector3 *)&mCamSpacePosX);
    GiveCoins(player->mPlayerNo, 1);
    player->Heal(0x100);
}

/* Hundred-coin star. Takes the Player (GiveCoins calls it with the player
 * number's entry already counted): unless event bit 0x1f is set, the level is
 * below 0xf and the player's coin total has reached 100, spawns a STAR (0xb2,
 * param 0x20) 300 units above the player, sets the event bit and gives the
 * star a marker. */
// @symbol func_ov002_020b18f0
extern "C" {
void func_ov002_020b18f0(char* self)
{
    extern signed char data_0209f2f8;
    extern short data_0209f358[];
    extern int SublevelToLevel(int);

    Player* player = (Player*)self;
    daStar_c* star;
    struct Vector3 spawnPos;
    struct Vector3* playerPos;
    int y;
    if (Event::GetBit(0x1f)) return;
    if (SublevelToLevel(data_0209f2f8) >= 0xf) return;
    if (player == 0) return;
    if (data_0209f358[player->mPlayerNo] < 0x64) return;
    playerPos = (struct Vector3*)&player->mPosX;
    spawnPos.x = playerPos->x;
    y = playerPos->y;
    spawnPos.y = y;
    spawnPos.z = playerPos->z;
    spawnPos.y = y + 0x12c000;
    star = (daStar_c*)dActor_c::Spawn(0xb2, 0x20, spawnPos, 0, player->mAreaId, -1);
    if (star == 0) return;
    Event::SetBit(0x1f);
    star->AddStarMarker();
}
}

/* Collection by the player: if the coin was hit by a player (hitFlags 0x400000)
 * it stops its disappear timer and hands out the reward for its coin type
 * (blue: func_ov002_020b1674, red: func_ov002_020b16c4, else
 * func_ov002_020b1884). Returns 1 when it did. */
// @symbol _ZN8daCoin_c19func_ov002_020b19dcEv
int daCoin_c::func_ov002_020b19dc()
{
    unsigned int playerID = mdCc_c.otherOwner;
    if (playerID != 0) {
        Player *player = (Player *)dActor_c::FindWithID(playerID);
        if (player != 0) {
            if (mdCc_c.hitFlags & 0x400000) {
                mDisappearTimer = 0;
                if (mCoinType == COIN_RED)
                    func_ov002_020b16c4(player);
                else if (mCoinType == COIN_BLUE)
                    func_ov002_020b1674(player);
                else
                    func_ov002_020b1884(player);
                return 1;
            }
        }
    }
    return 0;
}

/* Follows mFollowTarget: copies its position (raised by 0xc8000), and marks
 * the coin for destruction once the disappear timer, counted up here, passes
 * 0x40. */
// @symbol _ZN8daCoin_c19func_ov002_020b1a60Ev
void daCoin_c::func_ov002_020b1a60()
{
    mDisappearTimer += 1;
    {
        int *targetPos = (int *)((int)mFollowTarget + 0x5c);
        mPosX = targetPos[0];
        mPosY = targetPos[1];
        mPosZ = targetPos[2];
    }
    {
        int *posY = (int *)((int)this + 0x60);
        *posY += 0xc8000;
    }
    if (mDisappearTimer < 0x41) return;
    MarkForDestruction();
    mDisappearTimer = 0;
}

/* Handler that launches the coin when the player comes near: with no bounces
 * yet, and the closest player within 1200 units (600 in level 0xb), it takes
 * the player's horizontal speed (clamped to 20..40 units, doubled in level
 * 0xb) as its own, gets an upward speed of 20 units, counts one bounce, enables
 * limited movement and sets the disappear timer to 0x1c2. On any landing on a
 * floor without surface flag 0x20 it plays sound 0x52 and hops (0x19000). */
// @symbol _ZN8daCoin_c19func_ov002_020b1ad4Ev
void daCoin_c::func_ov002_020b1ad4()
{
    extern signed char data_0209f2f8;

    Player *player;
    int range;
    int speed;

    if (((CoinFlagBits *)&mCoinFlags)->bounces == 0) {
        player = ClosestPlayer();
        if (player == 0) return;
        range = (data_0209f2f8 == 0xb) ? 0x258000 : 0x4b0000;
        if (Vec3_Dist((Vector3 *)&mPosX, (Vector3 *)&player->mPosX) > range) return;
        speed = player->mHorzSpeed;
        if (speed < 0x14000) speed = 0x14000;
        else if (speed > 0x28000) speed = 0x28000;
        if (data_0209f2f8 == 0xb) speed <<= 1;
        mHorzSpeed = speed;
        mVertSpeed = 0x14000;
        ((CoinFlagBits *)(int)&mCoinFlags)->bounces++;
        mWithMeshClsn.SetLimMovFlag();
        mDisappearTimer = 0x1c2;
    }
    if (mWithMeshClsn.JustHitGround()) {
        if (SurfaceInfo_TestFlag0x20((char*)_ZNK10dBgCh_Actr14GetFloorResultEv(&mWithMeshClsn) + 4) == 0) {
            _ZN5Sound9PlayBank3EjRK7Vector3(0x52, *(const Vector3 *)&mCamSpacePosX);
            mVertSpeed = 0x19000;
        }
    }
    func_ov002_020b13e0();
    func_ov002_020b1384();
}

/* Movement handler: runs the shared tail and wall bounce, and on a landing on a
 * floor without surface flag 0x20 plays sound 0x52 and hops. A coin with no
 * horizontal speed is given 0x13000 and a disappear timer of 0x12c; the hop
 * then heads back toward the closest player if one is within 1200 units, else
 * toward the farthest one. */
// @symbol _ZN8daCoin_c19func_ov002_020b1bfcEv
void daCoin_c::func_ov002_020b1bfc()
{
    func_ov002_020b13e0();
    func_ov002_020b1384();
    if (!mWithMeshClsn.JustHitGround()) return;
    if (SurfaceInfo_TestFlag0x20((char*)_ZNK10dBgCh_Actr14GetFloorResultEv(&mWithMeshClsn) + 4) != 0) return;
    _ZN5Sound9PlayBank3EjRK7Vector3(0x52, *(const Vector3 *)&mCamSpacePosX);
    if (mHorzSpeed == 0) {
        mHorzSpeed = 0x13000;
        mDisappearTimer = 0x12c;
    } else {
        if (mHorzSpeed > 0x13000) mHorzSpeed = 0x13000;
    }
    mVertSpeed = 0x23000;
    if (DistToCPlayer() < 0x4b0000) {
        mPrevAngleY = HorzAngleToCPlayer() + 0x8000;
    } else {
        mPrevAngleY = HorzAngleToFPlayer();
    }
}

/* Tumbling handler for the coins that bounce and slide: bounces off an FL_COIN
 * (0x4f) puzzle manager found through mPuzzleManagerID once the coin has fallen
 * to its height, and otherwise off the floor. Each bounce bumps the bounce
 * count and scales the vertical speed by a fix12 factor that shrinks with it
 * (2/3, 1/3, then 0x555/0x1000) and halves the horizontal speed. On a sloped
 * floor it accelerates along the surface normal by data_ov002_020ff078 for the
 * floor's class, caps the speed at 0x1c000 and steers toward the slope. The
 * coin settles (BEHAVIOR_RESTING) once its vertical speed is spent. */
// @symbol _ZN8daCoin_c19func_ov002_020b1cc0Ev
void daCoin_c::func_ov002_020b1cc0()
{
    extern int data_ov002_020ff078[];

    Vector3 normal;
    char *floor;
    int floorClass;
    int isFlCoin;
    int bounces;
    int damping;
    CoinFlagBits *bits;
    dActor_c *flCoin;

    func_ov002_020b13e0();
    func_ov002_020b1384();
    flCoin = dActor_c::FindWithID(mPuzzleManagerID);
    if (flCoin != 0) {
        isFlCoin = flCoin->actorID;
        isFlCoin = isFlCoin == 0x4f;
        if (isFlCoin != 0) {
            if (mPosY <= flCoin->mPosY) {
                mPosY = flCoin->mPosY;
                _ZN5Sound9PlayBank3EjRK7Vector3(0x52, *(const Vector3 *)&mCamSpacePosX);
                bits = (CoinFlagBits *)((int)&mCoinFlags);
                bits->bounces++;
                bounces = ((CoinFlagBits *)&mCoinFlags)->bounces;
                if (bounces < 3u)
                    damping = ((3 - bounces) << 12) / 3;
                else
                    damping = 0x555;
                mVertSpeed = -mVertSpeed * damping / 0x1000;
                *(int *)((int)&mHorzSpeed) >>= 1;
                if (mVertSpeed >= -mVertAccel)
                    return;
                mVertSpeed = 0;
                mHorzSpeed = 0;
                mVertAccel = 0;
                mBehaviorType = BEHAVIOR_RESTING;
                return;
            }
        }
    }

    if (mWithMeshClsn.IsOnGround() == 0)
        return;

    floor = (char*)_ZNK10dBgCh_Actr14GetFloorResultEv(&mWithMeshClsn);
    floorClass = func_02037e58((unsigned int*)(floor + 4));
    ((SurfaceInfo*)(floor + 4))->CopyNormalTo(normal);

    if (mWithMeshClsn.JustHitGround() != 0) {
        if (SurfaceInfo_TestFlag0x20((char*)_ZNK10dBgCh_Actr14GetFloorResultEv(&mWithMeshClsn) + 4) == 0) {
            if (mDisappearTimer > 0xf000)
                mDisappearTimer = 0xf;
            _ZN5Sound9PlayBank3EjRK7Vector3(0x52, *(const Vector3 *)&mCamSpacePosX);
            bits = (CoinFlagBits *)((int)&mCoinFlags);
            bits->bounces++;
            bounces = ((CoinFlagBits *)&mCoinFlags)->bounces;
            if (bounces < 3u)
                damping = ((3 - bounces) << 12) / 3;
            else
                damping = 0x555;
            mVertSpeed = -mVertSpeed * damping / 0x1000;
            *(int *)((int)&mHorzSpeed) >>= 1;
        }
    }

    if (normal.x != 0 || normal.z != 0) {
        int slide = data_ov002_020ff078[floorClass];
        int *velocity = (int *)((int)&unk_0a4);
        *velocity += normal.x * slide;
        *(int *)((int)&unk_0ac) += normal.z * slide;
        mHorzSpeed = Vec3_HorzLen(velocity);
        if (mHorzSpeed > 0x1c000) {
            mHorzSpeed = 0x1c000;
            UpdatePosWithHorzSpeedAndAng();
        }
        mPrevAngleY = _ZN4cstd5atan2E5Fix12IiES1_(unk_0a4, unk_0ac);
    }

    if (mWithMeshClsn.JustHitGround() != 0)
        return;
    if (SurfaceInfo_TestFlag0x20((char*)_ZNK10dBgCh_Actr14GetFloorResultEv(&mWithMeshClsn) + 4) != 0)
        return;

    if (normal.y != 0) {
        int dotX = (int)(((long long)normal.x * unk_0a4 + 0x800) >> 12);
        int dotZ = (int)(((long long)normal.z * unk_0ac + 0x800) >> 12);
        mVertSpeed = -(_ZN4cstd4fdivEii(dotX + dotZ, normal.y) + 0x8000);
    }

    if ((unsigned)(floorClass - 1) <= 1) {
        mHorzSpeed = 0;
    } else {
        mHorzSpeed = mHorzSpeed * 0xc00 / 0x1000;
    }

    if (mHorzSpeed >= 0x1000)
        return;
    mVertSpeed = 0;
    mHorzSpeed = 0;
    mVertAccel = 0;
    mBehaviorType = BEHAVIOR_RESTING;
}

/* Timer-only handler: counts down the disappear and no-collision timers and
 * kills the coin when the disappear timer reaches 1. */
// @symbol _ZN8daCoin_c19func_ov002_020b2070Ev
void daCoin_c::func_ov002_020b2070()
{
    DecIfAbove0_Short(&mDisappearTimer);
    DecIfAbove0_Byte(&mNoClsnTimer);
    if (mDisappearTimer == 1)
        KillAndTrackInDeathTable();
}

/* Waiting handler for a coin that starts inactive (the blue coins whose spawn
 * filter is under 8). Once event bit mSpawnFilter is set it becomes active,
 * re-enables its collider, rests, and takes its disappear timer from the
 * BC_SWITCH actor (0xa) if there is one, else 0xfa. */
// @symbol _ZN8daCoin_c19func_ov002_020b20b4Ev
void daCoin_c::func_ov002_020b20b4()
{
    dActor_c *found;

    func_ov002_020b1008();
    if (((CoinFlagBits *)&mCoinFlags)->active) return;
    if (Event::GetBit(mSpawnFilter) == 0) return;
    ((CoinFlagBits *)(int)&mCoinFlags)->active = 1;
    mBehaviorType = BEHAVIOR_RESTING;
    *(u32 *)(int)&mdCc_c.flags &= ~1;
    found = dActor_c::FindWithActorID(0xa, 0);
    if (found) {
        mDisappearTimer = *(unsigned short *)((char *)found + 0x32a);
    } else {
        mDisappearTimer = 0xfa;
    }
}

/* Movement handler that stops the coin dead on contact: shared tail, zero
 * horizontal speed on a wall, and on the ground zero vertical speed and
 * gravity, then BEHAVIOR_RESTING. */
// @symbol _ZN8daCoin_c19func_ov002_020b2150Ev
void daCoin_c::func_ov002_020b2150()
{
    func_ov002_020b13e0();
    if (mWithMeshClsn.IsOnWall()) mHorzSpeed = 0;
    if (mWithMeshClsn.IsOnGround()) {
        mVertSpeed = 0;
        mVertAccel = 0;
        mBehaviorType = BEHAVIOR_RESTING;
    }
}

/* Particle::System::NewSimple takes its coordinates as Fix12<int>, which this
 * tree still spells as a plain s32, so the call is reached through its mangled
 * name. The declaration must carry C linkage: a C++-linkage prototype of that
 * identifier mangles a second time and names a symbol nothing defines. */
extern "C" void *_ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, Fix12i x, Fix12i y, Fix12i z);

// @symbol _ZN8daCoin_c16CleanupResourcesEv
s32 daCoin_c::CleanupResources()
{
    /* Vtable slot 3. Releases the files this coin's own type loaded, gives up its
     * star-tracking slot, decrements the live-coin count on the FL_COIN puzzle
     * manager that spawned it and, unless it is disappearing on a timer, puts up
     * the collection sparkle a little above itself. */
    extern char data_ov002_0210d9a8;
    extern char *data_ov002_020ff06c[];
    extern char *data_ov002_020ff060[];

    dActor_c *manager;
    int isRed = (int)(actorID == 0x121);
    if (isRed != 0) ((SharedFilePtr *)&data_ov002_0210d9a8)->Release();
    if (mCoinType == COIN_BLUE) {
        ((SharedFilePtr *)data_ov002_020ff06c[mCoinType])->Release();
        ((SharedFilePtr *)data_ov002_020ff060[mCoinType])->Release();
    }
    UntrackStar(mTrackStarID);
    manager = dActor_c::FindWithID(mPuzzleManagerID);
    if (manager != 0) {
        int isFlCoin = (int)(manager->actorID == 0x4f);
        if (isFlCoin != 0) {
            /* +0xd6 is the manager's live-coin count; no header describes
               that class yet, so the offset stays raw. */
            if (*(unsigned char *)((char *)manager + 0xd6) != 0) {
                unsigned char *count = (unsigned char *)((int)manager + 0xd6);
                *count = *count - 1;
            }
        }
    }
    if (mDisappearTimer != 0) return 1;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xd2, mPosX, mPosY + 0x28000, mPosZ);
    return 1;
}

// @symbol _ZN8daCoin_c6RenderEv
int daCoin_c::Render()
{
    int flags;
    int inYoshiMouth;

    if (!((CoinFlagBits *)&mCoinFlags)->active) return 1;
    flags = mFlags;
    inYoshiMouth = (flags & 0x40000) != 0;
    if (inYoshiMouth) return 1;
    {
        /* blinks while the disappear timer is below 0x2d */
        unsigned short timer = mDisappearTimer;
        if (timer < 0x2d && (timer & 1)) return 1;
    }
    if (!(flags & 0x10))
        mCommonModel1.Render(0);
    else
        mCommonModel2.Render(0);
    return 1;
}

// @symbol _ZN8daCoin_c8BehaviorEv
int daCoin_c::Behavior()
{
    /* Plays the launch sound once, un-binds a blue coin from its area when it
     * appears, handles collection and Yoshi's mouth, then runs the handler
     * selected by mBehaviorType and refreshes the collider. */
    typedef void (daCoin_c::*Handler)();
    extern Handler data_ov002_0210dc70[];
    extern unsigned char data_0209f2d8;

    if ((unsigned int)(mCoinFlags << 0x1e) >> 0x1f) {
        *(unsigned char *)((int)this + 0x3ae) &= ~2;
        _ZN5Sound9PlayBank3EjRK7Vector3(0x30, *(const Vector3 *)&mCamSpacePosX);
    }
    if ((int)(actorID == 0x122) != 0) {
        if ((unsigned int)(mCoinFlags << 0x1f) >> 0x1f)
            mAreaId = -1;
    }
    if (func_ov002_020b10a0() != 0) return 1;
    *(short *)((int)this + 0x8e) += 0xc00;
    if (func_ov002_020b12ec() != 0) {
        func_ov002_020b14d8();
        mdCc_c.Clear();
        return 1;
    }
    func_ov002_020b10e4();
    mEatingPlayer = 0;
    if (func_ov002_020b19dc() != 0) return 1;
    (this->*data_ov002_0210dc70[mBehaviorType])();
    if ((int)(data_0209f2d8 == 1) == 0 && (int)((mFlags & 8) != 0) != 0) {
        mdCc_c.Clear();
        if (mNoClsnTimer == 0 && LenVec3((char *)&mCamSpacePosX) < 0x64000) {
            if (mCoinType != COIN_RED || mInBrickBlock == 0)
                mdCc_c.Update();
        }
    } else {
        func_ov002_020b14d8();
        mdCc_c.Clear();
        if (mNoClsnTimer == 0) {
            if (mCoinType != COIN_RED || mInBrickBlock == 0)
                mdCc_c.Update();
        }
    }
    return 1;
}

// @symbol _ZN8daCoin_c13InitResourcesEv
extern "C" {
extern int SublevelToLevel(int i);
extern void SetStarMarker(int i, void* actor, int v2);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void* thiz, void* bmd, int a, int b);
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void* thiz, void* actor, s32 f1, s32 f2, u32 a, u32 b);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void* thiz, void* actor, s32 f1, s32 f2, void* v, s32 f3);
extern void _ZN10dBgCh_Actr13SetLimMovFlagEv(void* thiz);
extern void _ZN10dBgCh_Actr19StartDetectingWaterEv(void* thiz);
}
s32 daCoin_c::InitResources()
{
    /* Vtable slot 0. mBehaviorType is the low nibble of param1 (9..15 become 1)
     * and selects the physics (launch speed, gravity, terminal velocity), the
     * floor state and the flag bits in mCoinFlags. The actor ID picks the coin
     * type (0x121 red, 0x122 blue, else yellow), which decides the models
     * loaded and whether a red coin claims a star-tracking slot. The rest builds
     * the two models, the shadow cylinder, the actor collider and the mesh
     * collider, then sets the disappear and no-collision timers.
     *
     * Raw offsets kept:
     *  - Every read-modify-write of mCoinFlags is spelled
     *    `*(u8*)((int)this + 0x3ae)`; spelled as the member, InitResources
     *    changes size (measured). The launder is per site, not per field.
     *  - `*(s32*)((int)this + 0x190) |= 1` sets flags bit 0 of mdCc_c
     *    (dCc_c::flags at +0x18 of the sub-object at 0x178).
     *
     * `#pragma opt_common_subs off` is load-bearing: the ROM re-issues loads
     * this compiler would otherwise CSE. */
    #pragma opt_common_subs off

    typedef struct { void* sfp; void* bmd; } FileEntry;
    extern Matrix4x3 IDENTITY_MATRIX4X3;
    extern s8 data_0209f2f8;
    extern u8 data_0209f220;
    extern s32 data_0209f40c[];
    extern u8 data_0209f2d8;
    extern FileEntry* data_ov002_020ff06c[];
    extern FileEntry* data_ov002_020ff060[];
    extern void* data_ov002_0210d9a8;

    s32 ccRadius;
    s32 ccHeight;
    s32 behavior;
    s8 slot;
    s32 type;

    mCoinFlags = 0;
    ccRadius = 0x64000;
    behavior = (s32)param1 & 0xf;
    mBehaviorType = behavior;
    ccHeight = 0x40000;
    behavior = mBehaviorType;
    if (behavior >= 9) {
        behavior = 1;
        mBehaviorType = behavior;
    }
    behavior = mBehaviorType;
    if (behavior == 8) {
        mVertSpeed = 0x14000;
        mVertAccel = -0x4000;
        mTerminalVelocity = -0x37000;
        *(u8*)((int)this + 0x3ae) =
            (*(u8*)((int)this + 0x3ae) & ~0xe0) | (FLOOR_NO_SHADOW << 5);
        goto common;
    }
    if (behavior == 1 || behavior == 7) {
        goto unprobedFloor;
    }
    if (behavior == 5) {
        goto facing;
    }
    /* default */
    mVertSpeed = 0x24000;
    *(u8*)((int)this + 0x3ae) |= 2;
    goto gravity;
facing:
    {
        u32 flags = param1;
        if (flags & 8) {
            mPrevAngleY = (s16)(((flags & 0x70) << 8) + 0x8000);
        } else {
            mPrevAngleY = (s16)((flags & 0x70) << 8);
        }
    }
gravity:
    mVertAccel = -0x4000;
    mTerminalVelocity = -0x37000;
    *(u8*)((int)this + 0x3ae) &= ~0xe0;
    goto common;
unprobedFloor:
    *(u8*)((int)this + 0x3ae) =
        (*(u8*)((int)this + 0x3ae) & ~0xe0) | (FLOOR_UNPROBED << 5);
    mFloorPosY = mPosY - 0x1f4000;
    if (mBehaviorType == 7) {
        ccRadius = 0x32000;
        ccHeight = 0x28000;
    }
common:;

    mShadowMat = IDENTITY_MATRIX4X3;

    mTrackStarID = -1;
    mSpawnFilter = 0xff;

    {
        u16 id;
        int isRed;
        id = actorID;
        isRed = id;
        isRed = (isRed == 0x121);
        if (isRed) {
            mSpawnFilter = (u8)((param1 >> 4) & 7);
            Model::LoadFile(*(SharedFilePtr *)&data_ov002_0210d9a8);
            mCoinType = COIN_RED;
            if (SublevelToLevel(data_0209f2f8) == 0x13 ||
                mSpawnFilter == data_0209f220) {
                if (GetBitInDeathTable() == 0) {
                    for (slot = 0; slot < 0xc; slot = (s8)(slot + 1)) {
                        if (data_0209f40c[slot] == 0) {
                            SetStarMarker(slot, this, 4);
                            mTrackStarID = slot;
                            break;
                        }
                    }
                }
            }
        } else {
            int isBlue;
            isBlue = id;
            isBlue = (isBlue == 0x122);
            if (isBlue) {
                mSpawnFilter = (u8)((param1 >> 4) & 7);
                mCoinType = COIN_BLUE;
            } else {
                mCoinType = COIN_YELLOW;
            }
        }
    }

    type = mCoinType;
    if (type < COIN_BLUE) {
        if (_ZN9ModelBase7SetFileEP8BMD_Fileii(&mCommonModel1, data_ov002_020ff06c[type]->bmd, 1, 1) == 0) {
            return 0;
        }
        if (_ZN9ModelBase7SetFileEP8BMD_Fileii(&mCommonModel2, data_ov002_020ff060[mCoinType]->bmd, 1, 1) == 0) {
            return 0;
        }
    } else {
        Model::LoadFile(*(SharedFilePtr *)data_ov002_020ff06c[type]);
        Model::LoadFile(*(SharedFilePtr *)data_ov002_020ff060[mCoinType]);
        if (_ZN9ModelBase7SetFileEP8BMD_Fileii(&mCommonModel1, data_ov002_020ff06c[mCoinType]->bmd, 1, 1) == 0) {
            return 0;
        }
        if (_ZN9ModelBase7SetFileEP8BMD_Fileii(&mCommonModel2, data_ov002_020ff060[mCoinType]->bmd, 1, 1) == 0) {
            return 0;
        }
    }

    if (mShadowModel.InitCylinder() == 0) {
        return 0;
    }

    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCc_c, this, ccRadius, ccHeight, 0x100002, 0x8000);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x3c000, 0x3c000, 0, 0);
    _ZN10dBgCh_Actr13SetLimMovFlagEv(&mWithMeshClsn);
    _ZN10dBgCh_Actr19StartDetectingWaterEv(&mWithMeshClsn);

    behavior = mBehaviorType;
    if (behavior == 8) {
        mDisappearTimer = 0x2d;
        *(s32*)((int)this + 0x190) |= 1;
    } else {
        if (behavior == 6) {
            int inMode1;
            inMode1 = data_0209f2d8;
            inMode1 = (inMode1 == 1);
            if (inMode1 == 0) {
                mDisappearTimer = 0xffffu;
                *(s32*)((int)this + 0x190) |= 1;
                goto timerSet;
            }
        }
        mDisappearTimer = 0xd2;
    timerSet:;
    }

    *(u8*)((int)this + 0x3ae) =
        (*(u8*)((int)this + 0x3ae) & ~1) | 1;

    behavior = mBehaviorType;
    if (behavior == 1 || behavior == 7) {
        goto floorBound;
    }
    if (behavior == 5) {
        mNoClsnTimer = 0;
    } else {
        mNoClsnTimer = 0xf;
    }
    goto flagsDone;
floorBound:
    mNoClsnTimer = 0;
    if (mCoinType == COIN_BLUE && (u32)mSpawnFilter < 8) {
        *(u8*)((int)this + 0x3ae) &= ~1;
        *(s32*)((int)this + 0x190) |= 1;
    }
flagsDone:;

    *(u8*)((int)this + 0x3ae) &= ~0x1c;
    mPuzzleManagerID = 0;
    return 1;
}

// @symbol _ZN8daCoin_c13OnTurnIntoEggER6Player
void daCoin_c::OnTurnIntoEgg(Player &player)
{
    /* Vtable slot 19. Each path hands the payout to a helper. */
    int coinType = mCoinType;

    if (coinType == COIN_RED) {
        mPosY += 0x50000;
        func_ov002_020b16c4(&player);
    } else if (coinType == COIN_BLUE) {
        func_ov002_020b1674(&player);
    } else {
        func_ov002_020b1884(&player);
    }
}

// @symbol _ZN8daCoin_c13OnYoshiTryEatEv
s32 daCoin_c::OnYoshiTryEat()
{
    return 4;
}

/* Blue, red and yellow coin factories, in ROM order. */
// @symbol daCoin_c_classInit_BLUE_COIN
extern "C" daCoin_c *daCoin_c_classInit_BLUE_COIN()
{
    return new daCoin_c();
}

// @symbol daCoin_c_classInit_RED_COIN
extern "C" daCoin_c *daCoin_c_classInit_RED_COIN()
{
    return new daCoin_c();
}

// @symbol daCoin_c_classInit_COIN
extern "C" daCoin_c *daCoin_c_classInit_COIN()
{
    return new daCoin_c();
}

// @symbol g_profile_COIN
extern "C" CoinSpawnInfo g_profile_COIN = {
    daCoin_c_classInit_COIN,      0x0120, 0x009a, 0x20000002,
    0x00028000, 0x00028000, 0x01000000, 0x00bb8000
};

// @symbol g_profile_RED_COIN
extern "C" CoinSpawnInfo g_profile_RED_COIN = {
    daCoin_c_classInit_RED_COIN,  0x0121, 0x009b, 0x08000002,
    0x00028000, 0x00028000, 0x01800000, 0x00bb8000
};

// @symbol g_profile_BLUE_COIN
extern "C" CoinSpawnInfo g_profile_BLUE_COIN = {
    daCoin_c_classInit_BLUE_COIN, 0x0122, 0x009c, 0x00000002,
    0x00028000, 0x00028000, 0x01000000, 0x00bb8000
};

/* Per-behavior handler table (indexed by mBehaviorType). */
typedef void (daCoin_c::*CoinHandler)();

// @symbol data_ov002_0210dc70
CoinHandler data_ov002_0210dc70[9] = {
    &daCoin_c::func_ov002_020b2150,
    &daCoin_c::func_ov002_020b20b4,
    &daCoin_c::func_ov002_020b1cc0,
    &daCoin_c::func_ov002_020b1bfc,
    &daCoin_c::func_ov002_020b2070,
    &daCoin_c::func_ov002_020b1ad4,
    &daCoin_c::func_ov002_020b1cc0,
    &daCoin_c::func_ov002_020b20b4,
    &daCoin_c::func_ov002_020b1a60
};
