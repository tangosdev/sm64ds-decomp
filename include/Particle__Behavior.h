#ifndef PARTICLE__BEHAVIOR_H
#define PARTICLE__BEHAVIOR_H

#include "Particle__Element.h"
#include "nitro/fx/fx.h"

/* The six particle behaviours of the NitroSystem particle library (SPL).
 * func_0204a17c reads one record per behaviour bit of a definition's flags
 * straight out of the particle resource and pairs it with its apply function
 * in the definition's SPLBehavior table; the particle update calls each entry
 * once per particle per frame.
 *
 * The record, field and function names are pret's SPL coinages (in
 * pokeplatinum/lib/spl/include/spl_behavior.h), not ROM or SDK symbols, kept so the two can
 * be read side by side. The record layouts are field for field the same. This
 * cartridge carries an older SPL: the apply functions take no emitter, so the
 * collision plane has no per-emitter height override. */

#define SPL_SPIN_BEHAVIOR_AXIS_X 0
#define SPL_SPIN_BEHAVIOR_AXIS_Y 1
#define SPL_SPIN_BEHAVIOR_AXIS_Z 2

#define SPL_COLLISION_BEHAVIOR_TYPE_KILL 0   /* the particle dies at the plane */
#define SPL_COLLISION_BEHAVIOR_TYPE_BOUNCE 1 /* the particle bounces off it */

/* A constant push added to the acceleration. */
typedef struct SPLGravityBehavior {
    VecFx16 magnitude;
    u16 padding;
} SPLGravityBehavior;

/* A random push, every applyInterval frames of the particle's age. */
typedef struct SPLRandomBehavior {
    VecFx16 magnitude;
    u16 applyInterval;
} SPLRandomBehavior;

/* A pull toward target, through the acceleration. */
typedef struct SPLMagnetBehavior {
    VecFx32 target;
    fx16 force;
    u16 padding;
} SPLMagnetBehavior;

/* A rotation of the particle's position about one axis each frame. */
typedef struct SPLSpinBehavior {
    u16 angle;
    u16 axis;
} SPLSpinBehavior;

/* A horizontal plane at height y that kills or bounces the particle. */
typedef struct SPLCollisionPlaneBehavior {
    fx32 y;
    fx16 elasticity;
    u16 collisionType : 2;
} SPLCollisionPlaneBehavior;

/* A pull toward target applied to the position directly. */
typedef struct SPLConvergenceBehavior {
    VecFx32 target;
    fx16 force;
    u16 padding;
} SPLConvergenceBehavior;

/* One entry of a definition's behaviour table. */
typedef struct SPLBehavior {
    void (*applyFunc)(const void *obj, Particle::Element *ptcl, Vector3 *acc);
    const void *object;
} SPLBehavior;

extern "C" {
void SPLBehavior_ApplyGravity(const void *obj, Particle::Element *ptcl, Vector3 *acc);
void SPLBehavior_ApplyRandom(const void *obj, Particle::Element *ptcl, Vector3 *acc);
void SPLBehavior_ApplyMagnet(const void *obj, Particle::Element *ptcl, Vector3 *acc);
void SPLBehavior_ApplySpin(const void *obj, Particle::Element *ptcl, Vector3 *acc);
void SPLBehavior_ApplyCollisionPlane(const void *obj, Particle::Element *ptcl, Vector3 *acc);
void SPLBehavior_ApplyConvergence(const void *obj, Particle::Element *ptcl, Vector3 *acc);
}

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char SPLGravityBehavior_size_must_be_0x8[sizeof(SPLGravityBehavior) == 0x8 ? 1 : -1];
typedef char SPLRandomBehavior_size_must_be_0x8[sizeof(SPLRandomBehavior) == 0x8 ? 1 : -1];
typedef char SPLMagnetBehavior_size_must_be_0x10[sizeof(SPLMagnetBehavior) == 0x10 ? 1 : -1];
typedef char SPLSpinBehavior_size_must_be_0x4[sizeof(SPLSpinBehavior) == 0x4 ? 1 : -1];
typedef char
    SPLCollisionPlaneBehavior_size_must_be_0x8[sizeof(SPLCollisionPlaneBehavior) == 0x8 ? 1 : -1];
typedef char
    SPLConvergenceBehavior_size_must_be_0x10[sizeof(SPLConvergenceBehavior) == 0x10 ? 1 : -1];
typedef char SPLBehavior_size_must_be_0x8[sizeof(SPLBehavior) == 0x8 ? 1 : -1];
#endif

#endif
