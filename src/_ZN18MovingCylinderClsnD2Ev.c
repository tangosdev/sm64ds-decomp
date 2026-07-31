// @symbol _ZN18MovingCylinderClsnD2Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "MovingCylinderClsn.h"
/* _ZN18MovingCylinderClsnD2Ev at 0x02014954
 * Single-vtable D1/D2 destructor (CodeWarrior 1.2):
 *   write own vtable to [this], call immediate-base destructor, return this.
 * Base dtor call target: 0x02015058
 */
struct Obj { void *vtable; };
extern void base_dtor_MovingCylinderClsn(struct Obj *thiz); /* 0x02015058 */
struct Obj *_ZN18MovingCylinderClsnD2Ev(struct Obj *thiz)
{
    thiz->vtable = (void *)vtbl_MovingCylinderClsn;
    base_dtor_MovingCylinderClsn(thiz);
    return thiz;
}
