//cpp
// @symbol _ZN4ToadD1Ev
/* Force mwccarm to materialize Toad's inline complete destructor. */
#include "Toad.h"

void Toad_EmitDestructor(Toad *toad)
{
    toad->~Toad();
}
