// @symbol _ZN15daObjMarioCap_cD0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_ModelAnim.h"
#include "decl_dCcAc_c.h"
#include "decl_ShadowModel.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
extern void _ZN10dCapIcon_cD1Ev(void *);
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV15daObjMarioCap_c */
extern void _ZN12dEnemyBase_cD2Ev(void *);
extern void *data_020a0eac;
int *_ZN15daObjMarioCap_cD0Ev(int *t)
{
    t[0] = (int)_ZTV15daObjMarioCap_c;
    _ZN10dCapIcon_cD1Ev((char *)t + 0x3d0);
    _ZN11ShadowModelD1Ev((char *)t + 0x364);
    _ZN9ModelAnimD1Ev((char *)t + 0x300);
    _ZN10dBgCh_ActrD1Ev((char *)t + 0x144);
    _ZN7dCcAc_cD1Ev((char *)t + 0x110);
    _ZN12dEnemyBase_cD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
