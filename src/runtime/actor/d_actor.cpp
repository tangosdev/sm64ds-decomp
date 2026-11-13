//cpp
/* dActor_c -- the base class of every actor, arm9 0x0200f658..0x02011654
 * (97 functions).
 *
 * Source order is the reverse of the ROM's: mwccarm emits one .text section per
 * function in reverse declaration order, so the constructor tops this file and
 * the leaf helpers end it.
 *
 * ROM evidence: ~dActor_c is the key function, so one out-of-line definition
 * emits D2 0x020112c8, D0 0x02011314 and D1 0x02011374, and the object emits
 * this class's _ZTS8dActor_c (0x0208e384), _ZTI8dActor_c (0x0208e390) and
 * _ZTV8dActor_c (0x0208e3a4). The manifest keeps those three as deadstrip-data
 * at their symbols.txt homes, so the linked table is the cartridge's.
 * dActor_c::dActor_c() emits C1 0x020113c0 and C2 0x0201150c.
 *
 * Merged from 91 legacy one-function sources. The declarations are their union,
 * reconciled to the headers that own each type (fBase_c, dBase_c, dCc_c,
 * daPly_c, dBgCh_Gnd/Lin, SpawnInfo, SharedFilePtr, Sound) and spelled with
 * those headers' field names -- dCc_c's hitFlags/otherOwner/pushback, fBase_c's
 * MarkForDestruction, SharedFilePtr's Release. data_0209b468, the global actor
 * list, is defined here as ActorList (a coined name); the list helpers
 * func_0203b244 and func_0203b27c take its address and stay free C functions.
 *
 * Measured spellings kept for the bytes:
 * - The constructor runs an unreachable Vector3s[2], which odr-uses
 *   _ZN8Vector3sD1Ev into its helper slot (0x02011508, four bytes) between C1
 *   and C2. The union and dBase_c destructors mwcc emits in the same slot have
 *   no ROM home and are deadstripped.
 *
 * common.h is included before any math/Matrix.h consumer, so Matrix4x3 is the
 * flat s32 m[12] spelling; UpdateCarry's twelve-word copies depend on that.
 */

/* Includes are the union of the 91 legacy sources' (header-guarded). */
#include "types.h"
#include "Particle__System.h"
#include "ParticleIds.h"
#include "SpawnInfo.h"
#include "decl_common.h"
#include "dActor_c.h"
#include "daPly_c.h"
#include "dCamera_c.h"
#include "common.h"
#include "math/CalcLib.h"
#include "dCc_c.h"
#include "dBgCh_Lin.h"
#include "dBgCh_Gnd.h"
#include "Sound.h"
#include "daFPknBall_c.h"
#include "daObjNumber_c.h"
#include "daCoin_c.h"
#include "dExtSimpleModel_c.h"
#include "SharedFilePtr.h"
#include "LevelRng.h"
#include "dClipper.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "math/MtxStack.h"
namespace cstd {
s16 atan2(s32, s32);
}

/* The spawn arguments func_02010e78 stages for the constructor. */

extern s16 *data_0209b460;
extern s16 *data_0209b45c;
extern s16 data_0208e378;
extern "C" {

/* local extern: no header declares it. */
int func_0203b244(void *list, void *node);
}

/* local extern: no header declares it. The actor registry, indexed by actorID. */
extern void *data_020a4bb8;
/* local extern: no header declares it. Game mode; 2 pushes the clip distance. */
extern unsigned char gGameMode;


/* The global actor list: every dActor_c links its mActorListNode into it. The
   inline constructor empties it (mwcc emits __sinit_d_actor.cpp for that);
   func_0203b244 links a node and func_0203b27c unlinks one. */
struct ActorList {
    fLiNdBa_c *first;
    fLiNdBa_c *last;

    ActorList() : first(0), last(0) {}
};
ActorList data_0209b468;

/* Constructor first in source: reverse emission puts C1/C2 above the
   destructor. Both variants are this one body. */
dActor_c::dActor_c() : mActorListNode(this) {
    func_0203b244(&data_0209b468, &mActorListNode);
    {
        int *spawnPos = (int *)data_0209b460;
        if (spawnPos) {
            mPosX = spawnPos[0];
            mPosY = spawnPos[1];
            mPosZ = spawnPos[2];
        }
    }
    {
        short *spawnRot = (short *)data_0209b45c;
        if (spawnRot) {
            mAngleX = spawnRot[0];
            mAngleY = spawnRot[1];
            mAngleZ = spawnRot[2];
            /* The ROM reloads the global. Folding the two reads drops an ldr. */
            spawnRot = (short *)data_0209b45c;
            mPrevAngleX = spawnRot[0];
            mPrevAngleY = spawnRot[1];
            mPrevAngleZ = spawnRot[2];
        }
    }
    mAreaId = data_0209b44c;
    mDeathTableID = data_0208e378;
    {
        void **table = *(void ***)&data_020a4bb8;
        int actorID = this->actorID;
        SpawnInfo<void> *info = (SpawnInfo<void> *)table[actorID];
        mFlags = info->actorFlags;
        {
            int isMode2 = (gGameMode == 2);
            int clipDistance;
            int farDistance = info->farDistance;
            if (isMode2)
                clipDistance = info->clipDistance + 0x7d0000;
            else
                clipDistance = info->clipDistance;
            SetRanges(info->clipOffsetY, info->clipRadius, clipDistance, farDistance);
        }
    }
    /* Unreachable. An array cleanup has to pass the element address to
       __cxa_vec_cleanup, so mwcc emits the out-of-line Vector3s::~Vector3s()
       in the constructor's helper slot between C1 and C2. -O4 deletes the
       cleanup itself: both variants stay 0x148. */
    if (0) {
        Vector3s tmp[2];
        (void)tmp;
    }
}

