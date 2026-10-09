//cpp
// @symbol _ZN8Particle10SysTracker6UpdateEv
/* recovered: real class form -- the compiler spells the mangled name */
#include "Particle__SysTracker.h"

#include "decl_common.h"

namespace Particle {

void SysTracker::Update()
{
    mContents.Update();
    func_02049f58(mManager);
}

}
