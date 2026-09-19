//cpp
// @symbol _ZN11daKinopio_cD1Ev
/* Force mwccarm to materialize daKinopio_c's inline complete destructor. */
#include "daKinopio_c.h"

void daKinopio_c_EmitDestructor(daKinopio_c *toad)
{
    toad->~daKinopio_c();
}
