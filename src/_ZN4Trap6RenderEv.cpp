//cpp
// @symbol _ZN4Trap6RenderEv
/* Trap::Render -- vtable slot 9, ov010 0x021115a8. Defined here as a real
 * Trap::Render() method. A spawner Trap draws nothing; a spawned one draws its
 * ShadowModel at +0x320 through the model vtable (slot 5), still spelled as a
 * local shadow-vtable struct because that model is not yet a named member. */
#include "Trap.h"
struct B { virtual void m0(); virtual void m1(); virtual void m2(); virtual void m3(); virtual void m4(); virtual void m5(bool); };

int Trap::Render() {
    if (mIsSpawner == 0) {
        ((B *)((char *)this + 0x320))->m5(false);
    }
    return 1;
}
