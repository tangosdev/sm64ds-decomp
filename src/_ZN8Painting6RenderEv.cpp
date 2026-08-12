//cpp
// @symbol _ZN8Painting6RenderEv
#include "Painting.h"
// recovered name: daPicGate_c_Render
/* recovered: renamed to Class_Method */
/* daPicGate_c::Render - recovered from vtable slot identity */
struct C;
typedef int (C::*PMF)();
struct Entry { char pad[0x10]; PMF pmf; };
struct C { char pad[0x1a4]; Entry *ep; };
s32 Painting::Render() {
    C * c = (C *)this;
  (c->*c->ep->pmf)();
  return 1;
}
