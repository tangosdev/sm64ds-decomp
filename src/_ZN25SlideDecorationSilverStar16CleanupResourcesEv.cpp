//cpp
// @symbol _ZN25SlideDecorationSilverStar16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "SlideDecorationSilverStar.h"
#include "SharedFilePtr.h"

extern "C" SharedFilePtr *data_ov031_02111424[];

int SlideDecorationSilverStar::CleanupResources()
{
    data_ov031_02111424[mVariant]->Release();
    return 1;
}
