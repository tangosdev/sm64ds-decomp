//cpp
// @symbol _ZN6Fwoosh16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "Fwoosh.h"
#include "SharedFilePtr.h"
extern SharedFilePtr data_ov002_0210da40;
extern SharedFilePtr data_ov002_0210d9a0;
extern SharedFilePtr data_ov002_0210d9c0;
extern SharedFilePtr data_ov091_02135674;
extern SharedFilePtr data_ov091_0213567c;
extern SharedFilePtr data_ov091_02135684;

int Fwoosh::CleanupResources()
{
    if (mVariant == 1) return 1;
    data_ov002_0210da40.Release();
    data_ov002_0210d9a0.Release();
    data_ov002_0210d9c0.Release();
    data_ov091_02135674.Release();
    data_ov091_0213567c.Release();
    data_ov091_02135684.Release();
    return 1;
}
