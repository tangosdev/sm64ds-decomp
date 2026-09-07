//cpp
// @symbol _ZN5dBgPiC2Ev
/* recovered: real C++ base-object constructor
 *
 * Renamed from func_0203819c (2026-08-23). RTTI on the vtable it stores
 * (_ZTV5dBgPi -> typeinfo "5dBgPi") names the class dBgPi, and all three of
 * its ROM callers are derived-constructor base steps (dBgCh_Gnd, dBgCh_Lin
 * and dBgCh_SphCrr, each constructing their dBgPi base sub-object at +0x10)
 * -- so the variant label is C2. See notes/ctor-migration.md section 4/4b.
 *
 * The dBgPc base construction and the vptr store are the compiler's; the one
 * statement below is the constructor body. This is the same definition the
 * C1 file carries, because C1 and C2 are two of the functions mwcc emits from
 * one constructor and each file is bound to one of them by
 * config/arm9/delinks.txt.
 */
#include "dBgPi.h"

extern "C" void func_020380c0(dBgPi *result);

dBgPi::dBgPi()
{
    func_020380c0(this);
}
