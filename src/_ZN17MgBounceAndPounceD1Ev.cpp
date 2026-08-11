//cpp
// @symbol _ZN17MgBounceAndPounceD1Ev
/* Forces the complete-object destructor out-of-line. ~MgBounceAndPounce()
   is defined inline in MgBounceAndPounce.h (its four descendants need to
   inline it the same way Stage inlines Scene's) -- a bare include emits
   nothing here, and the definition can no longer live in this file
   directly. See src/_ZN5SceneD1Ev.cpp / _ZN11dScMgBase_cD1Ev.cpp for the
   identical pattern one and two levels up. */
#include "MgBounceAndPounce.h"
void MgBounceAndPounce_EmitDestructor(MgBounceAndPounce *p)
{
    p->~MgBounceAndPounce();
}
