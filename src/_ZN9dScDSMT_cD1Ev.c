// @symbol _ZN9dScDSMT_cD1Ev
/* dScDSMT_c::~dScDSMT_c() (complete-object / D1) -- vtable slots 16/17
 * (destructor pair, fBase_c/dScene_c convention). Tears down the opaque
 * unk_050 range via dFdDummy_c::~dFdDummy_c, then chains through dScene_c/dBase_c's
 * own vtables (inlined, per dScene_c.h) into fBase_c's D2. Plain C function
 * carrying the literal mangled name -- see include/dScDSMT_c.h. */
#include "decl_common.h"
extern void _ZN7fBase_cD2Ev(void *);
extern void *_ZN10dFdDummy_cD1Ev(void *);
int *_ZN9dScDSMT_cD1Ev(int *t)
{
    t[0] = (int)_ZTV9dScDSMT_c;
    _ZN10dFdDummy_cD1Ev((char *)t + 0x54);
    t[0] = (int)_ZTV8dScene_c;
    t[0] = (int)_ZTV7dBase_c;
    _ZN7fBase_cD2Ev(t);
    return t;
}
