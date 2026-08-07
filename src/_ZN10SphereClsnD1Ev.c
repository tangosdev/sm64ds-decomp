// @symbol _ZN10SphereClsnD1Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "SphereClsn.h"
extern int _ZTV10SphereClsn[];
extern int VTable_ClsnResult_SphereClsnThunk[];
extern int VTable_dM3dGSph_SphereClsnThunk[];
extern void _ZN10ClsnResultD1Ev(void *);
int *_ZN10SphereClsnD1Ev(struct SphereClsn *self) {
    ((int *)self)[0] = (int)_ZTV10SphereClsn;
    *(int *)((char *)&self->unk_010) = (int)VTable_ClsnResult_SphereClsnThunk;
    *(int *)((char *)&self->unk_038) = (int)VTable_dM3dGSph_SphereClsnThunk;
    _ZN10ClsnResultD1Ev((char *)&self->mClsnResult3);
    _ZN10ClsnResultD1Ev((char *)&self->mClsnResult2);
    _ZN10ClsnResultD1Ev((char *)&self->mClsnResult1);
    _ZN8dM3dGSphD2Ev((char *)&self->unk_038);
    _ZN10ClsnResultD2Ev((char *)&self->unk_010);
    func_020354d0(((int *)self));
    return ((int *)self);
}
