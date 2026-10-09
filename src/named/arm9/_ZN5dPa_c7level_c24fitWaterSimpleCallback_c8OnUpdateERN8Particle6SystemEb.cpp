//cpp
// @symbol _ZN5dPa_c7level_c24fitWaterSimpleCallback_c8OnUpdateERN8Particle6SystemEb
#include "dPa_c.h"
#include "Particle__System.h"

extern "C" s32 data_0209f32c;

int dPa_c::level_c::fitWaterSimpleCallback_c::OnUpdate(
    Particle::System& system, bool active)
{
    s32 waterY;
    Particle::ParticleNode *particle;

    particle = system.particles.head;
    waterY = data_0209f32c >> 3;

    while (particle != 0) {
        particle->offsetAsr3.y = waterY - particle->positionAsr3.y;
        particle = particle->next;
    }

    return simpleCallback_c::OnUpdate(system, active);
}
