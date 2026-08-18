// @symbol _ZN11dScMiniGm_cD1Ev
/* dScMiniGm_c::~dScMiniGm_c() (complete-object / D1) -- vtable slots 16/17
 * (destructor pair, fBase_c/dScene_c convention). No fields to tear down
 * (all scalar); chains through dScene_c/dBase_c's own vtables (inlined,
 * per dScene_c.h) into fBase_c's D2. Plain C function carrying the literal
 * mangled name -- see include/dScMiniGm_c.h. */
#include "decl_common.h"
extern void _ZN7fBase_cD2Ev(void *);
int *_ZN11dScMiniGm_cD1Ev(int *t)
{
    t[0] = (int)_ZTV11dScMiniGm_c;
    t[0] = (int)_ZTV8dScene_c;
    t[0] = (int)_ZTV7dBase_c;
    _ZN7fBase_cD2Ev(t);
    return t;
}
