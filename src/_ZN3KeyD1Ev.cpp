//cpp
// @symbol _ZN3KeyD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * One vtable store and 6 destructor calls, every one a consequence of
 * `struct Key : dEnemyBase_c` and the members that declaration now types:
 * its own vptr, then dBgCh_Actr (0x260), dCcAcPos_c (0x220), 
 * ShadowModel (0x1c8), Model (0x178), 
 * ModelAnim (0x114)
 * in reverse declaration order, then dEnemyBase_c::~dEnemyBase_c.
 *
 * This body is the evidence for the header. It was the hand-written C that
 * named those offsets in the first place, and `Key_Spawn` constructs the
 * same types at the same offsets.
 */
#include "Key.h"

Key::~Key()
{
}
