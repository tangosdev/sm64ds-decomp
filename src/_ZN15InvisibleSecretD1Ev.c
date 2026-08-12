// @symbol _ZN15InvisibleSecretD1Ev
/* recovered: vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_MovingCylinderClsn.h"
#include "decl_common.h"
extern int _ZTV15InvisibleSecret[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV15InvisibleSecret */
int *_ZN15InvisibleSecretD1Ev(int *t)
{
    t[0] = (int)_ZTV15InvisibleSecret;
    _ZN18MovingCylinderClsnD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    return t;
}
