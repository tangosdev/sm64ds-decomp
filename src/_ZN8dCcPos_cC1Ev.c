// @symbol _ZN19CylinderClsnWithPosC1Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "CylinderClsnWithPos.h"
/* _ZN19CylinderClsnWithPosC1Ev at 0x02014878
 * CylinderClsnWithPos C1 (complete object) constructor:
 *   call base CylinderClsn::CylinderClsn() (C2), then set own vtable.
 */
struct Obj { void *vtable; };
extern void _ZN12CylinderClsnC2Ev(struct Obj *thiz); /* 0x020150cc */

struct Obj *_ZN19CylinderClsnWithPosC1Ev(struct Obj *thiz)
{
    _ZN12CylinderClsnC2Ev(thiz);
    thiz->vtable = (void *)_ZTV19CylinderClsnWithPos;
    return thiz;
}
