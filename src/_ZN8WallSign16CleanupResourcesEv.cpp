//cpp
// @symbol _ZN8WallSign16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * One release. A wall sign owns exactly one shared file -- the sign model --
 * and gives it back on the way out.
 *
 * `SharedFilePtr::Release` is spelt as the member call it is, so the compiler
 * emits _ZN13SharedFilePtr7ReleaseEv itself.
 */
#include "WallSign.h"
#include "SharedFilePtr.h"

extern char data_ov085_02130858;

int WallSign::CleanupResources()
{
    ((SharedFilePtr *)(&data_ov085_02130858))->Release();
    return 1;
}
