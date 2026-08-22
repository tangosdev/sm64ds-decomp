// @symbol CameraTag_Spawn
#include "CameraTag.h"
/* recovered: globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_common.h"
/* recovered: globals resolved */
/* resolved: VT = _ZTV9CameraTag */
int *CameraTag_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(sizeof(struct CameraTag));
    if (p) { _ZN8dActor_cC2Ev(p); p[0] = (int)_ZTV9CameraTag; }
    return p;
}
