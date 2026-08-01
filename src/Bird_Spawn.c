// @symbol Bird_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_ModelAnim.h"
#include "decl_ShadowModel.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV4Bird */
int *Bird_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(388);
    if (p) {
        _ZN5ActorC2Ev(p);
        p[0] = (int)_ZTV4Bird;
        _ZN9ModelAnimC1Ev((char *)p + 0xd4);
        _ZN11ShadowModelC1Ev((char *)p + 0x138);
    }
    return p;
}
