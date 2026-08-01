// @symbol _ZN25MovingCylinderClsnWithPosD1Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "MovingCylinderClsnWithPos.h"
/* _ZN25MovingCylinderClsnWithPosD1Ev at 0x02014a60
 * Single-vtable D1/D2 destructor (CodeWarrior 1.2):
 *   write own vtable to [this], call immediate-base destructor, return this.
 * Base dtor call target: 0x02014954
 */
struct Obj { void *vtable; };
extern void base_dtor_MovingCylinderClsnWithPos(struct Obj *thiz); /* 0x02014954 */
struct Obj *_ZN25MovingCylinderClsnWithPosD1Ev(struct Obj *thiz)
{
    thiz->vtable = (void *)vtbl_MovingCylinderClsnWithPos;
    base_dtor_MovingCylinderClsnWithPos(thiz);
    return thiz;
}
