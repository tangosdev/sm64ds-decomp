//cpp
// @symbol _ZN5SceneD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * Destroy through Scene (which inlines ~ActorDerived, see _ZN5SceneD1Ev.cpp),
 * then hand the object back through Scene's own inline operator delete --
 * declared there rather than on ActorDerived, because mwcc only inlines it
 * when found on the class itself or its IMMEDIATE base. The hand-written
 * version declared its own local `data_020a0eac`, which collides with the
 * `void *` Scene.h now supplies for that same symbol.
 */
#include "Scene.h"

Scene::~Scene()
{
}
