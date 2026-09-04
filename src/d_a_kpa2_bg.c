// @symbol daKpa2Bg_c_classInit
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Model.h"
#include "decl_dBgW_KcMbg.h"
#include "decl_Platform.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV10daKpa2Bg_c */
int *daKpa2Bg_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(1392);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV10daKpa2Bg_c;
        _ZN5ModelC1Ev((char *)p + 0x324);
        _ZN10dBgW_KcMbgC1Ev((char *)p + 0x374);
    }
    return p;
}
