//cpp
// @symbol _ZN8IceSheet8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "IceSheet.h"
#include "MeshColliderBase.h"
extern "C" {
}

int IceSheet::Behavior()
{
  if (!((MeshColliderBase *)((char*)&mMovingMeshCollider))->IsEnabled())
    ((MeshColliderBase *)(((char*)this)+0x124))->Enable((Actor *)(((char*)this)));
  return 1;
}
