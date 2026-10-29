//cpp
// @symbol daStar_c_classInit_SILVER_STAR
#include "daStar_c.h"

/* Reconstructed source-style name: SM64DS proves daStar_c through RTTI,
 * allocation size, vtable identity, and the SILVER_STAR registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: SilverStar_Spawn.
 *
 * `new` alone reproduces the body: the fBase_c::operator new(1220) call, the
 * dEnemyBase_c C2, the vptr store and the member constructors (dCcAcPos_c,
 * dBgCh_Actr, two dExtAnmModel_c, dExtShadowModel_c). */
extern "C" daStar_c *daStar_c_classInit_SILVER_STAR() {
    return new daStar_c();
}
