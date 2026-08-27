// @symbol MotherPenguin_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_ModelAnim.h"
#include "decl_dCcAc_c.h"
#include "decl_ShadowModel.h"
#include "decl_TextureSequence.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV13MotherPenguin */
int *MotherPenguin_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(908);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)_ZTV13MotherPenguin;
        _ZN9ModelAnimC1Ev((char *)p + 0xd4);
        _ZN15TextureSequenceC1Ev((char *)p + 0x138);
        _ZN11ShadowModelC1Ev((char *)p + 0x14c);
        _ZN7dCcAc_cC1Ev((char *)p + 0x174);
        _ZN10dBgCh_ActrC1Ev((char *)p + 0x1a8);
    }
    return p;
}
