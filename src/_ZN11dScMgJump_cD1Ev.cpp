//cpp
// @symbol _ZN11dScMgJump_cD1Ev
/* recovered: real C++ destructor. The remaining explicit call destroys the
   still-untyped second array. The compiler then destroys mPlayers and mModel
   in reverse of the factory's construction order. Everything after -- this class's vtable
   store, dScMgD3DBase_c's vptr store, mSysTracker destruction and the chain
   to ~dScMgBase_c() -- is the compiler's own inlining of
   dScMgD3DBase_c's inline destructor (see include/dScMgD3DBase_c.h). */
#include "dScMgJump_c.h"
dScMgJump_c::~dScMgJump_c()
{
    __destroy_arr(mArray2, 6, 0xf0, (void *)func_ov006_020c6f3c);
}
