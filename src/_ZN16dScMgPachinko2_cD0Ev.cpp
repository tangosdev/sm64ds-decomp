// @symbol func_ov006_020ff444
// recovered name: dScMgPachinko2_c_OnYoshiTryEat
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
extern void* data_020a0eac;
extern int data_ov006_0213dbbc[];
/* recovered: renamed to Class_Method */
/* dScMgPachinko2_c::OnYoshiTryEat - recovered from vtable slot identity */
int *func_ov006_020ff444(int *t)
{
    t[0] = (int)data_ov006_0213dbbc;
    _ZN11dScMgBase_cD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
