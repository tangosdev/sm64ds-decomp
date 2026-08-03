//cpp
// @symbol _ZN5Actor24KillAndTrackInDeathTableEv
/* Actor::KillAndTrackInDeathTable() at 0x0200f9b8.
 *
 * Records the actor in the death table, then marks it for destruction through
 * the ActorBase helper it inherits.
 */
#include "Actor.h"

void Actor::KillAndTrackInDeathTable()
{
    TrackInDeathTable();
    MarkForDestruction();
}
