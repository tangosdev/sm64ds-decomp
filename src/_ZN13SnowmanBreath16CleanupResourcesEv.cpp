//cpp
// @symbol _ZN10daSnmBth_c16CleanupResourcesEv
/* daSnmBth_c::CleanupResources -- vtable slot 3. Releases the three shared
 * files the class holds; it never touches `this`. */
#include "daSnmBth_c.h"
#include "SharedFilePtr.h"

extern "C" {
extern SharedFilePtr data_ov002_0210da40;
extern SharedFilePtr data_ov002_0210d9a0;
extern SharedFilePtr data_ov002_0210d9c0;
}

int daSnmBth_c::CleanupResources()
{
    data_ov002_0210da40.Release();
    data_ov002_0210d9a0.Release();
    data_ov002_0210d9c0.Release();
    return 1;
}
