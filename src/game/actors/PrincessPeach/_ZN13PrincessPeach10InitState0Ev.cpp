//cpp
#include "PrincessPeach.h"

extern "C" void _ZN8dActor_c9UpdatePosEP5dCc_c(void *, void *);
extern "C" int func_ov085_02129f8c(void *);

int PrincessPeach::InitState0()
{
  char *c = (char *)this;
  _ZN8dActor_c9UpdatePosEP5dCc_c(c, c+0x160);
  UpdateGroundCollision(&mWithMeshClsn);
  func_ov085_02129f8c(c);
  return 1;
}
