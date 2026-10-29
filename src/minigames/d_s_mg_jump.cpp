//cpp
/* dScMgJump_c's factory. `new dScMgJump_c` reproduces it: the inline
 * dScMgD3DBase_c constructor (dScMgBase_c's C2, its vtable, mDrawSubScreen,
 * the empty loop over the two cameras, mSysTracker), then this class's
 * vtable, mModel, the mPlayers array and the mHeyhos array.
 */

#include "fBase_c.h"
#include "dScMgJump_c.h"

/* Reconstructed source-style name: SM64DS proves dScMgJump_c through RTTI,
 * allocation size, vtable identity, and the MG_JUMP registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: MgBounceAndPounce_Spawn. */
extern "C" dScMgJump_c *dScMgJump_c_classInit(void) {
    return new dScMgJump_c;
}