dActor_c::~dActor_c() {
    func_0203b27c((int)&data_0209b468, (int)&mActorListNode);
}

/* Kills an actor born dead: unless its profile sets 0x08000000 ("spawn even if
   the death table says already killed"), a death-table hit destroys it. */
bool dActor_c::BeforeInitResources()
{
    int spawnEvenIfKilledBefore = (mFlags & 0x8000000) ? 1 : 0;

    if (spawnEvenIfKilledBefore == 0 && GetBitInDeathTable() != 0) {
        MarkForDestruction();
        return true;
    }
    if (!fBase_c::BeforeInitResources())
        return false;
    return true;
}

void dActor_c::AfterInitResources(u32 vfSuccess)
{
    dBase_c::AfterInitResources(vfSuccess);
    mFlags |= 0x38;
}

int dActor_c::BeforeCleanupResources()
{
    if (fBase_c::BeforeCleanupResources() != 0)
        return 1;
    return 0;
}

void dActor_c::AfterCleanupResources(u32 vfSuccess)
{
    fBase_c::AfterCleanupResources(vfSuccess);
}

u8 IsAreaShowing(int areaId);
extern "C" {

void MTX_MultVec43(Vector3 *v, Matrix4x3 *m, Vector3 *dst);
}

extern int data_0209fc68;
/* nonzero forces the actor to think anyway */
extern Matrix4x3 data_0209b3ec;
/* world -> camera */
extern dClipper data_0209f43c;
/* the dClipper instance */
extern u8 data_0209f274;
/* doubles the clip distance when set */
extern u8 gGamePaused;
extern u8 data_0209f20c;
extern u8 gVsExitScreenActive;
/* This frame's actor update mask, copied from gNextActorUpdateMask; see
   ACTOR_UPDATE_MASK_STAGE_ENDING in dActor_c.h. */
extern u32 gActorUpdateMask;

/* Culls the actor for this frame: flags 0x38 plus a far-away camera position
   when its area is hidden or the dClipper rejects it, then the pause and
   update-mask gates, then snapshots mPos into mPrevPos. */
int dActor_c::BeforeBehavior()
{
    if (!fBase_c::BeforeBehavior())
        return 0;

    data_0209b458 = 0;

    signed char areaId = mAreaId;
    if (areaId >= 0 && !IsAreaShowing(areaId)) {
        mFlags |= 0x38;
        mCamSpacePosX = 0x7fffffff;
        mCamSpacePosY = 0x7fffffff;
        mCamSpacePosZ = 0x7fffffff;
        if (data_0209fc68 == 0 || (mFlags & 0x10000))
            return 0;
    } else {
        if (mClipRadius != 0) {
            Vector3 tmp;
            tmp.x = mPosX >> 3;
            tmp.y = (mPosY + mClipOffsetY) >> 3;
            tmp.z = mPosZ >> 3;
            MTX_MultVec43(&tmp, &data_0209b3ec, &mCamSpacePos);
        }
        if (mFlags & 0x10003) {
            int r = data_0209f43c.ClipSphere(mCamSpacePos, mClipRadius, &mClipResult);
            int thresh = mClipDistance;
            if (data_0209f274)
                thresh <<= 1;
            if (r > thresh) {
                mFlags |= 0x18;
                if (mFlags & 0x10000)
                    return 0;
            } else {
                mFlags &= ~0x38;
                if (r > mFarDistance)
                    mFlags |= 0x10;
            }
        } else {
            mFlags &= ~0x38;
        }
    }

    if (((gGamePaused | data_0209f20c | gVsExitScreenActive) & 0xff) != 0 ||
        (((mFlags & 9) == 9 || (gActorUpdateMask != 0 && (mFlags & gActorUpdateMask) == 0)) &&
         data_0209fc68 == 0))
        return 0;

    mPrevPosX = mPosX;
    mPrevPosY = mPosY;
    mPrevPosZ = mPosZ;
    return 1;
}

void dActor_c::AfterBehavior(u32 vfSuccess)
{
    fBase_c::AfterBehavior(vfSuccess);
}

/* Skips render when the area is not showing (0x20), or when the actor is off
   screen (0x08) and mFlags has the profile's 0x02 skip-render bit. */
int dActor_c::BeforeRender()
{
    u32 flags;

    if (!fBase_c::BeforeRender())
        return 0;

    flags = mFlags;
    if ((flags & 0x20) || ((flags & 8) && (flags & 2)))
        return 0;
    return 1;
}

void dActor_c::AfterRender(u32 vfSuccess)
{
    fBase_c::AfterRender(vfSuccess);
}

/* local extern: defined in ActorBase.cpp, which has no header. */
extern fLiNdBa_c *func_02043f98(fLiNdBa_c **head, unsigned int key);

dActor_c *dActor_c::FindWithID(u32 id)
{
    fLiNdBa_c *node = func_02043f98(&data_0209b468.first, id);
    if (node)
        return static_cast<dActor_c *>(node->owner);
    return 0;
}

