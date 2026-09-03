//cpp
// @symbol _ZN14daObjC0Water_cD1Ev
/* The destructor is INLINE in include/daObjC0Water_c.h, so mwccarm emits the
 * D1/D0 pair itself, in the ROM's own order (D1 0x02111450, D0 0x0211149c) and
 * with no homeless D2. This file therefore does not DEFINE the destructor --
 * it only forces the compiler to emit it into this object, so the existing
 * per-function delink entry still has its symbol until the TU promotion
 * deletes this file.
 */
#include "daObjC0Water_c.h"

void daObjC0Water_c_EmitDestructor(daObjC0Water_c *water) { water->~daObjC0Water_c(); }
