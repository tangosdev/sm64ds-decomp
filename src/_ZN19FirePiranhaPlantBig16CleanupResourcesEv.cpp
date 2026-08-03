//cpp
// @symbol _ZN19FirePiranhaPlantBig16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "FirePiranhaPlantBig.h"
#include "SharedFilePtr.h"
extern "C" {
void UnloadBlueCoinModel(void*);
extern int data_ov084_02130dfc;
extern int data_ov002_0210da38;
}

int FirePiranhaPlantBig::CleanupResources()
{
  ((SharedFilePtr *)(&data_ov084_02130dfc))->Release();
  for (int i = 0; i < 6; i++) {
    ((SharedFilePtr *)(data_ov085_021302f4[i]))->Release();
  }
  ((SharedFilePtr *)(&data_ov002_0210da38))->Release();
  UnloadBlueCoinModel(((void *)this));
  return 1;
}
