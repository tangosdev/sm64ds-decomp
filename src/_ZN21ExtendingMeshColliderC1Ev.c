// @symbol _ZN21ExtendingMeshColliderC1Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "ExtendingMeshCollider.h"
/* _ZN21ExtendingMeshColliderC1Ev at 0x02014878
 * ExtendingMeshCollider C1 (complete object) constructor:
 *   call base CylinderClsn::CylinderClsn() (C2), then set own vtable.
 */
struct Obj { void *vtable; };
extern void func_0203a4b8(struct Obj *thiz); /* 0x020150cc */

struct Obj *_ZN21ExtendingMeshColliderC1Ev(struct Obj *thiz)
{
    func_0203a4b8(thiz);
    thiz->vtable = (void *)_ZTV21ExtendingMeshCollider;
    return thiz;
}
