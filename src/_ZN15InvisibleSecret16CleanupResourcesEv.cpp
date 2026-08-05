//cpp
// @symbol _ZN15InvisibleSecret16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "InvisibleSecret.h"
#include "SharedFilePtr.h"
extern "C" {
extern int data_ov002_0210da28[];
extern int data_ov002_0210da08[];
extern int data_ov002_0210d9a8[];
extern int data_ov002_0210d9e8[];
}

int InvisibleSecret::CleanupResources()
{
  if(mParam & 0x10){
    ((SharedFilePtr *)(data_ov002_0210da28))->Release();
    ((SharedFilePtr *)(data_ov002_0210da08))->Release();
  } else {
    ((SharedFilePtr *)(data_ov002_0210d9a8))->Release();
    ((SharedFilePtr *)(data_ov002_0210d9e8))->Release();
  }
  return 1;
}
