// @symbol daCamTag_c_Spawn
#include "daCamTag_c.h"
/* recovered: globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_common.h"
/* recovered: globals resolved */
/* resolved: VT = _ZTV10daCamTag_c */
int *daCamTag_c_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(sizeof(struct daCamTag_c));
    if (p) { _ZN8dActor_cC2Ev(p); p[0] = (int)_ZTV10daCamTag_c; }
    return p;
}
