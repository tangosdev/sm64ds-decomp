//cpp
// @symbol _ZN15dScMgHanachan_cD0Ev
/* Real out-of-line definition, identical body to
   _ZN15dScMgHanachan_cD1Ev.cpp -- see that file's note and
   dScMgHanachan_c.h's own note. dScMgBase_c's own operator delete (its
   immediate base) covers the Memory::Deallocate call the pre-migration
   source made explicitly; no per-class copy needed, same precedent as
   every other dScMgBase_c leaf's D0. The tree-wide OnYoshiTryEat mislabel
   on this class landed HERE (its source comment said "recovered name:
   dScMgHanachan_c_OnYoshiTryEat") rather than on the real OnYoshiTryEat-
   shaped slot 18 helper -- this body is the textbook D0 shape (vtable
   write, array destroy, base D2, Deallocate), not gameplay logic. */
#include "dScMgHanachan_c.h"

dScMgHanachan_c::~dScMgHanachan_c()
{
    __destroy_arr((char *)this + 0x4678, 0xf, 0x98, (void *)func_ov006_020ea324);
}
