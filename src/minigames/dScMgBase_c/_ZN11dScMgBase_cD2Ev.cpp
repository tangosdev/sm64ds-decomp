//cpp
// @symbol _ZN11dScMgBase_cD2Ev
/* D2, not D1. This file carries the same definition as
 * src/_ZN11dScMgBase_cD1Ev.cpp, and deliberately so: dScMgBase_c has no virtual
 * bases, so mwcc emits D1 and D2 as byte-identical code. Only the way the
 * ROM REACHES an address separates them -- a vtable slot holds D1, a derived
 * destructor's base-chain `bl` reaches D2 -- so comparing the two bodies
 * proves nothing and the binding in config/arm9/overlays/ov004/delinks.txt is what decides.
 * objisolate keeps the D2 variant; the C2/D0/D1 siblings stay in their own
 * files. */
/* ~dScMgBase_c() is declared, NOT defined inline, in dScMgBase_c.h -- see
   the header's own note. mwcc doesn't inline a body this size into a
   descendant (measured against dScMgD3DBase_c), so this is a REAL
   out-of-line definition, same shape Stage.h uses for a leaf -- except
   here D0Ev.cpp carries an identical copy for its own key-function TU. */
#include "dScMgBase_c.h"
dScMgBase_c::~dScMgBase_c()
{
    data_ov004_020beb68 = 0;
    func_ov004_020b929c((char *)this + 0xf4);
}
