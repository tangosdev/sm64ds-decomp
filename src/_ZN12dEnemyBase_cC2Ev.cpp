//cpp
// @symbol _ZN12dEnemyBase_cC2Ev
/* recovered: real C++ constructor -- the compiler emits the whole body
 *
 * Two steps, and neither is written below: the dActor_c base-subobject call, then
 * this class's own vptr store. `struct dEnemyBase_c : dActor_c` supplies the first
 * and `virtual ~dEnemyBase_c()` the second, in the order mwcc always uses
 * (notes/ctor-migration.md §6: base calls -> derived vptr store -> members -> body).
 *
 * The base call is a real `bl` only because include/dActor_c.h DECLARES dActor_c()
 * and never defines it; left undeclared the compiler would synthesise and inline
 * the base construction here and the candidate would land short. That is
 * notes/ctor-migration.md §2, and it is why dEnemyBase_c.h declares dEnemyBase_c()
 * the same way for the 51 classes below it.
 *
 * One definition emits C1 AND C2 (§1); objisolate keeps the C2 this file is bound
 * to. There is no C1 file and no C1 in the ROM -- nothing constructs a standalone
 * dEnemyBase_c, so the linker never kept one.
 */
#include "dEnemyBase_c.h"

dEnemyBase_c::dEnemyBase_c()
{
}
