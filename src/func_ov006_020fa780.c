// @symbol func_ov006_020fa780
// recovered name: dScMgPachinko_c_OnYoshiTryEat
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
extern void* data_020a0eac;
extern int data_ov006_0213d9cc[];
/* recovered: renamed to Class_Method */
/* dScMgPachinko_c::OnYoshiTryEat - recovered from vtable slot identity */
int *func_ov006_020fa780(int *t)
{
    t[0] = (int)data_ov006_0213d9cc;
    func_ov004_020b29c0(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
