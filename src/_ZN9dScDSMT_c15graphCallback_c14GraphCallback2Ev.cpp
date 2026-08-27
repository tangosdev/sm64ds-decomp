//cpp
// @symbol _ZN9dScDSMT_c15graphCallback_c14GraphCallback2Ev
/* dScDSMT_c::graphCallback_c::GraphCallback2 -- slot 2, ov007 0x020cc0f4.
 *
 * Hands the callback object itself to func_ov007_020b6eb4 and declines the
 * slot. The callee is still an unnamed ov007 free function; it takes this
 * object, not the owning scene.
 */
#include "dScDSMT_c.h"

extern "C" void func_ov007_020b6eb4(void *);

int dScDSMT_c::graphCallback_c::GraphCallback2()
{
    func_ov007_020b6eb4(this);
    return 0;
}
