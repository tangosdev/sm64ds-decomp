//cpp
// @symbol func_ov010_021115a8
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "daObjC1_Trap_c.h"
// recovered name: daObjC1_Trap_c_Render
/* recovered: renamed to Class_Method */
/* daObjC1_Trap_c::Render - recovered from vtable slot identity */
extern "C" {
struct A { char pad[0x320]; };
struct B { virtual void m0(); virtual void m1(); virtual void m2(); virtual void m3(); virtual void m4(); virtual void m5(bool); };
int func_ov010_021115a8(char* c){
    struct daObjC1_Trap_c *self = (struct daObjC1_Trap_c *)(void *)c;
  if(self->unk_3ab == 0){
    ((B*)(c+0x320))->m5(false);
  }
  return 1;
}
}
