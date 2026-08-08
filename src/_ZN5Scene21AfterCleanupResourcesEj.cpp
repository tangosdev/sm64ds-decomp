//cpp
/* Scene::AfterCleanupResources(u32) at 0x0202e5d0, 0x20 bytes -- vtable slot 5.
 *
 * This is the function that proves how the `vfSuccess` argument arrives: the first
 * instruction is `cmp r1,#2`, so the parameter is in r1 and r0 is the `this` the
 * method never otherwise touches. Every other Before/After pair in the class
 * inherits its argument convention from this one reading.
 *
 * It ends in the same `ldr ip,[pc]; bx ip` tail call as the three-word veneers, in
 * a function that plainly is not one -- which is the evidence that the shape
 * belongs to -interworking tail calls in general and says nothing about branch
 * range. */
#include "Scene.h"

extern "C" {
extern u8 data_02092660;
}

void Scene::AfterCleanupResources(u32 vfSuccess)
{
    if (vfSuccess == 2)
        data_02092660 = 0;
    ActorBase::AfterCleanupResources(vfSuccess);
}
