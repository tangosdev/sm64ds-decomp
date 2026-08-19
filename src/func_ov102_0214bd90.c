// @symbol func_ov102_0214bd90
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
extern int _ZNK10dBgCh_Actr13JustHitGroundEv(char* c);
extern void func_0200fc44(char* c, struct Vector3* v, int a, int z);
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(char* c);
extern void func_ov102_0214beb4(char* c);

void func_ov102_0214bd90(char* r4){
  if (_ZNK10dBgCh_Actr13JustHitGroundEv(r4 + 0x144)) {
    struct Vector3 v;
    v.x = *(int*)(r4 + 0x5c);
    v.y = *(int*)(r4 + 0x60);
    v.z = *(int*)(r4 + 0x64);
    func_0200fc44(r4, &v, 1, v.z);
  }
  if (_ZNK10dBgCh_Actr10IsOnGroundEv(r4 + 0x144)) {
    if (*(int*)(r4 + 0x38c) == 0) {
      *(int*)(r4 + 0x98) = 0x5000;
    }
    func_ov102_0214beb4(r4);
  } else {
    func_ov102_0214be1c(r4);
  }
}
