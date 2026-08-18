//cpp
#include "Actor.h"
/* Actor::DistToCPlayer()
 * Recomputes the closest player (which, as a side effect, stores the
 * closest distance into a file-scope global), then returns that distance.
 *
 * Callee (symbols/verified.tsv):
 *   0x02010ad8 = Actor::ClosestPlayer() -> _ZN5Actor13ClosestPlayerEv
 * Global:
 *   0x0208e380 = closest-player distance (Fix12i), populated by ClosestPlayer()
 */

typedef int Fix12i; /* 20.12 fixed-point */

struct Actor;

extern struct Actor *_ZN5Actor13ClosestPlayerEv(void);
extern "C" Fix12i data_0208e380; /* @ 0x0208e380 */

s32 Actor::DistToCPlayer()
{
    ClosestPlayer();
    return data_0208e380;
}
