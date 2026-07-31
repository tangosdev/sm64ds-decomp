//cpp
// @symbol func_ov064_02117a44
/* recovered: shared common types */
#include "common.h"

extern "C" {
void _ZN5Actor9UpdatePosEP12CylinderClsn(char* self, void* c);
int Vec3_HorzDist(const void* a, const void* b);
int _Z14ApproachLinearRiii(int* v, int target, int step);
void _ZNK7PathPtr7GetNodeER7Vector3j(void* self, void* v, unsigned int j);
void func_ov064_02117a14(char* c, Vector3* a, Vector3* b);
}

extern "C" int func_ov064_02117a44(char* c) {
  Vector3 a;
  Vector3 b;
  int r;
  _ZN5Actor9UpdatePosEP12CylinderClsn(c, 0);
  r = (Vec3_HorzDist(c+0x5c, c+0x354) < 0x69000) ? 0 : 0xa000;
  if (_Z14ApproachLinearRiii((int*)(c+0x98), r, 0x800) != 0 && r == 0) {
    *(int*)(c+0x348) = *(int*)(c+0x354);
    *(int*)(c+0x34c) = *(int*)(c+0x358);
    *(int*)(c+0x350) = *(int*)(c+0x35c);
    *(int*)(c+0x5c) = *(int*)(c+0x348);
    *(int*)(c+0x60) = *(int*)(c+0x34c);
    *(int*)(c+0x64) = *(int*)(c+0x350);
    r = 1;
    if (*(unsigned char*)(c+0x33b) == 1) {
      ++*(int*)(((int)c + 0x344) & 0xFFFFFFFFFFFFFFFF);
      if (*(int*)(c+0x344) >= *(int*)(c+0x340)) {
        *(int*)(c+0x344) = *(int*)(c+0x340) - 2;
        r = -1;
      }
    } else {
      --*(int*)(((int)c + 0x344) & 0xFFFFFFFFFFFFFFFF);
      if (*(int*)(c+0x344) < 0) {
        *(int*)(c+0x344) = r;
        r = -1;
      }
    }
    _ZNK7PathPtr7GetNodeER7Vector3j(c+0x360, c+0x354, *(int*)(c+0x344));
    a.x = *(int*)(c+0x354);
    a.y = *(int*)(c+0x358);
    a.z = *(int*)(c+0x35c);
    b.x = *(int*)(c+0x348);
    b.y = *(int*)(c+0x34c);
    b.z = *(int*)(c+0x350);
    func_ov064_02117a14(c, &a, &b);
    return r;
  }
  return 0;
}
