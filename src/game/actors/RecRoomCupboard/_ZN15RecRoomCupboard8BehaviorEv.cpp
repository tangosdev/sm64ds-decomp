//cpp
// @symbol _ZN15RecRoomCupboard8BehaviorEv
/* recovered: real class form -- the compiler spells the mangled name. */
#include "RecRoomCupboard.h"

typedef long long s64;

/* The scratch position triples below are deliberately NOT include/types.h's
   Vector3: that one declares an (empty) destructor, because the ROM destroys
   arrays of it, and the five locals here grow the frame by 0x10 bytes under a
   non-POD type. A file-local POD triple is what the ROM's frame says was
   here. */
struct Vec3Raw { int x, y, z; };

extern "C" {
int _ZN6Player12GetTalkStateEv(void* self);
int _Z14ApproachLinearRsss(short* cur, short target, short step);
void _ZN7Message11PrepareTalkEv(void);
void _ZN6Player12ShowMessage2ER7fBase_cjPK7Vector3hh(void* self, void* actor, unsigned int msg, const void* vec, unsigned int a, unsigned int b);
void func_02012790(int a);
int _ZN6Player18HasFinishedTalkingEv(void* self);
void _ZN7Message7EndTalkEv(void);
void _ZN7Message13DisplaySavingEt(unsigned short a);
void StartMinigameMenu(unsigned char a);
void* _ZN8dActor_c10FindWithIDEj(unsigned int id);
short Vec3_HorzAngle(const struct Vec3Raw* a, const struct Vec3Raw* b);
int AngleDiff(int a, int b);
int _ZN6Player9StartTalkER7fBase_cb(void* actor, void* self, int b);
void _ZN5dCc_c5ClearEv(void* self);
void _ZN5dCc_c6UpdateEv(void* self);
}

extern s16 data_02082214[];
extern u8 data_0209d684;
extern u8 data_0209d660;

#pragma opt_strength_reduction off
s32 RecRoomCupboard::Behavior()
{
    volatile Vec3Raw v1, v2, v3;
    Vec3Raw apos;
    volatile Vec3Raw pos;
    char* target = (char*)mTalkingPlayer;

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
            struct Vec3Raw* tp = (struct Vec3Raw*)((int)target + 0x5c);
            v3.x = tp->x;
            v3.y = tp->y;
            v3.z = tp->z;
        }

        switch (_ZN6Player12GetTalkStateEv(target)) {
        case 0: {
            short cur = (short)(mAngleY + 0x8000);
            if (_Z14ApproachLinearRsss((short*)(target + 0x8e), cur, 0x800) != 0) {
                mMessageID = 0xb09;
                _ZN7Message11PrepareTalkEv();
                _ZN6Player12ShowMessage2ER7fBase_cjPK7Vector3hh(
                    target, this, (s16)mMessageID, 0, 1, 0);
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
                    if (_ZN6Player18HasFinishedTalkingEv(target) != 0) {
                        _ZN7Message7EndTalkEv();
                        mTalkingPlayer = 0;
                        mSoundStarted = 0;
                    }
                } else if (st == 2) {
                    if (mSoundStarted == 0) {
                        func_02012790(0x97);
                        mSoundStarted = 1;
                    }
                    _ZN7Message13DisplaySavingEt(0x295);
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
                char* actor = (char*)_ZN8dActor_c10FindWithIDEj(id);
                if (actor) {
                    int isMatch = (*(u16*)(actor + 0xc) == 0xbf);
                    if (isMatch != false) {
                        struct Vec3Raw* ap = (struct Vec3Raw*)((int)actor + 0x5c);
                        short ang;
                        int diff;
                        apos.x = ap->x;
                        apos.y = ap->y;
                        apos.z = ap->z;
                        ang = Vec3_HorzAngle((struct Vec3Raw*)&mPosX, &apos);
                        diff = AngleDiff(ang, mAngleY);
                        if (diff < 0x4000) {
                            if (_ZN6Player9StartTalkER7fBase_cb(actor, this, 0) != 0) {
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
