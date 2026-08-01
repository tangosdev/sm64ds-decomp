// @symbol func_ov006_020cd864
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
extern void func_020072c0(void);

extern void func_020731dc(int a, int b, void** node);

void func_ov006_020cd864(char* arg) {
  struct Vector3 a;
  struct Vector3 b;
  struct Vector3 c;
  if ((data_ov006_02140820 & 1) == 0) {
    data_ov006_02140938.x = 0;
    data_ov006_02140938.y = 0x1000;
    data_ov006_02140938.z = 0;
    func_020731dc((int)&data_ov006_02140938, (int)func_020072c0, &data_ov006_02140920);
    data_ov006_02140820 |= 1;
  }
  if ((data_ov006_02140840 & 1) == 0) {
    data_ov006_02140884.x = 0;
    data_ov006_02140884.y = 0;
    data_ov006_02140884.z = 0x1000;
    func_020731dc((int)&data_ov006_02140884, (int)func_020072c0, &data_ov006_0214086c);
    data_ov006_02140840 |= 1;
  }
  func_0203cc28((int*)(arg + 0x38), 0x100);
  func_0203ce80(&a, (struct Vector3*)(arg + 0x38));
  func_0203cf00(&b, (struct Vector3*)(arg + 0x38), &data_ov006_02140884);
  *(int*)(arg + 0x44) = b.x;
  *(int*)(arg + 0x48) = b.y;
  *(int*)(arg + 0x4c) = b.z;
  func_0203ce80(&c, (struct Vector3*)(arg + 0x44));
  Quaternion_FromVector3((int*)(arg + 0x74), &data_ov006_02140938, (struct Vector3*)(arg + 0x38));
  Quaternion_Normalize((int*)(arg + 0x74));
  func_ov006_020cdc38(arg);
}
