// @symbol _ZN11dScMiniGm_cD0Ev
/* dScMiniGm_c::~dScMiniGm_c() (deleting / D0) -- vtable slots 16/17. Same
 * teardown as D1, then deallocates. Plain C function carrying the literal
 * mangled name -- see include/dScMiniGm_c.h. */
#include "decl_common.h"
extern void _ZN7fBase_cD2Ev(void *self);
extern int data_0208e4b8[];
extern void *data_020a0eac;

int *_ZN11dScMiniGm_cD0Ev(int *t)
{
    t[0] = (int)data_ov005_020c2490;
    t[0] = (int)_ZTV8dScene_c;
    t[0] = (int)data_0208e4b8;
    _ZN7fBase_cD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
