// @symbol _ZN10PyramidTagD0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_dCcAc_c.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV21daObjDlPyramidDummy_c */
extern void *data_020a0eac;
int *_ZN10PyramidTagD0Ev(int *t)
{
    t[0] = (int)_ZTV21daObjDlPyramidDummy_c;
    _ZN7dCcAc_cD1Ev((char *)t + 0xd4);
    _ZN8dActor_cD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
