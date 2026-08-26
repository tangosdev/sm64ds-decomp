// @symbol _ZN9dBgCh_LinD1Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "dBgCh_Lin.h"
extern int _ZTV9dBgCh_Lin[];
extern int VTable_dBgPi_dBgCh_LinThunk[];
int *_ZN9dBgCh_LinD1Ev(struct dBgCh_Lin *self) {
    ((int *)self)[0] = (int)_ZTV9dBgCh_Lin;
    *(int *)((char *)&self->mBgPiBase) = (int)VTable_dBgPi_dBgCh_LinThunk;
    _ZN8dM3dGSphD1Ev((char *)&self->mBoundSphere);
    func_ov002_020feab8((char *)&self->mLineBase);
    _ZN5dBgPiD2Ev((char *)&self->mBgPiBase);
    _ZN5dBgChD2Ev(((int *)self));
    return ((int *)self);
}
