//cpp
/**
 * daObjCloset_c -- the rec-room cupboard (PL_CLOSET, actor 182), ov058.
 *
 * With nobody talking, Behavior offers talk to a colliding PLAYER (actor
 * 0xbf) standing within a quarter turn of the cupboard's front. With a
 * talker, it turns them to face the cupboard, then runs message 0xb09.
 * data_0209d684 picks the ending: 1 waits out the talk (jingle 0x98),
 * 2 puts up the saving banner (jingle 0x97, text 0x295), and anything
 * else opens the minigame menu while data_0209d660 is clear.
 *
 * InitResources plants all five cylinders on the actor (radius 110.0,
 * height 140.0, flags 0x4800002). Behavior spreads them every frame,
 * 100 units apart across the cupboard's width, centred on it.
 *
 * deslop leftovers:
 * - Behavior: selfPos, frontPos and playerPos are stored and never read.
 *   Dropping volatile on those three is 0x314 against the ROM's 0x3e0.
 * - Behavior: colliderPos stays volatile. Without it the spread loop is
 *   0x3c4 against 0x3e0; the Y store reloads that stack slot.
 * - Behavior: the front point reloads mAngleY as a volatile unsigned
 *   halfword, twice. (u16)mAngleY is 0x3ec against 0x3e0.
 * - Behavior: the positions are Vec3Raw, not Vector3. Vector3's
 *   destructor makes the function 0x3f0 against 0x3e0.
 * - Behavior: the jingles call func_02012790. Sound::Play2D(2, id) is
 *   0x3e8 and relocates to _ZN5Sound6Play2DEjj, not 0x02012790.
 * - Behavior: the turn calls _Z14ApproachLinearRsss. Spelling it
 *   ApproachLinear matches the bytes and leaves that reloc unresolved.
 * - InitResources: dCcAcPos_c::Init with Fix12<int> by value is 0x7c
 *   against 0x6c. The call stays the scalar mangled symbol
 *   (notes/mwccarm-codegen.md 6az).
 */

#include "daObjCloset_c.h"
#include "Player.h"
#include "Message.h"

/* include/types.h's Vector3 declares a destructor. Under that type the
   five locals below grow the frame by 0x10. The ROM copies are a plain
   12-byte triple, so this TU keeps a POD. */
struct Vec3Raw { int x, y, z; };

enum {
    kPlayerActor = 0xbf,
    kCylinderCount = 5,
    kCylinderSpacing = 100,
    kCylinderRadius = 110 << 12,   /* 110.0 */
    kCylinderHeight = 140 << 12,   /* 140.0 */
    kCylinderFlags = 0x4800002,
    kFrontReach = 90 << 12,        /* 90.0, straight ahead */
    kQuarterTurn = 0x4000,
    kHalfTurn = 0x8000,
    kFxRound = 0x800,
    kTurnStep = 0x800,
    kMenuMessage = 0xb09,
    kSavingMessage = 0x295,
    kMenuJingle = 0x98,
    kSaveJingle = 0x97,
    kTouched = 0x8000000,
    kResultFinish = 1,
    kResultSaving = 2
};

extern "C" {
/* Sound::Play2D(2, id). The ROM calls this wrapper, not Play2D. */
unsigned int func_02012790(unsigned int id);
int _Z14ApproachLinearRsss(short *cur, short target, short step);
short Vec3_HorzAngle(const struct Vec3Raw *a, const struct Vec3Raw *b);
int AngleDiff(int a, int b);
/* 1: return to the rec room when the menu closes. */
void StartMinigameMenu(unsigned char returnToRecRoom);
extern s16 data_02082214[];
extern u8 data_0209d684;
extern u8 data_0209d660;
/* Scalar Fix12i, not Fix12<int>: the class-typed header method pools
   the constants (notes/mwccarm-codegen.md 6az). */
void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
    void *self, void *actor, void *offset, Fix12i radius, Fix12i height,
    unsigned int flags, unsigned int vulnFlags);
}

/* Emission order is ROM order: the destructor pair must stay first.
 * Do not reorder. */
#pragma defer_codegen off

// @symbol _ZN13daObjCloset_cD1Ev
// @symbol _ZN13daObjCloset_cD0Ev
/* One out-of-line definition; mwccarm emits D1 and D0 from it, in that
 * order: one vtable store, the five dCcAcPos_c members in reverse,
 * then ~dActor_c. */
daObjCloset_c::~daObjCloset_c()
{
}

// @symbol _ZN13daObjCloset_c16CleanupResourcesEv
s32 daObjCloset_c::CleanupResources()
{
    return 1;
}

