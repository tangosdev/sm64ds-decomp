// @symbol RollingIronBall_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Enemy.h"
#include "decl_Model.h"
#include "decl_dCcAc_c.h"
#include "decl_ShadowModel.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV15RollingIronBall */
extern void _ZN7PathPtrC1Ev(void *);
int *RollingIronBall_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(1020);
    if (p) {
        _ZN12dEnemyBase_cC2Ev(p);
        p[0] = (int)_ZTV15RollingIronBall;
        _ZN10dBgCh_ActrC1Ev((char *)p + 0x110);
        _ZN5ModelC1Ev((char *)p + 0x2cc);
        _ZN11ShadowModelC1Ev((char *)p + 0x31c);
        _ZN7dCcAc_cC1Ev((char *)p + 0x374);
        _ZN7PathPtrC1Ev((char *)p + 0x3f4);
    }
    return p;
}
