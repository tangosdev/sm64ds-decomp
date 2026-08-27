//cpp
// @symbol _ZN5dBgChD0Ev
/* recovered: real C++ deleting destructor
 *
 * D0 is the DELETING destructor: run the destructor body, then return the
 * object to its heap. Nobody writes that; the same empty `~dBgCh()` the D1
 * file defines emits D2, D0 and D1 together, and objisolate keeps the one
 * this file is bound to. The ROM carries this one at 0x020354e0 (currently
 * `func_020354e0`). The heap hand-off is the family's inline operator delete,
 * which the header declares.
 */
#include "dBgCh.h"

dBgCh::~dBgCh()
{
}
