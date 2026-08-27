// @symbol Moneybag_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_Model.h"
#include "decl_ModelAnim.h"
#include "decl_dCcAc_c.h"
#include "decl_ShadowModel.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV8Moneybag */
int *Moneybag_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(1012);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)_ZTV8Moneybag;
        _ZN9ModelAnimC1Ev((char *)p + 0xd4);
        _ZN5ModelC1Ev((char *)p + 0x138);
        _ZN11ShadowModelC1Ev((char *)p + 0x188);
        _ZN7dCcAc_cC1Ev((char *)p + 0x1b0);
        _ZN10dBgCh_ActrC1Ev((char *)p + 0x1e4);
    }
    return p;
}
