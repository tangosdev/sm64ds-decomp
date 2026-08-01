// @symbol _ZN15MaterialChangerD1Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "MaterialChanger.h"
/* _ZN15MaterialChangerD1Ev at 0x0201582c
 * Single-vtable destructor (CodeWarrior 1.2):
 *   write own vtable to [this], call base/helper destructor, return this.
 * Call target: 0x02015cb4
 */
struct Obj { void *vtable; };
extern void _ZN9AnimationD2Ev(struct Obj *thiz); /* 0x02015cb4 */
struct Obj *_ZN15MaterialChangerD1Ev(struct Obj *thiz)
{
    thiz->vtable = (void *)_ZTV15MaterialChanger;
    _ZN9AnimationD2Ev(thiz);
    return thiz;
}
