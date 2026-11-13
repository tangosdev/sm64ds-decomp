//cpp
/**
 * daObjCloset_c -- the rec-room cupboard (PL_CLOSET, actor 182), ov058.
 *
 * daObjCloset_c_classInit (0x02111688..0x021116e4, historical alias
 * RecRoomCupboard_Spawn) is a reconstructed name (RTTI daObjCloset_c, the
 * sole PL_CLOSET registry profile); retail does not store it. It hand-called
 * fBase_c::operator new(0x21c) + the inherited dActor_c ctor + this class's
 * vtable store + __cxa_vec_ctor over the five dCcAcPos_c mColliders. The
 * class has no user-declared constructor, so `new daObjCloset_c()`
 * reproduces the identical sequence.
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
 * Notes (each measured with mwccarm 2004/b56):
 * - Behavior: selfPos, frontPos and playerPos are stored and never read.
 *   They are Vector3, whose user-declared destructor keeps mwccarm from
 *   dropping the stores; a POD VecFx32 triple loses them (0x30c against
 *   the ROM's 0x3e0). The same aggregate stores make each FX_SinIdx /
 *   FX_CosIdx re-read mAngleY, as the ROM does.
 * - Behavior: the player's and the touching actor's positions are copied
 *   through a `const Vector3 &`; a direct `= actor->mPos` is a word short.
 * - Behavior: the jingles call func_02012790 (Sound.h), the ROM's wrapper
 *   for Sound::Play2D(2, id). Calling Play2D directly is 0x3e8 and
 *   relocates to _ZN5Sound6Play2DEjj, not 0x02012790.
 */

#include "daObjCloset_c.h"
#include "daPly_c.h"
#include "Message.h"
#include "Sound.h"
#include "math/Approach.h"
#include "math/CalcLib.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"

enum {
    kPlayerActor = 0xbf,
    kCylinderCount = 5,
    kCylinderSpacing = 100,
    kCylinderRadius = 110 << 12, /* 110.0 */
    kCylinderHeight = 140 << 12, /* 140.0 */
    kCylinderFlags = 0x4800002,
    kFrontReach = 90 << 12, /* 90.0, straight ahead */
    kQuarterTurn = 0x4000,
    kHalfTurn = 0x8000,
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

int AngleDiff(int a, int b);
}

/* 1: return to the rec room when the menu closes. */
void StartMinigameMenu(unsigned char returnToRecRoom);

extern u8 data_0209d684;
extern u8 data_0209d660;

/* Emission order is ROM order: the destructor pair must stay first.
 * Do not reorder. */
#pragma defer_codegen off

/* One out-of-line definition; mwccarm emits D1 and D0 from it, in that
 * order: one vtable store, the five dCcAcPos_c members in reverse,
 * then ~dActor_c. */
daObjCloset_c::~daObjCloset_c() {}

s32 daObjCloset_c::CleanupResources() {
    return 1;
}

s32 daObjCloset_c::Behavior() {
    Vector3 selfPos, frontPos, playerPos;
    Vector3 actorPos;
    Vector3 colliderPos;
    daPly_c *target = mTalkingPlayer;

    if (target) {
        selfPos = mPos;
        frontPos = mPos;
        frontPos.x += FX_MUL(FX_SinIdx((u16)mAngleY >> 4), kFrontReach);
        frontPos.z += FX_MUL(FX_CosIdx((u16)mAngleY >> 4), kFrontReach);
        {
            const Vector3 &plyPos = target->mPos;
            playerPos = plyPos;
        }

        switch (target->GetTalkState()) {
        case 0: {
            /* Face the cupboard: half a turn off its yaw, stepped by 0x800. */
            short facing = (short)(mAngleY + kHalfTurn);
            if (ApproachLinear(target->mAngleY, facing, kTurnStep) != 0) {
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
            short angleToActor;
            int diff;
            {
                const Vector3 &ap = actor->mPos;
                actorPos = ap;
            }
            angleToActor = Vec3_HorzAngle(&mPos, &actorPos);
            diff = AngleDiff(angleToActor, mAngleY);
            if (diff < kQuarterTurn) {
                if (((daPly_c *)actor)->StartTalk(*this, 0) != 0) {
                    mTalkingPlayer = (daPly_c *)actor;
                    break;
                }
            }
        }
    }

    {
        int j;
        for (j = 0; j < kCylinderCount; j++) {
            colliderPos = mPos;
            {
                int dist = ((2 - j) * kCylinderSpacing) << 12;
                s16 side = mAngleY + kQuarterTurn;
                colliderPos.x += FX_MUL(FX_SinIdx((u16)side >> 4), dist);
                colliderPos.z += FX_MUL(FX_CosIdx((u16)side >> 4), dist);
            }
            mColliders[j].pos = colliderPos;
            mColliders[j].Clear();
            mColliders[j].Update();
        }
    }

    return 1;
}

s32 daObjCloset_c::InitResources() {
    int i;
    dCcAcPos_c *collider = mColliders;
    for (i = 0; i < kCylinderCount; i++) {
        collider->Init(this, mPos, kCylinderRadius, kCylinderHeight, kCylinderFlags, 0);
        collider++;
    }
    return 1;
}

daObjCloset_c *daObjCloset_c_classInit() {
    return new daObjCloset_c();
}
