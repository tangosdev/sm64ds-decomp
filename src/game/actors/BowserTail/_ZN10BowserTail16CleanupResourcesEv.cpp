//cpp
// @symbol _ZN10BowserTail16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * `return 1`, no releases. The tail shares Bowser's translation unit and his
 * files -- tu_map puts both classes in one TU at 0x2111900..0x2116484 -- so
 * Bowser::CleanupResources frees everything and the tail takes no reference of
 * its own.
 */
#include "BowserTail.h"

int BowserTail::CleanupResources()
{
    return 1;
}
