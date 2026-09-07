//cpp
// @symbol _ZN9BootSceneD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * Destroy through BootScene (which inlines ~dScene_c and ~dBase_c, see
 * _ZN9BootSceneD1Ev.cpp), then a real call to fBase_c::~fBase_c, then
 * hand the object back through dScene_c's inline operator delete -- declared on
 * dScene_c, BootScene's IMMEDIATE base, which is where mwcc finds it (it only
 * inlines an operator found on the class itself or its immediate base). The
 * hand-written version declared its own local `data_020a0eac` for the actor
 * heap, which collides with the `void *` dScene_c.h now supplies for that same
 * symbol.
 */
#include "BootScene.h"

BootScene::~BootScene()
{
}
