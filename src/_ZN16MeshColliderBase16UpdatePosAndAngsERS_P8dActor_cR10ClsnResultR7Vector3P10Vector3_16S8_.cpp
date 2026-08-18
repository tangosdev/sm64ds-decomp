//cpp
// @symbol _ZN16MeshColliderBase16UpdatePosAndAngsERS_P8dActor_cR10ClsnResultR7Vector3P10Vector3_16S8_
#include "MeshColliderBase.h"

void MeshColliderBase::UpdatePosAndAngs(MeshColliderBase &clsn, dActor_c *clsnActor,
                                        ClsnResult &res, Vector3 &pos,
                                        Vector3_16 *motionAng, Vector3_16 *ang)
{
    UpdatePosWithTransform(clsn, clsnActor, res, pos, motionAng, ang);
    UpdateAngsWithAngularVelY(clsn, clsnActor, res, pos, motionAng, ang);
}
