//cpp
// @symbol _ZN16MeshColliderBase25UpdateAngsWithAngularVelYERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_
#include "MeshColliderBase.h"

void MeshColliderBase::UpdateAngsWithAngularVelY(MeshColliderBase &clsn, Actor *clsnActor,
                                                 ClsnResult &res, Vector3 &pos,
                                                 Vector3_16 *motionAng, Vector3_16 *ang)
{
    int angY = clsn.GetAngularVelY();
    if (ang) {
        short *py = (short *)(((long long)(int)&ang->y));
        *py = *py + angY;
    }
    if (motionAng) {
        short *py = (short *)(((long long)(int)&motionAng->y));
        *py = *py + angY;
    }
}
