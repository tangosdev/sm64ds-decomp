//cpp
// @symbol _ZN6ToxBox16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "ToxBox.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
extern "C" {
extern int data_ov092_02132540[];
extern int data_ov092_02132548[];
}

int ToxBox::CleanupResources()
{
  ((SharedFilePtr *)((void*)data_ov092_02132540))->Release();
  ((SharedFilePtr *)((void*)data_ov092_02132548))->Release();
  if(((dBgW *)((char*)&mMeshCollider))->IsEnabled())
    ((dBgW *)((char*)&mMeshCollider))->Disable();
  return 1;
}
