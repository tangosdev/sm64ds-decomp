//cpp
// @symbol func_ov027_0211181c
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "daIDonketu_c.h"
// recovered name: daIDonketu_c_InitResources
/* recovered: renamed to Class_Method */
/* daIDonketu_c::InitResources - recovered from vtable slot identity */
extern "C" {
extern int func_ov064_02116ec0(void*);
extern int _ZN5Actor9TrackStarEjj(void*, unsigned int, unsigned int);
int func_ov027_0211181c(char* c){
    struct daIDonketu_c *self = (struct daIDonketu_c *)(void *)c;
  self->unk_330 = (int)data_ov027_021138f4;
  int r = func_ov064_02116ec0(c);
  self->unk_3fa = *(int*)(c+8) & 0xf;
  self->unk_3fb = _ZN5Actor9TrackStarEjj(c, self->unk_3fa, 2);
  return r;
}
}
