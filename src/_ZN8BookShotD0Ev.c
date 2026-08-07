// @symbol _ZN8BookShotD0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_Model.h"
#include "decl_ModelAnim.h"
#include "decl_ShadowModel.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV8daBook_c */
extern void _ZN25MovingCylinderClsnWithPosD1Ev(void *);
extern void _ZN5EnemyD2Ev(void *);
extern void *data_020a0eac;
int *_ZN8BookShotD0Ev(int *t)
{
    t[0] = (int)_ZTV8daBook_c;
    _ZN12WithMeshClsnD1Ev((char *)t + 0x25c);
    _ZN25MovingCylinderClsnWithPosD1Ev((char *)t + 0x21c);
    _ZN11ShadowModelD1Ev((char *)t + 0x1c4);
    _ZN5ModelD1Ev((char *)t + 0x174);
    _ZN9ModelAnimD1Ev((char *)t + 0x110);
    _ZN5EnemyD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
