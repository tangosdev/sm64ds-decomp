//cpp
// @symbol _ZN11dScMgBase_cD0Ev
/* Forces the deleting destructor out-of-line. Same reasoning as
   _ZN11dScMgBase_cD1Ev.cpp; see src/_ZN5SceneD0Ev.cpp for the identical
   pattern one level up. */
#include "dScMgBase_c.h"
void dScMgBase_c_EmitDeletingDestructor(dScMgBase_c *p)
{
    delete p;
}
