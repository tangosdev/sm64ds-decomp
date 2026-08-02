// @symbol _ZN18MovingMeshColliderC1Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
/* _ZN18MovingMeshColliderC1Ev at 0x02014878
 * MovingMeshCollider C1 (complete object) constructor:
 *   call base CylinderClsn::CylinderClsn() (C2), then set own vtable.
 */
struct Obj { void *vtable; };
extern void func_020398c8(struct Obj *thiz); /* 0x020150cc */

struct Obj *_ZN18MovingMeshColliderC1Ev(struct Obj *thiz)
{
    func_020398c8(thiz);
    thiz->vtable = (void *)_ZTV18MovingMeshCollider;
    return thiz;
}