// @symbol _ZN13daObjCloset_c8BehaviorEv
s32 daObjCloset_c::Behavior()
{
    volatile Vec3Raw selfPos, frontPos, playerPos;
    Vec3Raw actorPos;
    volatile Vec3Raw colliderPos;
    Player *target = mTalkingPlayer;

    if (target) {
        /* Copied and never read. volatile keeps the stores. */
        selfPos.x = *(volatile s32 *)&mPosX;
        selfPos.y = *(volatile s32 *)&mPosY;
        selfPos.z = *(volatile s32 *)&mPosZ;
        frontPos.x = mPosX;
        frontPos.y = mPosY;
        frontPos.z = mPosZ;
        frontPos.x = mPosX + (int)(((s64)kFrontReach * data_02082214[(*(volatile u16 *)&mAngleY >> 4) * 2] + kFxRound) >> 12);
        frontPos.z = mPosZ + (int)(((s64)kFrontReach * data_02082214[(*(volatile u16 *)&mAngleY >> 4) * 2 + 1] + kFxRound) >> 12);
        {
            struct Vec3Raw *tp = (struct Vec3Raw *)&target->mPosX;
            playerPos.x = tp->x;
            playerPos.y = tp->y;
            playerPos.z = tp->z;
        }

        switch (target->GetTalkState()) {
        case 0: {
            /* Face the cupboard: half a turn off its yaw, stepped by 0x800. */
            short facing = (short)(mAngleY + kHalfTurn);
            if (_Z14ApproachLinearRsss(&target->mAngleY, facing, kTurnStep) != 0) {
                mMessageID = kMenuMessage;
                Message::PrepareTalk();
                target->ShowMessage2(*this, (s16)mMessageID, 0, 1, 0);
            }
            break;
        }
        case 1:
            /* The message is up. Wait. */
            break;
        default: {
            u8 result = data_0209d684;
            if (mMessageID == kMenuMessage) {
                if (result == kResultFinish) {
                    if (mSoundStarted == 0) {
                        func_02012790(kMenuJingle);
                        mSoundStarted = 1;
                    }
                    if (target->HasFinishedTalking() != 0) {
                        Message::EndTalk();
                        mTalkingPlayer = 0;
                        mSoundStarted = 0;
                    }
                } else if (result == kResultSaving) {
                    if (mSoundStarted == 0) {
                        func_02012790(kSaveJingle);
                        mSoundStarted = 1;
                    }
                    Message::DisplaySaving(kSavingMessage);
                } else if (data_0209d660 == 0) {
                    StartMinigameMenu(1);
                    mTalkingPlayer = 0;
                    mSoundStarted = 0;
                }
            } else {
                mTalkingPlayer = 0;
                mSoundStarted = 0;
            }
            break;
        }
        }
    } else {
        int i;
        for (i = 0; i < kCylinderCount; i++) {
            if ((mColliders[i].hitFlags & kTouched) == 0)
                continue;
            u32 id = mColliders[i].otherOwner;
            dActor_c *actor = dActor_c::FindWithID(id);
            if (actor == 0)
                continue;
            int isMatch = (actor->actorID == kPlayerActor);
            if (isMatch == 0)
                continue;
            struct Vec3Raw *ap = (struct Vec3Raw *)&actor->mPosX;
            short angleToActor;
            int diff;
            actorPos.x = ap->x;
            actorPos.y = ap->y;
            actorPos.z = ap->z;
            angleToActor = Vec3_HorzAngle((struct Vec3Raw *)&mPosX, &actorPos);
            diff = AngleDiff(angleToActor, mAngleY);
            if (diff < kQuarterTurn) {
                if (((Player *)actor)->StartTalk(*this, 0) != 0) {
                    mTalkingPlayer = (Player *)actor;
                    break;
                }
            }
        }
    }

    {
        int j;
        for (j = 0; j < kCylinderCount; j++) {
            colliderPos.x = mPosX;
            colliderPos.y = mPosY;
            colliderPos.z = mPosZ;
            {
                int distFixed = ((2 - j) * kCylinderSpacing) << 12;
                /* Side direction, a quarter turn from yaw: sideSin is
                   cos(mAngleY) and sideCos is -sin(mAngleY). */
                int idx = (unsigned short)(short)(mAngleY + kQuarterTurn) >> 4;
                s16 sideSin = data_02082214[idx * 2];
                s16 sideCos = data_02082214[idx * 2 + 1];
                int offX = (int)(((s64)distFixed * sideSin + kFxRound) >> 12);
                int offZ = (int)(((s64)distFixed * sideCos + kFxRound) >> 12);
                int newX = mPosX + offX;
                int newZ = mPosZ + offZ;
                colliderPos.x = newX;
                colliderPos.z = newZ;
                mColliders[j].pos.x = newX;
                mColliders[j].pos.y = colliderPos.y;
                mColliders[j].pos.z = colliderPos.z;
            }
            mColliders[j].Clear();
            mColliders[j].Update();
        }
    }

    return 1;
}

// @symbol _ZN13daObjCloset_c13InitResourcesEv
s32 daObjCloset_c::InitResources()
{
    int i;
    dCcAcPos_c *collider = mColliders;
    for (i = 0; i < kCylinderCount; i++) {
        _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
            collider, this, &mPosX, kCylinderRadius, kCylinderHeight, kCylinderFlags, 0);
        collider++;
    }
    return 1;
}
