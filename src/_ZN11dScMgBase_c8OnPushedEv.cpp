//cpp
// @symbol _ZN11dScMgBase_c8OnPushedEv
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dScMgBase_c.h"
// recovered name: dScMgBase_c_OnPushed
/* recovered: renamed to Class_Method */
/* dScMgBase_c::OnPushed - recovered from vtable slot identity.
   Converted from .c to .cpp: dScMgBase_c.h now includes the real dScene_c.h,
   which -- like dBase_c.h -- has no C spelling. Same shape as dScene_c.h's
   own two affected .c files in the prior slice. */

int dScMgBase_c::OnPushed()
{
    return mMenuOpen == 0;
}
