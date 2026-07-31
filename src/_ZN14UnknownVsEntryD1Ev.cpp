//cpp
// @symbol _ZN14UnknownVsEntryD1Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_Model.h"
#include "decl_ModelAnim.h"
#include "decl_common.h"
/* recovered: named members + shared header */
#include "UnknownVsEntry.h"
extern "C" {
extern int func_0207328c(void*, int, int, void*);
extern void _ZN8Particle10SysTrackerD1Ev(void*);
extern void _ZN9ActorBaseD2Ev(void*);
extern int func_ov075_02113fdc[];
extern int data_0208e4b8[];
void* _ZN14UnknownVsEntryD1Ev(struct UnknownVsEntry *self) {
  *(int**)((char*)self) = _ZTV14UnknownVsEntry;
  func_0207328c(((char*)self)+0x920, 4, 0x158, func_ov075_02113fdc);
  _ZN9ModelAnimD1Ev((char*)&self->mModelAnim);
  _ZN5ModelD1Ev((char*)&self->mModel);
  _ZN8Particle10SysTrackerD1Ev((char*)&self->mParticle);
  *(int**)((char*)self) = data_0208e4b8;
  _ZN9ActorBaseD2Ev(((char*)self));
  return ((char*)self);
}
}
