//cpp
// @symbol _ZN9SpikeBomb16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * One release -- the platform's own model.
 *
 * `SharedFilePtr::Release` is spelt as the member call it is, so the compiler
 * emits _ZN13SharedFilePtr7ReleaseEv itself.
 */
#include "SpikeBomb.h"
#include "SharedFilePtr.h"

extern char data_ov060_0211b1c4;

int SpikeBomb::CleanupResources()
{
    ((SharedFilePtr *)(&data_ov060_0211b1c4))->Release();
    return 1;
}
