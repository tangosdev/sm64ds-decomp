//cpp
// @symbol _ZN9LakituBro16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * Three releases, all this overlay's own, in the reverse of no particular
 * order -- it is simply the ROM's, and is reproduced verbatim.
 */
#include "LakituBro.h"
#include "SharedFilePtr.h"

extern char data_ov085_0213074c;
extern char data_ov085_02130744;
extern char data_ov085_0213073c;

int LakituBro::CleanupResources()
{
    ((SharedFilePtr *)(&data_ov085_0213074c))->Release();
    ((SharedFilePtr *)(&data_ov085_02130744))->Release();
    ((SharedFilePtr *)(&data_ov085_0213073c))->Release();
    return 1;
}
