//cpp
// @symbol _ZN18MovingMeshCollider10DetectClsnER11RaycastLine
/* recovered: named members + shared header, real C++ method
 *
 * Vtable slot 7. A moving collider is tested by moving the QUERY instead of the
 * mesh: both ends of the caller's line are pulled into the collider's local
 * frame by func_02039e48, the scratch RaycastLine at 0x020a0d0c is aimed along
 * the transformed segment, and the base MeshCollider::DetectClsn does the real
 * work against the static mesh. A hit is then pushed back out to world space.
 *
 * clsnDist is carried across by hand rather than through the ClsnResult copy:
 * it is read off the scratch line before func_020375ec overwrites the caller's
 * leading words, and restored after.
 */
#include "MovingMeshCollider.h"
#include "RaycastLine.h"

extern "C" {
extern int func_02039e48(void* a, void* b, void* c);
extern int _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(void* o, void* a, void* b, void* c);
extern int func_02035394(void* o, void* r);
extern int func_02039e30(void* o, void* a, void* b);
extern void func_020375ec(int* d, int* s);
extern int _ZN10ClsnResultaSERKS_(void* d, void* s);
extern char data_020a0d0c[];
extern char data_020a0d60[];
extern char data_020a0d1c[];
}

int MovingMeshCollider::DetectClsn(RaycastLine & ray_)
{
  RaycastLine* ray = &ray_;
  int sp0[3];
  int sp0xc[3];
  int sp0x18[3];
  func_02039e48(this, &ray->unk_038, sp0);
  func_02039e48(this, &ray->lineEnd, sp0xc);
  unsigned char f50 = ray->hasClsn;
  _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(data_020a0d0c, sp0, sp0xc, 0);
  if (f50) *(unsigned char*)(data_020a0d0c + 0x50) = 1;
  func_02035394(data_020a0d0c, ray);
  int r = MeshCollider::DetectClsn(*(RaycastLine*)data_020a0d0c);
  if (r) {
    int saved = *(int*)(data_020a0d0c + 0x60);
    func_02039e30(this, data_020a0d60, sp0x18);
    func_020375ec((int*)ray, sp0x18);
    ray->clsnDist = saved;
    _ZN10ClsnResultaSERKS_(&ray->unk_010, data_020a0d1c);
    ray->hasClsn = 1;
  }
  return r;
}
