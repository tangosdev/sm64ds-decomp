//cpp
// @symbol _ZN6ToxBox16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "ToxBox.h"
#include "SharedFilePtr.h"
#include "MeshColliderBase.h"
extern "C" {
extern int data_ov092_02132540[];
extern int data_ov092_02132548[];
}

int ToxBox::CleanupResources()
{
  ((SharedFilePtr *)((void*)data_ov092_02132540))->Release();
  ((SharedFilePtr *)((void*)data_ov092_02132548))->Release();
  if(((MeshColliderBase *)((char*)&mMeshCollider))->IsEnabled())
    ((MeshColliderBase *)((char*)&mMeshCollider))->Disable();
  return 1;
}
