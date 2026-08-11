// @symbol func_ov006_020dbe64
// recovered name: dScMgCoin_c_OnYoshiTryEat
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
extern void* data_020a0eac;
extern int data_ov006_0213bf50[];
/* recovered: renamed to Class_Method */
/* dScMgCoin_c::OnYoshiTryEat - recovered from vtable slot identity */
int *func_ov006_020dbe64(int *t)
{
    t[0] = (int)data_ov006_0213bf50;
    _ZN11dScMgBase_cD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
