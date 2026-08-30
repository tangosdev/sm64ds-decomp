//cpp
// Readable method name inferred from the registry behavior; not ROM text.
#include "Particle__System.h"

namespace Particle {

void SysTracker::Contents::Entry::Reset()
{
    SystemDefinition::Data& data =
        *data_0209ee74->mManager->mDefinitions[definitionID].data;
    u32 *flagsAddress;
    u32 flags;

    data.callbackValue = savedValue;
    flagsAddress = &system->callbackFlags;
    flags = *flagsAddress;
    flags &= ~1;
    flags |= 1;
    *flagsAddress = flags;
    uniqueID = 0;
}

} // namespace Particle
