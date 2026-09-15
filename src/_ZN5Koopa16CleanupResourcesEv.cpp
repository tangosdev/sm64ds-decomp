//cpp
// @symbol _ZN5Koopa16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "Koopa.h"
#include "SharedFilePtr.h"
extern "C" {
extern void UnloadBlueCoinModel(void *c);
}
extern SharedFilePtr *data_ov062_0211cee0[];
extern SharedFilePtr *data_ov062_0211ced8[];
extern SharedFilePtr *data_ov062_0211cee8[];

int Koopa::CleanupResources()
{
  int b = (int) ((*((unsigned short *) ((char *)&actorID))) == 0xcc);
  if (b == 0)
  {
    data_ov062_0211cee0[*((int *) ((char *)&mModelIndex))]->Release();
  }
  data_ov062_0211ced8[*((int *) ((char *)&mModelIndex))]->Release();
  {
    int i = 0;
    do
    {
      data_ov062_0211cee8[(long) i]->Release();
      i++;
    }
    while (i < 9);
  }
  UnloadBlueCoinModel(((char *)this));
  return 1;
}
