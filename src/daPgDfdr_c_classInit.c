// @symbol daPgDfdr_c_classInit
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_ModelAnim.h"
#include "decl_dCcAc_c.h"
#include "decl_Platform.h"
#include "decl_TextureSequence.h"
#include "decl_common.h"
extern int _ZTV10daPgDfdr_c[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV10daPgDfdr_c */
int *daPgDfdr_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(988);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV10daPgDfdr_c;
        _ZN9ModelAnimC1Ev((char *)p + 0x320);
        _ZN15TextureSequenceC1Ev((char *)p + 0x384);
        _ZN7dCcAc_cC1Ev((char *)p + 0x398);
    }
    return p;
}