dActor_c *dActor_c::FindWithActorID(u32 actorID, dActor_c *after) {
    fLiNdBa_c *node;
    if (after)
        node = func_02043f4c(&data_0209b468.first, actorID, &after->mActorListNode);
    else
        node = func_02043f4c(&data_0209b468.first, actorID, 0);
    if (node)
        return static_cast<dActor_c *>(node->owner);
    return 0;
}

dActor_c *dActor_c::Next(const dActor_c *after)
{
    fLiNdBa_c *node;
    if (after)
        node = after->mActorListNode.next;
    else
        node = data_0209b468.first;
    if (node)
        return static_cast<dActor_c *>(node->owner);
    return 0;
}

/* Stages Spawn's arguments for the constructor to pick up. */
void func_02010e78(const Vector3 *pos, const Vector3_16 *rot, s8 areaID, s16 deathTableID) {
    data_0209b460 = (s16 *)pos;
    data_0209b45c = (s16 *)rot;
    data_0209b44c = areaID;
    data_0208e378 = deathTableID;
    data_0209b458 = 0;
}

extern fBase_c *gCurrentScene;
void func_02010e78(const Vector3 *pos, const Vector3_16 *rot, s8 areaID, s16 deathTableID);

dActor_c *dActor_c::Spawn(u32 actorID, u32 spawnParam, const Vector3 &pos,
                    const Vector3_16 *rot, s8 areaID, s16 deathTableID)
{
    func_02010e78(&pos, rot, areaID, deathTableID);
    return (dActor_c *)dBase_c::Spawn(actorID, gCurrentScene, spawnParam, 2);
}

/* The radius, clip distance and far distance are stored at 1/8 scale, the
   scale BeforeBehavior projects into. */
void dActor_c::SetRanges(Fix12i clipOffsetY, Fix12i clipRadius, Fix12i clipDistance,
                         Fix12i farDistance) {
    mClipOffsetY = clipOffsetY;
    mClipRadius = clipRadius >> 3;
    mClipDistance = clipDistance >> 3;
    mFarDistance = farDistance >> 3;
}

/* Clamps each axis of mPos to +-32000.0, the edge of the world. */
void dActor_c::func_02010da4() {
    s32 v;
    v = mPosX;
    if (v < -0x7d00000)
        mPosX = -0x7d00000;
    else if (v > 0x7d00000)
        mPosX = 0x7d00000;
    v = mPosY;
    if (v < -0x7d00000)
        mPosY = -0x7d00000;
    else if (v > 0x7d00000)
        mPosY = 0x7d00000;
    v = mPosZ;
    if (v < -0x7d00000)
        mPosZ = -0x7d00000;
    else if (v > 0x7d00000)
        mPosZ = 0x7d00000;
}

/* Adds the velocity (mSpeedX, mVertSpeed, mSpeedZ) to mPos, then the
   cylinder's horizontal push-out, then clamps to the world bounds. */
void dActor_c::UpdatePosWithOnlySpeed(dCc_c *clsn) {
    VEC_Add(&mPos, (Vector3 *)&mSpeedX, &mPos);
    if (clsn != 0) {
        mPosX += clsn->pushback.x;
        mPosZ += clsn->pushback.z;
    }
    func_02010da4();
}

/* Splits mHorzSpeed along mPrevAngleY (the heading the velocity follows) into
   the x/z velocity words, x by sin and z by cos, and applies mVertAccel to
   mVertSpeed, clamped at mTerminalVelocity. */
void dActor_c::UpdatePosWithHorzSpeedAndAng() {
    s32 horzSpeed = mHorzSpeed;
    if (horzSpeed == 0) {
        s32 vertSpeed = mVertSpeed + mVertAccel;
        s32 terminal = mTerminalVelocity;
        if (vertSpeed >= terminal)
            terminal = vertSpeed;
        mSpeedX = 0;
        mVertSpeed = terminal;
        mSpeedZ = 0;
    } else {
        s32 vertSpeed;
        s32 accel;
        s32 terminal;
        s32 speedZ;
        s16 cosv;
        s32 next;
        s16 sinv;
        s32 speedX;
        cosv = FX_CosIdx((u16)mPrevAngleY >> 4);
        vertSpeed = mVertSpeed;
        accel = mVertAccel;
        terminal = mTerminalVelocity;
        speedZ = FX_MUL(horzSpeed, cosv);
        next = vertSpeed + accel;
        if (next >= terminal)
            terminal = next;
        sinv = FX_SinIdx((u16)mPrevAngleY >> 4);
        speedX = FX_MUL(horzSpeed, sinv);
        mSpeedX = speedX;
        mVertSpeed = terminal;
        mSpeedZ = speedZ;
    }
}

void dActor_c::UpdatePos(dCc_c *clsn)
{
    UpdatePosWithHorzSpeedAndAng();
    UpdatePosWithOnlySpeed(clsn);
}

void dActor_c::DropShadowRadHeight(dExtShadowModel_c &shadow, Matrix4x3 &matrix, Fix12i radius,
                                   Fix12i depth, u8 opacity) {
    if (mFlags & 0x10) /* beyond mFarDistance: no drop shadow */
        return;
    shadow.InitModel(&matrix, radius, depth, radius, opacity);
}

