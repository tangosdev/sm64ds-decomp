//cpp
// @symbol _ZN8Particle19SetSelfDestructFlagEj
#include "Particle__Manager.h"
#include "Particle__SysTracker.h"

extern "C" Particle::SysTracker *data_0209ee74;

namespace Particle {

void SetSelfDestructFlag(u32 definitionID)
{
    data_0209ee74->mManager->mDefinitions[definitionID].data->flags |= 0x4000;
}

}
