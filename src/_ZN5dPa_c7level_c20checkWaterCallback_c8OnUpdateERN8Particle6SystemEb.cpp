//cpp
// @symbol _ZN5dPa_c7level_c20checkWaterCallback_c8OnUpdateERN8Particle6SystemEb
#include "dPa_c.h"
#include "Particle__System.h"

extern "C" s32 data_0209f32c;

int dPa_c::level_c::checkWaterCallback_c::OnUpdate(
    Particle::System& system, bool active)
{
    Particle::ParticleNode *particle = system.particles.head;

    if (active) {
        system.callbackFlags &= ~2;
    } else {
        system.callbackFlags |= 2;
        if (particle == 0)
            return 0;
    }

    while (particle != 0) {
        if (((particle->offsetAsr3.y + particle->positionAsr3.y) << 3)
            > data_0209f32c) {
            particle->age = particle->lifetime;
        }
        particle = particle->next;
    }
    return 1;
}
