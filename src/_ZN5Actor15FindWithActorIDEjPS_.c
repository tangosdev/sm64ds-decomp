// @symbol _ZN5Actor15FindWithActorIDEjPS_
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "Actor.h"
extern int data_0209b468[];
int _ZN5Actor15FindWithActorIDEjPS_(unsigned int j, int p) {
  int *r;
  if (p) r = func_02043f4c(data_0209b468, j, p+0x50);
  else r = func_02043f4c(data_0209b468, j, 0);
  if (r) return r[2];
  return 0;
}
