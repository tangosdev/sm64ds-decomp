// @symbol func_ov026_02111678
/* recovered: vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_Model.h"
#include "decl_dCcAcPos_c.h"
#include "decl_ShadowModel.h"
#include "decl_common.h"
extern int data_ov026_02113ae0[];
/* recovered: vtable identified */
/* vtable identified: VT0 = data_ov026_02113ae0 */
extern void _ZN7PathPtrC1Ev(void *);
int *func_ov026_02111678(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(484);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)data_ov026_02113ae0;
        _ZN5ModelC1Ev((char *)p + 0xd4);
        _ZN10dCcAcPos_cC1Ev((char *)p + 0x124);
        _ZN7PathPtrC1Ev((char *)p + 0x164);
        _ZN11ShadowModelC1Ev((char *)p + 0x188);
    }
    return p;
}
