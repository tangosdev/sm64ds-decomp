// @symbol daStarGate_c_classInit
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV8StarDoor */
/* Reconstructed source-style name: SM64DS proves daStarGate_c through RTTI,
 * allocation size, vtable identity, and the STAR_GATE registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: StarDoor_Spawn. */
int *daStarGate_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(280);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)_ZTV8StarDoor;
        _ZN11CommonModelC1Ev((char *)p + 0xd4);
    }
    return p;
}
