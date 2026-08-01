// @symbol _ZN10SphereClsnD1Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "SphereClsn.h"
extern void _ZN10ClsnResultD1Ev(void *);
int *_ZN10SphereClsnD1Ev(struct SphereClsn *self) {
    ((int *)self)[0] = (int)VT0;
    *(int *)((char *)&self->unk_010) = (int)VT1;
    *(int *)((char *)&self->unk_038) = (int)VT2;
    _ZN10ClsnResultD1Ev((char *)&self->mClsnResult3);
    _ZN10ClsnResultD1Ev((char *)&self->mClsnResult2);
    _ZN10ClsnResultD1Ev((char *)&self->mClsnResult1);
    func_0203ac1c((char *)&self->unk_038);
    func_020380ec((char *)&self->unk_010);
    func_020354d0(((int *)self));
    return ((int *)self);
}
