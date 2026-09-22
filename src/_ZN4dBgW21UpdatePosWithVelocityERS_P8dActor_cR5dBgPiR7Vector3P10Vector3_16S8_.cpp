//cpp
// @symbol _ZN4dBgW21UpdatePosWithVelocityERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_
#include "dBgW.h"

void dBgW::UpdatePosWithVelocity(dBgW &clsn, dActor_c *clsnActor,
                                             dBgPi &res, Vector3 &pos,
                                             Vector3_16 *motionAng, Vector3_16 *ang)
{
    Vector3 vel;
    clsn.GetVelocity(vel);
    pos.x = pos.x + vel.x;
    {
        /* launder: the ROM re-reads through the materialized addresses */
        int *py = (int *)(&pos.y);
        *py = *py + vel.y;
    }
    {
        int *pz = (int *)(&pos.z);
        *pz = *pz + vel.z;
    }
}
