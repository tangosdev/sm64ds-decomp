//cpp
// @symbol _ZN7daTrs_cD0Ev
/* recovered: named members + shared header, real derived class (daTrs_c : dCapEnemy_c)
 * -- chains to dCapEnemy_c's out-of-line D1 the same way D1 does. */
#include "daTrs_c.h"
extern "C" {
extern int _ZN11ShadowModelD1Ev(void*);
extern int _ZN5ModelD1Ev(void*);
extern int _ZN9ModelAnimD1Ev(void*);
extern int _ZN10dBgCh_ActrD1Ev(void*);
extern int _ZN10dCcAcPos_cD1Ev(void*);
extern int _ZN11dCapEnemy_cD1Ev(void*);
extern int _ZTV7daTrs_c[];

int *_ZN7daTrs_cD0Ev(struct daTrs_c *self) {
    *((int *)self) = (int)_ZTV7daTrs_c;
    _ZN11ShadowModelD1Ev((char *)&self->mShadowModel2);
    _ZN11ShadowModelD1Ev((char *)&self->mShadowModel1);
    _ZN5ModelD1Ev((char *)&self->mBodyModel);
    _ZN9ModelAnimD1Ev((char *)&self->mModelAnim);
    _ZN10dBgCh_ActrD1Ev((char *)&self->mWithMeshClsn);
    _ZN10dCcAcPos_cD1Ev((char *)&self->mdCcAcPos_c);
    _ZN11dCapEnemy_cD1Ev(((int *)self));
    _ZN6Memory10DeallocateEPvP4Heap(((int *)self), data_020a0eac);
    return ((int *)self);
}
}
