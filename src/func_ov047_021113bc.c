// @symbol func_ov047_021113bc
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
extern int _ZTV16daObjKaitendai_c[];
extern int _ZTV20daObjKm3_Kaitendai_c[];
/* recovered: vtable identified. This is daObjKm3_Kaitendai_c's real factory --
   see include/daObjKm3_Kaitendai_c.h. */
/* vtable identified: VT0 = _ZTV16daObjKaitendai_c */
int *func_ov047_021113bc(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(800);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV16daObjKaitendai_c;
        p[0] = (int)_ZTV20daObjKm3_Kaitendai_c;
    }
    return p;
}
