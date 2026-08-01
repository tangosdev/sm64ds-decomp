// @symbol func_ov072_02120c00
/* recovered: vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_Model.h"
#include "decl_MovingCylinderClsnWithPos.h"
#include "decl_ShadowModel.h"
#include "decl_TextureSequence.h"
#include "decl_common.h"
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV11daBgSnwmn_c */
int *func_ov072_02120c00(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(496);
    if (p) {
        _ZN5ActorC2Ev(p);
        p[0] = (int)_ZTV11daBgSnwmn_c;
        _ZN5ModelC1Ev((char *)p + 0xd4);
        _ZN5ModelC1Ev((char *)p + 0x124);
        _ZN15TextureSequenceC1Ev((char *)p + 0x174);
        _ZN11ShadowModelC1Ev((char *)p + 0x188);
        _ZN25MovingCylinderClsnWithPosC1Ev((char *)p + 0x1b0);
    }
    return p;
}
