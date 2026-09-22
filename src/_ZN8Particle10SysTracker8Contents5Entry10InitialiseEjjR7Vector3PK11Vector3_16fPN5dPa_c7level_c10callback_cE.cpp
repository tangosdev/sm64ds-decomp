//cpp
// Readable method name inferred from the registry behavior; not ROM text.
#include "Particle__System.h"

namespace Particle {

bool SysTracker::Contents::Entry::Initialise(
    u32 newUniqueID, u32 newDefinitionID, Vector3& position,
    const Vector3_16f *direction,
    dPa_c::level_c::callback_c *newCallback)
{
    SystemDefinition::Data& data =
        *data_0209ee74->mManager->mDefinitions[newDefinitionID].data;

    data.flags &= ~0x4000;
    system = data_0209ee74->mManager->AddSystem(newDefinitionID, position);
    if (system == 0)
        return false;

    if (newDefinitionID == 0x52 || newDefinitionID == 0x50) {
        if (system->flags.unk_000 <= 5 && system->flags.unk_006 >= 5)
            system->flags.unk_00c = 5;
    }

    savedValue = data.callbackValue;
    data.callbackValue = 0;

    if (direction != 0) {
        System *createdSystem = system;
        createdSystem->direction.x = direction->x;
        createdSystem->direction.y = direction->y;
        createdSystem->direction.z = direction->z;
    }

    if (newCallback != 0)
        newCallback->SpawnParticles(*system);

    uniqueID = newUniqueID;
    definitionID = newDefinitionID;
    active = 1;
    callback = newCallback;
    prev = 0;
    next = 0;
    return true;
}

} // namespace Particle
