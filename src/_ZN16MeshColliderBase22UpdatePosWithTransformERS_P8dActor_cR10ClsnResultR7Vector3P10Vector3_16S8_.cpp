//cpp
// @symbol _ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_
#include "MeshColliderBase.h"

void MeshColliderBase::UpdatePosWithTransform(MeshColliderBase &clsn, Actor *clsnActor,
                                              ClsnResult &res, Vector3 &pos,
                                              Vector3_16 *motionAng, Vector3_16 *ang)
{
    Vector3 local;
    local.x = pos.x;
    local.y = pos.y;
    local.z = pos.z;
    if (clsn.TransformPos(local, pos) == 0)
        return;
    return;
}
