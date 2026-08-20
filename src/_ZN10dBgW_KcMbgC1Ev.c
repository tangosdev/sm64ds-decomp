// @symbol _ZN10dBgW_KcMbgC1Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
/* _ZN10dBgW_KcMbgC1Ev at 0x02014878
 * dBgW_KcMbg C1 (complete object) constructor:
 *   call base dCc_c::dCc_c() (C2), then set own vtable.
 */
struct Obj { void *vtable; };
extern void func_020398c8(struct Obj *thiz); /* 0x020150cc */

struct Obj *_ZN10dBgW_KcMbgC1Ev(struct Obj *thiz)
{
    func_020398c8(thiz);
    thiz->vtable = (void *)_ZTV10dBgW_KcMbg;
    return thiz;
}
