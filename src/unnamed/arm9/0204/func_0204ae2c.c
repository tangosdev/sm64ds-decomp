#include "types.h"
// @symbol func_0204ae2c
/* recovered: shared common types */
#include "common.h"
struct H2 { u16 a, b; };

/* Initialise a Particle::System that Particle::Manager::AddSystem
 * (Particle_Manager.cpp) has just taken from the free list: clear its links
 * and both particle lists, store the definition and position, zero the
 * velocity, age and callback value, and seed the emitter from the
 * definition's header.
 *
 * File-local views; Particle__System.h and Particle__Manager.h are C++.
 * The System view reuses that header's names plus func_0204a730.c's
 * (velocity, age, emitInterval, callback) and func_0204c584.c's (emitRadius,
 * particleLifetime). Header +0x12 is func_0204d294.c's color; the other
 * header fields are named for the System field this function seeds from
 * them. */
typedef struct PtclData {
    u8 pad_00[0x8];
    int emitRadius;       /* 0x08 -- System::emitRadius */
    u16 axis[3];          /* 0x0c -- System::direction; read unsigned (ldrh) */
    u16 color;            /* 0x12 */
    int callbackParam;    /* 0x14 -- System::callbackParam */
    int callbackVelocity; /* 0x18 -- System::callbackVelocity */
    int callbackScale;    /* 0x1c -- System::callbackScale */
    u8 pad_20[0xa];
    u16 particleLifetime; /* 0x2a -- System::particleLifetime */
    u8 pad_2c[0x4];
    u8 emitInterval; /* 0x30 -- System::emitInterval */
} PtclData;

typedef struct SystemDefinition {
    PtclData *data; /* 0x00 */
} SystemDefinition;

typedef struct PtclList {
    void *head; /* 0x00 */
    int count;  /* 0x04 */
} PtclList;

/* Particle::System, named as func_0204c584.c's view. */
struct ParticleSystem {
    void *previous;                /* 0x00 */
    void *next;                    /* 0x04 */
    PtclList particles;            /* 0x08 */
    PtclList particles2;           /* 0x10 */
    SystemDefinition *mDefinition; /* 0x18 */
    int callbackFlags;             /* 0x1c */
    struct Vector3 position;       /* 0x20 */
    struct Vector3 velocity;       /* 0x2c */
    u16 age;                       /* 0x38 */
    s16 callbackValue;             /* 0x3a */
    s16 direction[3];              /* 0x3c */
    char pad42[2];
    int emitRadius;       /* 0x44 */
    int callbackParam;    /* 0x48 */
    int callbackVelocity; /* 0x4c */
    int callbackScale;    /* 0x50 */
    u16 particleLifetime; /* 0x54 */
    char pad56[2];
    u8 emitInterval; /* 0x58 */
    u8 m59;          /* 0x59 -- header +0x31; no established role */
    char pad5a[0xe];
    void *callback; /* 0x68 */
    int m6c;        /* 0x6c */
    int m70;        /* 0x70 */
};

void func_0204ae2c(struct ParticleSystem *o, void *p18, struct Vector3 *src3) {
    o->mDefinition = p18;
    o->callbackFlags = 0;
    o->position = *src3;
    o->velocity.z = 0;
    o->velocity.y = o->velocity.z;
    o->velocity.x = o->velocity.y;
    o->age = 0;
    o->callbackValue = 0;
    {
        PtclData *s = o->mDefinition->data;
        *(struct H2 *)o->direction = *(struct H2 *)s->axis;
        o->direction[2] = s->axis[2];
        o->emitRadius = o->mDefinition->data->emitRadius;
        o->callbackParam = o->mDefinition->data->callbackParam;
        o->callbackVelocity = o->mDefinition->data->callbackVelocity;
        o->callbackScale = o->mDefinition->data->callbackScale;
        o->particleLifetime = o->mDefinition->data->particleLifetime;
        o->emitInterval = o->mDefinition->data->emitInterval;
        o->m59 = *(u8 *)((char *)(*(void **)(o->mDefinition)) + 0x31);
    }
    o->next = 0;
    o->previous = o->next;
    o->particles2.head = 0;
    o->particles.head = o->particles2.head;
    o->particles2.count = 0;
    o->particles.count = o->particles2.count;
    o->callback = 0;
    o->m6c = 0;
    o->m70 = 0;
}
