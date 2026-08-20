// @symbol _ZN11WingFeatherD0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_dCcAc_c.h"
#include "decl_ShadowModel.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV11daFeather_c */
extern void *data_020a0eac;
int *_ZN11WingFeatherD0Ev(int *t)
{
    t[0] = (int)_ZTV11daFeather_c;
    _ZN11ShadowModelD1Ev((char *)t + 0x314);
    _ZN10dBgCh_ActrD1Ev((char *)t + 0x158);
    _ZN7dCcAc_cD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN8dActor_cD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
