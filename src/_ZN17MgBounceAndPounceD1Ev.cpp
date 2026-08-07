//cpp
// @symbol _ZN17MgBounceAndPounceD1Ev
/* recovered: named members + shared header */
#include "MgBounceAndPounce.h"
extern "C" {
extern int _ZN8Particle10SysTrackerD1Ev(void*);
extern int _ZN11dScMgBase_cD2Ev(void*);
extern int _ZTV17MgBounceAndPounce[];
int _ZN17MgBounceAndPounceD1Ev(struct MgBounceAndPounce *self) {
  *(int*)((char*)self)=(int)_ZTV17MgBounceAndPounce;
  _ZN8Particle10SysTrackerD1Ev((char*)&self->unk_47e4);
  _ZN11dScMgBase_cD2Ev(((char*)self));
  return (int)((char*)self);
}
}
