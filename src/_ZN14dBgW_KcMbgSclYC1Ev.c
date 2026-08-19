// @symbol _ZN14dBgW_KcMbgSclYC1Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
/* _ZN14dBgW_KcMbgSclYC1Ev at 0x02014878
 * dBgW_KcMbgSclY C1 (complete object) constructor:
 *   call base dCc_c::dCc_c() (C2), then set own vtable.
 */
struct Obj { void *vtable; };
extern void func_0203a4b8(struct Obj *thiz); /* 0x020150cc */

struct Obj *_ZN14dBgW_KcMbgSclYC1Ev(struct Obj *thiz)
{
    func_0203a4b8(thiz);
    thiz->vtable = (void *)_ZTV14dBgW_KcMbgSclY;
    return thiz;
}
