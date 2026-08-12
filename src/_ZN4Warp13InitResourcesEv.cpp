//cpp
// @symbol _ZN4Warp13InitResourcesEv
#include "Warp.h"
// recovered name: daWarpkun_c_InitResources
/* recovered: renamed to Class_Method */
/* daWarpkun_c::InitResources - recovered from vtable slot identity */

extern "C" {
typedef int Fix12i;
extern int _ZN5Actor9SetRangesE5Fix12IiES1_S1_S1_(void *, Fix12i, Fix12i, Fix12i, Fix12i);
extern int _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void *, void *, Fix12i, Fix12i, unsigned int, unsigned int);
}

s32 Warp::InitResources() {
    void * c = (void *)this;
  int v = *((int *) (((char *) c) + 8));
  int r = (((*((int *) (((char *) c) + 8))) & 0xf) + 1) << 0x12;
  _ZN5Actor9SetRangesE5Fix12IiES1_S1_S1_(c, 0, r, r + 0x7d0000, 0);
  _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(((char *) c) + 0xd4, c, r, ((((v & 0xFFFFFFFFu) >> 4) & 0xf) + 1) << 0x12, 2, 0x400000);
  return 1;
}
