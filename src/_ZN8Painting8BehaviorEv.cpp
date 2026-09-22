//cpp
// @symbol _ZN8Painting8BehaviorEv
#include "Painting.h"
// recovered name: daPicGate_c_Behavior
/* recovered: renamed to Class_Method */
/* daPicGate_c::Behavior - recovered from vtable slot identity */
struct C;
typedef int (C::*PMF)();
struct Entry { char pad[0x8]; PMF pmf; };
struct C { char pad[0x1a4]; Entry *ep; };
s32 Painting::Behavior() {
    C * c = (C *)this;
  (c->*c->ep->pmf)();
  return 1;
}
