//cpp
// @symbol _ZN12dScMgJump2_cD0Ev
/* Real out-of-line definition, identical body to
   _ZN12dScMgJump2_cD1Ev.cpp -- see that file's note. */
#include "dScMgJump2_c.h"
dScMgJump2_c::~dScMgJump2_c()
{
    _ZN5ModelD1Ev((char *)this + 0x5a14);
    __destroy_arr(mArray3, 0x10, 0x24, (void *)func_ov006_020eed64);
    __destroy_arr(mArray2, 6, 0xf0, (void *)func_ov006_020c6f3c);
    __destroy_arr(mArray1, 3, 0xb8, (void *)func_ov006_020c893c);
}
