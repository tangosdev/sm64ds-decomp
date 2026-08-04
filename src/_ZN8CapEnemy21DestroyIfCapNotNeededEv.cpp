//cpp
// @symbol _ZN8CapEnemy21DestroyIfCapNotNeededEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "CapEnemy.h"
extern unsigned char data_0209f2d8;
extern "C" {
extern unsigned char *_ZN5Actor13ClosestPlayerEv(unsigned char *t);
}

int CapEnemy::DestroyIfCapNotNeeded()
{
  if (((unsigned char *)this)[0x113] >= 6) return 1;
  if ((int)(data_0209f2d8 == 1) != 0) return 1;
  unsigned char *p = _ZN5Actor13ClosestPlayerEv(((unsigned char *)this));
  if (p == 0) { func_02005ed8(((unsigned char *)this)); return 0; }
  if (((unsigned char *)this)[0x113] != p[0x6d9]) return 1;
  func_02005ed8(((unsigned char *)this));
  return 0;
}
