//cpp
/* d_s_boot.cpp -- dScBoot_c's registration unit: the destructor pair and
 * the dScBoot_c_classInit factory, .text 0x02023598..0x02023688. The class's
 * scene methods live in the separate TU src/actors/dScBoot_c.cpp.
 *
 * THE DESTRUCTOR. The cartridge places D1 (0x02023598) BELOW D0
 * (0x020235d4). Under deferred codegen an out-of-line destructor emits D2,
 * D0, D1 -- the wrong order -- which is why the pair used to stay in its own
 * files. With `#pragma defer_codegen off` and the file in ascending order,
 * the same out-of-line definition emits D1, D0, D2: the cartridge's order,
 * both byte-identical, written first so they land first. The extra D2 is
 * unreferenced and is dropped at link (the cartridge has no D2 here). Because the
 * destructor is the first declared non-inline virtual in dScBoot_c.h, it is
 * the key function, so this file also emits the vtable and the typeinfo
 * chain; the cartridge's copies of those are outside this range and stay
 * canonical.
 *
 * Source order IS the ROM's: `#pragma defer_codegen off` below makes
 * mwccarm emit .text in source order. Do not reorder.
 */
#include "dScBoot_c.h"

#pragma defer_codegen off

// @symbol _ZN9dScBoot_cD1Ev
// @symbol _ZN9dScBoot_cD0Ev
dScBoot_c::~dScBoot_c()
{
}

/* The BOOT registry factory. Every instruction the cartridge has here falls
 * out of the one `new`: 0x58 -- the class's own size -- into
 * fBase_c::operator new; the inlined ctor runs fBase_c's C2, stores dBase_c
 * then dScene_c's vptrs, ORs pauseFlags 1 and 4 (dScene_c::dScene_c), then
 * stores dScBoot_c's vptr. The null check is the one `new` itself emits. */
// @symbol dScBoot_c_classInit
extern "C" dScBoot_c *dScBoot_c_classInit(void)
{
    return new dScBoot_c();
}
