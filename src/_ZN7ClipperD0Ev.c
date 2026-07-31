// @symbol _ZN7ClipperD0Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "Clipper.h"
/* _ZN7ClipperD0Ev at 0x020156fc
 * Single-vtable destructor (CodeWarrior 1.2):
 *   write own vtable to [this], call base/helper destructor, return this.
 * Call target: 0x0203cbcc
 */
struct Obj { void *vtable; };
extern void base_dtor_Clipper(struct Obj *thiz); /* 0x0203cbcc */
struct Obj *_ZN7ClipperD0Ev(struct Obj *thiz)
{
    thiz->vtable = (void *)vtbl_Clipper;
    base_dtor_Clipper(thiz);
    return thiz;
}
