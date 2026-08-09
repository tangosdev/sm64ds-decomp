//cpp
// @symbol _ZN10KoopaShell16OnPendingDestroyEv
/* recovered: shared header, real C++ method
 *
 * Empty, and that is the finding rather than a stub: the ROM body is a single
 * `bx lr`. KoopaShell overrides the slot to do NOTHING, which suppresses
 * whatever the base does on pending destroy.
 */
#include "KoopaShell.h"

void KoopaShell::OnPendingDestroy()
{
}
