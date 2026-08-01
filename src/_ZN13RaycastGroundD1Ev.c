// @symbol _ZN13RaycastGroundD1Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "RaycastGround.h"
int *_ZN13RaycastGroundD1Ev(struct RaycastGround *self) {
    ((int *)self)[0] = (int)data_02099264;
    *(int *)((char *)&self->unk_010) = (int)data_02099274;
    func_020380ec((char *)&self->unk_010);
    func_020354d0(((int *)self));
    return ((int *)self);
}
