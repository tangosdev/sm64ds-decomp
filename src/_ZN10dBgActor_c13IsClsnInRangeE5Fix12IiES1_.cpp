//cpp
// @symbol _ZN8Platform13IsClsnInRangeE5Fix12IiES1_
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "Platform.h"
#include "MeshColliderBase.h"
extern "C" {
extern void* _ZN5Actor13ClosestPlayerEv(void*);
extern int Vec3_Dist(void*, void*);
int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(struct Platform *self, int a, int b) {
  struct Vector3 v;
  v.x = self->mPosX;
  v.y = self->mPosY;
  v.z = self->mPosZ;
  if (a == 0) a = self->unk_0b8 << 3;
  if (b == 0) v.y = v.y + self->unk_0b4;
  else v.y = v.y + b;
  void* p = _ZN5Actor13ClosestPlayerEv(((char*)self));
  int d = Vec3_Dist(&v, (char*)p+0x5c);
  if (d > a) {
    if (((MeshColliderBase *)((char*)&self->mMeshCollider))->IsEnabled())
      ((MeshColliderBase *)((char*)&self->mMeshCollider))->Disable();
    return 0;
  }
  if (!((MeshColliderBase *)((char*)&self->mMeshCollider))->IsEnabled())
    ((MeshColliderBase *)(((char*)self)+0x124))->Enable((Actor *)(((char*)self)));
  return 1;
}
}
