//cpp
/* Which Wiggler (MG_HANACHAN): the scene factory and the constructor for
 * the five 8-byte entries inside each of its fifteen 0x98-byte elements.
 * The scene's own methods are in src/minigames/d_s_mg_hanachan_methods.cpp.
 *
 * The factory is `new dScMgHanachan_c`: the inlined implicit constructor
 * runs dScMgBase_c's C2, stores the vptr and builds mWiggler through
 * __cxa_vec_ctor with Wiggler's structors. Wiggler's empty inline
 * constructor (dScMgHanachan_c.h) is emitted after it, at 0x020ede80; its
 * only work is building the five Vector2 segment points at +0x18.
 */

#include "dScMgHanachan_c.h"

void *dScMgHanachan_c_classInit(void);

void *dScMgHanachan_c_classInit(void) {
    return new dScMgHanachan_c;
}

/* Wiggler's empty inline constructor (dScMgHanachan_c.h), emitted here. */
