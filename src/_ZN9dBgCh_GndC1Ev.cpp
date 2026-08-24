//cpp
// @symbol _ZN9dBgCh_GndC1Ev
/* recovered: real C++ constructor, MI bases (notes/ctor-migration.md item 9)
 *
 * ROM body, in order:
 *   bl _ZN5dBgChC2Ev        the dBgCh base sub-object at +0x00
 *   bl _ZN5dBgPiC2Ev        the dBgPi base sub-object at +0x10 -- the hit
 *                           record every DetectClsn writes into
 *   two vptr stores         0x02099264 (+0) and 0x02099274 (+0x10) -- the
 *                           derived class's primary block and its one
 *                           secondary thunk block, written over whatever the
 *                           base ctors left behind
 *   unk_04c = 0x1f4000      the only state this constructor itself supplies:
 *                           the default probe height, a Fix12i of 496.0
 *
 * Everything except that last store is synthesized: declaring the two bases
 * in include/dBgCh_Gnd.h is what makes mwcc emit the base calls and the vptr
 * rebinding, each with the variant the ABI picks by context. Complete-object
 * context for every caller, hence C1. The ROM kept no C2 sibling -- nothing
 * derives from dBgCh_Gnd -- so objisolate strips the sibling this TU emits.
 */
#include "dBgCh_Gnd.h"

dBgCh_Gnd::dBgCh_Gnd() : unk_04c(0x1f4000) {}
