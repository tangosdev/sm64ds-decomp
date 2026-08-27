//cpp
// @symbol _ZN11dScMgCard_cD0Ev
/* Real out-of-line definition, identical body to
   _ZN11dScMgCard_cD1Ev.cpp -- see that file's note. */
#include "dScMgCard_c.h"
dScMgCard_c::~dScMgCard_c()
{
    __destroy_arr(mArray2, 5, 0x30, (void *)func_ov006_020d96f0);
    __destroy_arr(mArray1, 5, 0x30, (void *)func_ov006_020d96e0);
    func_ov006_020c1c64((char *)this + 0x4f38);
}
