//cpp
// @symbol _ZN15dScMgSnowball_cD1Ev
/* recovered: real C++ destructor. The three explicit calls reproduce the
   ROM's own recovered body (func_ov006_0212568c, pre-migration): destroy
   mArray3, mArray2, mArray1 -- reverse of the constructor's order. The
   FOURTH call the ROM makes, `_ZN5ModelD1Ev(c + 0xaba4)`, is deliberately
   NOT written here: mModel is a real typed member declared ahead of the
   three arrays, so the compiler emits its destruction last, which is
   exactly where the ROM has it. Everything after -- own vtable store,
   mSysTracker destruction, chain to ~dScMgBase_c() -- is the compiler's
   own inlining of dScMgSingle3DBase_c's now-inline destructor (see
   include/dScMgSingle3DBase_c.h's own note). */
#include "dScMgSnowball_c.h"
dScMgSnowball_c::~dScMgSnowball_c()
{
    __destroy_arr(mArray3, 0x20, 0x24, (void *)func_ov006_02125800);
    __destroy_arr(mArray2, 0x80, 8, (void *)NullDestructor_0203d47c);
    __destroy_arr(mArray1, 0x80, 8, (void *)NullDestructor_0203d47c);
}
