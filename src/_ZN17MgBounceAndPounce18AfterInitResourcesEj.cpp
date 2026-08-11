//cpp
// @symbol _ZN17MgBounceAndPounce18AfterInitResourcesEj
/* recovered: named members + shared header, real C++ method */
#include "decl_common.h"
#include "MgBounceAndPounce.h"

extern "C" int _ZN8Particle10SysTracker10InitialiseEv(void *self);

void MgBounceAndPounce::AfterInitResources(unsigned int)
{
    _ZN11dScMgBase_c18AfterInitResourcesEj(this);
    _ZN8Particle10SysTracker10InitialiseEv(&mSysTracker);
}
