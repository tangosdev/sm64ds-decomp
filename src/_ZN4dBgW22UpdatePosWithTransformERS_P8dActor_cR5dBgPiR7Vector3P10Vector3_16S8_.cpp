//cpp
// @symbol _ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR10ClsnResultR7Vector3P10Vector3_16S8_
#include "dBgW.h"

void dBgW::UpdatePosWithTransform(dBgW &clsn, dActor_c *clsnActor,
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
