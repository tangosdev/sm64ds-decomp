//cpp
// @symbol _ZN17MgBounceAndPounceD0Ev
/* recovered: named members + shared header */
#include "MgBounceAndPounce.h"
extern "C" {
extern int _ZN8Particle10SysTrackerD1Ev(void*);
extern int func_ov004_020b29c0(void*);
extern void _ZN6Memory10DeallocateEPvP4Heap(void*,void*);
extern int _ZTV17MgBounceAndPounce[];
extern void* data_020a0eac[];
int _ZN17MgBounceAndPounceD0Ev(struct MgBounceAndPounce *self) {
  *(int*)((char*)self)=(int)_ZTV17MgBounceAndPounce;
  _ZN8Particle10SysTrackerD1Ev((char*)&self->unk_47e4);
  func_ov004_020b29c0(((char*)self));
  _ZN6Memory10DeallocateEPvP4Heap(((char*)self),data_020a0eac[0]);
  return (int)((char*)self);
}
}
