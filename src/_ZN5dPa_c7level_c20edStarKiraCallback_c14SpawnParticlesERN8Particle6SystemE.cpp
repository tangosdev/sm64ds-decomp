//cpp
// @symbol _ZN5dPa_c7level_c20edStarKiraCallback_c14SpawnParticlesERN8Particle6SystemE
#include "dPa_c.h"
#include "Particle__System.h"

void dPa_c::level_c::edStarKiraCallback_c::SpawnParticles(
    Particle::System& system)
{
    system.mDefinition->data->callbackParam = unk308;
    system.callbackParam = unk30c;
    simpleCallback_c::SpawnParticles(system);
}
