//cpp
// @symbol _ZN9daShark_c16CleanupResourcesEv

#include "SharedFilePtr.h"
#include "daShark_c.h"

extern "C" {
extern SharedFilePtr data_ov090_021345a4;
extern SharedFilePtr data_ov090_021345ac;
}

int daShark_c::CleanupResources()
{
    data_ov090_021345a4.Release();
    data_ov090_021345ac.Release();
    return 1;
}
