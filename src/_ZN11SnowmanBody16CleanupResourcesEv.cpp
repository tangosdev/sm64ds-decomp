//cpp
// @symbol _ZN11SnowmanBody16CleanupResourcesEv
/* SnowmanBody::CleanupResources -- vtable slot 3. Releases the one shared file
 * the class holds; it never touches `this`. */
#include "SnowmanBody.h"
#include "SharedFilePtr.h"

extern "C" SharedFilePtr data_ov072_02122b20;

int SnowmanBody::CleanupResources()
{
    data_ov072_02122b20.Release();
    return 1;
}
