//cpp
// @symbol _ZN12PiranhaPlant16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "PiranhaPlant.h"
#include "SharedFilePtr.h"
extern "C" {
extern void UnloadBlueCoinModel(void*);
extern int data_ov084_02130dfc;
extern int* data_ov084_021302f4;
extern int data_ov002_0210da38;
}

int PiranhaPlant::CleanupResources()
{
  int i;
  ((SharedFilePtr *)(&data_ov084_02130dfc))->Release();
  for(i=0;i<6;i++){
    ((SharedFilePtr *)((&data_ov084_021302f4)[i]))->Release();
  }
  UnloadBlueCoinModel(((void*)this));
  ((SharedFilePtr *)(&data_ov002_0210da38))->Release();
  return 1;
}
