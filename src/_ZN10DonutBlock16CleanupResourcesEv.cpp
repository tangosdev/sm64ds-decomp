//cpp
// @symbol _ZN10DonutBlock16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "DonutBlock.h"
#include "SharedFilePtr.h"
#include "MeshColliderBase.h"
extern "C" {
extern int* data_ov036_02113d78[];
}

int DonutBlock::CleanupResources()
{
  if(((MeshColliderBase *)((char*)&mMeshCollider))->IsEnabled()) ((MeshColliderBase *)((char*)&mMeshCollider))->Disable();
  ((SharedFilePtr *)(data_ov036_02113d78[0]))->Release();
  ((SharedFilePtr *)(data_ov036_02113d78[1]))->Release();
  return 1;
}
