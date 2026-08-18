// @symbol func_ov007_020cc070
/* recovered: vtable identified, renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: vtable identified, renamed to Class_Method */
extern void _ZN7fBase_cD2Ev(void *);
extern void *GAME_HEAP_PTR;
extern void *_ZN10dFdDummy_cD1Ev(void *);
int *func_ov007_020cc070(int *t)
{
    t[0] = (int)_ZTV9dScDSMT_c;
    _ZN10dFdDummy_cD1Ev((char *)t + 0x54);
    t[0] = (int)_ZTV8dScene_c;
    t[0] = (int)_ZTV7dBase_c;
    _ZN7fBase_cD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, GAME_HEAP_PTR);
    return t;
}
