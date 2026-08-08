//cpp
#include "Actor.h"
/* Actor::FarthestPlayer()
 * Recomputes the closest player (which, as a side effect, also stores the
 * farthest player into a file-scope global), then returns that cached
 * farthest-player pointer.
 *
 * Callee (symbols/verified.tsv):
 *   0x02010ad8 = Actor::ClosestPlayer() -> _ZN5Actor13ClosestPlayerEv
 * Global:
 *   0x0209b450 = farthest-player pointer, populated by ClosestPlayer()
 */

struct Actor;

extern struct Actor *_ZN5Actor13ClosestPlayerEv(void);
extern "C" struct Actor *data_0209b450; /* @ 0x0209b450 */

Player *Actor::FarthestPlayer()
{
    ClosestPlayer();
    return (Player *)data_0209b450;
}
