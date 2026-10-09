//cpp
// @symbol _ZN5dPa_c7level_c18fitWaterCallback_c8OnUpdateERN8Particle6SystemEb
#include "dPa_c.h"
#include "Particle__System.h"

extern "C" s32 data_0209f32c;

int dPa_c::level_c::fitWaterCallback_c::OnUpdate(
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

    const s32 waterY = (waterOffset + data_0209f32c) >> 3;
    while (particle != 0) {
        particle->offsetAsr3.y = waterY - particle->positionAsr3.y;
        particle = particle->next;
    }
    return 1;
}
