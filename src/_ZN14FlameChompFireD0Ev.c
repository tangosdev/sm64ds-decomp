// @symbol _ZN14FlameChompFireD0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_MovingCylinderClsn.h"
#include "decl_ShadowModel.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV8daKpFr_c */
extern void *G0;
int *_ZN14FlameChompFireD0Ev(int *t)
{
    t[0] = (int)_ZTV8daKpFr_c;
    _ZN12WithMeshClsnD1Ev((char *)t + 0x130);
    _ZN18MovingCylinderClsnD1Ev((char *)t + 0xfc);
    _ZN11ShadowModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, G0);
    return t;
}
