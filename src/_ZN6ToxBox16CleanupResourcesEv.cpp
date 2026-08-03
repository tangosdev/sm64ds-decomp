//cpp
// @symbol _ZN6ToxBox16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "ToxBox.h"
#include "SharedFilePtr.h"
extern "C" {
int _ZN16MeshColliderBase9IsEnabledEv(void*);
int _ZN16MeshColliderBase7DisableEv(void*);
extern int data_ov092_02132540[];
extern int data_ov092_02132548[];
}

int ToxBox::CleanupResources()
{
  ((SharedFilePtr *)((void*)data_ov092_02132540))->Release();
  ((SharedFilePtr *)((void*)data_ov092_02132548))->Release();
  if(_ZN16MeshColliderBase9IsEnabledEv((char*)&mMeshCollider))
    _ZN16MeshColliderBase7DisableEv((char*)&mMeshCollider);
  return 1;
}
