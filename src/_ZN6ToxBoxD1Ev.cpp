//cpp
// @symbol _ZN6ToxBoxD1Ev
/* recovered: named members + shared header */
#include "ToxBox.h"
extern "C" {
extern int _ZN25MovingCylinderClsnWithPosD1Ev(void*);
extern int _ZN12WithMeshClsnD1Ev(void*);
extern int _ZN18MovingMeshColliderD1Ev(void*);
extern int _ZN5ModelD1Ev(void*);
extern int _ZN5ActorD2Ev(void*);
extern void* _ZTV6ToxBox;
extern void* _ZTV17ExclamationSwitch;
void* _ZN6ToxBoxD1Ev(struct ToxBox *self) {
  *(void**)((char*)self) = &_ZTV6ToxBox;
  _ZN25MovingCylinderClsnWithPosD1Ev((char*)&self->mMovingCylinderClsnWithPos);
  _ZN12WithMeshClsnD1Ev((char*)&self->mWithMeshClsn);
  *(void**)((char*)self) = &_ZTV17ExclamationSwitch;
  _ZN18MovingMeshColliderD1Ev((char*)&self->mMeshCollider);
  _ZN5ModelD1Ev((char*)&self->mModel);
  _ZN5ActorD2Ev(((char*)self));
  return ((char*)self);
}
}
