//cpp
// @symbol _ZN14daObjTtWater_cD1Ev
/* Force mwccarm to emit the class-body destructor as a genuine C++ D1.
 *
 * The destructor is inline in include/daObjTtWater_c.h -- that is what keeps
 * D1 ahead of D0 in ROM order and stops mwcc emitting a homeless D2 -- so this
 * file cannot define it. It INSTANTIATES it instead: an explicit destructor
 * call is enough to make the compiler emit the out-of-line copy this delink
 * entry is bound to. The file goes away when the class is promoted to a single
 * translation unit.
 */
#include "daObjTtWater_c.h"

void daObjTtWater_c_EmitDestructor(daObjTtWater_c *water) { water->~daObjTtWater_c(); }
