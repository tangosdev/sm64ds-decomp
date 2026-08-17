//cpp
/* Scene::BeforeRender() at 0x0202e3a4, 0x24 bytes -- vtable slot 10.
 *
 * Chains to ActorBase's and narrows the result to 0 or 1. The narrowing is in the
 * ROM, not invented here: after the call the bytes are `cmp r0,#0; movne r0,#1;
 * moveq r0,#0`, which is exactly what a `!= 0` costs and would be absent if the
 * result were returned unchanged.
 *
 * Return type is `int` because ActorBase declares slot 10 as `int`; an override
 * whose return type differs is not an override at all, it is a nineteenth slot. */
#include "Scene.h"

int Scene::BeforeRender()
{
    return ActorBase::BeforeRender() != 0;
}
