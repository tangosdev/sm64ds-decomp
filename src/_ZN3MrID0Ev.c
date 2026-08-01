// @symbol _ZN3MrID0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ModelAnim.h"
#include "decl_ShadowModel.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV8daEykn_c */
extern void _ZN25MovingCylinderClsnWithPosD1Ev(void *);
extern void _ZN15TextureSequenceD1Ev(void *);
extern void *data_020a0eac;
int *_ZN3MrID0Ev(int *t)
{
    t[0] = (int)_ZTV8daEykn_c;
    _ZN25MovingCylinderClsnWithPosD1Ev((char *)t + 0x174);
    _ZN11ShadowModelD1Ev((char *)t + 0x14c);
    _ZN15TextureSequenceD1Ev((char *)t + 0x138);
    _ZN9ModelAnimD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