void dActor_c::DropShadowScaleXYZ(dExtShadowModel_c &shadow, Matrix4x3 &matrix, Fix12i scaleX,
                                  Fix12i scaleY, Fix12i scaleZ, u8 opacity) {
    if (mFlags & 0x10)
        return;
    shadow.InitModel(&matrix, scaleX, scaleY, scaleZ, opacity);
}

extern int data_0208e380;
/* distance to the cached closest player */
extern int data_0208e37c;
/* distance to the cached farthest player */
extern unsigned char gNumPlayers;
/* number of players */
extern daPly_c *gPlayers[];
/* the player table */

daPly_c *dActor_c::ClosestPlayer() {
    if (!data_0209b458) {
        data_0208e380 = 0x7fffffff;
        data_0208e37c = 0x80000000;
        int i;
        for (i = 0; i < gNumPlayers; i++) {
            daPly_c *player = gPlayers[i];
            if (!player)
                continue;
            int dist = Vec3_Dist(&mPos, &player->mPos);
            if (dist < data_0208e380) {
                data_0208e380 = dist;
                data_0209b458 = player;
            }
            if (dist > data_0208e37c) {
                data_0208e37c = dist;
                data_0209b450 = player;
            }
        }
    }
    return (daPly_c *)data_0209b458;
}

daPly_c *dActor_c::ClosestNonVanishPlayer() {
    if (!data_0209b458) {
        data_0208e380 = 0x7fffffff;
        data_0208e37c = 0x80000000;
        int i;
        for (i = 0; i < gNumPlayers; i++) {
            daPly_c *player = gPlayers[i];
            if (!player)
                continue;
            if (player->mIsVanish)
                continue;
            int dist = Vec3_Dist(&mPos, &player->mPos);
            if (dist < data_0208e380) {
                data_0208e380 = dist;
                data_0209b458 = player;
            }
            if (dist > data_0208e37c) {
                data_0208e37c = dist;
                data_0209b450 = player;
            }
        }
    }
    return (daPly_c *)data_0209b458;
}

s32 dActor_c::DistToCPlayer()
{
    ClosestPlayer();
    return data_0208e380;
}

s16 dActor_c::HorzAngleToCPlayer()
{
    ClosestPlayer();
    return Vec3_HorzAngle(&mPos, &((dActor_c *)data_0209b458)->mPos);
}

s16 dActor_c::HorzAngleToCPlayerOrAng()
{
    dActor_c *player;

    ClosestPlayer();
    player = (dActor_c *)data_0209b458;
    if (player == 0)
        return mAngleY;

    return Vec3_HorzAngle(&mPos, &player->mPos);
}

daPly_c *dActor_c::FarthestPlayer() {
    ClosestPlayer();
    return (daPly_c *)data_0209b450;
}

s16 dActor_c::HorzAngleToFPlayer()
{
    ClosestPlayer();
    return Vec3_HorzAngle(&mPos, &((dActor_c *)data_0209b450)->mPos);
}

dActor_c *dActor_c::ClosestWithActorID(u32 actorID)
{
    dActor_c *actor = 0;
    dActor_c *closest = 0;
    Fix12i closestDist = 0x7fffffff;

    for (;;) {
        actor = FindWithActorID(actorID, actor);
        if (!actor)
            break;

        if (actor != this) {
            Fix12i dist = Vec3_Dist(&mPos, &actor->mPos);
            if (dist < closestDist) {
                closestDist = dist;
                closest = actor;
            }
        }
    }
    return closest;
}

/* atan2 of v's horizontal length, projected onto heading `angle`, against
   v->y. The actor is only the member receiver. */
s16 dActor_c::func_02010844(Vector3 *v, s16 angle) {
    s32 dir;
    s32 horzLen;

    dir = cstd::atan2(v->x, v->z);
    angle = (s16)(dir - angle);
    horzLen = Vec3_HorzLen(v);
    return cstd::atan2(FX_MUL(horzLen, FX_CosIdx((u16)angle >> 4)), v->y);
}

/* count is SIGNED, although the old mangled name claimed unsigned: 0x02010734
   branches with `ble` and the loop closes with `blt`, which an unsigned count
   assembles as `bls`/`blo`. */
void dActor_c::SpawnCoins(const Vector3 &pos, s32 count, Fix12i spread, s16 angle) {
    dActor_c *actor;
    s32 ang;
    s32 prev;
    s32 i;
    u32 r;
    s16 ang0;
    Vector3_16 *rot;
    s32 deathId;
    u32 id;
    u32 param;

    ang0 = angle;
    rot = 0;
    id = 0x120;
    param = 2;
    deathId = -1;
    prev = 0xff;

    if (count > 1) {
        if (spread < 0x4000)
            spread = 0x4000;
    }

    for (i = 0; i < count; i++) {
        actor = dActor_c::Spawn(id, param, pos, rot, mAreaId, deathId);

        if (actor != 0) {
            do {
                ang = (s32)(((u32)RandomIntInternal(&data_0209e650.value) >> 16) << 27) >> 16;
            } while (ang == prev);

            r = (u32)RandomIntInternal(&data_0209e650.value);
            r >>= 16;

            actor->mPrevAngleX = 0;
            spread = (u32)((u32)spread * ((r % 50) + 100)) / 100;
            prev = ang;
            actor->mPrevAngleY = (s16)(ang0 + ang);
            actor->mPrevAngleZ = 0;
            actor->mHorzSpeed = spread;
        }
    }
}

