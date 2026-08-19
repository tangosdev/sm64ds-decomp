// @symbol _ZN10dScTitle_cD1Ev
/* dScTitle_c::~dScTitle_c() (complete-object / D1) -- vtable slots 16/17
 * (destructor pair, actor-family convention -- fBase_c.h). Chains through
 * dScene_c/dBase_c's own vtables (inlined, per dScene_c.h) into fBase_c's
 * D2. Plain C carries the literal mangled name with no mangling needed --
 * see include/dScTitle_c.h. */
#include "decl_common.h"
extern void _ZN7fBase_cD2Ev(void *);
int *_ZN10dScTitle_cD1Ev(int *t)
{
    t[0] = (int)_ZTV10dScTitle_c;
    t[0] = (int)_ZTV8dScene_c;
    t[0] = (int)_ZTV7dBase_c;
    _ZN7fBase_cD2Ev(t);
    return t;
}
