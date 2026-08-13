//cpp
// @symbol _ZN14dScMgD3DBase_cD1Ev
/* Forces the complete-object destructor out-of-line. ~dScMgD3DBase_c()
   is defined inline in dScMgD3DBase_c.h (its four descendants need to
   inline it the same way Stage inlines Scene's) -- a bare include emits
   nothing here, and the definition can no longer live in this file
   directly. See src/_ZN5SceneD1Ev.cpp / _ZN11dScMgBase_cD1Ev.cpp for the
   identical pattern one and two levels up. */
#include "dScMgD3DBase_c.h"
void dScMgD3DBase_c_EmitDestructor(dScMgD3DBase_c *p)
{
    p->~dScMgD3DBase_c();
}
