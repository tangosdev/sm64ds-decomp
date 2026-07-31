//cpp
// @symbol _ZN5UnagiD0Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_BlendModelAnim.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: named members + shared header */
#include "Unagi.h"
extern "C" {
extern int func_0207328c(void*,int,int,void*);
extern void _ZN25MovingCylinderClsnWithPosD1Ev(void*);
extern int func_ov002_020aed18(int*);
extern void func_020072c0(void);
extern void* data_020a0eac[];
void* _ZN5UnagiD0Ev(struct Unagi *self) {
  *(int**)((char*)self) = _ZTV5Unagi;
  func_0207328c(((char*)self)+0x448, 7, 0xc, (void*)func_020072c0);
  _ZN14BlendModelAnimD1Ev((char*)&self->mBlendModelAnim);
  _ZN12WithMeshClsnD1Ev((char*)&self->mWithMeshClsn);
  _ZN25MovingCylinderClsnWithPosD1Ev((char*)&self->mMovingCylinderClsnWithPos2);
  _ZN25MovingCylinderClsnWithPosD1Ev((char*)&self->mMovingCylinderClsnWithPos1);
  func_ov002_020aed18((int*)((char*)self));
  _ZN6Memory10DeallocateEPvP4Heap(((char*)self), data_020a0eac[0]);
  return ((char*)self);
}
}