extern const u16 data_02075230[4];
/* yellow, red, blue */
extern const u32 data_02075238[3];
void GiveCoins(s32 playerNo, s32 amount);
/* 0x0202a7b8 */

void dActor_c::GivePlayerCoins(daPly_c &player, u8 count, u32 coinKind) {
    /* Null-tested on purpose: the mangled name says R6Player and the ROM's
       test is real (see dActor_c.h). */
    if (&player == 0)
        return;
    if (count == 0)
        return;
    if (coinKind >= 3)
        return;

    if (player.mIsUnderwater)
        Sound::PlayBank3(0x12, mCamSpacePos);
    else
        Sound::PlayBank3(0x11, mCamSpacePos);

    s32 amount = count * data_02075238[coinKind];
    GiveCoins(player.mPlayerNo, amount);
    player.Heal(amount << 8);

    /* Through a base pointer, not three field reads: the ROM materialises
       `add r2, r5, #0x5c` once and loads [r2], [r2,#4], [r2,#8] off it.
       Reading player.mPosX/Y/Z directly costs one instruction less (0xf4, not
       0xf8) because the offsets fold into r5. */
    const Vector3 *src = &player.mPos;
    Vector3 spawnPos;
    spawnPos.x = src->x;
    spawnPos.y = src->y;
    spawnPos.z = src->z;
    spawnPos.y = spawnPos.y + 0x96000;

    /* A coin of the same kind (actor 0x120, 0x121 or 0x122, all daCoin_c),
       spawned above the player; it then tracks mFollowTarget every frame. */
    daCoin_c *coin =
        static_cast<daCoin_c *>(Spawn(data_02075230[coinKind], 8, spawnPos, 0, player.mAreaId, -1));
    if (coin != 0)
        coin->mFollowTarget = &player;
}

/* The landing thump's sound: 0xa for 0x380, 9 for 0x40. */
void dActor_c::func_020105cc(u32 flags) {
    if (flags & 0x380) {
        Sound::PlayBank0(0xa, mCamSpacePos);
        return;
    }
    if (flags & 0x40) {
        Sound::PlayBank0(9, mCamSpacePos);
        return;
    }
}

int dActor_c::JumpedOnByPlayer(dCc_c &clsn, daPly_c &player) {
    /* The ROM reserves 0x10 bytes of stack (`sub sp, sp, #0x10`); this
       volatile, address-taken array is what produces that frame. */
    volatile s32 pad[4];
    s32 vel = player.mVertSpeed;

    (void)&pad;
    if (clsn.flags & 6) {
        if (vel < 0 && player.mIsAirborne != 0
            && mPosY < player.mPosY - 0xa000)
            return 1;
        return 0;
    } else {
        s32 pushbackY = clsn.pushback.y;
        if (vel < 0 && pushbackY < 0 && pushbackY > vel)
            return 1;
        return 0;
    }
}

int dActor_c::BumpedUnderneathByPlayer(daPly_c &player) {
    if (player.mIsAirborne != 0 && player.mVertSpeed > 0
        && player.mPosY < mPosY)
        return 1;

    return 0;
}

bool dActor_c::IsPlayerInRange(s32 maxDist)
{
    daPly_c *closest = ClosestPlayer();

    return Vec3_Dist(&mPos, &closest->mPos) < (maxDist << 12);
}

bool dActor_c::IsPlayerInRange(Fix12i posX, Fix12i posY, Fix12i posZ, s32 maxDist) {
    Vector3 pos;
    pos.x = posX;
    pos.y = posY;
    pos.z = posZ;
    daPly_c *closest = ClosestPlayer();
    return Vec3_Dist(&pos, &closest->mPos) < (maxDist << 12);
}

bool dActor_c::IsPlayerInRange(const Vector3 &pos, s32 maxDist)
{
    daPly_c *closest = ClosestPlayer();

    return Vec3_Dist(&pos, &closest->mPos) < (maxDist << 12);
}

/* Mirrors angToReflect, turned around, about the normal's heading. */
s16 dActor_c::ReflectAngle(Fix12i normalX, Fix12i normalZ, s16 angToReflect) {
    s16 normalAngle = cstd::atan2(normalX, normalZ);
    s16 delta = (s16)((angToReflect + 0x8000) - normalAngle);
    return (s16)(normalAngle - delta);
}

/* The magnitude of the shortest turn from angle a to angle b. */
int dActor_c::GetSubtraction(short a, short b)
{
    int d = (short)(b - a);
    if (d == -0x8000)
        d = -0x7fff;
    if (d < 0)
        d = (short)(-d);
    return d;
}

dActor_c *dActor_c::FindEgg(dCc_c &clsn)
{
    u32 id = clsn.otherOwner;
    u32 flags;

    if (id != 0) {
        flags = clsn.hitFlags & 0x2000;
        if (flags != 0)
            return FindWithID(id);
    }
    return 0;
}

dActor_c *dActor_c::FindExplosionActor(dCc_c &clsn)
{
    u32 id = clsn.otherOwner;
    u32 flags;

    if (id != 0) {
        flags = clsn.hitFlags & 0x4000;
        if (flags != 0)
            return FindWithID(id);
    }
    return 0;
}

/* Returns the player behind `clsn`'s last hit if that hit carried hitFlags
   0x1000 and the player then grabs this actor; otherwise null. 0xbf is the
   player's actor ID. */
