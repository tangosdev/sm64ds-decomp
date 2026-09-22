//cpp
// @symbol _ZN11dScMgBase_c15graphCallback_c14GraphCallback0Ev
/* dScMgBase_c::graphCallback_c::GraphCallback0 -- slot 0, ov004.
 *
 * Forwards to the registered scene's own virtual at vtable offset +0x5c
 * (slot 23) and reports handled.
 *
 * THE DISPATCH STAYS RAW. dScMgBase_c's slots 18-35 are eighteen virtuals whose
 * signatures are not reconstructed yet (see include/dScMgBase_c.h), so slot
 * 23 cannot be called by name -- declaring it would require declaring
 * slots 18-23 above it in order, and guessing those would misnumber the
 * table. The compiler owns this function's symbol, `this` and mScene; only the
 * callee's identity is still an offset.
 */
#include "dScMgBase_c.h"

int dScMgBase_c::graphCallback_c::GraphCallback0()
{
    dScMgBase_c *p = mScene;

    if (p != 0) {
        void (*fn)(dScMgBase_c *) = *(void (**)(dScMgBase_c *))((char *)*(void **)p + 0x5c);
        fn(p);
    }

    return 1;
}
