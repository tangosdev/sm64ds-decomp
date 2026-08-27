//cpp
#include "dActor_c.h"
// @symbol _ZN8dActor_c15FindWithActorIDEjPS_
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "dActor_c.h"
extern int data_0209b468[];
dActor_c *dActor_c::FindWithActorID(u32 j, dActor_c *after) {
  int p = (int)after;
  int *r;
  if (p) r = func_02043f4c(data_0209b468, j, p+0x50);
  else r = func_02043f4c(data_0209b468, j, 0);
  if (r) return (dActor_c *)r[2];
  return 0;
}
