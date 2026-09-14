//cpp
// @symbol _ZN11daTBasket_c16CleanupResourcesEv
/* Vtable slot 3. Releases the shared cage model file (daTrs_c releases it
 * too; the sinit owns the handle). */

#include "daTBasket_c.h"
#include "SharedFilePtr.h"

extern SharedFilePtr data_ov063_0211edec;

int daTBasket_c::CleanupResources()
{
    data_ov063_0211edec.Release();
    return 1;
}
