// @symbol daDossyCap_c_classInit
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_Model.h"
#include "decl_dCcAc_c.h"
#include "decl_common.h"
extern void _ZN10dCapIcon_cC1Ev(void *);
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV12daDossyCap_c */
/* Reconstructed source-style name: SM64DS proves daDossyCap_c through RTTI,
 * allocation size, vtable identity, and the DOSSY_CAP registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: DorrieCap_Spawn. */
int *daDossyCap_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(388);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)_ZTV12daDossyCap_c;
        _ZN10dCapIcon_cC1Ev((char *)p + 0xd4);
        _ZN5ModelC1Ev((char *)p + 0xf0);
        _ZN7dCcAc_cC1Ev((char *)p + 0x140);
    }
    return p;
}
