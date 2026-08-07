// @symbol _ZN9RabbitKeyD0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_Model.h"
#include "decl_ShadowModel.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV15daObj_Mip_Key_c */
extern void _ZN5EnemyD2Ev(void *);
extern void *data_020a0eac;
int *_ZN9RabbitKeyD0Ev(int *t)
{
    t[0] = (int)_ZTV15daObj_Mip_Key_c;
    _ZN11ShadowModelD1Ev((char *)t + 0x160);
    _ZN5ModelD1Ev((char *)t + 0x110);
    _ZN5EnemyD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
