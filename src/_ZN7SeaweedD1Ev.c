// @symbol _ZN7SeaweedD1Ev
/* recovered: vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ModelAnim.h"
#include "decl_common.h"
extern int _ZTV7Seaweed[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV7Seaweed */
int *_ZN7SeaweedD1Ev(int *t)
{
    t[0] = (int)_ZTV7Seaweed;
    _ZN9ModelAnimD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    return t;
}
