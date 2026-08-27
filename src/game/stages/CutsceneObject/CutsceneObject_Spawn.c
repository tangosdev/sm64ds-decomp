// @symbol CutsceneObject_Spawn
/* recovered: globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_common.h"
/* recovered: globals resolved */
/* resolved: VT = _ZTV14CutsceneObject */
int *CutsceneObject_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(260);
    if (p) { _ZN8dActor_cC2Ev(p); p[0] = (int)_ZTV14CutsceneObject; }
    return p;
}
