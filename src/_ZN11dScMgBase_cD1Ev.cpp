//cpp
// @symbol _ZN11dScMgBase_cD1Ev
/* Forces the complete-object destructor out-of-line. ~dScMgBase_c() is
   defined inline in dScMgBase_c.h (its 32 descendants need to inline it the
   same way Stage inlines Scene's) -- a bare include emits nothing here, and
   the definition can no longer live in this file directly. See
   src/_ZN5SceneD1Ev.cpp for the identical pattern one level up. */
#include "dScMgBase_c.h"
void dScMgBase_c_EmitDestructor(dScMgBase_c *p)
{
    p->~dScMgBase_c();
}
