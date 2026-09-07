#ifndef PARTICLE__SYSTEM_H
#define PARTICLE__SYSTEM_H

#include "Particle__Manager.h"
#include "Particle__SysTracker.h"

/* System::New's compiler-produced name distinguishes this six-byte particle
 * direction from the actor-facing Vector3_16 type. */
#ifndef VECTOR3_16F_DEFINED
#define VECTOR3_16F_DEFINED
struct Vector3_16f { s16 x, y, z; };
#endif

namespace Particle {

/* The list element's exact source type and member spellings are not
 * RTTI-backed. The water callbacks prove the intrusive link and the two
 * ASR-3 position vectors below; the names are descriptive. */
struct ParticleNode {
    ParticleNode *next;          /* 0x00 */
    ParticleNode *previous;      /* 0x04 */
    Vector3 positionAsr3;        /* 0x08 */
    Vector3 offsetAsr3;          /* 0x14 */
    /* Splash callbacks test the y component before expiring the particle. */
    Vector3 velocityAsr3;        /* 0x20 */
    /* Water/lava boundary callbacks expire a particle by copying the first
     * value into the second. Roles and spellings are descriptive/inferred. */
    u16 lifetime;                /* 0x2c */
    u16 age;                     /* 0x2e */
};

struct ParticleList {
    ParticleNode *head;
    s32 count;
};

struct System {
    struct Flags {
        u32 unk_000 : 6;
        u32 unk_006 : 6;
        u32 unk_00c : 6;
        u32 unused  : 14;
    };

    void *previous;              /* 0x00 */
    void *next;                  /* 0x04 */
    ParticleList particles;      /* 0x08 */
    ParticleList particles2;     /* 0x10 */
    SystemDefinition *mDefinition; /* 0x18 */
    /* Callback virtuals toggle bit 1 while spawning or suppressing particles. */
    u32 callbackFlags;             /* 0x1c */
    Vector3 position;              /* 0x20 */
    u8 pad_02c[0x0e];
    /* simpleCallback_c copies this value in both directions. */
    s16 callbackValue;             /* 0x3a */
    Vector3_16f direction;         /* 0x3c */
    u8 pad_042[0x06];
    /* Descriptive names for callback-owned, body-proven fields. */
    s32 callbackParam;             /* 0x48 */
    s32 callbackVelocity;          /* 0x4c */
    s32 callbackScale;             /* 0x50 */
    u8 pad_054[0x20];
    Flags flags;                   /* 0x74 */

    static System *FromUniqueID(u32 uniqueID);
};

typedef char System_size_must_be_0x78[sizeof(System) == 0x78 ? 1 : -1];

}

extern "C" Particle::SysTracker *data_0209ee74;

#endif
