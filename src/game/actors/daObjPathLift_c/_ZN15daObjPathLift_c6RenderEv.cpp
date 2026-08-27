//cpp
// @symbol _ZN15daObjPathLift_c6RenderEv
/* daObjPathLift_c::Render - name recovered from the vtable slot it fills.
   The body is a decompilation verified against the ROM, not an
   inferred stub. unk_428 is PathLift's own tail-padding, not this class's --
   see the header comment -- so it is read by raw offset. Real C++ method. */
#include "decl_common.h"
#include "daObjPathLift_c.h"
struct Obj { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(int); };
int daObjPathLift_c::Render(){
  unsigned short h = *(unsigned short*)((char*)this + 0x428);
  if(h < 0x5a){
    if(h & 1) return 1;
  }
  func_ov002_020efc74(this);
  ((Obj*)((char*)this+0xd4))->m(0);
  return 1;
}
