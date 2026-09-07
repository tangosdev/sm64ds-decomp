//cpp
// @symbol _ZN5dPa_c7level_c16simpleCallback_c14SpawnParticlesERN8Particle6SystemE
#include "dPa_c.h"
#include "Particle__System.h"

extern "C" void func_02049d60(
    Particle::Manager *manager, Particle::System *system);

void dPa_c::level_c::simpleCallback_c::SpawnParticles(
    Particle::System& system)
{
    system.callbackFlags |= 2;
    system.callbackValue = value;
    func_02049d60(data_0209ee74->mManager, &system);
}
