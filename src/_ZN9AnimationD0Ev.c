// @symbol _ZN9AnimationD0Ev
/* recovered: named members + shared header, declarations from a shared header */
/* recovered: named members + shared header */
#include "Animation.h"
/* _ZN9AnimationD0Ev at 0x02015cc4
 * Single-vtable destructor (CodeWarrior 1.2):
 *   write own vtable to [this], call base/helper destructor, return this.
 * Call target: 0x0203cbcc
 */
struct Obj { void *vtable; };
extern int _ZTV9Animation[];                            /* vtable for Animation */
extern void _ZN6Memory16operator_delete2EPv(void *p);  /* 0x0203cbcc */
struct Obj *_ZN9AnimationD0Ev(struct Obj *thiz)
{
    thiz->vtable = (void *)_ZTV9Animation;
    _ZN6Memory16operator_delete2EPv(thiz);
    return thiz;
}
