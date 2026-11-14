//cpp
#include "daStarBase_c.h"

/* Reconstructed source-style name: SM64DS proves daStarBase_c through RTTI,
 * allocation size, vtable identity, and the STARBASE registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: StarMarker_Spawn.
 *
 * `new` alone reproduces the body: the fBase_c::operator new(476) call, the
 * dActor_c C2, the vptr store and the member constructors (dCcAcPos_c,
 * dExtSimpleModel_c, dExtShadowModel_c). */
daStarBase_c *daStarBase_c_classInit() {
    return new daStarBase_c();
}
