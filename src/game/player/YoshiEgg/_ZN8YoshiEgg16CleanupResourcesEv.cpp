//cpp
// @symbol _ZN8YoshiEgg16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "YoshiEgg.h"
#include "SharedFilePtr.h"
extern "C" {
extern int func_ov002_020ec628(void*);
extern void UnloadBlueCoinModel(void*);
}
extern char data_ov002_0210e6b0;
extern char data_ov002_0210eb78;

int YoshiEgg::CleanupResources()
{
  ((SharedFilePtr *)(&data_ov002_0210e6b0))->Release();
  ((SharedFilePtr *)(&data_ov002_0210eb78))->Release();
  if (func_ov002_020ec628(((void*)this)) != 0)
    UnloadBlueCoinModel(((void*)this));
  return 1;
}
