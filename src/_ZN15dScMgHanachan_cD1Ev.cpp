//cpp
// @symbol _ZN15dScMgHanachan_cD1Ev
/* ~dScMgHanachan_c() (D1, complete-object destructor) -- explicitly
   destroys the 15x0x98 array at 0x4678 via __destroy_arr, same idiom
   dScMgBase_c's own D1 uses for its touchIcon_0f4 array (see
   dScMgBase_c.h's file banner and src/_ZN11dScMgBase_cD1Ev.cpp). The
   base-D2 call and own-vtable-write are compiler generated. */
#include "dScMgHanachan_c.h"

dScMgHanachan_c::~dScMgHanachan_c()
{
    __destroy_arr((char *)this + 0x4678, 0xf, 0x98, (void *)func_ov006_020ea324);
}
