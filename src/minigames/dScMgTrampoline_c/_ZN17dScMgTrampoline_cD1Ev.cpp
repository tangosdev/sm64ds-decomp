//cpp
// @symbol _ZN17dScMgTrampoline_cD1Ev
/* recovered: real C++ destructor. The explicit calls reproduce the ROM's
   own recovered body exactly, destroying the members in reverse of the
   factory's construction order. Everything after -- this class's vtable
   store, dScMgD3DBase_c's vptr store, mSysTracker destruction and the chain
   to ~dScMgBase_c() -- is the compiler's own inlining of
   dScMgD3DBase_c's inline destructor (see include/dScMgD3DBase_c.h). */
#include "dScMgTrampoline_c.h"
dScMgTrampoline_c::~dScMgTrampoline_c()
{
    __destroy_arr(mArray3, 5, 0x24, (void *)func_ov006_02120938);
    __destroy_arr(mArray2, 3, 0x32c, (void *)func_ov006_020d1008);
    __destroy_arr(mArray1, 4, 0xd0, (void *)func_ov006_020ccfc8);
}
