//cpp
#include "types.h"
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
 * source order.
 */
// leftovers:
// - The FL_COIN puzzle manager (actor 0x4f) has no header, so its live-coin
//   count at +0xd6 stays a raw offset.
// - ReflectAngle and DropShadowRadHeight stay mangled (Fix12<int> by value).
//   GetClsnPos keeps the free (res, self) spelling: the real member returns
//   Vector3 by value. GetWallResult and GetFloorResult have no header member.
//   dBgCh_Lin and dBgPi C1/D1 run on raw char buffers.
// - func_ov002_020b18f0 stays a free extern "C": GiveCoins passes a daPly_c,
//   not a coin.
// - DiscreteNoLava_veneer keeps a local extern. Its definition drops the r0
//   it forwards.
// - Several bodies keep `(int)ptr + off` casts. PMF records
//   stay on the sinit side, not this TU's .data.

#pragma defer_codegen off

#include "common.h"
#include "nitro/fx/fx.h"
#include "daCoin_c.h"
#include "SpawnInfo.h"
#include "SharedFilePtr.h"
#include "dExtSimpleModel_c.h"
#include "daPly_c.h"
#include "daStar_c.h"
#include "daStarBase_c.h"
#include "daObjBlockL_c.h"
#include "SurfaceInfo.h"
#include "dExtModel_c.h"
#include "dBgCh_Actr.h"
#include "Sound.h"
#include "cstd.h"
#include "Particle__System.h"
#include "ParticleIds.h"
#include "nitro/fx/fx_cp.h"

/* Views of daObjBC_Switch_c.h and daObjFl_Coin_c.h, same member names and
   offsets: including either header renumbers this TU's @NNN labels. */
struct daObjBC_Switch_cView {
    u8 pad_000[0x32a];
    u16 mCoinTimerSeed; /* 0x32a */
};
struct daObjFl_Coin_cView {
    u8 pad_000[0xd6];
    u8 mLiveCoinCount; /* 0x0d6 */
};

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

/* ROM symbols. Mangled spellings live inside extern "C" so the C++ front end
 * does not mangle them a second time. */
extern "C" {
extern int Vec3_Dist(const Vector3 *a, const Vector3 *b);
extern Fix12i Vec3_HorzDist(const Vector3 *a, const Vector3 *b);
extern int Vec3_HorzLen(void *v);
extern int VEC_Mag(const Vector3 *v);
extern unsigned char DecIfAbove0_Byte(unsigned char* p);
extern unsigned short DecIfAbove0_Short(unsigned short* p);
extern void GiveCoins(int idx, int amount);
extern int GiveRedCoins(int i, int amt);
extern s8 NumRedCoins(void);
extern void func_02012694(unsigned int id, const Vector3 *v);
extern int SurfaceInfo_TestFlag0x20(void* p);
}
extern "C" {
extern void Matrix4x3_FromRotationY(void *m, int angle);
extern short atan2_hold_d_a_coin_1(int a, int b);
}

daCoin_c::~daCoin_c()
{
}

/* Red coin, first frame: looks for a BLOCK_L actor within 150 units. If there is
 * one the coin is marked as sitting in it (mInBrickBlock) and the block keeps a
 * pointer back to the coin. */
void daCoin_c::LinkToBlock() {
    dActor_c *block;

    if (mBlockScanned) return;
    if (mCoinType != COIN_RED) return;
    block = dActor_c::FindWithActorID(0xf, 0);
    while (block) {
        if (Vec3_Dist(&mPos, &block->mPos) < 0x96000) {
            mInBrickBlock = 1;
            ((daObjBlockL_c *)block)->mLinkedActor = this;
            break;
        }
        block = dActor_c::FindWithActorID(0xf, block);
    }
    mBlockScanned = 1;
}

/* If this coin's death-table bit is set, lets its star marker react (see
 * FindStarMarker) and kills the coin. Returns 1 when it did. */
int daCoin_c::KillIfCollected() {
    daStarBase_c *marker;

    if (GetBitInDeathTable() == 0) return 0;
    marker = FindStarMarker();
    if (marker) marker->SpawnRedCoinStarIfNecessary();
    KillAndTrackInDeathTable();
    return 1;
}

