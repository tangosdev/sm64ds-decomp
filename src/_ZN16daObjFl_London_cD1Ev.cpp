//cpp
// @symbol _ZN16daObjFl_London_cD1Ev
/* The destructor is INLINE in include/daObjFl_London_c.h, so mwccarm emits the
 * D1/D0 pair itself, in the ROM's own order (D1 0x02111980, D0 0x021119c4) and
 * with no homeless D2. This file therefore does not DEFINE the destructor --
 * it only forces the compiler to emit it into this object, so the existing
 * per-function delink entry still has its symbol until the TU promotion
 * deletes this file.
 */
#include "daObjFl_London_c.h"

void daObjFl_London_c_EmitDestructor(daObjFl_London_c *london) { london->~daObjFl_London_c(); }
