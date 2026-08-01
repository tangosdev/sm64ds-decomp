// @symbol _ZN8MantaRayD1Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_ModelAnim.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: named members + shared header */
#include "MantaRay.h"
extern void _ZN25MovingCylinderClsnWithPosD1Ev(void *);
extern void func_ov002_020aed18(void *);
int *_ZN8MantaRayD1Ev(struct MantaRay *self) {
    ((int *)self)[0] = (int)_ZTV8MantaRay;
    _ZN9ModelAnimD1Ev((char *)&self->mModelAnim);
    _ZN12WithMeshClsnD1Ev((char *)&self->mWithMeshClsn);
    _ZN25MovingCylinderClsnWithPosD1Ev((char *)&self->mMovingCylinderClsnWithPos);
    func_ov002_020aed18(((int *)self));
    return ((int *)self);
}
