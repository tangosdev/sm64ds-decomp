//cpp
// @symbol _ZN8Particle7Manager9AddSystemEiR7Vector3
#include "Particle__Manager.h"
#include "Particle__System.h"

extern "C" Particle::System *func_0204d958(Particle::Manager::SystemList *list);
extern "C" void func_0204ae2c(
    Particle::System *system,
    Particle::SystemDefinition *definition,
    Vector3 *position);
extern "C" void func_0204d9a0(
    Particle::Manager::SystemList *list,
    Particle::System *system);

namespace Particle {

System *Manager::AddSystem(int definitionID, Vector3 &position)
{
    System *result;
    System *system;

    result = 0;
    if (mFreeSystems.head != 0) {
        system = func_0204d958(&mFreeSystems);
        result = system;
        func_0204ae2c(system, &mDefinitions[definitionID], &position);

        system->flags.unk_000 = unk_02c;
        system->flags.unk_006 = unk_02e;
        system->flags.unk_00c = system->flags.unk_000;
        system->flags.unused = 0;

        func_0204d9a0(&mActiveSystems, system);
        if ((system->mDefinition->data->flags << 17) >> 31)
            result = 0;
    }

    return result;
}

}
