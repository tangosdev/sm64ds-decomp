//cpp
// @symbol _ZN14UnchainedChompD0Ev
/* recovered: named members + shared header
 *
 * Memory::Deallocate and data_020a0eac are NOT declared here. Enemy.h already
 * declares both -- as `void` and `void *` -- and a second extern "C" declaration
 * with a different type is not a redeclaration but an attempt to overload a
 * C-linkage name, which mwccarm rejects outright. */
#include "UnchainedChomp.h"
extern "C" {
int __destroy_arr(void*, int, int, void*);
int _ZN11ShadowModelD1Ev(void*);
int _ZN9ModelAnimD1Ev(void*);
int _ZN12WithMeshClsnD1Ev(void*);
int _ZN25MovingCylinderClsnWithPosD1Ev(void*);
int _ZN5EnemyD2Ev(void*);
extern int _ZTV14UnchainedChomp[];
extern void _ZN8Vector3sD1Ev();
extern void _ZN7Vector3D1Ev();
extern void _ZN5ModelD1Ev();
void* _ZN14UnchainedChompD0Ev(struct UnchainedChomp *self) {
  *(int**)((char*)self) = _ZTV14UnchainedChomp;
  __destroy_arr(((char*)self)+0x768, 6, 6, (void*)_ZN8Vector3sD1Ev);
  __destroy_arr(((char*)self)+0x720, 6, 0xc, (void*)_ZN7Vector3D1Ev);
  __destroy_arr(((char*)self)+0x6d8, 6, 0xc, (void*)_ZN7Vector3D1Ev);
  _ZN11ShadowModelD1Ev((char*)&self->mShadowModel);
  __destroy_arr(((char*)self)+0x550, 6, 0x28, (void*)_ZN11ShadowModelD1Ev);
  __destroy_arr(((char*)self)+0x370, 6, 0x50, (void*)_ZN5ModelD1Ev);
  _ZN9ModelAnimD1Ev((char*)&self->mModelAnim);
  _ZN12WithMeshClsnD1Ev((char*)&self->mWithMeshClsn);
  _ZN25MovingCylinderClsnWithPosD1Ev((char*)&self->mMovingCylinderClsnWithPos);
  _ZN5EnemyD2Ev(((char*)self));
  _ZN6Memory10DeallocateEPvP4Heap(((char*)self), data_020a0eac);
  return ((char*)self);
}
}
