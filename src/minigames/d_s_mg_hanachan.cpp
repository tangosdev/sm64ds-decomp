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

extern "C" void *dScMgHanachan_c_classInit(void);

// @symbol dScMgHanachan_c_classInit
extern "C" void *dScMgHanachan_c_classInit(void) {
    return new dScMgHanachan_c;
}

// @symbol _ZN15dScMgHanachan_c7WigglerC1Ev
/* Wiggler's empty inline constructor (dScMgHanachan_c.h), emitted here. */
