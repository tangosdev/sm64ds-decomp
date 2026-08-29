//cpp
// @symbol _ZN12dScMgJump2_cD1Ev
/* recovered: real C++ destructor. The explicit Model and untyped-array calls
   reproduce the first three steps of the ROM body. The compiler then destroys
   mPlayers as the final derived member. Everything after -- this class's vtable
   store, dScMgD3DBase_c's vptr store, mSysTracker destruction and the chain
   to ~dScMgBase_c() -- is the compiler's own inlining of
   dScMgD3DBase_c's inline destructor (see include/dScMgD3DBase_c.h). The Model
   call remains explicit because the ROM destroys it before both arrays. */
#include "dScMgJump2_c.h"
dScMgJump2_c::~dScMgJump2_c()
{
    _ZN5ModelD1Ev((char *)this + 0x5a14);
    __destroy_arr(mArray3, 0x10, 0x24, (void *)func_ov006_020eed64);
    __destroy_arr(mArray2, 6, 0xf0, (void *)func_ov006_020c6f3c);
}
