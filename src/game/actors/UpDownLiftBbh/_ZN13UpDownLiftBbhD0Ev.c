// @symbol _ZN13UpDownLiftBbhD0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_dBgW_KcMbg.h"
#include "decl_common.h"
extern int _ZTV13UpDownLiftBbh[];
extern int _ZTV10dBgActor_c[];
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV13UpDownLiftBbh; VT1 = _ZTV10dBgActor_c */
extern void *data_020a0eac;
int *_ZN13UpDownLiftBbhD0Ev(int *t)
{
    t[0] = (int)_ZTV13UpDownLiftBbh;
    t[0] = (int)_ZTV10dBgActor_c;
    _ZN10dBgW_KcMbgD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN8dActor_cD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
