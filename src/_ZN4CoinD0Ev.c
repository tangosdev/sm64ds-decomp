// @symbol _ZN4CoinD0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_dCcAc_c.h"
#include "decl_ShadowModel.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV8daCoin_c */
extern void *data_020a0eac;
int *_ZN4CoinD0Ev(int *t)
{
    t[0] = (int)_ZTV8daCoin_c;
    _ZN10dBgCh_ActrD1Ev((char *)t + 0x1ac);
    _ZN7dCcAc_cD1Ev((char *)t + 0x178);
    _ZN11ShadowModelD1Ev((char *)t + 0x150);
    _ZN11CommonModelD1Ev((char *)t + 0x114);
    _ZN11CommonModelD1Ev((char *)t + 0xd8);
    _ZN8dActor_cD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
