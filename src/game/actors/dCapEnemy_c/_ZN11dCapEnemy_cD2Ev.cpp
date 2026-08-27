//cpp
// @symbol _ZN11dCapEnemy_cD2Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * The base-object variant, and the one every derived cap enemy chains to.
 * mwcc emits D1, D2 and D0 from the single `dCapEnemy_c::~dCapEnemy_c() {}`
 * below; this file is bound to D2 and `src/_ZN11dCapEnemy_cD1Ev.cpp` to D1.
 * They are byte-identical -- dCapEnemy_c has no virtual bases -- which is why
 * this address spent so long carrying the placeholder name func_ov002_020aedbc
 * while arm9 0x0200651c wore the D2 name that belongs here.
 *
 * Which is which is settled by how the ROM reaches them, not by their bytes:
 * slot 16 of _ZTV11dCapEnemy_c (0x021082c4) holds 0x0200651c, so that one is
 * D1; nothing in the image calls it directly. This one has no vtable slot and
 * is reached only by `bl` from daTrs_c's and daKrb_c's destructors tearing
 * down their base sub-object, which is exactly what D2 is for.
 */
#include "dCapEnemy_c.h"

dCapEnemy_c::~dCapEnemy_c()
{
}
