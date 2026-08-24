// @symbol _ZN12dBgCh_SphCrrC1Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "dBgCh_SphCrr.h"
extern void _ZN5dBgChC2Ev(char *c);
extern int *_ZN5dBgPiC2Ev(int *t);
extern void _ZN5dBgPiC1Ev(void *p);


void *_ZN12dBgCh_SphCrrC1Ev(struct dBgCh_SphCrr *self) {
    _ZN5dBgChC2Ev(((char *)self));
    _ZN5dBgPiC2Ev((int *)((char *)&self->unk_010));
    _ZN8dM3dGSphC2Ev((int *)((char *)&self->unk_038));
    *(int **)((char *)self) = &data_02099338;
    *(int **)((char *)&self->unk_010) = &data_02099348;
    *(int **)((char *)&self->unk_038) = &data_02099358;
    _ZN5dBgPiC1Ev((char *)&self->mClsnResult1);
    _ZN5dBgPiC1Ev((char *)&self->mClsnResult2);
    _ZN5dBgPiC1Ev((char *)&self->mClsnResult3);
    self->unk_0ec = 0;
    return ((char *)self);
}
