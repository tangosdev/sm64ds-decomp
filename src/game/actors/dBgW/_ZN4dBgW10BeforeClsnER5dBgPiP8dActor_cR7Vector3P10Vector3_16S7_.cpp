//cpp
// @symbol _ZN4dBgW10BeforeClsnER5dBgPiP8dActor_cR7Vector3P10Vector3_16S7_
#include "dBgW.h"

/* Note the argument order swap: the callback receives (self, actor, res, ...)
   while the virtual receives (res, actor, ...). */
void dBgW::BeforeClsn(dBgPi &res, dActor_c *actor, Vector3 &pos,
                                  Vector3_16 *motionAng, Vector3_16 *ang)
{
    beforeClsnCallback(this, actor, &res, &pos, motionAng, ang);
}
