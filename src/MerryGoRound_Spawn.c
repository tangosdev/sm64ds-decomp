// @symbol MerryGoRound_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV12MansionSteps */
int *MerryGoRound_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(852);
    if (p) {
        _ZN5ActorC2Ev(p);
        p[0] = (int)_ZTV12MansionSteps;
        _ZN5ModelC1Ev((char *)p + 0xd4);
        _ZN18MovingMeshColliderC1Ev((char *)p + 0x15c);
    }
    return p;
}
