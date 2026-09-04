//cpp
// @symbol _ZN5WhompD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * One vtable store and 6 destructor calls, every one a consequence of
 * `struct Whomp : dEnemyBase_c` and the members that declaration now types:
 * its own vptr, then dBgW_KcMbg (0x418), ShadowModel (0x344), 
 * TextureSequence (0x330), ModelAnim (0x2cc), 
 * dBgCh_Actr (0x110)
 * in reverse declaration order, then dEnemyBase_c::~dEnemyBase_c.
 *
 * This body is the evidence for the header. It was the hand-written C that
 * named those offsets in the first place, and `daBtn_c_classInit_BATAN` constructs the
 * same types at the same offsets.
 */
#include "Whomp.h"

Whomp::~Whomp()
{
}
