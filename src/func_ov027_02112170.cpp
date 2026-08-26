//cpp
// @symbol _ZN21SnowmanBreathParticle9HitPlayerEv
#include "SnowmanBreath.h"
#include "decl_Player.h"

extern "C" {
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32, s32, s32, s32);
int _ZN6Player15IsCollectingCapEv(void *);
s16 Vec3_HorzAngle(const Vector3 *, const Vector3 *);
void _ZN6Player18SetNewHatCharacterEjjb(void *, u32, u32, int);
int _ZN8SaveData16HasPlayerLostCapEv();
void _ZN8SaveData13PlayerLoseCapEv();
extern Vector3 data_ov027_02113d10;
}

void SnowmanBreathParticle::HitPlayer()
{
    u32 id = mCollider.otherOwner;
    if (id == 0)
        return;
    dActor_c *actor = dActor_c::FindWithID(id);
    if (actor == 0)
        return;
    if ((int)(actor->actorID == 0xbf) == 0)
        return;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
        0x100, mPos.x, mPos.y, mPos.z);

    char *player = (char *)actor;
    if (*(u8 *)(player + 0x6f9))
        return;
    if (*(u8 *)(player + 0x703))
        return;
    if (*(u8 *)(player + 0x6fd))
        return;
    _ZN6Player8BlowAwayEs(player, mAngleY);
    if (_ZN6Player15IsCollectingCapEv(player))
        return;
    if (*(u8 *)(player + 0x6ff))
        return;
    if (*(u8 *)(player + 0x6fb))
        return;

    s32 angle = Vec3_HorzAngle(&data_ov027_02113d10,
                               (Vector3 *)&actor->mPosX);
    Vector3_16 rotation;
    rotation.x = 0;
    rotation.y = (s16)angle;
    rotation.z = 0;
    if (*(u8 *)(player + 0x6d9) != actor->param1) {
        _ZN6Player18SetNewHatCharacterEjjb(
            player, *(u8 *)(player + 0x6d9), 0, 0);
    } else {
        if (_ZN8SaveData16HasPlayerLostCapEv())
            return;
        _ZN8SaveData13PlayerLoseCapEv();
    }

    Vector3 spawnPos;
    {
        s32 posX = mPos.x;
        s32 posZ = mPos.z;
        s32 posY = mPos.y + 0x96000;
        spawnPos.x = posX;
        spawnPos.y = posY;
        spawnPos.z = posZ;
    }
    dActor_c *cap = dActor_c::Spawn(
        0x10d, (actor->param1 << 8) | 3, spawnPos, &rotation, 0, -1);
    if (cap == 0)
        return;
    cap->unk_0a4 = 0;
    cap->mVertSpeed = 0x14000;
    cap->unk_0ac = 0;
    cap->mHorzSpeed = 0x1c000;
}
