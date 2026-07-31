// @symbol _ZN14UnknownVsEntryD0Ev
/* recovered: named members + shared header */
#include "UnknownVsEntry.h"
extern int func_0207328c(void* p, int a, int b, void* d);
extern void _ZN9ModelAnimD1Ev(void* p);
extern void _ZN5ModelD1Ev(void* p);
extern void _ZN8Particle10SysTrackerD1Ev(void* p);
extern void _ZN9ActorBaseD2Ev(void* p);
extern int _ZN6Memory10DeallocateEPvP4Heap(void* p, int h);
extern int _ZTV14UnknownVsEntry[];
extern int func_ov075_02113fdc;
extern int data_0208e4b8[];
extern int data_020a0eac[];
int _ZN14UnknownVsEntryD0Ev(struct UnknownVsEntry *self) {
  *(int**)((int*)self) = _ZTV14UnknownVsEntry;
  func_0207328c((char*)((int*)self)+0x920, 4, 0x158, &func_ov075_02113fdc);
  _ZN9ModelAnimD1Ev((char*)&self->mModelAnim);
  _ZN5ModelD1Ev((char*)&self->mModel);
  _ZN8Particle10SysTrackerD1Ev((char*)&self->mParticle);
  *(int**)((int*)self) = data_0208e4b8;
  _ZN9ActorBaseD2Ev(((int*)self));
  _ZN6Memory10DeallocateEPvP4Heap(((int*)self), data_020a0eac[0]);
  return (int)((int*)self);
}
