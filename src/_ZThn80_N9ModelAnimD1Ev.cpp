//cpp
#include "ModelAnim.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT THUNK NAME, AND MSVC NEVER EMITS IT. mwccarm
 * emits this thunk beside the destructor below: `ldr ip,=-80; add r0,r0,ip;
 * b _ZN9ModelAnimD1Ev`, which moves `this` from the Animation base at +0x50 back to
 * the whole ModelAnim and runs its complete-object destructor. MSVC makes
 * adjustor thunks only for its own vftables, and those are not this one. This
 * arm is the same two steps, reached from slot 0 of the port's copy of the
 * ROM's secondary table VTable_Animation_ModelAnimThunk (0x0208e9a4, filled in
 * port/hal/cxxname_bridge.cpp). The destructor itself is defined by the class's D1
 * file; defining it here as well would define it twice under MSVC. Nothing
 * here reaches mwccarm: it builds the `#else` arm, byte for byte as before. */
extern "C" void _ZN9ModelAnimD1Ev(void *self);

extern "C" void _ZThn80_N9ModelAnimD1Ev(Animation *thiz)
{
    _ZN9ModelAnimD1Ev((void *)((char *)thiz - 80));
}
#else
ModelAnim::~ModelAnim() {}
#endif
