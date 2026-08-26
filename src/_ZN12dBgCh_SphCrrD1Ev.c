// @symbol _ZN12dBgCh_SphCrrD1Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "dBgCh_SphCrr.h"
extern int _ZTV12dBgCh_SphCrr[];
extern int VTable_dBgPi_dBgCh_SphCrrThunk[];
extern int VTable_dM3dGSph_dBgCh_SphCrrThunk[];
extern void _ZN5dBgPiD1Ev(void *);
int *_ZN12dBgCh_SphCrrD1Ev(struct dBgCh_SphCrr *self) {
    ((int *)self)[0] = (int)_ZTV12dBgCh_SphCrr;
    *(int *)((char *)&self->mBgPiBase) = (int)VTable_dBgPi_dBgCh_SphCrrThunk;
    *(int *)((char *)&self->mSphereBase) = (int)VTable_dM3dGSph_dBgCh_SphCrrThunk;
    _ZN5dBgPiD1Ev((char *)&self->mClsnResult3);
    _ZN5dBgPiD1Ev((char *)&self->mClsnResult2);
    _ZN5dBgPiD1Ev((char *)&self->mClsnResult1);
    _ZN8dM3dGSphD2Ev((char *)&self->mSphereBase);
    _ZN5dBgPiD2Ev((char *)&self->mBgPiBase);
    _ZN5dBgChD2Ev(((int *)self));
    return ((int *)self);
}
