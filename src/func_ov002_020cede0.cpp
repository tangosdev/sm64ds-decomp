//cpp
struct Vector3 { int x, y, z; };
extern "C" {
extern void _ZN13RaycastGroundC1Ev(void* self);
extern void _ZN4BgCh19StartDetectingWaterEv(void* self);
extern void _ZN4BgCh21StopDetectingOrdinaryEv(void* self);
extern void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(void* self, void* v, void* act);
extern int _ZN13RaycastGround10DetectClsnEv(void* self);
extern int func_02037e78(int* p);
extern void _ZN13RaycastGroundD1Ev(void* self);
extern int WATER_HEIGHT;
extern int STAR_CAP_MAX_POS_Y;
extern signed char LEVEL_ID;
}

extern "C" int func_ov002_020cede0(char* thiz) {
  Vector3 v;
  char rg[0x54];
  int y = *(int*)(thiz + 0x60);
  int g = WATER_HEIGHT;
  if (g < y - 0x64000)
    return 0;
  {
    int d = g - y;
    if (d < 0) d = -d;
    if (d >= 0xc8000)
      return 1;
  }
  _ZN13RaycastGroundC1Ev(rg);
  {
    int z = *(int*)(thiz + 0x64);
    int yy = WATER_HEIGHT + 0xc8000;
    int x = *(int*)(thiz + 0x5c);
    v.x = x;
    v.y = yy;
    v.z = z;
  }
  _ZN4BgCh19StartDetectingWaterEv(rg);
  _ZN4BgCh21StopDetectingOrdinaryEv(rg);
  _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(rg, &v, thiz);
  if (_ZN13RaycastGround10DetectClsnEv(rg) != 0 && func_02037e78((int*)(rg + 0x14)) != 0) {
    _ZN13RaycastGroundD1Ev(rg);
    return 1;
  }
  v.y = STAR_CAP_MAX_POS_Y;
  _ZN4BgCh19StartDetectingWaterEv(rg);
  _ZN4BgCh21StopDetectingOrdinaryEv(rg);
  _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(rg, &v, thiz);
  if (_ZN13RaycastGround10DetectClsnEv(rg) != 0 && func_02037e78((int*)(rg + 0x14)) != 0) {
    *(int*)(thiz + 0x64c) = *(int*)(rg + 0x44);
    WATER_HEIGHT = *(int*)(thiz + 0x64c);
    _ZN13RaycastGroundD1Ev(rg);
    return 1;
  }
  if (LEVEL_ID == 0x21) {
    *(int*)(thiz + 0x64c) = 0x80000000;
    WATER_HEIGHT = *(int*)(thiz + 0x64c);
    _ZN13RaycastGroundD1Ev(rg);
    return 0;
  }
  _ZN13RaycastGroundD1Ev(rg);
  return 1;
}
