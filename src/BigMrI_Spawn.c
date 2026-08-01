// @symbol BigMrI_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_ModelAnim.h"
#include "decl_MovingCylinderClsnWithPos.h"
#include "decl_ShadowModel.h"
#include "decl_TextureSequence.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV3MrI */
int *BigMrI_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(536);
    if (p) {
        _ZN5ActorC2Ev(p);
        p[0] = (int)_ZTV3MrI;
        _ZN9ModelAnimC1Ev((char *)p + 0xd4);
        _ZN15TextureSequenceC1Ev((char *)p + 0x138);
        _ZN11ShadowModelC1Ev((char *)p + 0x14c);
        _ZN25MovingCylinderClsnWithPosC1Ev((char *)p + 0x174);
    }
    return p;
}
