// @symbol _ZN8dCcPos_cC1Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "dCcPos_c.h"
/* _ZN8dCcPos_cC1Ev at 0x02014878
 * dCcPos_c C1 (complete object) constructor:
 *   call base dCc_c::dCc_c() (C2), then set own vtable.
 */
struct Obj { void *vtable; };
extern void _ZN5dCc_cC2Ev(struct Obj *thiz); /* 0x020150cc */

struct Obj *_ZN8dCcPos_cC1Ev(struct Obj *thiz)
{
    _ZN5dCc_cC2Ev(thiz);
    thiz->vtable = (void *)_ZTV8dCcPos_c;
    return thiz;
}
