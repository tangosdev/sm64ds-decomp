//cpp
// @symbol _ZN12dBgCh_SphCrrC1Ev
/* recovered: real C++ constructor, MI bases (notes/ctor-migration.md item 2)
 *
 * ROM body, in order:
 *   bl _ZN5dBgChC2Ev        the dBgCh    base sub-object at +0x00
 *   bl _ZN5dBgPiC2Ev        the dBgPi    base sub-object at +0x10
 *   bl _ZN8dM3dGSphC2Ev     the dM3dGSph base sub-object at +0x38
 *   three vptr stores       0x02099338 (+0), 0x02099348 (+0x10),
 *                           0x02099358 (+0x38) -- the derived class's primary
 *                           block and its two secondary thunk blocks, written
 *                           over whatever the base ctors left behind
 *   bl _ZN5dBgPiC1Ev x3     mClsnResult1/2/3 at +0x74/+0x9c/+0xc4 -- complete-
 *                           object MEMBERS, hence C1 where the bases got C2
 *   unk_0ec = 0             the only state this constructor itself supplies
 *
 * Everything except that last store is synthesized: declaring the three bases
 * and the three members in include/dBgCh_SphCrr.h is what makes mwcc emit the
 * base calls, the vptr rebinding, and the member calls, each with the variant
 * the ABI picks by context. Complete-object context for every caller, hence
 * C1; the C2 sibling objisolate strips.
 */
#include "dBgCh_SphCrr.h"

dBgCh_SphCrr::dBgCh_SphCrr() : unk_0ec(0) {}
