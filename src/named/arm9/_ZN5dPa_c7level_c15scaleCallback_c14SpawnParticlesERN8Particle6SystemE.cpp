//cpp
// @symbol _ZN5dPa_c7level_c15scaleCallback_c14SpawnParticlesERN8Particle6SystemE
#include "dPa_c.h"
#include "Particle__System.h"

void dPa_c::level_c::scaleCallback_c::SpawnParticles(Particle::System& system)
{
    system.callbackScale = scale;
    system.callbackVelocity = velocity;
    simpleCallback_c::SpawnParticles(system);
    timer = 0x10;
}
