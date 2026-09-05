//cpp
// @symbol _ZN12daStarGate_c26CalculateRelativePlayerPosEv
#include "daStarGate_c.h"

#include "Player.h"

extern "C" void Vec3_Sub(Vector3* out, Vector3* a, Vector3* b);
extern "C" void Vec3_RotateYAndTranslate(Vector3* out, void* m, int ang, Vector3* in);

extern unsigned char data_0209f250;
extern Player* data_0209f394[];
extern char data_020a0ebc[];

Player *daStarGate_c::CalculateRelativePlayerPos()
{
    Vector3 tmp;
    Player* player = data_0209f394[data_0209f250];
    Vec3_Sub(&tmp, (Vector3*)&player->mPosX, (Vector3*)&mPosX);
    Vec3_RotateYAndTranslate((Vector3*)&mScaleX, data_020a0ebc,
                             (short)-mAngleY, &tmp);
    return player;
}
