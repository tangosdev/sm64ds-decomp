//cpp
#include "Particle__Behavior.h"

void Particle::RadiusConverge::Func(EffectData& effect, char* particle, Vector3&)
{
    Element& state = *(Element*)particle;

    state.offset.x += (int)(((s64)effect.radiusConverge.strength
        * (effect.radiusConverge.targetX - state.offset.x) + 0x800) >> 12);
    state.offset.y += (int)(((s64)effect.radiusConverge.strength
        * (effect.radiusConverge.targetY - state.offset.y) + 0x800) >> 12);
    state.offset.z += (int)(((s64)effect.radiusConverge.strength
        * (effect.radiusConverge.targetZ - state.offset.z) + 0x800) >> 12);
}
