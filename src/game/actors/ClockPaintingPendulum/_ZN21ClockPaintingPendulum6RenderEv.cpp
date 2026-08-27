//cpp
// @symbol _ZN21ClockPaintingPendulum6RenderEv
/* ClockPaintingPendulum::Render -- vtable slot 9, ov013 0x02111280. Declared
 * as an override in include/ClockPaintingPendulum.h; real C++ method here,
 * rendering the Model sub-object at +0xd4 through its own vtable. */
#include "ClockPaintingPendulum.h"
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(int); };
struct Derived { char pad[0xd4]; Base base; };
s32 ClockPaintingPendulum::Render() { Base *b = &((Derived *)this)->base; b->m(0); return 1; }
