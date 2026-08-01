//cpp
// @symbol _ZN17MgBounceAndPounceD1Ev
/* recovered: named members + shared header */
#include "MgBounceAndPounce.h"
extern "C" {
extern int _ZN8Particle10SysTrackerD1Ev(void*);
extern int func_ov004_020b29c0(void*);
extern int _ZTV17MgBounceAndPounce[];
int _ZN17MgBounceAndPounceD1Ev(struct MgBounceAndPounce *self) {
  *(int*)((char*)self)=(int)_ZTV17MgBounceAndPounce;
  _ZN8Particle10SysTrackerD1Ev((char*)&self->unk_47e4);
  func_ov004_020b29c0(((char*)self));
  return (int)((char*)self);
}
}
