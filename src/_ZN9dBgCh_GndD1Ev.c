// @symbol _ZN9dBgCh_GndD1Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "dBgCh_Gnd.h"
int *_ZN9dBgCh_GndD1Ev(struct dBgCh_Gnd *self) {
    ((int *)self)[0] = (int)data_02099264;
    *(int *)((char *)&self->unk_010) = (int)data_02099274;
    _ZN5dBgPiD2Ev((char *)&self->unk_010);
    func_020354d0(((int *)self));
    return ((int *)self);
}
