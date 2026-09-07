//cpp
#include "decl_common.h"
#include "Particle__Behavior.h"

void Particle::Jitter::Func(EffectData& effect, char* particle, Vector3& velocity)
{
    Element& state = *(Element*)particle;
    u32 s;
    int r;
    int amp;

    if ((int)state.age % (int)effect.jitter.period != 0)
        return;

    s = data_020a4d30 * 0x5eedf715u + 0x1b0cb173u;
    data_020a4d30 = s;
    amp = effect.jitter.xAmplitude;
    r = s >> 23;
    velocity.x += (amp * r - (amp << 8)) >> 8;

    s = data_020a4d30 * 0x5eedf715u + 0x1b0cb173u;
    data_020a4d30 = s;
    amp = effect.jitter.yAmplitude;
    r = s >> 23;
    velocity.y += (amp * r - (amp << 8)) >> 8;

    s = data_020a4d30 * 0x5eedf715u + 0x1b0cb173u;
    data_020a4d30 = s;
    amp = effect.jitter.zAmplitude;
    r = s >> 23;
    velocity.z += (amp * r - (amp << 8)) >> 8;
}
