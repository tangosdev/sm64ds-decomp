//cpp
// @symbol _ZN8dActor_c24KillAndTrackInDeathTableEv
/* dActor_c::KillAndTrackInDeathTable() at 0x0200f9b8.
 *
 * Records the actor in the death table, then marks it for destruction through
 * the fBase_c helper it inherits.
 */
#include "dActor_c.h"

void dActor_c::KillAndTrackInDeathTable()
{
    TrackInDeathTable();
    MarkForDestruction();
}
