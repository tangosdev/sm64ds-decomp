//cpp
// @symbol _ZN12dScMgAmida_cD0Ev
/* Real out-of-line definition, identical body to
   _ZN12dScMgAmida_cD1Ev.cpp -- see that file's note and
   include/dScMgAmida_c.h's own class banner. dScMgBase_c's own operator
   delete (its immediate base) covers the Memory::Deallocate call the
   pre-migration source made explicitly; no per-class copy needed, same
   precedent as every other dScMgBase_c leaf's D0. The tree-wide
   OnYoshiTryEat mislabel on this class landed HERE too (its source comment
   said "recovered name: dScMgAmida_c_OnYoshiTryEat") rather than on the
   real OnYoshiTryEat-shaped slot 18 helper -- this body is the textbook D0
   shape (vtable write, four array destroys, base D2, Deallocate), not
   gameplay logic, same mislabel dScMgHanachan_c's own D0 carried. */
#include "dScMgAmida_c.h"

dScMgAmida_c::~dScMgAmida_c()
{
    __destroy_arr((char *)this + 0x4768, 0x80, 0x18, (void *)func_ov006_020d116c);
    __destroy_arr((char *)this + 0x4744, 4, 8, (void *)NullDestructor_0203d47c);
    __destroy_arr((char *)this + 0x4724, 4, 8, (void *)NullDestructor_0203d47c);
    __destroy_arr((char *)this + 0x4660, 4, 8, (void *)NullDestructor_0203d47c);
}
