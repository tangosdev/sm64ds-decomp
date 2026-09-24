//cpp
// @symbol _ZThn80_N8daDemo_c10anmModel_cD1Ev
/* Compiler-owned complete-destructor adjustment thunk. The source describes
   the real destructor; mwccarm emits the -0x50 thunk named by this file as a
   byproduct of the virtual ScaleHolder base in daDemo_c.h. */
#include "daDemo_c.h"
#include "SharedFilePtr.h"

typedef void (*VFN)(void *);

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT THUNK NAME, AND MSVC NEVER EMITS IT. mwccarm
 * emits this thunk beside the destructor below: `ldr ip,=-80; add r0,r0,ip;
 * b _ZN8daDemo_c10anmModel_cD1Ev`, which moves `this` from the Animation base at +0x50 back to
 * the whole daDemo_c::anmModel_c and runs its complete-object destructor. MSVC makes
 * adjustor thunks only for its own vftables, and those are not this one. This
 * arm is the same two steps, reached from slot 0 of the port's copy of the
 * ROM's secondary table data_ov002_0210bce8 (0x0210bce8, filled in
 * port/hal/w2_dtor_heads.cpp). The destructor itself is defined by the class's D1
 * file; defining it here as well would define it twice under MSVC. Nothing
 * here reaches mwccarm: it builds the `#else` arm, byte for byte as before. */
extern "C" void _ZN8daDemo_c10anmModel_cD1Ev(void *self);

extern "C" void _ZThn80_N8daDemo_c10anmModel_cD1Ev(Animation *thiz)
{
    _ZN8daDemo_c10anmModel_cD1Ev((void *)((char *)thiz - 80));
}
#else
daDemo_c::anmModel_c::~anmModel_c()
{
    char *c = (char *)this;
    void *p;
    int i;

    p = *(void **)(c + 0x70);
    if (p != 0) ((SharedFilePtr *)(p))->Release();
    for (i = 0; i < *(unsigned char *)(c + 0x80); i++) {
        p = (*(void ***)(c + 0x74))[i];
        if (p != 0) ((SharedFilePtr *)(p))->Release();
    }
    if (*(void **)(c + 0x7c) != 0) {
        for (i = 0; i < *(unsigned char *)(c + 0x81); i++) {
            p = (*(void ***)(c + 0x78))[i];
            if (p != 0) ((SharedFilePtr *)(p))->Release();
        }
        p = *(void **)(c + 0x7c);
        if (p != 0) {
            (*(VFN)((*(int **)p)[1]))(p);
        }
    }
}
#endif
