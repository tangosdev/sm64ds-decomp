//cpp
// @symbol _ZN5Pokey16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "Pokey.h"
#include "SharedFilePtr.h"
extern "C" {
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
    ((SharedFilePtr *)(data_ov096_02137b20))->Release();
    ((SharedFilePtr *)(data_ov096_02137b28))->Release();
  } else {
    a = (id == 0xf1);
    if (a) {
      ((SharedFilePtr *)(data_ov096_02137b28))->Release();
    }
  }
  return 1;
}
