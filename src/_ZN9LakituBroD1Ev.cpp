//cpp
// @symbol _ZN9LakituBroD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * One vtable store and 6 destructor calls, every one a consequence of
 * `struct LakituBro : Enemy` and the members that declaration now types:
 * its own vptr, then ShadowModel (0x218), ShadowModel (0x1f0), TextureSequence (0x1d8), 
 * ModelAnim (0x174), 
 * ModelAnim (0x110)
 * in reverse declaration order, then Enemy::~Enemy.
 *
 * This body is the evidence for the header. It was the hand-written C that
 * named those offsets in the first place, and `LakituBro_Spawn` constructs the
 * same types at the same offsets.
 */
#include "LakituBro.h"

LakituBro::~LakituBro()
{
}
