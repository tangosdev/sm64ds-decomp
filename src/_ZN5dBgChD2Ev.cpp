//cpp
// @symbol _ZN5dBgChD2Ev
/* recovered: real C++ base-object destructor
 *
 * The hand-written version spelled out what a base-object destructor does
 * anyway: store this class's vtable over the one a derived constructor left.
 * That is all `dBgCh::~dBgCh()` with an empty body means, and the ROM carries
 * it at 0x020354d0 (currently `func_020354d0`). The vtable is emitted here
 * because this is the key function: the destructor is the first non-inline
 * virtual.
 */
#include "dBgCh.h"

dBgCh::~dBgCh()
{
}
