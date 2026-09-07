//cpp
// @symbol _ZN5dPa_c7level_c16bubbleCallback_c8OnUpdateERN8Particle6SystemEb
#include "dPa_c.h"
#include "Particle__System.h"

extern "C" s32 data_0209f32c;
extern "C" u32 _ZN8Particle6System9NewRippleE5Fix12IiES2_S2_(
    Fix12i x, Fix12i y, Fix12i z);

int dPa_c::level_c::bubbleCallback_c::OnUpdate(
    Particle::System& system, bool active)
{
    Particle::ParticleNode *particle = system.particles.head;

    while (particle != 0) {
        const s32 y = particle->offsetAsr3.y + particle->positionAsr3.y;
        const s32 x = particle->offsetAsr3.x + particle->positionAsr3.x;
        const s32 z = particle->offsetAsr3.z + particle->positionAsr3.z;

        if ((y << 3) > data_0209f32c) {
            particle->age = particle->lifetime;
            _ZN8Particle6System9NewRippleE5Fix12IiES2_S2_(
                x << 3, data_0209f32c + 0x3000, z << 3);
        }
        particle = particle->next;
    }

    return simpleCallback_c::OnUpdate(system, active);
}
