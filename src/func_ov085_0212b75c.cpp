//cpp
// @symbol func_ov085_0212b75c
/* recovered: shared common types */
#include "common.h"


extern "C" {
extern char *_ZN5Actor13ClosestPlayerEv(char *);
extern void _ZN7PathPtrC1Ev(void *);
extern void _ZN7PathPtr6FromIDEj(void *, unsigned int);
extern void _ZNK7PathPtr7GetNodeER7Vector3j(void *, struct Vector3 *, unsigned int);
extern int Vec3_Dist(const struct Vector3 *a, const struct Vector3 *b);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *, void *, int, int, unsigned int);
extern void *data_ov085_021305d0[];
}

extern "C" int func_ov085_0212b75c(char *c)
{
  char pathptr[8];
  int indices[2];
  struct Vector3 v;
  struct Vector3 nodes[2];
  int i;
  int d0;
  struct Vector3 *src;

  char *p = _ZN5Actor13ClosestPlayerEv(c);
  *(int *)(c + 0x41c) = 0;
  if (p)
  {
    src = (struct Vector3 *)(((long long)(int)(p + 0x5c)));
    v = *src;
    _ZN7PathPtrC1Ev(pathptr);
    _ZN7PathPtr6FromIDEj(pathptr, *(int *)(c + 0x438));

    indices[0] = *(int *)(c + 0x448) - 1;
    if (*(int *)(c + 0x448) - 1 < 0)
      indices[0] = *(int *)(c + 0x444) - 1;
    indices[1] = *(int *)(c + 0x448) + 1;
    if (*(int *)(c + 0x448) + 1 >= *(int *)(c + 0x444))
      indices[1] = 0;

    for (i = 0; i < 2; i++)
      _ZNK7PathPtr7GetNodeER7Vector3j(pathptr, &nodes[i], indices[i]);

    *(int *)(c + 0x44c) = 1;
    d0 = Vec3_Dist(&v, &nodes[0]);
    if (d0 > Vec3_Dist(&v, &nodes[1]))
      *(int *)(c + 0x44c) = -1;
  }

  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((void *)(c + 0x300), data_ov085_021305d0[1], 0, 0x1000, 0);
  return 1;
}
