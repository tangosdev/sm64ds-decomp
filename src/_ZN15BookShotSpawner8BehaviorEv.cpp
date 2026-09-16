//cpp
#include "dActor_c.h"
#include "BookShotSpawner.h"
#include "Player.h"

extern "C" {
extern int Vec3_HorzDist(const Vector3 *a, const Vector3 *b);
extern short Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
}

int BookShotSpawner::Behavior()
{
    if (mSpawnTimer > 0x28) {
        Player *player = ClosestPlayer();
        if (player != 0) {

            Vector3 tmp;
            Vector3 *ps = (Vector3 *)&player->mPosX;
            tmp = *ps;

            if (Vec3_HorzDist((Vector3 *)&mPosX, &tmp) < 0x258000) {
                short angle = Vec3_HorzAngle((Vector3 *)&mPosX, &tmp);
                if (((dActor_c *)this)->GetSubtraction(mAngleY, angle) < 0x2000) {
                    signed char sc = mAreaId;
                    dActor_c::Spawn(0x145, 0, *(const Vector3 *)&mPosX, (const Vector3_16 *)&mPrevAngleX, sc, -1);
                    mSpawnTimer = 0;
                }
            }
        }
    } else {
        u16 *timer = &mSpawnTimer;
        *timer = *timer + 1;
    }
    return 1;
}
