// @symbol _ZN11RaycastLineD1Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "RaycastLine.h"
extern int _ZTV11RaycastLine[];
extern int VTable_ClsnResult_RaycastLineThunk[];
int *_ZN11RaycastLineD1Ev(struct RaycastLine *self) {
    ((int *)self)[0] = (int)_ZTV11RaycastLine;
    *(int *)((char *)&self->unk_010) = (int)VTable_ClsnResult_RaycastLineThunk;
    func_0203ac50((char *)&self->unk_064);
    func_ov002_020feab8((char *)&self->unk_038);
    _ZN10ClsnResultD2Ev((char *)&self->unk_010);
    func_020354d0(((int *)self));
    return ((int *)self);
}
