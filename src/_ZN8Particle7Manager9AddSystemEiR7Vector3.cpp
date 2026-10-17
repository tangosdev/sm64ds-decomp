//cpp
// @symbol _ZN8Particle7Manager9AddSystemEiR7Vector3
/* Particle::Manager::AddSystem -- spawn a System, arm9
 * 0x02049e0c..0x02049ee8. func_0204d958 pops one off mFreeSystems,
 * func_0204ae2c initializes it from the indexed SystemDefinition, the
 * manager's two flag words seed the system's flags, func_0204d9a0 pushes
 * it onto mActiveSystems, and a definition whose flags carry the
 * self-destruct bit fails the spawn.
 *
 * leftovers: `(flags << 17) >> 31` stays the arithmetic-shift bit-14 test
 * -- `& 0x4000` emits a different sequence. flags.unk_000/unk_006/unk_00c
 * and manager unk_02c/unk_02e keep their placeholder names: the manager
 * ctor (func_0204a4c8) stores its third and fourth arguments there for
 * AddSystem to loan into each new system's flags, but no consumer names
 * what they index.
 */
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
