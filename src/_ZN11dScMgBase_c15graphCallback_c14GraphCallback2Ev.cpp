//cpp
// @symbol _ZN11dScMgBase_c15graphCallback_c14GraphCallback2Ev
/* dScMgBase_c::graphCallback_c::GraphCallback2 -- slot 2, ov004 0x020ae06c.
 *
 * Unlike slots 0, 1 and 3, this one RETURNS the scene virtual's own result
 * rather than discarding it and reporting handled; it only reports handled
 * when no scene is registered.
 *
 * The dispatch stays raw for the reason the sibling slots give: dScMgBase_c's
 * slots 18-35 have no reconstructed signatures, so slot 24 (+0x60) cannot be
 * called by name.
 */
#include "dScMgBase_c.h"

int dScMgBase_c::graphCallback_c::GraphCallback2()
{
    dScMgBase_c *p = mScene;

    if (p == 0) {
        return 1;
    }

    return (*(int (**)(dScMgBase_c *))((char *)*(void **)p + 0x60))(p);
}
