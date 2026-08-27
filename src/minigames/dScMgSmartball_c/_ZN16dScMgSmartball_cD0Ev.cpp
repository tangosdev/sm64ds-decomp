//cpp
// @symbol _ZN16dScMgSmartball_cD0Ev
/* Real out-of-line definition, identical body to
   _ZN16dScMgSmartball_cD1Ev.cpp -- see that file's note. */
#include "dScMgSmartball_c.h"
dScMgSmartball_c::~dScMgSmartball_c()
{
    __destroy_arr(mArray4, 0x40, 0x24, (void *)func_ov006_0210d894);
    __destroy_arr(mArray3, 0x10, 8, (void *)NullDestructor_0203d47c);
    __destroy_arr(mArray2, 0x10, 8, (void *)NullDestructor_0203d47c);
    __destroy_arr(mArray1, 5, 8, (void *)NullDestructor_0203d47c);
}
