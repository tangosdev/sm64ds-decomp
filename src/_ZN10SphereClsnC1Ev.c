// @symbol _ZN10SphereClsnC1Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "SphereClsn.h"
extern void func_02035514(char *c);
extern int *func_0203819c(int *t);
extern void _ZN10ClsnResultC1Ev(void *p);


void *_ZN10SphereClsnC1Ev(struct SphereClsn *self) {
    func_02035514(((char *)self));
    func_0203819c((int *)((char *)&self->unk_010));
    func_0203ac70((int *)((char *)&self->unk_038));
    *(int **)((char *)self) = &data_02099338;
    *(int **)((char *)&self->unk_010) = &data_02099348;
    *(int **)((char *)&self->unk_038) = &data_02099358;
    _ZN10ClsnResultC1Ev((char *)&self->mClsnResult1);
    _ZN10ClsnResultC1Ev((char *)&self->mClsnResult2);
    _ZN10ClsnResultC1Ev((char *)&self->mClsnResult3);
    self->unk_0ec = 0;
    return ((char *)self);
}
