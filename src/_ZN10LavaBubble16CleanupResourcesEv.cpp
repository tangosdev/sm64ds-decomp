//cpp
// @symbol _ZN10LavaBubble16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * `return 1` with no release calls, which is the finding rather than a stub:
 * every other class in this overlay releases at least one SharedFilePtr here
 * (Clam 3, TreasureChest 4, BowserPuzzlePiece 2, WaterRing 1). LavaBubble
 * holds none, so it has nothing to give back.
 */
#include "LavaBubble.h"

int LavaBubble::CleanupResources()
{
    return 1;
}
