//cpp
// @symbol _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "dBgActor_c.h"
#include "dBgW.h"
extern "C" {
extern void* _ZN8dActor_c13ClosestPlayerEv(void*);
extern int Vec3_Dist(void*, void*);
int _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(struct dBgActor_c *self, int a, int b) {
  int on = (self->mFlags & 8) != 0;
  if (on) {
    if (((dBgW *)((char*)&self->mMeshCollider))->IsEnabled())
      ((dBgW *)((char*)&self->mMeshCollider))->Disable();
    return 0;
  }
  if (a == 0) {
    if (!((dBgW *)((char*)&self->mMeshCollider))->IsEnabled())
      ((dBgW *)(((char*)self)+0x124))->Enable((dActor_c *)(((char*)self)));
    goto done;
  }
  {
  struct Vector3 v;
  v.x = self->mPosX;
  v.y = self->mPosY;
  v.z = self->mPosZ;
  if (b == 0) v.y = v.y + self->mClipOffsetY;
  else v.y = v.y + b;
  void* p = _ZN8dActor_c13ClosestPlayerEv(((char*)self));
  int d = Vec3_Dist(&v, (char*)p+0x5c);
  if (d > a) {
    if (((dBgW *)((char*)&self->mMeshCollider))->IsEnabled())
      ((dBgW *)((char*)&self->mMeshCollider))->Disable();
    return 0;
  }
  if (!((dBgW *)((char*)&self->mMeshCollider))->IsEnabled())
    ((dBgW *)(((char*)self)+0x124))->Enable((dActor_c *)(((char*)self)));
  }
done:
  return 1;
}
}
