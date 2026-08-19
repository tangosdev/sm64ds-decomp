// @symbol _ZN9dScDSMT_cD0Ev
/* dScDSMT_c::~dScDSMT_c() (deleting / D0) -- vtable slots 16/17. Same
 * teardown as D1, then deallocates. Plain C function carrying the literal
 * mangled name -- see include/dScDSMT_c.h. */
#include "decl_common.h"
extern void _ZN7fBase_cD2Ev(void *);
extern void *_ZN10dFdDummy_cD1Ev(void *);
extern void *GAME_HEAP_PTR;
int *_ZN9dScDSMT_cD0Ev(int *t)
{
    t[0] = (int)_ZTV9dScDSMT_c;
    _ZN10dFdDummy_cD1Ev((char *)t + 0x54);
    t[0] = (int)_ZTV8dScene_c;
    t[0] = (int)_ZTV7dBase_c;
    _ZN7fBase_cD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, GAME_HEAP_PTR);
    return t;
}
