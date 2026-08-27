//cpp
// @symbol _ZN21ArmedRotatingPlatform16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "ArmedRotatingPlatform.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
extern "C" {
extern int* data_ov036_02113d78[];
}

int ArmedRotatingPlatform::CleanupResources()
{
  if(((dBgW *)((char*)&mMeshCollider))->IsEnabled()) ((dBgW *)((char*)&mMeshCollider))->Disable();
  ((SharedFilePtr *)(data_ov036_02113d78[0]))->Release();
  ((SharedFilePtr *)(data_ov036_02113d78[1]))->Release();
  return 1;
}
