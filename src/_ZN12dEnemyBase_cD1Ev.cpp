//cpp
// @symbol _ZN12dEnemyBase_cD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * Store this class's vtable over the one dActor_c's constructor left, then run
 * the dActor_c subobject destructor. Both follow from `struct dEnemyBase_c : dActor_c` and
 * `virtual ~dEnemyBase_c()`; dEnemyBase_c owns nothing with a destructor of its own.
 *
 * This needed the vtable to have a NAME. A real destructor stores _ZTV12dEnemyBase_c,
 * and the tree only knew 0x021081e4 as data_ov002_021081e4, so the link failed
 * on an undefined symbol while the bytes matched perfectly. The alias is now in
 * ov002's symbols.txt -- see the note there.
 *
 * The destructor stays OUT-OF-LINE, unlike dBgActor_c's: the 51 classes derived
 * from dEnemyBase_c call _ZN12dEnemyBase_cD2Ev rather than inlining its vptr store, so a
 * visible body would make each of them emit code the ROM does not have.
 */
#include "dEnemyBase_c.h"

dEnemyBase_c::~dEnemyBase_c()
{
}
