// @symbol _ZN5FaderD0Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "Fader.h"
/* _ZN5FaderD0Ev at 0x02017848
 * Single-vtable destructor: write own vtable, call base/helper destructor (0x0203cbcc), return this.
 */
struct Obj { void *vtable; };
extern void base_dtor_Fader(struct Obj *thiz); /* 0x0203cbcc */
struct Obj *_ZN5FaderD0Ev(struct Obj *thiz)
{
    thiz->vtable = (void *)vtbl_Fader;
    base_dtor_Fader(thiz);
    return thiz;
}
