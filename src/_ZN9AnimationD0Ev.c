// @symbol _ZN9AnimationD0Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "Animation.h"
/* _ZN9AnimationD0Ev at 0x02015cc4
 * Single-vtable destructor (CodeWarrior 1.2):
 *   write own vtable to [this], call base/helper destructor, return this.
 * Call target: 0x0203cbcc
 */
struct Obj { void *vtable; };
extern void base_dtor_Animation(struct Obj *thiz); /* 0x0203cbcc */
struct Obj *_ZN9AnimationD0Ev(struct Obj *thiz)
{
    thiz->vtable = (void *)vtbl_Animation;
    base_dtor_Animation(thiz);
    return thiz;
}
