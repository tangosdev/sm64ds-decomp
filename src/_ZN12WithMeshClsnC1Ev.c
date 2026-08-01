// @symbol _ZN12WithMeshClsnC1Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_SphereClsn.h"
#include "decl_common.h"
/* recovered: named members + shared header */
#include "WithMeshClsn.h"
extern void func_02035514(void *);
extern void _ZN11RaycastLineC1Ev(void *);
int *_ZN12WithMeshClsnC1Ev(struct WithMeshClsn *self) {
    func_02035514(((int *)self));
    ((int *)self)[0] = (int)VT0;
    _ZN10SphereClsnC1Ev((char *)&self->mSphereClsn);
    _ZN11RaycastLineC1Ev((char *)&self->mRaycastLine);
    return ((int *)self);
}
