//cpp
// @symbol _ZN5Koopa16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "Koopa.h"
struct SharedFilePtr
{
  unsigned int data[4];
};
extern void _ZN13SharedFilePtr7ReleaseEv(struct SharedFilePtr *);
extern void UnloadBlueCoinModel(void *c);
extern struct SharedFilePtr *data_ov062_0211cee0[];
extern struct SharedFilePtr *data_ov062_0211ced8[];
extern struct SharedFilePtr *data_ov062_0211cee8[];

int Koopa::CleanupResources()
{
  int b = (int) ((*((unsigned short *) ((char *)&mActorID))) == 0xcc);
  if (b == 0)
  {
    _ZN13SharedFilePtr7ReleaseEv(data_ov062_0211cee0[*((int *) ((char *)&mModelIndex))]);
  }
  _ZN13SharedFilePtr7ReleaseEv(data_ov062_0211ced8[*((int *) ((char *)&mModelIndex))]);
  {
    int i = 0;
    do
    {
      _ZN13SharedFilePtr7ReleaseEv(data_ov062_0211cee8[(long) i]);
      i++;
    }
    while (i < 9);
  }
  UnloadBlueCoinModel(((char *)this));
  return 1;
}
