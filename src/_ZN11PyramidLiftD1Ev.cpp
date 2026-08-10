//cpp
// @symbol _ZN11PyramidLiftD1Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
#include "decl_common.h"
/* recovered: named members + shared header */
#include "PyramidLift.h"
extern "C" {
extern void __destroy_arr(void*,int,int,void*);
extern int _ZN7Vector3D1Ev();
extern int _ZTV8Platform[];
extern int _ZTV11PyramidLift[];
void* _ZN11PyramidLiftD1Ev(struct PyramidLift *self) {
  *(int**)((char*)self) = _ZTV11PyramidLift;
  __destroy_arr(((char*)self)+0x37c, 0xa, 0xc, (void*)_ZN7Vector3D1Ev);
  _ZN5ModelD1Ev((char*)&self->mModel2);
  *(int**)((char*)self) = _ZTV8Platform;
  _ZN18MovingMeshColliderD1Ev((char*)&self->mMeshCollider);
  _ZN5ModelD1Ev((char*)&self->mModel1);
  _ZN5ActorD2Ev(((char*)self));
  return ((char*)self);
}
}
