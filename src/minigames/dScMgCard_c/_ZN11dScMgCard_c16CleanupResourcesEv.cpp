//cpp
// @symbol _ZN11dScMgCard_c16CleanupResourcesEv
#include "dScMgCard_c.h"
/* dScMgCard_c::CleanupResources -- vtable slot 3, ov006 0x020da994.
 *
 * Named from the table: 0x020da994 is the word slot 3 of _ZTV11dScMgCard_c
 * holds where its base's table holds something else, so it is this class's
 * own override of the virtual fBase_c declares.
 *
 * The body never touches the instance -- it only clears three ov006 globals
 * -- which is why the pre-migration stub could take `void` and still match:
 * `this` arrives in r0 and is simply not read. */

extern "C" {
extern int data_ov006_02141768[];
extern int data_ov006_0214176c[];
extern int data_ov006_02141770[];
}

s32 dScMgCard_c::CleanupResources()
{
    data_ov006_0214176c[0] = 0;
    data_ov006_02141768[0] = 0;
    data_ov006_02141770[0] = 0;
    return 1;
}
