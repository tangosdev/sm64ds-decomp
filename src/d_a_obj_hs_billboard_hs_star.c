// @symbol daObjHsBillboard_c_classInit_HS_STAR
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_Model.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV18daObjHsBillboard_c */
/* Reconstructed source-style name: SM64DS proves daObjHsBillboard_c through RTTI,
 * allocation size, vtable identity, and the HS_STAR registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: SlideDecorationYellowStar_Spawn. */
int *daObjHsBillboard_c_classInit_HS_STAR(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(296);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)_ZTV18daObjHsBillboard_c;
        _ZN5ModelC1Ev((char *)p + 0xd4);
    }
    return p;
}
