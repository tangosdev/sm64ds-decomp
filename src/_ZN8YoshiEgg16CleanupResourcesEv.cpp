//cpp
// @symbol _ZN8YoshiEgg16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "YoshiEgg.h"
extern void _ZN13SharedFilePtr7ReleaseEv(void*);
extern int func_ov002_020ec628(void*);
extern void UnloadBlueCoinModel(void*);
extern char data_ov002_0210e6b0;
extern char data_ov002_0210eb78;

int YoshiEgg::CleanupResources()
{
  _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0210e6b0);
  _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0210eb78);
  if (func_ov002_020ec628(((void*)this)) != 0)
    UnloadBlueCoinModel(((void*)this));
  return 1;
}
