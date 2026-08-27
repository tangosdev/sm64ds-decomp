//cpp
// @symbol _ZN15RecRoomCupboardD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body.
 *
 * Nobody writes the vptr store, the five-element array destroy at +0xd4 or the
 * dActor_c base step: declaring `dCcAcPos_c mColliders[5]` and deriving from
 * dActor_c is enough. mwcc emits D2, D0 and D1 together (plus this class's
 * vtable and typeinfo, which the module's gap object also supplies from ROM)
 * and objisolate keeps the one this file is bound to.
 */
#include "RecRoomCupboard.h"

RecRoomCupboard::~RecRoomCupboard()
{
}
