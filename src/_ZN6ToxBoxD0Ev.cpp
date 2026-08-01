//cpp
// @symbol _ZN6ToxBoxD0Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: named members + shared header */
#include "ToxBox.h"
extern "C" {
extern void _ZN25MovingCylinderClsnWithPosD1Ev(void*);
extern int _ZTV6ToxBox[];
extern int _ZTV17ExclamationSwitch[];
extern void* data_020a0eac;
void* _ZN6ToxBoxD0Ev(struct ToxBox *self) {
  *(int**)(((char*)self))=_ZTV6ToxBox;
  _ZN25MovingCylinderClsnWithPosD1Ev((char*)&self->mMovingCylinderClsnWithPos);
  _ZN12WithMeshClsnD1Ev((char*)&self->mWithMeshClsn);
  *(int**)(((char*)self))=_ZTV17ExclamationSwitch;
  _ZN18MovingMeshColliderD1Ev((char*)&self->mMeshCollider);
  _ZN5ModelD1Ev((char*)&self->mModel);
  _ZN5ActorD2Ev(((char*)self));
  _ZN6Memory10DeallocateEPvP4Heap(((char*)self), data_020a0eac);
  return ((char*)self);
}
}
