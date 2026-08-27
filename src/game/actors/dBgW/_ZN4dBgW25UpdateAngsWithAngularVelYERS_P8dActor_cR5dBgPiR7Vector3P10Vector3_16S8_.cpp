//cpp
// @symbol _ZN4dBgW25UpdateAngsWithAngularVelYERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_
#include "dBgW.h"

void dBgW::UpdateAngsWithAngularVelY(dBgW &clsn, dActor_c *clsnActor,
                                                 dBgPi &res, Vector3 &pos,
                                                 Vector3_16 *motionAng, Vector3_16 *ang)
{
    int angY = clsn.GetAngularVelY();
    if (ang) {
        short *py = (short *)(&ang->y);
        *py = *py + angY;
    }
    if (motionAng) {
        short *py = (short *)(&motionAng->y);
        *py = *py + angY;
    }
}
