// @symbol _ZN7TornadoD0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ModelAnim.h"
#include "decl_MovingCylinderClsn.h"
#include "decl_TextureTransformer.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV7daTor_c */
extern void *G0;
int *_ZN7TornadoD0Ev(int *t)
{
    t[0] = (int)_ZTV7daTor_c;
    _ZN18TextureTransformerD1Ev((char *)t + 0x328);
    _ZN9ModelAnimD1Ev((char *)t + 0x2c4);
    _ZN12WithMeshClsnD1Ev((char *)t + 0x108);
    _ZN18MovingCylinderClsnD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, G0);
    return t;
}
