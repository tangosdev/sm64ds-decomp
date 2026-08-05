// @symbol func_ov006_020e065c
// recovered name: dScMgCurling_c_OnYoshiTryEat
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
extern void* data_020a0eac;
extern int data_ov006_0213c304[];
/* recovered: renamed to Class_Method */
/* dScMgCurling_c::OnYoshiTryEat - recovered from vtable slot identity */
int *func_ov006_020e065c(int *t)
{
    t[0] = (int)data_ov006_0213c304;
    func_ov004_020b29c0(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
