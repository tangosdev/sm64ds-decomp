//cpp
// @symbol _ZN8dActor_c4NextEPKS_
/* recovered: named members + shared header */
#include "dActor_c.h"
extern "C" {
struct dActor_c;
extern struct dActor_c* data_0209b468;
struct dActor_c* _ZN8dActor_c4NextEPKS_(struct dActor_c *self) {
  struct dActor_c* p;
  if(((struct dActor_c*)self)) p = *(struct dActor_c**)((char*)&self->unk_054);
  else  p = *(struct dActor_c**)&data_0209b468;
  if(p) return *(struct dActor_c**)((char*)p+8);
  return 0;
}
}
