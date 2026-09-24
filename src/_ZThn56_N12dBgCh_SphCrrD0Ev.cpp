//cpp
// @symbol _ZThn56_N12dBgCh_SphCrrD0Ev
/* Compiler-owned deleting-destructor adjustment thunk for the dM3dGSph base. */
#include "dBgCh_SphCrr.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT THUNK NAME, AND MSVC NEVER EMITS IT. mwccarm
 * emits this thunk beside the destructor below: `ldr ip,=-56; add r0,r0,ip;
 * b _ZN12dBgCh_SphCrrD0Ev`, which moves `this` from the dM3dGSph base at +0x38 back to
 * the whole dBgCh_SphCrr and runs its deleting destructor. MSVC makes
 * adjustor thunks only for its own vftables, and those are not this one. This
 * arm is the same two steps, reached from slot 1 of the port's copy of the
 * ROM's secondary table data_02099358 (0x0209935c, filled in
 * port/hal/w2_dtor_heads.cpp). The destructor itself is defined by the class's D1
 * file; defining it here as well would define it twice under MSVC. Nothing
 * here reaches mwccarm: it builds the `#else` arm, byte for byte as before. */
extern "C" dBgCh_SphCrr *_ZN12dBgCh_SphCrrD0Ev(dBgCh_SphCrr *thiz);

extern "C" dBgCh_SphCrr *_ZThn56_N12dBgCh_SphCrrD0Ev(dM3dGSph *thiz)
{
    return _ZN12dBgCh_SphCrrD0Ev((dBgCh_SphCrr *)((char *)thiz - 56));
}
#else
dBgCh_SphCrr::~dBgCh_SphCrr()
{
}
#endif
