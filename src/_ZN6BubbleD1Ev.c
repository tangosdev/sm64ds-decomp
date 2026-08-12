// @symbol _ZN6BubbleD1Ev
/* recovered: vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_MovingCylinderClsn.h"
#include "decl_common.h"
extern int _ZTV6Bubble[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV6Bubble */
int *_ZN6BubbleD1Ev(int *t)
{
    t[0] = (int)_ZTV6Bubble;
    _ZN18MovingCylinderClsnD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    return t;
}
