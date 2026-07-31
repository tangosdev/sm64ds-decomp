// @symbol _ZN11BobOmbBuddyD0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ModelAnim.h"
#include "decl_MovingCylinderClsn.h"
#include "decl_ShadowModel.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV14daRedBombhei_c */
extern void *G0;
int *_ZN11BobOmbBuddyD0Ev(int *t)
{
    t[0] = (int)_ZTV14daRedBombhei_c;
    _ZN11ShadowModelD1Ev((char *)t + 0x16c);
    _ZN9ModelAnimD1Ev((char *)t + 0x108);
    _ZN18MovingCylinderClsnD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, G0);
    return t;
}
