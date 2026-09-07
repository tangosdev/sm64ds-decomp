//cpp
// @symbol _ZN9JetStream16OnPendingDestroyEv
/* recovered: shared header, real C++ method
 *
 * Empty -- the ROM body is a single `bx lr`. The override exists to suppress
 * whatever the base does on pending destroy, not to do anything itself.
 */
#include "JetStream.h"

void JetStream::OnPendingDestroy()
{
}
