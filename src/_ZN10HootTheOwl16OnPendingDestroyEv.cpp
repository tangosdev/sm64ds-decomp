//cpp
// @symbol _ZN10HootTheOwl16OnPendingDestroyEv
/* recovered: shared header, real C++ method
 *
 * fBase_c slot 12. Empty in the ROM: four bytes, `bx lr`. HootTheOwl has
 * nothing to do when it is marked for destruction -- its four members are
 * released by CleanupResources and destroyed by the destructor.
 */
#include "HootTheOwl.h"

void HootTheOwl::OnPendingDestroy()
{
}
