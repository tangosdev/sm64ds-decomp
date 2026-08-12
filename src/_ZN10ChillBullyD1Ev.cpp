//cpp
// @symbol _ZN10ChillBullyD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * Vtable slot 16, and previously func_ov027_021115c4. Two vtable stores and five
 * destructor calls, none of them written here: its own vptr, then daOts_c's --
 * inlined, because that class's destructor is defined in its class body -- then the
 * base's four members in reverse declaration order, then Enemy. ChillBully adds no
 * member with a destructor of its own, only two bytes.
 *
 * Byte-for-byte the same shape as Bully's and BigBully's, which is what three
 * siblings sharing a base look like.
 */
#include "ChillBully.h"

ChillBully::~ChillBully()
{
}
