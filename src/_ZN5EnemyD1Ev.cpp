//cpp
// @symbol _ZN5EnemyD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * Store this class's vtable over the one Actor's constructor left, then run
 * the Actor subobject destructor. Both follow from `struct Enemy : Actor` and
 * `virtual ~Enemy()`; Enemy owns nothing with a destructor of its own.
 *
 * This needed the vtable to have a NAME. A real destructor stores _ZTV5Enemy,
 * and the tree only knew 0x021081e4 as data_ov002_021081e4, so the link failed
 * on an undefined symbol while the bytes matched perfectly. The alias is now in
 * ov002's symbols.txt -- see the note there.
 *
 * The destructor stays OUT-OF-LINE, unlike Platform's: the 51 classes derived
 * from Enemy call _ZN5EnemyD2Ev rather than inlining its vptr store, so a
 * visible body would make each of them emit code the ROM does not have.
 */
#include "Enemy.h"

Enemy::~Enemy()
{
}
