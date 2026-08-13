//cpp
// @symbol _ZN11dScMgJump_cD1Ev
/* recovered: real C++ destructor. The explicit calls reproduce the ROM's
   own recovered body exactly, destroying the members in reverse of the
   factory's construction order. Everything after -- this class's vtable
   store, dScMgD3DBase_c's vptr store, mSysTracker destruction and the chain
   to ~dScMgBase_c() -- is the compiler's own inlining of
   dScMgD3DBase_c's inline destructor (see include/dScMgD3DBase_c.h).   The FOURTH thing the ROM's body does -- _ZN5ModelD1Ev(c + 0x501c) -- is
   deliberately not written here: mModel is a real typed member declared
   ahead of both arrays, so the compiler emits its destruction last, which
   is exactly where the ROM has it. */
#include "dScMgJump_c.h"
dScMgJump_c::~dScMgJump_c()
{
    __destroy_arr(mArray2, 6, 0xf0, (void *)func_ov006_020c6f3c);
    __destroy_arr(mArray1, 3, 0xb8, (void *)func_ov006_020c893c);
}
