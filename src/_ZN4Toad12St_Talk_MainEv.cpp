//cpp
// @symbol _ZN4Toad12St_Talk_MainEv
#pragma opt_propagation off
#include "Toad.h"

extern "C" {
extern s16 Vec3_HorzAngle(const Vector3 *, const Vector3 *);
extern int _ZN6Player12GetTalkStateEv(char *);
extern int _Z14ApproachLinearRsss(s16 *, s16, s16);
extern int _ZN5Sound7PlaySubEjjj5Fix12IiEb(u32, u32, u32, int, int);
extern void _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(
    char *, char *, u32, const Vector3 *, u32, u32);
extern int IsStarCollectedInCurLevel();
extern u8 NumStars();
extern char *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
    u32, u32, const void *, const void *, int, int);
extern int func_02013a44();
extern char *_ZN8dActor_c10FindWithIDEj(u32);
extern void _ZN7fBase_c18MarkForDestructionEv(char *);
extern void _ZN8dActor_c13SpawnSoundObjEj(char *, u32);
extern void _ZN7Message7EndTalkEv();
extern s8 data_0209f2f8;
extern int data_0209caa0[];
extern u8 data_ov085_0212f27c[];
}

#define M(p) (p)

void Toad::St_Talk_Main()
{
    char *c = (char *)this;
    char *player;
    int messageID;
    s16 angle;
    Vector3 playerPos;
    Vector3 messagePos;
    Vector3 starPos;
    int talkState;

    player = (char *)mTalkPlayer;
    messageID = GetMessageID();
    {
        int *pos = (int *)(int)M(player + 0x5c);
        playerPos.x = pos[0];
        playerPos.y = pos[1];
        playerPos.z = pos[2];
    }
    angle = Vec3_HorzAngle((Vector3 *)(c + 0x5c), &playerPos);
    talkState = _ZN6Player12GetTalkStateEv(player);
    switch (talkState) {
    case 0: {
        if (_Z14ApproachLinearRsss(&mAngleY, angle, 0x800) == 0)
            return;
        if (data_0209f2f8 == 0x32) {
            if ((data_0209caa0[1] & 0x40000000) == 0)
                data_0209caa0[1] |= 0x40000000;
        }
        if (_ZN5Sound7PlaySubEjjj5Fix12IiEb(
                0x25, 0x14, 0x7f, 0x15666, 1) == 0)
            return;
        {
            int y = mPosY;
            int z = mPosZ;
            int x;
            y += 0x46000;
            messagePos.x = mPosX;
            messagePos.y = y;
            messagePos.z = z;
            (void)x;
        }
        _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(
            player, c, (u32)(s16)messageID, &messagePos, 0, 0);
        return;
    }
    case 1:
        return;
    default:
        break;
    }

    if (mTalkFinished == 0) {
        mTalkFinished = 1;
        if (mStarID != 0xff && IsStarCollectedInCurLevel() == 0) {
            if (NumStars() >= data_ov085_0212f27c[mStarReqIndex]) {
                starPos.x = mPosX;
                starPos.y = mPosY;
                starPos.z = mPosZ;
                starPos.y += 0xc8000;
                _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
                    0xb2, mStarID | 0x20, &starPos, 0, mAreaId, -1);
                u16 *message = (u16 *)(int)M(c + 0x208);
                *message = (u16)(*message + 1);
            }
        }
    }

    if (mVariant == 1 && func_02013a44() != 0) {
        u32 id = mCapUniqueID;
        if (id != 0) {
            char *found = _ZN8dActor_c10FindWithIDEj(id);
            if (found != 0) {
                _ZN7fBase_c18MarkForDestructionEv(found);
                mCapUniqueID = 0;
                _ZN8dActor_c13SpawnSoundObjEj(c, 1);
                {
                    u32 param = 0x13;
                    u8 character = *(u8 *)(player + 0x6d9);
                    _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
                        0x10d, param | ((u32)character << 8),
                        &playerPos, 0, mAreaId, -1);
                }
            }
        }
    }

    if (_ZN5Sound7PlaySubEjjj5Fix12IiEb(0x25, 0x7f, 0, 0x7222, 0) == 0)
        return;
    _ZN7Message7EndTalkEv();
    SetState(0);
}
