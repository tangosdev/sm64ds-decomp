//cpp
// @symbol _ZN4Trap6RenderEv
/* recovered: renamed to Class_Method, RTTI class fields named
 *
 * Trap::Render -- vtable slot 9, ov010 0x021115a8. Attributed by the vtable:
 * _ZTV4Trap carries this address at slot 9, where _ZTV10dBgActor_c carries
 * fBase_c's generic 0x02043af0.
 *
 * Kept as an extern "C" free function under the literal mangled name -- the
 * body is the same virtual-dispatch-through-a-shadow-vtable trick the file
 * used before the rename (Model's own Render, called through the Model
 * sub-object at +0x320), unconverted. include/Trap.h is included for the
 * `struct Trap` cast that reads unk_3ab by hand offset. */
#include "Trap.h"
extern "C" {
struct A { char pad[0x320]; };
struct B { virtual void m0(); virtual void m1(); virtual void m2(); virtual void m3(); virtual void m4(); virtual void m5(bool); };
int _ZN4Trap6RenderEv(char* c){
    struct Trap *self = (struct Trap *)(void *)c;
  if(self->unk_3ab == 0){
    ((B*)(c+0x320))->m5(false);
  }
  return 1;
}
}
