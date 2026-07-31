//cpp
// @symbol _ZN12PiranhaPlant16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "PiranhaPlant.h"
extern "C" {
extern void _ZN13SharedFilePtr7ReleaseEv(void*);
extern void UnloadBlueCoinModel(void*);
extern int data_ov084_02130dfc;
extern int* data_ov084_021302f4;
extern int data_ov002_0210da38;
}

int PiranhaPlant::CleanupResources()
{
  int i;
  _ZN13SharedFilePtr7ReleaseEv(&data_ov084_02130dfc);
  for(i=0;i<6;i++){
    _ZN13SharedFilePtr7ReleaseEv((&data_ov084_021302f4)[i]);
  }
  UnloadBlueCoinModel(((void*)this));
  _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0210da38);
  return 1;
}
