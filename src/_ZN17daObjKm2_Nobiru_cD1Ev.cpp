//cpp
// @symbol _ZN17daObjKm2_Nobiru_cD1Ev
/* Force mwccarm to emit the class-body destructor as a genuine C++ D1. The
 * destructor itself is inline in the header; this scaffold exists only so the
 * per-function delink build still produces the symbol at 0x02111840, and it
 * goes away when the TU is promoted. */
#include "daObjKm2_Nobiru_c.h"

void daObjKm2_Nobiru_c_EmitDestructor(daObjKm2_Nobiru_c *platform)
{
    platform->~daObjKm2_Nobiru_c();
}
