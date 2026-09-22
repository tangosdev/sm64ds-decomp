//cpp
#include "common.h"
#include "Bully.h"
#include "BigBully.h"

extern "C" {
int RandomIntInternal(void *seed);
extern int data_0209e650;
int func_ov064_0211616c(void *self);
}

void Bully::UpdateDeathState()
{
    if (func_ov064_0211616c(this) == 0)
        return;
    int pz = mPosZ;
    int py = mPosY + 0x136000;
    int px = mPosX;
    Vector3 pos;
    pos.x = px;
    pos.y = py;
    pos.z = pz;
    int r = RandomIntInternal(&data_0209e650);
    Vector3_16 rot;
    s16 ang = (s16)(mPrevAngleY + 0x8000 + (((u32)r >> 8) & 0x3ff));
    rot.x = 0;
    rot.z = 0;
    rot.y = ang;
    dActor_c *coin = dActor_c::Spawn(0x120, 2, pos, &rot, mAreaId, -1);
    if (coin) {
        coin->mHorzSpeed = 0xa000;
        coin->unk_0a4 = 0;
        coin->mVertSpeed = 0x50000;
        coin->unk_0ac = 0;
    }
    dActor_c *spawner = dActor_c::FindWithID(mBigBullyID);
    if (spawner) {
        BigBully *bigBully = (BigBully *)spawner;
        bigBully->mNumBulliesKilled++;
    }
}
