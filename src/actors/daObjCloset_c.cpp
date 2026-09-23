//cpp
/**
 * Break room cupboard (PL_CLOSET): talk to it, pick a minigame.
 *
 * With no player nearby, Behavior offers each colliding Mario to
 * talk; with one talking, it runs the message/save/minigame-menu
 * script by talk state. InitResources lays the five colliders along
 * the facing in 100-unit steps and updates them every frame.
 *
 * deslop
 * Leftover: _Z14ApproachLinearRsss / Vec3_HorzAngle / AngleDiff keep
 *   linker names (no header home; kaitendai precedent keeps
 *   ApproachLinear mangled too).
 * Leftover: data_02082214 is the shared arm9 sine table.
 * Leftover: data_0209d684 / data_0209d660 are save/menu state bytes;
 *   only this script reads them, so they stay address-named.
 * Leftover: target + 0x8e is a Player short with no named header
 *   field.
 * Leftover: the base C2 is called with no argument, as the ROM does.
 * Leftover: v1 is written never read; volatile keeps the stores.
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
/* The scratch position triples below are deliberately NOT include/types.h's
   Vector3: that one declares an (empty) destructor, because the ROM destroys
   arrays of it, and the five locals here grow the frame by 0x10 bytes under a
   non-POD type. A file-local POD triple is what the ROM's frame says was
   here. */
s32 daObjCloset_c::Behavior()
{
    volatile Vec3Raw v1, v2, v3;
    Vec3Raw apos;
    volatile Vec3Raw pos;
    Player* target = mTalkingPlayer;

    if (target) {
        v1.x = *(volatile s32*)&mPosX;
        v1.y = *(volatile s32*)&mPosY;
        v1.z = *(volatile s32*)&mPosZ;
        v2.x = mPosX;
        v2.y = mPosY;
        v2.z = mPosZ;
        v2.x = mPosX + (int)(((s64)0x5a000 * data_02082214[(*(volatile u16*)&mAngleY >> 4) * 2] + 0x800) >> 12);
        v2.z = mPosZ + (int)(((s64)0x5a000 * data_02082214[(*(volatile u16*)&mAngleY >> 4) * 2 + 1] + 0x800) >> 12);
        {
            struct Vec3Raw* tp = (struct Vec3Raw*)&target->mPosX;
            v3.x = tp->x;
            v3.y = tp->y;
            v3.z = tp->z;
        }

        switch (target->GetTalkState()) {
        case 0: {
            short cur = (short)(mAngleY + 0x8000);
            if (_Z14ApproachLinearRsss((short*)((char*)target + 0x8e), cur, 0x800) != 0) {
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
                        short ang;
                        int diff;
                        apos.x = ap->x;
                        apos.y = ap->y;
                        apos.z = ap->z;
                        ang = Vec3_HorzAngle((struct Vec3Raw*)&mPosX, &apos);
                        diff = AngleDiff(ang, mAngleY);
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
            pos.x = mPosX;
            pos.y = mPosY;
            pos.z = mPosZ;
            {
                int distFixed = ((2 - j) * 100) << 12;
                int idx = (unsigned short)(short)(mAngleY + 0x4000) >> 4;
                s16 cosv = data_02082214[idx * 2];
                s16 sinv = data_02082214[idx * 2 + 1];
                int offX = (int)(((s64)distFixed * cosv + 0x800) >> 12);
                int offZ = (int)(((s64)distFixed * sinv + 0x800) >> 12);
                int newX = mPosX + offX;
                int newZ = mPosZ + offZ;
                pos.x = newX;
                pos.z = newZ;
                mColliders[j].pos.x = newX;
                mColliders[j].pos.y = pos.y;
                mColliders[j].pos.z = pos.z;
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
