//cpp
// @symbol _ZN12dScMgJump2_cD1Ev
/* recovered: real C++ destructor. The explicit calls reproduce the ROM's
   own recovered body exactly, destroying the members in reverse of the
   factory's construction order. Everything after -- this class's vtable
   store, dScMgD3DBase_c's vptr store, mSysTracker destruction and the chain
   to ~dScMgBase_c() -- is the compiler's own inlining of
   dScMgD3DBase_c's inline destructor (see include/dScMgD3DBase_c.h).   The Model call IS written out here, unlike dScMgJump_c's. The ROM
   destroys it FIRST, and a typed member is always destroyed after the user
   body -- so a typed Model could only produce the opposite order. See the
   header banner. */
#include "dScMgJump2_c.h"
dScMgJump2_c::~dScMgJump2_c()
{
    _ZN5ModelD1Ev((char *)this + 0x5a14);
    __destroy_arr(mArray3, 0x10, 0x24, (void *)func_ov006_020eed64);
    __destroy_arr(mArray2, 6, 0xf0, (void *)func_ov006_020c6f3c);
    __destroy_arr(mArray1, 3, 0xb8, (void *)func_ov006_020c893c);
}
