//cpp
// @symbol _ZN15RollingIronBallD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * One vtable store and 5 destructor calls, every one a consequence of
 * `struct RollingIronBall : dEnemyBase_c` and the members that declaration now types:
 * its own vptr, then dCcAc_c (0x374), ShadowModel (0x31c), Model (0x2cc), 
 * dBgCh_Actr (0x110)
 * in reverse declaration order, then dEnemyBase_c::~dEnemyBase_c.
 *
 * This body is the evidence for the header. It was the hand-written C that
 * named those offsets in the first place, and `RollingIronBall_Spawn` constructs the
 * same types at the same offsets.
 */
#include "RollingIronBall.h"

RollingIronBall::~RollingIronBall()
{
}
