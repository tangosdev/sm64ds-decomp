//cpp
/* arm9/Particle_Behavior -- the six particle behaviours of the NitroSystem
 * particle library (SPL), folded from six legacy shards at .text
 * 0x0204d4d0..0x0204d8e8. func_0204a17c (Particle_Manager.cpp) pairs each
 * apply function with its record in the particle resource in a definition's
 * SPLBehavior table, and the particle update calls each entry once per
 * particle per frame. Gravity, random and magnet add into the particle's
 * acceleration; spin, collision plane and convergence act on the particle
 * itself. The span is bounded on both sides by C free functions:
 * SPLAnim_Scale below and func_0204d8e8 above. mwccarm emits .text in
 * reverse source order, so the definitions below run ROM-descending.
 *
 * Each body is the function pret's pokeplatinum decomp of SPL
 * (pokeplatinum/lib/spl/src/spl_behavior.c) gives the same job, in the same source order,
 * so the names are pret's coinages (see Particle__Behavior.h). This
 * cartridge's SPL is older: no emitter argument, the spin angle is shifted
 * down four bits into a sine-table index, and the collision plane always
 * uses the record's height. SPL is a C library; the file stays C++ because
 * Particle::Element is still a C++ type.
 */

#include "decl_common.h"
#include "Particle__Behavior.h"
#include "math/Matrix.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"

extern "C" {
/* local extern: no header declares these matrix entry points (checked
 * include/math/*.h and decl_*.h). */
void MTX_RotX33_(Matrix3x3 *matrix, int sin, int cos);
void MTX_RotY33_(Matrix3x3 *matrix, int sin, int cos);
void MTX_RotZ33_(Matrix3x3 *matrix, int sin, int cos);
void MTX_MultVec33(const Vector3 *in, const Matrix3x3 *matrix, Vector3 *out);
}

/* SPL's random draw (pret's pokeplatinum/lib/spl/include/spl_random.h);
 * data_020a4d30 is its state, read by the SPLAnim callbacks too. */
static inline u32 SPLRandom_Next(void) {
    data_020a4d30 = data_020a4d30 * 0x5eedf715 + 0x1b0cb173;
    return data_020a4d30;
}

static inline u32 SPLRandom_U32(u32 bits) {
    return SPLRandom_Next() >> (sizeof(u32) * 8 - bits);
}

/* A random number in [-num, num). */
#define SPLRandom_RangeFX32(num) (((num) * (s32)SPLRandom_U32(9) - ((num) << 8)) >> 8)

void SPLBehavior_ApplyGravity(const void *obj, Particle::Element *, Vector3 *acc) {
    const SPLGravityBehavior *gravity = (const SPLGravityBehavior *)obj;

    acc->x += gravity->magnitude.x;
    acc->y += gravity->magnitude.y;
    acc->z += gravity->magnitude.z;
}

void SPLBehavior_ApplyRandom(const void *obj, Particle::Element *ptcl, Vector3 *acc) {
    const SPLRandomBehavior *rng = (const SPLRandomBehavior *)obj;

    if (ptcl->age % rng->applyInterval != 0)
        return;

    acc->x += SPLRandom_RangeFX32(rng->magnitude.x);
    acc->y += SPLRandom_RangeFX32(rng->magnitude.y);
    acc->z += SPLRandom_RangeFX32(rng->magnitude.z);
}

void SPLBehavior_ApplyMagnet(const void *obj, Particle::Element *ptcl, Vector3 *acc) {
    const SPLMagnetBehavior *magnet = (const SPLMagnetBehavior *)obj;

    acc->x += magnet->force * ((magnet->target.x - ptcl->offset.x) - ptcl->velocity.x) >> 12;
    acc->y += magnet->force * ((magnet->target.y - ptcl->offset.y) - ptcl->velocity.y) >> 12;
    acc->z += magnet->force * ((magnet->target.z - ptcl->offset.z) - ptcl->velocity.z) >> 12;
}

void SPLBehavior_ApplySpin(const void *obj, Particle::Element *ptcl, Vector3 *) {
    const SPLSpinBehavior *spin = (const SPLSpinBehavior *)obj;
    Matrix3x3 matrix;
    int idx;

    switch (spin->axis) {
    case SPL_SPIN_BEHAVIOR_AXIS_X:
        idx = spin->angle >> 4;
        MTX_RotX33_(&matrix, FX_SinIdx(idx), FX_CosIdx(idx));
        break;
    case SPL_SPIN_BEHAVIOR_AXIS_Y:
        idx = spin->angle >> 4;
        MTX_RotY33_(&matrix, FX_SinIdx(idx), FX_CosIdx(idx));
        break;
    case SPL_SPIN_BEHAVIOR_AXIS_Z:
        idx = spin->angle >> 4;
        MTX_RotZ33_(&matrix, FX_SinIdx(idx), FX_CosIdx(idx));
        break;
    }

    MTX_MultVec33(&ptcl->offset, &matrix, &ptcl->offset);
}

void SPLBehavior_ApplyCollisionPlane(const void *obj, Particle::Element *ptcl, Vector3 *) {
    const SPLCollisionPlaneBehavior *plane = (const SPLCollisionPlaneBehavior *)obj;

    switch (plane->collisionType) {
    case SPL_COLLISION_BEHAVIOR_TYPE_KILL: {
        int ey, y;
        y = plane->y;
        ey = ptcl->basePosition.y;
        if (ey < y) {
            if (ey + ptcl->offset.y > y) {
                ptcl->age = ptcl->lifetime;
                return;
            }
        }
        if (ey > y) {
            if (ey + ptcl->offset.y < y)
                ptcl->age = ptcl->lifetime;
        }
    } break;
    case SPL_COLLISION_BEHAVIOR_TYPE_BOUNCE: {
        int ey, y;
        y = plane->y;
        ey = ptcl->basePosition.y;
        if (ey < y) {
            if (ey + ptcl->offset.y > y) {
                ptcl->offset.y = y - ey;
                ptcl->velocity.y = -FX_MUL(ptcl->velocity.y, plane->elasticity);
                return;
            }
        }
        if (ey > y) {
            if (ey + ptcl->offset.y < y) {
                ptcl->offset.y = y - ey;
                ptcl->velocity.y = -FX_MUL(ptcl->velocity.y, plane->elasticity);
            }
        }
    } break;
    }
}

void SPLBehavior_ApplyConvergence(const void *obj, Particle::Element *ptcl, Vector3 *) {
    const SPLConvergenceBehavior *conv = (const SPLConvergenceBehavior *)obj;

    ptcl->offset.x += FX_MUL(conv->force, (conv->target.x - ptcl->offset.x));
    ptcl->offset.y += FX_MUL(conv->force, (conv->target.y - ptcl->offset.y));
    ptcl->offset.z += FX_MUL(conv->force, (conv->target.z - ptcl->offset.z));
}
