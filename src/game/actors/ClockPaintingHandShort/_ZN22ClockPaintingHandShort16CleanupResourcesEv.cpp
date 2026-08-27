//cpp
// @symbol _ZN22ClockPaintingHandShort16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "ClockPaintingHandShort.h"
class SharedFilePtr {
public:
void Release();
};


int ClockPaintingHandShort::CleanupResources()
{
unsigned char r1 = mHandIndex;
SharedFilePtr *ptr = (SharedFilePtr *)data_ov013_021116b0[r1];
ptr->Release();
return 1;
}
