// @symbol _ZN13PoleBillboardD1Ev
/* recovered: vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_common.h"
extern int _ZTV13PoleBillboard[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV13PoleBillboard */
int *_ZN13PoleBillboardD1Ev(int *t)
{
    t[0] = (int)_ZTV13PoleBillboard;
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    return t;
}
