//cpp
// @symbol _ZN5dBgPcC2Ev
/* recovered: real C++ base-object constructor
 *
 * The same five stores the C1 file makes -- dBgPc is a non-polymorphic base,
 * so there is no vptr and the two variants are byte-identical in the ROM.
 * The identical body in both files is not duplication: C1 and C2 are two of
 * the functions the compiler emits from one constructor, and each file is
 * bound to one of them by config/arm9/delinks.txt.
 */
#include "dBgPc.h"

dBgPc::dBgPc()
{
    surface.clps.w0 = 0xfc0;
    surface.clps.w1 = 0xff;
    surface.normal.z = 0;
    surface.normal.y = surface.normal.z;
    surface.normal.x = surface.normal.y;
}
