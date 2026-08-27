//cpp
// @symbol _ZN5Cloud8BehaviorEv

#include "Cloud.h"

extern "C" {
extern int Vec3_Dist(const Vector3* a, const Vector3* b);
}

#pragma opt_propagation off
int Cloud::Behavior()
{
    int d = Vec3_Dist((const Vector3*)&mCamSpacePosX, (const Vector3*)&mPosX);
    int rank = 1;
    int base = 2;
    dActor_c *actor = FindWithActorID(0x13a, 0);
    while (actor) {
        if (actor != this) {
            int d2 = Vec3_Dist((const Vector3*)&mCamSpacePosX, (const Vector3*)&actor->mPosX);
            if (d > d2)
                rank++;
        }
        actor = FindWithActorID(0x13a, actor);
    }
    mModel.SetPolygonID((base + rank) & 0xff);
    return 1;
}
