//cpp
#include "Particle__Behavior.h"

void Particle::Acceleration::Func(EffectData& effect, char*, Vector3& velocity)
{
    velocity.x += effect.acceleration.x;
    velocity.y += effect.acceleration.y;
    velocity.z += effect.acceleration.z;
}
