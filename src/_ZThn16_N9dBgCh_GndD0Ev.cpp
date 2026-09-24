//cpp
// @symbol _ZThn16_N9dBgCh_GndD0Ev
/* Compiler-owned deleting-destructor adjustment thunk for the dBgPi base. */
#include "dBgCh_Gnd.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT THUNK NAME, AND MSVC NEVER EMITS IT. mwccarm
 * emits this thunk beside the destructor below: `ldr ip,=-16; add r0,r0,ip;
 * b _ZN9dBgCh_GndD0Ev`, which moves `this` from the dBgPi base at +0x10 back to
 * the whole dBgCh_Gnd and runs its deleting destructor. MSVC makes
 * adjustor thunks only for its own vftables, and those are not this one. This
 * arm is the same two steps, reached from slot 1 of the port's copy of the
 * ROM's secondary table VTable_dBgPi_dBgCh_GndThunk (0x02099278, filled in
 * port/hal/w2_dtor_heads.cpp). The destructor itself is defined by the class's D1
 * file; defining it here as well would define it twice under MSVC. Nothing
 * here reaches mwccarm: it builds the `#else` arm, byte for byte as before. */
extern "C" dBgCh_Gnd *_ZN9dBgCh_GndD0Ev(dBgCh_Gnd *thiz);

extern "C" dBgCh_Gnd *_ZThn16_N9dBgCh_GndD0Ev(dBgPi *thiz)
{
    return _ZN9dBgCh_GndD0Ev((dBgCh_Gnd *)((char *)thiz - 16));
}
#else
dBgCh_Gnd::~dBgCh_Gnd()
{
}
#endif
