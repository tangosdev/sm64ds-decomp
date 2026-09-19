//cpp
#include "dActor_c.h"
#include "daPeach_c.h"

extern "C" int func_ov085_02129f8c(void *);

int daPeach_c::InitState0()
{
  char *c = (char *)this;
  ((dActor_c *)c)->UpdatePos((dCc_c *)(c+0x160));
  UpdateGroundCollision(&mWithMeshClsn);
  func_ov085_02129f8c(c);
  return 1;
}
