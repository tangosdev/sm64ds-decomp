// @symbol func_ov002_020aedbc
/* recovered: vtable identified, declarations from a shared header */
#include "decl_Model.h"
#include "decl_common.h"
extern int data_ov002_02108284[];
/* recovered: vtable identified */
/* vtable identified: VT0 = data_ov002_02108284 */
extern void _ZN5EnemyD2Ev(void *);
int *func_ov002_020aedbc(int *t)
{
    t[0] = (int)data_ov002_02108284;
    func_ov001_020ab3a0((char *)t + 0x164);
    _ZN5ModelD1Ev((char *)t + 0x114);
    _ZN5EnemyD2Ev(t);
    return t;
}
