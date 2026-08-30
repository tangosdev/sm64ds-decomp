//cpp
// @symbol func_ov002_020cede0
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
#include "dBgCh_Gnd.h"

extern "C" {
extern int SurfaceInfo_TestFlag0x20(int* p);
extern int data_0209f32c;
extern signed char data_0209f2f8;
}

extern "C" int func_ov002_020cede0(void* thiz_) {
    char* thiz = (char*)thiz_;
  Vector3 v;
  int y = *(int*)(thiz + 0x60);
  int g = data_0209f32c;
  if (g < y - 0x64000)
    return 0;
  {
    int d = g - y;
    if (d < 0) d = -d;
    if (d >= 0xc8000)
      return 1;
  }
  dBgCh_Gnd rg;
  {
    int z = *(int*)(thiz + 0x64);
    int yy = data_0209f32c + 0xc8000;
    int x = *(int*)(thiz + 0x5c);
    v.x = x;
    v.y = yy;
    v.z = z;
  }
  rg.StartDetectingWater();
  rg.StopDetectingOrdinary();
  rg.SetObjAndPos(v, (dActor_c*)thiz);
  if (rg.DetectClsn() != 0 && SurfaceInfo_TestFlag0x20((int*)&rg.surface) != 0) {
    return 1;
  }
  v.y = data_0209212c;
  rg.StartDetectingWater();
  rg.StopDetectingOrdinary();
  rg.SetObjAndPos(v, (dActor_c*)thiz);
  if (rg.DetectClsn() != 0 && SurfaceInfo_TestFlag0x20((int*)&rg.surface) != 0) {
    *(int*)(thiz + 0x64c) = rg.clsnY;
    data_0209f32c = *(int*)(thiz + 0x64c);
    return 1;
  }
  if (data_0209f2f8 == 0x21) {
    *(int*)(thiz + 0x64c) = 0x80000000;
    data_0209f32c = *(int*)(thiz + 0x64c);
    return 0;
  }
  return 1;
}
