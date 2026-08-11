//cpp
// @symbol _ZN9BootSceneD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * Destroy through BootScene (which inlines ~Scene and ~ActorDerived, see
 * _ZN9BootSceneD1Ev.cpp), then a real call to ActorBase::~ActorBase, then
 * hand the object back through Scene's inline operator delete -- declared on
 * Scene, BootScene's IMMEDIATE base, which is where mwcc finds it (it only
 * inlines an operator found on the class itself or its immediate base). The
 * hand-written version declared its own local `data_020a0eac` for the actor
 * heap, which collides with the `void *` Scene.h now supplies for that same
 * symbol.
 */
#include "BootScene.h"

BootScene::~BootScene()
{
}
