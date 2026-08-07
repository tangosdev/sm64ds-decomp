// @symbol _ZN9LakituBroD0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_ModelAnim.h"
#include "decl_ShadowModel.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV11daC_Jugem_c */
extern void _ZN15TextureSequenceD1Ev(void *);
extern void _ZN5EnemyD2Ev(void *);
extern void *data_020a0eac;
int *_ZN9LakituBroD0Ev(int *t)
{
    t[0] = (int)_ZTV11daC_Jugem_c;
    _ZN11ShadowModelD1Ev((char *)t + 0x218);
    _ZN11ShadowModelD1Ev((char *)t + 0x1f0);
    _ZN15TextureSequenceD1Ev((char *)t + 0x1d8);
    _ZN9ModelAnimD1Ev((char *)t + 0x174);
    _ZN9ModelAnimD1Ev((char *)t + 0x110);
    _ZN5EnemyD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
