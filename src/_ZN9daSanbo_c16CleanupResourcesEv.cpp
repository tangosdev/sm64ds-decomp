//cpp
// @symbol _ZN9daSanbo_c16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "daSanbo_c.h"
#include "SharedFilePtr.h"
extern "C" {
void UnloadBlueCoinModel(void *);
extern int data_ov096_02137b20[];
extern int data_ov096_02137b28[];
}

int daSanbo_c::CleanupResources()
{
  int id = actorID;
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
