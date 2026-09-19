//cpp
// @symbol _ZN15daObj_Mip_Key_c16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * One release, and it is the SAME handle Rabbit::CleanupResources releases
 * first (data_ov085_021305d8). The key and the rabbit that carries it share a
 * file; each gives back its own reference.
 */
#include "daObj_Mip_Key_c.h"
#include "SharedFilePtr.h"

extern char data_ov085_021305d8;

int daObj_Mip_Key_c::CleanupResources()
{
    ((SharedFilePtr *)(&data_ov085_021305d8))->Release();
    return 1;
}
