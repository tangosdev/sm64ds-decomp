// @symbol EnemySpawner_Spawn
/* recovered: globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_common.h"
/* recovered: globals resolved */
/* resolved: VT = _ZTV12EnemySpawner */
int *EnemySpawner_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(224);
    if (p) { _ZN8dActor_cC2Ev(p); p[0] = (int)_ZTV12EnemySpawner; }
    return p;
}
