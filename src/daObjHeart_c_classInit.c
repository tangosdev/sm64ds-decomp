// @symbol daObjHeart_c_classInit
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_ModelAnim.h"
#include "decl_dCcAc_c.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV12daObjHeart_c */
int *daObjHeart_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(372);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)_ZTV12daObjHeart_c;
        _ZN9ModelAnimC1Ev((char *)p + 0xd4);
        _ZN7dCcAc_cC1Ev((char *)p + 0x138);
    }
    return p;
}
