//cpp
#include "Particle__Behavior.h"

void Particle::LimitPlane::Func(EffectData& effect, char* particle, Vector3&)
{
    Element& state = *(Element*)particle;

    switch (effect.limitPlane.mode) {
    case 0:
        {
            int current, plane;
            plane = effect.limitPlane.position;
            current = state.basePosition.y;
            if (current < plane) {
                if (current + state.offset.y > plane) {
                    state.age = state.lifetime;
                    return;
                }
            }
            if (current > plane) {
                if (current + state.offset.y < plane)
                    state.age = state.lifetime;
            }
        }
        break;
    case 1:
        {
            int current, plane;
            plane = effect.limitPlane.position;
            current = state.basePosition.y;
            if (current < plane) {
                if (current + state.offset.y > plane) {
                    state.offset.y = plane - current;
                    state.velocity.y = -(int)(((s64)state.velocity.y
                        * effect.limitPlane.restitution + 0x800) >> 12);
                    return;
                }
            }
            if (current > plane) {
                if (current + state.offset.y < plane) {
                    state.offset.y = plane - current;
                    state.velocity.y = -(int)(((s64)state.velocity.y
                        * effect.limitPlane.restitution + 0x800) >> 12);
                }
            }
        }
        break;
    }
}
