//cpp
// @symbol _ZN14dScMgD3DBase_cD0Ev
/* Forces the deleting destructor out-of-line. Same reasoning as
   _ZN14dScMgD3DBase_cD1Ev.cpp. */
#include "dScMgD3DBase_c.h"
void dScMgD3DBase_c_EmitDeletingDestructor(dScMgD3DBase_c *p)
{
    delete p;
}
