//cpp
// @symbol func_ov002_020cede0
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"

extern "C" {
extern void _ZN9dBgCh_GndC1Ev(void* self);
extern void _ZN5dBgCh19StartDetectingWaterEv(void* self);
extern void _ZN5dBgCh21StopDetectingOrdinaryEv(void* self);
extern void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(void* self, void* v, void* act);
extern int _ZN9dBgCh_Gnd10DetectClsnEv(void* self);
extern int SurfaceInfo_TestFlag0x20(int* p);
extern void _ZN9dBgCh_GndD1Ev(void* self);
extern int data_0209f32c;
extern signed char data_0209f2f8;
}

extern "C" int func_ov002_020cede0(void* thiz_) {
    char* thiz = (char*)thiz_;
  Vector3 v;
  char rg[0x54];
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
  _ZN9dBgCh_GndC1Ev(rg);
  {
    int z = *(int*)(thiz + 0x64);
    int yy = data_0209f32c + 0xc8000;
    int x = *(int*)(thiz + 0x5c);
    v.x = x;
    v.y = yy;
    v.z = z;
  }
  _ZN5dBgCh19StartDetectingWaterEv(rg);
  _ZN5dBgCh21StopDetectingOrdinaryEv(rg);
  _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(rg, &v, thiz);
  if (_ZN9dBgCh_Gnd10DetectClsnEv(rg) != 0 && SurfaceInfo_TestFlag0x20((int*)(rg + 0x14)) != 0) {
    _ZN9dBgCh_GndD1Ev(rg);
    return 1;
  }
  v.y = data_0209212c;
  _ZN5dBgCh19StartDetectingWaterEv(rg);
  _ZN5dBgCh21StopDetectingOrdinaryEv(rg);
  _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(rg, &v, thiz);
  if (_ZN9dBgCh_Gnd10DetectClsnEv(rg) != 0 && SurfaceInfo_TestFlag0x20((int*)(rg + 0x14)) != 0) {
    *(int*)(thiz + 0x64c) = *(int*)(rg + 0x44);
    data_0209f32c = *(int*)(thiz + 0x64c);
    _ZN9dBgCh_GndD1Ev(rg);
    return 1;
  }
  if (data_0209f2f8 == 0x21) {
    *(int*)(thiz + 0x64c) = 0x80000000;
    data_0209f32c = *(int*)(thiz + 0x64c);
    _ZN9dBgCh_GndD1Ev(rg);
    return 0;
  }
  _ZN9dBgCh_GndD1Ev(rg);
  return 1;
}