daPly_c *dActor_c::func_02010304(dCc_c *clsn) {
    u32 id = clsn->otherOwner;
    dActor_c *found;
    if (id != 0 && (found = dActor_c::FindWithID(id)) != 0 && (clsn->hitFlags & 0x1000)) {
        int isPlayer = found->actorID == 0xbf;
        if (isPlayer && ((daPly_c *)found)->TryGrab(*this))
            return (daPly_c *)found;
    }
    return 0;
}

/* Spawns actor 0xfe, daFPknBall_c (the fire piranha plant's fireball), with
   its starting speed and the speed it then approaches. Returns the ball: r0
   holds it from the Spawn call to the `bx lr`, and daTrs_c reads it back. */
dActor_c *dActor_c::SpawnFireball(const Vector3 &pos, const Vector3_16 *rot, Fix12i horzSpeed,
                                  Fix12i targetSpeed, u32 param1) {
    dActor_c *ball = dActor_c::Spawn(0xfe, param1, pos, rot, mAreaId, -1);
    if (ball != 0) {
        ball->mHorzSpeed = horzSpeed;
        static_cast<daFPknBall_c *>(ball)->mTargetSpeed = targetSpeed;
    }
    return ball;
}

extern "C" {

/* local extern: daPly_c has no accessor for the current body model. */
void *func_ov002_020e496c(daPly_c *p);
}
/* the player's current mBodyModels entry */

Matrix4x3 *dActor_c::UpdateCarry(daPly_c &player, const Vector3 &vec) {
    Vector3 t;
    t.x = 0;
    t.y = 0;
    t.z = 0;
    /* Bone 14 of the body model, carried into world space by the model's
       own matrix; only its translation survives. */
    dExtSimpleModel_c *body = (dExtSimpleModel_c *)func_ov002_020e496c(&player);
    Matrix4x3 *bones = body->data.transforms;
    data_020a0e68 = body->mat4x3;
    MTX_Concat43(&bones[14], &data_020a0e68, &data_020a0e68);
    t = *(Vector3 *)&data_020a0e68.m[9];
    mPrevAngleY = player.mAngleY;
    mAngleY = mPrevAngleY;
    Matrix4x3_FromTranslation(&data_020a0e68, t.x, t.y, t.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, mAngleX, mAngleY, mAngleZ);
    Vector3 asr;
    /* vec is a const reference; Vec3_Asr's definition is not const-qualified. */
    Vec3_Asr(&asr, (Vector3 *)&vec, 3);
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, asr.x, asr.y, asr.z);
    mPosX = data_020a0e68.m[9];
    mPosY = data_020a0e68.m[10];
    mPosZ = data_020a0e68.m[11];
    Vector3 lsl;
    Vec3_Lsl(&lsl, &mPos, 3);
    mPosX = lsl.x;
    mPosY = lsl.y;
    mPosZ = lsl.z;
    return &data_020a0e68;
}

extern dCamera_c *gCamera;
/* the camera */

void dActor_c::Earthquake(const Vector3 &pos, Fix12i magnitude) {
    gCamera->func_0200d8c8(&pos, magnitude);
}

int dActor_c::OnYoshiTryEat()
{
    return 0;
}

void dActor_c::OnTurnIntoEgg(daPly_c &player) {
    KillAndTrackInDeathTable();
}

int dActor_c::Virtual50()
{
    return 1;
}

void dActor_c::OnGroundPounded(dActor_c &pounder)
{
}

int dActor_c::OnAttacked1(dActor_c &attacker)
{
}

int dActor_c::OnAttacked2(dActor_c &attacker)
{
}

void dActor_c::OnKicked(dActor_c &kicker)
{
}

int dActor_c::OnPushed(dActor_c &pusher)
{
}

int dActor_c::OnHitByCannonBlastedChar(dActor_c &attacker)
{
}

void dActor_c::OnHitByMegaChar(daPly_c &player) {}

int dActor_c::OnHitFromUnderneath(dActor_c &attacker)
{
}

int dActor_c::OnAimedAtWithEgg()
{
    return 0x14000;
}

Vector3 dActor_c::OnAimedAtWithEggReturnVec()
{
    Vector3 result;
    result.x = mPosX;
    result.y = mPosY;
    result.z = mPosZ;
    result.y += OnAimedAtWithEgg();
    return result;
}

/* Spawns actor 0x14a, daObjNumber_c, the number that pops up over an actor. */
dActor_c *dActor_c::SpawnNumber(const Vector3 &pos, u32 value, bool packLowNibble, u16 delay,
                                dActor_c *owner) {
    daObjNumber_c *number;

    if (packLowNibble)
        number =
            static_cast<daObjNumber_c *>(Spawn(0x14a, (value & 0xf) | 0x10, pos, 0, mAreaId, -1));
    else
        number = static_cast<daObjNumber_c *>(Spawn(0x14a, value, pos, 0, mAreaId, -1));

    if (number == 0)
        return number;

    number->mDelay = delay;
    return number->SetOwner(owner);
}

extern u8 gCurrentStarId[];
/* the star index this mission was entered for */
extern s8 gCurrentSublevel[];
/* the current sublevel id */
extern dActor_c *STAR_MARKERS[12];
/* 0x0209f40c */
void SetStarMarker(s32 slot, dActor_c *actor, s32 markerType);
int IsStarCollectedInCurLevel(s32 starIdx);

