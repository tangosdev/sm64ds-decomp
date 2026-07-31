//cpp
// @symbol func_ov027_021116f0
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "daIDonketu_c.h"
// @emits daIDonketu_c_AfterClsn
/* recovered: renamed to Class_Method */
/* daIDonketu_c::AfterClsn - recovered from vtable slot identity */
extern "C" {
extern void _ZN5Actor14TriplePoofDustEv(void*);
extern void _ZN5Actor19UntrackAndSpawnStarERajRK7Vector3j(void*,void*,unsigned int,void*,unsigned int);
int daIDonketu_c_AfterClsn(char* c){
    struct daIDonketu_c *self = (struct daIDonketu_c *)(void *)c;
  int r=func_ov064_0211616c(c);
  if(r==0) return r;
  _ZN5Actor14TriplePoofDustEv(c);
  int v[3];
  v[0]=self->unk_05c;
  v[1]=self->unk_060;
  v[2]=self->unk_064;
  v[1]+=0x64000;
  _ZN5Actor19UntrackAndSpawnStarERajRK7Vector3j(
    c, c+0x3fb, (self->unk_3fa|0x40)&0xff, v, 4);
}
}
