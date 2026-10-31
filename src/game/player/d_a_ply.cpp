//cpp
// @symbol daPly_c_classInit
#include "daPly_c.h"

/* Reconstructed source-style name: SM64DS proves daPly_c through RTTI,
 * allocation size, vtable identity, and the PLAYER registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: Player_Spawn. */
extern "C" daPly_c *daPly_c_classInit() {
    return new daPly_c;
}
