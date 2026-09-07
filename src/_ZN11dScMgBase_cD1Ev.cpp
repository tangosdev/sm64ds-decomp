//cpp
// @symbol _ZN11dScMgBase_cD1Ev
/* ~dScMgBase_c() is declared, NOT defined inline, in dScMgBase_c.h -- see
   the header's own note. mwcc doesn't inline a body this size into a
   descendant (measured against dScMgD3DBase_c), so this is a REAL
   out-of-line definition, same shape Stage.h uses for a leaf -- except
   here D0Ev.cpp carries an identical copy for its own key-function TU. */
#include "dScMgBase_c.h"
dScMgBase_c::~dScMgBase_c()
{
    data_ov004_020beb68 = 0;
}
