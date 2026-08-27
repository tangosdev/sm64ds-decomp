//cpp
// @symbol _ZN7daOts_c16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method -- vtable slot 3
 *
 * WAS _ZN5Bully16CleanupResourcesEv, and that was a misattribution, not a spelling
 * choice: slot 3 holds 0x02116ca0 in daOts_c's table AND in all three children's, so
 * Bully does not override this -- it inherits it. See the vtable diff in daOts_c.h.
 *
 * Releases the five SharedFilePtrs the file table points at. mFileTable is the base's
 * field, which is the other half of the same evidence: all three children declare it.
 */
#include "daOts_c.h"
#include "SharedFilePtr.h"

int daOts_c::CleanupResources()
{
    ((SharedFilePtr *)(*(void**)(*(char**)((char *)&mFileTable)+0)))->Release();
    ((SharedFilePtr *)(*(void**)(*(char**)((char *)&mFileTable)+4)))->Release();
    ((SharedFilePtr *)(*(void**)(*(char**)((char *)&mFileTable)+8)))->Release();
    ((SharedFilePtr *)(*(void**)(*(char**)((char *)&mFileTable)+0xc)))->Release();
    ((SharedFilePtr *)(*(void**)(*(char**)((char *)&mFileTable)+0x10)))->Release();
    return 1;
}
