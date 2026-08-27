// @symbol TTC_MovingBar_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_ShadowModel.h"
#include "decl_common.h"
extern int _ZTV16daObjCtMecha05_c[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV16daObjCtMecha05_c */
int *TTC_MovingBar_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(916);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV16daObjCtMecha05_c;
        _ZN11ShadowModelC1Ev((char *)p + 0x33c);
    }
    return p;
}
