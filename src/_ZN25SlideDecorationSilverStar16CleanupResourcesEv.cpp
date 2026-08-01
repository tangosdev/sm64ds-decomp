//cpp
// @symbol _ZN25SlideDecorationSilverStar16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "SlideDecorationSilverStar.h"
class SharedFilePtr {
public:
void Release();
};

extern "C" int data_ov031_02111424[];

int SlideDecorationSilverStar::CleanupResources()
{
unsigned char r1 = mVariant;
SharedFilePtr *ptr = (SharedFilePtr *)data_ov031_02111424[r1];
ptr->Release();
return 1;
}