s32 dActor_c::TrackStar(u32 starIdx, u32 markerType)
{
    s32 type = markerType;

    if (markerType == 2) {
        if (starIdx != gCurrentStarId[0]) {
            if (SublevelToLevel(gCurrentSublevel[0]) <= 0xe)
                return -1;
        }
        if (IsStarCollectedInCurLevel(starIdx))
            type = 3;
    }

    s8 slot;
    for (slot = 0; slot < 0xc; slot++) {
        if (STAR_MARKERS[slot] == 0) {
            SetStarMarker(slot, this, type);
            return slot;
        }
    }
    return -1;
}

void dActor_c::UntrackStar(s8 &starID)
{
    s8 slot = starID;
    if (slot < 0)
        return;

    SetStarMarker(slot, 0, 2);
    starID = -1;
}

dActor_c *dActor_c::UntrackAndSpawnStar(s8 &trackStarID, u32 starID,
                                  const Vector3 &spawnPos, u8 howToSpawnStar)
{
    UntrackStar(trackStarID);

    /* 0xb2 is the star actor. */
    return Spawn(0xb2, starID | (howToSpawnStar << 4), spawnPos, 0,
                 mAreaId, -1);
}

extern unsigned char data_0209f2c0[];
/* WDW's water level, 0..2 */
extern int data_02075244[];
/* the water height for each level */

/* In area 0 of sublevel 0x15 (WDW), the current water height; anywhere else,
   the actor's own height. */
s32 dActor_c::GetWaterHeightWDW()
{
    s32 height = mPosY;
    if (gCurrentSublevel[0] == 0x15 && mAreaId == 0) {
        unsigned int level = data_0209f2c0[0];
        if (level >= 3)
            level = 2;
        height = data_02075244[level];
    }
    return height;
}

extern SharedFilePtr data_ov002_0210da00;
/* the blue coin's two model files */
extern SharedFilePtr data_ov002_0210d9c8;

void dActor_c::LoadBlueCoinModel() {
    dExtSimpleModel_c::LoadFile(data_ov002_0210da00);
    dExtSimpleModel_c::LoadFile(data_ov002_0210d9c8);
}

void dActor_c::UnloadBlueCoinModel() {
    data_ov002_0210da00.Release();
    data_ov002_0210d9c8.Release();
}

void dActor_c::PoofDust()
{
    Vector3 pos;
    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    PoofDustAt(pos);
}

void dActor_c::PoofDustAt(const Vector3 &pos)
{
    Particle::System::NewSimple(kPtclPoofDustA, pos.x, pos.y, pos.z);
    Particle::System::NewSimple(kPtclPoofDustB, pos.x, pos.y, pos.z);
}

void dActor_c::TriplePoofDust()
{
    Vector3 pos;
    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    TriplePoofDustAt(pos);
}

void dActor_c::TriplePoofDustAt(const Vector3 &pos)
{
    Particle::System::NewSimple(kPtclTriplePoofA, pos.x, pos.y, pos.z);
    Particle::System::NewSimple(kPtclTriplePoofB, pos.x, pos.y, pos.z);
    Particle::System::NewSimple(kPtclTriplePoofC, pos.x, pos.y, pos.z);
}

void dActor_c::SmallPoofDust()
{
    Vector3 pos;
    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    DisappearPoofDustAt(pos);
}

void dActor_c::DisappearPoofDustAt(const Vector3 &pos)
{
    Particle::System::NewSimple(kPtclDisappearPoofA, pos.x, pos.y, pos.z);
    Particle::System::NewSimple(kPtclDisappearPoofB, pos.x, pos.y, pos.z);
}

/* func_0200fc44 at this actor's position. */
int dActor_c::func_0200fccc(int doRaycast) {
    Vector3 pos;
    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    return func_0200fc44(&pos, doRaycast);
}

/* Particle 0xb3 25.0 above pos, optionally first dropping pos onto the
   ground below it; pos is moved. The actor is only the member receiver. */
int dActor_c::func_0200fc44(Vector3 *pos, int doRaycast) {
    if (doRaycast) {
        dBgCh_Gnd rc;
        pos->y += 0x32000;
        rc.SetObjAndPos(*pos, 0);
        if (rc.DetectClsn())
            pos->y = rc.clsnY;
    }
    pos->y += 0x19000;
    return (int)Particle::System::NewSimple(kPtclGroundPuff, pos->x, pos->y, pos->z);
}

/* LandingDust and HugeLandingDust call each other's `...At` worker; the names
   are imported and the ROM really is crossed (see dActor_c.h). */
void dActor_c::LandingDust(bool doRaycast)
{
    Vector3 pos;
    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    HugeLandingDustAt(pos, doRaycast);
}

/* Particle 0xb2 40.0 above pos, optionally first dropping pos onto the
   ground below it; pos is moved. */
void dActor_c::HugeLandingDustAt(Vector3 &pos, bool doRaycast) {
    if (doRaycast) {
        dBgCh_Gnd rc;
        pos.y += 0x32000;
        rc.SetObjAndPos(pos, 0);
        if (rc.DetectClsn())
            pos.y = rc.clsnY;
    }
    pos.y += 0x28000;
    Particle::System::NewSimple(kPtclLandingDust, pos.x, pos.y, pos.z);
}

