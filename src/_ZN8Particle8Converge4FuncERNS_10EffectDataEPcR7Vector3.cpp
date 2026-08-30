//cpp
#include "Particle__Behavior.h"

void Particle::Converge::Func(EffectData& effect, char* particle,
                              Vector3& acceleration)
{
    Element& state = *(Element*)particle;

    acceleration.x += effect.converge.strength
        * ((effect.converge.targetX - state.offset.x) - state.velocity.x) >> 12;
    acceleration.y += effect.converge.strength
        * ((effect.converge.targetY - state.offset.y) - state.velocity.y) >> 12;
    acceleration.z += effect.converge.strength
        * ((effect.converge.targetZ - state.offset.z) - state.velocity.z) >> 12;
}
