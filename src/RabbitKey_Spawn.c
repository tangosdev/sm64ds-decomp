// @symbol RabbitKey_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Enemy.h"
#include "decl_Model.h"
#include "decl_ShadowModel.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV9RabbitKey */
int *RabbitKey_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(416);
    if (p) {
        _ZN5EnemyC2Ev(p);
        p[0] = (int)_ZTV9RabbitKey;
        _ZN5ModelC1Ev((char *)p + 0x110);
        _ZN11ShadowModelC1Ev((char *)p + 0x160);
    }
    return p;
}
