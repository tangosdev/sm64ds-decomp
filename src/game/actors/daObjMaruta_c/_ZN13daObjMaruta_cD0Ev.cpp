//cpp
// @symbol _ZN13daObjMaruta_cD0Ev
/* recovered: real C++ deleting destructor -- forced out of line via the key function
 *
 * daObjMaruta_c.h defines `~daObjMaruta_c() {}` in the class body on purpose,
 * so no TU that merely includes the header emits _ZN13daObjMaruta_cD0Ev --
 * but the ROM carries one at ov080 0x02126fbc. The vtable is what needs the
 * deleting destructor as a symbol, and the vtable is emitted by the TU that
 * defines the key function (OnHitByMegaChar, the first non-inline virtual --
 * include/daObjMaruta_c.h records the rule). Defining it here makes mwcc emit
 * _ZTV13daObjMaruta_c and the destructor variants alongside it, and
 * objisolate keeps only the D0 .text this file is bound to, discarding the
 * duplicate OnHitByMegaChar body and the .data
 * (notes/dtor-migration.md section 3).
 */
#include "daObjMaruta_c.h"

void daObjMaruta_c::OnHitByMegaChar(Player &player)
{
}
