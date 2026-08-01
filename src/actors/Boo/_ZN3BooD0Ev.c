// @symbol _ZN3BooD0Ev
/* recovered: named members + shared header */
#include "Boo.h"
extern int _ZN11ShadowModelD1Ev();
extern int _ZN5ModelD1Ev();
extern int _ZN9ModelAnimD1Ev();
extern int _ZN12WithMeshClsnD1Ev();
extern int _ZN25MovingCylinderClsnWithPosD1Ev();
extern int func_ov002_020aedbc();
extern int _ZN6Memory10DeallocateEPvP4Heap();
extern int _ZTV3Boo[];
extern int *data_020a0eac;

int *_ZN3BooD0Ev(struct Boo *self) {
    *((int *)self) = (int)_ZTV3Boo;
    _ZN11ShadowModelD1Ev((char *)&self->mShadowModel2);
    _ZN11ShadowModelD1Ev((char *)&self->mShadowModel1);
    _ZN5ModelD1Ev((char *)&self->mModel);
    _ZN9ModelAnimD1Ev((char *)&self->mModelAnim);
    _ZN12WithMeshClsnD1Ev((char *)&self->mWithMeshClsn);
    _ZN25MovingCylinderClsnWithPosD1Ev((char *)&self->mMovingCylinderClsnWithPos);
    func_ov002_020aedbc(((int *)self));
    _ZN6Memory10DeallocateEPvP4Heap(((int *)self), data_020a0eac);
    return ((int *)self);
}
