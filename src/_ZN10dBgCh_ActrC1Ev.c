// @symbol _ZN10dBgCh_ActrC1Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_dBgCh_SphCrr.h"
#include "decl_common.h"
/* recovered: named members + shared header */
#include "dBgCh_Actr.h"
extern int data_02099204[];
extern void _ZN5dBgChC2Ev(void *);
extern void _ZN9dBgCh_LinC1Ev(void *);
int *_ZN10dBgCh_ActrC1Ev(struct dBgCh_Actr *self) {
    _ZN5dBgChC2Ev(((int *)self));
    ((int *)self)[0] = (int)data_02099204;
    _ZN12dBgCh_SphCrrC1Ev((char *)&self->mSphereClsn);
    _ZN9dBgCh_LinC1Ev((char *)&self->mRaycastLine);
    return ((int *)self);
}
