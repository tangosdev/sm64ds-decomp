// @symbol _ZN9DorrieCapD0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingCylinderClsn.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV12daDossyCap_c */
extern void *data_020a0eac;
int *_ZN9DorrieCapD0Ev(int *t)
{
    t[0] = (int)_ZTV12daDossyCap_c;
    _ZN18MovingCylinderClsnD1Ev((char *)t + 0x140);
    _ZN5ModelD1Ev((char *)t + 0xf0);
    func_ov001_020ab3a0((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
