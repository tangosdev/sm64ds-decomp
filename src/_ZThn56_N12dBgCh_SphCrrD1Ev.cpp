//cpp
// @symbol _ZThn56_N12dBgCh_SphCrrD1Ev
/* Compiler-owned complete-destructor adjustment thunk for the dM3dGSph base. */
#include "dBgCh_SphCrr.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT THUNK NAME, AND MSVC NEVER EMITS IT. mwccarm
 * emits this thunk beside the destructor below: `ldr ip,=-56; add r0,r0,ip;
 * b _ZN12dBgCh_SphCrrD1Ev`, which moves `this` from the dM3dGSph base at +0x38 back to
 * the whole dBgCh_SphCrr and runs its complete-object destructor. MSVC makes
 * adjustor thunks only for its own vftables, and those are not this one. This
 * arm is the same two steps, reached from slot 0 of the port's copy of the
 * ROM's secondary table data_02099358 (0x02099358, filled in
 * port/hal/w2_dtor_heads.cpp). The destructor itself is defined by the class's D1
 * file; defining it here as well would define it twice under MSVC. Nothing
 * here reaches mwccarm: it builds the `#else` arm, byte for byte as before. */
extern "C" void _ZN12dBgCh_SphCrrD1Ev(void *self);

extern "C" void _ZThn56_N12dBgCh_SphCrrD1Ev(dM3dGSph *thiz)
{
    _ZN12dBgCh_SphCrrD1Ev((void *)((char *)thiz - 56));
}
#else
dBgCh_SphCrr::~dBgCh_SphCrr()
{
}
#endif
