// @symbol _ZN7ClipperD0Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "Clipper.h"
extern void* _ZTV7Clipper[];
/* _ZN7ClipperD0Ev at 0x020156fc
 * Single-vtable destructor (CodeWarrior 1.2):
 *   write own vtable to [this], call base/helper destructor, return this.
 * Call target: 0x0203cbcc
 */
struct Obj { void *vtable; };
/* Declared by decl_common.h as taking void*. This used to be a local declaration of
   `base_dtor_Clipper`, a name no module defines: the call at 0x0203cbcc goes to
   Memory::operator_delete2, not to a Clipper base destructor. */
struct Obj *_ZN7ClipperD0Ev(struct Obj *thiz)
{
    thiz->vtable = (void *)_ZTV7Clipper;
    _ZN6Memory16operator_delete2EPv(thiz);
    return thiz;
}
