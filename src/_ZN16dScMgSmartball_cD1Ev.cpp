//cpp
// @symbol _ZN16dScMgSmartball_cD1Ev
/* recovered: real C++ destructor. The four explicit calls reproduce the
   ROM's own recovered body (func_ov006_0210d740, pre-migration) exactly,
   destroying the arrays in reverse of the factory's construction order.
   The own-vtable store and the chain to ~dScMgBase_c() are compiler-
   generated; dScMgBase_c's destructor is NOT inlined here (it has real
   statements, and mwcc refuses to inline that across a TU boundary), so the
   ROM calls _ZN11dScMgBase_cD2Ev as a real bl -- which is exactly what a
   plain chain to the immediate base emits. */
#include "dScMgSmartball_c.h"
dScMgSmartball_c::~dScMgSmartball_c()
{
    __destroy_arr(mArray4, 0x40, 0x24, (void *)func_ov006_0210d894);
    __destroy_arr(mArray3, 0x10, 8, (void *)NullDestructor_0203d47c);
    __destroy_arr(mArray2, 0x10, 8, (void *)NullDestructor_0203d47c);
    __destroy_arr(mArray1, 5, 8, (void *)NullDestructor_0203d47c);
}
