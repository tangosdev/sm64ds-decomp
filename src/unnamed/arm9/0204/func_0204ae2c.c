#include "types.h"
// @symbol func_0204ae2c
/* recovered: shared common types */
#include "common.h"
#include "private/spl_particle_views.h"
struct H2 {
    u16 a, b;
};

/* Initialise a Particle::System that Particle::Manager::AddSystem
 * (Particle_Manager.cpp) has just taken from the free list: clear its links
 * and both particle lists, store the definition and position, zero the
 * velocity, age and callback value, and seed the emitter from the
 * definition's header.
 *
 * The views are include/private/spl_particle_views.h's; the definition
 * header's fields are named for the System field this function seeds from
 * them. */
void func_0204ae2c(ParticleSystem *o, void *p18, struct Vector3 *src3) {
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
        o->baseAlpha = o->mDefinition->data->baseAlpha;
    }
    o->previous = 0;
    o->next = o->previous;
    o->particles2.head = 0;
    o->particles.head = o->particles2.head;
    o->particles2.count = 0;
    o->particles.count = o->particles2.count;
    o->callback = 0;
    o->m6c = 0;
    o->m70 = 0;
}
