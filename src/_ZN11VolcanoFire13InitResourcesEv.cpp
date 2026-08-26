//cpp
// @symbol _ZN11VolcanoFire13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "VolcanoFire.h"
struct dActor_c;
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *, dActor_c *a, int b, int c, unsigned int d, unsigned int e);


int VolcanoFire::InitResources()
{
    mTerminalVelocity = -0xc8000;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mCylinderClsn, this, 0x1e000, 0x1e000, 0x200002, 0);
    func_ov022_02112790(this, &data_ov022_02114690);
    return 1;
}
