// @symbol _ZN8BigBullyD0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_ModelAnim.h"
#include "decl_dCcAc_c.h"
#include "decl_ShadowModel.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
extern int _ZTV8BigBully[];
extern int data_ov064_0211b768[];
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV8BigBully */
extern void _ZN12dEnemyBase_cD2Ev(void *);
extern void *data_020a0eac;
int *_ZN8BigBullyD0Ev(int *t)
{
    t[0] = (int)_ZTV8BigBully;
    t[0] = (int)data_ov064_0211b768;
    _ZN11ShadowModelD1Ev((char *)t + 0x370);
    _ZN7dCcAc_cD1Ev((char *)t + 0x33c);
    _ZN10dBgCh_ActrD1Ev((char *)t + 0x174);
    _ZN9ModelAnimD1Ev((char *)t + 0x110);
    _ZN12dEnemyBase_cD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
