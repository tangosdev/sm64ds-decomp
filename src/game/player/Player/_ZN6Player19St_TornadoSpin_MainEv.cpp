//cpp
// @symbol _ZN6Player19St_TornadoSpin_MainEv
/* recovered: named members + shared header, real C++ method
 *
 * Being carried up a tornado. The player orbits mAttachedActor: the horizontal
 * offset is rotated by an ever-increasing mAngleYSpeed while mAttachOffsetY
 * climbs at mVertSpeed. Leaving happens at the tornado's own height limit
 * (+0xdc on it) or on a collision bit. A dBgCh_Gnd from 0x64000 above the
 * candidate position decides whether the player is placed there or clamped to
 * mGroundY.
 */
#include "Player.h"

struct dBgCh_Gnd { int filler[21]; };

extern "C" {
extern void _ZN6Player11ChangeStateERNS_5StateE(void* self, int state);
extern void _ZN9dBgCh_GndC1Ev(struct dBgCh_Gnd* self);
extern void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(struct dBgCh_Gnd* self, const Vector3* pos, void* actor);
extern int _ZN9dBgCh_Gnd10DetectClsnEv(struct dBgCh_Gnd* self);
extern void Player_AdvanceAnims(void* self);
extern void _ZN9dBgCh_GndD1Ev(struct dBgCh_Gnd* self);
extern int data_ov002_02110454[];
extern short data_02082214[];
}

int Player::St_TornadoSpin_Main()
{
    Vector3 pos;
    Vector3 clsnPos;
    struct dBgCh_Gnd rc;
    int dx = mPosX - ((dActor_c*)mAttachedActor)->mPosX;
    int dz = mPosZ - ((dActor_c*)mAttachedActor)->mPosZ;
    int sx = (int)((dx * 0xf33LL + 0x800) >> 12);
    int sz = (int)((dz * 0xf33LL + 0x800) >> 12);
    int spd = mVertSpeed;
    if (spd < 0) {
        mVertSpeed = 0x1000;
    } else if (spd < 0x3c000) {
        mVertSpeed += 0x1000;
    }
    mAttachOffsetY += mVertSpeed;
    if (mAttachOffsetY < 0)
        mAttachOffsetY = 0;
    /* +0xdc on the tornado is past sizeof(dActor_c); it belongs to whatever
       concrete actor class this is, which nothing here evidences. */
    if (mAttachOffsetY > *(int*)((char*)mAttachedActor + 0xdc) || (mClsnFlags & 4) != 0) {
        mStatePhase = 0;
        _ZN6Player11ChangeStateERNS_5StateE(this, (int)data_ov002_02110454);
        if (mVertSpeed < 0x14000)
            mVertSpeed = 0x14000;
        return 1;
    }
    if (mAngleYSpeed < 0x3000)
        mAngleYSpeed += 0x100;
    mAngleY += mAngleYSpeed;
    {
        int v = mAngleYSpeed;
        if (v >= 0x1000) v = 0x1000;
        {
            int k = (u16)v >> 4;
            int sinv = data_02082214[k * 2 + 1];
            int cosv = data_02082214[k * 2];
            ((int*)&pos)[0] = (int)(((s64)sz * cosv + 0x800) >> 12) + (((dActor_c*)mAttachedActor)->mPosX + (int)(((s64)sx * sinv + 0x800) >> 12));
            ((int*)&pos)[2] = (int)(((s64)sz * sinv + 0x800) >> 12) + (((dActor_c*)mAttachedActor)->mPosZ - (int)(((s64)sx * cosv + 0x800) >> 12));
            ((int*)&pos)[1] = mAttachOffsetY + ((dActor_c*)mAttachedActor)->mPosY;
        }
    }
    _ZN9dBgCh_GndC1Ev(&rc);
    clsnPos = pos;
    clsnPos.y += 0x64000;
    _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(&rc, &clsnPos, this);
    if (_ZN9dBgCh_Gnd10DetectClsnEv(&rc) != 0) {
        mPosX = pos.x;
        mPosY = pos.y;
        mPosZ = pos.z;
    } else {
        if (mPosY >= mGroundY)
            mPosY = pos.y;
        else
            mPosY = mGroundY;
    }
    Player_AdvanceAnims(this);
    _ZN9dBgCh_GndD1Ev(&rc);
    return 1;
}
