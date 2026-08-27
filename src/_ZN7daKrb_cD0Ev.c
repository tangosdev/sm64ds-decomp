// @symbol _ZN7daKrb_cD0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_ModelAnim.h"
#include "decl_dCcAc_c.h"
#include "decl_ShadowModel.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV7daKrb_c */
extern void _ZN15MaterialChangerD1Ev(void *);
extern void _ZN11dCapEnemy_cD1Ev(void *);
extern void *data_020a0eac;
int *_ZN7daKrb_cD0Ev(int *t)
{
    t[0] = (int)_ZTV7daKrb_c;
    _ZN15MaterialChangerD1Ev((char *)t + 0x3fc);
    _ZN11ShadowModelD1Ev((char *)t + 0x3d4);
    _ZN9ModelAnimD1Ev((char *)t + 0x370);
    _ZN10dBgCh_ActrD1Ev((char *)t + 0x1b4);
    _ZN7dCcAc_cD1Ev((char *)t + 0x180);
    _ZN11dCapEnemy_cD1Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
