// @symbol Amp_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_Model.h"
#include "decl_ModelAnim.h"
#include "decl_dCcAcPos_c.h"
#include "decl_ShadowModel.h"
#include "decl_TextureSequence.h"
#include "decl_TextureTransformer.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV3Amp */
int *Amp_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(1076);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)_ZTV3Amp;
        _ZN9ModelAnimC1Ev((char *)p + 0xd4);
        _ZN5ModelC1Ev((char *)p + 0x138);
        _ZN15TextureSequenceC1Ev((char *)p + 0x188);
        _ZN18TextureTransformerC1Ev((char *)p + 0x19c);
        _ZN11ShadowModelC1Ev((char *)p + 0x1b0);
        _ZN10dCcAcPos_cC1Ev((char *)p + 0x1d8);
        _ZN10dBgCh_ActrC1Ev((char *)p + 0x218);
    }
    return p;
}
