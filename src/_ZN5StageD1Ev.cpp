//cpp
// @symbol _ZN5StageD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * Destroy through Stage's own three fields -- MeshCollider at 0x91c, Model at
 * 0x86c, Particle::SysTracker at 0x50 -- in reverse declaration order, then
 * Scene (now inline-defined in Scene.h, so its vptr store and ActorDerived's
 * inline in turn), then ActorBase's subobject destructor, called rather than
 * inlined because ActorBase's is not.
 */
#include "Stage.h"

Stage::~Stage()
{
}
