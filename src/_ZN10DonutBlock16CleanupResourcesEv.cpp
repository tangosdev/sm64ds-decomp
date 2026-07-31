//cpp
// @symbol _ZN10DonutBlock16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "DonutBlock.h"
extern "C" {
extern int _ZN16MeshColliderBase9IsEnabledEv(void*);
extern int _ZN16MeshColliderBase7DisableEv(void*);
extern int _ZN13SharedFilePtr7ReleaseEv(void*);
extern int* data_ov036_02113d78[];
}

int DonutBlock::CleanupResources()
{
  if(_ZN16MeshColliderBase9IsEnabledEv((char*)&mMeshCollider)) _ZN16MeshColliderBase7DisableEv((char*)&mMeshCollider);
  _ZN13SharedFilePtr7ReleaseEv(data_ov036_02113d78[0]);
  _ZN13SharedFilePtr7ReleaseEv(data_ov036_02113d78[1]);
  return 1;
}
