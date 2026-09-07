//cpp
// @symbol _ZN21SnowmanBreathParticle8TrySpawnER6Player
#include "SnowmanBreath.h"

extern "C" {
s16 Vec3_HorzAngle(const Vector3 *, const Vector3 *);
s32 RandomIntInternal(s32 *);
extern Vector3 data_ov027_02113d10;
extern s32 data_0209e650;
}

int SnowmanBreathParticle::TrySpawn(Player &player)
{
    s32 angle;
    if (mTimer != 0)
        return 0;
    mPos = data_ov027_02113d10;
    angle = Vec3_HorzAngle(&mPos, (Vector3 *)((char *)&player + 0x5c));
    if (angle > 0x2af8)
        angle = 0x2af8;
    if (angle < 0x9de)
        angle = 0x9de;
    {
        s32 random = RandomIntInternal(&data_0209e650);
        mAngleX = 0xc1c;
        mAngleY = (s16)(angle + ((random & 0x1fff) - 0x1000));
    }
    mAngleZ = 0;
    mTimer = 0x16;
    return 1;
}
