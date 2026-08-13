//cpp
// @symbol _ZN18dScMgTrampoline2_cD1Ev
/* recovered: real C++ destructor. The explicit calls reproduce the ROM's
   own recovered body exactly, destroying the members in reverse of the
   factory's construction order. Everything after -- this class's vtable
   store, dScMgD3DBase_c's vptr store, mSysTracker destruction and the chain
   to ~dScMgBase_c() -- is the compiler's own inlining of
   dScMgD3DBase_c's inline destructor (see include/dScMgD3DBase_c.h). */
#include "dScMgTrampoline2_c.h"
dScMgTrampoline2_c::~dScMgTrampoline2_c()
{
    __destroy_arr(mArray6, 5, 0x24, (void *)func_ov006_02120938);
    __destroy_arr(mArray5, 0x14, 0x78, (void *)func_ov006_02122c68);
    __destroy_arr(mArray4, 0xa, 0x24, (void *)func_ov006_020eed64);
    __destroy_arr(mArray3, 0xa, 0x1d0, (void *)func_ov006_021227c8);
    __destroy_arr(mArray2, 3, 0x32c, (void *)func_ov006_020d1008);
    __destroy_arr(mArray1, 5, 0xdc, (void *)func_ov006_020ca604);
}
