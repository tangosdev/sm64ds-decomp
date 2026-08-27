//cpp
// @symbol _ZN7daOts_cD0Ev
/* recovered: real C++ deleting destructor -- forced out of line via the key function
 *
 * daOts_c.h defines `~daOts_c() {}` in the class body on purpose, so no TU
 * that merely includes the header emits _ZN7daOts_cD0Ev -- but the ROM
 * carries one at ov064 0x02115f28. The vtable is what needs the deleting
 * destructor as a symbol, and the vtable is emitted by the TU that defines
 * the key function (CleanupResources, the first non-inline virtual).
 * Defining it here makes mwcc emit _ZTV7daOts_c and the destructor variants
 * alongside it, and objisolate keeps only the D0 .text this file is bound
 * to, discarding the duplicate CleanupResources body and the .data
 * (notes/dtor-migration.md section 3).
 */
#include "daOts_c.h"

int daOts_c::CleanupResources()
{
    return 0;
}
