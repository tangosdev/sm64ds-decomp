// @symbol func_ov006_020dabec
// @emits dScMgCard_c_Behavior
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScMgCard_c::Behavior - recovered from vtable slot identity */
extern void func_ov006_020c19d0(void*);
extern void func_ov004_020b65e4(void*);

int dScMgCard_c_Behavior(char* c){
  *(short*)(((int)c+0x5396) & 0xFFFFFFFFFFFFFFFF) += 1;
  func_ov006_020c19d0(c+0x4f38);
  func_ov004_020b65e4(func_ov006_020dac34(c));
  return 1;
}
