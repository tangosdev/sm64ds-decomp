// @symbol func_ov002_020aedbc
/* recovered: vtable identified, declarations from a shared header */
#include "decl_Model.h"
#include "decl_common.h"
extern void _ZN10dCapIcon_cD1Ev(void *);
extern int data_ov002_02108284[];
/* recovered: vtable identified */
/* vtable identified: VT0 = data_ov002_02108284 */
extern void _ZN12dEnemyBase_cD2Ev(void *);
int *func_ov002_020aedbc(int *t)
{
    t[0] = (int)data_ov002_02108284;
    _ZN10dCapIcon_cD1Ev((char *)t + 0x164);
    _ZN5ModelD1Ev((char *)t + 0x114);
    _ZN12dEnemyBase_cD2Ev(t);
    return t;
}
