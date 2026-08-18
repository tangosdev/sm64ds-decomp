//cpp
#include "dActor_c.h"
/* dActor_c::DistToCPlayer()
 * Recomputes the closest player (which, as a side effect, stores the
 * closest distance into a file-scope global), then returns that distance.
 *
 * Callee (symbols/verified.tsv):
 *   0x02010ad8 = dActor_c::ClosestPlayer() -> _ZN8dActor_c13ClosestPlayerEv
 * Global:
 *   0x0208e380 = closest-player distance (Fix12i), populated by ClosestPlayer()
 */

typedef int Fix12i; /* 20.12 fixed-point */

struct dActor_c;

extern struct dActor_c *_ZN8dActor_c13ClosestPlayerEv(void);
extern "C" Fix12i data_0208e380; /* @ 0x0208e380 */

s32 dActor_c::DistToCPlayer()
{
    ClosestPlayer();
    return data_0208e380;
}
