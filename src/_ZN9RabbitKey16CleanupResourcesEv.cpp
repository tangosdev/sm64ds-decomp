//cpp
// @symbol _ZN9RabbitKey16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * One release, and it is the SAME handle Rabbit::CleanupResources releases
 * first (data_ov085_021305d8). The key and the rabbit that carries it share a
 * file; each gives back its own reference.
 */
#include "RabbitKey.h"
#include "SharedFilePtr.h"

extern char data_ov085_021305d8;

int RabbitKey::CleanupResources()
{
    ((SharedFilePtr *)(&data_ov085_021305d8))->Release();
    return 1;
}
