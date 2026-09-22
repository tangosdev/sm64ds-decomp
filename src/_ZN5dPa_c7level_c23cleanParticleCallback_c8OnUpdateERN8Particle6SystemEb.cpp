//cpp
// @symbol _ZN5dPa_c7level_c23cleanParticleCallback_c8OnUpdateERN8Particle6SystemEb
#include "dPa_c.h"
#include "Particle__System.h"

int dPa_c::level_c::cleanParticleCallback_c::OnUpdate(
    Particle::System& system, bool done)
{
    Particle::ParticleNode *particle;

    if (!done) {
        particle = system.particles.head;
        while (particle != 0) {
            particle->age = particle->lifetime;
            return 0;
        }
    }
    return 1;
}
