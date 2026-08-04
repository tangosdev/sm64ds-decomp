// @symbol _ZN8CapEnemyD0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_Model.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV11dCapEnemy_c */
extern void func_ov002_020aed18(void *);
extern void *data_020a0eac;
int *_ZN8CapEnemyD0Ev(int *t)
{
    t[0] = (int)_ZTV11dCapEnemy_c;
    func_ov001_020ab3a0((char *)t + 0x164);
    _ZN5ModelD1Ev((char *)t + 0x114);
    func_ov002_020aed18(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
