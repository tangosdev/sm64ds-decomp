//cpp
// @symbol _ZN9OneUpLogo16CleanupResourcesEv

#include "OneUpLogo.h"
#include "SharedFilePtr.h"

extern SharedFilePtr data_ov002_02110aa4;
extern SharedFilePtr data_ov002_02110a9c;

int OneUpLogo::CleanupResources()
{
    data_ov002_02110aa4.Release();
    data_ov002_02110a9c.Release();
    return 1;
}
