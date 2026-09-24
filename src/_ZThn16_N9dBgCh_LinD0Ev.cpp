//cpp
// @symbol _ZThn16_N9dBgCh_LinD0Ev
/* Compiler-owned deleting-destructor adjustment thunk for the dBgPi base. */
#include "dBgCh_Lin.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT THUNK NAME, AND MSVC NEVER EMITS IT. mwccarm
 * emits this thunk beside the destructor below: `ldr ip,=-16; add r0,r0,ip;
 * b _ZN9dBgCh_LinD0Ev`, which moves `this` from the dBgPi base at +0x10 back to
 * the whole dBgCh_Lin and runs its deleting destructor. MSVC makes
 * adjustor thunks only for its own vftables, and those are not this one. This
 * arm is the same two steps, reached from slot 1 of the port's copy of the
 * ROM's secondary table data_020992b4 (0x020992b8, filled in
 * port/hal/w2_dtor_heads.cpp). The destructor itself is defined by the class's D1
 * file; defining it here as well would define it twice under MSVC. Nothing
 * here reaches mwccarm: it builds the `#else` arm, byte for byte as before. */
extern "C" dBgCh_Lin *_ZN9dBgCh_LinD0Ev(dBgCh_Lin *thiz);

extern "C" dBgCh_Lin *_ZThn16_N9dBgCh_LinD0Ev(dBgPi *thiz)
{
    return _ZN9dBgCh_LinD0Ev((dBgCh_Lin *)((char *)thiz - 16));
}
#else
dBgCh_Lin::~dBgCh_Lin()
{
}
#endif
