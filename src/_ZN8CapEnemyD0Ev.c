// @symbol _ZN8CapEnemyD0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_Model.h"
#include "decl_common.h"
extern int data_ov002_02108284[];
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = data_ov002_02108284 */
extern void _ZN5EnemyD2Ev(void *);
extern void *data_020a0eac;
int *_ZN8CapEnemyD0Ev(int *t)
{
    t[0] = (int)data_ov002_02108284;
    func_ov001_020ab3a0((char *)t + 0x164);
    _ZN5ModelD1Ev((char *)t + 0x114);
    _ZN5EnemyD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
