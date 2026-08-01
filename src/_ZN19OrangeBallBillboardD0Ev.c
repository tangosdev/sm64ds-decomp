// @symbol _ZN19OrangeBallBillboardD0Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_common.h"
/* recovered: named members + shared header */
#include "OrangeBallBillboard.h"
extern void *data_020a0eac;
int *_ZN19OrangeBallBillboardD0Ev(struct OrangeBallBillboard *self) {
    ((int *)self)[0] = (int)_ZTV19OrangeBallBillboard;
    _ZN5ModelD1Ev((char *)&self->mModel);
    _ZN5ActorD2Ev(((int *)self));
    _ZN6Memory10DeallocateEPvP4Heap(((int *)self), data_020a0eac);
    return ((int *)self);
}
