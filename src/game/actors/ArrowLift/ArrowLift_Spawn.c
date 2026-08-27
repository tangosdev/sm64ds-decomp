#include "ArrowLift.h"
// @symbol ArrowLift_Spawn
/* recovered: globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
/* recovered: globals resolved */
/* resolved: VT = _ZTV9ArrowLift */
int *ArrowLift_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(sizeof(struct ArrowLift));
    if (p) { _ZN10dBgActor_cC2Ev(p); p[0] = (int)_ZTV9ArrowLift; }
    return p;
}
