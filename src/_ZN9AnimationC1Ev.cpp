//cpp
// @symbol _ZN9AnimationC1Ev
/* Stays a mangled free function: the destructor is the key function and
 * defining any structor as a real method risks the compiler emitting the
 * class vtable over the gap object's ROM copy. See include/ModelBase.h. */
#include "Animation.h"
extern "C" {
extern int _ZTV9Animation[];   /* vtable for Animation */
void _ZN9AnimationC1Ev(Animation *self) {
  *(int*)((char*)self) = (int)_ZTV9Animation;
  *(int*)((char*)&self->currFrame) = 0;
  *(int*)((char*)&self->speed) = 0x1000;
}
}
