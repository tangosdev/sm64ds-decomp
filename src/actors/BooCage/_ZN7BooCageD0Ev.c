// @symbol _ZN7BooCageD0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_Model.h"
#include "decl_dCcAc_c.h"
#include "decl_ShadowModel.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV11daTBasket_c */
extern void _ZN12dEnemyBase_cD2Ev(void *);
extern void *data_020a0eac;
int *_ZN7BooCageD0Ev(int *t)
{
    t[0] = (int)_ZTV11daTBasket_c;
    _ZN11ShadowModelD1Ev((char *)t + 0x350);
    _ZN5ModelD1Ev((char *)t + 0x300);
    _ZN10dBgCh_ActrD1Ev((char *)t + 0x144);
    _ZN7dCcAc_cD1Ev((char *)t + 0x110);
    _ZN12dEnemyBase_cD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
