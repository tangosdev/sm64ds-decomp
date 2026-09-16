//cpp
#include "dActor_c.h"
#include "PrincessPeach.h"

extern "C" int func_ov085_02129f8c(void *);

int PrincessPeach::InitState0()
{
  char *c = (char *)this;
  ((dActor_c *)c)->UpdatePos((dCc_c *)(c+0x160));
  UpdateGroundCollision(&mWithMeshClsn);
  func_ov085_02129f8c(c);
  return 1;
}
