//cpp
// @symbol _ZN10dBgW_KcMbg10DetectClsnER9dBgCh_Lin
/* recovered: named members + shared header, real C++ method
 *
 * Vtable slot 7. A moving collider is tested by moving the QUERY instead of the
 * mesh: both ends of the caller's line are pulled into the collider's local
 * frame by func_02039e48, the scratch dBgCh_Lin at 0x020a0d0c is aimed along
 * the transformed segment, and the base dBgW_Kc::DetectClsn does the real
 * work against the static mesh. A hit is then pushed back out to world space.
 *
 * clsnDist is carried across by hand rather than through the dBgPi copy:
 * it is read off the scratch line before func_020375ec overwrites the caller's
 * leading words, and restored after.
 */
#include "dBgW_KcMbg.h"
#include "dBgCh_Lin.h"

extern "C" {
extern int func_02039e48(void* a, void* b, void* c);
extern int _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(void* o, void* a, void* b, void* c);
extern int func_02035394(void* o, void* r);
extern int func_02039e30(void* o, void* a, void* b);
extern void func_020375ec(int* d, int* s);
extern int _ZN5dBgPiaSERKS_(void* d, void* s);
extern char data_020a0d0c[];
extern char data_020a0d60[];
extern char data_020a0d1c[];
}

int dBgW_KcMbg::DetectClsn(dBgCh_Lin & ray_)
{
  dBgCh_Lin* ray = &ray_;
  int sp0[3];
  int sp0xc[3];
  int sp0x18[3];
  func_02039e48(this, &ray->start, sp0);
  func_02039e48(this, &ray->lineEnd, sp0xc);
  unsigned char f50 = ray->hasClsn;
  _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(data_020a0d0c, sp0, sp0xc, 0);
  if (f50) *(unsigned char*)(data_020a0d0c + 0x50) = 1;
  func_02035394(data_020a0d0c, ray);
  int r = dBgW_Kc::DetectClsn(*(dBgCh_Lin*)data_020a0d0c);
  if (r) {
    int saved = *(int*)(data_020a0d0c + 0x60);
    func_02039e30(this, data_020a0d60, sp0x18);
    func_020375ec((int*)ray, sp0x18);
    ray->clsnDist = saved;
    /* the dBgPi base sub-object, at +0x10 */
    _ZN5dBgPiaSERKS_((char *)ray + 0x10, data_020a0d1c);
    ray->hasClsn = 1;
  }
  return r;
}
