//cpp
/* arm9/Particle_Behavior -- the six effect behaviors of the particle engine,
 * folded from six legacy shards at .text 0x0204d4d0..0x0204d8e8. Each
 * Particle::<Name> is a POD behavior struct carrying one static Func that a
 * SystemDefinition's Particle::Behavior table (built by func_0204a17c in
 * Particle_Manager.cpp) calls per particle: the resource supplies the
 * EffectData record, the Element the per-particle state, and Func advances
 * the particle's velocity/offset accordingly. The span is bounded on both
 * sides by func_ free functions: func_0204d440 below and func_0204d8e8 above.
 * mwccarm emits .text in reverse source order, so the definitions below run
 * ROM-descending; the roster reads ROM-ascending.
 *
 *   RadiusConverge, LimitPlane, Turn, Converge, Jitter, Acceleration -- the
 *   Func of each behavior, against the typed data records in
 *   Particle__Behavior.h.
 *
 * Member and field names are the tree's readable inferences (see
 * Particle__Behavior.h's header comment); offsets, extents and the effect
 * record sizes are ROM-proven.
 */

#include "decl_common.h"
#include "Particle__Behavior.h"
#include "math/Matrix.h"
#include "nitro/fx/fx.h"

extern "C" {
/* local extern: no header declares the sine table or these three matrix
 * entry points (checked include/math/*.h and decl_*.h). */
extern s16 data_02082214[];
void func_02052550(Matrix3x3* matrix, int sin, int cos);
void func_0205256c(Matrix3x3* matrix, int sin, int cos);
void Matrix3x3_SetRotationZ(Matrix3x3* matrix, int sin, int cos);
void MulVec3Mat3x3(const Vector3* in, const Matrix3x3* matrix, Vector3* out);
}

namespace Particle {

// @symbol _ZN8Particle12Acceleration4FuncERNS_10EffectDataEPcR7Vector3
void Acceleration::Func(EffectData& effect, char*, Vector3& velocity)
{
    velocity.x += effect.acceleration.x;
    velocity.y += effect.acceleration.y;
    velocity.z += effect.acceleration.z;
}

// @symbol _ZN8Particle6Jitter4FuncERNS_10EffectDataEPcR7Vector3
void Jitter::Func(EffectData& effect, char* particle, Vector3& velocity)
{
    Element& state = *(Element*)particle;
    u32 s;
    int r;
    int amp;

    if (state.age % effect.jitter.period != 0)
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

// @symbol _ZN8Particle8Converge4FuncERNS_10EffectDataEPcR7Vector3
void Converge::Func(EffectData& effect, char* particle,
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

// @symbol _ZN8Particle4Turn4FuncERNS_10EffectDataEPcR7Vector3
void Turn::Func(EffectData& effect, char* particle, Vector3&)
{
    Element& state = *(Element*)particle;
    Matrix3x3 matrix;
    int idx;

    switch (effect.turn.axis) {
    case 0:
        idx = effect.turn.angle >> 4;
        func_02052550(&matrix, data_02082214[idx * 2],
                     data_02082214[idx * 2 + 1]);
        break;
    case 1:
        idx = effect.turn.angle >> 4;
        func_0205256c(&matrix, data_02082214[idx * 2],
                     data_02082214[idx * 2 + 1]);
        break;
    case 2:
        idx = effect.turn.angle >> 4;
        Matrix3x3_SetRotationZ(&matrix, data_02082214[idx * 2],
                              data_02082214[idx * 2 + 1]);
        break;
    }

    MulVec3Mat3x3(&state.offset, &matrix, &state.offset);
}

// @symbol _ZN8Particle10LimitPlane4FuncERNS_10EffectDataEPcR7Vector3
void LimitPlane::Func(EffectData& effect, char* particle, Vector3&)
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
                    state.velocity.y = -FX_MUL(state.velocity.y, effect.limitPlane.restitution);
                    return;
                }
            }
            if (current > plane) {
                if (current + state.offset.y < plane) {
                    state.offset.y = plane - current;
                    state.velocity.y = -FX_MUL(state.velocity.y, effect.limitPlane.restitution);
                }
            }
        }
        break;
    }
}

// @symbol _ZN8Particle14RadiusConverge4FuncERNS_10EffectDataEPcR7Vector3
void RadiusConverge::Func(EffectData& effect, char* particle, Vector3&)
{
    Element& state = *(Element*)particle;

    state.offset.x +=
        FX_MUL(effect.radiusConverge.strength, (effect.radiusConverge.targetX - state.offset.x));
    state.offset.y +=
        FX_MUL(effect.radiusConverge.strength, (effect.radiusConverge.targetY - state.offset.y));
    state.offset.z +=
        FX_MUL(effect.radiusConverge.strength, (effect.radiusConverge.targetZ - state.offset.z));
}

} // namespace Particle
