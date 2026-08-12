// @symbol HealingHeart_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_ModelAnim.h"
#include "decl_MovingCylinderClsn.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV12HealingHeart */
int *HealingHeart_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(372);
    if (p) {
        _ZN5ActorC2Ev(p);
        p[0] = (int)_ZTV12HealingHeart;
        _ZN9ModelAnimC1Ev((char *)p + 0xd4);
        _ZN18MovingCylinderClsnC1Ev((char *)p + 0x138);
    }
    return p;
}
