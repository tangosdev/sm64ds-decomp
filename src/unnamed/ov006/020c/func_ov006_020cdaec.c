// @symbol func_ov006_020cdaec
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
extern void _ZN7Vector3D1Ev(void);

extern void func_020731dc(int a, int b, void** node);

void func_ov006_020cdaec(char* arg) {
  struct Vector3 a;
  struct Vector3 b;
  struct Vector3 c;
  if ((data_ov006_02140808 & 1) == 0) {
    data_ov006_02140860.x = 0;
    data_ov006_02140860.y = 0x1000;
    data_ov006_02140860.z = 0;
    func_020731dc((int)&data_ov006_02140860, (int)_ZN7Vector3D1Ev, &data_ov006_0214095c);
    data_ov006_02140808 |= 1;
  }
  if ((data_ov006_02140810 & 1) == 0) {
    data_ov006_02140890.x = 0;
    data_ov006_02140890.y = 0;
    data_ov006_02140890.z = 0x1000;
    func_020731dc((int)&data_ov006_02140890, (int)_ZN7Vector3D1Ev, &data_ov006_02140878);
    data_ov006_02140810 |= 1;
  }
  func_0203cc28((int*)(arg + 0x38), 0x100);
  func_0203ce80(&a, (struct Vector3*)(arg + 0x38));
  func_0203cf00(&b, (struct Vector3*)(arg + 0x38), &data_ov006_02140890);
  *(int*)(arg + 0x44) = b.x;
  *(int*)(arg + 0x48) = b.y;
  *(int*)(arg + 0x4c) = b.z;
  func_0203ce80(&c, (struct Vector3*)(arg + 0x44));
  Quaternion_FromVector3((int*)(arg + 0x74), &data_ov006_02140860, (struct Vector3*)(arg + 0x38));
  Quaternion_Normalize((int*)(arg + 0x74));
  func_ov006_020cdc8c(arg);
}
