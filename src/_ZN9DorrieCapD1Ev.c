// @symbol _ZN9DorrieCapD1Ev
/* recovered: named members + shared header, vtable identified, globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingCylinderClsn.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified, globals resolved */
/* resolved: VT0 = _ZTV9DorrieCap */
int *_ZN9DorrieCapD1Ev(int *t)
{
    t[0] = (int)_ZTV9DorrieCap;
    _ZN18MovingCylinderClsnD1Ev((char *)t + 0x140);
    _ZN5ModelD1Ev((char *)t + 0xf0);
    func_ov001_020ab3a0((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    return t;
}
