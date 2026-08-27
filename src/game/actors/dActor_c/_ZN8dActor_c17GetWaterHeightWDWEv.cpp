//cpp
// @symbol _ZN8dActor_c17GetWaterHeightWDWEv
/* dActor_c::GetWaterHeightWDW() at 0x02010b70.
 *
 * Normally the actor's own Y. In Wet-Dry World (level 0x15) and only for actors
 * that are not area-bound, the height instead comes from a three-entry table
 * indexed by the current water level.
 */
#include "dActor_c.h"

extern "C" {
extern signed char data_0209f2f8[];
extern unsigned char data_0209f2c0[];
extern int data_02075244[];
}

s32 dActor_c::GetWaterHeightWDW()
{
    s32 ret = mPosY;
    if (data_0209f2f8[0] == 0x15 && mAreaId == 0) {
        unsigned int i = data_0209f2c0[0];
        if (i >= 3) i = 2;
        ret = data_02075244[i];
    }
    return ret;
}
