//cpp
// @symbol _ZN12dScMgAmida_cD1Ev
/* ~dScMgAmida_c() (D1, complete-object destructor) -- explicitly destroys
   FOUR arrays via __destroy_arr, in this exact order, same idiom
   dScMgBase_c's own D1 uses for its touchIcon_0f4 array (see
   dScMgBase_c.h's file banner and src/_ZN11dScMgBase_cD1Ev.cpp) and
   dScMgHanachan_c's own D1/D0 pair uses for its single array -- just four
   calls instead of one here. See the class header banner
   (include/dScMgAmida_c.h) for each array's shape. The base-D2 call and
   own-vtable-write are compiler generated. */
#include "dScMgAmida_c.h"

dScMgAmida_c::~dScMgAmida_c()
{
    __destroy_arr((char *)this + 0x4768, 0x80, 0x18, (void *)func_ov006_020d116c);
    __destroy_arr((char *)this + 0x4744, 4, 8, (void *)NullDestructor_0203d47c);
    __destroy_arr((char *)this + 0x4724, 4, 8, (void *)NullDestructor_0203d47c);
    __destroy_arr((char *)this + 0x4660, 4, 8, (void *)NullDestructor_0203d47c);
}
