//cpp
// @symbol func_ov100_021470a4
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "daObjPathLift_c.h"
// recovered name: daObjPathLift_c_Render
/* recovered: renamed to Class_Method */
/* daObjPathLift_c::Render - name recovered from the vtable slot it fills.
   The body is a decompilation verified against the ROM, not an
   inferred stub. */
struct Obj { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(int); };
extern "C" {
int func_ov100_021470a4(char* c){
    struct daObjPathLift_c *self = (struct daObjPathLift_c *)(void *)c;
  unsigned short h = self->unk_428;
  if(h < 0x5a){
    if(h & 1) return 1;
  }
  func_ov002_020efc74(c);
  ((Obj*)(c+0xd4))->m(0);
  return 1;
}
}
