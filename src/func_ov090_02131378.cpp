//cpp
#include "dBgCh_Gnd.h"

extern "C" {
typedef struct dActor_c dActor_c;
extern void func_ov090_02130f94(void* c);
int Vec3_HorzDist(const Vector3* a, const Vector3* b);
extern unsigned char data_0209f2d8;
extern signed char data_0209f2f8;
extern int data_0209f32c;

void func_ov090_02131378(char* c){
  int b = (int)(data_0209f2d8 == 2);
  if (b != 0) {
    if (data_0209f2f8 == 0x12) {
      func_ov090_02130f94(c);
      return;
    }
  }
  dBgCh_Gnd rg;
  Vector3 v;
  int vz = *(int*)(c+0x64);
  int vy = *(int*)(c+0x60) + 0x32000;
  v.x = *(int*)(c+0x5c);
  v.y = vy;
  v.z = vz;
  *(unsigned char*)(c+0x39c) = 0;
  if (Vec3_HorzDist((Vector3*)(c+0x5c), (Vector3*)(c+0x68)) != 0) {
    rg.SetObjAndPos(v, (dActor_c*)c);
    if (rg.DetectClsn() != 0)
      *(int*)(c+0x3a8) = rg.clsnY;
  }
  if (*(int*)(c+0x3a8) < data_0209f32c) {
    *(int*)(c+0x3ac) = data_0209f32c;
    if (*(int*)(c+0x60) <= *(int*)(c+0x3ac)) {
      *(unsigned char*)(c+0x39c) = 1;
      func_ov090_02130f94(c);
    }
  } else {
    *(int*)(c+0x3ac) = *(int*)(c+0x3a8);
  }
  if (*(int*)(c+0x60) <= *(int*)(c+0x3ac)) {
    *(int*)(c+0x60) = *(int*)(c+0x3ac);
    *(int*)(c+0xa8) = 0;
  }
}
}
