//cpp
/**
 * daObjCloset_c -- the break-room cupboard (PL_CLOSET): talk to it and
 * pick a minigame.
 *
 * With nobody talking, Behavior offers talk to a colliding Mario (actor
 * 0xbf) standing within a quarter turn of its front. With a talker, it
 * turns them (mAngleY) to face the cupboard, then runs message 0xb09
 * through the save/menu script (data_0209d684 picks: finish talking, show
 * saving, or open the minigame menu). InitResources places all five
 * colliders at mPosX (radius 110.0, height 140.0); Behavior spreads them
 * every frame, 100 units apart along the cupboard's width, centred on it.
 *
 * DO NOT "TIDY" THESE -- each one is load-bearing:
 *
 *   selfPos, frontPos and playerPos are written and never read; volatile
 *   keeps their stores, which the ROM has.
 *
 * Known limits:
 *   _Z14ApproachLinearRsss / Vec3_HorzAngle / AngleDiff keep linker names:
 *   no header declares them (kaitendai keeps ApproachLinear mangled too).
 *   data_02082214 is the shared arm9 sine table.
 *   data_0209d684 / data_0209d660 are save/menu state bytes; only this
 *   script reads them, so they stay address-named.
 */

#include "daObjCloset_c.h"
#include "Player.h"
#include "Message.h"

/* File-local POD triple: include/types.h's Vector3 declares an (empty)
   destructor, because the ROM destroys arrays of it, and the five locals
   below grow the frame by 0x10 bytes under a non-POD type. */
struct Vec3Raw { int x, y, z; };

extern "C" {
void func_02012790(int a);
int _Z14ApproachLinearRsss(short* cur, short target, short step);
short Vec3_HorzAngle(const struct Vec3Raw* a, const struct Vec3Raw* b);
int AngleDiff(int a, int b);
void StartMinigameMenu(unsigned char a);
void* _ZN8dActor_c10FindWithIDEj(unsigned int id);
void _ZN5dCc_c5ClearEv(void* self);
void _ZN5dCc_c6UpdateEv(void* self);
extern s16 data_02082214[];
extern u8 data_0209d684;
extern u8 data_0209d660;
extern "C" void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj( void *self, void *actor, void *offset, int radius, int height, unsigned int flags, unsigned int vulnFlags);
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
    Player* target = mTalkingPlayer;

    if (target) {
        selfPos.x = *(volatile s32*)&mPosX;
        selfPos.y = *(volatile s32*)&mPosY;
        selfPos.z = *(volatile s32*)&mPosZ;
        frontPos.x = mPosX;
        frontPos.y = mPosY;
        frontPos.z = mPosZ;
        frontPos.x = mPosX + (int)(((s64)0x5a000 * data_02082214[(*(volatile u16*)&mAngleY >> 4) * 2] + 0x800) >> 12);
        frontPos.z = mPosZ + (int)(((s64)0x5a000 * data_02082214[(*(volatile u16*)&mAngleY >> 4) * 2 + 1] + 0x800) >> 12);
        {
            struct Vec3Raw* tp = (struct Vec3Raw*)&target->mPosX;
            playerPos.x = tp->x;
            playerPos.y = tp->y;
            playerPos.z = tp->z;
        }

        switch (target->GetTalkState()) {
        case 0: {
            short cur = (short)(mAngleY + 0x8000);
            if (_Z14ApproachLinearRsss(&target->mAngleY, cur, 0x800) != 0) {
                mMessageID = 0xb09;
                Message::PrepareTalk();
                target->ShowMessage2(*this, (s16)mMessageID, 0, 1, 0);
            }
            break;
        }
        case 1:
            break;
        default: {
            u8 st = data_0209d684;
            if (mMessageID == 0xb09) {
                if (st == 1) {
                    if (mSoundStarted == 0) {
                        func_02012790(0x98);
                        mSoundStarted = 1;
                    }
                    if (target->HasFinishedTalking() != 0) {
                        Message::EndTalk();
                        mTalkingPlayer = 0;
                        mSoundStarted = 0;
                    }
                } else if (st == 2) {
                    if (mSoundStarted == 0) {
                        func_02012790(0x97);
                        mSoundStarted = 1;
                    }
                    Message::DisplaySaving(0x295);
                } else {
                    if (data_0209d660 == 0) {
                        StartMinigameMenu(1);
                        mTalkingPlayer = 0;
                        mSoundStarted = 0;
                    }
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
        for (i = 0; i < 5; i++) {
            if (mColliders[i].hitFlags & 0x8000000) {
                u32 id = mColliders[i].otherOwner;
                dActor_c* actor = (dActor_c*)_ZN8dActor_c10FindWithIDEj(id);
                if (actor) {
                    int isMatch = (actor->actorID == 0xbf);
                    if (isMatch != false) {
                        struct Vec3Raw* ap = (struct Vec3Raw*)&actor->mPosX;
                        short angleToActor;
                        int diff;
                        actorPos.x = ap->x;
                        actorPos.y = ap->y;
                        actorPos.z = ap->z;
                        angleToActor = Vec3_HorzAngle((struct Vec3Raw*)&mPosX, &actorPos);
                        diff = AngleDiff(angleToActor, mAngleY);
                        if (diff < 0x4000) {
                            if (((Player*)actor)->StartTalk(*this, 0) != 0) {
                                mTalkingPlayer = (Player*)actor;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    {
        int j;
        dCcAcPos_c* cyl = mColliders;
        for (j = 0; j < 5; j++) {
            colliderPos.x = mPosX;
            colliderPos.y = mPosY;
            colliderPos.z = mPosZ;
            {
                int distFixed = ((2 - j) * 100) << 12;
                /* sin and cos of the side direction, a quarter turn from
                   mAngleY: sideSin is cos(mAngleY), sideCos is
                   -sin(mAngleY). */
                int idx = (unsigned short)(short)(mAngleY + 0x4000) >> 4;
                s16 sideSin = data_02082214[idx * 2];
                s16 sideCos = data_02082214[idx * 2 + 1];
                int offX = (int)(((s64)distFixed * sideSin + 0x800) >> 12);
                int offZ = (int)(((s64)distFixed * sideCos + 0x800) >> 12);
                int newX = mPosX + offX;
                int newZ = mPosZ + offZ;
                colliderPos.x = newX;
                colliderPos.z = newZ;
                mColliders[j].pos.x = newX;
                mColliders[j].pos.y = colliderPos.y;
                mColliders[j].pos.z = colliderPos.z;
            }
            _ZN5dCc_c5ClearEv(cyl);
            _ZN5dCc_c6UpdateEv(cyl);
            cyl++;
        }
    }

    return 1;
}

// @symbol _ZN13daObjCloset_c13InitResourcesEv
/* dCcAcPos_c::Init is CALLED, not defined, through its mangled name with int
 * parameters: its ROM name carries by-value Fix12<int> arguments, which
 * mwccarm passes differently from a plain int at the call site, so spelling
 * the true types breaks the byte match (notes/mwccarm-codegen.md 6az). */
s32 daObjCloset_c::InitResources()
{
    int i;
    dCcAcPos_c *collider = mColliders;
    for (i = 0; i < 5; i++) {
        _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
            collider, this, &mPosX, 0x6e000, 0x8c000, 0x4800002, 0);
        collider++;
    }
    return 1;
}
