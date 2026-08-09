//cpp
// @symbol _ZN17MgBounceAndPounce18AfterInitResourcesEj
/* recovered: named members + shared header, real C++ method */
#include "decl_common.h"
#include "MgBounceAndPounce.h"

extern "C" int _ZN8Particle10SysTracker10InitialiseEv(void *self);

int MgBounceAndPounce::AfterInitResources(unsigned int)
{
    func_ov004_020b08f0(this);
    return _ZN8Particle10SysTracker10InitialiseEv(&unk_47e4);
}
