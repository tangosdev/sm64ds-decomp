//cpp
// @symbol _ZN7daTrs_cD0Ev
/* recovered: named members + shared header, real derived class (daTrs_c : dCapEnemy_c)
 * -- chains to dCapEnemy_c's out-of-line D1 (func_ov002_020aedbc, not yet
 * renamed; see include/daTrs_c.h) the same way _ZN7daTrs_cD1Ev.cpp does. */
#include "daTrs_c.h"
extern "C" {
extern int _ZN11ShadowModelD1Ev(void*);
extern int _ZN5ModelD1Ev(void*);
extern int _ZN9ModelAnimD1Ev(void*);
extern int _ZN12WithMeshClsnD1Ev(void*);
extern int _ZN25MovingCylinderClsnWithPosD1Ev(void*);
extern int func_ov002_020aedbc(void*);
extern int _ZTV7daTrs_c[];

int *_ZN7daTrs_cD0Ev(struct daTrs_c *self) {
    *((int *)self) = (int)_ZTV7daTrs_c;
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
}
