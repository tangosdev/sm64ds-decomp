//cpp
// @symbol func_ov002_020e7eb8
/* recovered: shared common types */
#include "common.h"
extern "C" {

extern void func_ov002_020e8244(Vector3* out, char* c);
extern int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
  int a0, unsigned int a1, int a2, int a3, int a4, void* a5, void* a6);
void func_ov002_020e7eb8(char* c){
  Vector3 v;
  int b = (*(unsigned short*)(c+0xc) == 0xb3);
  if (b == 0) return;
  func_ov002_020e8244(&v, c);
  *(int*)(c+0x4c0) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    *(int*)(c+0x4c0), 0x10e, v.x, v.y, v.z, 0, 0);
}
}
