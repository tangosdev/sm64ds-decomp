// @symbol GoombaSmall_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_dCapEnemy_c.h"
#include "decl_MaterialChanger.h"
#include "decl_ModelAnim.h"
#include "decl_dCcAc_c.h"
#include "decl_ShadowModel.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV7daKrb_c */
int *GoombaSmall_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(1144);
    if (p) {
        _ZN11dCapEnemy_cC2Ev(p);
        p[0] = (int)_ZTV7daKrb_c;
        _ZN7dCcAc_cC1Ev((char *)p + 0x180);
        _ZN10dBgCh_ActrC1Ev((char *)p + 0x1b4);
        _ZN9ModelAnimC1Ev((char *)p + 0x370);
        _ZN11ShadowModelC1Ev((char *)p + 0x3d4);
        _ZN15MaterialChangerC1Ev((char *)p + 0x3fc);
    }
    return p;
}
