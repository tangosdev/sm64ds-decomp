//cpp
// @symbol _ZN19FirePiranhaPlantBig16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "FirePiranhaPlantBig.h"
extern "C" {
void _ZN13SharedFilePtr7ReleaseEv(void*);
void UnloadBlueCoinModel(void*);
extern int data_ov084_02130dfc;
extern int data_ov002_0210da38;
}

int FirePiranhaPlantBig::CleanupResources()
{
  _ZN13SharedFilePtr7ReleaseEv(&data_ov084_02130dfc);
  for (int i = 0; i < 6; i++) {
    _ZN13SharedFilePtr7ReleaseEv(data_ov085_021302f4[i]);
  }
  _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0210da38);
  UnloadBlueCoinModel(((void *)this));
  return 1;
}
