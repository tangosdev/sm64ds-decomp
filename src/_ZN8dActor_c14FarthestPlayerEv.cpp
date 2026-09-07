//cpp
#include "dActor_c.h"
/* dActor_c::FarthestPlayer()
 * Recomputes the closest player (which, as a side effect, also stores the
 * farthest player into a file-scope global), then returns that cached
 * farthest-player pointer.
 *
 * Callee (symbols/verified.tsv):
 *   0x02010ad8 = dActor_c::ClosestPlayer() -> _ZN8dActor_c13ClosestPlayerEv
 * Global:
 *   0x0209b450 = farthest-player pointer, populated by ClosestPlayer()
 */

struct dActor_c;

extern struct dActor_c *_ZN8dActor_c13ClosestPlayerEv(void);
extern "C" struct dActor_c *data_0209b450; /* @ 0x0209b450 */

Player *dActor_c::FarthestPlayer()
{
    ClosestPlayer();
    return (Player *)data_0209b450;
}
