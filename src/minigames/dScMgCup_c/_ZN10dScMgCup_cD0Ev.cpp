//cpp
// @symbol _ZN10dScMgCup_cD0Ev
/* Real out-of-line definition, identical body to
   _ZN10dScMgCup_cD1Ev.cpp -- see that file's note. */
#include "dScMgCup_c.h"
dScMgCup_c::~dScMgCup_c()
{
    __destroy_arr(mArray2, 3, 8, (void *)NullDestructor_0203d47c);
    __destroy_arr(mArray1, 0x20, 0x18, (void *)func_ov006_020deac4);
    func_ov006_020c3288((char *)this + 0x4f38);
}
