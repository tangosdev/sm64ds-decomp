// @symbol func_ov005_020bfefc
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
extern void _ZN7fBase_cD2Ev(void *self);
extern int data_0208e4b8[];
extern void *data_020a0eac;

int *func_ov005_020bfefc(int *t)
{
    t[0] = (int)data_ov005_020c2490;
    t[0] = (int)_ZTV8dScene_c;
    t[0] = (int)data_0208e4b8;
    _ZN7fBase_cD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
