//cpp
// @symbol _ZN5Pokey16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "Pokey.h"
extern "C" {
void _ZN13SharedFilePtr7ReleaseEv(void *);
void UnloadBlueCoinModel(void *);
extern int data_ov096_02137b20[];
extern int data_ov096_02137b28[];
}

int Pokey::CleanupResources()
{
  int id = mActorID;
  int a = (id == 0xf0);
  if (a) {
    UnloadBlueCoinModel(((char *)this));
    _ZN13SharedFilePtr7ReleaseEv(data_ov096_02137b20);
    _ZN13SharedFilePtr7ReleaseEv(data_ov096_02137b28);
  } else {
    a = (id == 0xf1);
    if (a) {
      _ZN13SharedFilePtr7ReleaseEv(data_ov096_02137b28);
    }
  }
  return 1;
}
