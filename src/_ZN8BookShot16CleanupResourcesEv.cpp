//cpp
// @symbol _ZN8BookShot16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "BookShot.h"
extern "C" {
void _ZN13SharedFilePtr7ReleaseEv(void *);
void UnloadBlueCoinModel(void *);
extern int data_ov020_02114aa0;
extern int data_ov020_02114ab8;
extern int data_ov020_02114aa8;
extern int data_ov020_02114ab0;
}

int BookShot::CleanupResources()
{
  _ZN13SharedFilePtr7ReleaseEv(&data_ov020_02114aa0);
  _ZN13SharedFilePtr7ReleaseEv(&data_ov020_02114ab8);
  _ZN13SharedFilePtr7ReleaseEv(&data_ov020_02114aa8);
  _ZN13SharedFilePtr7ReleaseEv(&data_ov020_02114ab0);
  UnloadBlueCoinModel(((void *)this));
  return 1;
}
