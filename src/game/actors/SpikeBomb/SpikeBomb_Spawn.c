// @symbol SpikeBomb_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_Model.h"
#include "decl_dCcAcPos_c.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV9SpikeBomb */
int *SpikeBomb_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(432);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)_ZTV9SpikeBomb;
        _ZN5ModelC1Ev((char *)p + 0xd4);
        _ZN10dCcAcPos_cC1Ev((char *)p + 0x124);
    }
    return p;
}