/* Floor probe for a coin whose floorState is FLOOR_UNPROBED. In levels 0x1c and
 * 0x27 it first looks for an RC_TIKUWA, RC_KAITEN or KM3_KAITENDAI actor (0x7e,
 * 0x81, 0x9c) just below the coin and takes its top as the floor. Failing that
 * it casts a ray 500 units down: a hit on a surface owned by an actor sets
 * FLOOR_DYNAMIC, any other hit records the hit height and sets FLOOR_FIXED. */
void daCoin_c::ProbeFloor() {
    extern signed char gCurrentSublevel;

    int offScreen;
    dActor_c* platform;

    offScreen = (int)((mFlags & 8) != 0);
    if (offScreen) return;

    if (((CoinFlagBits*)&mCoinFlags)->floorState != FLOOR_UNPROBED) return;

    if (gCurrentSublevel == 0x1c || gCurrentSublevel == 0x27) {
        platform = dActor_c::Next(0);
        if (platform) {
            do {
                u16 type = platform->actorID;
                if (type == 0x7e || type == 0x81 || type == 0x9c) {
                    int dy = mPosY - platform->mPosY;
                    int radius = platform->mClipRadius;
                    int dist = Vec3_HorzDist(&mPos, &platform->mPos);
                    if (dist < (radius << 3) && dy <= 0x1f4000 && dy >= 0) {
                        mFloorPosY = platform->mPosY + 0x32000;
                        ((CoinFlagBits *)&mCoinFlags)->floorState = FLOOR_FIXED;
                        return;
                    }
                }
                platform = dActor_c::Next(platform);
            } while (platform);
        }
    }

    {
        dBgCh_Lin ray;
        dBgPi hit;
        Vector3 rayTop, rayBottom;
        rayBottom.x = mPosX;
        rayBottom.y = mPosY;
        rayBottom.z = mPosZ;
        rayTop.x = rayBottom.x;
        rayTop.y = rayBottom.y;
        rayTop.z = rayBottom.z;
        rayTop.y += 0x14000;
        rayBottom.y -= 0x1f4000;
        ray.SetObjAndLine(rayTop, rayBottom, this);
        if (ray.DetectClsn()) {
            ray.CopyTo(hit);
            if (hit.GetClsnID() != (u32)-1 && dActor_c::FindWithID(hit.GetClsnID()) != 0) {
                ((CoinFlagBits *)&mCoinFlags)->floorState = FLOOR_DYNAMIC;
            } else {
                Vector3 pos = ray.GetClsnPos();
                mFloorPosY = pos.y;
                ((CoinFlagBits *)&mCoinFlags)->floorState = FLOOR_FIXED;
            }
        }
    }
}

/* Yoshi-mouth check: returns 1 if any of the mFlags bits 0x20000, 0x40000 or
 * 0x80000 is set. Otherwise it zeroes mEatingPlayer, clears those bits (already
 * clear on this path) and returns 0. */
