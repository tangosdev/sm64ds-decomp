//cpp
// @symbol _ZN16MeshColliderBase21UpdatePosWithVelocityERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_
#include "MeshColliderBase.h"

void MeshColliderBase::UpdatePosWithVelocity(MeshColliderBase &clsn, Actor *clsnActor,
                                             ClsnResult &res, Vector3 &pos,
                                             Vector3_16 *motionAng, Vector3_16 *ang)
{
    Vector3 vel;
    clsn.GetVelocity(vel);
    pos.x = pos.x + vel.x;
    {
        /* launder: the ROM re-reads through the materialized addresses */
        int *py = (int *)(((long long)(int)&pos.y));
        *py = *py + vel.y;
    }
    {
        int *pz = (int *)(((long long)(int)&pos.z));
        *pz = *pz + vel.z;
    }
}
