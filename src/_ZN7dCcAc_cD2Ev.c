// @symbol _ZN7dCcAc_cD2Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "dCcAc_c.h"
extern void* _ZTV7dCcAc_c[];
/* _ZN7dCcAc_cD2Ev at 0x02014954
 * Single-vtable D1/D2 destructor (CodeWarrior 1.2):
 *   write own vtable to [this], call immediate-base destructor, return this.
 * Base dtor call target: 0x02015058
 */
struct Obj { void *vtable; };
extern void _ZN5dCc_cD2Ev(struct Obj *thiz); /* 0x02015058 */
struct Obj *_ZN7dCcAc_cD2Ev(struct Obj *thiz)
{
    thiz->vtable = (void *)_ZTV7dCcAc_c;
    _ZN5dCc_cD2Ev(thiz);
    return thiz;
}
