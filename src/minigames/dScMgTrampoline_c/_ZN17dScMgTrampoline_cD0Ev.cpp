//cpp
// @symbol _ZN17dScMgTrampoline_cD0Ev
/* Real out-of-line definition, identical body to
   _ZN17dScMgTrampoline_cD1Ev.cpp -- see that file's note. */
#include "dScMgTrampoline_c.h"
dScMgTrampoline_c::~dScMgTrampoline_c()
{
    __destroy_arr(mArray3, 5, 0x24, (void *)func_ov006_02120938);
    __destroy_arr(mArray2, 3, 0x32c, (void *)func_ov006_020d1008);
    __destroy_arr(mArray1, 4, 0xd0, (void *)func_ov006_020ccfc8);
}
