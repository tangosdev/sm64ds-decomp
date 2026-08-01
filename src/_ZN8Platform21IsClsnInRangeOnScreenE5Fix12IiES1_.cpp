//cpp
// @symbol _ZN8Platform21IsClsnInRangeOnScreenE5Fix12IiES1_
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "Platform.h"
extern "C" {
extern void _ZN16MeshColliderBase6EnableEP5Actor(void*, void*);
extern void* _ZN5Actor13ClosestPlayerEv(void*);
extern int Vec3_Dist(void*, void*);
int _ZN8Platform21IsClsnInRangeOnScreenE5Fix12IiES1_(struct Platform *self, int a, int b) {
  int on = (self->unk_0b0 & 8) != 0;
  if (on) {
    if (_ZN16MeshColliderBase9IsEnabledEv((char*)&self->mMeshCollider))
      _ZN16MeshColliderBase7DisableEv((char*)&self->mMeshCollider);
    return 0;
  }
  if (a == 0) {
    if (!_ZN16MeshColliderBase9IsEnabledEv((char*)&self->mMeshCollider))
      _ZN16MeshColliderBase6EnableEP5Actor(((char*)self)+0x124, ((char*)self));
    goto done;
  }
  {
  struct Vector3 v;
  v.x = self->mPosX;
  v.y = self->mPosY;
  v.z = self->mPosZ;
  if (b == 0) v.y = v.y + self->unk_0b4;
  else v.y = v.y + b;
  void* p = _ZN5Actor13ClosestPlayerEv(((char*)self));
  int d = Vec3_Dist(&v, (char*)p+0x5c);
  if (d > a) {
    if (_ZN16MeshColliderBase9IsEnabledEv((char*)&self->mMeshCollider))
      _ZN16MeshColliderBase7DisableEv((char*)&self->mMeshCollider);
    return 0;
  }
  if (!_ZN16MeshColliderBase9IsEnabledEv((char*)&self->mMeshCollider))
    _ZN16MeshColliderBase6EnableEP5Actor(((char*)self)+0x124, ((char*)self));
  }
done:
  return 1;
}
}
