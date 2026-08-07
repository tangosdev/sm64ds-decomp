// @symbol _ZN8MantaRayD0Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_ModelAnim.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: named members + shared header */
#include "MantaRay.h"
extern void _ZN25MovingCylinderClsnWithPosD1Ev(void *);
extern void _ZN5EnemyD2Ev(void *);
extern void *data_020a0eac;
int *_ZN8MantaRayD0Ev(struct MantaRay *self) {
    ((int *)self)[0] = (int)_ZTV8MantaRay;
    _ZN9ModelAnimD1Ev((char *)&self->mModelAnim);
    _ZN12WithMeshClsnD1Ev((char *)&self->mWithMeshClsn);
    _ZN25MovingCylinderClsnWithPosD1Ev((char *)&self->mMovingCylinderClsnWithPos);
    _ZN5EnemyD2Ev(((int *)self));
    _ZN6Memory10DeallocateEPvP4Heap(((int *)self), data_020a0eac);
    return ((int *)self);
}
