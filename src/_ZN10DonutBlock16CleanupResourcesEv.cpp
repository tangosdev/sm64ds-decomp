//cpp
// @symbol _ZN10DonutBlock16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "DonutBlock.h"
#include "SharedFilePtr.h"
extern "C" {
extern int _ZN16MeshColliderBase9IsEnabledEv(void*);
extern int _ZN16MeshColliderBase7DisableEv(void*);
extern int* data_ov036_02113d78[];
}

int DonutBlock::CleanupResources()
{
  if(_ZN16MeshColliderBase9IsEnabledEv((char*)&mMeshCollider)) _ZN16MeshColliderBase7DisableEv((char*)&mMeshCollider);
  ((SharedFilePtr *)(data_ov036_02113d78[0]))->Release();
  ((SharedFilePtr *)(data_ov036_02113d78[1]))->Release();
  return 1;
}
