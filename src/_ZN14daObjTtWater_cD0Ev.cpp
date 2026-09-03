//cpp
// @symbol _ZN14daObjTtWater_cD0Ev
/* A delete expression forces the compiler-spelled deleting destructor.
 *
 * D0 destroys through this class and its bases -- which is why more than one
 * vptr store appears -- then returns the object to its heap through an inlined
 * operator delete. Nobody writes that body; the inline declaration in
 * include/daObjTtWater_c.h plus this one use is the whole of it. The file goes
 * away when the class is promoted to a single translation unit.
 */
#include "daObjTtWater_c.h"

void daObjTtWater_c_EmitDeletingDestructor(daObjTtWater_c *water) { delete water; }
