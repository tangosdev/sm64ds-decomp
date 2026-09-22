//cpp
// @symbol _ZN5dPa_c7level_c26checkWaterRippleCallback_c8OnUpdateERN8Particle6SystemEb
#include "dPa_c.h"
#include "Particle__System.h"

extern "C" s32 data_0209f32c;
/* Fix12<int> by value is the measured 2004/b56 caller-side ABI wall. */
extern "C" void _ZN8Particle6System9NewRippleE5Fix12IiES2_S2_(
    s32 x, s32 y, s32 z);

int dPa_c::level_c::checkWaterRippleCallback_c::OnUpdate(
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
        s32 x = particle->offsetAsr3.x + particle->positionAsr3.x;
        s32 y = particle->offsetAsr3.y + particle->positionAsr3.y;
        s32 z = particle->offsetAsr3.z + particle->positionAsr3.z;

        if (particle->velocityAsr3.y < 0 && (y << 3) < data_0209f32c) {
            particle->age = particle->lifetime;
            _ZN8Particle6System9NewRippleE5Fix12IiES2_S2_(
                x << 3, data_0209f32c + 0x3000, z << 3);
        }
        particle = particle->next;
    }
    return 1;
}