int daCoin_c::CheckInYoshiMouth() {
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
daStarBase_c *daCoin_c::FindStarMarker() {
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
void daCoin_c::BounceOffWall() {
    if (mWithMeshClsn.IsOnWall() == 0) return;
    int normal[3];
    dBgPi *wall = mWithMeshClsn.GetWallResult();
    wall->surface.CopyNormalTo(*(Vector3 *)normal);
    mPrevAngleY = ReflectAngle(normal[0], normal[2], mPrevAngleY);
}

/* Shared per-frame tail of the movement handlers. Counts down the no-collision
 * timer and the disappear timer (the coin is killed when the latter reaches 1),
 * moves the coin, and runs the mesh collision -- the continuous variant when
 * the speed vector (mSpeedX, mVertSpeed, mSpeedZ) is longer than the collision
 * radius. Landing on a floor with surface flag 0x20 while falling makes a big
 * splash and leaves the coin sinking slowly (gravity -0x800, terminal velocity
 * -0x5000). */
void daCoin_c::Step() {
    DecIfAbove0_Byte(&mNoClsnTimer);
    if (DecIfAbove0_Short(&mDisappearTimer) == 1) {
        KillAndTrackInDeathTable();
        return;
    }
    UpdatePos(&mdCc_c);
    if (VEC_Mag((const Vector3 *)&mSpeedX) > mWithMeshClsn.mRadius)
        dBgCh_Actr_UpdateContinuous_Veneer(&mWithMeshClsn);
    else
        dBgCh_Actr_UpdateDiscreteNoLava_veneer(&mWithMeshClsn);
    if (!mWithMeshClsn.IsOnGround()) return;
    if (SurfaceInfo_TestFlag0x20((char *)mWithMeshClsn.GetFloorResult() + 4) == 0)
        return;
    if (mVertSpeed > 0) return;
    func_02012694(0xe2, &mCamSpacePos);
    Particle::System::NewBigSplash(mPosX, mPosY, mPosZ);
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
void daCoin_c::UpdateModelMatrix() {
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
        DropShadowRadHeight(mShadowModel, mShadowMat, 0x50000, 0x1f4000, 0xf);
        mClipRadius = 0x3e800;
        return;
    }
    if (floorState != FLOOR_FIXED) return;

    {
        int depth = mPosY - mFloorPosY;
        mClipRadius = (depth + 0x50000) >> 3;
        DropShadowRadHeight(mShadowModel, mShadowMat, 0x50000, depth + 0x28000, 0xf);
    }
}

/* Blue coin picked up: 5 coins and 0x500 health for the player. */
void daCoin_c::CollectBlueCoin(daPly_c *player) {
    mDisappearTimer = 0;
    KillAndTrackInDeathTable();
    func_02012694(0x1c, &mCamSpacePos);
    GiveCoins(player->mPlayerNo, 5);
    player->Heal(0x500);
}

/* Red coin picked up: plays bank-3 sound 0x11 (0x12 for a swimming player),
 * gives 2 coins and 0x200 health. For a coin with a spawn filter of 1..7 it
 * also counts the red coin, puts up the running total as a score popup and plays
 * sound 0x2f + total. When the eighth is collected it spawns a STAR (0xb2) 120
 * units above the coin's STARBASE and hands that marker over to the star. */
void daCoin_c::CollectRedCoin(daPly_c *player) {
    Vector3 pos;
    Vector3 starPos;
    Vector3 popupPos;
    daStarBase_c *marker;
    daStar_c *star;
    Vector3 *markerPos;
    u8 filter;

    mDisappearTimer = 0;
    KillAndTrackInDeathTable();
    if (player->mIsUnderwater != 0)
        Sound::PlayBank3(0x12, mCamSpacePos);
    else
        Sound::PlayBank3(0x11, mCamSpacePos);

    filter = mSpawnFilter;
    if (filter != 0 && filter <= 7) {
        GiveRedCoins(player->mPlayerNo, 1);
        pos = mPos;
        pos.y += 0x64000;
        popupPos = pos;
        SpawnNumber(popupPos, NumRedCoins(), 0, 0, 0);
        func_02012790(NumRedCoins() + 0x2f);
    }

    GiveCoins(player->mPlayerNo, 2);
    player->Heal(0x200);
    if (NumRedCoins() != 8) return;
    marker = FindStarMarker();
    if (marker == 0) return;
    markerPos = &marker->mPos;
    starPos = *markerPos;
    starPos.y += 0x78000;
    star = (daStar_c *)dActor_c::Spawn(
        0xb2, mSpawnFilter | 0x40, starPos, 0, marker->mAreaId, -1);
    if (star == 0) return;
    if (mAreaId != star->mAreaId)
        star->mAreaId = -1;
    star->AddStarMarker();
    star->mStarFlags |= daStar_c::FLAG_COIN_REWARD;
    star->mMarkerID = marker->uniqueID;
    marker->mBits.spawned = 1;
}

/* Yellow coin picked up: same sound choice as the red coin, 1 coin and 0x100
 * health. */
void daCoin_c::CollectYellowCoin(daPly_c *player) {
    mDisappearTimer = 0;
    KillAndTrackInDeathTable();
    if (player->mIsUnderwater)
        Sound::PlayBank3(0x12, mCamSpacePos);
    else
        Sound::PlayBank3(0x11, mCamSpacePos);
    GiveCoins(player->mPlayerNo, 1);
    player->Heal(0x100);
}

/* Hundred-coin star. Takes the daPly_c (GiveCoins calls it with the player
 * number's entry already counted): unless event bit 0x1f is set, the level is
 * below 0xf and the player's coin total has reached 100, spawns a STAR (0xb2,
 * param 0x20) 300 units above the player, sets the event bit and gives the
 * star a marker. */
extern "C" {
void func_ov002_020b18f0(char* self)
{
    extern signed char gCurrentSublevel;
    extern short gNumCoins[];
    extern int SublevelToLevel(int);

    daPly_c *player = (daPly_c *)self;
    daStar_c* star;
    struct Vector3 spawnPos;
    struct Vector3* playerPos;
    int y;
    if (Event::GetBit(0x1f)) return;
    if (SublevelToLevel(gCurrentSublevel) >= 0xf)
        return;
    if (player == 0) return;
    if (gNumCoins[player->mPlayerNo] < 0x64)
        return;
    playerPos = &player->mPos;
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
 * (blue: CollectBlueCoin, red: CollectRedCoin, else
 * CollectYellowCoin). Returns 1 when it did. */
int daCoin_c::CheckPlayerHit() {
    unsigned int playerID = mdCc_c.otherOwner;
    if (playerID != 0) {
        daPly_c *player = (daPly_c *)dActor_c::FindWithID(playerID);
        if (player != 0) {
            if (mdCc_c.hitFlags & 0x400000) {
                mDisappearTimer = 0;
                if (mCoinType == COIN_RED)
                    CollectRedCoin(player);
                else if (mCoinType == COIN_BLUE)
                    CollectBlueCoin(player);
                else
                    CollectYellowCoin(player);
                return 1;
            }
        }
    }
    return 0;
}

/* Follows mFollowTarget: copies its position (raised by 0xc8000), and marks
 * the coin for destruction once the disappear timer, counted up here, passes
 * 0x40. */
void daCoin_c::StateFollowTarget() {
    mDisappearTimer += 1;
    {
        int *targetPos = &mFollowTarget->mPosX;
        mPosX = targetPos[0];
        mPosY = targetPos[1];
        mPosZ = targetPos[2];
    }
    {
        int *posY = &mPosY;
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
void daCoin_c::StateLaunchNearPlayer() {
    extern signed char gCurrentSublevel;

    daPly_c *player;
    int range;
    int speed;

    if (((CoinFlagBits *)&mCoinFlags)->bounces == 0) {
        player = ClosestPlayer();
        if (player == 0) return;
        range = (gCurrentSublevel == 0xb) ? 0x258000 : 0x4b0000;
        if (Vec3_Dist(&mPos, &player->mPos) > range)
            return;
        speed = player->mHorzSpeed;
        if (speed < 0x14000) speed = 0x14000;
        else if (speed > 0x28000) speed = 0x28000;
        if (gCurrentSublevel == 0xb)
            speed <<= 1;
        mHorzSpeed = speed;
        mVertSpeed = 0x14000;
        ((CoinFlagBits *)&mCoinFlags)->bounces++;
        mWithMeshClsn.SetLimMovFlag();
        mDisappearTimer = 0x1c2;
    }
    if (mWithMeshClsn.JustHitGround()) {
        if (SurfaceInfo_TestFlag0x20((char *)mWithMeshClsn.GetFloorResult() + 4) == 0) {
            Sound::PlayBank3(0x52, mCamSpacePos);
            mVertSpeed = 0x19000;
        }
    }
    Step();
    BounceOffWall();
}

/* Movement handler: runs the shared tail and wall bounce, and on a landing on a
 * floor without surface flag 0x20 plays sound 0x52 and hops. A coin with no
 * horizontal speed is given 0x13000 and a disappear timer of 0x12c; the hop
 * then heads back toward the closest player if one is within 1200 units, else
 * toward the farthest one. */
void daCoin_c::StateSlideAwayFromMario() {
    Step();
    BounceOffWall();
    if (!mWithMeshClsn.JustHitGround()) return;
    if (SurfaceInfo_TestFlag0x20((char *)mWithMeshClsn.GetFloorResult() + 4) != 0)
        return;
    Sound::PlayBank3(0x52, mCamSpacePos);
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
void daCoin_c::StateBounce() {
    extern int data_ov002_020ff078[];

    Vector3 normal;
    char *floor;
    int floorClass;
    int isFlCoin;
    int bounces;
    int damping;
    CoinFlagBits *bits;
    dActor_c *flCoin;

    Step();
    BounceOffWall();
    flCoin = dActor_c::FindWithID(mPuzzleManagerID);
    if (flCoin != 0) {
        isFlCoin = flCoin->actorID;
        isFlCoin = isFlCoin == 0x4f;
        if (isFlCoin != 0) {
            if (mPosY <= flCoin->mPosY) {
                mPosY = flCoin->mPosY;
                Sound::PlayBank3(0x52, mCamSpacePos);
                bits = (CoinFlagBits *)&mCoinFlags;
                bits->bounces++;
                bounces = ((CoinFlagBits *)&mCoinFlags)->bounces;
                if (bounces < 3u)
                    damping = ((3 - bounces) << 12) / 3;
                else
                    damping = 0x555;
                mVertSpeed = -mVertSpeed * damping / 0x1000;
                mHorzSpeed >>= 1;
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

    floor = (char *)mWithMeshClsn.GetFloorResult();
    floorClass = ((SurfaceInfo *)(floor + 4))->clps.GetFloorClass();
    ((SurfaceInfo*)(floor + 4))->CopyNormalTo(normal);

    if (mWithMeshClsn.JustHitGround() != 0) {
        if (SurfaceInfo_TestFlag0x20((char *)mWithMeshClsn.GetFloorResult() + 4) == 0) {
            if (mDisappearTimer > 0xf000)
                mDisappearTimer = 0xf;
            Sound::PlayBank3(0x52, mCamSpacePos);
            bits = (CoinFlagBits *)&mCoinFlags;
            bits->bounces++;
            bounces = ((CoinFlagBits *)&mCoinFlags)->bounces;
            if (bounces < 3u)
                damping = ((3 - bounces) << 12) / 3;
            else
                damping = 0x555;
            mVertSpeed = -mVertSpeed * damping / 0x1000;
            mHorzSpeed >>= 1;
        }
    }

    if (normal.x != 0 || normal.z != 0) {
        int slide = data_ov002_020ff078[floorClass];
        int *velocity = (int *)&mSpeedX;
        *velocity += normal.x * slide;
        mSpeedZ += normal.z * slide;
        mHorzSpeed = Vec3_HorzLen(velocity);
        if (mHorzSpeed > 0x1c000) {
            mHorzSpeed = 0x1c000;
            UpdatePosWithHorzSpeedAndAng();
        }
        mPrevAngleY = cstd::atan2(mSpeedX, mSpeedZ);
    }

    if (mWithMeshClsn.JustHitGround() != 0)
        return;
    if (SurfaceInfo_TestFlag0x20((char *)mWithMeshClsn.GetFloorResult() + 4) != 0)
        return;

    if (normal.y != 0) {
        int dotX = FX_MUL(normal.x, mSpeedX);
        int dotZ = FX_MUL(normal.z, mSpeedZ);
        mVertSpeed = -(FX_Div(dotX + dotZ, normal.y) + 0x8000);
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
void daCoin_c::StateRest() {
    DecIfAbove0_Short(&mDisappearTimer);
    DecIfAbove0_Byte(&mNoClsnTimer);
    if (mDisappearTimer == 1)
        KillAndTrackInDeathTable();
}

/* Waiting handler for a coin that starts inactive (the blue coins whose spawn
 * filter is under 8). Once event bit mSpawnFilter is set it becomes active,
 * re-enables its collider, rests, and takes its disappear timer from the
 * BC_SWITCH actor (0xa) if there is one, else 0xfa. */
void daCoin_c::StateIdle() {
    dActor_c *found;

    LinkToBlock();
    if (((CoinFlagBits *)&mCoinFlags)->active) return;
    if (Event::GetBit(mSpawnFilter) == 0) return;
    ((CoinFlagBits *)&mCoinFlags)->active = 1;
    mBehaviorType = BEHAVIOR_RESTING;
    *(u32 *)&mdCc_c.flags &= ~1;
    found = dActor_c::FindWithActorID(0xa, 0);
    if (found) {
        mDisappearTimer = ((daObjBC_Switch_cView *)found)->mCoinTimerSeed;
    } else {
        mDisappearTimer = 0xfa;
    }
}

/* Movement handler that stops the coin dead on contact: shared tail, zero
 * horizontal speed on a wall, and on the ground zero vertical speed and
 * gravity, then BEHAVIOR_RESTING. */
void daCoin_c::StateDrop() {
    Step();
    if (mWithMeshClsn.IsOnWall()) mHorzSpeed = 0;
    if (mWithMeshClsn.IsOnGround()) {
        mVertSpeed = 0;
        mVertAccel = 0;
        mBehaviorType = BEHAVIOR_RESTING;
    }
}

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
            if (((daObjFl_Coin_cView *)manager)->mLiveCoinCount != 0) {
                unsigned char *count = &((daObjFl_Coin_cView *)manager)->mLiveCoinCount;
                *count = *count - 1;
            }
        }
    }
    if (mDisappearTimer != 0) return 1;
    Particle::System::NewSimple(kPtclItemVanish, mPosX, mPosY + 0x28000, mPosZ);
    return 1;
}

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

int daCoin_c::Behavior()
{
    /* Plays the launch sound once, un-binds a blue coin from its area when it
     * appears, handles collection and Yoshi's mouth, then runs the handler
     * selected by mBehaviorType and refreshes the collider. */
    typedef void (daCoin_c::*Handler)();
    extern Handler data_ov002_0210dc70[];
    extern unsigned char gGameMode;

    if ((unsigned int)(mCoinFlags << 0x1e) >> 0x1f) {
        mCoinFlags &= ~2;
        Sound::PlayBank3(0x30, mCamSpacePos);
    }
    if ((int)(actorID == 0x122) != 0) {
        if ((unsigned int)(mCoinFlags << 0x1f) >> 0x1f)
            mAreaId = -1;
    }
    if (KillIfCollected() != 0)
        return 1;
    mAngleY += 0xc00;
    if (CheckInYoshiMouth() != 0) {
        UpdateModelMatrix();
        mdCc_c.Clear();
        return 1;
    }
    ProbeFloor();
    mEatingPlayer = 0;
    if (CheckPlayerHit() != 0)
        return 1;
    (this->*data_ov002_0210dc70[mBehaviorType])();
    if ((int)(gGameMode == 1) == 0 && (int)((mFlags & 8) != 0) != 0) {
        mdCc_c.Clear();
        if (mNoClsnTimer == 0 && VEC_Mag(&mCamSpacePos) < 0x64000) {
            if (mCoinType != COIN_RED || mInBrickBlock == 0)
                mdCc_c.Update();
        }
    } else {
        UpdateModelMatrix();
        mdCc_c.Clear();
        if (mNoClsnTimer == 0) {
            if (mCoinType != COIN_RED || mInBrickBlock == 0)
                mdCc_c.Update();
        }
    }
    return 1;
}

extern "C" {
extern int SublevelToLevel(int i);
extern void SetStarMarker(int i, void* actor, int v2);
/* Parameter names keep this TU's @NNN. */
typedef void BgchInitAt_(void *thiz, void *actor, s32 f1, s32 f2, void *v, s32 f3);
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
     * Launders kept: the three `mCoinFlags = (mCoinFlags & ~m) | v` stores
     * go through `*(u8 *)((int)&mCoinFlags)`. Spelled as the plain member,
     * each one changes the code (measured per site); the plain `&=`/`|=`
     * sites take the member as is. */

    typedef struct { void* sfp; void* bmd; } FileEntry;
    extern Matrix4x3 IDENTITY_MATRIX4X3;
    extern s8 gCurrentSublevel;
    extern u8 gCurrentStarId;
    extern dActor_c *STAR_MARKERS[12];
    extern u8 gGameMode;
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
        *(u8 *)((intptr_t)(&mCoinFlags)) = (*(u8 *)((intptr_t)(&mCoinFlags)) & ~0xe0) | (FLOOR_NO_SHADOW << 5);
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
    mCoinFlags |= 2;
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
    mCoinFlags &= ~0xe0;
    goto common;
unprobedFloor:
    *(u8 *)((intptr_t)(&mCoinFlags)) = (*(u8 *)((intptr_t)(&mCoinFlags)) & ~0xe0) | (FLOOR_UNPROBED << 5);
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
            dExtSimpleModel_c::LoadFile(*(SharedFilePtr *)&data_ov002_0210d9a8);
            mCoinType = COIN_RED;
            if (SublevelToLevel(gCurrentSublevel) == 0x13 || mSpawnFilter == gCurrentStarId) {
                if (GetBitInDeathTable() == 0) {
                    for (slot = 0; slot < 0xc; slot = (s8)(slot + 1)) {
                        if (STAR_MARKERS[slot] == 0) {
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
        if (mCommonModel1.SetFile((BMD_File *)data_ov002_020ff06c[type]->bmd, 1, 1) == 0) {
            return 0;
        }
        if (mCommonModel2.SetFile((BMD_File *)data_ov002_020ff060[mCoinType]->bmd, 1, 1) == 0) {
            return 0;
        }
    } else {
        dExtSimpleModel_c::LoadFile(*(SharedFilePtr *)data_ov002_020ff06c[type]);
        dExtSimpleModel_c::LoadFile(*(SharedFilePtr *)data_ov002_020ff060[mCoinType]);
        if (mCommonModel1.SetFile((BMD_File *)data_ov002_020ff06c[mCoinType]->bmd, 1, 1) == 0) {
            return 0;
        }
        if (mCommonModel2.SetFile((BMD_File *)data_ov002_020ff060[mCoinType]->bmd, 1, 1) == 0) {
            return 0;
        }
    }

    if (mShadowModel.InitCylinder() == 0) {
        return 0;
    }

    mdCc_c.Init(this, ccRadius, ccHeight, 0x100002, 0x8000);
    mWithMeshClsn.Init(this, 0x3c000, 0x3c000, 0, 0);
    mWithMeshClsn.SetLimMovFlag();
    mWithMeshClsn.StartDetectingWater();

    behavior = mBehaviorType;
    if (behavior == 8) {
        mDisappearTimer = 0x2d;
        mdCc_c.flags |= 1;
    } else {
        if (behavior == 6) {
            int inMode1;
            inMode1 = gGameMode;
            inMode1 = (inMode1 == 1);
            if (inMode1 == 0) {
                mDisappearTimer = 0xffffu;
                mdCc_c.flags |= 1;
                goto timerSet;
            }
        }
        mDisappearTimer = 0xd2;
    timerSet:;
    }

    *(u8 *)((intptr_t)(&mCoinFlags)) = (*(u8 *)((intptr_t)(&mCoinFlags)) & ~1) | 1;

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
        mCoinFlags &= ~1;
        mdCc_c.flags |= 1;
    }
flagsDone:;

    mCoinFlags &= ~0x1c;
    mPuzzleManagerID = 0;
    return 1;
}

void daCoin_c::OnTurnIntoEgg(daPly_c &player) {
    /* Vtable slot 19. Each path hands the payout to a helper. */
    int coinType = mCoinType;

    if (coinType == COIN_RED) {
        mPosY += 0x50000;
        CollectRedCoin(&player);
    } else if (coinType == COIN_BLUE) {
        CollectBlueCoin(&player);
    } else {
        CollectYellowCoin(&player);
    }
}

s32 daCoin_c::OnYoshiTryEat()
{
    return 4;
}

/* Blue, red and yellow coin factories, in ROM order. */
extern "C" daCoin_c *daCoin_c_classInit_BLUE_COIN()
{
    return new daCoin_c();
}

extern "C" daCoin_c *daCoin_c_classInit_RED_COIN()
{
    return new daCoin_c();
}

extern "C" daCoin_c *daCoin_c_classInit_COIN()
{
    return new daCoin_c();
}

/* The three coin actor profiles. */
extern "C" SpawnInfo<daCoin_c> g_profile_COIN = {
    daCoin_c_classInit_COIN,
    0x0120,
    0x009a,
    0x20000002,
    0x00028000,
    0x00028000,
    0x01000000,
    0x00bb8000,
};

extern "C" SpawnInfo<daCoin_c> g_profile_RED_COIN = {
    daCoin_c_classInit_RED_COIN,
    0x0121,
    0x009b,
    0x08000002,
    0x00028000,
    0x00028000,
    0x01800000,
    0x00bb8000,
};

extern "C" SpawnInfo<daCoin_c> g_profile_BLUE_COIN = {
    daCoin_c_classInit_BLUE_COIN,
    0x0122,
    0x009c,
    0x00000002,
    0x00028000,
    0x00028000,
    0x01000000,
    0x00bb8000,
};

/* Per-behavior handler table (indexed by mBehaviorType). */
typedef void (daCoin_c::*CoinHandler)();

CoinHandler data_ov002_0210dc70[9] = {
    &daCoin_c::StateDrop,        &daCoin_c::StateIdle,
    &daCoin_c::StateBounce,      &daCoin_c::StateSlideAwayFromMario,
    &daCoin_c::StateRest,        &daCoin_c::StateLaunchNearPlayer,
    &daCoin_c::StateBounce,      &daCoin_c::StateIdle,
    &daCoin_c::StateFollowTarget};
