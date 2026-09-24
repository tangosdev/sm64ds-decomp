//cpp
// @symbol _ZThn16_N9dBgCh_GndD1Ev
/* Compiler-owned complete-destructor adjustment thunk for the dBgPi base. */
#include "dBgCh_Gnd.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT THUNK NAME, AND MSVC NEVER EMITS IT. mwccarm
 * emits this thunk beside the destructor below: `ldr ip,=-16; add r0,r0,ip;
 * b _ZN9dBgCh_GndD1Ev`, which moves `this` from the dBgPi base at +0x10 back to
 * the whole dBgCh_Gnd and runs its complete-object destructor. MSVC makes
 * adjustor thunks only for its own vftables, and those are not this one. This
 * arm is the same two steps, reached from slot 0 of the port's copy of the
 * ROM's secondary table VTable_dBgPi_dBgCh_GndThunk (0x02099274, filled in
 * port/hal/w2_dtor_heads.cpp). The destructor itself is defined by the class's D1
 * file; defining it here as well would define it twice under MSVC. Nothing
 * here reaches mwccarm: it builds the `#else` arm, byte for byte as before. */
extern "C" void _ZN9dBgCh_GndD1Ev(void *self);

extern "C" void _ZThn16_N9dBgCh_GndD1Ev(dBgPi *thiz)
{
    _ZN9dBgCh_GndD1Ev((void *)((char *)thiz - 16));
}
#else
dBgCh_Gnd::~dBgCh_Gnd()
{
}
#endif
