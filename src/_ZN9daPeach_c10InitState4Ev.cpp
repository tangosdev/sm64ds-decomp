//cpp
#include "daPeach_c.h"

extern "C" {
void _ZN8dActor_c9UpdatePosEP5dCc_c(void*, void*);
int func_ov085_02129f8c(void*);
}
int daPeach_c::InitState4()
{
  char *c = (char *)this;
  short v = *(short*)(c+0x8e);
  *(short*)(c+0x94) = v;
  _ZN8dActor_c9UpdatePosEP5dCc_c(c, c+0x160);
  UpdateGroundCollision(&mWithMeshClsn);
  func_ov085_02129f8c(c);
  return 1;
}
