// @symbol _ZN4DoorD1Ev
/* recovered: vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ModelAnim.h"
#include "decl_common.h"
extern int _ZTV4Door[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV4Door */
int *_ZN4DoorD1Ev(int *t)
{
    t[0] = (int)_ZTV4Door;
    _ZN9ModelAnimD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    return t;
}