void dActor_c::HugeLandingDust(bool doRaycast)
{
    Vector3 pos;
    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    LandingDustAt(pos, doRaycast);
}

/* As HugeLandingDustAt, with particle 0xb1 raised 90.0. */
void dActor_c::LandingDustAt(Vector3 &pos, bool doRaycast) {
    if (doRaycast) {
        dBgCh_Gnd rc;
        pos.y += 0x32000;
        rc.SetObjAndPos(pos, 0);
        if (rc.DetectClsn())
            pos.y = rc.clsnY;
    }
    pos.y += 0x5a000;
    Particle::System::NewSimple(kPtclHugeLandingDust, pos.x, pos.y, pos.z);
}

/* func_0200fa04 at this actor's position; its void * result is ignored here. */
void dActor_c::func_0200fa8c(int doRaycast) {
    Vector3 pos;
    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    func_0200fa04(&pos, doRaycast);
}

/* As func_0200fc44, with particle 0x1b raised 120.0 instead of 25.0. */
void *dActor_c::func_0200fa04(Vector3 *pos, int doRaycast) {
    if (doRaycast) {
        dBgCh_Gnd rc;
        pos->y += 0x32000;
        rc.SetObjAndPos(*pos, 0);
        if (rc.DetectClsn())
            pos->y = rc.clsnY;
    }
    pos->y += 0x78000;
    return Particle::System::NewSimple(kPtclBossLandingDust, pos->x, pos->y, pos->z);
}

unsigned int DeathTable_GetBit(int id);

int dActor_c::GetBitInDeathTable()
{
    return DeathTable_GetBit(mDeathTableID);
}

void DeathTable_SetBit(int id);

void dActor_c::TrackInDeathTable()
{
    DeathTable_SetBit(mDeathTableID);
}

void DeathTable_ClearBit(int id);

void dActor_c::UntrackInDeathTable()
{
    DeathTable_ClearBit(mDeathTableID);
}

void dActor_c::KillAndTrackInDeathTable()
{
    TrackInDeathTable();
    MarkForDestruction();
}

dActor_c *dActor_c::SpawnSoundObj(u32 soundObjParam) {
    return Spawn(0x167, soundObjParam, mPos, 0, mAreaId, -1);
}

/* PlaySub's distance for the is-it-finished poll. */
static const int kPoolDist = 0x8777;
namespace Sound {

int PlaySecretSound(dActor_c *actor, u16 *counter) {
    if (*counter < 0x4b) {
        Sound::PlaySub(0x20, 0x14, 0x7f, 0x6b000, 0);
        *counter += 1;
    } else if (Sound::PlaySub(0x20, 0x7f, 0, kPoolDist, 0)) {
        return 1;
    }
    return 0;
}
}

namespace Sound {

int PlaySmallSecretSound(dActor_c *actor, u16 *counter) {
    if (*counter < 0x19) {
        Sound::PlaySub(0x29, 0x14, 0x7f, 0x6b000, 0);
        *counter += 1;
    } else if (Sound::PlaySub(0x29, 0x7f, 0, kPoolDist, 0)) {
        return 1;
    }
    return 0;
}
}

int func_0200f7f0(dActor_c *actor, u16 *counter) {
    if (*counter < 0x69) {
        Sound::PlaySub(0x21, 0x14, 0x7f, 0x6b000, 0);
        *counter += 1;
    } else if (Sound::PlaySub(0x21, 0x7f, 0, kPoolDist, 0)) {
        return 1;
    }
    return 0;
}

void dActor_c::MakeVanishLuigiWork(dCc_c &clsn)
{
    daPly_c *player;

    clsn.flags &= ~2;

    player = ClosestPlayer();
    if (player == 0)
        return;

    if (player->mIsVanish)
        clsn.flags |= 2;
}

/* MakeVanishLuigiWork again, taking the cylinder by pointer; the ROM keeps
   both copies. */
void dActor_c::func_0200f760(dCc_c *clsn) {
    daPly_c *player;

    clsn->flags &= ~2;
    player = ClosestPlayer();
    if (player == 0)
        return;
    if (player->mIsVanish)
        clsn->flags |= 2;
}

/* True only while the actor is off screen (mFlags 0x08), nothing forces it to
   think, and the closest player is at least threshold away. */
int dActor_c::IsTooFarAwayFromPlayer(Fix12i threshold) {
    if (data_0209fc68 == 0) {
        int offScreen = (mFlags & 8) != 0;
        if (offScreen != 0) {
            if (DistToCPlayer() >= threshold)
                return 1;
        }
    }
    return 0;
}

/* func_02037dc4 returns the hit surface's normal. The definition takes the
   surface address as an int. */
int func_02037dc4(int surface);

bool dActor_c::DetectRaycastClsn(Vector3 &start, Vector3 &end, bool storeHitPos)
{
    dBgCh_Lin ray;
    ray.SetObjAndLine(start, end, 0);
    if (ray.DetectClsn()) {
        if (storeHitPos) {
            Vector3 *hitPos = &ray.lineEnd;
            Vector3 *normal = (Vector3 *)func_02037dc4((int)&ray.surface);
            int x = hitPos->x + normal->x;
            int z = hitPos->z + normal->z;
            int y = hitPos->y + normal->y;
            end.x = x;
            end.y = y;
            end.z = z;
        }
        return true;
    }
    return false;
}
