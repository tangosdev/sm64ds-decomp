//cpp
// @symbol _ZN16MeshColliderBase10BeforeClsnER10ClsnResultP5ActorR7Vector3P10Vector3_16S7_
#include "MeshColliderBase.h"

/* Note the argument order swap: the callback receives (self, actor, res, ...)
   while the virtual receives (res, actor, ...). */
void MeshColliderBase::BeforeClsn(ClsnResult &res, Actor *actor, Vector3 &pos,
                                  Vector3_16 *motionAng, Vector3_16 *ang)
{
    beforeClsnCallback(this, actor, &res, &pos, motionAng, ang);
}
