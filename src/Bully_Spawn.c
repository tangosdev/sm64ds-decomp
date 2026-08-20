// @symbol Bully_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Enemy.h"
#include "decl_ModelAnim.h"
#include "decl_dCcAc_c.h"
#include "decl_ShadowModel.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
extern int data_ov064_0211b768[];
extern int _ZTV5Bully[];
/* recovered: vtable identified */
/* vtable identified: VT0 = data_ov064_0211b768 */
int *Bully_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(1024);
    if (p) {
        _ZN12dEnemyBase_cC2Ev(p);
        p[0] = (int)data_ov064_0211b768;
        _ZN9ModelAnimC1Ev((char *)p + 0x110);
        _ZN10dBgCh_ActrC1Ev((char *)p + 0x174);
        _ZN7dCcAc_cC1Ev((char *)p + 0x33c);
        _ZN11ShadowModelC1Ev((char *)p + 0x370);
        p[0] = (int)_ZTV5Bully;
    }
    return p;
}
