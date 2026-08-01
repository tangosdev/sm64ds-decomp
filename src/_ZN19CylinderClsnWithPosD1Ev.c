// @symbol _ZN19CylinderClsnWithPosD1Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "CylinderClsnWithPos.h"
/* _ZN19CylinderClsnWithPosD1Ev at 0x02014854
 * Single-vtable D1/D2 destructor (CodeWarrior 1.2):
 *   write own vtable to [this], call immediate-base destructor, return this.
 * Base dtor call target: 0x02015058
 */
struct Obj { void *vtable; };
extern void _ZN12CylinderClsnD2Ev(struct Obj *thiz); /* 0x02015058 */
struct Obj *_ZN19CylinderClsnWithPosD1Ev(struct Obj *thiz)
{
    thiz->vtable = (void *)_ZTV19CylinderClsnWithPos;
    _ZN12CylinderClsnD2Ev(thiz);
    return thiz;
}
