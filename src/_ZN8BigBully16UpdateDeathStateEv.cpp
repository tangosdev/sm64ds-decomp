//cpp
// @symbol func_ov064_021175cc
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "daBDonketu_c.h"
// recovered name: daBDonketu_c_AfterClsn
/* recovered: renamed to Class_Method */
/* daBDonketu_c::AfterClsn - recovered from vtable slot identity */
extern "C" {
extern void _ZN8dActor_c14TriplePoofDustEv(void*);
extern void _ZN8dActor_c19UntrackAndSpawnStarERajRK7Vector3h(void*,void*,unsigned int,void*,unsigned int);
int func_ov064_021175cc(char* c){
    struct daBDonketu_c *self = (struct daBDonketu_c *)(void *)c;
  int r=func_ov064_0211616c(c);
  if(r==0) return r;
  _ZN8dActor_c14TriplePoofDustEv(c);
  int v[3];
  v[0]=self->mPosX;
  v[1]=self->mPosY;
  v[2]=self->mPosZ;
  v[1]+=0x64000;
  _ZN8dActor_c19UntrackAndSpawnStarERajRK7Vector3h(
    c, c+0x3fd, (self->mStarID|0x40)&0xff, v, 4);
}
}
