//cpp
// @symbol _ZN17daObjKm2_Nobiru_cD0Ev
/* A delete expression forces the compiler-spelled deleting destructor. Same
 * scaffold role as the D1 file next to it. */
#include "daObjKm2_Nobiru_c.h"

void daObjKm2_Nobiru_c_EmitDeletingDestructor(daObjKm2_Nobiru_c *platform)
{
    delete platform;
}
