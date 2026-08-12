// @symbol func_ov018_021126f8
// recovered name: daSCre_c_OnYoshiTryEat
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_common.h"
extern void* data_020a0eac;
extern int data_ov018_02113a74[];
/* recovered: renamed to Class_Method */
/* daSCre_c::OnYoshiTryEat - verified 2004/b56 byte-match (ov018), strict-reloc */
int *func_ov018_021126f8(int *t)
{
    t[0] = (int)data_ov018_02113a74;
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
