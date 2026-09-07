//cpp
// @symbol _ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_
#include "dBgW.h"

void dBgW::UpdatePosAndAngs(dBgW &clsn, dActor_c *clsnActor,
                                        dBgPi &res, Vector3 &pos,
                                        Vector3_16 *motionAng, Vector3_16 *ang)
{
    UpdatePosWithTransform(clsn, clsnActor, res, pos, motionAng, ang);
    UpdateAngsWithAngularVelY(clsn, clsnActor, res, pos, motionAng, ang);
}
