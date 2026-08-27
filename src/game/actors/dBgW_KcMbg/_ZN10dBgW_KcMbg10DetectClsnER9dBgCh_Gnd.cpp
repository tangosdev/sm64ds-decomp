//cpp
// @symbol _ZN10dBgW_KcMbg10DetectClsnER9dBgCh_Gnd
/* recovered: named members + shared header, real C++ method
 *
 * Vtable slot 6. A downward ground probe answered as a LINE query: the probe
 * point and a second point b4c below it are pulled into the collider's local
 * frame, and the scratch dBgCh_Lin at 0x020a0d0c is aimed between them, so
 * the base dBgW_Kc::DetectClsn(dBgCh_Lin&) does the work.
 *
 * How far down to look is the interesting part. It starts at mProbeHeight, the
 * caller's search depth; but if the caller already HAS a hit (hasClsn), the
 * drop to that existing hit bounds the search instead, whenever that is
 * shorter. So a collider can only improve on the caller's best floor, never
 * report one below it -- which is what makes several moving colliders
 * queryable in sequence without ordering them.
 *
 * The volatile on the local is load-bearing and original: it forces the
 * probe's y to be re-read from the stack for the subtraction.
 */
#include "dBgW_KcMbg.h"
#include "dBgCh_Gnd.h"
#include "dBgCh_Lin.h"

extern "C" {
extern void func_020374b8(int* a, int* b);
extern int func_02039e48(void* a, void* b, void* c);
extern int _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(void* o, void* a, void* b, void* c);
extern int func_02035394(void* o, void* r);
extern int func_02039e30(void* o, void* a, void* b);
extern int _ZN5dBgPiaSERKS_(void* d, void* s);
extern char data_020a0d0c[];
extern char data_020a0d60[];
extern char data_020a0d1c[];
}

int dBgW_KcMbg::DetectClsn(dBgCh_Gnd & ray_)
{
  dBgCh_Gnd* ray = &ray_;
  int sp0[3];
  int sp0xc[3];
  int sp0x18[3];
  int sp0x24[3];
  int sp0x30[3];
  func_020374b8((int*)ray, sp0xc);
  sp0x24[0] = sp0xc[0];
  sp0x24[1] = sp0xc[1];
  sp0x24[2] = sp0xc[2];
  int b4c = ray->mProbeHeight;
  if (ray->hasClsn != 0) {
    int diff = *(volatile int*)&sp0xc[1] - *(int*)&ray->clsnY;
    if (diff < b4c) b4c = diff;
  }
  sp0x24[1] = sp0x24[1] - b4c;
  func_02039e48(this, sp0xc, sp0);
  func_02039e48(this, sp0x24, sp0x18);
  _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(data_020a0d0c, sp0, sp0x18, 0);
  func_02035394(data_020a0d0c, ray);
  int r = dBgW_Kc::DetectClsn(*(dBgCh_Lin*)data_020a0d0c);
  if (r) {
    func_02039e30(this, data_020a0d60, sp0x30);
    /* through the REFERENCE: a pointer-level upcast makes mwcc emit the
       null-checked MI adjustment (movs/addne), the ROM's is unconditional */
    _ZN5dBgPiaSERKS_(&(dBgPi &)*ray, data_020a0d1c);
    ray->clsnY = sp0x30[1];
    ray->hasClsn = 1;
  }
  return r;
}
