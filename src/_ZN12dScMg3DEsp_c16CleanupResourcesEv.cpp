//cpp
// @symbol _ZN12dScMg3DEsp_c16CleanupResourcesEv
#include "dScMg3DEsp_c.h"
#include "SharedFilePtr.h"
/* dScMg3DEsp_c::CleanupResources, vtable slot 3 (ov006 0x020e9cec). Releases
 * the files of the two models; InitResources loads mModel1 from the first
 * handle and mModel2 from the second. The handles are globals, not members.
 * decl_common.h declares both as void *, so this file declares them as
 * SharedFilePtr and does not include decl_common.h. */

extern "C" {
extern SharedFilePtr data_ov006_02141e9c;
extern SharedFilePtr data_ov006_02141e74;
}

s32 dScMg3DEsp_c::CleanupResources()
{
    data_ov006_02141e9c.Release();
    data_ov006_02141e74.Release();
    return 1;
}
