//cpp
/* Scene::BeforeInitResources() at 0x0202e638, 0x34 bytes -- vtable slot 1.
 *
 * The header this file used to include declared the return type as `Bool`, a
 * spelling that existed nowhere but inside this one file's own `typedef int Bool;`
 * -- the generator had lifted the token out of the source without checking it named
 * anything visible from a header. Nothing ever caught it because Scene.h's
 * __cplusplus block had never been compiled by anybody. It is `bool` now, matching
 * ActorBase's declaration of slot 1, which is what makes this an override.
 *
 * `ResetFadersAndSound()` is called on `this`: the ROM branches to 0x0202e66c with
 * r0 untouched, which is a `this` passthrough and is why that one is not static. */
#include "Scene.h"

bool Scene::BeforeInitResources()
{
    if (!ResetFadersAndSound())
        return false;
    Initialise3dGraphics();
    return true;
}
