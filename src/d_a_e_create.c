// @symbol daECreate_c_classInit
/* recovered: globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_common.h"
/* recovered: globals resolved */
/* resolved: VT = _ZTV12EnemySpawner */
/* Reconstructed source-style name: SM64DS proves daECreate_c through RTTI,
 * allocation size, vtable identity, and the ENEMY_CREATE registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: EnemySpawner_Spawn. */
int *daECreate_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(224);
    if (p) { _ZN8dActor_cC2Ev(p); p[0] = (int)_ZTV12EnemySpawner; }
    return p;
}
