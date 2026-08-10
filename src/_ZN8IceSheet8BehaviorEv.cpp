//cpp
// @symbol _ZN8IceSheet8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "IceSheet.h"
#include "MeshColliderBase.h"
extern "C" {
}

int IceSheet::Behavior()
{
  if (!((MeshColliderBase *)((char*)&(*(u8 *)&mMeshCollider)))->IsEnabled())
    ((MeshColliderBase *)(&mMeshCollider))->Enable((Actor *)(((char*)this)));
  return 1;
}
