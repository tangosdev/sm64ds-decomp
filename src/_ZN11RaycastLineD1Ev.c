// @symbol _ZN11RaycastLineD1Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "RaycastLine.h"
int *_ZN11RaycastLineD1Ev(struct RaycastLine *self) {
    ((int *)self)[0] = (int)VT0;
    *(int *)((char *)&self->unk_010) = (int)VT1;
    func_0203ac50((char *)&self->unk_064);
    func_ov002_020feab8((char *)&self->unk_038);
    func_020380ec((char *)&self->unk_010);
    func_020354d0(((int *)self));
    return ((int *)self);
}
