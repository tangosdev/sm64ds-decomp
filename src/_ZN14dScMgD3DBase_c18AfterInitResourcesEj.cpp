//cpp
// @symbol _ZN14dScMgD3DBase_c18AfterInitResourcesEj
/* recovered: named members + shared header, real C++ method */
#include "decl_common.h"
#include "dScMgD3DBase_c.h"

extern "C" int _ZN8Particle10SysTracker10InitialiseEv(void *self);

void dScMgD3DBase_c::AfterInitResources(unsigned int)
{
    _ZN11dScMgBase_c18AfterInitResourcesEj(this);
    _ZN8Particle10SysTracker10InitialiseEv(&mSysTracker);
}
