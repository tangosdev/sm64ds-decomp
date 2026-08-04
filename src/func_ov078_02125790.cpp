//cpp
// @symbol func_ov078_02125790
/* recovered: shared common types, declarations from a shared header */
#include "decl_Animation.h"
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"


extern "C" {
extern int _ZN5Actor18HorzAngleToCPlayerEv(void* a);
extern void ApproachAngle(short* p, int target, int step, int band, int max);
extern void func_02012694(int a, void* b);
extern void MulMat4x3Mat4x3(void* d, Matrix4x3* a, Matrix4x3* b);
extern void Vec3_Lsl(Vector3* d, Vector3* s, int sh);
extern int _ZN5Actor17HugeLandingDustAtER7Vector3b(void* a, Vector3* v, int b);
extern int _ZN9Animation8FinishedEv(void* a);
extern void KingBobOmb_SetState(char* c, void* p);
}
extern Matrix4x3 data_020a0e68;
extern int data_ov078_0212703c;

extern "C" int func_ov078_02125790(char* self)
{
  Vector3 s;
  Vector3 d;
  Vector3 v;
  if (func_ov078_02123804(self) == 1) return 1;
  ApproachAngle((short*)(self + 0x94), _ZN5Actor18HorzAngleToCPlayerEv(self), 1, 0x500, 0x500);
  *(short*)(self + 0x8e) = *(short*)(self + 0x94);
  if (_ZNK9Animation12WillHitFrameEi(self + 0x31c, 0x46)) {
    func_ov078_02125c24(self, 0x7d0000);
    func_02012694(0x12c, self + 0x74);
    s.x = 0;
    s.y = 0;
    s.z = 0;
    data_020a0e68 = *(Matrix4x3*)(self + 0x2e8);
    MulMat4x3Mat4x3((char*)*(void**)(self + 0x2e0) + 0x120, &data_020a0e68, &data_020a0e68);
    s.x = data_020a0e68.m[9];
    s.y = data_020a0e68.m[10];
    s.z = data_020a0e68.m[11];
    Vec3_Lsl(&d, &s, 3);
    s.x = d.x;
    v.x = d.x;
    s.y = d.y;
    v.y = d.y;
    s.z = d.z;
    v.z = d.z;
    _ZN5Actor17HugeLandingDustAtER7Vector3b(self, &v, 1);
  }
  if (_ZN9Animation8FinishedEv(self + 0x31c)) {
    KingBobOmb_SetState(self, &data_ov078_0212703c);
  }
  return 1;
}
